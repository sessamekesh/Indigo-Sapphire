#include <sim/lakescene/entities/tree.h>
#include <sim/lakescene/entities/treeloader.h>
#include <resources.h>

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

		bool TreeEntity::prepare(std::shared_ptr<TreeShader> shader, util::PipelineState& pso)
		{
			if (isReady_)
			{
				release();
			}

			std::optional<TreeData> tree = {};

			switch (treeType_)
			{
			case TreeModel::TREE_TYPE_0:
				tree = TREE_0;
			}

			if (!tree)
			{
				log.error << "Failed to load tree - no tree file name found (programmer was an idiot if this is ever reached)" << util::endl;
				return false;
			}

			auto treeMeshOpt = loadFromFile(tree->modelFilename, log);
			if (!treeMeshOpt)
			{
				log.error << "Failed to load tree - could not parse from file" << util::endl;
				return false;
			}

			auto trunkDiffuseTexture = std::make_shared<view::Texture>();
			auto trunkImage = model::readPNG(tree->trunkDiffuseFilename);
			if (!trunkImage)
			{
				log.error << "Failed to load trunk diffuse map image" << util::endl;
				return false;
			}
			if (!trunkDiffuseTexture->init(view::Texture::RGBA, *trunkImage))
			{
				log.error << "Failed to initialize trunk diffuse map texture" << util::endl;
				return false;
			}
			trunkDiffuseMap_ = trunkDiffuseTexture;

			auto leavesDiffuseTexture = std::make_shared<view::Texture>();
			auto leavesImage = model::readPNG(tree->leavesDiffuseFilename);
			if (!leavesImage)
			{
				log.error << "Failed to load leaves diffuse map image" << util::endl;
				return false;
			}
			if (!leavesDiffuseTexture->init(view::Texture::RGBA, *leavesImage))
			{
				log.error << "Failed to initialize leaves diffuse map texture" << util::endl;
				return false;
			}
			leavesDiffuseMap_ = leavesDiffuseTexture;

			auto treeMeshes = *treeMeshOpt;

			if (treeMeshes.size() < 3u)
			{
				log.error << "Failed to load tree - not enough meshes present" << util::endl;
				return false;
			}

			if (treeMeshes.size() > 3u)
			{
				log.warn << "Too many meshes present in tree - loading, but behavior may be unexpected" << util::endl;
			}

			// Trunk
			glCreateVertexArrays(1, &trunkVao_);
			glBindVertexArray(trunkVao_);

			glGenBuffers(1, &trunkVB_);
			glBindBuffer(GL_ARRAY_BUFFER, trunkVB_);
			glBufferStorage(
				GL_ARRAY_BUFFER,
				treeMeshes[1].vertices.size() * sizeof(TreeShader::Vertex),
				&treeMeshes[1].vertices[0],
				0x00
			);
			glGenBuffers(1, &trunkIB_);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, trunkIB_);
			glBufferStorage(
				GL_ELEMENT_ARRAY_BUFFER,
				treeMeshes[1].indices.size() * sizeof(std::uint32_t),
				&treeMeshes[1].indices[0],
				0x00
			);
			trunkNumIndices_ = treeMeshes[1].indices.size();
			shader->setVertexAttribPointers(pso);

			// Leaves
			glCreateVertexArrays(1, &leavesVao_);
			glBindVertexArray(leavesVao_);

			glGenBuffers(1, &leavesVB_);
			glBindBuffer(GL_ARRAY_BUFFER, leavesVB_);
			glBufferStorage(
				GL_ARRAY_BUFFER,
				treeMeshes[2].vertices.size() * sizeof(TreeShader::Vertex),
				&treeMeshes[2].vertices[0],
				0x00
			);
			glGenBuffers(1, &leavesIB_);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, leavesIB_);
			glBufferStorage(
				GL_ELEMENT_ARRAY_BUFFER,
				treeMeshes[2].indices.size() * sizeof(std::uint32_t),
				&treeMeshes[2].indices[0],
				0x00
			);
			leavesNumIndices_ = treeMeshes[2].indices.size();
			shader->setVertexAttribPointers(pso);

			isReady_ = true;
			return true;
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