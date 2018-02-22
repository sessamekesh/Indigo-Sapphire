#pragma once

#include <string>

/**
 * Menu Item
 *
 * A leaf-level menu item that can be selected, attached to a property that can be changed.
 * Possible states: DISABLED, INACTIVE, HOVERED, SELECTED
 * The MenuItemValue will actually handle the changing of values - a MenuItem only holds data
 * MenuItems also do not know about their layout - they are placed in a containing InnerMenu object
 */

#include <string>
#include <memory>
#include <model/menu/menuitemvaluebase.h>

namespace model
{
	namespace menu
	{
		template<typename Value>
		struct MenuItem
		{
			enum STATE
			{
				DISABLED, // Cannot be selected
				INACTIVE, // Can be selected, but is not
				HOVERED, // Can be selected, and is being considered for selection
				SELECTED // Currently selected
			};

			STATE State;
			std::string Name; // Uniquely identifying name within the context of this menu
			std::string Label; // Label that identifies what this menu item is showing
			std::shared_ptr<MenuItemValueBase<Value>> MenuItemValue; // Value of this menu item
		};
	}
}