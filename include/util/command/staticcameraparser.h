#pragma once

#include <util/command/icommandparser.h>
#include <util/command/vec3parser.h>

namespace util
{
	namespace command
	{
		class StaticCameraParser : public ICommandParser
		{
		public:
			StaticCameraParser(const Vec3Parser& vp);
			~StaticCameraParser() = default;
			StaticCameraParser(const StaticCameraParser&) = default;

			static const std::string uuid;

			// Inherited via ICommandParser
			virtual bool setValue(const PropertyCommand& cmd, std::shared_ptr<void> ptr) override;
			virtual std::vector<PropertyCommand> serializeValue(std::shared_ptr<void> ptr) override;
			virtual std::string getUUID() override;

		private:
			Vec3Parser vec3_;
		};
	}
}