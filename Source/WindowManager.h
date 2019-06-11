#pragma  once

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "GLSL.h"

using namespace std;

// This interface let's us write our own class that can be notified by input
// events, such as key presses and mouse movement.
class EventCallbacks
{
public:
	virtual void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) = 0;

	virtual void mouseCallback(GLFWwindow *window, int button, int action, int mods) = 0;
	virtual void scrollCallback(GLFWwindow *window, double dX, double dY) = 0;

	virtual void resizeCallback(GLFWwindow *window, int in_width, int in_height) = 0;
};

// This class is responsible for all window management code, i.e. GLFW3 code
// You shouldn't have to touch this code for any of the early lab assignments
class WindowManager
{
public:

	WindowManager();
	~WindowManager();

	// Go ahead and ignore these two lines :-) for now
	WindowManager(const WindowManager&) = delete;
	WindowManager& operator= (const WindowManager&) = delete;

	bool init(int const width, int const height, bool maximized, bool fullscreen);
	void shutdown();

	void setEventCallbacks(EventCallbacks *callbacks);

	GLFWwindow *getHandle();

protected:

	// This class implements the singleton design pattern
	static WindowManager * instance;

	GLFWwindow *windowHandle = nullptr;
	EventCallbacks *callbacks = nullptr;

private:

	// What are these?!
	//
	// GLFW3 expects C-style callbacks, but we want to be able to use C++-style
	// callbacks so that we can avoid global variables.
	//
	// This is a common trick or `idiom` that makes it possible
	static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
	static void mouse_callback(GLFWwindow *window, int button, int action, int mods);
	static void scroll_callback(GLFWwindow *window, double dX, double dY);
	static void resize_callback(GLFWwindow *window, int in_width, int in_height);

};
