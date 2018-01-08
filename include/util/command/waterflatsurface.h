#pragma once

#include <util/command/icommandparser.h>
#include <util/command/vec3parser.h>
#include <util/command/withworldtransformparser.h>

namespace util
{
	namespace command
	{
		class WaterFlatSurface : public ICommandParser
		{
		public:
			WaterFlatSurface(
				const WithWorldTransformParser& wwp,
				const FloatParser& fp
			);
			~WaterFlatSurface() = default;
			WaterFlatSurface(const WaterFlatSurface&) = default;

			// Inherited via ICommandParser
			virtual bool setValue(const PropertyCommand& cmd, std::shared_ptr<void> ptr) override;
			virtual std::vector<PropertyCommand> serializeValue(std::shared_ptr<void> ptr) override;
			virtual std::string getUUID() override;

			static const std::string uuid;

		private:
			WithWorldTransformParser withWorldTransform_;
			FloatParser float_;
		};
	}
}