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
#include <glad/glad.h>
#include <string>
#include <irrKlang.h>

#include "Camera.h"
#include "GLSL.h"
#include "GLTextureWriter.h"
#include "MatrixStack.h"
#include "Object3D.h"
#include "Program.h"
#include "effects/Sound.h"
#include "Shape.h"
#include "Skybox.h"
#include "WindowManager.h"
#include "engine/Collider.h"
#include "engine/ColliderSphere.h"
#include "engine/GameObject.h"
#include "gameobjects/Ball.h"
#include "gameobjects/Box.h"
#include "gameobjects/Goal.h"
#include "gameobjects/Enemy.h"
#include "engine/ColliderSphere.h"
#include "engine/Collider.h"
#include "engine/Octree.h"
#include "engine/Frustum.h"
#include "engine/ParticleEmitter.h"
#include "engine/SceneManager.h"
#include "engine/ModelManager.h"
#include "engine/TextureManager.h"
#include "effects/ParticleSpark.h"

// value_ptr for glm
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// number of skin textures to load and swap through
#define SHADOW_QUALITY 4 // [-1, 0, 1, 2, 3, 4] (-1: default) (0: OFF);
#define PLAY_MUSIC false
#define RESOURCE_DIRECTORY string("../Resources")

using namespace std;
using namespace glm;

shared_ptr<Sound> soundEngine;

class Application : public EventCallbacks
{

public:
    WindowManager *windowManager = nullptr;
    SceneManager sceneManager = SceneManager();
    ModelManager modelManager = ModelManager(RESOURCE_DIRECTORY + "/models/");
    TextureManager textureManager = TextureManager(RESOURCE_DIRECTORY + "/textures/");
    shared_ptr<Octree> octree;

    // Game Info Globals
    bool editMode = false;
    float START_TIME = 0.0f;
    bool MOVING = false;
    bool MOUSE_DOWN = false;
    int SCORE = 0;
    vector<string> marbleSkins = { "brown_rock.png", "seaside_rocks.png", "coal_matte_tiles.png", "marble_tiles.png" };
    int currentSkin = 0;
    vec3 START_POSITION = vec3(120, 3, 7);
    vec3 CENTER_LVL_POSITION = vec3(70, 3, 40);

    // Shadow Globals
    int SHADOWS = 1;
    int SHADOW_AA = 4;
    int DEBUG_LIGHT = 0;
    int GEOM_DEBUG = 1;
    GLuint SHADOW_SIZE = 0;
    GLuint depthMapFBO = 0;
    GLuint depthMap = 0;

    // Light Position Globals
    vec3 gameLight = vec3(300, 150, 250);
    vec3 gameLightColor = vec3(250000, 250000, 250000);

    struct
    {
        shared_ptr<Program> pbr;
        shared_ptr<Program> sky;
        shared_ptr<Program> circle;
        shared_ptr<Program> cubeOutline;
        shared_ptr<Program> depth;
        shared_ptr<Program> depthDebug;
        shared_ptr<Program> debug;
        shared_ptr<Program> particle;
    } programs;

    // Effects
    shared_ptr<ParticleEmitter> sparkEmitter;
    shared_ptr<ParticleEmitter> fireworkEmitter;

    // Camera
    shared_ptr<Camera> camera;
    Frustum viewFrustum;

    struct
    {
        shared_ptr<Ball> marble;
        shared_ptr<Enemy> enemy1;
        shared_ptr<Enemy> enemy2;
        shared_ptr<Goal> goal;
        shared_ptr<Box> goalObject;
    } gameObjects;
    vector<shared_ptr<PhysicsObject>> boxes;

    // BillBoard for rendering a texture to screen (like the shadow map)
    GLuint quad_VertexArrayID;
    GLuint quad_vertexbuffer;

    shared_ptr<Skybox> skybox;

    void init()
    {
        int width, height;
        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
        GLSL::checkVersion();

        // Set background color.
        glClearColor(.12f, .34f, .56f, 1.0f);
        // Enable z-buffer test.
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        // Initialize camera
        camera = make_shared<Camera>(windowManager, CENTER_LVL_POSITION);
        camera->init();

        sparkEmitter = make_shared<ParticleEmitter>(100);
        soundEngine = make_shared<Sound>();
        #if PLAY_MUSIC
        soundEngine->music();
        #endif
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }

    //=================================================
    // SHADERS
    //=================================================
    void initShader(shared_ptr<Program> &program, string file, vector<string> attributes, vector<string> uniforms)
    {
        // Shader for textured models
        program = make_shared<Program>();
        program->setVerbose(true);
        program->setShaderNames(RESOURCE_DIRECTORY + "/shaders/" + file + ".vert.glsl", RESOURCE_DIRECTORY + "/shaders/" + file + ".frag.glsl");
        if (!program->init())
        {
            cerr << "Failed to compile " << file << " shader" << endl;
            exit(1);
        }

        for (int i = 0; i < attributes.size(); i++)
        {
            program->addAttribute(attributes[i]);
        }

        for (int i = 0; i < uniforms.size(); i++)
        {
            program->addUniform(uniforms[i]);
        }
    }

    void initShaders()
    {
        initShader(
            programs.sky,
            "sky",
            {"vertPos"},
            {"P", "V", "Texture0"});

        initShader(
            programs.pbr,
            "pbr",
            {"vertPos", "vertNor", "vertTex"},
            {"P", "V", "M", "shadows", "shadowSize", "shadowAA", "shadowDepth", "LS", "albedoMap", "roughnessMap", "metallicMap", "aoMap", "lightPosition", "lightColor", "viewPos"});

        initShader(
            programs.circle,
            "circle",
            {"vertPos"},
            {"P", "V", "M", "radius"});

        initShader(
            programs.cubeOutline,
            "cube_outline",
            {"vertPos"},
            {"P", "V", "M", "edge"});

        initShader(
            programs.depth,
            "depth",
            {"vertPos"},
            {"LP", "LV", "M"});

        initShader(
            programs.debug,
            "pass",
            {"vertPos"},
            {"texBuf"});

        initShader(
            programs.depthDebug,
            "depth_debug",
            {"vertPos"},
            {"LP", "LV", "M"});

        initShader(
            programs.particle,
            "particle",
            {"vertPos", "vertTex"},
            {"P", "V", "M", "pColor", "alphaTexture"});
    }

    //=================================================
    // TEXTURES
    //=================================================
    void initTextures()
    {
        bindMaterialPBR("pbr/marble_tiles.png", false);
        bindMaterialPBR("pbr/coal_matte_tiles.png", false);
        bindMaterialPBR("pbr/brown_rock.png", false);

        initSkyBox();
        initParticleTexture();
        initShadow();
    }

    void bindMaterialPBR(string fileName, bool bind = true) {
        int extensionDotIndex = fileName.find_last_of(".");
        string beforeExtension = fileName.substr(0, extensionDotIndex);
        string dotAndExtension = fileName.substr(extensionDotIndex);

        shared_ptr<Texture> albedo = textureManager.get(beforeExtension + "_albedo" + dotAndExtension, 1);
        shared_ptr<Texture> roughness = textureManager.get(beforeExtension + "_roughness" + dotAndExtension, 2);
        shared_ptr<Texture> metallic = textureManager.get(beforeExtension + "_metallic" + dotAndExtension, 3);
        shared_ptr<Texture> ao = textureManager.get(beforeExtension + "_ao" + dotAndExtension, 4);

        if (bind) {
            albedo->bind(programs.pbr->getUniform("albedoMap"));
            roughness->bind(programs.pbr->getUniform("roughnessMap"));
            metallic->bind(programs.pbr->getUniform("metallicMap"));
            ao->bind(programs.pbr->getUniform("aoMap"));
        }
    }

    void initParticleTexture()
    {
        textureManager.get("particle/star_07.png", 1);
        textureManager.get("particle/scorch_02.png", 1);
    }

    void initSkyBox()
    {
        // Load skybox
        string skyboxFilenames[] = {"px.jpg", "nx.jpg", "py.jpg", "ny.jpg", "pz.jpg", "nz.jpg"};
        for (int i = 0; i < 6; i++)
        {
            skyboxFilenames[i] = RESOURCE_DIRECTORY + "/skybox/" + skyboxFilenames[i];
        }

        skybox = make_shared<Skybox>();
        skybox->setFilenames(skyboxFilenames);
        skybox->init();
        skybox->setUnit(1);
        skybox->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    }

    void initShadow()
    {
        // Calculate shadow quality
        if (SHADOW_QUALITY == 0) SHADOW_AA = 1;
        SHADOWS = !!SHADOW_QUALITY;
        SHADOW_SIZE = 256 * (int)pow(2, SHADOW_QUALITY);

        // Generate the FBO for the shadow depth
        glGenFramebuffers(1, &depthMapFBO);

        // Generate the texture
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_SIZE, SHADOW_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

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

    //=================================================
    // Sounds
    //=================================================

    //=================================================
    // GEOMETRY
    //=================================================
    void initGeom()
    {
        initOctree();
        initEffects();
        initFBOQuad();
        loadModels();
        loadLevel();
        initGameObjects();
    }

    void initOctree() {
        octree = make_shared<Octree>(vec3(-200, -210, -200), vec3(200, 190, 200));
        octree->init(modelManager.get("billboard.obj"), modelManager.get("cube.obj"));
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
    }

    void loadLevel() {
        sceneManager.load(RESOURCE_DIRECTORY + "/levels/Level-1.yaml");

        ifstream inLevel(RESOURCE_DIRECTORY + "/levels/Level1.txt");
        vec3 transform;
        while (inLevel >> transform.x) {
            inLevel >> transform.y >> transform.z;
            shared_ptr<Box> box = make_shared<Box>(vec3(transform.x * 8, transform.y, transform.z * 6), quat(1, 0, 0, 0), vec3(8, 2, 6), modelManager.get("cube.obj"));
            boxes.push_back(box);
        }
        octree->insert(boxes);
    }

    void initGameObjects()
    {
        // Marble
        gameObjects.marble = make_shared<Ball>(START_POSITION, quat(1, 0, 0, 0), modelManager.get("quadSphere.obj"), 1);
        gameObjects.marble->init(windowManager, sparkEmitter);
        octree->insert(gameObjects.marble);

        // Enemy 1
        vector<glm::vec3> enemyPath = {
            vec3{95.0, 2.0, 7.0},
            vec3{100.0, 2.0, 15.0},
            vec3{110.0, 2.0, -1.0},
            vec3{115.0, 2.0, 7.0}};
        gameObjects.enemy1 = make_shared<Enemy>(enemyPath, quat(1, 0, 0, 0), modelManager.get("Robot/RobotHead.obj"), modelManager.get("Robot/RobotLeg.obj"), modelManager.get("Robot/RobotFoot.obj"), 1.5);
        gameObjects.enemy1->init(windowManager);
        octree->insert(gameObjects.enemy1);

        // Enemy 2
        enemyPath = {
            vec3{125.0, 8.0, 55.0},
            vec3{115.0, 20.0, 55.0},
            vec3{105.0, 5.0, 55.0},
            vec3{95.0, 8.0, 55.0}};
        gameObjects.enemy2 = make_shared<Enemy>(enemyPath, quat(1, 0, 0, 0), modelManager.get("Robot/RobotHead.obj"), modelManager.get("Robot/RobotLeg.obj"), modelManager.get("Robot/RobotFoot.obj"), 1.5);
        gameObjects.enemy2->init(windowManager);
        octree->insert(gameObjects.enemy2);

        // Goal model
        gameObjects.goalObject = make_shared<Box>(vec3(0, 11.5, 0), quat(1, 0, 0, 0), vec3(4, 4, 4), modelManager.get("goal.obj"));
        octree->insert(gameObjects.goalObject);

        // Goal functionality
        gameObjects.goal = make_shared<Goal>(gameObjects.goalObject->position + vec3(0, 1, 0), quat(1, 0, 0, 0), nullptr, 1);
        gameObjects.goal->init(fireworkEmitter, &START_TIME);
        octree->insert(gameObjects.goal);
    }

    void initEffects() {
        sparkEmitter = make_shared<ParticleEmitter>(100);
        sparkEmitter->init(modelManager.get("billboard.obj"), textureManager.get("particle/star_07.png"));
        fireworkEmitter = make_shared<ParticleEmitter>(100);
        fireworkEmitter->init(modelManager.get("billboard.obj"), textureManager.get("particle/scorch_02.png"));
    }

    void initFBOQuad()
    {
        /* set up a quad for rendering a framebuffer */

        // now set up a simple quad for rendering FBO
        glGenVertexArrays(1, &quad_VertexArrayID);
        glBindVertexArray(quad_VertexArrayID);

        static const GLfloat g_quad_vertex_buffer_data[] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,

            -1.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
        };

        glGenBuffers(1, &quad_vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
    }

    //=================================================
    // RENDERERING

    void render()
    {
        // Get current frame buffer size.
        int width, height;
        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);

        mat4 LS;

        if (SHADOW_QUALITY) createShadowMap(&LS);

        glViewport(0, 0, width, height); // frame width and height
        // Clear framebuffer.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (DEBUG_LIGHT) drawDepthMap();
        else renderPlayerView(&LS);
    }

    void drawScene(shared_ptr<Program> shader)
    {
        // Create the model transformation matrix stack
        auto M = make_shared<MatrixStack>();
        M->pushMatrix();
        M->loadIdentity();

        if (shader == programs.pbr)
        {
            glUniform1f(shader->getUniform("shadowSize"), (float)SHADOW_SIZE);
            glUniform1f(shader->getUniform("shadowAA"), (float)SHADOW_AA);
            glUniform1i(shader->getUniform("shadows"), SHADOWS);

            glUniform3fv(shader->getUniform("viewPos"), 1, value_ptr(camera->eye));
        }

        // Draw marble
        if (shader == programs.pbr) bindMaterialPBR("pbr/" + marbleSkins[currentSkin]);
        gameObjects.marble->draw(shader, M);

        // Draw finish
        if (shader == programs.pbr) bindMaterialPBR("pbr/painted_metal.png");
        gameObjects.goalObject->draw(shader, M);

        // Draw enemies
        if (shader == programs.pbr) bindMaterialPBR("pbr/rusted_metal.jpg");
        gameObjects.enemy1->draw(shader, M);
        gameObjects.enemy2->draw(shader, M);

        // Draw Boxes
        if (shader == programs.pbr) bindMaterialPBR("pbr/marble_tiles.png");
        for (auto box : boxes)
        {
            box->draw(shader, M);
        }

        // Cleanup
        M->popMatrix();
    }

    void createShadowMap(mat4 *LS)
    {
        // set up light's depth map
        glViewport(0, 0, SHADOW_SIZE, SHADOW_SIZE); // shadow map width and height
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);

        // set up shadow shader
        // render scene
        programs.depth->bind();
        // TODO you will need to fix these
        mat4 LP = SetOrthoMatrix(programs.depth);
        mat4 LV = SetLightView(programs.depth, gameLight, vec3(60, 0, 0), vec3(0, 1, 0));
        *LS = LP * LV;
        // SetLightView(programs.depth, gameLight, g_lookAt, vec3(0, 1, 0));
        drawScene(programs.depth);
        programs.depth->unbind();
        glCullFace(GL_BACK);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void drawDepthMap()
    {
        // Code to draw the light depth buffer

        // geometry style debug on light - test transforms, draw geometry from
        // light perspective
        if (GEOM_DEBUG)
        {
            programs.depthDebug->bind();
            // render scene from light's point of view
            SetOrthoMatrix(programs.depthDebug);
            SetLightView(programs.depthDebug, gameLight, vec3(60, 0, 0), vec3(0, 1, 0));
            drawScene(programs.depthDebug);
            programs.depthDebug->unbind();
        }
        else
        {
            // actually draw the light depth map
            programs.debug->bind();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            glUniform1i(programs.debug->getUniform("texBuf"), 0);
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glDisableVertexAttribArray(0);
            programs.debug->unbind();
        }
    }

    void drawSkyBox()
    {
        programs.sky->bind();
        setProjectionMatrix(programs.sky);
        setView(programs.sky);

        skybox->bind(programs.sky->getUniform("Texture0"));
        glDepthMask(GL_FALSE);
        glDisable(GL_CULL_FACE);
        modelManager.get("cube.obj")->draw(programs.sky);
        glEnable(GL_CULL_FACE);
        glDepthMask(GL_TRUE);

        programs.sky->unbind();
    }

    void sendShadowMap()
    {
        // Also set up light depth map
        glActiveTexture(GL_TEXTURE30);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glUniform1i(programs.pbr->getUniform("shadowDepth"), 0);
    }

    void renderPlayerView(mat4 *LS)
    {
        drawSkyBox();

        programs.pbr->bind();

        setLight(programs.pbr);
        setProjectionMatrix(programs.pbr);
        setView(programs.pbr);

        sendShadowMap();

        glUniformMatrix4fv(programs.pbr->getUniform("LS"), 1, GL_FALSE, value_ptr(*LS));
        drawScene(programs.pbr);

        programs.pbr->unbind();

        if (octree->debug)
        {
            drawOctree();
        }

        programs.particle->bind();
        setProjectionMatrix(programs.particle);
        setView(programs.particle);
        sparkEmitter->draw(programs.particle);
        fireworkEmitter->draw(programs.particle);
        programs.particle->unbind();
    }

    void drawOctree()
    {
        programs.circle->bind();
        setProjectionMatrix(programs.circle);
        setView(programs.circle);
        octree->drawDebugBoundingSpheres(programs.circle);
        programs.circle->unbind();

        programs.cubeOutline->bind();
        setProjectionMatrix(programs.cubeOutline);
        setView(programs.cubeOutline);
        octree->drawDebugOctants(programs.cubeOutline);
        programs.cubeOutline->unbind();
    }

    //=================================================
    // GENERAL HELPERS

    void resetPlayer()
    {
        soundEngine->reset();

        gameObjects.marble->position = START_POSITION;
        gameObjects.marble->velocity = vec3(0);
        START_TIME = glfwGetTime();
        gameObjects.goal->reset();
    }

    void update(float dt)
    {
        octree->update();

        if (gameObjects.marble->position.y < -25.0)
        {
            resetPlayer();
        }

        auto boxesToCheck = octree->query(gameObjects.marble);
        for (auto box : boxesToCheck)
        {
            box->checkCollision(gameObjects.marble.get());
        }

        for (auto box : boxes)
        {
            box->update(dt);
        }
        //TODO:: Do Collision checks between marble and Enemy
        /*
        auto enemiesToCheck = octree->query(marble);
        for (auto enemies : enemiesToCheck) {
            
        }*/

        gameObjects.goalObject->update(dt);
        gameObjects.marble->update(dt, camera->dolly, camera->strafe);
        camera->update(dt, gameObjects.marble);
        gameObjects.goal->update(dt);
        gameObjects.enemy1->update(dt);
        gameObjects.enemy2->update(dt);

        sparkEmitter->update(dt);
        fireworkEmitter->update(dt);

        viewFrustum.extractPlanes(setProjectionMatrix(nullptr), setView(nullptr));
        octree->markInView(viewFrustum);
    }

    void setLight(shared_ptr<Program> prog)
    {
        glUniform3f(prog->getUniform("lightPosition"), gameLight.x, gameLight.y, gameLight.z);
        glUniform3f(prog->getUniform("lightColor"), gameLightColor.x, gameLightColor.y, gameLightColor.z);
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

    void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        //skin switching key call back
        if (key == GLFW_KEY_O && action == GLFW_PRESS)
        {
            currentSkin++;
            currentSkin %= marbleSkins.size();
        }
        // else if (key == GLFW_KEY_I && action == GLFW_PRESS)
        // {
            
        // }
        else if (key == GLFW_KEY_Y && action == GLFW_PRESS)
        {
            SHADOW_AA = (SHADOW_AA + 1) % 9;
            if (SHADOW_AA == 0)
            {
                SHADOW_AA++;
            }
        }
        else if (key == GLFW_KEY_T && action == GLFW_PRESS)
        {
            SHADOWS = !SHADOWS;
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
            gameObjects.marble->frozen = editMode;
            if (editMode) {
                camera->saveMarbleView();
                gameObjects.marble->playPosition = gameObjects.marble->position;
                gameObjects.marble->position = gameObjects.marble->startPosition;
            }
            else {
                camera->restoreMarbleView();
                gameObjects.marble->position = gameObjects.marble->playPosition;
            }
        }
        else if (key == GLFW_KEY_U && action == GLFW_PRESS)
        {
            DEBUG_LIGHT = !DEBUG_LIGHT;
        }
        else if (key == GLFW_KEY_H && action == GLFW_PRESS)
        {
            octree->debug = !octree->debug;
        }
        else if (key == GLFW_KEY_R && action == GLFW_PRESS)
        {
            resetPlayer();
        }
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
        float newDistance = deltaY + camera->distToBall;
        if (newDistance < 20 && newDistance > 2.5)
        {
            camera->distToBall += deltaY;
        }
    }

    void mouseCallback(GLFWwindow *window, int button, int action, int mods)
    {
        double posX, posY;

        if (action == GLFW_PRESS)
        {
            MOUSE_DOWN = true;
            glfwGetCursorPos(window, &posX, &posY);
            cout << "Pos X " << posX << " Pos Y " << posY << endl;
            cout << "" << gameObjects.marble->position.x << ", " << gameObjects.marble->position.y << ", " << gameObjects.marble->position.z << endl;
            MOVING = true;
        }

        if (action == GLFW_RELEASE)
        {
            MOVING = false;
            MOUSE_DOWN = false;
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

    void resizeCallback(GLFWwindow *window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }
};

int main(int argc, char **argv)
{
    Application *application = new Application();

    // Your main will always include a similar set up to establish your window
    // and GL context, etc.

    WindowManager *windowManager = new WindowManager();
    // windowManager->init(1280, 720);
    windowManager->init(1920, 1080);
    // windowManager->init(2560, 1440);
    windowManager->setEventCallbacks(application);
    application->windowManager = windowManager;

    // This is the code that will likely change program to program as you
    // may need to initialize or set up different data and state

    application->init();
    application->initShaders();
    application->initTextures();
    application->initGeom();

    application->START_TIME = glfwGetTime();

    double t = 0;
    const double dt = 0.02;
    double currentTime = application->START_TIME;
    double accumulator = 0;

    // Loop until the user closes the window.
    while (!glfwWindowShouldClose(windowManager->getHandle()))
    {
        // Render scene.
        double newTime = glfwGetTime();
        double frameTime = newTime - currentTime;
        currentTime = newTime;

        accumulator += frameTime;

        while (accumulator >= dt)
        {
            application->update(dt);
            accumulator -= dt;
            t += dt;
        }

        application->render();

        // Swap front and back buffers.
        glfwSwapBuffers(windowManager->getHandle());
        // Poll for and process events.
        glfwPollEvents();
    }

    // Quit program.
    windowManager->shutdown();
    return 0;
}
