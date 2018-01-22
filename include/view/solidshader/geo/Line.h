#pragma once

#include <model\geo\Line.h>
#include <memory>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <view/solidshader/solidshader.h>

namespace view
{
	namespace solidshader
	{
		namespace geo
		{
			class Line
			{
			public:
				Line(std::shared_ptr<model::geo::Line> line, std::uint8_t nSubdivisions, const glm::vec4& color);
				~Line();

				bool prepare(std::shared_ptr<view::solidshader::SolidShader> shader, util::PipelineState& pso);
				bool release();

				void render(std::shared_ptr<view::solidshader::SolidShader> shader);

				glm::mat4 worldTransform();

				std::vector<view::solidshader::SolidShaderVertex> getVertices();
				std::vector<std::uint32_t> getIndices();

			private:
				std::uint8_t nSubdivisions_;
				glm::vec4 color_;

				std::shared_ptr<model::geo::Line> lineModel_;
				GLuint vertexBuffer_, indexBuffer_, vao_;
				std::uint32_t numIndices_;
				bool isReady_;

				glm::mat4 worldTransform_;
				bool worldTransformDirty_;
			};
		}
	}
}