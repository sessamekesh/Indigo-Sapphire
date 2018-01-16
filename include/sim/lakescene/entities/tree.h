#pragma once

#include <sim/lakescene/model/tree.h>
#include <sim/lakescene/shaders/tree.h>

namespace sim
{
	namespace lake
	{
		class TreeEntity
		{
		public:
			TreeEntity(
				TreeModel::TREE_TYPE type
			);
			~TreeEntity();
			TreeEntity(const TreeEntity&) = delete;

			bool prepare(std::shared_ptr<sim::lake::TreeShader> shader, util::PipelineState& pso);
			bool release();

			// This could also be accomplished with indexing in a f'reals game engine
			//  For here, we're just going to swallow the extra draw calls.
			void render(std::shared_ptr<TreeModel> model, std::shared_ptr<sim::lake::TreeShader> shader);

		private:
			bool isReady_;

			TreeModel::TREE_TYPE treeType_;

			GLuint trunkVao_;
			GLuint trunkVB_;
			GLuint trunkIB_;
			std::uint32_t trunkNumIndices_;
			GLuint leavesVao_;
			GLuint leavesVB_;
			GLuint leavesIB_;
			std::uint32_t leavesNumIndices_;
			std::shared_ptr<view::Texture> trunkDiffuseMap_;
			std::shared_ptr<view::Texture> leavesDiffuseMap_;

			util::Logger log;
		};
	}
}