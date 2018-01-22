#include <glad/glad.h>
#include <view/solidshader/geo/grid.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <view/solidshader/geo/Line.h>

namespace view
{
	namespace solidshader
	{
		namespace geo
		{
			Grid::Grid(std::shared_ptr<model::geo::Grid> gridModel, std::uint8_t lineSubdivisions, const glm::vec4& color)
				: _gridModel(gridModel)
				, _vertexBuffer(0)
				, _indexBuffer(0)
				, _numIndices(0u)
				, _isReady(false)
				, _worldTransform(glm::mat4(1.f))
				, _worldTransformDirty(true)
				, _lineSubdivisions(lineSubdivisions)
				, color_(color)
				, vertsCache_(0)
				, indicesCache_(0)
			{}

			Grid::~Grid()
			{
				release();
			}

			bool Grid::prepare(std::shared_ptr<view::solidshader::SolidShader> shader, util::PipelineState& pso)
			{
				if (!_isReady)
				{
					bool clear = false;
					if (vertsCache_.size() == 0u)
					{
						genVertsIndicesCache();
						clear = true;
					}

					glGenVertexArrays(1, &_vao);
					glBindVertexArray(_vao);

					glGenBuffers(1, &_vertexBuffer);
					glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
					auto&& verts = getVertices();
					glBufferStorage(GL_ARRAY_BUFFER, verts.size() * sizeof(view::solidshader::SolidShaderVertex), &verts[0], 0x00);

					shader->setVertexAttribPointers(pso);

					glGenBuffers(1, &_indexBuffer);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
					auto&& indices = getIndices();
					glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(std::uint32_t), &indices[0], 0x00);

					glBindVertexArray(0);
					glBindBuffer(GL_ARRAY_BUFFER, 0);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

					_numIndices = indices.size();

					if (clear)
					{
						clearVertsIndicesCache();
					}
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
				bool clear = false;
				if (vertsCache_.size() == 0u)
				{
					genVertsIndicesCache();
					clear = true;
				}

				auto tr = std::vector<view::solidshader::SolidShaderVertex>(vertsCache_);

				if (clear)
				{
					clearVertsIndicesCache();
				}

				return tr;
			}

			std::vector<std::uint32_t> Grid::getIndices()
			{
				bool clear = false;
				if (indicesCache_.size() == 0u)
				{
					genVertsIndicesCache();
					clear = true;
				}

				auto tr = std::vector<std::uint32_t>(indicesCache_);

				if (clear)
				{
					clearVertsIndicesCache();
				}

				return tr;
			}

			void Grid::render(std::shared_ptr<view::solidshader::SolidShader> shader)
			{
				shader->setWorldMatrix(worldTransform());

				glBindVertexArray(_vao);
				glDrawElements(GL_TRIANGLES, _numIndices, GL_UNSIGNED_INT, NULL);
			}

			void Grid::genVertsIndicesCache()
			{
				// Create a bunch of lines
				auto lineModels = _gridModel->getLines();
				auto lineGeo = std::vector<view::solidshader::geo::Line>();
				lineGeo.reserve(lineModels.size());
				for (auto&& lm : lineModels)
				{
					lineGeo.push_back(view::solidshader::geo::Line(std::shared_ptr<model::geo::Line>(&lm, [](model::geo::Line*) {}), _lineSubdivisions, color_));
				}

				// Not exact figures, just enough to prevent memory thrashing.
				vertsCache_.reserve(lineModels.size() * _lineSubdivisions * 2u);
				indicesCache_.reserve(lineModels.size() * _lineSubdivisions * 6u);

				// Is this wrong? Not sure...
				for (auto lg : lineGeo)
				{
					for (auto idx : lg.getIndices())
					{
						indicesCache_.push_back(idx + vertsCache_.size());
					}

					for (auto vert : lg.getVertices())
					{
						vertsCache_.push_back(vert);
					}
				}
			}

			void Grid::clearVertsIndicesCache()
			{
				// As per the C++ standard, there's really not much we can do to actually resize a vector, so instead, we'll re-assign it to an empty one
				vertsCache_ = std::vector<view::solidshader::SolidShaderVertex>(0u);
				indicesCache_ = std::vector<std::uint32_t>(0u);
			}
		}
	}
}