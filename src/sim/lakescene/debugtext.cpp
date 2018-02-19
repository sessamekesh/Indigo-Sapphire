#pragma once

#include <sim/lakescene/debugtext.h>
#include <model/imagedata.h>
#include <resources.h>

namespace sim
{
	namespace lake
	{
		DebugText::DebugText(std::shared_ptr<view::text::MSDFTextShader> shader)
			: shader_(shader)
			, atlasTexture_(nullptr)
			, atlasLookup_(8u, 8u, "defghijQklmnopqRrstuvwxSyz01234T56789ABUCDEFGHIVJKLMNOPWXYZabc")
			, debugText_()
			, log(util::DEBUG, util::WARN, "[LakeScene DebugText] ")
		{}

		DebugText::~DebugText()
		{
			release();
		}

		std::optional<DebugText::DeferrableWorkItem> DebugText::prepare()
		{
			if (!shader_)
			{
				log.error << "No shader was provided in construction of the DebugText manager - aborting!" << util::endl;
				return std::nullopt;
			}

			auto atlasImage = model::readPNG(ASSET_PATH("text/OverpassMonoMSDF2.png"));
			if (!atlasImage)
			{
				log.error << "Failed to read in atlas image - aborting!" << util::endl;
				return std::nullopt;
			}

			auto atlasTexture = std::make_shared<view::Texture>();
			if (!atlasTexture || !atlasTexture->init(view::Texture::RGBA, *atlasImage))
			{
				log.error << "Failed to create atlas texture - aborting!" << util::endl;
				atlasTexture = nullptr;
				return std::nullopt;
			}

			return { { atlasTexture } };
		}

		bool DebugText::prepare(const std::optional<DebugText::DeferrableWorkItem>& deferredWork)
		{
			if (!deferredWork)
			{
				log.error << "Deferred work failed - check logs" << util::endl;
				return false;
			}

			atlasTexture_ = deferredWork->AtlasTexture;

			return true;
		}

		bool DebugText::release()
		{
			atlasTexture_ = nullptr;
			debugText_.clear();

			return true;
		}

		DebugText::DeferrableAddTextItem DebugText::addDebugText(
			const glm::vec3& startPosition,
			const glm::vec3& up,
			const glm::vec3& fwd,
			const glm::vec2& glyphSize,
			const glm::vec4& color,
			std::string text,
			std::string entityName
		)
		{
			auto upRotation = glm::rotation(glm::vec3(0.f, 1.f, 0.f), up);
			auto fwdRotation = glm::rotation(glm::vec3(0.f, 0.f, -1.f), fwd);
			auto rot = fwdRotation * upRotation;

			view::text::MSDFStringEntity toAdd(startPosition, rot, glm::vec3(1.f, 1.f, 1.f));
			return {
				entityName,
				startPosition,
				rot,
				toAdd.prepareCPUDeferrable(text, glyphSize.x, glyphSize.y, atlasLookup_, color)
			};
		}

		bool DebugText::addDebugText(const DebugText::DeferrableAddTextItem& item, util::PipelineState& pso)
		{
			// Finish creation of debug text item
			auto toAdd = std::make_shared<view::text::MSDFStringEntity>(item.Pos, item.Rot, glm::vec3(1.f, 1.f, 1.f));
			if (!toAdd->prepareFromDeferrable(item.MSDFWorkItem, shader_, pso, atlasTexture_))
			{
				log.error << "Failed to add debug text " << item.Name << " - failed to create MSDFStringEntity" << util::endl;
				return false;
			}
			else
			{
				debugText_[item.Name] = toAdd;
				return true;
			}
		}

		bool DebugText::removeDebugText(std::string entityName)
		{
			return debugText_.erase(entityName) > 0u;
		}

		void DebugText::render(
			std::shared_ptr<util::camera::CameraBase> camera,
			std::shared_ptr<model::specialgeo::ProjectionBase> projection,
			const std::optional<model::geo::Plane>& clipPlane,
			util::PipelineState& pso
		)
		{
			// Early out in the case of no text present
			if (debugText_.size() == 0u)
			{
				return;
			}

			pso.enableBlending();
			pso.enableDepthTest();
			pso.enableDepthMask();

			if (shader_->activate())
			{
				shader_->setViewMatrix(camera->getViewTransform());
				shader_->setProjMatrix(projection->getProjectionMatrix());
				if (clipPlane)
				{
					shader_->setClipPlane(*clipPlane);
				}

				for (auto&& text : debugText_)
				{
					text.second->render(shader_);
				}
			}
		}
	}
}