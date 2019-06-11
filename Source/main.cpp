#define _USE_MATH_DEFINES

#include <cmath>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <glad/glad.h>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <string>
#include <irrKlang.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "GLSL.h"
#include "GLTextureWriter.h"
#include "MatrixStack.h"
#include "GLTextureWriter.h"
#include "Object3D.h"
#include "Program.h"
#include "effects/Sound.h"
#include "Shape.h"
#include "Skybox.h"
#include "WindowManager.h"
#include "engine/Time.h"
#include "effects/ParticleSpark.h"
#include "gameobjects/Ball.h"
#include "gameobjects/Goal.h"
#include "gameobjects/Enemy.h"
#include "gameobjects/Blower.h"
#include "gameobjects/Beam.h"
#include "engine/Collider.h"
#include "engine/ColliderSphere.h"
#include "engine/GameObject.h"
#include "engine/ColliderSphere.h"
#include "engine/Collider.h"
#include "engine/Octree.h"
#include "engine/Frustum.h"
#include "engine/ParticleEmitter.h"
#include "engine/UIObject.h"
#include "engine/Prefab.h"
#include "engine/SceneManager.h"
#include "engine/ModelManager.h"
#include "engine/TextureManager.h"
#include "engine/SkyboxManager.h"
#include "engine/ShaderManager.h"
#include "engine/MaterialManager.h"
#include "engine/PrefabManager.h"
#include "engine/EmitterManager.h"
#include "engine/Preferences.h"

#define RESOURCE_DIRECTORY string("../Resources")

using namespace std;
using namespace glm;

TimeData Time;
shared_ptr<Sound> soundEngine;

class Application : public EventCallbacks
{
public:
    Preferences preferences = Preferences(RESOURCE_DIRECTORY + "/preferences.yaml");
    WindowManager *windowManager = new WindowManager();
    ModelManager modelManager = ModelManager(RESOURCE_DIRECTORY + "/models/");
    TextureManager textureManager = TextureManager(RESOURCE_DIRECTORY + "/textures/");
    SkyboxManager skyboxManager = SkyboxManager(RESOURCE_DIRECTORY + "/skyboxes/");
    ShaderManager shaderManager = ShaderManager(RESOURCE_DIRECTORY + "/shaders/");
    MaterialManager materialManager = MaterialManager();
    EmitterManager emitterManager = EmitterManager();
    PrefabManager prefabManager = PrefabManager(RESOURCE_DIRECTORY + "/prefabs/", shared_ptr<ModelManager>(&modelManager), shared_ptr<MaterialManager>(&materialManager));
    SceneManager sceneManager = SceneManager(shared_ptr<PrefabManager>(&prefabManager));

    // Game Info Globals
    bool editMode = false;
    float startTime = 0.0f;

    bool debugLight = 0;
    bool debugGeometry = 1;
    GLuint depthMapFBO = 0;
    GLuint depthMap = 0;
    GLuint objectMapFBO;
    GLuint objectMapTex;
    GLuint objectMapDepthBuf;
    GLuint beamFBO;
    GLuint beamDepthBuf;
    GLuint beamWorldDepthBuf;

    // Camera
    shared_ptr<Camera> camera;
    Frustum viewFrustum;

    // Marble
    shared_ptr<Ball> marble;

    struct
    {
        shared_ptr<Enemy> enemy1;
        shared_ptr<Enemy> enemy2;
        shared_ptr<Enemy> sentry1;
        shared_ptr<Enemy> sentry2;
        shared_ptr<Goal> goal;
        shared_ptr<PowerUp> powerUp1;
        shared_ptr<PowerUp> powerUp2;
        shared_ptr<Blower> blower;
        shared_ptr<PhysicsObject> beam;
        vector<shared_ptr<PhysicsObject>> general;
    } gameObjects;

	struct
	{
		shared_ptr<UIObject> logo;
		shared_ptr<UIObject> winMessage;
		shared_ptr<UIObject> dummy;
	} uiObjects;

    // Billboard for rendering a texture to screen (like the shadow map)
    GLuint fboQuadVertexArrayID;
    GLuint fboQuadVertexBuffer;

    /*
     * Loading
    */
    void loadWindow()
    {
        windowManager->init(preferences.window.resolutionX, preferences.window.resolutionY, preferences.window.maximized, preferences.window.fullscreen);
        windowManager->setEventCallbacks(this);
    }

    void loadCanvas()
    {
        int width, height;
        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
        GLSL::checkVersion();

        // Enable z-buffer test.
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        // Initialize camera
        camera = make_shared<Camera>(windowManager, vec3(0, 0, 0));
        camera->init();
    }

    void loadSounds()
    {
        soundEngine = make_shared<Sound>();

        if (preferences.sound.music) soundEngine->playPauseMusic();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void loadShaders()
    {
        vector<string> pbrUniforms = {"P", "V", "M", "shadowResolution", "shadowAA", "shadowDepth", "LS", "albedoMap", "roughnessMap", "metallicMap", "aoMap", "lightPosition", "lightColor", "viewPos"};
        shared_ptr<Program> pbr = shaderManager.get("pbr", {"vertPos", "vertNor", "vertTex"}, pbrUniforms);
        materialManager.init(pbr, shared_ptr<TextureManager>(&textureManager));

        shaderManager.get("sky", {"vertPos"}, {"P", "V", "Texture0"});
        shaderManager.get("circle", {"vertPos"}, {"P", "V", "M", "radius"});
        shaderManager.get("cube_outline", {"vertPos"}, {"P", "V", "M", "edge"});
        shaderManager.get("depth", {"vertPos"}, {"LP", "LV", "M"});
        shaderManager.get("pass", {"vertPos"}, {"texBuf"});
        shaderManager.get("depth_debug", {"vertPos"}, {"LP", "LV", "M"});
        shaderManager.get("particle", {"vertPos", "vertTex"}, {"P", "V", "M", "pColor", "alphaTexture"});
        shaderManager.get("object_map", {"vertPos"}, {"P", "V", "M", "objectIndex"});
		shaderManager.get("ui", { "vertPos", "vertTex" }, {"M", "Texture"});
		shaderManager.get("beam", { "vertPos" }, {"P", "V", "M", "depthBuf", "viewport", "density"});
		shaderManager.get("world_depth", { "vertPos" }, {"P", "V", "M"});
    }

    void loadSkybox()
    {
        skyboxManager.get("desert_hill", 1);
    }

    void loadShadows()
    {
        // Generate the FBO for the shadow depth
        glGenFramebuffers(1, &depthMapFBO);

        // Generate the texture
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, preferences.shadows.resolution, preferences.shadows.resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Bind with framebuffer's depth buffer
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void loadObjectMap()
    {
        int width, height;
        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);

        // Generate FBO for object map
        glGenFramebuffers(1, &objectMapFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, objectMapFBO);

        // Generate texture where IDs of objects are stored
        glGenTextures(1, &objectMapTex);
        glBindTexture(GL_TEXTURE_2D, objectMapTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, width, height, 0, GL_RED_INTEGER, GL_INT, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, objectMapTex, 0);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Generate depth buffer
        glGenTextures(1, &objectMapDepthBuf);
        glBindTexture(GL_TEXTURE_2D, objectMapDepthBuf);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, objectMapDepthBuf, 0);

        // Bind with framebuffer's depth buffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void loadBeam()
    {
        int width, height;
        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);

        // Generate FBO for beam
        glGenFramebuffers(1, &beamFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, beamFBO);

        // Generate texture where depth is stored in world units
        glGenTextures(1, &beamWorldDepthBuf);
        glBindTexture(GL_TEXTURE_2D, beamWorldDepthBuf);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, beamWorldDepthBuf, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Generate depth buffer
        glGenTextures(1, &beamDepthBuf);
        glBindTexture(GL_TEXTURE_2D, beamDepthBuf);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, beamDepthBuf, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Bind with framebuffer's depth buffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void loadEffects()
    {
        emitterManager.get("fireworks", modelManager.get("billboard.obj"), textureManager.get("particles/scorch_02.png"), 100);
        emitterManager.get("sparks", modelManager.get("billboard.obj"), textureManager.get("particles/star_07.png"), 100);
        emitterManager.get("wind", modelManager.get("billboard.obj"), textureManager.get("particles/smoke_04.png"), 1000);
    }

    void loadFBOQuad()
    {
        glGenVertexArrays(1, &fboQuadVertexArrayID);
        glBindVertexArray(fboQuadVertexArrayID);

        static const GLfloat g_quad_vertex_buffer_data[] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,

            -1.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
        };

        glGenBuffers(1, &fboQuadVertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, fboQuadVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
    }

    void loadModels()
    {
        modelManager.get("cube.obj", false, true);
        modelManager.get("Robot/RobotHead.obj", true);
        modelManager.get("Robot/RobotLeg.obj", true);
        modelManager.get("Robot/RobotFoot.obj", true);
        modelManager.get("billboard.obj", true);
        modelManager.get("goal.obj", true);
        modelManager.get("quadSphere.obj", true);
        modelManager.get("bunny.obj", true);
        modelManager.get("cone.obj", true);
    }

    void loadLevel()
    {
        sceneManager.load(RESOURCE_DIRECTORY + "/levels/" + preferences.scenes.list[preferences.scenes.startup] + ".yaml");
    }

    void loadGameObjects()
    {
        // Marble
        marble = dynamic_pointer_cast<Ball>(sceneManager.findInstance("Marble")->physicsObject);
        marble->init(sceneManager.marbleStart, windowManager, emitterManager.get("sparks"), camera);
        marble->addSkin(materialManager.get("brown_rock", "jpg"));
        marble->addSkin(materialManager.get("seaside_rocks", "jpg"));
        marble->addSkin(materialManager.get("coal_matte_tiles", "jpg"));
        marble->addSkin(materialManager.get("marble_tiles", "jpg"));

        if (preferences.scenes.startup == 0)
        {
            // Blower
            gameObjects.blower = make_shared<Blower>(vec3(112, -3, 21), rotate(quat(1, 0, 0, 0), (float)M_PI_4, vec3(0, 0, 1)), 3.0f, 10.0f);
            gameObjects.blower->init(emitterManager.get("wind"));
            sceneManager.octree.insert(gameObjects.blower);

            // Enemy 1
            vector<vec3> enemyPath = {
                vec3{95.0, 2.0, 7.0},
                vec3{100.0, 2.0, 15.0},
                vec3{110.0, 2.0, -1.0},
                vec3{115.0, 2.0, 7.0}};
            gameObjects.enemy1 = make_shared<Enemy>(enemyPath, quat(1, 0, 0, 0), modelManager.get("Robot/RobotHead.obj"), modelManager.get("Robot/RobotLeg.obj"), modelManager.get("Robot/RobotFoot.obj"), 1.75f);
            gameObjects.enemy1->init(windowManager);
            sceneManager.octree.insert(gameObjects.enemy1);

            // Enemy 2
            enemyPath = {
                vec3{125.0, 8.0, 55.0},
                vec3{115.0, 20.0, 55.0},
                vec3{105.0, 5.0, 55.0},
                vec3{95.0, 8.0, 55.0}};
            gameObjects.enemy2 = make_shared<Enemy>(enemyPath, quat(1, 0, 0, 0), modelManager.get("Robot/RobotHead.obj"), modelManager.get("Robot/RobotLeg.obj"), modelManager.get("Robot/RobotFoot.obj"), 1.75f);
            gameObjects.enemy2->init(windowManager);
            sceneManager.octree.insert(gameObjects.enemy2);

            // Sentry 1
            enemyPath = { vec3{65.0, 7.0, 32.0} };
            gameObjects.sentry1 = make_shared<Enemy>(enemyPath, quat(1, 0, 0, 0), modelManager.get("Robot/RobotHead.obj"), modelManager.get("Robot/RobotLeg.obj"), modelManager.get("Robot/RobotFoot.obj"), 1.75f);
            gameObjects.sentry1->init(windowManager);
            sceneManager.octree.insert(gameObjects.sentry1);

            // Sentry 2
            enemyPath = { vec3{90.0, 2.0, 32.0} };
            gameObjects.sentry2 = make_shared<Enemy>(enemyPath, quat(1, 0, 0, 0), modelManager.get("Robot/RobotHead.obj"), modelManager.get("Robot/RobotLeg.obj"), modelManager.get("Robot/RobotFoot.obj"), 1.75f);
            gameObjects.sentry2->init(windowManager);
            sceneManager.octree.insert(gameObjects.sentry2);

            //Power Up 1
            gameObjects.powerUp1 = make_shared<PowerUp>(vec3(120, 2, 30), 0, quat(1, 0, 0, 0), modelManager.get("bunny.obj", true), 1, 1);
            gameObjects.powerUp1->init();
            sceneManager.octree.insert(gameObjects.powerUp1);


            // Power Up 2
            gameObjects.powerUp2 = make_shared<PowerUp>(vec3(80, 10, 55.0), 1, quat(1, 0, 0, 0), modelManager.get("bunny.obj", true), 1, 1);
            gameObjects.powerUp2->init();
            sceneManager.octree.insert(gameObjects.powerUp2);
            gameObjects.goal = make_shared<Goal>(vec3(0, 11.5, 0), quat(1, 0, 0, 0), nullptr, 1.50f);
        }
        else if (preferences.scenes.startup == 1)
        {
            gameObjects.goal = make_shared<Goal>(vec3(-4 * 8, 3 * 8, 5.4 * 8) + vec3(0, 1, 0), quat(1, 0, 0, 0), nullptr, 1.50f);
        }
        else if (preferences.scenes.startup == 2)
        {
            gameObjects.goal = make_shared<Goal>(vec3(0, 0, 60), quat(1, 0, 0, 0), nullptr, 1.50f);     

            // Enemy 1
            vector<vec3> enemyPath = {
                vec3(16.0, -37, -76.0),
                vec3(6.0, -37, -100.0),
                vec3(-6.0, -37, -100.0),
                vec3(-16.0, -37, -76.0),
            };
            auto enemy = make_shared<Enemy>(enemyPath, quat(1, 0, 0, 0), modelManager.get("Robot/RobotHead.obj"), modelManager.get("Robot/RobotLeg.obj"), modelManager.get("Robot/RobotFoot.obj"), 1.75f);
            sceneManager.octree.insert(enemy);
            gameObjects.general.push_back(enemy);

            // Enemy 2
            enemyPath = {
                vec3(-16.0, -37, -76.0),
                vec3(-6.0, -37, -52.0),
                vec3(6.0, -37, -52.0),
                vec3(16.0, -37, -76.0),
            };
            enemy = make_shared<Enemy>(enemyPath, quat(1, 0, 0, 0), modelManager.get("Robot/RobotHead.obj"), modelManager.get("Robot/RobotLeg.obj"), modelManager.get("Robot/RobotFoot.obj"), 1.75f);
            sceneManager.octree.insert(enemy);
            gameObjects.general.push_back(enemy);

            // Enemy 3
            enemyPath = {
                vec3(0, -40, -76.0),
                vec3(0, -47, -76.0),
                vec3(0, -53, -76.0),
                vec3(0, -60, -76.0)
            };
            enemy = make_shared<Enemy>(enemyPath, quat(1, 0, 0, 0), modelManager.get("Robot/RobotHead.obj"), modelManager.get("Robot/RobotLeg.obj"), modelManager.get("Robot/RobotFoot.obj"), 1.75f);
            sceneManager.octree.insert(enemy);
            gameObjects.general.push_back(enemy);

            // Enemy 4
            enemyPath = {
                vec3(-84, -10, -108.0),
                vec3(-84, -3, -108.0),
                vec3(-84, 3, -108.0),
                vec3(-84, 10, -108.0)
            };
            enemy = make_shared<Enemy>(enemyPath, quat(1, 0, 0, 0), modelManager.get("Robot/RobotHead.obj"), modelManager.get("Robot/RobotLeg.obj"), modelManager.get("Robot/RobotFoot.obj"), 1.75f);
            sceneManager.octree.insert(enemy);
            gameObjects.general.push_back(enemy);

            // Enemy 5
            enemyPath = {
                vec3(-40, 10, 30),
                vec3(-40, 13, 30),
                vec3(-40, 16, 30),
                vec3(-40, 19, 30)
            };
            enemy = make_shared<Enemy>(enemyPath, quat(1, 0, 0, 0), modelManager.get("Robot/RobotHead.obj"), modelManager.get("Robot/RobotLeg.obj"), modelManager.get("Robot/RobotFoot.obj"), 1.75f);
            sceneManager.octree.insert(enemy);
            gameObjects.general.push_back(enemy);

            // Blower 1
            auto blower = make_shared<Blower>(vec3(0, -35, -108), rotate(quat(1, 0, 0, 0), 0.0f, vec3(0, 0, 1)), 3.0f, 10.0f);
            blower->force = 200;
            blower->init(emitterManager.get("wind"));
            sceneManager.octree.insert(blower);
            gameObjects.general.push_back(blower);

            // Blower 2
            blower = make_shared<Blower>(vec3(-80, 4, 2), rotate(quat(1, 0, 0, 0), 0.0f, vec3(0, 0, 1)), 1.0f, 30.0f);
            blower->force = 500;
            sceneManager.octree.insert(blower);
            gameObjects.general.push_back(blower);
        }

        // Goal functionality
        gameObjects.goal->init(emitterManager.get("fireworks"), &startTime);
        sceneManager.octree.insert(gameObjects.goal);

        // Beam
        gameObjects.beam = make_shared<Beam>(gameObjects.goal->position + vec3(0, 10, 0), quat(0, 1, 0, 0), modelManager.get("cone.obj"));
        gameObjects.beam->scale = vec3(10, 20, 10);
        sceneManager.octree.insert(gameObjects.beam);

        sceneManager.octree.init(modelManager.get("billboard.obj"), modelManager.get("cube.obj"));
    }

	void loadUIObjects() {
		uiObjects.logo = make_shared<UIObject>(vec3(-0.78f, 0.78f, 0), vec3(0.4f, 0.4f, 0), quat(1, 1, 1, 1), modelManager.get("billboard.obj"), textureManager.get(preferences.scenes.startup == 0 ? "hud/Level1.png" : "hud/Level2.png", 0, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE));
		uiObjects.winMessage = make_shared<UIObject>(vec3(0, 0, 0), vec3(0.8f, 0.4f, 0), quat(1, 1, 1, 1), modelManager.get("billboard.obj"), textureManager.get("hud/YouWin.png", 0, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE));
	}

    /*
     * Rendering
     */
    void render()
    {
        // Get current frame buffer size.
        int width;
        int height;
        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);

        mat4 LS;

        if (preferences.shadows.resolution > 0) drawShadowMap(&LS);

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (debugLight) drawDepthMap();
        else renderPlayerView(&LS);
    }

    void drawScene(shared_ptr<Program> shader)
    {
        // Create the model transformation matrix stack
        auto M = make_shared<MatrixStack>();
        M->pushMatrix();
        M->loadIdentity();

        shared_ptr<Program> pbr = shaderManager.get("pbr");
        shared_ptr<Program> objectMap = shaderManager.get("object_map");

        if (shader == pbr)
        {
            glUniform1f(shader->getUniform("shadowResolution"), (float)preferences.shadows.resolution);
            glUniform1f(shader->getUniform("shadowAA"), (float)preferences.shadows.samples);

            glUniform3fv(shader->getUniform("viewPos"), 1, value_ptr(camera->eye));
        }

        // Draw marble
        if (shader == pbr) marble->getSkinMaterial()->bind();
        marble->draw(shader, M);

        // Draw enemies
            if (shader == pbr) materialManager.get("rusted_metal", "jpg")->bind();
        if (preferences.scenes.startup == 0)
        {
            gameObjects.enemy1->draw(shader, M);
            gameObjects.enemy2->draw(shader, M);
            gameObjects.sentry1->draw(shader, M);
            gameObjects.sentry2->draw(shader, M);
            if (!gameObjects.powerUp1->destroyed)
            {
                gameObjects.powerUp1->draw(shader,M);
            }
            if (!gameObjects.powerUp2->destroyed)
            {
                gameObjects.powerUp2->draw(shader,M);
            }
        }

        // Draw scene instances
        int i = 0;
        for (shared_ptr<Instance> instance : sceneManager.scene)
        {
            if (shader == pbr) instance->material->bind();
            else if (shader == objectMap)
                glUniform1i(shader->getUniform("objectIndex"), i);
            instance->physicsObject->draw(shader, M);
            i++;
        }

        for (auto obj : gameObjects.general)
        {
            obj->draw(shader, M);
        }

        // Cleanup
        M->popMatrix();
    }

    shared_ptr<Instance> getClickedObject(int x, int y)
    {
        int width, height;
        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
        y = height - y;

        GameObject::setCulling(true);
        glBindFramebuffer(GL_FRAMEBUFFER, objectMapFBO);

        glViewport(0, 0, width, height);

        // Resize textures to current window size
        glBindTexture(GL_TEXTURE_2D, objectMapTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, width, height, 0, GL_RED_INTEGER, GL_INT, NULL);
        glBindTexture(GL_TEXTURE_2D, objectMapDepthBuf);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

        glClear(GL_DEPTH_BUFFER_BIT);
        GLint clear = -2;
        glClearBufferiv(GL_COLOR, 0, &clear);

        shared_ptr<Program> objectMap = shaderManager.get("object_map");

        // Render scene as object IDs
        objectMap->bind();
        setProjectionMatrix(objectMap);
        setView(objectMap);
        glUniform1i(objectMap->getUniform("objectIndex"), -1);
        drawScene(objectMap);
        objectMap->unbind();

        // Get index of clicked object in sceneManager.scene
        // -2: no object clicked
        // -1: object clicked but not in sceneManager.scene
        GLint index;
        glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &index);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (index >= 0)
        {
            return sceneManager.scene[index];
        }
        else
        {
            return nullptr;
        }
    }

    void drawShadowMap(mat4 *LS)
    {
        GameObject::setCulling(false);

        // set up light's depth map
        glViewport(0, 0, preferences.shadows.resolution, preferences.shadows.resolution); // shadow map width and height
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);

        shared_ptr<Program> depth = shaderManager.get("depth");

        depth->bind();
        mat4 LP = SetOrthoMatrix(depth);
        mat4 LV = SetLightView(depth, sceneManager.light.direction, vec3(60, 0, 0), vec3(0, 1, 0));
        *LS = LP * LV;
        drawScene(depth);
        depth->unbind();
        glCullFace(GL_BACK);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void drawDepthMap()
    {
        // Code to draw the light depth buffer

        GameObject::setCulling(true);

        // geometry style debug on light - test transforms, draw geometry from light perspective
        if (debugGeometry)
        {
            shared_ptr<Program> depthDebug = shaderManager.get("depth_debug");

            depthDebug->bind();
            // render scene from light's point of view
            SetOrthoMatrix(depthDebug);
            SetLightView(depthDebug, sceneManager.light.direction, vec3(60, 0, 0), vec3(0, 1, 0));
            drawScene(depthDebug);
            depthDebug->unbind();
        }
        else
        {
            shared_ptr<Program> debug = shaderManager.get("debug");

            // actually draw the light depth map
            debug->bind();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            glUniform1i(debug->getUniform("texBuf"), 0);
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, fboQuadVertexBuffer);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glDisableVertexAttribArray(0);
            debug->unbind();
        }
    }

    void drawBeam()
    {
        if (!gameObjects.beam->inView) return;

        shared_ptr<Program> beam = shaderManager.get("beam");
        shared_ptr<Program> depth = shaderManager.get("world_depth");

        int width, height;
        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);

        // Render the depth of the scene to a texture
        depth->bind();
        setProjectionMatrix(depth);
        setView(depth);

        glBindFramebuffer(GL_FRAMEBUFFER, beamFBO);
        glDisable(GL_BLEND);

        glBindTexture(GL_TEXTURE_2D, beamWorldDepthBuf);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);        
        glBindTexture(GL_TEXTURE_2D, beamDepthBuf);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

        glViewport(0, 0, width, height);
        glClear(GL_DEPTH_BUFFER_BIT);

        float clear = 0;
        glClearBufferfv(GL_COLOR, 0, &clear);

        drawScene(depth);

        // Render the back faces of the beam
        glCullFace(GL_FRONT);
        gameObjects.beam->draw(depth, make_shared<MatrixStack>());
        glCullFace(GL_BACK);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glEnable(GL_BLEND);
        depth->unbind();

        // Render the beam using the depth data to make a volumetric effect
        beam->bind();
        setProjectionMatrix(beam);
        setView(beam);
        glUniform2f(beam->getUniform("viewport"), (float)width, (float)height);
        glUniform1i(beam->getUniform("depthBuf"), 0);
        glUniform1f(beam->getUniform("density"), sin(Time.timeSinceStart*3) * 0.025f + 0.075f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, beamWorldDepthBuf);
        gameObjects.beam->draw(beam, make_shared<MatrixStack>());
        
        beam->unbind();
    }

    void drawSkybox()
    {
        shared_ptr<Program> sky = shaderManager.get("sky");

        sky->bind();
        setProjectionMatrix(sky);
        setView(sky);

        skyboxManager.get("desert_hill", 1)->bind(sky->getUniform("Texture0"));
        glDepthMask(GL_FALSE);
        glDisable(GL_CULL_FACE);
        modelManager.get("cube.obj")->draw(sky);
        glEnable(GL_CULL_FACE);
        glDepthMask(GL_TRUE);

        sky->unbind();
    }

    void drawOctree()
    {
        shared_ptr<Program> circle = shaderManager.get("circle");

        circle->bind();
        setProjectionMatrix(circle);
        setView(circle);
        sceneManager.octree.drawDebugBoundingSpheres(circle);
        circle->unbind();

        shared_ptr<Program> cubeOutline = shaderManager.get("cube_outline");

        cubeOutline->bind();
        setProjectionMatrix(cubeOutline);
        setView(cubeOutline);
        sceneManager.octree.drawDebugOctants(cubeOutline);
        cubeOutline->unbind();
    }

    void drawGameplayUI()
    {
		auto M = make_shared<MatrixStack>();

        // Current level
		uiObjects.logo->draw(shaderManager.get("ui"), M, 1);

        // Win message
		if (gameObjects.goal->didWin) {
			uiObjects.winMessage->draw(shaderManager.get("ui"), M, 2);
		}
    }

    void renderPlayerView(mat4 *LS)
    {
        GameObject::setCulling(true);

        drawSkybox();

        shared_ptr<Program> pbr = shaderManager.get("pbr");
        pbr->bind();

        setLight(pbr);
        setProjectionMatrix(pbr);
        setView(pbr);

        // Send shadow map
        glActiveTexture(GL_TEXTURE30);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glUniform1i(shaderManager.get("pbr")->getUniform("shadowDepth"), 0);

        glUniformMatrix4fv(pbr->getUniform("LS"), 1, GL_FALSE, value_ptr(*LS));

        drawScene(pbr);

        pbr->unbind();

        drawBeam();

        if (sceneManager.octree.debug) drawOctree();

        shared_ptr<Program> particle = shaderManager.get("particle");
        particle->bind();
        setProjectionMatrix(particle);
        setView(particle);
        for (shared_ptr<ParticleEmitter> emitter : emitterManager.list())
        {
            emitter->draw(particle);
        }
        particle->unbind();

        drawGameplayUI();
    }

    /*
     * Render loop calls
     */
    void resetPlayer()
    {
        soundEngine->reset();

        marble->position = sceneManager.marbleStart;
        marble->setVelocity(vec3(0.0f));
        startTime = (float)glfwGetTime();
        gameObjects.goal->reset();
        marble->frozen = 0;
    }

    void beforePhysics()
    {
        gameObjects.goal->update();
        if (preferences.scenes.startup == 0)
        {
            gameObjects.enemy1->update(marble->position);
            gameObjects.enemy2->update(marble->position);
            gameObjects.sentry1->update(marble->position);
            gameObjects.sentry2->update(marble->position);
            if (!gameObjects.powerUp1->destroyed){
                gameObjects.powerUp1->update();
            }
            
            if (!gameObjects.powerUp2->destroyed){
                gameObjects.powerUp2->update();
            } 
            gameObjects.blower->update();
        }
    }

    void physicsTick()
    {
        sceneManager.octree.update();

        vector<shared_ptr<PhysicsObject>> instancesToCheck = sceneManager.octree.query(marble);
        for (shared_ptr<PhysicsObject> object : instancesToCheck)
        {
            object->checkCollision(marble.get());
        }

        for (shared_ptr<Instance> instance : sceneManager.scene)
        {
            instance->physicsObject->update();
        }

        for (auto obj : gameObjects.general)
        {
            obj->update();
        }
    }

    void beforeRender()
    {
        if (marble->position.y < sceneManager.deathBelow) resetPlayer();

        camera->update(marble);

        emitterManager.get("sparks")->update();
        emitterManager.get("fireworks")->update();
        emitterManager.get("wind")->update();

        viewFrustum.extractPlanes(setProjectionMatrix(nullptr), setView(nullptr));
        sceneManager.octree.markInView(viewFrustum);
    }

    /*
     * General
     */
    void setLight(shared_ptr<Program> prog)
    {
        glUniform3f(prog->getUniform("lightPosition"), sceneManager.light.direction.x, sceneManager.light.direction.y, sceneManager.light.direction.z);
        glUniform3f(prog->getUniform("lightColor"), sceneManager.light.brightness.x, sceneManager.light.brightness.y, sceneManager.light.brightness.z);
    }

    mat4 SetOrthoMatrix(shared_ptr<Program> curShade)
    {
        // shadow mapping helper

        mat4 ortho = glm::ortho(-96.0f, 96.0f, -96.0f, 96.0f, 0.1f, 500.0f);
        // fill in the glUniform call to send to the right shader!
        glUniformMatrix4fv(curShade->getUniform("LP"), 1, GL_FALSE, value_ptr(ortho));
        return ortho;
    }

    mat4 SetLightView(shared_ptr<Program> curShade, vec3 pos, vec3 LA, vec3 up)
    {
        // shadow mapping helper

        mat4 Cam = lookAt(pos, LA, up);
        // fill in the glUniform call to send to the right shader!
        glUniformMatrix4fv(curShade->getUniform("LV"), 1, GL_FALSE, value_ptr(Cam));
        return Cam;
    }

    mat4 setProjectionMatrix(shared_ptr<Program> curShade)
    {
        int width, height;
        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
        float aspect = width / (float)height;
        mat4 Projection = perspective(radians(50.0f), aspect, 0.1f, 200.0f);
        if (curShade != nullptr)
        {
            glUniformMatrix4fv(curShade->getUniform("P"), 1, GL_FALSE, value_ptr(Projection));
        }
        return Projection;
    }

    mat4 setView(shared_ptr<Program> curShade)
    {
        mat4 Cam = lookAt(camera->eye, camera->lookAtPoint, camera->upVec);
        if (curShade != nullptr)
        {
            glUniformMatrix4fv(curShade->getUniform("V"), 1, GL_FALSE, value_ptr(Cam));
        }
        return Cam;
    }

    /*
     * User input
     */
    void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_O && action == GLFW_PRESS)
        {
            marble->nextSkin();
        }
        else if (key == GLFW_KEY_P && action == GLFW_PRESS)
        {
            soundEngine->playPauseMusic();
        }
        else if (key == GLFW_KEY_L && action == GLFW_PRESS)
        {
            soundEngine->nextTrackMusic();
        }
        // other call backs
        else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            glfwSetInputMode(windowManager->getHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        else if (key == GLFW_KEY_Z && action == GLFW_PRESS)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else if (key == GLFW_KEY_Z && action == GLFW_RELEASE)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
        {
            editMode = !editMode;
            camera->cameraMode = editMode ? Camera::edit : Camera::marble;
            marble->frozen = editMode;
            if (editMode)
            {
                camera->saveMarbleView();
                marble->playPosition = marble->position;
                marble->position = marble->startPosition;
            }
            else
            {
                camera->restoreMarbleView();
                marble->position = marble->playPosition;
            }
        }
        else if (key == GLFW_KEY_U && action == GLFW_PRESS)
        {
            debugLight = !debugLight;
        }
        else if (key == GLFW_KEY_H && action == GLFW_PRESS)
        {
            sceneManager.octree.debug = !sceneManager.octree.debug;
        }
        else if (key == GLFW_KEY_R && action == GLFW_PRESS)
        {
            resetPlayer();
            if (preferences.scenes.startup == 0)
            {
                gameObjects.powerUp1->destroyed = false;
                gameObjects.powerUp2->destroyed = false;
            }
        }
        else if (key == GLFW_KEY_M && action == GLFW_PRESS)
        { // just a test since super bounce has no trigger yet
            soundEngine->superBounce();
        }
        // TODO - FIX camera flythrough so that it follows a predefined bezier curve
        //   1st - find points in the world first to use for debugging/ trial with
        
        //else if (key == GLFW_KEY_C && action == GLFW_PRESS)
        //{
        //    camera->previewLvl = !camera->previewLvl;
        //    if (camera->previewLvl)
        //    {
        //        camera->startLvlPreview(CENTER_LVL_POSITION);
        //    }
        //}
    }

    void scrollCallback(GLFWwindow *window, double deltaX, double deltaY)
    {
        deltaY *= -1;
        float newDistance = (float)deltaY + camera->distToBall;
        if (newDistance < 20 && newDistance > 2.5)
        {
            camera->distToBall += (float)deltaY;
        }
    }

    void mouseCallback(GLFWwindow *window, int button, int action, int mods)
    {
        double posX, posY;

        if (action == GLFW_PRESS)
        {
            glfwGetCursorPos(window, &posX, &posY);
            cout << "Pos X " << posX << " Pos Y " << posY << endl;
            cout << "" << marble->position.x << ", " << marble->position.y << ", " << marble->position.z << endl;

            if (editMode)
            {
                shared_ptr<Instance> instance = getClickedObject((int)posX, (int)posY);
                if (instance != nullptr)
                {
                    instance->physicsObject->position += vec3(0, 1, 0);
                }
            }
        }

        if (action == GLFW_RELEASE)
        {
        }

        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        {
            camera->freeViewing = true;
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        {
            camera->freeViewing = false;
        }
    }

    void updateAudio()
    {
        soundEngine->updateMusic();
    }

    void resizeCallback(GLFWwindow *window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }
};

int main(int argc, char **argv)
{
    Application *application = new Application();

    // Load game
    application->loadWindow();
    application->loadCanvas();
    application->loadShaders();
    application->loadSkybox();
    application->loadShadows();
    application->loadObjectMap();
    application->loadBeam();
    application->loadEffects();
    application->loadFBOQuad();
    application->loadModels();
    application->loadLevel();
    application->loadGameObjects();
    application->loadSounds();
	application->loadUIObjects();

    application->startTime = (float)glfwGetTime();

    float startTimestamp = (float)glfwGetTime();
    float lastFrameTimestamp = 0;
    float lastPhysicsTimestamp = 0;
    float rollingAverageFrameTime = 0;
    float accumulator = 0;
    Time.physicsDeltaTime = 0.02f;

    // Loop until the user closes the window.
    while (!glfwWindowShouldClose(application->windowManager->getHandle()))
    {
        float t = (float)glfwGetTime();
        Time.deltaTime = t - Time.timeSinceStart - startTimestamp;
        accumulator += Time.deltaTime;
        Time.timeSinceStart = t - startTimestamp;

        while (accumulator >= Time.physicsDeltaTime)
        {
            // Call gameobject physics update and ropagate physics
            application->beforePhysics();
            application->physicsTick();
            accumulator -= Time.physicsDeltaTime;
        }

        rollingAverageFrameTime = rollingAverageFrameTime * (20 - 1) / 20 + (Time.deltaTime / 20);
        // printf("FPS: %i\n", (int)(1.0 / rollingAverageFrameTime));

        // Get user input, call gameobject update, and render visuals
        glfwPollEvents();
        application->beforeRender();
        application->render();
        glfwSwapBuffers(application->windowManager->getHandle());
    }

    application->windowManager->shutdown();
    return 0;
}