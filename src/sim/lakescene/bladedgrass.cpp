#include <sim/lakescene/bladedgrass.h>
#include <util/threadpool.h>
#include <glm/gtc/quaternion.hpp>
#include <resources.h>
#include <model/geo/interactions.h>
#include <util/math/quadraticeaseout11.h>

namespace sim
{
	namespace lake
	{
		BladedGrassConfig BladedGrass::STANDARD_COMPILED_CONFIG = {
			25.f,
			1000.f,
			0.005f, 0.018f,
			0.05f, 0.12f,
			0.f, 3.14f,
			0.01f, 0.15f,
			0.3f, 0.45f,
			glm::vec4(0.2f, 0.2f, 0.2f, 1.f),
			0.03f, 1u
		};

		BladedGrass::BladedGrass(
			std::shared_ptr<model::specialgeo::Heightfield> heightfield,
			std::shared_ptr<util::SurfaceProbabilityFieldBase> probabilityField,
			std::shared_ptr<util::SurfaceMaskBase> surfaceMask,
			glm::vec3 terrainBaseTransform,
			const BladedGrassConfig& config
		)
			: isReady_(false)
			, heightfield_(heightfield)
			, probabilityField_(probabilityField)
			, surfaceMask_(surfaceMask)
			, grassTexture_(nullptr)
			, numBladesLodCurve_(nullptr)
			, terrainBaseTransform_(terrainBaseTransform)
			, config_(config)
			, bladedGrassShader_(nullptr)
			, grassEntities_({})
			, log(util::DEBUG, util::WARN, "[LakeScene::BladedGrass ]")
			, numBladesInScene_(0u)
			, numBladesInView_(0u)
			, numBladesRendered_(0u)
		{}

		BladedGrass::~BladedGrass()
		{
			if (isReady_) release();
		}

		bool BladedGrass::prepare(const glm::vec2& startCorner, const glm::vec2& endCorner, util::PipelineState& pso, util::ThreadPool& threadPool)
		{
			if (isReady_)
			{
				log.warn << "Bladed grass entity is already prepared - destroying first, but this most likely indicates a programming error" << util::endl;
				release();
			}

			bladedGrassShader_ = std::make_shared<view::grass::BladedGrassPatchShader>();
			if (!bladedGrassShader_ || !bladedGrassShader_->initialize())
			{
				return false;
			}

			//
			// Start grass texture loading
			//
			auto path = ASSET_PATH("texture/grassBlade.png");
			auto grassImage = threadPool.enqueue([path]() {
				return model::readPNG(path);
			});

			//
			// Create LOD curve (while texture runs, but before entities have started loading)
			//
			numBladesLodCurve_ = std::make_shared<util::math::QuadraticEaseOut11>(8.f, 25.f);

			//
			// Create entities (but do not prepare them yet)
			//
			std::vector<std::future<std::pair<unsigned int, std::optional<view::grass::BladedGrassPatchEntity::CPUDeferrableWork>>>> deferrableWork;
			unsigned int numPatches = (unsigned int)((endCorner.x - startCorner.x) / config_.maxPatchRadius + 1.f)
				* (unsigned int)((endCorner.y - startCorner.y) / config_.maxPatchRadius + 1.f);
			deferrableWork.reserve(numPatches);
			grassEntities_.reserve(numPatches);

			for (float xStart = startCorner.x; xStart < endCorner.x; xStart = glm::min(endCorner.x, xStart + config_.maxPatchRadius))
			{
				for (float zStart = startCorner.y; zStart < endCorner.y; zStart = glm::min(endCorner.y, zStart + config_.maxPatchRadius))
				{
					grassEntities_.push_back(
						std::shared_ptr<view::grass::BladedGrassPatchEntity>(new view::grass::BladedGrassPatchEntity(
							terrainBaseTransform_, glm::angleAxis(0.f, glm::vec3(0.f, 1.f, 0.f)), glm::vec3(1.f, 1.f, 1.f)
						))
					);
				}
			}

			//
			// Asynchronously (using thread pool) start work for preparing the patches
			//
			{
				unsigned int i = 0u;
				for (float xStart = startCorner.x; xStart < endCorner.x; xStart = glm::min(endCorner.x, xStart + config_.maxPatchRadius))
				{
					for (float zStart = startCorner.y; zStart < endCorner.y; zStart = glm::min(endCorner.y, zStart + config_.maxPatchRadius))
					{
						float width = glm::min(endCorner.x, xStart + config_.maxPatchRadius) - xStart;
						float height = glm::min(endCorner.y, zStart + config_.maxPatchRadius) - zStart;
						const auto idx = i++;
						auto cpudw = grassEntities_[idx]->prepareCPUDeferrable(
							heightfield_, probabilityField_, surfaceMask_,
							glm::vec2(xStart, zStart), glm::vec2(xStart + width, zStart + height),
							config_.maxBladeDensityPerSquareMeter * width * height,
							config_.minBladeBaseWidth, config_.maxBladeBaseWidth,
							config_.minBladeHeight, config_.maxBladeHeight,
							config_.minBladeRotation, config_.maxBladeRotation,
							config_.minBladeTwistRate, config_.maxBladeTwistRate,
							config_.minBladeTaperRate, config_.maxBladeTaperRate,
							config_.randomSeed
						);
						const auto ge = grassEntities_[idx];
						auto startCorner = glm::vec2(xStart, zStart);
						auto endCorner = glm::vec2(xStart + width, zStart + height);
						auto f = [idx, ge, this, startCorner, endCorner, width, height]() -> std::pair<unsigned int, std::optional<view::grass::BladedGrassPatchEntity::CPUDeferrableWork>> { return { idx, ge->prepareCPUDeferrable(
							heightfield_, probabilityField_, surfaceMask_,
							startCorner, endCorner,
							config_.maxBladeDensityPerSquareMeter * width * height,
							config_.minBladeBaseWidth, config_.maxBladeBaseWidth,
							config_.minBladeHeight, config_.maxBladeHeight,
							config_.minBladeRotation, config_.maxBladeRotation,
							config_.minBladeTwistRate, config_.maxBladeTwistRate,
							config_.minBladeTaperRate, config_.maxBladeTaperRate,
							config_.randomSeed
							) };
						};
						deferrableWork.push_back(threadPool.enqueue(f));
					}
				}
			}

			bool wasSuccessful = true;
			auto imgOpt = grassImage.get();
			if (imgOpt)
			{
				grassTexture_ = std::make_shared<view::Texture>();
				wasSuccessful &= grassTexture_->init(view::Texture::TEXTURE_TYPE::RGBA, *imgOpt);
			}
			else
			{
				log.error << "Failed to load grass texture!" << util::endl;
				wasSuccessful = false;
			}

			//
			// Scan for finished work, finish preparing those entities
			//
			std::vector<std::uint32_t> unpreparedIndices;
			for (auto&& workItem : deferrableWork)
			{
				auto res = workItem.get();
				auto idx = res.first;
				auto val = res.second;

				if (val)
				{
					if (!grassEntities_[idx]->prepareFromDeferrable(*val, bladedGrassShader_, pso, grassTexture_, config_.specularColor, config_.windStrength))
					{
						unpreparedIndices.push_back(idx);
					}
				}
				else
				{
					log.error << "Failed to prepare a grass entity - index " << idx << util::endl;
				}
			}
			deferrableWork.resize(0u);

			std::sort(unpreparedIndices.begin(), unpreparedIndices.end());
			for (std::int32_t idx = unpreparedIndices.size() - 1; idx > 0; idx--)
			{
				grassEntities_.erase(grassEntities_.begin() + unpreparedIndices[idx]);
			}

			isReady_ = true;

			if (!wasSuccessful)
			{
				release();
			}

			return wasSuccessful;
		}

		bool BladedGrass::release()
		{
			bool wasAllSuccessful = true;

			for (auto&& e : grassEntities_)
			{
				wasAllSuccessful &= e->release();
			}

			grassEntities_.resize(0u);

			if (grassTexture_)
			{
				wasAllSuccessful &= grassTexture_->release();
			}

			grassTexture_ = nullptr;

			return wasAllSuccessful;
		}

		void BladedGrass::getFrameRenderingData(
			std::uint32_t& o_numBladesInScene,
			std::uint32_t& o_numBladesInView,
			std::uint32_t& o_numBladesRendered
		) {
			o_numBladesInScene = numBladesInScene_;
			o_numBladesInView = numBladesInView_;
			o_numBladesRendered = numBladesRendered_;
		}

		void BladedGrass::update(float dt)
		{
			numBladesInScene_ = numBladesInView_ = numBladesRendered_ = 0.f;

			for (auto&& e : grassEntities_)
			{
				e->update(dt);
			}
		}

		void BladedGrass::render(
			std::shared_ptr<util::camera::CameraBase> camera,
			std::shared_ptr<model::specialgeo::ProjectionBase> projection,
			const model::light::DirectionalLight& sunlight,
			std::optional<model::geo::Plane> clipPlane
		) {
			auto frustum = projection->getBoundingFrustum(camera);

			if (bladedGrassShader_->activate())
			{
				bladedGrassShader_->setViewMatrix(camera->getViewTransform());
				bladedGrassShader_->setCameraPosition(camera->pos());
				bladedGrassShader_->setProjMatrix(projection->getProjectionMatrix());
				bladedGrassShader_->setLight(sunlight);

				if (clipPlane)
				{
					bladedGrassShader_->setClipPlane(*clipPlane);
				}
				
				for (auto&& grassEntity : grassEntities_)
				{
					auto numBlades = grassEntity->numBlades();
					
					bool showThisOne = true;
					auto sphere = grassEntity->getBoundingSphere();
					for (auto&& plane : frustum)
					{
						showThisOne &= model::geo::isSphereAtAllInPlane(plane, sphere);
					}

					numBladesInScene_ += numBlades;

					if (showThisOne)
					{
						float renderProportion = numBladesLodCurve_->evaluateAt(
							glm::length2(grassEntity->pos() - camera->pos())
						);
						numBladesInView_ += numBlades;
						numBladesRendered_ += numBlades * renderProportion;

						grassEntity->render(bladedGrassShader_, renderProportion);
					}
				}
			}
		}
	}
}