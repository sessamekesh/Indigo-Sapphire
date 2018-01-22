#include <view/rawentities/proctree.h>

namespace view
{
	namespace raw
	{
		bool getProcTreeMesh(
			std::shared_ptr<Proctree::Tree> t,
			util::Logger& log,
			view::GenericMesh& o_Tree,
			view::GenericMesh& o_Twigs
		) {
			if (t == nullptr)
			{
				log.error << "Cannot generate mesh - no tree supplied (null pointer)" << util::endl;
				return false;
			}
			
			if (!t->mVert || !t->mNormal || !t->mUV || !t->mTwigVert || !t->mTwigNormal || !t->mTwigUV)
			{
				log.error << "Cannot generate mesh - need to generate proctree first" << util::endl;
				return false;
			}

			o_Tree.vertices.clear();
			o_Tree.vertices.reserve(t->mVertCount);
			o_Tree.indices.clear();
			o_Tree.indices.reserve(t->mFaceCount);

			o_Twigs.vertices.clear();
			o_Twigs.vertices.reserve(t->mVertCount);
			o_Twigs.indices.clear();
			o_Twigs.indices.reserve(t->mFaceCount);

			for (std::uint32_t idx = 0u; idx < t->mVertCount; idx++)
			{
				o_Tree.vertices.push_back({
					{ t->mVert[idx].x, t->mVert[idx].y, t->mVert[idx].z }, // Position
					{ t->mNormal[idx].x, t->mNormal[idx].y, t->mNormal[idx].z }, // Normal
					{ t->mUV[idx].u, t->mUV[idx].v }, // UV
					{ 0.f, 0.f, 1.f } // Tangent TODO SESS: Expand the tangents
				});
			}

			for (std::uint32_t idx = 0u; idx < t->mTwigVertCount; idx++)
			{
				o_Twigs.vertices.push_back({
					{ t->mTwigVert[idx].x, t->mTwigVert[idx].y, t->mTwigVert[idx].z }, // Position
					{ t->mTwigNormal[idx].x, t->mTwigNormal[idx].y, t->mTwigNormal[idx].z }, // Normal
					{ t->mTwigUV[idx].u, t->mTwigUV[idx].v }, // UV
					{ 0.f, 0.f, 1.f } // Tangent TODO SESS: Expand the tangents
				});
			}

			for (std::uint32_t idx = 0u; idx < t->mFaceCount; idx++)
			{
				o_Tree.indices.push_back(t->mFace[idx].x);
				o_Tree.indices.push_back(t->mFace[idx].y);
				o_Tree.indices.push_back(t->mFace[idx].z);
			}

			for (std::uint32_t idx = 0u; idx < t->mTwigFaceCount; idx++)
			{
				o_Twigs.indices.push_back(t->mTwigFace[idx].x);
				o_Twigs.indices.push_back(t->mTwigFace[idx].y);
				o_Twigs.indices.push_back(t->mTwigFace[idx].z);
			}

			return true;
		}
	}
}