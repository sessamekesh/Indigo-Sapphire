// Main entry point for the program!
// Specify which app to set in motion here, but really that's all you should do here.
//  Maybe have a selection screen if you're feeling fancy, but that can be console-driven.

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <sim/TestScene.h>
#include <sim/lakescene/lakescene.h>
#include <thread>

int main()
{
	{
		// This would be able to change things that would otherwise be hard-coded, but you won't be able to add/remove
		//  objects using the CLI interface. Still though. It's something!

		// TODO SESS: Read in input values from a file. This way we can avoid recompiling every time you want
		//  to move something just a little little bit.
		//sim::TestScene scene;

		//if (scene.Init())
		//{
		//	scene.Run();
		//}

		util::command::ParserFactory pf;
		sim::lake::LakeScene ls(pf);
		if (ls.Init())
		{
			// TODO SESS: Make this more elaborate and global, so that the
			//  `t.detach()` at the bottom is unnecessary.
			using namespace std::chrono_literals;
			// Start up command listener thread...
			std::thread t([&ls](){
				std::this_thread::sleep_for(1s);
				std::cout << "Listening for commands." << std::endl;
				while (ls.isRunning())
				{
					std::string nextCmd;
					std::getline(std::cin, nextCmd);
					ls.queueCommand(nextCmd);
				}
				return true;
			});
			ls.Run();
			t.detach();
		}
		ls.Shutdown();
	}

	std::cin.ignore();
	return 0;
}