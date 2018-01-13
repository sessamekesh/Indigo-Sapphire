#pragma once

#include <view/mappedphong/mappedphongshader.h>
#include <optional>
#include <util/Logger.h>
#include <map>
#include <string>
#include <model/withworldtransform.h>

namespace view
{
	namespace mappedphong
	{
		// TODO SESS: Implement CPP file for this
		class AssimpGeo : public model::WithWorldTransform
		{
		public:
			AssimpGeo(
				const glm::vec3& pos,
				const glm::quat& orientation,
				const glm::vec3& scale,
				std::string modelName,
				std::shared_ptr<view::Texture> normalMap,
				std::shared_ptr<view::Texture> diffuseMap,
				std::shared_ptr<view::Texture> specularMap
			);
			~AssimpGeo();
			AssimpGeo(const AssimpGeo&) = delete;

			bool prepare(std::string fileName, std::shared_ptr<MappedPhongShader> shader, util::PipelineState& pso);
			bool release();

			void render(std::shared_ptr<MappedPhongShader> shader);

		protected:
			struct GLResources
			{
				GLuint vertexBuffer;
				GLuint indexBuffer;
				GLuint vao;
				unsigned int numIndices;
			};

		protected:
			std::map<std::string, GLResources> glResources_;
			bool isReady_;
			std::string modelName_;
			util::Logger log;

			std::shared_ptr<view::Texture> normalMap_;
			std::shared_ptr<view::Texture> diffuseMap_;
			std::shared_ptr<view::Texture> specularMap_;
		};
	}
}