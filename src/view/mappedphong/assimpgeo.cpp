#include <view/mappedphong/assimpgeo.h>

#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>

namespace view
{
	namespace mappedphong
	{
		AssimpGeo::AssimpGeo(
			const glm::vec3& pos,
			const glm::quat& orientation,
			const glm::vec3& scale,
			std::string modelName,
			std::shared_ptr<view::Texture> normalMap,
			std::shared_ptr<view::Texture> diffuseMap,
			std::shared_ptr<view::Texture> specularMap
		)
			: model::WithWorldTransform(pos, orientation, scale)
			, glResources_()
			, isReady_(false)
			, modelName_(modelName)
			, log(util::DEBUG, util::DEBUG, std::string("[MappedPhong Import ") + modelName + "] ")
			, normalMap_(normalMap)
			, diffuseMap_(diffuseMap)
			, specularMap_(specularMap)
		{}

		AssimpGeo::~AssimpGeo()
		{}

		bool AssimpGeo::prepare(std::string fileName, std::shared_ptr<MappedPhongShader> shader, util::PipelineState& pso)
		{
			const aiScene* scene = aiImportFile(fileName.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
			if (!scene)
			{
				log.error << "Failed to load Assimp scene - " << aiGetErrorString() << util::endl;
				return false;
			}

			for (std::uint32_t meshIdx = 0u; meshIdx < scene->mNumMeshes; meshIdx++)
			{
				if (isReady_)
				{
					log.warn << "Cannot prepare an already prepared scene" << util::endl;
					return false;
				}

				auto mesh = scene->mMeshes[meshIdx];
				std::string meshName = mesh->mName.C_Str();

				std::vector<MappedPhongShader::Vertex> verts;
				std::vector<std::uint32_t> indices;

				if (mesh->GetNumUVChannels() < 1u)
				{
					log.warn << "Mesh " << meshName << " has too few UV channels, not importing..." << util::endl;
					continue;
				}

				verts.reserve(mesh->mNumVertices);
				indices.reserve(mesh->mNumFaces * 3u);

				// Load vertices
				for (std::uint32_t vertIdx = 0u; vertIdx < mesh->mNumVertices; vertIdx++)
				{

					verts.push_back(
					{
						{ mesh->mVertices[vertIdx].x, mesh->mVertices[vertIdx].y, mesh->mVertices[vertIdx].z },
						{ mesh->mNormals[vertIdx].x, mesh->mNormals[vertIdx].y, mesh->mVertices[vertIdx].z },
						{ mesh->mTangents[vertIdx].x, mesh->mTangents[vertIdx].y, mesh->mTangents[vertIdx].z },
						{ mesh->mTextureCoords[0][vertIdx].x, mesh->mTextureCoords[0][vertIdx].y }
					}
					);
				}

				// Load indices
				for (std::uint32_t faceIdx = 0u; faceIdx < mesh->mNumFaces; faceIdx++)
				{
					indices.push_back(mesh->mFaces[faceIdx].mIndices[0u]);
					indices.push_back(mesh->mFaces[faceIdx].mIndices[1u]);
					indices.push_back(mesh->mFaces[faceIdx].mIndices[2u]);
				}

				// Prepare GL resources...
				GLResources glr = {};
				glGenVertexArrays(1, &glr.vao);
				glBindVertexArray(glr.vao);

				glGenBuffers(1, &glr.vertexBuffer);
				glBindBuffer(GL_ARRAY_BUFFER, glr.vertexBuffer);
				glBufferStorage(
					GL_ARRAY_BUFFER,
					verts.size() * sizeof(MappedPhongShader::Vertex),
					&verts[0],
					0x00
				);
				glGenBuffers(1, &glr.indexBuffer);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glr.indexBuffer);
				glBufferStorage(
					GL_ELEMENT_ARRAY_BUFFER,
					indices.size() * sizeof(std::uint32_t),
					&indices[0],
					0x00
				);

				glr.numIndices = indices.size();
				shader->setVertexAttribPointers(pso);
				glBindVertexArray(NULL);

				glResources_[meshName] = glr;
			}

			isReady_ = true;
			return true;
		}

		bool AssimpGeo::release()
		{
			for (auto&& glr : glResources_)
			{
				if (glr.second.indexBuffer != 0u)
				{
					glDeleteBuffers(1, &glr.second.indexBuffer);
					glr.second.indexBuffer = 0u;
				}
				if (glr.second.vertexBuffer != 0u)
				{
					glDeleteBuffers(1, &glr.second.vertexBuffer);
					glr.second.vertexBuffer = 0u;
				}
				if (glr.second.vao != 0u)
				{
					glDeleteVertexArrays(1, &glr.second.vao);
					glr.second.vao = 0u;
				}
			}
			glResources_.clear();
			isReady_ = false;
			return true;
		}

		void AssimpGeo::render(std::shared_ptr<MappedPhongShader> shader)
		{
			shader->setWorldMatrix(worldTransform());
			shader->setNormalMap(normalMap_);
			shader->setDiffuseMap(diffuseMap_);
			shader->setSpecularMap(specularMap_);
			for (auto&& glr : glResources_)
			{
				glBindVertexArray(glr.second.vao);
				glDrawElements(GL_TRIANGLES, glr.second.numIndices, GL_UNSIGNED_INT, NULL);
			}
		}
	}
}