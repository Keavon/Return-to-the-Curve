#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <glad/glad.h>
#include <cmath>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <glad/glad.h>
#include <iomanip>
#include <iostream>
#include <string>

#include "Camera.h"
#include "GLSL.h"
#include "GLTextureWriter.h"
#include "MatrixStack.h"
#include "Object3D.h"
#include "Program.h"
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

// value_ptr for glm
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// number of skin textures to load and swap through
#define NUMBER_OF_MARBLE_SKINS 22

using namespace std;
using namespace glm;

extern bool ballInGoal;

class Application : public EventCallbacks
{

public:
    WindowManager *windowManager = nullptr;

    int score = 0;

    // Game info
    float startTime;
    vec3 startPos = vec3(120, 3, 7);
    vec3 centerOfLevel = vec3(70, 3, 40);
    bool didWin = false;

    // Constants
    float SPAWN_RATE = 5;

    int SHADOWS = 1;
    int DEBUG_LIGHT = 0;
    int GEOM_DEBUG = 1;

    float timer = 0;
    float printTimer = 0;

    // for shadows
    GLuint depthMapFBO;
    const GLuint S_WIDTH = 2048, S_HEIGHT = 2048;
    GLuint depthMap;

    // light position for shadows
    vec3 g_light = vec3(80, 40, -20);

    shared_ptr<Camera> camera;

    // Shader programs
    shared_ptr<Program> texProg;
    shared_ptr<Program> skyProg;
    shared_ptr<Program> circleProg;
    shared_ptr<Program> cubeOutlineProg;

    shared_ptr<Program> DepthProg;
    shared_ptr<Program> DepthProgDebug;
    shared_ptr<Program> DebugProg;

    // Shapes
    shared_ptr<Shape> cube;
    shared_ptr<Shape> roboHead;
    shared_ptr<Shape> roboLeg;
    shared_ptr<Shape> roboFoot;
    shared_ptr<Shape> boxModel;
    shared_ptr<Shape> plane;
    shared_ptr<Shape> bunny;
    shared_ptr<Shape> billboard;
    shared_ptr<Shape> goalModel;
    shared_ptr<Shape> sphere;

    vector<shared_ptr<Object3D>> bunnies;
    vector<shared_ptr<PhysicsObject>> boxes;

    // Game objects
    shared_ptr<Ball> ball;
    shared_ptr<Enemy> enemy;
    shared_ptr<Enemy> enemy2;
    shared_ptr<Goal> goal;
    shared_ptr<Box> goalObject;

    shared_ptr<Octree> octree;

    // geometry for texture render
    GLuint quad_VertexArrayID;
    GLuint quad_vertexbuffer;

    // Textures
    shared_ptr<Skybox> skyboxTexture;
    shared_ptr<Texture> grassTexture;
    shared_ptr<Texture> brickTexture;
    shared_ptr<Texture> crateTexture;

    vector<shared_ptr<Texture>> marbleTextures;

    // Contains vertex information for OpenGL
    GLuint VertexArrayID;

    // Data necessary to give our triangle to OpenGL
    GLuint VertexBufferID;

    bool FirstTime = true;
    bool Moving = false;
    int gMat = 0;

    // used to track current player skin
    int CURRENT_SKIN = 0;

    float cTheta;
    bool mouseDown = false;
    bool showCursor = false;

    void init(const string &resourceDirectory)
    {
        int width, height;
        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
        GLSL::checkVersion();

        cTheta = 0;
        // Set background color.
        glClearColor(.12f, .34f, .56f, 1.0f);
        // Enable z-buffer test.
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        // Initialize camera
        camera = make_shared<Camera>(windowManager);
        camera->init();

        ballInGoal = false;
    }

    //=================================================
    // SHADERS
    void initShaders(const string &resourceDirectory)
    {
        initShader_skyProg(resourceDirectory);
        initShader_texProg(resourceDirectory);

        initShader_circleProg(resourceDirectory);
        initShader_cubeOutlineProg(resourceDirectory);

        initShader_DepthProg(resourceDirectory);
        initShader_DebugProg(resourceDirectory);
        initShader_DepthProgDebug(resourceDirectory);
    }

    void initShader_DepthProg(const string &resourceDirectory)
    {
        DepthProg = make_shared<Program>();
        DepthProg->setVerbose(true);
        DepthProg->setShaderNames(resourceDirectory + "/shaders/depth_vert.glsl",
                                  resourceDirectory + "/shaders/depth_frag.glsl");
        if (!DepthProg->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!"
                      << std::endl;
            exit(1);
        }

        DepthProg->addUniform("LP");
        DepthProg->addUniform("LV");
        DepthProg->addUniform("M");
        DepthProg->addAttribute("vertPos");
        // un-needed, but easier then modifying shape
        DepthProg->addAttribute("vertNor");
        DepthProg->addAttribute("vertTex");
    }

    void initShader_DepthProgDebug(const string &resourceDirectory)
    {
        DepthProgDebug = make_shared<Program>();
        DepthProgDebug->setVerbose(true);
        DepthProgDebug->setShaderNames(
            resourceDirectory + "/shaders/depth_vertDebug.glsl",
            resourceDirectory + "/shaders/depth_fragDebug.glsl");
        if (!DepthProgDebug->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!"
                      << std::endl;
            exit(1);
        }

        DepthProgDebug->addUniform("LP");
        DepthProgDebug->addUniform("LV");
        DepthProgDebug->addUniform("M");
        DepthProgDebug->addAttribute("vertPos");
        // un-needed, but easier then modifying shape
        DepthProgDebug->addAttribute("vertNor");
        DepthProgDebug->addAttribute("vertTex");
    }

    void initShader_DebugProg(const string &resourceDirectory)
    {
        DebugProg = make_shared<Program>();
        DebugProg->setVerbose(true);
        DebugProg->setShaderNames(resourceDirectory + "/shaders/pass_vert.glsl",
                                  resourceDirectory + "/shaders/pass_texfrag.glsl");
        if (!DebugProg->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!"
                      << std::endl;
            exit(1);
        }

        DebugProg->addUniform("texBuf");
        DebugProg->addAttribute("vertPos");
    }

    void initShader_texProg(const string &resourceDirectory)
    {
        // Shader for textured models
        texProg = make_shared<Program>();
        texProg->setVerbose(true);
        texProg->setShaderNames(resourceDirectory + "/shaders/tex.vert.glsl",
                                resourceDirectory + "/shaders/tex.frag.glsl");
        if (!texProg->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!"
                      << std::endl;
            exit(1);
        }

        texProg->addUniform("P");
        texProg->addUniform("V");
        texProg->addUniform("M");
        texProg->addUniform("MatAmb");
        texProg->addUniform("Texture0");
        texProg->addUniform("MatSpec");
        texProg->addUniform("Shine");
        texProg->addUniform("lightPos");
        texProg->addUniform("dirLightColor");
        texProg->addUniform("viewPos");
        texProg->addAttribute("vertPos");
        texProg->addAttribute("vertNor");
        texProg->addAttribute("vertTex");

        texProg->addUniform("shadowDepth");
        texProg->addUniform("LS");
        // texProg->addUniform("lightDir");
    }

    void initShader_skyProg(const string &resourceDirectory)
    {
        // Shader for skybox
        skyProg = make_shared<Program>();
        skyProg->setVerbose(true);
        skyProg->setShaderNames(
            resourceDirectory + "/shaders/sky_vert.glsl",
            resourceDirectory + "/shaders/sky_frag.glsl");
        if (!skyProg->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        skyProg->addUniform("P");
        skyProg->addUniform("V");
        skyProg->addUniform("Texture0");
        skyProg->addAttribute("vertPos");
    }

    void initShader_circleProg(const string &resourceDirectory)
    {
        // Shader for debug circle
        circleProg = make_shared<Program>();
        circleProg->setVerbose(true);
        circleProg->setShaderNames(
            resourceDirectory + "/shaders/circle_vert.glsl",
            resourceDirectory + "/shaders/circle_frag.glsl");
        if (!circleProg->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        circleProg->addUniform("P");
        circleProg->addUniform("V");
        circleProg->addUniform("M");
        circleProg->addUniform("radius");
        circleProg->addAttribute("vertPos");
    }

    void initShader_cubeOutlineProg(const string &resourceDirectory)
    {
        // Shader for debug cube
        cubeOutlineProg = make_shared<Program>();
        cubeOutlineProg->setVerbose(true);
        cubeOutlineProg->setShaderNames(
            resourceDirectory + "/shaders/cube_outline_vert.glsl",
            resourceDirectory + "/shaders/cube_outline_frag.glsl");
        if (!cubeOutlineProg->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        cubeOutlineProg->addUniform("P");
        cubeOutlineProg->addUniform("V");
        cubeOutlineProg->addUniform("M");
        cubeOutlineProg->addUniform("edge");
        cubeOutlineProg->addAttribute("vertPos");
    }
    //=================================================
    //=================================================
    // TEXTURES
    void initTextures(const string &resourceDirectory)
    {
        initCrateTexture(resourceDirectory);
        initMarbleTexture(resourceDirectory);
        initSkyBox(resourceDirectory);
        initShadow();
    }

    void initCrateTexture(const string &resourceDirectory)
    {

        crateTexture = make_shared<Texture>();
        crateTexture->setFilename(resourceDirectory + "/textures/crate.png");
        crateTexture->init();
        crateTexture->setUnit(1);
        crateTexture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    }

    void initMarbleTexture(const string &resourceDirectory)
    {
        // loops over the number of skin textures, initializing them and adding them to a vector
        string textureBaseFolder, textureNumber, textureExtension, textureName;
        double completion = 0;
        for (int i = 0; i < NUMBER_OF_MARBLE_SKINS; i++)
        {
            textureBaseFolder = "/textures/marble/";
            textureNumber = to_string(i);
            textureExtension = ".jpg";

            textureName = textureBaseFolder + textureNumber + textureExtension;
            completion = ((float)i * 100 / (float)NUMBER_OF_MARBLE_SKINS);

            cout << std::setprecision(2) << "Loading Textures: " << completion << "\% complete." << endl;

            shared_ptr<Texture> marbleTexture = make_shared<Texture>();
            marbleTexture->setFilename(resourceDirectory + textureName);
            marbleTexture->init();
            marbleTexture->setUnit(1);
            marbleTexture->setWrapModes(GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT);

            marbleTextures.push_back(marbleTexture);
        }
        cout << "Loading Textures: complete." << endl;
    }

    void initSkyBox(const string &resourceDirectory)
    {
        // Load skybox
        string skyboxFilenames[] = {"sea_ft.JPG", "sea_bk.JPG", "sea_up.JPG",
                                    "sea_dn.JPG", "sea_rt.JPG", "sea_lf.JPG"};
        for (int i = 0; i < 6; i++)
        {
            skyboxFilenames[i] = resourceDirectory + "/skybox/" + skyboxFilenames[i];
        }
        skyboxTexture = make_shared<Skybox>();
        skyboxTexture->setFilenames(skyboxFilenames);
        skyboxTexture->init();
        skyboxTexture->setUnit(1);
        skyboxTexture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    }

    void initShadow()
    {
        /* set up the FBO for the light's depth */
        // generate the FBO for the shadow depth
        glGenFramebuffers(1, &depthMapFBO);

        // generate the texture
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, S_WIDTH, S_HEIGHT, 0,
                     GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // bind with framebuffer's depth buffer
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                               depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void setTextureMaterial(int i)
    {
        switch (i)
        {
            // pulled these out of the switch since they were all identical
            glUniform3f(texProg->getUniform("MatSpec"), 0.2f, 0.2f, 0.2f);
            glUniform3f(texProg->getUniform("MatAmb"), 0.05f, 0.05f, 0.05f);
            glUniform1f(texProg->getUniform("Shine"), 32);

        case 0:
            marbleTextures[CURRENT_SKIN]->bind(texProg->getUniform("Texture0"));
            break;
        case 1:
            marbleTextures[CURRENT_SKIN]->bind(texProg->getUniform("Texture0"));
            break;
        case 2:
            crateTexture->bind(texProg->getUniform("Texture0"));
            break;
        }
    }
    //=================================================
    //=================================================
    // GEOMETRY
    void initGeom(const string &resourceDirectory)
    {

        loadModels(resourceDirectory);

        loadLevel(resourceDirectory);

        initGameObjects();
    }

    void loadModels(const string &resourceDirectory)
    {
        initQuad();

        //TODO:: update cube to use texture Coords
        cube = make_shared<Shape>();
        cube->loadMesh(resourceDirectory + "/models/cube.obj");
        cube->resize();
        cube->measure();
        cube->init();

        boxModel = make_shared<Shape>();
        boxModel->loadMesh(resourceDirectory + "/models/box.obj");
        boxModel->measure();
        boxModel->findEdges();
        boxModel->init();

        plane = make_shared<Shape>();
        plane->loadMesh(resourceDirectory + "/models/plane.obj");
        plane->measure();
        plane->init();

        roboHead = make_shared<Shape>();
        roboHead->loadMesh(resourceDirectory + "/models/Robot/RobotHead.obj");
        roboHead->resize();
        roboHead->measure();
        roboHead->init();

        roboLeg = make_shared<Shape>();
        roboLeg->loadMesh(resourceDirectory + "/models/Robot/RobotLeg.obj");
        roboLeg->resize();
        roboLeg->measure();
        roboLeg->init();

        roboFoot = make_shared<Shape>();
        roboFoot->loadMesh(resourceDirectory + "/models/Robot/RobotFoot.obj");
        roboFoot->resize();
        roboFoot->measure();
        roboFoot->init();

        billboard = make_shared<Shape>();
        billboard->loadMesh(resourceDirectory + "/models/billboard.obj");
        billboard->resize();
        billboard->measure();
        billboard->init();

        goalModel = make_shared<Shape>();
        goalModel->loadMesh(resourceDirectory + "/models/goal.obj");
        goalModel->resize();
        goalModel->measure();
        goalModel->init();

        sphere = make_shared<Shape>();
        sphere->loadMesh(resourceDirectory + "/models/quadSphere.obj");
        sphere->resize();
        sphere->measure();
        sphere->init();
    }

    void initGameObjects()
    {
        ball = make_shared<Ball>(startPos, quat(1, 0, 0, 0), sphere, 1);
        ball->init(windowManager);
        // Control points for enemy's bezier curve path
        std::vector<glm::vec3> enemyPath = {
            vec3{95.0, 2.0, 7.0},
            vec3{100.0, 2.0, 15.0},
            vec3{110.0, 2.0, -1.0},
            vec3{115.0, 2.0, 7.0}
        };
        enemy = make_shared<Enemy>(enemyPath, quat(1, 0, 0, 0), roboHead, roboLeg, roboFoot, 1);
        enemy->init(windowManager);
        enemyPath = {
            vec3{125.0, 8.0, 55.0},
            vec3{115.0, 20.0, 55.0},
            vec3{105.0, 5.0, 55.0},
            vec3{95.0, 8.0, 55.0}
        };
        enemy2 = make_shared<Enemy>(enemyPath, quat(1, 0, 0, 0), roboHead, roboLeg, roboFoot, 1);
        enemy2->init(windowManager);

        goalObject = make_shared<Box>(vec3(0, 11.5, 0), quat(1, 0, 0, 0), goalModel);
        goalObject->scale = vec3(4);

        goal = make_shared<Goal>(goalObject->position + vec3(0, 1, 0), quat(1, 0, 0, 0), nullptr, 1);

        // Need to add each physics object to the octree
        octree = make_shared<Octree>(vec3(-200, -210, -200), vec3(200, 190, 200));
        octree->init(billboard, cube);
        octree->queue(goal);
        octree->queue(goalObject);
        octree->queue(ball);
        octree->queue(boxes);
        octree->queue(enemy);
        octree->queue(enemy2);
    }

    void loadLevel(const string &resourceDirectory)
    {
        ifstream inLevel(resourceDirectory + "/levels/Level1.txt");

        float xval, yval, zval;
        while (inLevel >> xval)
        {
            inLevel >> yval >> zval;
            auto box = make_shared<Box>(vec3(xval * 8, yval, zval * 6), normalize(quat(0, 0, 0, 0)), boxModel);
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
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data),
                     g_quad_vertex_buffer_data, GL_STATIC_DRAW);
    }
    //=================================================
    //=================================================
    // RENDERERING

    void render()
    {
        // Get current frame buffer size.
        int width, height;
        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);

        mat4 LS;

        if (SHADOWS)
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

    void drawScene(shared_ptr<Program> shader, GLint texID, int TexOn)
    {
        // Draw textured models

        // Create the matrix stacks
        auto M = make_shared<MatrixStack>();

        // Model Identity
        M->pushMatrix();
        M->loadIdentity();

        if (shader == texProg)
        {
            glUniform3fv(shader->getUniform("viewPos"), 1, value_ptr(camera->eye));
        }
        // =================================================================================================

        // Draw plane
        if (shader == texProg)
        {
            setTextureMaterial(0);
        }
        M->pushMatrix();
        glUniformMatrix4fv(shader->getUniform("M"), 1, GL_FALSE,
                           value_ptr(M->topMatrix()));
        plane->draw(shader);
        M->popMatrix();
        // =================================================================================================

        // Draw ball
        if (shader == texProg)
        {
            setTextureMaterial(1);
        }
        ball->draw(shader, M);
        goalObject->draw(shader, M);
        enemy->draw(shader, M);
        enemy2->draw(shader, M);
        // =================================================================================================

        // Draw Boxes
        if (shader == texProg)
        {
            setTextureMaterial(2);
        }
        for (auto box : boxes)
        {
            box->draw(shader, M);
        }
        // =================================================================================================

        // cleanup
        M->popMatrix();
        // =================================================================================================
    }

    void createShadowMap(mat4 *LS)
    {
        // set up light's depth map
        glViewport(0, 0, S_WIDTH, S_HEIGHT); // shadow map width and height
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);

        // set up shadow shader
        // render scene
        DepthProg->bind();
        // TODO you will need to fix these
        mat4 LP = SetOrthoMatrix(DepthProg);
        mat4 LV = SetLightView(DepthProg, g_light, vec3(60, 0, 0), vec3(0, 1, 0));
        *LS = LP * LV;
        // SetLightView(DepthProg, g_light, g_lookAt, vec3(0, 1, 0));
        drawScene(DepthProg, 0, 0);
        DepthProg->unbind();
        glCullFace(GL_BACK);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void drawDepthMap()
    {
        /* code to draw the light depth buffer */
        // geometry style debug on light - test transforms, draw geometry from
        // light perspective
        if (GEOM_DEBUG)
        {
            DepthProgDebug->bind();
            // render scene from light's point of view
            SetOrthoMatrix(DepthProgDebug);
            SetLightView(DepthProgDebug, g_light, vec3(60, 0, 0), vec3(0, 1, 0));
            drawScene(DepthProgDebug, texProg->getUniform("Texture0"), 0);
            DepthProgDebug->unbind();
        }
        else
        {
            // actually draw the light depth map
            DebugProg->bind();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            glUniform1i(DebugProg->getUniform("texBuf"), 0);
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glDisableVertexAttribArray(0);
            DebugProg->unbind();
        }
    }

    void drawSkyBox()
    {
        // Draw skybox
        skyProg->bind();
        setProjectionMatrix(skyProg);
        setView(skyProg);

        skyboxTexture->bind(skyProg->getUniform("Texture0"));
        glDepthMask(GL_FALSE);
        glDisable(GL_CULL_FACE);
        cube->draw(skyProg);
        glEnable(GL_CULL_FACE);
        glDepthMask(GL_TRUE);

        skyProg->unbind();
    }

    void sendShadowMap()
    {
        /* also set up light depth map */
        glActiveTexture(GL_TEXTURE30);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glUniform1i(texProg->getUniform("shadowDepth"), 0);
    }

    void renderPlayerView(mat4 *LS)
    {
        drawSkyBox();

        texProg->bind();

        setLight(texProg);
        setProjectionMatrix(texProg);
        setView(texProg);

        sendShadowMap();

        glUniformMatrix4fv(texProg->getUniform("LS"), 1, GL_FALSE, value_ptr(*LS));
        drawScene(texProg, texProg->getUniform("Texture0"), 1);

        texProg->unbind();

        if (octree->debug)
        {
            drawOctree();
        }
    }

    void drawOctree()
    {
        circleProg->bind();
        setProjectionMatrix(circleProg);
        setView(circleProg);
        octree->drawDebugBoundingSpheres(circleProg);
        circleProg->unbind();

        cubeOutlineProg->bind();
        setProjectionMatrix(cubeOutlineProg);
        setView(cubeOutlineProg);
        octree->drawDebugOctants(cubeOutlineProg);
        cubeOutlineProg->unbind();
    }
    //=================================================
    //=================================================
    // GENERAL HELPERS

    void update(float dt)
    {
        octree->clear();
        octree->build();

        if (ballInGoal && !didWin)
        {
            didWin = true;
            cout << "✼　 ҉ 　✼　 ҉ 　✼" << endl;
            cout << "You win!" << endl;
            cout << "Time: " << glfwGetTime() - startTime << endl;
            cout << "✼　 ҉ 　✼　 ҉ 　✼" << endl;
        }
        auto boxesToCheck = octree->query(ball);
        for (auto box : boxesToCheck)
        {
            box->checkCollision(ball.get());
        }

        for (auto box : boxes)
        {
            box->update(dt);
        }
        //TODO:: Do Collision checks between ball and Enemy
        /*
        auto enemiesToCheck = octree->query(ball);
        for (auto enemies : enemiesToCheck) {
            
        }*/
        
        goalObject->update(dt);
        ball->update(dt, camera->getDolly(), camera->getStrafe());
        camera->update(dt, ball);
        goal->update(dt);
        enemy->update(dt);
        enemy2->update(dt);
    }

    void setLight(shared_ptr<Program> prog)
    {
        glUniform3f(prog->getUniform("lightPos"), 0, 1, 1);
        glUniform3f(prog->getUniform("dirLightColor"), 1, 1, 1);
    }

    mat4 SetOrthoMatrix(shared_ptr<Program> curShade)
    {
        // shadow mapping helper

        mat4 ortho = glm::ortho(-96.0f, 96.0f, -96.0f, 96.0f, 0.1f, 128.0f);
        // fill in the glUniform call to send to the right shader!
        glUniformMatrix4fv(curShade->getUniform("LP"), 1, GL_FALSE,
                           value_ptr(ortho));
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

    void setProjectionMatrix(shared_ptr<Program> curShade)
    {
        int width, height;
        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
        float aspect = width / (float)height;
        mat4 Projection = perspective(radians(50.0f), aspect, 0.1f, 200.0f);
        glUniformMatrix4fv(curShade->getUniform("P"), 1, GL_FALSE,
                           value_ptr(Projection));
    }

    void setView(shared_ptr<Program> curShade)
    {
        mat4 Cam = lookAt(camera->eye, camera->lookAtPoint, camera->upVec);
        glUniformMatrix4fv(curShade->getUniform("V"), 1, GL_FALSE, value_ptr(Cam));
    }

    void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        //skin switching key call back
        if (key == GLFW_KEY_O && action == GLFW_PRESS)
        {
            CURRENT_SKIN = (CURRENT_SKIN + 1) % NUMBER_OF_MARBLE_SKINS;
        }
        else if (key == GLFW_KEY_Y && action == GLFW_PRESS)
        {
            SHADOWS = !SHADOWS;
        }

        // other call backs
        else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            glfwSetInputMode(windowManager->getHandle(), GLFW_CURSOR,
                             GLFW_CURSOR_NORMAL);
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        else if (key == GLFW_KEY_M && action == GLFW_PRESS)
        {
            gMat = (gMat + 1) % 4;
        }
        else if (key == GLFW_KEY_Z && action == GLFW_PRESS)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else if (key == GLFW_KEY_Z && action == GLFW_RELEASE)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else if (key == GLFW_KEY_V && action == GLFW_PRESS)
        {
            camera->flying = !camera->flying;
        }
        else if (key == GLFW_KEY_U && action == GLFW_PRESS)
        {
            DEBUG_LIGHT = !DEBUG_LIGHT;
        }
        else if (key == GLFW_KEY_P && action == GLFW_PRESS)
        {
            showCursor = !showCursor;
            if (showCursor)
            {
                glfwSetInputMode(windowManager->getHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
            else
            {
                glfwSetInputMode(windowManager->getHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
        }
        else if (key == GLFW_KEY_H && action == GLFW_PRESS)
        {
            octree->debug = !octree->debug;
        }
        else if (key == GLFW_KEY_R && action == GLFW_PRESS)
        {
            ball->position = startPos;
            ball->velocity = vec3(0);
            didWin = false;
            ballInGoal = false;
            startTime = glfwGetTime();
        }
        else if (key == GLFW_KEY_C && action == GLFW_PRESS) {
            camera->previewLvl = !camera->previewLvl;
            if (camera->previewLvl){
                camera->startLvlPreview(centerOfLevel);
            }
        }
    }

    void scrollCallback(GLFWwindow *window, double deltaX, double deltaY)
    {
        cTheta += (float)deltaX / 10;
    }

    void mouseCallback(GLFWwindow *window, int button, int action, int mods)
    {
        double posX, posY;

        if (action == GLFW_PRESS)
        {
            mouseDown = true;
            glfwGetCursorPos(window, &posX, &posY);
            cout << "Pos X " << posX << " Pos Y " << posY << endl;
            cout << "" << ball->position.x << ", " << ball->position.y << ", " << ball->position.z << endl;
            Moving = true;
        }

        if (action == GLFW_RELEASE)
        {
            Moving = false;
            mouseDown = false;
        }
    }

    void resizeCallback(GLFWwindow *window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }
    //=================================================
};

int main(int argc, char **argv)
{
    // Where the resources are loaded from
    std::string resourceDir = "../resources";

    if (argc >= 2)
    {
        resourceDir = argv[1];
    }

    Application *application = new Application();

    // Your main will always include a similar set up to establish your window
    // and GL context, etc.

    WindowManager *windowManager = new WindowManager();
    windowManager->init(1280, 720);
    windowManager->setEventCallbacks(application);
    application->windowManager = windowManager;

    // This is the code that will likely change program to program as you
    // may need to initialize or set up different data and state

    application->init(resourceDir);
    application->initShaders(resourceDir);
    application->initTextures(resourceDir);
    application->initGeom(resourceDir);

    double prevTime = glfwGetTime();
    application->startTime = glfwGetTime();

    // Loop until the user closes the window.
    while (!glfwWindowShouldClose(windowManager->getHandle()))
    {
        // Render scene.
        double t = glfwGetTime();
        double dt = std::min(t - prevTime, 0.1);
        prevTime = t;
        application->render();
        application->update(dt);

        // Swap front and back buffers.
        glfwSwapBuffers(windowManager->getHandle());
        // Poll for and process events.
        glfwPollEvents();
    }

    // Quit program.
    windowManager->shutdown();
    return 0;
}
