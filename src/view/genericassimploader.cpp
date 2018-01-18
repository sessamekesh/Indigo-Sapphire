#pragma once

#include <view/genericassmiploader.h>

#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>

namespace view
{
	std::shared_ptr<std::map<std::string, view::GenericMesh>> loadFromScene(std::string fName, util::Logger log)
	{
		const aiScene* scene = aiImportFile(fName.c_str(), aiProcessPreset_TargetRealtime_Fast);
		if (!scene)
		{
			log.error << "Failed to load Assimp scene - " << aiGetErrorString() << util::endl;
			return nullptr;
		}

		std::shared_ptr<std::map<std::string, view::GenericMesh>> tr = std::make_shared<std::map<std::string, view::GenericMesh>>();
		for (std::uint32_t meshIdx = 0u; meshIdx < scene->mNumMeshes; meshIdx++)
		{
			auto mesh = scene->mMeshes[meshIdx];
			std::string meshName = mesh->mName.C_Str();
			std::vector<view::GenericVertex> vertices;
			std::vector<std::uint32_t> indices;

			auto useUVs = mesh->GetNumUVChannels() >= 1u;
			if (!useUVs)
			{
				log.warn << "Mesh " << meshName << " has no UV channels, setting UVs to <0,0>..." << util::endl;
			}

			vertices.reserve(mesh->mNumVertices);
			indices.reserve(mesh->mNumFaces * 3u);

			// Load vertices
			for (std::uint32_t vertIdx = 0u; vertIdx < mesh->mNumVertices; vertIdx++)
			{
				view::GenericVertex v = {};
				v.position = { mesh->mVertices[vertIdx].x, mesh->mVertices[vertIdx].y, mesh->mVertices[vertIdx].z };
				v.normal = { mesh->mNormals[vertIdx].x, mesh->mNormals[vertIdx].y, mesh->mNormals[vertIdx].z };
				v.tangent = { mesh->mTangents[vertIdx].x, mesh->mTangents[vertIdx].y, mesh->mTangents[vertIdx].z };
				if (useUVs)
				{
					v.uv = { mesh->mTextureCoords[0][vertIdx].x, mesh->mTextureCoords[0][vertIdx].y };
				}
				vertices.push_back(v);
			}

			// Load indices
			for (std::uint32_t faceIdx = 0u; faceIdx < mesh->mNumFaces; faceIdx++)
			{
				indices.push_back(mesh->mFaces[faceIdx].mIndices[0u]);
				indices.push_back(mesh->mFaces[faceIdx].mIndices[1u]);
				indices.push_back(mesh->mFaces[faceIdx].mIndices[2u]);
			}

			(*tr)[meshName] = { vertices, indices };
		}

		aiReleaseImport(scene);

		return tr;
	}
}