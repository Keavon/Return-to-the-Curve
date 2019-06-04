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
#include "effects/ParticleSpark.h"

#include "engine/SceneManager.h"
#include "engine/ModelManager.h"
#include "engine/TextureManager.h"
#include "engine/SkyboxManager.h"
#include "engine/ShaderManager.h"
#include "engine/MaterialManager.h"
#include "engine/EmitterManager.h"

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
    SkyboxManager skyboxManager = SkyboxManager(RESOURCE_DIRECTORY + "/skyboxes/");
    ShaderManager shaderManager = ShaderManager(RESOURCE_DIRECTORY + "/shaders/");
    MaterialManager materialManager = MaterialManager();
    EmitterManager emitterManager = EmitterManager();
    shared_ptr<Octree> octree;

    // Game Info Globals
    bool editMode = false;
    float startTime = 0.0f;
    bool MOVING = false;
    bool MOUSE_DOWN = false;
    int SCORE = 0;
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

    // Billboard for rendering a texture to screen (like the shadow map)
    GLuint fboQuadVertexArrayID;
    GLuint fboQuadVertexBuffer;

    /*
     * Loading
    */
    void loadCanvas()
    {
        int width, height;
        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
        GLSL::checkVersion();

        // Enable z-buffer test.
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        // Initialize camera
        camera = make_shared<Camera>(windowManager, CENTER_LVL_POSITION);
        camera->init();
    }

    void loadSounds()
    {
        soundEngine = make_shared<Sound>();

        #if PLAY_MUSIC
            soundEngine->music();
        #endif

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void loadShaders()
    {
        vector<string> pbrUniforms = { "P", "V", "M", "shadows", "shadowSize", "shadowAA", "shadowDepth", "LS", "albedoMap", "roughnessMap", "metallicMap", "aoMap", "lightPosition", "lightColor", "viewPos" };
        shared_ptr<Program> pbr = shaderManager.get("pbr", { "vertPos", "vertNor", "vertTex" }, pbrUniforms);
        materialManager.init(pbr, shared_ptr<TextureManager>(&textureManager));

        shaderManager.get("sky", {"vertPos"}, {"P", "V", "Texture0"});
        shaderManager.get("circle", {"vertPos"}, {"P", "V", "M", "radius"});
        shaderManager.get("cube_outline", {"vertPos"}, {"P", "V", "M", "edge"});
        shaderManager.get("depth", {"vertPos"}, {"LP", "LV", "M"});
        shaderManager.get("pass", {"vertPos"}, {"texBuf"});
        shaderManager.get("depth_debug", {"vertPos"}, {"LP", "LV", "M"});
        shaderManager.get("particle", {"vertPos", "vertTex"}, {"P", "V", "M", "pColor", "alphaTexture"});
    }

    void loadMaterials()
    {
        materialManager.get("marble_tiles", "png");
        materialManager.get("coal_matte_tiles", "png");
        materialManager.get("brown_rock", "png");
        materialManager.get("seaside_rocks", "png");
        materialManager.get("coal_matte_tiles", "png");
        materialManager.get("marble_tiles", "png");
    }

    void loadSkybox()
    {
        skyboxManager.get("desert_hill", 1);
    }

    void loadParticleTextures()
    {
        textureManager.get("particles/star_07.png", 1);
        textureManager.get("particles/scorch_02.png", 1);
    }

    void loadShadows()
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

    void loadOctree() {
        octree = make_shared<Octree>(vec3(-200, -210, -200), vec3(200, 190, 200));
        octree->init(modelManager.get("billboard.obj"), modelManager.get("cube.obj"));
    }

    void loadEffects() {
        emitterManager.get("sparks", modelManager.get("billboard.obj"), textureManager.get("particles/star_07.png"), 100);
        emitterManager.get("fireworks", modelManager.get("billboard.obj"), textureManager.get("particles/scorch_02.png"), 100);
    }

    void loadFBOQuad() {
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

    void loadGameObjects()
    {
        // Marble
        gameObjects.marble = make_shared<Ball>(START_POSITION, quat(1, 0, 0, 0), modelManager.get("quadSphere.obj"), 1);
        gameObjects.marble->init(windowManager, emitterManager.get("sparks"));
        octree->insert(gameObjects.marble);
        gameObjects.marble->addSkin(materialManager.get("brown_rock", "png"));
        gameObjects.marble->addSkin(materialManager.get("seaside_rocks", "png"));
        gameObjects.marble->addSkin(materialManager.get("coal_matte_tiles", "png"));
        gameObjects.marble->addSkin(materialManager.get("marble_tiles", "png"));

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
        gameObjects.goal->init(emitterManager.get("fireworks"), &startTime);
        octree->insert(gameObjects.goal);
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

        if (SHADOW_QUALITY) drawShadowMap(&LS);

        glViewport(0, 0, width, height);
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

        shared_ptr<Program> pbr = shaderManager.get("pbr");

        if (shader == pbr)
        {
            glUniform1f(shader->getUniform("shadowSize"), (float)SHADOW_SIZE);
            glUniform1f(shader->getUniform("shadowAA"), (float)SHADOW_AA);
            glUniform1i(shader->getUniform("shadows"), SHADOWS);

            glUniform3fv(shader->getUniform("viewPos"), 1, value_ptr(camera->eye));
        }

        // Draw marble
        if (shader == pbr) gameObjects.marble->getSkinMaterial()->bind();
        gameObjects.marble->draw(shader, M);

        // Draw finish
        if (shader == pbr) materialManager.get("painted_metal", "png")->bind();
        gameObjects.goalObject->draw(shader, M);

        // Draw enemies
        if (shader == pbr) materialManager.get("rusted_metal", "jpg")->bind();
        gameObjects.enemy1->draw(shader, M);
        gameObjects.enemy2->draw(shader, M);

        // Draw Boxes
        if (shader == pbr) materialManager.get("marble_tiles", "png")->bind();
        for (auto box : boxes)
        {
            box->draw(shader, M);
        }

        // Cleanup
        M->popMatrix();
    }

    void drawShadowMap(mat4 *LS)
    {
        // set up light's depth map
        glViewport(0, 0, SHADOW_SIZE, SHADOW_SIZE); // shadow map width and height
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);

        shared_ptr<Program> depth = shaderManager.get("depth");

        depth->bind();
        // TODO you will need to fix these
        mat4 LP = SetOrthoMatrix(depth);
        mat4 LV = SetLightView(depth, gameLight, vec3(60, 0, 0), vec3(0, 1, 0));
        *LS = LP * LV;
        drawScene(depth);
        depth->unbind();
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
            shared_ptr<Program> depthDebug = shaderManager.get("depth_debug");

            depthDebug->bind();
            // render scene from light's point of view
            SetOrthoMatrix(depthDebug);
            SetLightView(depthDebug, gameLight, vec3(60, 0, 0), vec3(0, 1, 0));
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

    void drawOctree() {
        shared_ptr<Program> circle = shaderManager.get("circle");

        circle->bind();
        setProjectionMatrix(circle);
        setView(circle);
        octree->drawDebugBoundingSpheres(circle);
        circle->unbind();

        shared_ptr<Program> cubeOutline = shaderManager.get("cube_outline");

        cubeOutline->bind();
        setProjectionMatrix(cubeOutline);
        setView(cubeOutline);
        octree->drawDebugOctants(cubeOutline);
        cubeOutline->unbind();
    }

    void renderPlayerView(mat4 *LS)
    {
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

        if (octree->debug) drawOctree();

        shared_ptr<Program> particle = shaderManager.get("particle");
        particle->bind();
        setProjectionMatrix(particle);
        setView(particle);
        for (shared_ptr<ParticleEmitter> emitter : emitterManager.list()) {
            emitter->draw(particle);
        }
        particle->unbind();
    }

    /*
     * General
     */
    void resetPlayer()
    {
        soundEngine->reset();

        gameObjects.marble->position = START_POSITION;
        gameObjects.marble->velocity = vec3(0);
        startTime = glfwGetTime();
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

        emitterManager.get("sparks")->update(dt);
        emitterManager.get("fireworks")->update(dt);

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

    /*
     * User input
     */
    void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_O && action == GLFW_PRESS)
        {
            gameObjects.marble->nextSkin();
        }
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

    // Load game assets
    application->loadCanvas();
    application->loadSounds();
    application->loadShaders();
    application->loadMaterials();
    application->loadSkybox();
    application->loadParticleTextures();
    application->loadShadows();
    application->loadOctree();
    application->loadEffects();
    application->loadFBOQuad();
    application->loadModels();
    application->loadLevel();
    application->loadGameObjects();

    application->startTime = glfwGetTime();

    double t = 0;
    const double dt = 0.02;
    double currentTime = application->startTime;
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
