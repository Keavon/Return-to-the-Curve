#define _USE_MATH_DEFINES

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

// value_ptr for glm
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// number of skin textures to load and swap through
#define NUMBER_OF_MARBLE_SKINS 13
#define SHADOW_QUALITY 1 // [-1, 0, 1, 2, 3, 4] (-1: default) (0: OFF);

#define RESOURCE_DIRECTORY string("../Resources")

using namespace std;
using namespace glm;

extern shared_ptr<Sound> soundEngine;

class Application : public EventCallbacks
{

public:
    WindowManager *windowManager = nullptr;

    // Game Info Globals
    float START_TIME = 0.0f;
    bool MOVING = false;
    bool MOUSE_DOWN = false;
    bool SHOW_CURSOR = false;
    int SCORE = 0;
    int CURRENT_SKIN = 0;
    vec3 START_POSITION = vec3(120, 3, 7);
    vec3 CENTER_LVL_POSITION = vec3(70, 3, 40);

    // Shadow Globals
    int SHADOWS = 1;
    int SHADOW_AA = 2;
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

    struct
    {
        shared_ptr<Shape> cube;
        shared_ptr<Shape> roboHead;
        shared_ptr<Shape> roboLeg;
        shared_ptr<Shape> roboFoot;
        shared_ptr<Shape> boxModel;
        shared_ptr<Shape> plane;
        shared_ptr<Shape> billboard;
        shared_ptr<Shape> goalModel;
        shared_ptr<Shape> sphere;
    } shapes;

    // Effects
    shared_ptr<ParticleEmitter> sparkEmitter;
    shared_ptr<ParticleEmitter> fireworkEmitter;

    // Camera
    shared_ptr<Camera> camera;
    Frustum viewFrustum;

    struct
    {
        shared_ptr<Ball> ball;
        shared_ptr<Enemy> enemy1;
        shared_ptr<Enemy> enemy2;
        shared_ptr<Goal> goal;
        shared_ptr<Box> goalObject;
        shared_ptr<Octree> octree;
    } gameObjects;
    vector<shared_ptr<PhysicsObject>> boxes;

    // BillBoard for rendering a texture to screen. (like the shadow map)
    GLuint quad_VertexArrayID;
    GLuint quad_vertexbuffer;

    struct
    {
        shared_ptr<Skybox> skybox;
        shared_ptr<Texture> crateAlbedo;
        shared_ptr<Texture> crateRoughness;
        shared_ptr<Texture> crateMetallic;
        shared_ptr<Texture> crateAO;
        shared_ptr<Texture> panelAlbedo;
        shared_ptr<Texture> panelRoughness;
        shared_ptr<Texture> panelMetallic;
        shared_ptr<Texture> panelAO;
        shared_ptr<Texture> spark;
        shared_ptr<Texture> firework;
    } textures;
    vector<shared_ptr<Texture>> marbleTextures;

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
            {"vertPos", "vertNor", "vertTex"},
            {"LP", "LV", "M"});

        initShader(
            programs.debug,
            "pass",
            {"vertPos"},
            {"texBuf"});

        initShader(
            programs.depthDebug,
            "depth_debug",
            {"vertPos", "vertNor", "vertTex"},
            {"LP", "LV", "M"});

        initShader(
            programs.particle,
            "particle",
            {"vertPos", "vertNor", "vertTex"},
            {"P", "V", "M", "pColor", "alphaTexture"});
    }

    //=================================================
    // TEXTURES
    //=================================================
    void initTextures()
    {
        initPBR(textures.crateAlbedo, textures.crateRoughness, textures.crateMetallic, textures.crateAO, "marble_tiles", "png");
        initPBR(textures.panelAlbedo, textures.panelRoughness, textures.panelMetallic, textures.panelAO, "panel", "png");

        initMarbleTexture();
        initSkyBox();
        initParticleTexture();
        initShadow();
    }

    void initTexture(shared_ptr<Texture> &texture, string filePath, int textureUnit = 0)
    {
        texture = make_shared<Texture>();
        texture->setFilename(RESOURCE_DIRECTORY + "/textures/" + filePath);
        texture->init();
        texture->setUnit(textureUnit);
        texture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    }

    void initPBR(shared_ptr<Texture> &albedo, shared_ptr<Texture> &roughness, shared_ptr<Texture> &metallic, shared_ptr<Texture> &ao, string fileName, string fileExt)
    {
        initTexture(albedo, "pbr/" + fileName + "_albedo." + fileExt, 1);
        initTexture(roughness, "pbr/" + fileName + "_roughness." + fileExt, 2);
        initTexture(metallic, "pbr/" + fileName + "_metallic." + fileExt, 3);
        initTexture(ao, "pbr/" + fileName + "_ao." + fileExt, 4);
    }

    void initParticleTexture()
    {
        initTexture(textures.spark, "particle/star_07.png", 1);
        initTexture(textures.firework, "particle/scorch_02.png", 1);
    }

    void initMarbleTexture()
    {
        // loops over the number of skin textures, initializing them and adding them to a vector
        string textureBaseFolder, textureNumber, textureExtension, textureName;
        double completion = 0;
        for (int i = 1; i < NUMBER_OF_MARBLE_SKINS + 1; i++)
        {
            textureBaseFolder = "/textures/marble/albedo/";
            textureNumber = to_string(i);
            textureExtension = ".jpg";

            textureName = textureBaseFolder + textureNumber + textureExtension;
            completion = ((float)i * 100 / (float)NUMBER_OF_MARBLE_SKINS);
            cout << setprecision(3) << "Loading Textures: " << completion << "% complete." << endl;

            shared_ptr<Texture> marbleTexture = make_shared<Texture>();
            marbleTexture->setFilename(RESOURCE_DIRECTORY + textureName);
            marbleTexture->init();
            marbleTexture->setUnit(1);
            marbleTexture->setWrapModes(GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);

            marbleTextures.push_back(marbleTexture);
        }
        cout << "Loading Textures: complete." << endl;
    }

    void initSkyBox()
    {
        // Load skybox
        string skyboxFilenames[] = {"px.jpg", "nx.jpg", "py.jpg", "ny.jpg", "pz.jpg", "nz.jpg"};
        for (int i = 0; i < 6; i++)
        {
            skyboxFilenames[i] = RESOURCE_DIRECTORY + "/skybox/" + skyboxFilenames[i];
        }

        textures.skybox = make_shared<Skybox>();
        textures.skybox->setFilenames(skyboxFilenames);
        textures.skybox->init();
        textures.skybox->setUnit(1);
        textures.skybox->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    }

    void initShadow()
    {
        switch (SHADOW_QUALITY)
        {
        case 0:
            SHADOWS = 0;
            SHADOW_AA = 1;
            SHADOW_SIZE = 256;
            break;
        case 1:
            SHADOWS = 1;
            SHADOW_SIZE = 512;
            break;
        case 2:
            SHADOWS = 1;
            SHADOW_SIZE = 1024;
            break;
        case 3:
            SHADOWS = 1;
            SHADOW_SIZE = 2048;
            break;
        case 4:
            SHADOWS = 1;
            SHADOW_SIZE = 4096;
            break;
        default:
            SHADOWS = 1;
            SHADOW_SIZE = 1024;
            break;
        }
        /* set up the FBO for the light's depth */
        // generate the FBO for the shadow depth
        glGenFramebuffers(1, &depthMapFBO);

        // generate the texture
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_SIZE, SHADOW_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // bind with framebuffer's depth buffer
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void setTextureMaterial(int i)
    {
        // pulled these out of the switch since they were all identical

        textures.crateAlbedo->bind(programs.pbr->getUniform("albedoMap"));
        textures.crateRoughness->bind(programs.pbr->getUniform("roughnessMap"));
        textures.crateMetallic->bind(programs.pbr->getUniform("metallicMap"));

        switch (i)
        {
        case 0:
            // marbleTextures[CURRENT_SKIN]->bind(programs.pbr->getUniform("albedoMap"));
            break;
        case 1:
            marbleTextures[CURRENT_SKIN]->bind(programs.pbr->getUniform("albedoMap"));
            break;
        case 2:
            // marbleTextures[CURRENT_SKIN]->bind(programs.pbr->getUniform("albedoMap"));
            break;
        }
    }
    //=================================================
    // Sounds
    //=================================================

    //=================================================
    // GEOMETRY
    //=================================================
    void initGeom()
    {
        loadModels();
        loadLevel();
        initEffects();
        initGameObjects();
    }

    void loadModel(shared_ptr<Shape> &shape, string file, bool resize = false, bool findEdges = false)
    {
        shape = make_shared<Shape>();
        shape->loadMesh(RESOURCE_DIRECTORY + "/models/" + file);
        if (resize)
            shape->resize();
        shape->measure();
        if (findEdges)
            shape->findEdges();
        shape->init();
    }

    void loadModels()
    {
        initQuad();

        loadModel(shapes.cube, "cube.obj", true);
        loadModel(shapes.boxModel, "box.obj", false, true);
        loadModel(shapes.plane, "plane.obj");
        loadModel(shapes.roboHead, "Robot/RobotHead.obj", true);
        loadModel(shapes.roboLeg, "Robot/RobotLeg.obj", true);
        loadModel(shapes.roboFoot, "Robot/RobotFoot.obj", true);
        loadModel(shapes.billboard, "billboard.obj", true);
        loadModel(shapes.goalModel, "goal.obj", true);
        loadModel(shapes.sphere, "quadSphere.obj", true);
    }

    void initEffects()
    {
        sparkEmitter = make_shared<ParticleEmitter>(100);
        sparkEmitter->init(shapes.billboard, textures.spark);
        fireworkEmitter = make_shared<ParticleEmitter>(100);
        fireworkEmitter->init(shapes.billboard, textures.firework);
    }

    void initGameObjects()
    {
        gameObjects.ball = make_shared<Ball>(START_POSITION, quat(1, 0, 0, 0), shapes.sphere, 1);
        gameObjects.ball->init(windowManager, sparkEmitter);
        // Control points for enemy's bezier curve path
        vector<glm::vec3> enemyPath = {
            vec3{95.0, 2.0, 7.0},
            vec3{100.0, 2.0, 15.0},
            vec3{110.0, 2.0, -1.0},
            vec3{115.0, 2.0, 7.0}};
        gameObjects.enemy1 = make_shared<Enemy>(enemyPath, quat(1, 0, 0, 0), shapes.roboHead, shapes.roboLeg, shapes.roboFoot, 1);
        gameObjects.enemy1->init(windowManager);
        enemyPath = {
            vec3{125.0, 8.0, 55.0},
            vec3{115.0, 20.0, 55.0},
            vec3{105.0, 5.0, 55.0},
            vec3{95.0, 8.0, 55.0}};
        gameObjects.enemy2 = make_shared<Enemy>(enemyPath, quat(1, 0, 0, 0), shapes.roboHead, shapes.roboLeg, shapes.roboFoot, 1);
        gameObjects.enemy2->init(windowManager);

        gameObjects.goalObject = make_shared<Box>(vec3(0, 11.5, 0), quat(1, 0, 0, 0), shapes.goalModel);
        gameObjects.goalObject->scale = vec3(4);

        gameObjects.goal = make_shared<Goal>(gameObjects.goalObject->position + vec3(0, 1, 0), quat(1, 0, 0, 0), nullptr, 1);
        gameObjects.goal->init(fireworkEmitter, &START_TIME);

        // Need to add each physics object to the octree
        gameObjects.octree = make_shared<Octree>(vec3(-200, -210, -200), vec3(200, 190, 200));
        gameObjects.octree->init(shapes.billboard, shapes.cube);
        gameObjects.octree->insert(gameObjects.goal);
        gameObjects.octree->insert(gameObjects.goalObject);
        gameObjects.octree->insert(gameObjects.ball);
        gameObjects.octree->insert(boxes);
        gameObjects.octree->insert(gameObjects.enemy1);
    }

    void loadLevel()
    {
        ifstream inLevel(RESOURCE_DIRECTORY + "/levels/Level1.txt");

        float xval, yval, zval;
        while (inLevel >> xval)
        {
            inLevel >> yval >> zval;
            auto box = make_shared<Box>(vec3(xval * 8, yval, zval * 6), normalize(quat(0, 0, 0, 0)), shapes.boxModel);
            boxes.push_back(box);
        }
    }

    void initQuad()
    {
        /* set up a quad for rendering a framebuffer */

        // now set up a simple quad for rendering FBO
        glGenVertexArrays(1, &quad_VertexArrayID);
        glBindVertexArray(quad_VertexArrayID);

        static const GLfloat g_quad_vertex_buffer_data[] = {
            -1.0f,
            -1.0f,
            0.0f,
            1.0f,
            -1.0f,
            0.0f,
            -1.0f,
            1.0f,
            0.0f,

            -1.0f,
            1.0f,
            0.0f,
            1.0f,
            -1.0f,
            0.0f,
            1.0f,
            1.0f,
            0.0f,
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

        if (SHADOW_QUALITY)
        {
            createShadowMap(&LS);
        }

        glViewport(0, 0, width, height); // frame width and height
        // Clear framebuffer.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (DEBUG_LIGHT)
        {
            drawDepthMap();
        }
        else
        {
            renderPlayerView(&LS);
        }
    }

    void drawScene(shared_ptr<Program> shader)
    {
        // Draw textured models
        if (shader == programs.pbr)
        {
            glUniform1f(shader->getUniform("shadowSize"), (float)SHADOW_SIZE);
            glUniform1f(shader->getUniform("shadowAA"), (float)SHADOW_AA);
            glUniform1i(shader->getUniform("shadows"), SHADOWS);
        }

        // Create the matrix stacks
        auto M = make_shared<MatrixStack>();

        // Model identity
        M->pushMatrix();
        M->loadIdentity();

        if (shader == programs.pbr)
        {
            glUniform3fv(shader->getUniform("viewPos"), 1, value_ptr(camera->eye));
        }

        // Draw plane
        if (shader == programs.pbr)
        {
            setTextureMaterial(0);
        }
        M->pushMatrix();
        glUniformMatrix4fv(shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
        shapes.plane->draw(shader);
        M->popMatrix();

        // Draw ball
        if (shader == programs.pbr)
        {
            setTextureMaterial(1);
        }
        gameObjects.ball->draw(shader, M);
        gameObjects.goalObject->draw(shader, M);
        gameObjects.enemy1->draw(shader, M);
        gameObjects.enemy2->draw(shader, M);

        // Draw Boxes
        if (shader == programs.pbr)
        {
            setTextureMaterial(2);
        }
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

        textures.skybox->bind(programs.sky->getUniform("Texture0"));
        glDepthMask(GL_FALSE);
        glDisable(GL_CULL_FACE);
        shapes.cube->draw(programs.sky);
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

        if (gameObjects.octree->debug)
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
        gameObjects.octree->drawDebugBoundingSpheres(programs.circle);
        programs.circle->unbind();

        programs.cubeOutline->bind();
        setProjectionMatrix(programs.cubeOutline);
        setView(programs.cubeOutline);
        gameObjects.octree->drawDebugOctants(programs.cubeOutline);
        programs.cubeOutline->unbind();
    }

    //=================================================
    // GENERAL HELPERS

    void resetPlayer()
    {
        soundEngine->reset();

        gameObjects.ball->position = START_POSITION;
        gameObjects.ball->velocity = vec3(0);
        START_TIME = glfwGetTime();
        gameObjects.goal->reset();
    }

    void update(float dt)
    {
        gameObjects.octree->update();

        if (gameObjects.ball->position.y < -25.0)
        {
            resetPlayer();
        }

        auto boxesToCheck = gameObjects.octree->query(gameObjects.ball);
        for (auto box : boxesToCheck)
        {
            box->checkCollision(gameObjects.ball.get());
        }

        for (auto box : boxes)
        {
            box->update(dt);
        }
        //TODO:: Do Collision checks between ball and Enemy
        /*
        auto enemiesToCheck = gameObjects.octree->query(ball);
        for (auto enemies : enemiesToCheck) {
            
        }*/

        gameObjects.goalObject->update(dt);
        gameObjects.ball->update(dt, camera->getDolly(), camera->getStrafe());
        camera->update(dt, gameObjects.ball);
        gameObjects.goal->update(dt);
        gameObjects.enemy1->update(dt);
        gameObjects.enemy2->update(dt);

        sparkEmitter->update(dt);
        fireworkEmitter->update(dt);

        viewFrustum.extractPlanes(setProjectionMatrix(nullptr), setView(nullptr));
        gameObjects.octree->markInView(viewFrustum);
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
            CURRENT_SKIN = (CURRENT_SKIN + 1) % NUMBER_OF_MARBLE_SKINS;
        }
        else if (key == GLFW_KEY_I && action == GLFW_PRESS)
        {
            // musicSounds[0]->setIsPaused(GL_FALSE);
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
            camera->flying = !camera->flying;

            SHOW_CURSOR = !SHOW_CURSOR;
            if (SHOW_CURSOR)
                glfwSetInputMode(windowManager->getHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            else
                glfwSetInputMode(windowManager->getHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else if (key == GLFW_KEY_U && action == GLFW_PRESS)
        {
            DEBUG_LIGHT = !DEBUG_LIGHT;
        }
        else if (key == GLFW_KEY_H && action == GLFW_PRESS)
        {
            gameObjects.octree->debug = !gameObjects.octree->debug;
        }
        else if (key == GLFW_KEY_R && action == GLFW_PRESS)
        {
            resetPlayer();
        }
        else if (key == GLFW_KEY_C && action == GLFW_PRESS)
        {
            camera->previewLvl = !camera->previewLvl;
            if (camera->previewLvl)
            {
                camera->startLvlPreview(CENTER_LVL_POSITION);
            }
        }
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
            cout << "" << gameObjects.ball->position.x << ", " << gameObjects.ball->position.y << ", " << gameObjects.ball->position.z << endl;
            MOVING = true;
        }

        if (action == GLFW_RELEASE)
        {
            MOVING = false;
            MOUSE_DOWN = false;
        }

        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        {
            camera->angleLocked = false;
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        {
            camera->angleLocked = true;
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
    windowManager->init(1280, 720);
    // windowManager->init(1920, 1080);
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
