#pragma once

#include <string>

/**
 * Submenu Entry
 *
 * A submenu item that can be selected. Possible states: DISABLED, INACTIVE, SELECTED
 *  Submenu items are not aware of their own position in the menu - they are placed by a containing
 *  submenu object.
 */

namespace model
{
	namespace menu
	{
		struct SubmenuEntry
		{
		public:
			enum STATE
			{
				DISABLED, // This submenu entry cannot be selected
				INACTIVE, // This submenu entry can be selected, but is not
				SELECTED // This submenu entry is currently selected
			};

			std::string Name; // Uniquely identifying name within the context of this menu
			std::string Caption; // Caption for this submenu entry
			STATE State; // Current state of the submenu item (should start out DISABLED or INACTIVE)
		};
	}
}