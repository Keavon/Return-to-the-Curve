/*
CSC 476 Lab 1
*/

#include <iostream>
#include <iomanip>
#include <glad/glad.h>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <string>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"
#include "Skybox.h"
#include "WindowManager.h"
#include "GLTextureWriter.h"
#include "Object3D.h"
#include "Camera.h"
#include "engine/GameObject.h"
#include "gameobjects/Ball.h"
#include "gameobjects/Box.h"
#include "gameobjects/Goal.h"
#include "gameobjects/Enemy.h"
#include "engine/ColliderSphere.h"
#include "engine/Collider.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

//number of skin textures to load and swap through
#define NUMBER_OF_MARBLE_SKINS 55

using namespace std;
using namespace glm;

extern bool ballInGoal;

class Application : public EventCallbacks
{

  public:
	WindowManager *windowManager = nullptr;

	int score = 0;

	// Constants
	float SPAWN_RATE = 5;

	float timer = 0;
	float printTimer = 0;

	shared_ptr<Camera> camera;

	// Shader programs
	shared_ptr<Program> texProg;
	shared_ptr<Program> matProg;
	shared_ptr<Program> skyProg;

	// Shapes
	shared_ptr<Shape> cube;
	shared_ptr<Shape> boxModel;
	shared_ptr<Shape> plane;
	shared_ptr<Shape> bunny;
	vector<shared_ptr<Object3D>> bunnies;
	vector<shared_ptr<PhysicsObject>> boxes;

	// Game objects
	shared_ptr<Ball> ball;
	shared_ptr<Enemy> enemy;

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

	//used to track current player skin
	int CURRENT_SKIN = 0;

	float cTheta;
	bool mouseDown = false;
	bool showCursor = false;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		//skin switching key call back
		if (key == GLFW_KEY_O && action == GLFW_PRESS)
		{
			CURRENT_SKIN = (CURRENT_SKIN + 1) % NUMBER_OF_MARBLE_SKINS;
		}

		//other call backs
		else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetInputMode(windowManager->getHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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

	void initShaders(const string &resourceDirectory)
	{
		// Shader for textured models
		texProg = make_shared<Program>();
		texProg->setVerbose(true);
		texProg->setShaderNames(
			resourceDirectory + "/shaders/tex_vert.glsl",
			resourceDirectory + "/shaders/tex_frag.glsl");
		if (!texProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		texProg->addUniform("P");
		texProg->addUniform("V");
		texProg->addUniform("M");
		texProg->addUniform("MatAmb");
		texProg->addUniform("Texture0");
		texProg->addUniform("MatSpec");
		texProg->addUniform("Shine");
		texProg->addUniform("dirLightDir");
		texProg->addUniform("dirLightColor");
		texProg->addUniform("viewPos");
		texProg->addAttribute("vertPos");
		texProg->addAttribute("vertNor");
		texProg->addAttribute("vertTex");

		// Shader for untextured models
		matProg = make_shared<Program>();
		matProg->setVerbose(true);
		matProg->setShaderNames(
			resourceDirectory + "/shaders/mat_vert.glsl",
			resourceDirectory + "/shaders/mat_frag.glsl");
		if (!matProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		matProg->addUniform("P");
		matProg->addUniform("V");
		matProg->addUniform("M");
		matProg->addUniform("MatAmb");
		matProg->addUniform("MatDif");
		matProg->addUniform("MatSpec");
		matProg->addUniform("Shine");
		matProg->addUniform("dirLightDir");
		matProg->addUniform("dirLightColor");
		matProg->addUniform("viewPos");
		matProg->addAttribute("vertPos");
		matProg->addAttribute("vertNor");

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

	void initTextures(const string &resourceDirectory)
	{
		grassTexture = make_shared<Texture>();
		grassTexture->setFilename(resourceDirectory + "/textures/grass-tiled.jpg");
		grassTexture->init();
		grassTexture->setUnit(1);
		grassTexture->setWrapModes(GL_REPEAT, GL_REPEAT);

		brickTexture = make_shared<Texture>();
		brickTexture->setFilename(resourceDirectory + "/textures/brick.jpg");
		brickTexture->init();
		brickTexture->setUnit(1);
		brickTexture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		crateTexture = make_shared<Texture>();
		crateTexture->setFilename(resourceDirectory + "/textures/crate.png");
		crateTexture->init();
		crateTexture->setUnit(1);
		crateTexture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

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

		// Load skybox
		string skyboxFilenames[] = {"sea_ft.JPG", "sea_bk.JPG", "sea_up.JPG", "sea_dn.JPG", "sea_rt.JPG", "sea_lf.JPG"};
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

	void initGeom(const string &resourceDirectory)
	{
		//TODO:: update cube to use texture Coords
		cube = make_shared<Shape>();
		cube->loadMesh(resourceDirectory + "/models/cube.obj");
		cube->resize();
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

		bunny = make_shared<Shape>();
		bunny->loadMesh(resourceDirectory + "/models/bunny.obj");
		bunny->resize();
		bunny->measure();
		bunny->init();

		auto sphere = make_shared<Shape>();
		sphere->loadMesh(resourceDirectory + "/models/quadSphere.obj");
		sphere->resize();
		sphere->init();

		ball = make_shared<Ball>(vec3(0, 3, -3), quat(1, 0, 0, 0), sphere, 1);
		ball->init(windowManager);

		for (int i = 0; i < 10; i++)
		{
			auto box = make_shared<Box>(vec3(6, 1 + 2.5 * i, -3 - 5 * i), normalize(quat(1, i % 2, i % 3, i % 4)), boxModel);
			boxes.push_back(box);
		}
		auto goal = make_shared<Goal>(vec3(-5, 5, 0), quat(1, 0, 0, 0), nullptr, 1);
		boxes.push_back(goal);
		auto planeObject = make_shared<Box>(vec3(0), quat(1, 0, 0, 0), plane);
		// planeObject->scale = vec3(10);
		boxes.push_back(planeObject);
		// auto box = make_shared<Box>(vec3(0), normalize(quat(1, 0, 0, 0.5)), boxModel);
		// boxes.push_back(box);
		// box = make_shared<Box>(vec3(0), normalize(quat(1, 0, 0, -0.5)), boxModel);
		// boxes.push_back(box);
	}

	void render(double dt)
	{
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* Leave this code to just draw the meshes alone */
		float aspect = width / (float)height;

		// Create the matrix stacks
		auto P = make_shared<MatrixStack>();
		auto M = make_shared<MatrixStack>();
		auto V = make_shared<MatrixStack>();
		// Apply perspective projection.
		P->pushMatrix();
		P->perspective(45.0f, aspect, 0.01f, 100.0f);

		V->loadIdentity();
		V->lookAt(camera->eye, camera->lookAtPoint, camera->upVec);

		M->pushMatrix();
		M->loadIdentity();

		// Draw skybox
		skyProg->bind();
		glUniformMatrix4fv(skyProg->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
		glUniformMatrix4fv(skyProg->getUniform("V"), 1, GL_FALSE, value_ptr(mat4(mat3(V->topMatrix()))));

		skyboxTexture->bind(skyProg->getUniform("Texture0"));
		glDepthMask(GL_FALSE);
		glDisable(GL_CULL_FACE);
		cube->draw(skyProg);
		glEnable(GL_CULL_FACE);
		glDepthMask(GL_TRUE);

		skyProg->unbind();

		matProg->bind();
		setLight(matProg);
		glUniformMatrix4fv(matProg->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
		glUniformMatrix4fv(matProg->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix()));
		glUniform3fv(matProg->getUniform("viewPos"), 1, value_ptr(camera->eye));

		// Draw bunnies
		for (auto bunny : bunnies)
		{
			setMaterial(bunny->material);
			M->pushMatrix();
			M->translate(bunny->position - vec3(0, bunny->model->min.y, 0));
			M->rotate(atan2(bunny->direction.x, bunny->direction.z) + M_PI_2, vec3(0, 1, 0));
			glUniformMatrix4fv(matProg->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
			bunny->draw(matProg);
			M->popMatrix();
		}

		matProg->unbind();

		// Draw textured models

		texProg->bind();
		setLight(texProg);
		glUniformMatrix4fv(texProg->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
		glUniformMatrix4fv(texProg->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix()));
		glUniform3fv(texProg->getUniform("viewPos"), 1, value_ptr(camera->eye));

		// Draw plane
		setTextureMaterial(0);
		M->pushMatrix();
		glUniformMatrix4fv(texProg->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
		plane->draw(texProg);
		M->popMatrix();

		// Draw ball
		setTextureMaterial(0);
		// enemy->draw(texProg, M);

		setTextureMaterial(1);
		ball->draw(texProg, M);

		setTextureMaterial(2);
		for (auto box : boxes)
		{
			box->draw(texProg, M);
		}

		texProg->unbind();

		M->popMatrix();

		P->popMatrix();
	}

	void printInfo(float dt)
	{
		cout << string(25, '-') << endl;
		cout << "FPS: " << 1 / dt << endl;
		cout << "Bunnies on the ground: " << bunnies.size() << endl;
		cout << "Bunnies touched: " << score << endl;
		cout << string(25, '-') << endl;
	}

	void update(float dt)
	{
		if (ballInGoal)
		{
			cout << "You win!" << endl;
		}
		for (auto box : boxes)
		{
			box->checkCollision(ball.get());
		}
		for (auto box : boxes)
		{
			box->update(dt);
		}
		ball->update(dt, camera->getDolly(), camera->getStrafe());
		camera->update(dt, ball);
		//TODO:: fix enemy's updating

		// Lab 1 stuff
		/*
		printTimer += dt;

		if (printTimer >= 1)
		{
			printTimer = 0;
			printInfo(dt);
		}

		timer += dt;

		if (timer >= SPAWN_RATE)
		{
			timer = 0;

			auto newBunny = make_shared<Object3D>();

			newBunny->position.x = fmod(rand(), plane->size.x) - plane->size.x / 2;
			newBunny->position.y = 0;
			newBunny->position.z = fmod(rand(), plane->size.z) - plane->size.z / 2;

			newBunny->direction.x = ((rand() % 200) / 100.0) - 1;
			newBunny->direction.y = 0;
			newBunny->direction.z = ((rand() % 200) / 100.0) - 1;
			newBunny->direction = normalize(newBunny->direction);

			newBunny->speed = rand() % 5 + 5;

			newBunny->model = bunny;
			newBunny->material = rand() % 4;
			newBunny->radius = 1;

			bunnies.push_back(newBunny);
		}


		for (auto bunny : bunnies)
		{
			bunny->update(dt);
		}

		// Collision
		for (int i = 0; i < bunnies.size(); i++)
		{
			if (!bunnies[i]->dead)
			{
				// Check walls
				if (bunnies[i]->position.x - bunnies[i]->radius < plane->min.x)
				{
					bunnies[i]->direction.x = abs(bunnies[i]->direction.x);
				}
				else if (bunnies[i]->position.x + bunnies[i]->radius > plane->max.x)
				{
					bunnies[i]->direction.x = -abs(bunnies[i]->direction.x);
				}
				if (bunnies[i]->position.z - bunnies[i]->radius < plane->min.z)
				{
					bunnies[i]->direction.z = abs(bunnies[i]->direction.z);
				}
				else if (bunnies[i]->position.z + bunnies[i]->radius > plane->max.z)
				{
					bunnies[i]->direction.z = -abs(bunnies[i]->direction.z);
				}


				// Check other bunnies
				for(int j = i+1; j < bunnies.size(); j++){
					if (!bunnies[j]->dead)
					{
						float distBetween = sqrt(pow(bunnies[i]->position.x - bunnies[j]->position.x, 2) +
													pow(bunnies[i]->position.z - bunnies[j]->position.z, 2));
						// if distance between them is less than the sum of their radii, collision happened
						if(distBetween < (bunnies[i]->radius + bunnies[j]->radius) ){
							vec3 v = normalize(bunnies[i]->position - bunnies[j]->position);
							vec3 n = cross(v, vec3(0, 1, 0));
							bunnies[i]->direction = -reflect(bunnies[i]->direction, n);
							bunnies[j]->direction = -reflect(bunnies[j]->direction, -n);
						}
					}
				}

				// Check camera
				float cameraDist = sqrt(pow(bunnies[i]->position.x - camera->eye.x, 2) +
										pow(bunnies[i]->position.y - camera->eye.y, 2) +
										pow(bunnies[i]->position.z - camera->eye.z, 2));
				if (cameraDist < camera->radius + bunnies[i]->radius)
				{
					bunnies[i]->material = 4;
					bunnies[i]->speed = 0;
					bunnies[i]->dead = true;
					score++;
				}

			}
		}
		*/
	}

	void setLight(shared_ptr<Program> prog)
	{
		glUniform3f(prog->getUniform("dirLightDir"), 0, 1, 1);
		glUniform3f(prog->getUniform("dirLightColor"), 1, 1, 1);
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
			grassTexture->bind(texProg->getUniform("Texture0"));
			break;
		case 1:
			marbleTextures[CURRENT_SKIN]->bind(texProg->getUniform("Texture0"));
			break;
		case 2:
			crateTexture->bind(texProg->getUniform("Texture0"));
			break;
		}
	}

	// helper function to set materials for shading
	void setMaterial(int i)
	{
		switch (i)
		{
		case 0: // turquoise
			glUniform3f(matProg->getUniform("MatAmb"), 0.1, 0.18725, 0.1745);
			glUniform3f(matProg->getUniform("MatDif"), 0.396, 0.74151, 0.69102);
			glUniform3f(matProg->getUniform("MatSpec"), 0.297254, 0.30829, 0.306678);
			glUniform1f(matProg->getUniform("Shine"), 12.8);
			break;
		case 1: // shiny blue plastic
			glUniform3f(matProg->getUniform("MatAmb"), 0.02, 0.04, 0.2);
			glUniform3f(matProg->getUniform("MatDif"), 0.0, 0.16, 0.9);
			glUniform3f(matProg->getUniform("MatSpec"), 0.14, 0.2, 0.8);
			glUniform1f(matProg->getUniform("Shine"), 120.0);
			break;
		case 2: // black plastic
			glUniform3f(matProg->getUniform("MatAmb"), 0, 0, 0);
			glUniform3f(matProg->getUniform("MatDif"), 0.01, 0.01, 0.01);
			glUniform3f(matProg->getUniform("MatSpec"), 0.5, 0.5, 0.5);
			glUniform1f(matProg->getUniform("Shine"), 32);
			break;
		case 3: // brass
			glUniform3f(matProg->getUniform("MatAmb"), 0.3294, 0.2235, 0.02745);
			glUniform3f(matProg->getUniform("MatDif"), 0.7804, 0.5686, 0.11373);
			glUniform3f(matProg->getUniform("MatSpec"), 0.9922, 0.94117, 0.8078);
			glUniform1f(matProg->getUniform("Shine"), 27.9);
			break;
		case 4: // ruby
			glUniform3f(matProg->getUniform("MatAmb"), 0.1745, 0.01175, 0.01175);
			glUniform3f(matProg->getUniform("MatDif"), 0.61424, 0.04136, 0.04136);
			glUniform3f(matProg->getUniform("MatSpec"), 0.727811, 0.626959, 0.626959);
			glUniform1f(matProg->getUniform("Shine"), 76.8);
			break;
		}
	}
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

	// Loop until the user closes the window.
	while (!glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		double t = glfwGetTime();
		double dt = std::min(t - prevTime, 0.1);
		prevTime = t;
		application->render(dt);
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
