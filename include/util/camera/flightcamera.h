#pragma once

#include <util/camera/camerabase.h>

namespace util
{
	namespace camera
	{
		class FlightCamera : public CameraBase
		{
		public:
			FlightCamera(
				const glm::vec3& pos,
				const glm::vec3& up,
				const glm::vec3& fwdDir,
				float rotation,
				float angle,
				float clamping = 0.05f
			);
			~FlightCamera() = default;
			FlightCamera(const FlightCamera&) = default;

			glm::vec3 pos() const override;
			glm::vec3 lookAt() const override;
			glm::vec3 up() const override;

			void pos(const glm::vec3& pos);
			void lookAt(const glm::vec3& lookAt);
			void up(const glm::vec3& up);
			void fwd(const glm::vec3& fwd);

			void rotateRight(float angle);
			void rotateUp(float angle);

		private:
			void computeRight();

		protected:
			glm::vec3 pos_;
			glm::vec3 up_;

			glm::vec3 fwd_;
			glm::vec3 right_;
			float rotation_;
			float angle_;
			float clamping_;
		};
	}
}