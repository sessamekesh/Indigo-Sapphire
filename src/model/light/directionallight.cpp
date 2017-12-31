#include <model/light/directionallight.h>

namespace model
{
	namespace light
	{
		DirectionalLight::DirectionalLight(
			const glm::vec3& ac,
			const glm::vec3& dc,
			const glm::vec3& sc,
			const glm::vec3& dir
		)
			: ambientColor_(ac)
			, diffuseColor_(dc)
			, specularColor_(sc)
			, direction_(dir)
		{
			if (dir.x == 0.f && dir.y == 0.f && dir.z == 0.f)
			{
				direction_ = { 0.f, -1.f, 0.f };
			}
			else
			{
				direction_ = glm::normalize(dir);
			}
		}

		glm::vec3 DirectionalLight::diffuse() const
		{
			return diffuseColor_;
		}

		void DirectionalLight::diffuse(const glm::vec3& d)
		{
			diffuseColor_ = d;
		}

		glm::vec3 DirectionalLight::ambient() const
		{
			return ambientColor_;
		}

		void DirectionalLight::ambient(const glm::vec3& a)
		{
			ambientColor_ = a;
		}

		glm::vec3 DirectionalLight::specular() const
		{
			return specularColor_;
		}

		void DirectionalLight::specular(const glm::vec3& s)
		{
			specularColor_ = s;
		}

		glm::vec3 DirectionalLight::direction() const
		{
			return direction_;
		}

		void DirectionalLight::direction(const glm::vec3& d)
		{
			if (d.x == 0.f && d.y == 0.f && d.z == 0.f)
			{
				return;
			}

			direction_ = glm::normalize(d);
		}
	}
}