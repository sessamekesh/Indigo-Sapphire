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

			// Compute tangents
			std::vector<glm::vec3> tangents;
			tangents.resize(t->mVertCount);
			for (std::uint32_t idx = 0u; idx < t->mFaceCount; idx++)
			{
				auto pv0 = t->mVert[t->mFace[idx].x];
				glm::vec3 v0 = { pv0.x, pv0.y, pv0.z };
				auto pv1 = t->mVert[t->mFace[idx].y];
				glm::vec3 v1 = { pv1.x, pv1.y, pv1.z };
				auto pv2 = t->mVert[t->mFace[idx].z];
				glm::vec3 v2 = { pv2.x, pv2.y, pv2.z };

				auto puv0 = t->mUV[t->mFace[idx].x];
				glm::vec2 uv0 = { puv0.u, puv0.v };
				auto puv1 = t->mUV[t->mFace[idx].y];
				glm::vec2 uv1 = { puv1.u, puv1.v };
				auto puv2 = t->mUV[t->mFace[idx].z];
				glm::vec2 uv2 = { puv2.u, puv2.v };

				glm::vec3 deltaPos1 = v1 - v0;
				glm::vec3 deltaPos2 = v2 - v0;

				glm::vec2 deltaUV1 = uv1 - uv0;
				glm::vec2 deltaUV2 = uv2 - uv0;

				float r = 1.f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
				glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
				tangents[t->mFace[idx].x] += tangent;
				tangents[t->mFace[idx].y] += tangent;
				tangents[t->mFace[idx].z] += tangent;
			}

			std::vector<glm::vec3> twigTangents;
			twigTangents.resize(t->mVertCount);
			for (std::uint32_t idx = 0u; idx < t->mTwigFaceCount; idx++)
			{
				auto pv0 = t->mVert[t->mFace[idx].x];
				glm::vec3 v0 = { pv0.x, pv0.y, pv0.z };
				auto pv1 = t->mVert[t->mFace[idx].y];
				glm::vec3 v1 = { pv1.x, pv1.y, pv1.z };
				auto pv2 = t->mVert[t->mFace[idx].z];
				glm::vec3 v2 = { pv2.x, pv2.y, pv2.z };

				auto puv0 = t->mUV[t->mFace[idx].x];
				glm::vec2 uv0 = { puv0.u, puv0.v };
				auto puv1 = t->mUV[t->mFace[idx].y];
				glm::vec2 uv1 = { puv1.u, puv1.v };
				auto puv2 = t->mUV[t->mFace[idx].z];
				glm::vec2 uv2 = { puv2.u, puv2.v };

				glm::vec3 deltaPos1 = v1 - v0;
				glm::vec3 deltaPos2 = v2 - v0;

				glm::vec2 deltaUV1 = uv1 - uv0;
				glm::vec2 deltaUV2 = uv2 - uv0;

				float r = 1.f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
				glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
				twigTangents[t->mFace[idx].x] += tangent;
				twigTangents[t->mFace[idx].y] += tangent;
				twigTangents[t->mFace[idx].z] += tangent;
			}

			for (std::uint32_t idx = 0u; idx < t->mVertCount; idx++)
			{
				o_Tree.vertices.push_back({
					{ t->mVert[idx].x, t->mVert[idx].y, t->mVert[idx].z }, // Position
					{ t->mNormal[idx].x, t->mNormal[idx].y, t->mNormal[idx].z }, // Normal
					{ t->mUV[idx].u, t->mUV[idx].v }, // UV
					glm::normalize(tangents[idx]) // Tangents
				});
			}

			for (std::uint32_t idx = 0u; idx < t->mTwigVertCount; idx++)
			{
				o_Twigs.vertices.push_back({
					{ t->mTwigVert[idx].x, t->mTwigVert[idx].y, t->mTwigVert[idx].z }, // Position
					{ t->mTwigNormal[idx].x, t->mTwigNormal[idx].y, t->mTwigNormal[idx].z }, // Normal
					{ t->mTwigUV[idx].u, t->mTwigUV[idx].v }, // UV
					glm::normalize(twigTangents[idx]) // Tangent TODO SESS: Expand the tangents
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