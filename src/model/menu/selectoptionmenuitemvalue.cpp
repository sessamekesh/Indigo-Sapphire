#include <model/menu/selectoptionmenuitemvalue.h>

namespace model
{
	namespace menu
	{
		SelectOptionMenuItemValue::SelectOptionMenuItemValue(
			const std::vector<std::string>& possibleOptions,
			std::string initiallySelected
		)
			: MenuItemValueBase<std::string>("")
			, options_(possibleOptions)
		{
			setValue(initiallySelected);
		}

		std::string SelectOptionMenuItemValue::enforceValueBounds(const std::string& attemptedVal) const
		{
			auto option = std::find(options_.begin(), options_.end(), attemptedVal);
			if (option == options_.end())
			{
				return currentValue_;
			}
			else
			{
				return attemptedVal;
			}
		}
	}
}