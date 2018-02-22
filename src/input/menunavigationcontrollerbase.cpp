#include <input/menunavigationcontrollerbase.h>

namespace input
{
	MenuNavigationControllerBase::MenuNavigationControllerBase()
		: nextHandle_(0u)
		, upButtonListeners_({})
		, downButtonListeners_({})
		, selectButtonListeners_({})
		, deselectButtonListeners_({})
		, toggleMenuButtonListeners_({})
	{}

	std::uint32_t MenuNavigationControllerBase::addUpButtonListener(std::function<void()> listener)
	{
		auto handle = nextHandle_++;
		upButtonListeners_.insert({ handle, listener });
		return handle;
	}

	std::uint32_t MenuNavigationControllerBase::addDownButtonListener(std::function<void()> listener)
	{
		auto handle = nextHandle_++;
		downButtonListeners_.insert({ handle, listener });
		return handle;
	}

	std::uint32_t MenuNavigationControllerBase::addSelectButtonListener(std::function<void()> listener)
	{
		auto handle = nextHandle_++;
		selectButtonListeners_.insert({ handle, listener });
		return handle;
	}

	std::uint32_t MenuNavigationControllerBase::addDeselectButtonListener(std::function<void()> listener)
	{
		auto handle = nextHandle_++;
		deselectButtonListeners_.insert({ handle, listener });
		return handle;
	}

	std::uint32_t MenuNavigationControllerBase::addToggleMenuButtonListener(std::function<void()> listener)
	{
		auto handle = nextHandle_++;
		toggleMenuButtonListeners_.insert({ handle, listener });
		return handle;
	}

	void MenuNavigationControllerBase::removeListener(std::uint32_t handle)
	{
		upButtonListeners_.erase(handle);
		downButtonListeners_.erase(handle);
		selectButtonListeners_.erase(handle);
		deselectButtonListeners_.erase(handle);
		toggleMenuButtonListeners_.erase(handle);
	}

	void MenuNavigationControllerBase::fireUpButton()
	{
		for (auto&& l : upButtonListeners_)
		{
			l.second();
		}
	}

	void MenuNavigationControllerBase::fireDownButton()
	{
		for (auto&& l : downButtonListeners_)
		{
			l.second();
		}
	}

	void MenuNavigationControllerBase::fireSelectButton()
	{
		for (auto&& l : selectButtonListeners_)
		{
			l.second();
		}
	}

	void MenuNavigationControllerBase::fireDeselectButton()
	{
		for (auto&& l : deselectButtonListeners_)
		{
			l.second();
		}
	}

	void MenuNavigationControllerBase::fireToggleMenuButton()
	{
		for (auto&& l : toggleMenuButtonListeners_)
		{
			l.second();
		}
	}
}