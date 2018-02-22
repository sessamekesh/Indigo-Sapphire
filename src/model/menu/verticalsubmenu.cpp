#include <model/menu/verticalsubmenu.h>

namespace model
{
	namespace menu
	{
		VerticalSubmenu::VerticalSubmenu(
			const MenuRect& submenuRect,
			float submenuEntryHeight,
			const std::vector<SubmenuEntry>& initialEntries
		)
			: submenuRect_(submenuRect)
			, submenuEntryHeight_(submenuEntryHeight)
			, entries_(initialEntries)
		{}

		void VerticalSubmenu::addMenuItem(const SubmenuEntry& add)
		{
			entries_.push_back(add);
		}

		std::vector<VerticalSubmenu::PlacedSubmenuEntry> VerticalSubmenu::getMenuItems() const
		{
			std::vector<VerticalSubmenu::PlacedSubmenuEntry> tr;
			tr.reserve(entries_.size());

			float y = submenuRect_.TopLeftPos.y;
			for (auto&& e : entries_)
			{
				VerticalSubmenu::PlacedSubmenuEntry next;
				next.Entry = e;
				next.Location.TopLeftPos.x = submenuRect_.TopLeftPos.x;
				next.Location.TopLeftPos.y = y;
				next.Location.Size.x = submenuRect_.Size.x;
				next.Location.Size.y = submenuEntryHeight_;

				y += submenuEntryHeight_;

				tr.push_back(next);
			}

			return tr;
		}

		std::optional<VerticalSubmenu::PlacedSubmenuEntry> VerticalSubmenu::getSelected() const
		{
			float y = submenuRect_.TopLeftPos.y;
			for (auto&& e : entries_)
			{
				if (e.State == SubmenuEntry::SELECTED)
				{
					VerticalSubmenu::PlacedSubmenuEntry next;
					next.Entry = e;
					next.Location.TopLeftPos.x = submenuRect_.TopLeftPos.x;
					next.Location.TopLeftPos.y = y;
					next.Location.Size.x = submenuRect_.Size.x;
					next.Location.Size.y = submenuEntryHeight_;
					return next;
				}
				y += submenuEntryHeight_;
			}

			return std::nullopt;
		}

		void VerticalSubmenu::unselectAll()
		{
			for (auto&& e : entries_)
			{
				if (e.State == SubmenuEntry::SELECTED)
				{
					e.State = SubmenuEntry::INACTIVE;
				}
			}
		}

		void VerticalSubmenu::moveSelectionUp()
		{
			for (std::int32_t idx = 1; idx < entries_.size(); idx++)
			{
				if (entries_[idx].State == SubmenuEntry::SELECTED)
				{
					entries_[idx].State = SubmenuEntry::INACTIVE;
					
					// Try from the entry directly above
					for (std::int32_t nextIdx = idx - 1; nextIdx >= 0; nextIdx--)
					{
						if (entries_[nextIdx].State == SubmenuEntry::INACTIVE)
						{
							entries_[nextIdx].State = SubmenuEntry::SELECTED;
							return;
						}
					}

					// Didn't find it? Try from the end of the list, cycle up
					for (std::int32_t nextIdx = entries_.size() - 1; nextIdx >= idx; idx--)
					{
						if (entries_[nextIdx].State == SubmenuEntry::INACTIVE)
						{
							entries_[nextIdx].State == SubmenuEntry::SELECTED;
							return;
						}
					}
				}
			}
		}

		void VerticalSubmenu::moveSelectionDown()
		{
			for (std::int32_t idx = 1u; idx < entries_.size(); idx++)
			{
				if (entries_[idx].State == SubmenuEntry::SELECTED)
				{
					entries_[idx].State = SubmenuEntry::INACTIVE;

					// Try from the entry directly below
					for (std::int32_t nextIdx = idx + 1; nextIdx < entries_.size(); nextIdx++)
					{
						if (entries_[nextIdx].State == SubmenuEntry::INACTIVE)
						{
							entries_[nextIdx].State = SubmenuEntry::SELECTED;
							return;
						}
					}

					// Didn't find it? Try from the end of the list, cycle up
					for (std::int32_t nextIdx = 0; nextIdx <= idx; idx++)
					{
						if (entries_[nextIdx].State == SubmenuEntry::INACTIVE)
						{
							entries_[nextIdx].State == SubmenuEntry::SELECTED;
							return;
						}
					}
				}
			}
		}
	}
}