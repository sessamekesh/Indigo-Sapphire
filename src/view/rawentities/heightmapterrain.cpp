#include <view/rawentities/heightmapterrain.h>

namespace view
{
	namespace raw
	{
		HeightmapTerrainEntity::HeightmapTerrainEntity(
			std::shared_ptr<model::specialgeo::Heightfield> heightField
		)
			: heightField_(heightField)
		{}

		std::shared_ptr<view::GenericMesh> HeightmapTerrainEntity::getMeshData(unsigned int rows, unsigned int cols, float defaultHeight, bool blend) const
		{
			std::shared_ptr<view::GenericMesh> tr = std::make_shared<view::GenericMesh>();
			unsigned int numVertices = (rows + 1u) * (cols + 1u);
			tr->vertices.resize(numVertices);
			tr->indices.reserve(rows * cols * 6u);

			// First pass: Generate positions and UVs (which only depend on data from the height field)
			for (unsigned int row = 0u; row <= rows; row++)
			{
				for (unsigned int col = 0u; col <= cols; col++)
				{
					float x = (((float)row / (float)rows) * 2.f - 1.f) * heightField_->width();
					float z = (((float)col / (float)cols) * 2.f - 1.f) * heightField_->depth();

					unsigned int vertIdx = row * (cols + 1u) + col;

					tr->vertices[vertIdx].position = glm::vec3(
						x, heightField_->heightAtPos(x, z, defaultHeight, blend), z
					);
					tr->vertices[vertIdx].uv = glm::vec2(
						(float)row / (float)rows,
						(float)col / (float)cols
					);
				}
			}

			// Second pass: Generate normals (which depend on positions) and tangents (which depend on positions and uvs)
			{
				for (unsigned int row = 0u; row <= rows; row++)
				{
					for (unsigned int col = 0u; col <= cols; col++)
					{
						glm::vec3 normal(0.f, 0.f, 0.f);
						glm::vec3 tangent(0.f, 0.f, 0.f);
						//
						// Generate normal by averaging normals for all 8 triangles surrounding self
						// Generate tangent by averaging tangents for all 8 triangles surrounding self
						//
						// * - * - *
						// |8\1|2/3|
						// * - * - *
						// |7/6|5\4|
						// * - * - *
						//

						unsigned int centerVertIdx = row * (cols + 1u) + col;
						if (row >= 1u)
						{
							if (col >= 1u)
							{
								// 1
								// TODO SESS: Verify winding order (are normals generated correctly? Are they pointing up, generally?
								unsigned int v1Idx = (row - 1u) * (cols + 1u) + col - 1u;
								unsigned int v2Idx = (row - 1u) * (cols + 1u) + col;
								normal += glm::normalize(glm::cross(
									tr->vertices[v1Idx].position - tr->vertices[centerVertIdx].position,
									tr->vertices[v2Idx].position - tr->vertices[centerVertIdx].position
								));

								auto duv1 = tr->vertices[v1Idx].uv - tr->vertices[centerVertIdx].uv;
								auto duv2 = tr->vertices[v2Idx].uv - tr->vertices[centerVertIdx].uv;
								auto dp1 = tr->vertices[v1Idx].position - tr->vertices[centerVertIdx].position;
								auto dp2 = tr->vertices[v2Idx].position - tr->vertices[centerVertIdx].position;
								float r = 1.f / (duv1.x * duv2.y - duv1.y * duv2.x);
								tangent += (dp1 * duv2.y - dp2 * duv1.y) * r;
							}

							if (col < cols)
							{
								// 2
								unsigned int v1Idx = (row - 1u) * (cols + 1u) + col;
								unsigned int v2Idx = (row - 1u) * (cols + 1u) + col + 1u;
								normal += glm::normalize(glm::cross(
									tr->vertices[v1Idx].position - tr->vertices[centerVertIdx].position,
									tr->vertices[v2Idx].position - tr->vertices[centerVertIdx].position
								));

								auto duv1 = tr->vertices[v1Idx].uv - tr->vertices[centerVertIdx].uv;
								auto duv2 = tr->vertices[v2Idx].uv - tr->vertices[centerVertIdx].uv;
								auto dp1 = tr->vertices[v1Idx].position - tr->vertices[centerVertIdx].position;
								auto dp2 = tr->vertices[v2Idx].position - tr->vertices[centerVertIdx].position;
								float r = 1.f / (duv1.x * duv2.y - duv1.y * duv2.x);
								tangent += (dp1 * duv2.y - dp2 * duv1.y) * r;
							}
						}

						if (col < cols)
						{
							if (row >= 1u)
							{
								// 3
								unsigned int v1Idx = (row - 1u) * (cols + 1u) + col + 1u;
								unsigned int v2Idx = row * (cols + 1u) + col + 1u;
								normal += glm::normalize(glm::cross(
									tr->vertices[v1Idx].position - tr->vertices[centerVertIdx].position,
									tr->vertices[v2Idx].position - tr->vertices[centerVertIdx].position
								));

								auto duv1 = tr->vertices[v1Idx].uv - tr->vertices[centerVertIdx].uv;
								auto duv2 = tr->vertices[v2Idx].uv - tr->vertices[centerVertIdx].uv;
								auto dp1 = tr->vertices[v1Idx].position - tr->vertices[centerVertIdx].position;
								auto dp2 = tr->vertices[v2Idx].position - tr->vertices[centerVertIdx].position;
								float r = 1.f / (duv1.x * duv2.y - duv1.y * duv2.x);
								tangent += (dp1 * duv2.y - dp2 * duv1.y) * r;
							}

							if (row < rows)
							{
								// 4
								unsigned int v1Idx = row * (cols + 1u) + col + 1u;
								unsigned int v2Idx = (row + 1u) * (cols + 1u) + col + 1u;
								normal += glm::normalize(glm::cross(
									tr->vertices[v1Idx].position - tr->vertices[centerVertIdx].position,
									tr->vertices[v2Idx].position - tr->vertices[centerVertIdx].position
								));

								auto duv1 = tr->vertices[v1Idx].uv - tr->vertices[centerVertIdx].uv;
								auto duv2 = tr->vertices[v2Idx].uv - tr->vertices[centerVertIdx].uv;
								auto dp1 = tr->vertices[v1Idx].position - tr->vertices[centerVertIdx].position;
								auto dp2 = tr->vertices[v2Idx].position - tr->vertices[centerVertIdx].position;
								float r = 1.f / (duv1.x * duv2.y - duv1.y * duv2.x);
								tangent += (dp1 * duv2.y - dp2 * duv1.y) * r;
							}
						}

						if (row < rows)
						{
							if (col < cols)
							{
								// 5
								unsigned int v1Idx = (row + 1u) * (cols + 1u) + col + 1u;
								unsigned int v2Idx = (row + 1u) * (cols + 1u) + col;
								normal += glm::normalize(glm::cross(
									tr->vertices[v1Idx].position - tr->vertices[centerVertIdx].position,
									tr->vertices[v2Idx].position - tr->vertices[centerVertIdx].position
								));

								auto duv1 = tr->vertices[v1Idx].uv - tr->vertices[centerVertIdx].uv;
								auto duv2 = tr->vertices[v2Idx].uv - tr->vertices[centerVertIdx].uv;
								auto dp1 = tr->vertices[v1Idx].position - tr->vertices[centerVertIdx].position;
								auto dp2 = tr->vertices[v2Idx].position - tr->vertices[centerVertIdx].position;
								float r = 1.f / (duv1.x * duv2.y - duv1.y * duv2.x);
								tangent += (dp1 * duv2.y - dp2 * duv1.y) * r;
							}

							if (col >= 1u)
							{
								// 6
								unsigned int v1Idx = (row + 1u) * (cols + 1u) + col;
								unsigned int v2Idx = (row + 1u) * (cols + 1u) + col - 1u;
								normal += glm::normalize(glm::cross(
									tr->vertices[v1Idx].position - tr->vertices[centerVertIdx].position,
									tr->vertices[v2Idx].position - tr->vertices[centerVertIdx].position
								));

								auto duv1 = tr->vertices[v1Idx].uv - tr->vertices[centerVertIdx].uv;
								auto duv2 = tr->vertices[v2Idx].uv - tr->vertices[centerVertIdx].uv;
								auto dp1 = tr->vertices[v1Idx].position - tr->vertices[centerVertIdx].position;
								auto dp2 = tr->vertices[v2Idx].position - tr->vertices[centerVertIdx].position;
								float r = 1.f / (duv1.x * duv2.y - duv1.y * duv2.x);
								tangent += (dp1 * duv2.y - dp2 * duv1.y) * r;
							}
						}

						if (col >= 1u)
						{
							if (row < rows)
							{
								// 7
								unsigned int v1Idx = (row + 1u) * (cols + 1u) + col - 1u;
								unsigned int v2Idx = row * (cols + 1u) + col - 1u;
								normal += glm::normalize(glm::cross(
									tr->vertices[v1Idx].position - tr->vertices[centerVertIdx].position,
									tr->vertices[v2Idx].position - tr->vertices[centerVertIdx].position
								));

								auto duv1 = tr->vertices[v1Idx].uv - tr->vertices[centerVertIdx].uv;
								auto duv2 = tr->vertices[v2Idx].uv - tr->vertices[centerVertIdx].uv;
								auto dp1 = tr->vertices[v1Idx].position - tr->vertices[centerVertIdx].position;
								auto dp2 = tr->vertices[v2Idx].position - tr->vertices[centerVertIdx].position;
								float r = 1.f / (duv1.x * duv2.y - duv1.y * duv2.x);
								tangent += (dp1 * duv2.y - dp2 * duv1.y) * r;
							}

							if (row >= 1u)
							{
								// 8
								unsigned int v1Idx = row * (cols + 1u) + col - 1u;
								unsigned int v2Idx = (row - 1u) * (cols + 1u) + col - 1u;
								normal += glm::normalize(glm::cross(
									tr->vertices[v1Idx].position - tr->vertices[centerVertIdx].position,
									tr->vertices[v2Idx].position - tr->vertices[centerVertIdx].position
								));

								auto duv1 = tr->vertices[v1Idx].uv - tr->vertices[centerVertIdx].uv;
								auto duv2 = tr->vertices[v2Idx].uv - tr->vertices[centerVertIdx].uv;
								auto dp1 = tr->vertices[v1Idx].position - tr->vertices[centerVertIdx].position;
								auto dp2 = tr->vertices[v2Idx].position - tr->vertices[centerVertIdx].position;
								float r = 1.f / (duv1.x * duv2.y - duv1.y * duv2.x);
								tangent += (dp1 * duv2.y - dp2 * duv1.y) * r;
							}
						}

						// Save the normalized normal and tangent
						tr->vertices[centerVertIdx].normal = glm::normalize(normal);
						tr->vertices[centerVertIdx].tangent = glm::normalize(tangent);
					}
				}
			}

			//
			// Generate indices
			//
			for (unsigned int row = 0u; row < rows; row++)
			{
				for (unsigned int col = 0u; col < cols; col++)
				{
					// TODO SESS: Verify winding order
					tr->indices.push_back(row * (cols + 1u) + col);
					tr->indices.push_back((row + 1u) * (cols + 1u) + col);
					tr->indices.push_back(row * (cols + 1u) + col + 1u);

					tr->indices.push_back(row * (cols + 1u) + col + 1u);
					tr->indices.push_back((row + 1u) * (cols + 1u) + col);
					tr->indices.push_back((row + 1u) * (cols + 1u) + col + 1u);
				}
			}

			return tr;
		}
	}
}