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
		// TODO SESS: Add a command structure, so you can send commands to the scene
		//  Probably on a separate thread, every frame checking to see if a command has been issued
		//  Commands like
		// - list
		// - pos <object_id>
		// - rot <object_id>
		// - scl <object_id>
		// - setpos <object_id> <pos>
		// - setrot <object_id> <rot>
		// - setscl <object_id> <scl>
		// - export <filename>
		// And then of course, standard WASD/arrows (shift for slow, ctrl fast) for moving the camera
		// Commands are interpreted by the scene

		// Envisioned example:
		// [start: null scene]
		// list scenes
		// run IntroScene
		// record IntroScene introsceneframes
		// run CharacterScene
		// inspect CharacterScene
		// show_pip
		// list
		// keyframes main_camera
		// set mc_kf1_pos 0 5 0
		// run CharacterScene

		// This would be able to change things that would otherwise be hard-coded, but you won't be able to add/remove
		//  objects using the CLI interface. Still though. It's something!

		// TODO SESS: Read in input values from a file. This way we can avoid recompiling every time you want
		//  to move something just a little little bit.
		sim::TestScene scene;

		if (scene.Init())
		{
			scene.Run();
		}
	}

	std::cin.ignore();
	return 0;
}