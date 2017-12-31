#pragma once

#include <util/command/icommandparser.h>
#include <util/command/quaternion.h>
#include <util/command/vec3parser.h>

// Useful for anything that has the "with world transform" class on it
namespace util
{
	namespace command
	{
		class WithWorldTransformParser : public ICommandParser
		{
		public:
			WithWorldTransformParser(
				const Vec3Parser& v3p,
				const QuaternionParser& qp
			);
			~WithWorldTransformParser() = default;
			WithWorldTransformParser(const WithWorldTransformParser&) = default;

			// Inherited via ICommandParser
			virtual bool setValue(const PropertyCommand& cmd, std::shared_ptr<void> ptr) override;
			virtual std::vector<PropertyCommand> serializeValue(std::shared_ptr<void> ptr) override;
			virtual std::string getUUID() override;

			static const std::string uuid;

		private:
			Vec3Parser vec3_;
			QuaternionParser quaternion_;
		};
	}
}