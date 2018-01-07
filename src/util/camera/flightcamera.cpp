#include <util/camera/flightcamera.h>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace util
{
	namespace camera
	{
		FlightCamera::FlightCamera(
			const glm::vec3& pos,
			const glm::vec3& up,
			const glm::vec3& fwdDir,
			float rotation,
			float angle,
			float clamping
		)
			: pos_(pos)
			, up_(glm::normalize(up))
			, fwd_(glm::normalize(fwdDir))
			, right_(glm::vec3(0.f, 0.f, 0.f))
			, rotation_(rotation)
			, angle_(angle)
			, clamping_(glm::max(glm::min(clamping, glm::half_pi<float>()), 0.f))
		{
			computeRight();
		}

		glm::vec3 FlightCamera::pos() const
		{
			return pos_;
		}

		glm::vec3 FlightCamera::lookAt() const
		{
			return pos_ + glm::angleAxis(rotation_, up_) * glm::angleAxis(angle_, right_)* fwd_;
		}

		glm::vec3 FlightCamera::up() const
		{
			return up_;
		}

		void FlightCamera::pos(const glm::vec3& p)
		{
			pos_ = p;
			isDirty_ = true;
		}

		void FlightCamera::lookAt(const glm::vec3& la)
		{
			auto toCamera = la - pos_;
			auto onFlatPlane = glm::normalize(glm::cross(glm::cross(up_, toCamera), up_));

			angle_ = glm::angle(glm::rotation(onFlatPlane, toCamera));
			rotation_ = glm::angle(glm::rotation(fwd_, onFlatPlane));
			isDirty_ = true;
		}

		void FlightCamera::computeRight()
		{
			right_ = glm::cross(up_, fwd_);
		}

		void FlightCamera::up(const glm::vec3& up)
		{
			up_ = glm::normalize(up);
			computeRight();
			isDirty_ = true;
		}

		void FlightCamera::fwd(const glm::vec3& fwd)
		{
			fwd_ = glm::normalize(fwd);
			computeRight();
			isDirty_ = true;
		}

		void FlightCamera::rotateRight(float angle)
		{
			rotation_ += angle;
			while (rotation_ > glm::two_pi<float>())
			{
				rotation_ -= glm::two_pi<float>();
			}
			while (rotation_ < 0.f)
			{
				rotation_ += glm::two_pi<float>();
			}
			isDirty_ = true;
		}

		void FlightCamera::rotateUp(float angle)
		{
			angle_ += angle;
			angle_ = glm::min(angle_, glm::half_pi<float>() - clamping_);
			angle_ = glm::max(angle_, clamping_ - glm::half_pi<float>());
			isDirty_ = true;
		}
	}
}