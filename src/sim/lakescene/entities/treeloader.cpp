#include <sim/lakescene/entities/treeloader.h>

#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>

namespace sim
{
	namespace lake
	{
		std::optional<std::vector<TreeMesh>> loadFromFile(
			std::string fName,
			util::Logger& log
		) {
			const aiScene* scene = aiImportFile(fName.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
			if (!scene)
			{
				log.error << "Failed to open Assimp scene: " << aiGetErrorString() << util::endl;
				return {};
			}

			std::vector<TreeMesh> meshes;
			for (std::uint32_t meshIdx = 0u; meshIdx < scene->mNumMeshes; meshIdx++)
			{
				auto mesh = scene->mMeshes[meshIdx];

				if (mesh->GetNumUVChannels() < 1u)
				{
					continue;
				}

				std::vector<TreeShader::Vertex> vertices;
				std::vector<std::uint32_t> indices;
				vertices.reserve(mesh->mNumVertices);
				indices.reserve(mesh->mNumFaces * 3u);

				// Load vertices
				auto maxU = 0.f;
				auto maxV = 0.f;
				for (std::uint32_t vertIdx = 0u; vertIdx < mesh->mNumVertices; vertIdx++)
				{
					vertices.push_back(
					{
						{ mesh->mVertices[vertIdx].x, mesh->mVertices[vertIdx].y, mesh->mVertices[vertIdx].z },
						{ mesh->mNormals[vertIdx].x, mesh->mNormals[vertIdx].y, mesh->mNormals[vertIdx].z },
						{ mesh->mTextureCoords[0][vertIdx].x, mesh->mTextureCoords[0][vertIdx].y }
					});
					if (mesh->mTextureCoords[0][vertIdx].x > maxU)
					{
						maxU = mesh->mTextureCoords[0][vertIdx].x;
					}
					if (mesh->mTextureCoords[0][vertIdx].y > maxV)
					{
						maxV = mesh->mTextureCoords[0][vertIdx].y;
					}
				}

				// Load indices
				for (std::uint32_t faceIdx = 0u; faceIdx < mesh->mNumFaces; faceIdx++)
				{
					indices.push_back(mesh->mFaces[faceIdx].mIndices[0u]);
					indices.push_back(mesh->mFaces[faceIdx].mIndices[1u]);
					indices.push_back(mesh->mFaces[faceIdx].mIndices[2u]);
				}

				meshes.push_back({ vertices, indices });
			}

			aiReleaseImport(scene);
			scene = nullptr;

			if (meshes.size() == 0u)
			{
				log.error << "No meshes successfully loaded!" << util::endl;
				return {};
			}
			else
			{
				return meshes;
			}
		}
	}
}