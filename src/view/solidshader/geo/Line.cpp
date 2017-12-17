#include <glad/glad.h>
#include <view/solidshader/geo/Line.h>
#include <glm/gtc/constants.hpp>

namespace view
{
	namespace solidshader
	{
		namespace geo
		{
			Line::Line(std::shared_ptr<model::geo::Line> line, std::uint8_t nSubdivisions, const glm::vec4& color)
				: lineModel_(line)
				, nSubdivisions_(nSubdivisions)
				, color_(color)
				, vertexBuffer_(0)
				, indexBuffer_(0)
				, numIndices_(0)
				, isReady_(false)
				, worldTransform_(glm::mat4(1.f))
				, worldTransformDirty_(true)
			{}

			Line::~Line()
			{
				release();
			}

			bool Line::prepare()
			{
				if (!isReady_)
				{
					glGenVertexArrays(1, &vao_);
					glBindVertexArray(vao_);

					glGenBuffers(1, &vertexBuffer_);
					glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
					auto&& verts = getVertices();
					glBufferStorage(GL_ARRAY_BUFFER, verts.size() * sizeof(view::solidshader::SolidShaderVertex), &verts[0], 0x00);

					view::solidshader::setVertexAttribPointers();

					glGenBuffers(1, &indexBuffer_);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer_);
					auto&& indices = getIndices();
					glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(std::uint32_t), &indices[0], 0x00);

					glBindVertexArray(NULL);

					numIndices_ = indices.size();
				}

				isReady_ = true;
				return true;
			}

			bool Line::release()
			{
				if (isReady_)
				{
					if (indexBuffer_ != 0u)
					{
						glDeleteBuffers(1, &indexBuffer_);
						indexBuffer_ = 0u;
					}

					if (vertexBuffer_ != 0u)
					{
						glDeleteBuffers(1, &vertexBuffer_);
						vertexBuffer_ = 0u;
					}

					if (vao_ != 0u)
					{
						glDeleteVertexArrays(1, &vao_);
						vao_ = 0u;
					}
				}
				isReady_ = false;

				return true;
			}

			glm::mat4 Line::worldTransform()
			{
				return glm::mat4(1.f);
			}

			void Line::render(std::shared_ptr<view::solidshader::SolidShader> shader)
			{
				shader->setWorldMatrix(worldTransform());

				glBindVertexArray(vao_);
				glDrawElements(GL_TRIANGLES, numIndices_, GL_UNSIGNED_INT, NULL);
			}


			std::vector<view::solidshader::SolidShaderVertex> Line::getVertices()
			{
				// Create a cyllinder with points around the endpoint
				std::vector<view::solidshader::SolidShaderVertex> tr;
				
				// First, find the normal of the line
				glm::vec3 vn = glm::normalize(lineModel_->ep2() - lineModel_->ep1());

				// If the two points are the same, no we cannot form anything!
				if (glm::length(vn) == 0.f)
				{
					return tr;
				}

				tr.reserve(nSubdivisions_ * 2u);

				// Find an orthonormal basis - this will be two vectors orthogonal to vn.
				// To form the first, pick literally any vector not linearly dependent on vn.
				// How about just rotate the components of vn over one?
				glm::vec3 vnSomeVectorNotVn = glm::vec3(vn.y, vn.z, vn.x);

				// Take the cross product of the two to get our first orthonormal vector
				glm::vec3 vec1 = glm::normalize(glm::cross(vn, vnSomeVectorNotVn));
				glm::vec3 vec2 = glm::normalize(glm::cross(vn, vec1));

				glm::vec3 ep1 = lineModel_->ep1();
				glm::vec3 ep2 = lineModel_->ep2();
				float radius = lineModel_->radius();

				// Sweeeet. Now we can form the circle points.
				for (std::uint8_t i = 0u; i < nSubdivisions_; i++)
				{
					auto pt1 = ep1 + (vec1 * radius * sinf(i * glm::two_pi<float>() / nSubdivisions_)) + (vec2 * radius * cosf(i * glm::two_pi<float>() / nSubdivisions_));
					auto pt2 = ep2 + (vec1 * radius * sinf(i * glm::two_pi<float>() / nSubdivisions_)) + (vec2 * radius * cosf(i * glm::two_pi<float>()  / nSubdivisions_));

					tr.push_back({ pt1, color_ });
					tr.push_back({ pt2, color_ });
				}

				return tr;
			}

			std::vector<std::uint32_t> Line::getIndices()
			{
				std::vector<std::uint32_t> tr;

				// First, find the normal of the line
				glm::vec3 vn = glm::normalize(lineModel_->ep2() - lineModel_->ep1());

				// If the two points are the same, no we cannot form anything!
				if (glm::length(vn) == 0.f)
				{
					return tr;
				}

				tr.reserve(nSubdivisions_ * 6u);

				for (std::uint8_t i = 0u; i < nSubdivisions_ * 2 - 2; i+=2)
				{
					// TODO SESS: Continue here, in making the line renderable!
					tr.push_back(i);
					tr.push_back(i + 1);
					tr.push_back(i + 2);
					
					tr.push_back(i + 2);
					tr.push_back(i + 3);
					tr.push_back(i + 1);
				}

				tr.push_back(nSubdivisions_ * 2 - 2);
				tr.push_back(nSubdivisions_ * 2 - 1);
				tr.push_back(0);

				tr.push_back(0);
				tr.push_back(1);
				tr.push_back(nSubdivisions_ * 2 - 1);

				return tr;
			}
		}
	}
}