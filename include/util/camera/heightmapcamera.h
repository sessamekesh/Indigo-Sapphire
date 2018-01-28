#pragma once

#include <util/camera/camerabase.h>
#include <input/cameracontrollerbase.h>
#include <memory>
#include <model/specialgeo/heightfield.h>

namespace util
{
	namespace camera
	{
		// TODO SESS: Continue here, make a camera that follows a heightmapped terrain
		class HeightmapCamera : public CameraBase
		{
		public:
			HeightmapCamera(
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
				float clamping = 0.05f
			);
			HeightmapCamera(const HeightmapCamera&) = default;
			~HeightmapCamera() = default;

			void update(float dt);

			glm::vec3 pos() const override;
			glm::vec3 lookAt() const override;
			glm::vec3 up() const override;
			float heightAboveGround() const;

			void pos(const glm::vec3& pos);
			void lookAt(const glm::vec3& lookAt);
			void up(const glm::vec3& up);
			void fwd(const glm::vec3& fwd);
			void heightAboveGround(float height);

			void rotateRight(float angle);
			void rotateUp(float angle);

		private:
			void computeRight();

		private:
			std::shared_ptr<input::CameraControllerBase> controller_;
			std::shared_ptr<model::specialgeo::Heightfield> heightField_;

		protected:
			glm::vec3 pos_;
			glm::vec3 up_;
			glm::vec3 fwd_;
			glm::vec3 right_;
			float rotation_;
			float angle_;
			float clamping_;
			float height_;
			
			float movementSpeed_;
			float rotationSpeed_;
		};
	}
}