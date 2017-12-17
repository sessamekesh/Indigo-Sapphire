#pragma once

#include <view/solidshader/assets/importedgeo.h>
#include <resources.h>
#include <glm/gtc/quaternion.hpp>
#include <util/Logger.h>

namespace view
{
	namespace solidshader
	{
		namespace assets
		{
			class ComputerMonitor
			{
			public:
				ComputerMonitor(
					const glm::vec3& pos,
					const glm::quat& orientation,
					const glm::vec3& scale
				);
				~ComputerMonitor();

				bool prepare();
				bool release();

				void render(std::shared_ptr<view::solidshader::SolidShader> shader);

				glm::mat4 worldTransform();

			private:
				struct GLResources
				{
					GLuint vertexBuffer;
					GLuint indexBuffer;
					GLuint vao;
					unsigned int numIndices;
				};

			private:
				glm::vec3 pos_;
				glm::quat orientation_;
				glm::vec3 scl_;

				glm::mat4 worldTransform_;
				bool worldTransformDirty_;
				
				std::map<std::string, GLResources> glResources_;

				bool isReady_;

				util::Logger log_;
			};
		}
	}
}