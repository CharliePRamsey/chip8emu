#include "pch.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "chip8emu.h"

chip8emu ch8;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS || action == GLFW_RELEASE)
	{
		ch8.keyEvent(key, action);
	}
}

GLFWwindow* setupGraphics()
{
	GLFWwindow* window;
	if (!glfwInit())
		std::cout << "Initialization of GLFWwindow failed." << std::endl;

	window = glfwCreateWindow(640, 320, "CHIM", NULL, NULL);
	if (!window)
	{
		std::cout << "Window Creation or OpenGL Context Creation Failed";
	}

	glfwMakeContextCurrent(window);
	glewInit();

	glViewport(0, -640, 640, 320);

	glfwSetKeyCallback(window, key_callback);

	glfwSwapInterval(1);

	return window;
}

int main(void)
{
	GLFWwindow* window = setupGraphics();
	double time;
	int cycleSecond;
	int cyclesThisSecond = 0;

	ch8.initialize();
	ch8.loadGame("SpaceInvadersDavidWinter.ch8");

	while (!glfwWindowShouldClose(window))
	{
		//Keep track of time for throttling to 60 cycles per second.
		time = glfwGetTime();
		if (cyclesThisSecond == 0)
			cycleSecond = (int)time;

		if (cyclesThisSecond < 60)
		{
			if(!ch8.waitForKey)
				ch8.emulateCycle();
			cyclesThisSecond++;
		}
		else if(time - cycleSecond >= 1)
			cyclesThisSecond = 0;

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Swap the frame buffer for rendering.
		glfwSwapBuffers(window);
		//Poll events to read input.
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}