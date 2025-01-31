#include <sim/lakescene/entities/tree.h>
#include <sim/lakescene/entities/treeloader.h>
#include <resources.h>
#include <view/genericassmiploader.h>

namespace sim
{
	namespace lake
	{
		struct TreeData
		{
			std::string modelFilename;
			std::string trunkDiffuseFilename;
			std::string leavesDiffuseFilename;
		};

		const TreeData TREE_0 = {
			ASSET_PATH("environment/trees/pine0/first.dae"),
			ASSET_PATH("environment/trees/pine0/Red_Pine_Bark_diffuse.png"),
			ASSET_PATH("environment/trees/pine0/Pine_Large_diffuse.PNG")
		};

		TreeEntity::TreeEntity(
			TreeModel::TREE_TYPE type
		)
			: isReady_(false)
			, treeType_(type)
			, trunkVao_(0u)
			, leavesVao_(0u)
			, trunkDiffuseMap_(nullptr)
			, leavesDiffuseMap_(nullptr)
			, trunkNumIndices_(0u)
			, leavesNumIndices_(0u)
			, log(util::DEBUG, util::WARN, "[TreeEntity ]")
		{}

		TreeEntity::~TreeEntity()
		{
			release();
		}

		bool TreeEntity::prepare(std::shared_ptr<sim::lake::TreeShader> shader, util::PipelineState& pso)
		{
			if (isReady_)
			{
				release();
			}

			// TODO SESS: You should use an ExternalFileCache, with promises instead that you can get results from.
			auto treeRawEntity = view::loadFromScene(ASSET_PATH("environment/trees/pine0/first.dae"), log);

			if (treeRawEntity == nullptr)
			{
				return false;
			}

			// TODO SESS: This is wrong, and specific to individual trees
			if (treeRawEntity->size() < 2u)
			{
				log.error << "Failed to load tree - not enough meshes present" << util::endl;
				return false;
			}

			if (treeRawEntity->size() > 2u)
			{
				log.warn << "Too many meshes present in tree - loading, but behavior may be unexpected" << util::endl;
			}

			// Trunk
			{
				auto trunkRawMesh = (*treeRawEntity)["branches23"];
				if (!prepareInternal(
					sim::lake::TreeShader::processGenericVertices(trunkRawMesh.vertices),
					trunkRawMesh.indices, shader, pso,
					trunkVao_, trunkVB_, trunkIB_, trunkNumIndices_
				)) {
					log.error << "Failed to prepare trunk mesh" << util::endl;
					return false;
				}
			}

			// Leaves
			{
				auto leavesRawMesh = (*treeRawEntity)["leaf21"];
				if (!prepareInternal(
					sim::lake::TreeShader::processGenericVertices(leavesRawMesh.vertices),
					leavesRawMesh.indices, shader, pso,
					leavesVao_, leavesVB_, leavesIB_, leavesNumIndices_
				)) {
					log.error << "Failed to prepare trunk mesh" << util::endl;
					return false;
				}
			}

			isReady_ = true;
			return true;
		}

		void TreeEntity::setTrunkDiffuseMap(std::shared_ptr<view::Texture> diffuseMap)
		{
			trunkDiffuseMap_ = diffuseMap;
		}

		void TreeEntity::setLeavesDiffuseMap(std::shared_ptr<view::Texture> diffuseMap)
		{
			leavesDiffuseMap_ = diffuseMap;
		}

		bool TreeEntity::release()
		{
			isReady_ = false;
			if (leavesIB_)
			{
				glDeleteBuffers(1, &leavesIB_);
				leavesIB_ = 0u;
			}
			if (leavesVB_)
			{
				glDeleteBuffers(1, &leavesVB_);
				leavesVB_ = 0u;
			}
			if (leavesVao_)
			{
				glDeleteVertexArrays(1, &leavesVao_);
				leavesVao_ = 0u;
			}

			if (trunkIB_)
			{
				glDeleteBuffers(1, &trunkIB_);
				trunkIB_ = 0u;
			}
			if (trunkVB_)
			{
				glDeleteBuffers(1, &trunkVB_);
				trunkVB_ = 0u;
			}
			if (trunkVao_)
			{
				glDeleteVertexArrays(1, &trunkVao_);
				trunkVao_ = 0u;
			}

			trunkDiffuseMap_ = nullptr;
			leavesDiffuseMap_ = nullptr;

			return true;
		}

		void TreeEntity::render(std::shared_ptr<TreeModel> model, std::shared_ptr<TreeShader> shader)
		{
			if (model->type() != treeType_)
			{
				log.warn << "Will not render the wrong type of tree" << util::endl;
			}

			shader->setWorldMatrix(model->worldTransform());
			shader->setDiffuseMap(trunkDiffuseMap_);
			glBindVertexArray(trunkVao_);
			glDrawElements(GL_TRIANGLES, trunkNumIndices_, GL_UNSIGNED_INT, NULL);

			shader->setDiffuseMap(leavesDiffuseMap_);
			glBindVertexArray(leavesVao_);
			glDrawElements(GL_TRIANGLES, leavesNumIndices_, GL_UNSIGNED_INT, NULL);
		}
	}
}