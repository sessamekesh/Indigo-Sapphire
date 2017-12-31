#pragma once

#include <util/command/icommandparser.h>
#include <util/command/floatparser.h>

namespace util
{
	namespace command
	{
		class Vec3Parser : public ICommandParser
		{
		public:
			Vec3Parser(const FloatParser& fp);
			~Vec3Parser() = default;
			Vec3Parser(const Vec3Parser&) = default;

			// Inherited via ICommandParser
			virtual bool setValue(const PropertyCommand& cmd, std::shared_ptr<void> ptr) override;
			virtual std::vector<PropertyCommand> serializeValue(std::shared_ptr<void> ptr) override;
			virtual std::string getUUID() override;

			static const std::string uuid;

		private:
			FloatParser float_;
		};
	}
}