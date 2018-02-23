#pragma once

#include <model/menu/verticalsubmenu.h>
#include <view/solidshader/solidshader.h>
#include <view/entity.h>
#include <model/withworldtransform.h>
#include <view/GenericVertex.h>
#include <view/solidshader/genericsolidentity.h>
#include <view/text/msdfstringentity.h>

namespace sim
{
	namespace lake
	{
		namespace entity
		{
			class VerticalSubmenuEntity
				: public view::Entity<view::solidshader::SolidShader, view::solidshader::SolidShaderVertex>
				, public model::WithWorldTransform
			{
			public:
				VerticalSubmenuEntity(
					model::menu::VerticalSubmenu model,
					const glm::vec4& inactiveBackgroundColor,
					const glm::vec4& selectedBackgroundColor,
					const glm::vec4& disabledBackgroundColor,
					const glm::vec4& inactiveTextColor,
					const glm::vec4& selectedTextColor,
					const glm::vec4& disabledTextColor,
					const glm::vec2& overMenuWorldDimensions,
					const glm::vec2& overMenuLogicalSize
				);
				~VerticalSubmenuEntity();
				VerticalSubmenuEntity(const VerticalSubmenuEntity&) = default;

				void setScroll(float logicalScrollAmt);

			public:
				bool prepare(
					std::shared_ptr<view::solidshader::SolidShader> solidShader,
					std::shared_ptr<view::text::MSDFTextShader> textShader,
					util::PipelineState& pso,
					const model::text::TextAtlas& atlas,
					std::shared_ptr<model::WithWorldTransform> menuTransform,
					std::shared_ptr<view::Texture> textAtlasTexture
				);
				bool release();
				void renderBackground(std::shared_ptr<view::solidshader::SolidShader> solidShader);
				void renderText(std::shared_ptr<view::text::MSDFTextShader> textShader);

			private:
				glm::vec2 modelToWorldLocation(const glm::vec2& modelPoint) const;
				glm::vec2 modelToWorldSize(const glm::vec2& size) const;

			private:
				model::menu::VerticalSubmenu model_;
				glm::vec4 inactiveBackgroundColor_;
				glm::vec4 selectedBackgroundColor_;
				glm::vec4 disabledBackgroundColor_;
				glm::vec4 inactiveTextColor_;
				glm::vec4 selectedTextColor_;
				glm::vec4 disabledTextColor_;

				std::shared_ptr<model::WithWorldTransform> scrollTransform_;
				std::shared_ptr<model::WithWorldTransform> submenuTransform_;
				glm::vec2 worldDimensions_;
				glm::vec2 worldMenuLogicalSize_;

				struct MenuItemEntity
				{
					std::shared_ptr<view::solidshader::GenericSolidEntity> BackgroundEntity;
					std::shared_ptr<view::text::MSDFStringEntity> TextEntity;
				};
				// TODO SESS: Consider this further. Do you really want this mapping? Or should you just do a list?
				std::map<std::string, MenuItemEntity> inactiveEntities_;
				std::map<std::string, MenuItemEntity> disabledEntities_;
				std::map<std::string, MenuItemEntity> selectedEntities_;

				util::Logger log;
			};
		}
	}
}