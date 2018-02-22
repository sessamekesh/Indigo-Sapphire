#pragma once

#include <model/menu/menuitemvaluebase.h>

namespace model
{
	namespace menu
	{
		class SelectOptionMenuItemValue : public MenuItemValueBase<std::string>
		{
		public:
			SelectOptionMenuItemValue(
				const std::vector<std::string>& possibleOptions,
				std::string initiallySelected
			);

			virtual std::string enforceValueBounds(const std::string& attemptedVal) const override;

		protected:
			std::vector<std::string> options_;
		};
	}
}