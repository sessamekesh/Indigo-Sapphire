#pragma once

/**
 * Vertical Inner Menu
 *
 * Represents an inner menu, which contains a bunch of menu items laid out vertically.
 * Notice that menu entries of each different type are stored distinctly. This is for type safety.
 *  However, this means that if this menu is to be used, all possible types must be anticipated and handled.
 * It is anticipated that this class will be overridden for domain-specific menus.
 */

#include <model/menu/menuitem.h>
#include <model/menu/menurect.h>
#include <model/menu/selectoptionmenuitemvalue.h>
#include <optional>

namespace model
{
	namespace menu
	{
		class VerticalInnerMenu
		{
		public:
			template<typename MenuEntryType>
			struct PlacedInnerMenuEntry
			{
				MenuEntryType Entry;
				MenuRect Location;
			};

		public:
			VerticalInnerMenu(
				float menuEntryHeight
			);
			~VerticalInnerMenu() = default;
			VerticalInnerMenu(const VerticalInnerMenu&) = default;

			//
			// Add menu item group - each of these methods adds a menu item of their respective type
			//
			void addMenuItem(const MenuItem<std::string>& toAdd);
			void addMenuItem(const MenuItem<bool>& toAdd);

			void setMenuRect(const MenuRect& rect);
			void unselectAll();
			void moveSelectionUp();
			void moveSelectionDown();

			//
			// Menu Item Entry Accessors
			//
			std::vector<PlacedInnerMenuEntry<MenuItem<std::string>>> getStringMenuItems() const;
			std::optional<PlacedInnerMenuEntry<MenuItem<std::string>>> getSelectedStringMenuItem() const;
			std::optional<PlacedInnerMenuEntry<MenuItem<bool>>> getSelectedBoolMenuItem() const;

		protected:
			std::uint32_t nextIdx_;
			std::optional<std::uint32_t> selectionIdx_;
			MenuRect menuRect_;
			float menuEntryHeight_;

			//
			// Menu Item Entries
			//
			std::map<std::uint32_t, MenuItem<std::string>> stringMenuItems_;
			std::map<std::uint32_t, MenuItem<bool>> boolMenuItems_;
		};
	}
}