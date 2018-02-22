#pragma once

/**
 * Menu Navigation Controller Base
 *
 * Menu navigation is only concerned with button presses, so this is a subscription-based
 *  class. Events get fired when an action is performed.
 */

#include <functional>
#include <map>

namespace input
{
	class MenuNavigationControllerBase
	{
	public:
		MenuNavigationControllerBase();
		~MenuNavigationControllerBase() = default;
		MenuNavigationControllerBase(const MenuNavigationControllerBase&) = default;

		std::uint32_t addUpButtonListener(std::function<void()> listener);
		std::uint32_t addDownButtonListener(std::function<void()> listener);
		std::uint32_t addSelectButtonListener(std::function<void()> listener);
		std::uint32_t addDeselectButtonListener(std::function<void()> listener);
		std::uint32_t addToggleMenuButtonListener(std::function<void()> listener);

		void removeListener(std::uint32_t handle);

		// This method is called per frame. This is what should update the states of the buttons,
		//  and invoke a method when it changes.
		virtual void tick() = 0;

	public:
		void fireUpButton();
		void fireDownButton();
		void fireSelectButton();
		void fireDeselectButton();
		void fireToggleMenuButton();

	protected:
		std::uint32_t nextHandle_;
		std::map<std::uint32_t, std::function<void()>> upButtonListeners_;
		std::map<std::uint32_t, std::function<void()>> downButtonListeners_;
		std::map<std::uint32_t, std::function<void()>> selectButtonListeners_;
		std::map<std::uint32_t, std::function<void()>> deselectButtonListeners_;
		std::map<std::uint32_t, std::function<void()>> toggleMenuButtonListeners_;
	};
}