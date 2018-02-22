#include <input/gamepadmenunavigationcontroller.h>
#include <GLFW\glfw3.h>
#include <iostream>

namespace input
{
	GamepadMenuNavigationController::GamepadMenuNavigationController(int gamepadIdx)
		: MenuNavigationControllerBase()
		, gamepadIdx_(gamepadIdx)
		, isUpPressed_(false)
		, isDownPressed_(false)
		, isAPressed_(false)
		, isBPressed_(false)
		, isStartPressed_(false)
	{}

	void GamepadMenuNavigationController::tick()
	{
		int count;
		const unsigned char* buttons = glfwGetJoystickButtons(gamepadIdx_, &count);

		// This gamepad controller can't be used - error state
		if (count <= 13)
		{
			return;
		}

		// XBox One controller
		bool up = buttons[10] == GLFW_PRESS;
		bool down = buttons[12] == GLFW_PRESS;
		bool a = buttons[0] == GLFW_PRESS;
		bool b = buttons[1] == GLFW_PRESS;
		bool start = buttons[7] == GLFW_PRESS;

		if (isUpPressed_ != up)
		{
			if (!isUpPressed_)
			{
				fireUpButton();
			}
			isUpPressed_ = up;
		}

		if (isDownPressed_ != down)
		{
			if (!isDownPressed_)
			{
				fireDownButton();
			}
			isDownPressed_ = down;
		}

		if (isAPressed_ != a)
		{
			if (!isAPressed_)
			{
				fireSelectButton();
			}
			isAPressed_ = a;
		}

		if (isBPressed_ != b)
		{
			if (!isBPressed_)
			{
				fireDeselectButton();
			}
			isBPressed_ = b;
		}

		if (isStartPressed_ != start)
		{
			if (!isStartPressed_)
			{
				fireToggleMenuButton();
			}
			isStartPressed_ = start;
		}
	}
}