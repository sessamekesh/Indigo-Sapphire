#pragma once

#include <view/text/msdfstringentity.h>
#include <map>
#include <model/specialgeo/projection/projectionbase.h>
#include <util/camera/camerabase.h>
#include <util/Logger.h>

namespace sim
{
	namespace lake
	{
		class DebugText
		{
		public:
			DebugText(std::shared_ptr<view::text::MSDFTextShader> shader);
			~DebugText();
			DebugText(const DebugText&) = delete;

			struct DeferrableWorkItem
			{
				model::ImageData AtlasImage;
			};

			std::optional<DeferrableWorkItem> prepare();
			bool prepare(const std::optional<DeferrableWorkItem>& deferredWork);
			bool release();

			struct DeferrableAddTextItem
			{
				std::string Name;
				glm::vec3 Pos;
				glm::quat Rot;
				view::text::MSDFStringEntity::DeferrableWorkItem MSDFWorkItem;
			};
			DeferrableAddTextItem addDebugText(
				const glm::vec3& startPosition,
				const glm::vec3& up,
				const glm::vec3& fwd,
				const glm::vec2& glyphSize,
				const glm::vec4& color,
				std::string text,
				std::string entityName
			);
			bool addDebugText(const DeferrableAddTextItem& item, util::PipelineState& pso);
			bool removeDebugText(std::string entityName);

			void render(
				std::shared_ptr<util::camera::CameraBase> camera,
				std::shared_ptr<model::specialgeo::ProjectionBase> projection,
				const std::optional<model::geo::Plane>& clipPlane,
				util::PipelineState& pso
			);

		protected:
			std::shared_ptr<view::text::MSDFTextShader> shader_;
			std::shared_ptr<view::Texture> atlasTexture_;
			const model::text::TextAtlas atlasLookup_;
			std::map<std::string, std::shared_ptr<view::text::MSDFStringEntity>> debugText_;
			util::Logger log;
		};
	}
}