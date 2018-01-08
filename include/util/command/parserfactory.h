#pragma once

#include <util/command/staticcameraparser.h>
#include <util/command/quaternion.h>
#include <util/command/withworldtransformparser.h>
#include <util/command/directionallightparser.h>
#include <util/command/waterflatsurface.h>
#include <memory>

namespace util
{
	namespace command
	{
		class ParserFactory
		{
		public:
			ParserFactory();
			~ParserFactory() = default;
			ParserFactory(const ParserFactory&) = delete;

			std::shared_ptr<FloatParser> floatParser();
			std::shared_ptr<Vec3Parser> vec3Parser();
			std::shared_ptr<StaticCameraParser> staticCameraParser();
			std::shared_ptr<QuaternionParser> quaternionParser();
			std::shared_ptr<WithWorldTransformParser> withWorldTransformParser();
			std::shared_ptr<DirectionalLightParser> directionalLightParser();
			std::shared_ptr<WaterFlatSurface> waterFlatSurfaceParser();

		private:
			std::shared_ptr<FloatParser> float_;
			std::shared_ptr<Vec3Parser> vec3_;
			std::shared_ptr<StaticCameraParser> staticCamera_;
			std::shared_ptr<QuaternionParser> quaternion_;
			std::shared_ptr<WithWorldTransformParser> withWorldTransform_;
			std::shared_ptr<DirectionalLightParser> directionalLight_;
			std::shared_ptr<WaterFlatSurface> waterFlatSurface_;
		};
	}
}