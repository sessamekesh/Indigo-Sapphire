#pragma once

#include <view/terrainshader/terrainshader.h>
#include <model/withworldtransform.h>
#include <string>
#include <map>
#include <optional>
#include <util/Logger.h>

namespace view
{
	namespace terrainshader
	{
		namespace assets
		{
			std::optional<std::map<std::string, view::terrainshader::Mesh>> importFromFile(
				const std::string& fileName,
				const std::vector<std::string> modelNames,
				std::map<std::string, std::shared_ptr<view::Texture>> textures,
				std::optional<util::Logger> logger = std::nullopt
			);

			// TODO SESS: Give this all the important stuff too (consume the fname, modelnames, textures, whatever)
			class ImportedGeo : public model::WithWorldTransform
			{
			public:
				ImportedGeo(
					const glm::vec3& pos,
					const glm::quat& orientation,
					const glm::vec3& scale,
					std::string modelName
				);
				~ImportedGeo();
				ImportedGeo(const ImportedGeo&) = delete;

				bool prepare(
					std::string fileName,
					const std::vector<std::string> modelNames,
					const std::map<std::string, std::shared_ptr<view::Texture>>& texturesForModels,
					std::shared_ptr<view::terrainshader::TerrainShader> shader,
					util::PipelineState& pso
				);
				bool release();

				void render(std::shared_ptr<view::terrainshader::TerrainShader> shader);

			protected:
				struct GLResources
				{
					GLuint vertexBuffer;
					GLuint indexBuffer;
					GLuint vao;
					unsigned int numIndices;
					std::shared_ptr<view::Texture> texture;
				};

			protected:
				std::map<std::string, GLResources> glResources_;

				bool isReady_;
				std::string modelName_;

				util::Logger log;
			};
		}
	}
}