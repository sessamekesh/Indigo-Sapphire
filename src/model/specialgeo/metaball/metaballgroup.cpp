#include <model/specialgeo/metaball/metaballgroup.h>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <model/geo/plane.h>

namespace model
{
	namespace specialgeo
	{
		namespace metaball
		{
			MetaballGroup::MetaballGroup(
				const glm::vec3& pos,
				const glm::quat& rot,
				const glm::vec3& scl,
				unsigned int anticipatedNumberOfMetaballs
			)
				: WithWorldTransform(pos, rot, scl)
				, activeMetaballs_()
			{
				activeMetaballs_.reserve(anticipatedNumberOfMetaballs);
			}

			// TODO SESS: Test this!
			std::optional<geo::Rectangle> MetaballGroup::getOrthoBindingRect(const glm::vec3& viewerPos, const glm::vec3& lookAtDirection, const glm::vec3& viewerUp)
			{
				// No metaballs? No rectangle.
				if (activeMetaballs_.size() == 0u)
				{
					return {};
				}

				auto matWorld = worldTransform();

				// Find the closest metaball to the camera. We will render at this depth
				struct
				{
					float distance;
					int idx;
				} closestMetaball = { glm::dot(activeMetaballs_[0].pos(matWorld) - viewerPos, lookAtDirection) - activeMetaballs_[0].radius(), 0 };
				for (auto i = 1; i < activeMetaballs_.size(); i++)
				{
					float dist = glm::dot(activeMetaballs_[i].pos(matWorld) - viewerPos, lookAtDirection) - activeMetaballs_[i].radius();
					if (closestMetaball.distance < 0.f || (dist < closestMetaball.distance && dist > 0.f))
					{
						closestMetaball = { dist, i };
					}
				}

				// All metaballs are behind the camera if the closest distance is less than zero.
				// In that case, just ignore this.
				if (closestMetaball.distance < 0.f)
				{
					return {};
				}
				
				// This will be the rotation of the rectangle
				// TODO SESS: Verify this!
				glm::quat rotquat = glm::rotation(glm::vec3(0.f, 0.f, -1.f), -lookAtDirection);

				glm::vec3 up = glm::normalize(viewerUp);
				glm::vec3 right = glm::normalize(glm::cross(lookAtDirection, up));
				geo::Plane plane(activeMetaballs_[closestMetaball.idx].pos(matWorld) - glm::normalize(lookAtDirection) * activeMetaballs_[closestMetaball.idx].radius(), -lookAtDirection);

				float mostLeft = 0.f;
				float mostRight = 0.f;
				float mostUp = 0.f;
				float mostDown = 0.f;

				glm::vec3 zeroVec = glm::vec3(0.f, 0.f, 0.f);
				for (auto&& m : activeMetaballs_)
				{
					float l = glm::dot(
						plane.getRayIntersection(
							m.pos(matWorld) - right * m.radius(),
							-lookAtDirection
						).value_or(zeroVec),
						right
					);
					float r = glm::dot(
						plane.getRayIntersection(
							m.pos(matWorld) + right * m.radius(),
							-lookAtDirection
						).value_or(zeroVec),
						right
					);
					float u = glm::dot(
						plane.getRayIntersection(
							m.pos(matWorld) + up * m.radius(),
							-lookAtDirection
						).value_or(zeroVec),
						up
					);
					float d = glm::dot(
						plane.getRayIntersection(
							m.pos(matWorld) - up * m.radius(),
							-lookAtDirection
						).value_or(zeroVec),
						up
					);

					if (l < mostLeft)
					{
						mostLeft = l;
					}
					if (r > mostRight)
					{
						mostRight = r;
					}
					if (u > mostUp)
					{
						mostUp = u;
					}
					if (d < mostDown)
					{
						mostDown = d;
					}
				}

				if ((mostRight - mostLeft <= 0.f) || (mostUp - mostDown <= 0.f))
				{
					return {};
				}

				return {
					geo::Rectangle(
						activeMetaballs_[closestMetaball.idx].pos(matWorld),
						rotquat,
						mostRight - mostLeft,
						mostUp - mostDown
					)
				};
			}

			Metaball MetaballGroup::getMetaball(const int idx) const
			{
				return activeMetaballs_[idx];
			}

			void MetaballGroup::removeMetaballAt(const int idx)
			{
				activeMetaballs_.erase(activeMetaballs_.begin() + idx);
			}

			unsigned int MetaballGroup::addMetaball(const Metaball& metaball)
			{
				activeMetaballs_.push_back(metaball);
				return activeMetaballs_.size() - 1u;
			}

			std::vector<Charge> MetaballGroup::getChargeData()
			{
				std::vector<Charge> tr;
				tr.reserve(activeMetaballs_.size());

				auto world = worldTransform();
				for (auto&& m : activeMetaballs_)
				{
					tr.push_back(m.getChargeData(world));
				}

				return tr;
			}
		}
	}
}