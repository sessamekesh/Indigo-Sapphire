#include <model/menu/verticalinnermenu.h>

namespace model
{
	namespace menu
	{
		VerticalInnerMenu::VerticalInnerMenu(
			float menuEntryHeight
		)
			: nextIdx_(0u)
			, selectionIdx_(std::nullopt)
			, menuRect_({})
			, menuEntryHeight_(menuEntryHeight)
			, stringMenuItems_({})
		{}

		void VerticalInnerMenu::addMenuItem(const MenuItem<std::string>& toAdd)
		{
			auto idx = nextIdx_++;
			stringMenuItems_[idx] = toAdd;
		}

		void VerticalInnerMenu::addMenuItem(const MenuItem<bool>& toAdd)
		{
			auto idx = nextIdx_++;
			boolMenuItems_.insert({ idx, toAdd });
		}

		void VerticalInnerMenu::setMenuRect(const MenuRect& rect)
		{
			menuRect_ = rect;
		}

		void VerticalInnerMenu::unselectAll()
		{
			selectionIdx_ = std::nullopt;
		}

		void VerticalInnerMenu::moveSelectionUp()
		{
			if (selectionIdx_)
			{
				(*selectionIdx_)++;
				if (*selectionIdx_ >= nextIdx_)
				{
					selectionIdx_ = { 0u };
				}
			}
		}

		void VerticalInnerMenu::moveSelectionDown()
		{
			if (selectionIdx_)
			{
				if (*selectionIdx_ == 0u)
				{
					if (nextIdx_ > 0u)
					{
						selectionIdx_ = { nextIdx_ - 1u };
					}
					else
					{
						selectionIdx_ = std::nullopt;
					}
				}
				else
				{
					(*selectionIdx_)--;
				}
			}
		}

		std::vector<VerticalInnerMenu::PlacedInnerMenuEntry<MenuItem<std::string>>> VerticalInnerMenu::getStringMenuItems() const
		{
			std::vector<VerticalInnerMenu::PlacedInnerMenuEntry<MenuItem<std::string>>> tr;

			for (auto&& entry : stringMenuItems_)
			{
				VerticalInnerMenu::PlacedInnerMenuEntry<MenuItem<std::string>> next;

				next.Entry = entry.second;
				next.Location.TopLeftPos.x = menuRect_.TopLeftPos.x;
				next.Location.TopLeftPos.y = menuRect_.TopLeftPos.y + entry.first * menuEntryHeight_;
				next.Location.Size.x = menuRect_.Size.x;
				next.Location.Size.y = menuEntryHeight_;

				tr.push_back(next);
			}

			return tr;
		}

		std::optional<VerticalInnerMenu::PlacedInnerMenuEntry<MenuItem<std::string>>> VerticalInnerMenu::getSelectedStringMenuItem() const
		{
			if (!selectionIdx_)
			{
				return std::nullopt;
			}

			auto idx = *selectionIdx_;

			auto itMenuItem = stringMenuItems_.find(idx);
			if (itMenuItem == stringMenuItems_.end())
			{
				return std::nullopt;
			}

			VerticalInnerMenu::PlacedInnerMenuEntry<MenuItem<std::string>> tr;
			tr.Entry = itMenuItem->second;
			tr.Location.TopLeftPos.x = menuRect_.TopLeftPos.x;
			tr.Location.TopLeftPos.y = menuRect_.TopLeftPos.y + itMenuItem->first * menuEntryHeight_;
			tr.Location.Size.x = menuRect_.Size.x;
			tr.Location.Size.y = menuEntryHeight_;

			return tr;
		}

		std::optional<VerticalInnerMenu::PlacedInnerMenuEntry<MenuItem<bool>>> VerticalInnerMenu::getSelectedBoolMenuItem() const
		{
			if (!selectionIdx_)
			{
				return std::nullopt;
			}

			auto idx = *selectionIdx_;

			auto itMenuItem = boolMenuItems_.find(idx);
			if (itMenuItem == boolMenuItems_.end())
			{
				return std::nullopt;
			}

			VerticalInnerMenu::PlacedInnerMenuEntry<MenuItem<bool>> tr = { itMenuItem->second };
			tr.Location.TopLeftPos.x = menuRect_.TopLeftPos.x;
			tr.Location.TopLeftPos.y = menuRect_.TopLeftPos.y + itMenuItem->first * menuEntryHeight_;
			tr.Location.Size.x = menuRect_.Size.x;
			tr.Location.Size.y = menuEntryHeight_;

			return tr;
		}
	}
}