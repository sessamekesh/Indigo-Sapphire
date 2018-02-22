#pragma once

#include <input/menunavigationcontrollerbase.h>

namespace input
{
	class GamepadMenuNavigationController : public MenuNavigationControllerBase
	{
	public:
		GamepadMenuNavigationController(int gamepadIdx);
		~GamepadMenuNavigationController() = default;
		GamepadMenuNavigationController(const GamepadMenuNavigationController&) = delete;

	public:
		// Inherited via MenuNavigationControllerBase
		void tick() override;

	private:
		int gamepadIdx_;

		bool isUpPressed_;
		bool isDownPressed_;
		bool isAPressed_;
		bool isBPressed_;
		bool isStartPressed_;
	};
}