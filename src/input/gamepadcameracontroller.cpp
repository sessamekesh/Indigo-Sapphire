#include <input/gamepadcameracontroller.h>
#include <GLFW/glfw3.h>

namespace input
{
	GamepadCameraController::GamepadCameraController(int gamepadIdx)
		: gamepadIdx_(gamepadIdx)
		, numAxes_(0)
		, axes_(nullptr)
	{}

	void GamepadCameraController::tick()
	{
		// Now 100% sure this works, just rolling with it for now
		if (glfwJoystickPresent(GLFW_JOYSTICK_1 + gamepadIdx_))
		{
			axes_ = glfwGetJoystickAxes(GLFW_JOYSTICK_1 + gamepadIdx_, &numAxes_);
		}
		else
		{
			axes_ = nullptr;
		}
	}

	bool GamepadCameraController::isActive()
	{
		return axes_ != nullptr && numAxes_ >= 4;
	}

	float GamepadCameraController::getY1Axis() const
	{
		if (!axes_)
		{
			return 0.f;
		}
		else
		{
			return axes_[1];
		}
	}

	float GamepadCameraController::getY2Axis() const
	{
		if (!axes_)
		{
			return 0.f;
		}
		else
		{
			return axes_[3];
		}
	}

	float GamepadCameraController::getX1Axis() const
	{
		if (!axes_)
		{
			return 0.f;
		}
		else
		{
			return axes_[0];
		}
	}

	float GamepadCameraController::getX2Axis() const
	{
		if (!axes_)
		{
			return 0.f;
		}
		else
		{
			return axes_[2];
		}
	}
}