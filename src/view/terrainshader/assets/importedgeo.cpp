#include <view/terrainshader/assets/importedgeo.h>

#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>

namespace view
{
	namespace terrainshader
	{
		namespace assets
		{
			std::optional<std::map<std::string, view::terrainshader::Mesh>> importFromFile(
				const std::string & fileName,
				const std::vector<std::string> modelNames,
				std::map<std::string, std::shared_ptr<view::Texture>> textures,
				std::optional<util::Logger> logger)
			{
				const aiScene* scene = aiImportFile(fileName.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);

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

				for (auto&& name : modelNames)
				{
					if (meshNames.find(name) == meshNames.end())
					{
						if (logger.has_value())
						{
							logger->error << "Model name " << name << " does not exist in the loaded scene." << util::endl;
						}
						return std::nullopt;
					}
				}

				// Verification complete. Load meshes.
				std::map<std::string, view::terrainshader::Mesh> meshes;
				for (auto meshName : modelNames)
				{
					if (textures.find(meshName) == textures.end())
					{
						if (logger.has_value())
						{
							logger->warn << "Mesh " << meshName << " has no associated texture, not importing..." << util::endl;
						}
						continue;
					}

					std::vector<view::terrainshader::Vertex> verts;
					std::vector<std::uint32_t> indices;
					auto mesh = scene->mMeshes[meshNames[meshName]];

					if (mesh->GetNumUVChannels() < 1u)
					{
						if (logger.has_value())
						{
							logger->warn << "Mesh " << meshName << " has too few UV channels, not importing..." << util::endl;
						}
						continue;
					}

					//
					// Load vertices
					//
					for (std::uint32_t vertIdx = 0u; vertIdx < mesh->mNumVertices; vertIdx++)
					{
						verts.push_back(
							{
								{ mesh->mVertices[vertIdx].x, mesh->mVertices[vertIdx].y, mesh->mVertices[vertIdx].z },
								{ mesh->mNormals[vertIdx].x, mesh->mNormals[vertIdx].y, mesh->mNormals[vertIdx].z },
								{ mesh->mTextureCoords[0][vertIdx].x, mesh->mTextureCoords[0][vertIdx].y }
							}
						);
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

					meshes[meshName] = {
						verts,
						indices,
						textures[meshName]
					};
				}

				// TODO SESS: This
				return meshes;
			}

			ImportedGeo::ImportedGeo(
				const glm::vec3& pos,
				const glm::quat& orientation,
				const glm::vec3& scale,
				std::string modelName
			)
				: model::WithWorldTransform(pos, orientation, scale)
				, glResources_({})
				, isReady_(false)
				, modelName_(modelName)
				, log(util::DEBUG, util::DEBUG, std::string("[") + modelName + " (terrain)] ")
			{}

			ImportedGeo::~ImportedGeo()
			{
				if (isReady_)
				{
					release();
				}
			}

			bool ImportedGeo::prepare(
				std::string fileName,
				const std::vector<std::string> modelNames,
				const std::map<std::string, std::shared_ptr<view::Texture>>& texturesForModels,
				std::shared_ptr<view::terrainshader::TerrainShader> shader,
				util::PipelineState& pso
			) {
				if (isReady_)
				{
					log.warn << "Attempted to prepare already prepared geometry" << util::endl;
					return false;
				}

				auto geosOpt = importFromFile(fileName, modelNames, texturesForModels, log);
				if (!geosOpt)
				{
					log.error << "Could not load geometry - asset loading failed" << util::endl;
					return false;
				}

				auto geos = *geosOpt;

				for (auto&& geo : geos)
				{
					GLResources glr = {};

					glGenVertexArrays(1, &glr.vao);
					glBindVertexArray(glr.vao);

					glGenBuffers(1, &glr.vertexBuffer);
					glBindBuffer(GL_ARRAY_BUFFER, glr.vertexBuffer);
					glBufferStorage(
						GL_ARRAY_BUFFER,
						geo.second.verts.size() * sizeof(view::terrainshader::Vertex),
						&geo.second.verts[0],
						0x00
					);

					glGenBuffers(1, &glr.indexBuffer);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glr.indexBuffer);
					glBufferStorage(
						GL_ELEMENT_ARRAY_BUFFER,
						geo.second.indices.size() * sizeof(std::uint32_t),
						&geo.second.indices[0],
						0x00
					);

					glr.texture = geo.second.texture;
					glr.numIndices = geo.second.indices.size();

					shader->setVertexAttribPointers(pso);

					glBindVertexArray(NULL);

					glResources_[geo.first] = glr;
				}

				isReady_ = true;

				return true;
			}

			bool ImportedGeo::release()
			{
				for (auto&& glr : glResources_)
				{
					if (glr.second.vao != 0u)
					{
						glDeleteVertexArrays(1, &glr.second.vao);
						glr.second.vao = 0u;
					}

					if (glr.second.vertexBuffer != 0u)
					{
						glDeleteBuffers(1, &glr.second.vertexBuffer);
						glr.second.vertexBuffer = 0u;
					}

					if (glr.second.indexBuffer != 0u)
					{
						glDeleteBuffers(1, &glr.second.indexBuffer);
						glr.second.indexBuffer = 0u;
					}

					glr.second.texture = nullptr;
				}
				glResources_.clear();
				isReady_ = false;

				return true;
			}

			void ImportedGeo::render(std::shared_ptr<view::terrainshader::TerrainShader> shader)
			{
				shader->setWorldMatrix(worldTransform());
				for (auto&& glr : glResources_)
				{
					glBindVertexArray(glr.second.vao);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, glr.second.texture->texture());
					glDrawElements(GL_TRIANGLES, glr.second.numIndices, GL_UNSIGNED_INT, NULL);
				}
			}
		}
	}
}