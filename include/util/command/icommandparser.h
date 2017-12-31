#pragma once

#include <vector>
#include <string>
#include <memory>

namespace util
{
	namespace command
	{
		struct PropertyCommand
		{
			std::vector<std::string> subVals;
			std::string val;
		};

		class ICommandParser
		{
		public:
			virtual bool setValue(const PropertyCommand& cmd, std::shared_ptr<void> ptr) = 0;
			virtual std::vector<PropertyCommand> serializeValue(std::shared_ptr<void> ptr) = 0;
			virtual std::string getUUID() = 0;
		};
	}
}