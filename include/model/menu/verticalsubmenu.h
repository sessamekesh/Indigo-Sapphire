#pragma once

#include <model/menu/menurect.h>
#include <model/menu/submenuentry.h>
#include <vector>
#include <optional>
#include <memory>

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
				std::shared_ptr<SubmenuEntry> Entry;
				MenuRect Location;
			};

		public:
			VerticalSubmenu(
				const MenuRect& submenuRect,
				float submenuEntryHeight,
				const std::vector<std::shared_ptr<SubmenuEntry>>& initialEntries
			);
			~VerticalSubmenu() = default;
			VerticalSubmenu(const VerticalSubmenu&) = default;
			void addMenuItem(const std::shared_ptr<SubmenuEntry>& add);

			std::vector<PlacedSubmenuEntry> getMenuItems() const;

			std::optional<VerticalSubmenu::PlacedSubmenuEntry> getSelected() const;
			void unselectAll();
			void moveSelectionUp();
			void moveSelectionDown();

			MenuRect rect() const { return submenuRect_; }
			float submenuEntryHeight() { return submenuEntryHeight_; }

		protected:
			MenuRect submenuRect_;
			float submenuEntryHeight_;
			std::vector<std::shared_ptr<SubmenuEntry>> entries_;
		};
	}
}