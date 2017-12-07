#pragma once

#include <model/geo/grid.h>
#include <memory>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <view/solidshader/solidshader.h>
#include <vector>
#include <ctype.h>

namespace view
{
	namespace solidshader
	{
		namespace geo
		{
			class Grid
			{
			public:
				Grid(std::shared_ptr<model::geo::Grid> gridModel);
				~Grid();
				bool prepare();
				bool release();

				void render(std::shared_ptr<view::solidshader::SolidShader> shader);

				glm::mat4 worldTransform();

			private:
				std::shared_ptr<model::geo::Grid> _gridModel;
				GLuint _vertexBuffer, _indexBuffer;
				GLuint _vao;
				unsigned int _numIndices;
				bool _isReady;

				glm::mat4 _worldTransform;
				bool _worldTransformDirty;

				std::vector<view::solidshader::SolidShaderVertex> getVertices();
				std::vector<std::uint32_t> getIndices();
			};
		}
	}
}