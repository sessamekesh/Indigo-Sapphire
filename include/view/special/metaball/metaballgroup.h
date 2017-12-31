#pragma once

#include <model/specialgeo/metaball/metaballgroup.h>
#include <memory>
#include <view/special/metaball/phongshader.h>

namespace view
{
	namespace special
	{
		namespace metaball
		{
			class MetaballGroup
			{
			public:
				MetaballGroup(
					std::shared_ptr<model::specialgeo::metaball::MetaballGroup> model,
					const glm::vec3& mtlDiffuse,
					const glm::vec3& mtlSpecular,
					float shininess
				);
				~MetaballGroup();
				MetaballGroup(const MetaballGroup&) = delete;

				bool preparePhong(
					const glm::vec3& cameraPos,
					const glm::vec3& lookAtDir,
					const glm::vec3& cameraUp
				);
				bool release();

				void renderPhong(std::shared_ptr<view::special::metaball::PhongShader> shader);

				std::vector<view::special::metaball::PhongVertex> getPhongVertices(
					const glm::vec3& cameraPos,
					const glm::vec3& lookAtDir,
					const glm::vec3& cameraUp
				);
				std::vector<std::uint32_t> getIndices();

			private:
				std::shared_ptr<model::specialgeo::metaball::MetaballGroup> model_;

				GLuint vertexBuffer_, indexBuffer_, vao_;
				std::uint32_t numIndices_;
				bool isReady_;

				glm::vec3 mtlDiffuse_;
				glm::vec3 mtlSpecular_;
				float shininess_;
			};
		}
	}
}