#include <view/rawentities/planegenerator.h>

namespace view
{
	namespace raw
	{
		view::GenericMesh PlaneGenerator::getGenericMeshData(
			const model::geo::Plane& plane,
			const glm::vec3& roughUpDir,
			const glm::vec2& dimensions
		)
		{
			view::GenericMesh tr;

			auto right = glm::normalize(glm::cross(plane.normal(), roughUpDir));
			auto up = glm::normalize(glm::cross(right, plane.normal()));

			auto halfDimensions = dimensions * 0.5f;

			// Upper left
			tr.vertices.push_back({
				plane.origin() + up * halfDimensions.y - right * halfDimensions.x,
				plane.normal(),
				{ 0.f, 0.f },
				right
			});

			// Upper right
			tr.vertices.push_back({
				plane.origin() + up * halfDimensions.y + right * halfDimensions.x,
				plane.normal(),
				{ 1.f, 0.f },
				right
			});

			// Lower left
			tr.vertices.push_back({
				plane.origin() - up * halfDimensions.y - right * halfDimensions.x,
				plane.normal(),
				{ 0.f, 1.f },
				right
			});

			// Lower right
			tr.vertices.push_back({
				plane.origin() - up * halfDimensions.y + right * halfDimensions.x,
				plane.normal(),
				{ 1.f, 1.f },
				right
			});
			
			// TODO SESS: Is this correct?
			tr.indices = { 0, 1, 2, 1, 3, 2 };

			return tr;
		}
	}
}