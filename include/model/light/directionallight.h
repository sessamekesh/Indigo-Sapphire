#pragma once

#include <glm/glm.hpp>

namespace model
{
	namespace light
	{
		class DirectionalLight
		{
		public:
			DirectionalLight(
				const glm::vec3& ambientColor,
				const glm::vec3& diffuseColor,
				const glm::vec3& specularColor,
				const glm::vec3& direction
			);
			~DirectionalLight() = default;
			DirectionalLight(const DirectionalLight&) = default;

			glm::vec3 diffuse() const;
			void diffuse(const glm::vec3& d);

			glm::vec3 ambient() const;
			void ambient(const glm::vec3& a);

			glm::vec3 specular() const;
			void specular(const glm::vec3& s);

			glm::vec3 direction() const;
			void direction(const glm::vec3& d);

		protected:
			glm::vec3 ambientColor_;
			glm::vec3 diffuseColor_;
			glm::vec3 specularColor_;
			glm::vec3 direction_;
		};
	}
}