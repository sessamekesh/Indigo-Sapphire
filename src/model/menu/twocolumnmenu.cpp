#include <model/menu/twocolumnmenu.h>

namespace model
{
	namespace menu
	{
		TwoColumnMenu::TwoColumnMenu(
			glm::vec2 logicalSize,
			float headerHeight,
			float footerHeight,
			float leftColumnWidth,
			float submenuEntryHeight
		)
			: boundingRect_({ glm::vec2(0.f, 0.f), logicalSize })
			, headerRect_({ glm::vec2(0.f, 0.f), glm::vec2(logicalSize.x, headerHeight) })
			, footerRect_({ glm::vec2(0.f, logicalSize.y - footerHeight), glm::vec2(logicalSize.x, footerHeight) })
			, submenuRect_({ glm::vec2(0.f, headerHeight), glm::vec2(leftColumnWidth, logicalSize.y - footerHeight - headerHeight) })
			, innerMenuRect_({ glm::vec2(leftColumnWidth, headerHeight), glm::vec2(logicalSize.x - leftColumnWidth, logicalSize.y - headerHeight - footerHeight) })
			, submenuEntryHeight_(submenuEntryHeight)
			, submenu_({ glm::vec2(0.f, headerHeight), glm::vec2(leftColumnWidth, logicalSize.y - footerHeight - headerHeight) }, submenuEntryHeight, {})
			, innerMenus_({})
			, isSubmenuActive_(true)
			, isEntryActive_(false)
			, log(util::DEBUG, util::WARN, "[TwoColumnMenu] ")
		{}

		void TwoColumnMenu::addSubmenu(std::string name, std::string caption, VerticalInnerMenu toAdd)
		{
			toAdd.setMenuRect(innerMenuRect_);
			submenu_.addMenuItem(std::make_shared<model::menu::SubmenuEntry>(model::menu::SubmenuEntry({ name, caption, SubmenuEntry::INACTIVE })));
			if (innerMenus_.find(name) != innerMenus_.end())
			{
				log.warn << "Warning: A menu item by name \"" << name << "\" already exists and will be overridden." << util::endl;
			}
			innerMenus_.insert({ name, toAdd });
		}

		std::optional<VerticalInnerMenu> TwoColumnMenu::getInnerMenu()
		{
			auto selectedEntryOpt = submenu_.getSelected();
			if (!selectedEntryOpt)
			{
				return std::nullopt;
			}

			auto selectedEntry = *selectedEntryOpt;

			auto menu = innerMenus_.find(selectedEntry.Entry->Name);
			if (menu == innerMenus_.end())
			{
				log.warn << "No menu by name \"" << selectedEntry.Entry->Name << "\" found!" << util::endl;
				return std::nullopt;
			}

			return { menu->second };
		}

		bool TwoColumnMenu::isSubmenuActive() const
		{
			return isSubmenuActive_;
		}

		bool TwoColumnMenu::isInnerMenuActive() const
		{
			return !isSubmenuActive_ && !isEntryActive_;
		}

		void TwoColumnMenu::moveSelectionUp()
		{
			if (isSubmenuActive())
			{
				submenu_.moveSelectionUp();
			}
			else if (isInnerMenuActive())
			{
				auto innerMenuOpt = getInnerMenu();
				if (!innerMenuOpt)
				{
					return;
				}
				innerMenuOpt->moveSelectionUp();
			}
		}

		void TwoColumnMenu::moveSelectionDown()
		{
			if (isSubmenuActive())
			{
				submenu_.moveSelectionDown();
			}
			else if (isInnerMenuActive())
			{
				auto innerMenuOpt = getInnerMenu();
				if (!innerMenuOpt)
				{
					return;
				}
				innerMenuOpt->moveSelectionDown();
			}
		}

		void TwoColumnMenu::select()
		{
			if (isSubmenuActive_)
			{
				isSubmenuActive_ = false;
				isEntryActive_ = false;
			}
			else if (!isEntryActive_)
			{
				// TODO SESS: Start modifying this entry
				isEntryActive_ = true;
			}
			else
			{
				// TODO SESS: Also "apply" the inner menu item
				isEntryActive_ = false;
			}
		}

		void TwoColumnMenu::deselect()
		{
			if (isSubmenuActive_)
			{
				return;
			}
			else if (isEntryActive_)
			{
				isEntryActive_ = false;
			}
			else
			{
				isSubmenuActive_ = true;
				isEntryActive_ = false;
			}
		}
	}
}