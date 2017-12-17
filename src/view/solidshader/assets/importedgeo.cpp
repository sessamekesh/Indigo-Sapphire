#include <view/solidshader/assets/importedgeo.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/material.h>

namespace view
{
	namespace solidshader
	{
		namespace assets
		{
			std::optional<std::map<std::string, BasicGeometry>> importFromFile(
				const std::string& file,
				const glm::vec4& defaultColor,
				std::optional<util::Logger> logger,
				const std::optional<std::vector<std::string>>& modelNamesOpt
			) {
				const aiScene* scene = aiImportFile(file.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);

				if (!scene)
				{
					if (logger.has_value())
					{
						(*logger).error << "Could not load scene - " << aiGetErrorString() << util::endl;
					}
					return std::nullopt;
				}

				std::map<std::string, int> meshNames;
				for (std::uint32_t meshIdx = 0u; meshIdx < scene->mNumMeshes; meshIdx++)
				{
					meshNames[scene->mMeshes[meshIdx]->mName.C_Str()] = meshIdx;
				}

				// If model names were given as input, verify that each of them exists in the populated map
				for (auto&& name : modelNamesOpt.value_or(std::vector<std::string>()))
				{
					if (meshNames.find(name) == meshNames.end())
					{
						if (logger.has_value())
						{
							(*logger).error << "Model name " << name << " does not exist in the loaded scene." << util::endl;
						}
						return {};
					}
				}

				std::vector<std::string> namesToUse;
				if (modelNamesOpt.has_value())
				{
					namesToUse = *modelNamesOpt;
				}
				else
				{
					namesToUse.reserve(scene->mNumMeshes);
					for (std::uint32_t meshIdx = 0u; meshIdx < scene->mNumMeshes; meshIdx++)
					{
						namesToUse.push_back(scene->mMeshes[meshIdx]->mName.C_Str());
					}
				}

				// Actually load each!
				std::map<std::string, BasicGeometry> outGeo;
				for (auto&& meshName : namesToUse)
				{
					std::vector<SolidShaderVertex> verts;
					std::vector<std::uint32_t> indices;
					auto mesh = scene->mMeshes[meshNames[meshName]];

					//
					// Load material
					//
					auto mat = scene->mMaterials[mesh->mMaterialIndex];
					aiColor4D diffuseColor;
					glm::vec4 color;
					if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor))
					{
						color = { diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a };
					}
					else
					{
						if (logger.has_value())
						{
							(*logger).warn << "Could not get diffuse color for model. Fallback to default." << util::endl;
							color = defaultColor;
						}
					}

					//
					// Load vertices
					//
					for (std::uint32_t vertIdx = 0u; vertIdx < mesh->mNumVertices; vertIdx++)
					{
						verts.push_back({
							{ mesh->mVertices[vertIdx].x, mesh->mVertices[vertIdx].y, mesh->mVertices[vertIdx].z },
							color
						});
					}

					//
					// Load indices
					//
					for (std::uint32_t faceIdx = 0u; faceIdx < mesh->mNumFaces; faceIdx++)
					{
						indices.push_back(mesh->mFaces[faceIdx].mIndices[0u]);
						indices.push_back(mesh->mFaces[faceIdx].mIndices[1u]);
						indices.push_back(mesh->mFaces[faceIdx].mIndices[2u]);
					}

					outGeo[meshName] = { verts, indices };
				}

				return outGeo;
			}
		}
	}
}