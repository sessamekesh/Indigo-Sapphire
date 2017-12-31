#pragma once

#include <util/command/vec3parser.h>
#include <util/command/floatparser.h>
#include <util/command/icommandparser.h>

namespace util
{
	namespace command
	{
		class QuaternionParser : public ICommandParser
		{
		public:
			QuaternionParser(
				const FloatParser& fp,
				const Vec3Parser& v3p
			);
			~QuaternionParser() = default;
			QuaternionParser(const QuaternionParser&) = default;

			// Inherited via ICommandParser
			virtual bool setValue(const PropertyCommand& cmd, std::shared_ptr<void> ptr) override;
			virtual std::vector<PropertyCommand> serializeValue(std::shared_ptr<void> ptr) override;
			virtual std::string getUUID() override;

			static const std::string uuid;

		private:
			FloatParser float_;
			Vec3Parser vec3_;
		};
	}
}