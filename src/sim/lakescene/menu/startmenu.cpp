#pragma once

#include <sim/lakescene/menu/startmenu.h>
#include <view/rawentities/planegenerator.h>

namespace sim
{
	namespace lake
	{
		const glm::vec3 DefaultFacingDirection(0.f, 0.f, 1.f);
		const glm::vec2 LogicalMenuSize(800.f, 600.f);
		const float HeaderHeight = 20.f;
		const float FooterHeight = 20.f;
		const float LeftColumnWidth = 350.f;
		const float SubmenuEntryHeight = 40.f;
		const float MenuItemHeight = 50.f;

		StartMenu::StartMenu(
			std::shared_ptr<view::text::MSDFTextShader> textShader,
			std::shared_ptr<view::solidshader::SolidShader> solidShader,
			std::shared_ptr<input::MenuNavigationControllerBase> controller,
			std::shared_ptr<model::specialgeo::Heightfield> heightfield,
			std::function<std::shared_ptr<util::camera::CameraBase>()> cbGetActiveCamera,
			const StartMenuConfiguration& config
		)
			: controller_(controller)
			, handles_(0u)
			, isShown_(false)
			, animationTimeRemaining_(0.f)
			, heightfield_(heightfield)
			, cbGetActiveCamera_(cbGetActiveCamera)
			, textShader_(textShader)
			, solidShader_(solidShader)
			, menuModel_(nullptr)
			, navigationListeners_({})
			, menuTransform_(nullptr)
			, menuAnimationTransform_(nullptr)
			, menuBackgroundEntity_(nullptr)
			, configuration_(config)
			, log(util::DEBUG, util::DEBUG, "[StartMenu] ")
			, verticalSubmenuEntity_(nullptr)
		{}

		StartMenu::~StartMenu()
		{
			releaseHandles();
		}

		std::optional<StartMenu::PrepareDeferrable> StartMenu::prepare()
		{
			auto model = prepareModel();

			return { { model } };
		}

		bool StartMenu::prepare(const model::text::TextAtlas& atlasData, std::shared_ptr<view::Texture> textTexture, const StartMenu::PrepareDeferrable& deferredWork, util::PipelineState& pso)
		{
			auto model = deferredWork.Model.MenuModel;
			menuModel_ = model;

			if (!prepareListeners(deferredWork.Model))
			{
				return false;
			}

			if (!prepareEntities(atlasData, textTexture, pso))
			{
				return false;
			}

			return true;

			// TODO KAM: Continue here, actually have an entity that represents the menu

			// TODO KAM: Also, you need to have a state - "SHOWING", "HIDING", "SHOWN", "HIDDEN"
			//  And a GamepadButtonEventConsumer that listends for start button presses to go between
			//  these states.
			// And a GamepadButtonEventSource that is also a GamepadButtonEventConsumer - it passes along
			//  events to the menu ONLY when the menu is in "SHOWN" state
			// The "SHOWING" and "HIDDEN" states themselves need to be stateful, to store how shown/hidden the menu is.
			//  I'm thinking the menu will just pop up from the ground wherever it is spawned?
		}

		StartMenu::ModelPrepareDeferrable StartMenu::prepareModel()
		{
			StartMenu::ModelPrepareDeferrable tr;

			tr.MenuModel = std::make_shared<model::menu::TwoColumnMenu>(
				LogicalMenuSize,
				HeaderHeight, FooterHeight,
				LeftColumnWidth,
				SubmenuEntryHeight
			);

			// Create vertical menu entries
			{
				model::menu::VerticalInnerMenu waterMenu(MenuItemHeight);
				model::menu::VerticalInnerMenu grassMenu(MenuItemHeight);

				{
					model::menu::MenuItem<std::string> mi;
					mi.State = model::menu::MenuItem<std::string>::INACTIVE;
					mi.Name = "water-ColoringType";
					mi.Label = "Coloring Type";
					tr.WaterColoringType = std::shared_ptr<model::menu::SelectOptionMenuItemValue>(new model::menu::SelectOptionMenuItemValue({
						"SolidColor", "ReflectionOnly", "RefractionOnly", "All"
					}, "All"));
					mi.MenuItemValue = tr.WaterColoringType;
					waterMenu.addMenuItem(mi);

					// TODO SESS: Continue here, by adding functionality to notice when the menu item is active,
					//  and allow the user to change its value. This will require (1) a GamepadButtonEventSource,
					//  (2) a GamepadButtonEventConsumer, (3) a way to activate/disengage said consumer,
					//  (4) a way for that consumer to update the SelectOptionMenuItemValue (this will probably involve
					//  holding a reference outside of this block), (5) a way to render this dumb thing...
				}

				tr.MenuModel->addSubmenu("water", "Water Options", waterMenu);
				tr.MenuModel->addSubmenu("grass", "Grass Options", grassMenu);
			}

			// Create geometry
			tr.LeftMenuItemMesh = std::make_shared<view::GenericMesh>(view::raw::PlaneGenerator::getGenericMeshData(
				model::geo::Plane(glm::vec3(0.f, 0.f, 0.f), DefaultFacingDirection),
				glm::vec3(0.f, 1.f, 0.f),
				glm::vec2(0.9f * LeftColumnWidth / LogicalMenuSize.x, 0.9f * SubmenuEntryHeight / LogicalMenuSize.y)
			));

			return tr;
		}

		float StartMenu::getOffsetAtAnimationTime(float animationTime, bool isIntroAnimation) const
		{
			auto t = glm::min(1.f, glm::max(0.f, animationTime / configuration_.AnimationDuration));

			if (isIntroAnimation)
			{
				return configuration_.AnimateInCurve->evaluateAt(t) * (configuration_.WorldSize.y + configuration_.DistanceAboveGround);
			}
			else
			{
				return configuration_.AnimateOutCurve->evaluateAt(t) * (configuration_.WorldSize.y + configuration_.DistanceAboveGround);
			}
		}

		bool StartMenu::prepareListeners(const StartMenu::ModelPrepareDeferrable& modelResults)
		{
			handles_.reserve(16u);
			releaseHandles();

			handles_.push_back(controller_->addUpButtonListener([this]() {
				auto state = getState();
				if (state == StartMenu::SHOWN)
				{
					menuModel_->moveSelectionUp();
				}
			}));

			handles_.push_back(controller_->addDownButtonListener([this]() {
				auto state = getState();
				if (state == StartMenu::SHOWN)
				{
					menuModel_->moveSelectionDown();
				}
			}));

			handles_.push_back(controller_->addToggleMenuButtonListener([this]() {
				log.debug << "Toggle menu button received!" << util::endl;

				auto state = getState();
				if (state == StartMenu::SHOWN)
				{
					isShown_ = !isShown_;
					animationTimeRemaining_ = configuration_.AnimationDuration;
					log.debug << "Current state: SHOWN, transitioning to HIDING" << util::endl;
				}
				else if (state == StartMenu::HIDDEN)
				{
					isShown_ = !isShown_;
					animationTimeRemaining_ = configuration_.AnimationDuration;

					// There is an underlying assumption in this engine that the camera will never be pointed directly
					//  up or down, and that directly up means [0, 1, 0]
					// If this assumption is broken, this logic will also break.
					auto camera = cbGetActiveCamera_();
					auto cameraPos = camera->pos();
					auto nCameraFwd = glm::normalize(camera->lookAt() - cameraPos);
					auto nCameraDirectlyFwd = glm::normalize(glm::vec3(nCameraFwd.x, 0.f, nCameraFwd.z));

					auto rotation = glm::rotation(DefaultFacingDirection, -nCameraDirectlyFwd);

					auto spawnBasePoint = cameraPos + nCameraDirectlyFwd * configuration_.SpawnDistanceForward;
					spawnBasePoint.y = heightfield_->heightAtPos(spawnBasePoint.x, spawnBasePoint.z, 0.f, true) + configuration_.WorldSize.y * 0.5f + configuration_.DistanceAboveGround;
					spawnBasePoint.y += configuration_.TerrainOffsetY;

					menuTransform_->pos(spawnBasePoint);
					menuTransform_->rot(rotation);

					float yOffsetAnimationStart = getOffsetAtAnimationTime(0.f, true);
					menuAnimationTransform_->pos(glm::vec3(0.f, yOffsetAnimationStart, 0.f));
					animationTimeRemaining_ = configuration_.AnimationDuration;

					log.debug << "Current state: HIDDEN, transitioning to SHOWING" << util::endl;
				}
			}));

			handles_.push_back(controller_->addSelectButtonListener([this]() {
				auto state = getState();
				if (state == StartMenu::SHOWN)
				{
					menuModel_->select();

					if (waterColoringTypeItem_->State == model::menu::MenuItem<std::string>::SELECTED)
					{
						// TODO SESS: The logic for this is starting to get complex, with things moving outside of the model.
						// Re-evaluate how to do this, and resume.
					}
				}
			}));

			handles_.push_back(controller_->addDeselectButtonListener([this]() {
				auto state = getState();
				if (state == StartMenu::SHOWN)
				{
					menuModel_->deselect();

					// TODO SESS: Resume logic here
				}
			}));

			return true;
		}

		bool StartMenu::prepareEntities(const model::text::TextAtlas& atlasData, std::shared_ptr<view::Texture> textTexture, util::PipelineState& pso)
		{
			menuTransform_ = std::make_shared<model::WithWorldTransform>(glm::vec3(0.f, 0.f, 0.f), glm::angleAxis(0.f, glm::vec3(0.f, 1.f, 0.f)), glm::vec3(1.f, 1.f, 1.f));
			menuAnimationTransform_ = std::make_shared<model::WithWorldTransform>(glm::vec3(0.f, 0.f, 0.f), glm::angleAxis(0.f, glm::vec3(0.f, 1.f, 0.f)), glm::vec3(1.f, 1.f, 1.f));
			menuTransform_->setParent(menuAnimationTransform_);

			menuBackgroundEntity_ = std::make_shared<view::solidshader::GenericSolidEntity>(glm::vec3(0.f, 0.f, 0.f), glm::angleAxis(0.f, glm::vec3(0.f, 1.f, 0.f)), glm::vec3(1.f, 1.f, 1.f));
			menuBackgroundEntity_->addMesh(std::make_shared<view::GenericMesh>(view::raw::PlaneGenerator::getGenericMeshData(
				model::geo::Plane(glm::vec3(0.f, 0.f, 0.f), DefaultFacingDirection),
				glm::vec3(0.f, 1.f, 0.f),
				configuration_.WorldSize // Size of th menu
			)), configuration_.Color);
			menuBackgroundEntity_->setParent(menuTransform_);

			if (!menuBackgroundEntity_->prepare(solidShader_, pso))
			{
				return false;
			}

			auto submenu = menuModel_->getSubmenu();
			verticalSubmenuEntity_ = std::make_shared<sim::lake::entity::VerticalSubmenuEntity>(
				submenu,
				configuration_.InactiveBackgroundColor,
				configuration_.SelectedBackgroundColor,
				configuration_.DisabledBackgroundColor,
				configuration_.InactiveTextColor,
				configuration_.SelectedTextColor,
				configuration_.DisabledTextColor,
				configuration_.WorldSize,
				LogicalMenuSize
			);
			if (!verticalSubmenuEntity_->prepare(solidShader_, textShader_, pso, atlasData, menuTransform_, textTexture))
			{
				return false;
			}

			return true;
		}

		bool StartMenu::releaseHandles()
		{
			for (auto&& h : handles_)
			{
				controller_->removeListener(h);
			}
			handles_.clear();

			return true;
		}

		void StartMenu::update(float dt)
		{
			if (animationTimeRemaining_ > 0.f)
			{
				auto state = getState();
				std::string stateString = "UNKNOWN";
				switch (state)
				{
				case StartMenu::SHOWING: stateString = "SHOWING"; break;
				case StartMenu::HIDING: stateString = "HIDING"; break;
				case StartMenu::SHOWN: stateString = "SHOWN"; break;
				case StartMenu::HIDDEN: stateString = "HIDDEN"; break;
				}
				animationTimeRemaining_ -= dt;
				auto yOffset = getOffsetAtAnimationTime(configuration_.AnimationDuration - animationTimeRemaining_, state == StartMenu::SHOWING);
				menuAnimationTransform_->pos(glm::vec3(0.f, yOffset, 0.f));
			}
		}

		void StartMenu::render(
			std::shared_ptr<util::camera::CameraBase> camera,
			std::shared_ptr<model::specialgeo::ProjectionBase> projection,
			const std::optional<model::geo::Plane>& clipPlane
		)
		{
			auto state = getState();
			if (state == StartMenu::HIDDEN)
			{
				// Don't show at all if we're hidden!
				return;
			}
			else
			{
				// Shown state
				// TODO SESS: Debug this, for some reason nothing is showing up still
				if (solidShader_->activate())
				{
					if (clipPlane)
					{
						solidShader_->setClipPlane(*clipPlane);
					}

					solidShader_->setViewMatrix(camera->getViewTransform());
					solidShader_->setProjMatrix(projection->getProjectionMatrix());
					
					menuBackgroundEntity_->render(solidShader_);
					verticalSubmenuEntity_->renderBackground(solidShader_);
				}

				if (textShader_->activate())
				{
					if (clipPlane)
					{
						textShader_->setClipPlane(*clipPlane);
					}
					
					textShader_->setViewMatrix(camera->getViewTransform());
					textShader_->setProjMatrix(projection->getProjectionMatrix());

					verticalSubmenuEntity_->renderText(textShader_);
				}
			}
		}

		StartMenu::STATE StartMenu::getState() const
		{
			if (isShown_)
			{
				if (animationTimeRemaining_ > 0.f)
				{
					return StartMenu::SHOWING;
				}
				else
				{
					return StartMenu::SHOWN;
				}
			}
			else
			{
				if (animationTimeRemaining_ > 0.f)
				{
					return StartMenu::HIDING;
				}
				else
				{
					return StartMenu::HIDDEN;
				}
			}
		}
	}
}