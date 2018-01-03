#include <util/camera/planarreflectioncamera.h>

namespace util
{
	namespace camera
	{
		PlanarReflectionCamera::PlanarReflectionCamera(
			std::shared_ptr<CameraBase> srcCamera,
			const model::geo::Plane& reflectionPlane
		)
			: CameraBase()
			, srcCamera_(srcCamera)
			, reflectionPlane_(reflectionPlane)
			, lastSrcPos_(0.f, 0.f, 0.f)
			, lastSrcUp_(0.f, 0.f, 0.f)
			, lastSrcLookAt_(0.f, 0.f, 0.f)
		{}

		const glm::mat4& PlanarReflectionCamera::getViewTransform()
		{
			if (!isDirty_ && (lastSrcPos_ != srcCamera_->pos()
				|| lastSrcLookAt_ != srcCamera_->lookAt()
				|| lastSrcUp_ != srcCamera_->up()))
			{
				isDirty_ = true;
			}
			return CameraBase::getViewTransform();
		}

		glm::vec3 reflect(const glm::vec3& p, const model::geo::Plane& reflectionPlane)
		{
			auto dist = 2.f * glm::dot(p - reflectionPlane.origin(), reflectionPlane.normal());
			return p - (reflectionPlane.normal() * dist);
		}

		glm::vec3 PlanarReflectionCamera::pos() const
		{
			return reflect(srcCamera_->pos(), reflectionPlane_);
		}

		glm::vec3 PlanarReflectionCamera::lookAt() const
		{
			return reflect(srcCamera_->lookAt(), reflectionPlane_);
		}

		glm::vec3 PlanarReflectionCamera::up() const
		{
			// Take the source up vector, and reflect directly against the plane. 
			//auto srcUp = srcCamera_->up();
			//auto dist = 2.f * glm::dot(srcUp, reflectionPlane_.normal());
			//return srcUp - reflectionPlane_.normal() * dist
			return srcCamera_->up();
		}

		void PlanarReflectionCamera::reflectionPlane(const model::geo::Plane& plane)
		{
			reflectionPlane_ = plane;
			isDirty_ = true;
		}

		model::geo::Plane PlanarReflectionCamera::reflectionPlane() const
		{
			return reflectionPlane_;
		}
	}
}