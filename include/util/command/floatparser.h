#pragma once

#include <util/command/icommandparser.h>

namespace util
{
	namespace command
	{
		class FloatParser : public ICommandParser
		{
		public:
			FloatParser() = default;
			~FloatParser() = default;
			FloatParser(const FloatParser&) = default;

			// Inherited via ICommandParser
			virtual bool setValue(const PropertyCommand& cmd, std::shared_ptr<void> ptr) override;
			virtual std::vector<PropertyCommand> serializeValue(std::shared_ptr<void> ptr) override;
			virtual std::string getUUID() override;

			static const std::string uuid_;
		};
	}
}