#pragma once

#include <model/menu/menurect.h>
#include <model/menu/submenuentry.h>
#include <vector>
#include <optional>

/**
 * Vertical Submenu
 *
 * A collection of submenu entries that can be arranged vertically, for example on the left side of a menu.
 *  This _should_ also contain logic for obscuring hidden menu items, and handling scrolling.
 */

namespace model
{
	namespace menu
	{
		class VerticalSubmenu
		{
		public:
			struct PlacedSubmenuEntry
			{
				SubmenuEntry Entry;
				MenuRect Location;
			};

		public:
			VerticalSubmenu(
				const MenuRect& submenuRect,
				float submenuEntryHeight,
				const std::vector<SubmenuEntry>& initialEntries
			);
			~VerticalSubmenu() = default;
			VerticalSubmenu(const VerticalSubmenu&) = default;
			void addMenuItem(const SubmenuEntry& add);

			std::vector<PlacedSubmenuEntry> getMenuItems() const;

			std::optional<VerticalSubmenu::PlacedSubmenuEntry> getSelected() const;
			void unselectAll();
			void moveSelectionUp();
			void moveSelectionDown();

			MenuRect rect() const { return submenuRect_; }

		protected:
			MenuRect submenuRect_;
			float submenuEntryHeight_;
			std::vector<SubmenuEntry> entries_;
		};
	}
}