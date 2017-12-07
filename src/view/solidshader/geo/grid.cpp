#include <glad/glad.h>
#include <view/solidshader/geo/grid.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace view
{
	namespace solidshader
	{
		namespace geo
		{
			Grid::Grid(std::shared_ptr<model::geo::Grid> gridModel)
				: _gridModel(gridModel)
				, _vertexBuffer(0)
				, _indexBuffer(0)
				, _numIndices(0u)
				, _isReady(false)
				, _worldTransform(glm::mat4(1.f))
				, _worldTransformDirty(true)
			{}

			Grid::~Grid()
			{
				release();
			}

			bool Grid::prepare()
			{
				if (!_isReady)
				{
					glGenBuffers(1, &_vertexBuffer);
					glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
					auto&& verts = getVertices();
					glBufferStorage(GL_ARRAY_BUFFER, verts.size() * sizeof(view::solidshader::SolidShaderVertex), &verts[0], 0x00);

					glGenVertexArrays(1, &_vao);
					glBindVertexArray(_vao);
					glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);

					view::solidshader::setVertexAttribPointers();

					glGenBuffers(1, &_indexBuffer);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
					auto&& indices = getIndices();
					glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(std::uint32_t), &indices[0], 0x00);

					glBindVertexArray(0);
					glBindBuffer(GL_ARRAY_BUFFER, 0);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

					_numIndices = indices.size();
				}

				_isReady = true;
				return true;
			}

			bool Grid::release()
			{
				if (_isReady)
				{
					if (_indexBuffer != 0u)
					{
						glDeleteBuffers(1, &_indexBuffer);
						_indexBuffer = 0u;
					}

					if (_vertexBuffer != 0u)
					{
						glDeleteBuffers(1, &_vertexBuffer);
						_vertexBuffer = 0u;
					}

					if (_vao != 0u)
					{
						glDeleteVertexArrays(1, &_vao);
						_vao = 0u;
					}
				}
				_isReady = false;

				return true;
			}

			glm::mat4 Grid::worldTransform()
			{
				if (_worldTransformDirty)
				{
					_worldTransform = glm::mat4();
					_worldTransform *= glm::translate(_gridModel->pos());
					_worldTransform *= glm::toMat4(_gridModel->orientation());
					_worldTransformDirty = false;
				}
				return _worldTransform;
			}

			std::vector<view::solidshader::SolidShaderVertex> Grid::getVertices()
			{
				// Create a box of width and depth described in the model at each location of the grid
				std::vector<view::solidshader::SolidShaderVertex> tr;
				tr.reserve(3u);
				tr.push_back({ { 0, 0, 0 },{ 1, 0, 0, 1 } });
				tr.push_back({ { 1, 0, 0 },{ 0, 1, 0, 1 } });
				tr.push_back({ { 0.5, 1, 0 },{ 0, 0, 1, 1 } });
				return tr;
			}

			std::vector<std::uint32_t> Grid::getIndices()
			{
				std::vector<std::uint32_t> tr;
				tr.reserve(3u);
				tr.push_back(0);
				tr.push_back(1);
				tr.push_back(2);
				return tr;
			}

			void Grid::render(std::shared_ptr<view::solidshader::SolidShader> shader)
			{
				shader->setWorldMatrix(worldTransform());

				glBindVertexArray(_vao);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
				glDrawElements(GL_TRIANGLES, _numIndices, GL_UNSIGNED_INT, NULL);
			}
		}
	}
}