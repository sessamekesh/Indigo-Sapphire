#pragma once

#include <util/command/parserfactory.h>

namespace util
{
	namespace command
	{
		ParserFactory::ParserFactory()
			: float_(nullptr)
			, vec3_(nullptr)
			, staticCamera_(nullptr)
			, withWorldTransform_(nullptr)
		{}

		std::shared_ptr<FloatParser> ParserFactory::floatParser()
		{
			if (float_ == nullptr)
			{
				float_ = std::make_shared<FloatParser>();
			}

			return float_;
		}

		std::shared_ptr<Vec3Parser> ParserFactory::vec3Parser()
		{
			if (vec3_ == nullptr)
			{
				vec3_ = std::make_shared<Vec3Parser>(*floatParser());
			}

			return vec3_;
		}

		std::shared_ptr<StaticCameraParser> ParserFactory::staticCameraParser()
		{
			if (staticCamera_ == nullptr)
			{
				staticCamera_ = std::make_shared<StaticCameraParser>(*vec3Parser());
			}

			return staticCamera_;
		}

		std::shared_ptr<QuaternionParser> ParserFactory::quaternionParser()
		{
			if (quaternion_ == nullptr)
			{
				quaternion_ = std::make_shared<QuaternionParser>(*floatParser(), *vec3Parser());
			}

			return quaternion_;
		}
		
		std::shared_ptr<WithWorldTransformParser> ParserFactory::withWorldTransformParser()
		{
			if (withWorldTransform_ == nullptr)
			{
				withWorldTransform_ = std::make_shared<WithWorldTransformParser>(*vec3Parser(), *quaternionParser());
			}

			return withWorldTransform_;
		}

		std::shared_ptr<DirectionalLightParser> ParserFactory::directionalLightParser()
		{
			if (directionalLight_ == nullptr)
			{
				directionalLight_ = std::make_shared<DirectionalLightParser>(*vec3Parser());
			}

			return directionalLight_;
		}
		std::shared_ptr<WaterFlatSurface> ParserFactory::waterFlatSurfaceParser()
		{
			if (waterFlatSurface_ == nullptr)
			{
				waterFlatSurface_ = std::make_shared<WaterFlatSurface>(*withWorldTransformParser(), *floatParser());
			}

			return waterFlatSurface_;
		}
	}
}