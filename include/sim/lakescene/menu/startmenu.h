#pragma once

/**
* Start Menu
*
* Lake scene start menu that pops up, complete with the model and entity to render said menu.
*/

#include <model/menu/twocolumnmenu.h>
#include <input/menunavigationcontrollerbase.h>
#include <view/texture.h>
#include <view/text/msdftextshader.h>
#include <view/solidshader/solidshader.h>
#include <view/solidshader/genericsolidentity.h>
#include <util/camera/camerabase.h>
#include <model/specialgeo/projection/projectionbase.h>
#include <functional>
#include <model/specialgeo/heightfield.h>
#include <util/math/curve1to1.h>
#include <util/math/linear11.h>

namespace sim
{
	namespace lake
	{
		struct StartMenuConfiguration
		{
			glm::vec2 WorldSize = glm::vec2(3.3f, 2.f);
			glm::vec4 Color = glm::vec4(0.f, 0.f, 0.f, 1.f);
			float SpawnDistanceForward = 6.f;
			float DistanceAboveGround = 0.25f;
			float AnimationDuration = 0.5f;
			std::shared_ptr<util::math::Curve1To1> AnimateInCurve = std::make_shared<util::math::Linear11>(0.f, -1.f, 1.f, 0.f);
			std::shared_ptr<util::math::Curve1To1> AnimateOutCurve = std::make_shared<util::math::Linear11>(0.f, 0.f, 1.f, -1.f);
			float TerrainOffsetY = 0.f;
		};

		class StartMenu
		{
		public:
			StartMenu(
				std::shared_ptr<view::text::MSDFTextShader> textShader,
				std::shared_ptr<view::solidshader::SolidShader> solidShader,
				std::shared_ptr<input::MenuNavigationControllerBase> controller,
				std::shared_ptr<model::specialgeo::Heightfield> heightfield,
				std::function<std::shared_ptr<util::camera::CameraBase>()> cbGetActiveCamera,
				const StartMenuConfiguration& config
			);
			~StartMenu();
			StartMenu(const StartMenu&) = delete;

		public:
			struct ModelPrepareDeferrable
			{
				std::shared_ptr<model::menu::TwoColumnMenu> MenuModel;
				std::shared_ptr<model::menu::SelectOptionMenuItemValue> WaterColoringType;
			};
			struct ListenersPrepareDeferrable
			{
				// std::vector<std::uint32_t> Handles; Handles actually have to be established in a thread-safe way, I'm afraid
			};
			struct PrepareDeferrable
			{
				ModelPrepareDeferrable Model;
				ListenersPrepareDeferrable Listeners;
			};
			std::optional<PrepareDeferrable> prepare();
			bool prepare(const PrepareDeferrable& deferredWork, util::PipelineState& pso);

			void update(float dtd);
			void render(
				std::shared_ptr<util::camera::CameraBase> camera,
				std::shared_ptr<model::specialgeo::ProjectionBase> projection,
				const std::optional<model::geo::Plane>& clipPlane = {}
			);

		protected:
			enum STATE
			{
				SHOWING, HIDING, SHOWN, HIDDEN
			};
			STATE getState() const;

		protected:
			ModelPrepareDeferrable prepareModel();
			bool prepareListeners(const ModelPrepareDeferrable& modelResults);
			bool prepareEntities(util::PipelineState& pso);
			bool releaseHandles();

			//
			// Animation
			//
		protected:
			float getOffsetAtAnimationTime(float animationTime, bool isIntroAnimation) const;

		protected:
			//
			// Configuration
			//
			StartMenuConfiguration configuration_;

			util::Logger log;

			//
			// Control
			//
			std::shared_ptr<input::MenuNavigationControllerBase> controller_;
			std::vector<std::uint32_t> handles_;
			bool isShown_;
			float animationTimeRemaining_;
			std::shared_ptr<model::specialgeo::Heightfield> heightfield_;
			std::function<std::shared_ptr<util::camera::CameraBase>()> cbGetActiveCamera_;

			//
			// Shaders
			//
			std::shared_ptr<view::text::MSDFTextShader> textShader_;
			std::shared_ptr<view::solidshader::SolidShader> solidShader_;

			//
			// Model
			//
			std::shared_ptr<model::menu::TwoColumnMenu> menuModel_;
			std::vector<std::uint32_t> navigationListeners_;
			std::shared_ptr<model::menu::MenuItem<std::string>> waterColoringTypeItem_;
			std::shared_ptr<model::menu::SelectOptionMenuItemValue> waterColoringTypeValue_;

			//
			// Entities
			//
			std::shared_ptr<model::WithWorldTransform> menuTransform_;
			std::shared_ptr<model::WithWorldTransform> menuAnimationTransform_;
			std::shared_ptr<view::solidshader::GenericSolidEntity> menuBackgroundEntity_;
		};
	}
}