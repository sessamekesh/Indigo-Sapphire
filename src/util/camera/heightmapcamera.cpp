#include <util/camera/heightmapcamera.h>

#include <glm/gtx/quaternion.hpp>

namespace util
{
	namespace camera
	{
		HeightmapCamera::HeightmapCamera(
			const glm::vec3& pos,
			const glm::vec3& up,
			const glm::vec3& fwdDir,
			std::shared_ptr<input::CameraControllerBase> controller,
			std::shared_ptr<model::specialgeo::Heightfield> heightField,
			float heightAboveGround,
			float rotation,
			float angle,
			float movementSpeed,
			float rotationSpeed,
			float clamping
		)
			: pos_(pos)
			, up_(up)
			, fwd_(fwdDir)
			, controller_(controller)
			, heightField_(heightField)
			, right_()
			, rotation_(rotation)
			, angle_(angle)
			, clamping_(clamping)
			, height_(heightAboveGround)
			, movementSpeed_(movementSpeed)
			, rotationSpeed_(rotationSpeed)
		{
			computeRight();
		}

		void HeightmapCamera::update(float dt)
		{
			// TODO SESS: The controller shouldn't update here
			controller_->tick();
			if (!controller_->isActive())
			{
				return;
			}

			glm::vec3 fwd = glm::angleAxis(rotation_, up_) * fwd_;
			glm::vec3 right = glm::cross(fwd, up_);
			
			auto movement = (fwd * controller_->getY1Axis() + right * controller_->getX1Axis()) * movementSpeed_ * dt;
			auto nextPos = pos() + movement;
			nextPos.y = heightField_->heightAtPos(nextPos.x, nextPos.z, 0.f, false) + height_;

			pos(nextPos);
			rotateRight(-rotationSpeed_ * dt * controller_->getX2Axis());
			rotateUp(-rotationSpeed_ * dt * controller_->getY2Axis());
		}

		glm::vec3 HeightmapCamera::pos() const
		{
			return pos_;
		}

		glm::vec3 HeightmapCamera::lookAt() const
		{
			return pos_ + glm::angleAxis(rotation_, up_) * glm::angleAxis(angle_, right_) * fwd_;
		}

		glm::vec3 HeightmapCamera::up() const
		{
			return up_;
		}

		float HeightmapCamera::heightAboveGround() const
		{
			return height_;
		}

		void HeightmapCamera::pos(const glm::vec3& p)
		{
			pos_ = p;
			isDirty_ = true;
		}

		void HeightmapCamera::lookAt(const glm::vec3& la)
		{
			auto toCamera = la - pos_;
			auto onFlatPlane = glm::normalize(glm::cross(glm::cross(up_, toCamera), up_));

			angle_ = glm::angle(glm::rotation(onFlatPlane, toCamera));
			rotation_ = glm::angle(glm::rotation(fwd_, onFlatPlane));
			isDirty_ = true;
		}

		void HeightmapCamera::computeRight()
		{
			right_ = glm::cross(up_, fwd_);
		}

		void HeightmapCamera::up(const glm::vec3& up)
		{
			up_ = glm::normalize(up);
			computeRight();
			isDirty_ = true;
		}

		void HeightmapCamera::fwd(const glm::vec3& fwd)
		{
			fwd_ = glm::normalize(fwd);
			computeRight();
			isDirty_ = true;
		}

		void HeightmapCamera::rotateRight(float angle)
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

		void HeightmapCamera::rotateUp(float angle)
		{
			angle_ += angle;
			angle_ = glm::min(angle_, glm::half_pi<float>() - clamping_);
			angle_ = glm::max(angle_, clamping_ - glm::half_pi<float>());
			isDirty_ = true;
		}

		void HeightmapCamera::heightAboveGround(float height)
		{
			height_ = height;
		}
	}
}