#pragma once

#include <input/cameracontrollerbase.h>

namespace input
{
	class GamepadCameraController : public CameraControllerBase
	{
	public:
		GamepadCameraController(int gamepadIdx);
		~GamepadCameraController() = default;
		GamepadCameraController(const GamepadCameraController&) = default;

	private:
		int gamepadIdx_;
		int numAxes_;
		const float* axes_;

		// Inherited via CameraControllerBase
		virtual void tick() override;
		virtual bool isActive() override;
		virtual float getY1Axis() const override;
		virtual float getY2Axis() const override;
		virtual float getX1Axis() const override;
		virtual float getX2Axis() const override;
	};
}