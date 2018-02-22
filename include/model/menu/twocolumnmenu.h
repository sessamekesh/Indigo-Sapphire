#pragma once

/**
 * Two Column Menu
 *
 * A type of menu that has two columns - the left is the submenu, where different inner menus are selected
 *  The right column displays the contents of the submenu, with all the properties that can be changed
 * There is also a header at the top, and a footer at the bottom. These can be used to show additional information, if needed.
 * This uses only the vanilla implementation of VerticalInnerMenu, so it cannot accomodate domain-specific menus
 */

#include <model/menu/menurect.h>
#include <model/menu/verticalsubmenu.h>
#include <model/menu/verticalinnermenu.h>
#include <optional>
#include <util/Logger.h>

namespace model
{
	namespace menu
	{
		class TwoColumnMenu
		{
		public:
			TwoColumnMenu(
				glm::vec2 logicalSize,
				float headerHeight,
				float footerHeight,
				float leftColumnWidth,
				float submenuEntryHeight
			);
			~TwoColumnMenu() = default;
			TwoColumnMenu(const TwoColumnMenu&) = default;

			MenuRect getBoundingRect() { return boundingRect_; }
			MenuRect getHeaderRect() { return headerRect_; }
			MenuRect getFooterRect() { return footerRect_; }

			VerticalSubmenu getSubmenu() { return submenu_; }

			std::optional<VerticalInnerMenu> getInnerMenu();

			void addSubmenu(std::string name, std::string caption, VerticalInnerMenu toAdd);

			void moveSelectionUp();
			void moveSelectionDown();
			void select();
			void deselect();

		protected:
			bool isSubmenuActive() const;
			bool isInnerMenuActive() const;

		protected:
			MenuRect boundingRect_;
			MenuRect headerRect_;
			MenuRect footerRect_;
			MenuRect submenuRect_;
			MenuRect innerMenuRect_;
			float submenuEntryHeight_;

			VerticalSubmenu submenu_;
			std::map<std::string, VerticalInnerMenu> innerMenus_;

			bool isSubmenuActive_;
			bool isEntryActive_;

			util::Logger log;
		};
	}
}