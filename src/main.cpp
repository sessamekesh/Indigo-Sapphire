// Main entry point for the program!
// Specify which app to set in motion here, but really that's all you should do here.
//  Maybe have a selection screen if you're feeling fancy, but that can be console-driven.

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <sim/TestScene.h>

int main()
{
	{
		sim::TestScene scene;

		if (scene.Init())
		{
			scene.Run();
		}
	}

	std::cin.ignore();
	return 0;
}