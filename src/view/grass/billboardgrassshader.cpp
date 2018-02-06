#include <view/grass/billboardgrassshader.h>
#include <util/io.h>
#include <resources.h>
#include <glm/gtc/type_ptr.hpp>

namespace view
{
	namespace grass
	{
		BillboardGrassShader::BillboardGrassShader()
			: view::Shader("[Billboard Grass Shader ]", SHADER_PATH("grass.vert"), SHADER_PATH("grass.frag"), SHADER_PATH("grass.geom"))
			, uniformLocations_({})
		{}

		void BillboardGrassShader::setWorldMatrix(const glm::mat4& w)
		{
			glUniformMatrix4fv(uniformLocations_.matWorld, 1, GL_FALSE, glm::value_ptr(w));
		}

		void BillboardGrassShader::setViewMatrix(const glm::mat4& v)
		{
			glUniformMatrix4fv(uniformLocations_.matView, 1, GL_FALSE, glm::value_ptr(v));
		}

		void BillboardGrassShader::setProjMatrix(const glm::mat4& p)
		{
			glUniformMatrix4fv(uniformLocations_.matProj, 1, GL_FALSE, glm::value_ptr(p));
		}

		void BillboardGrassShader::setPatchSizeData(float minPatchHeight, float maxPatchHeight, float patchSize)
		{
			glUniform1f(uniformLocations_.minPatchHeight, minPatchHeight);
			glUniform1f(uniformLocations_.maxPatchHeight, maxPatchHeight);
			glUniform1f(uniformLocations_.patchSize, patchSize);
		}

		void BillboardGrassShader::setWind(float windStrength)
		{
			glUniform1f(uniformLocations_.windStrength, windStrength);
		}

		void BillboardGrassShader::setTimePassed(float timePassed)
		{
			glUniform1f(uniformLocations_.fTimePassed, timePassed);
		}

		void BillboardGrassShader::setTexture(std::shared_ptr<view::Texture> tex)
		{
			glActiveTexture(GL_TEXTURE0);
			glUniform1i(uniformLocations_.gSampler, 0);
			glBindTexture(GL_TEXTURE_2D, tex->texture());
		}

		void BillboardGrassShader::setColor(const glm::vec4& color)
		{
			glUniform4fv(uniformLocations_.vColor, 1, glm::value_ptr(color));
		}
		
		void BillboardGrassShader::setEyePos(const glm::vec3& pos)
		{
			glUniform3fv(uniformLocations_.vEyePosition, 1, glm::value_ptr(pos));
		}

		void BillboardGrassShader::setAlphaTest(float test, float multiplier)
		{
			glUniform1f(uniformLocations_.fAlphaTest, test);
			glUniform1f(uniformLocations_.fAlphaMultiplier, multiplier);
		}

		void BillboardGrassShader::setClipPlane(const model::geo::Plane& plane)
		{
			glUniform3fv(uniformLocations_.clipPlaneNormal, 1, glm::value_ptr(plane.normal()));
			glUniform3fv(uniformLocations_.clipPlaneOrigin, 1, glm::value_ptr(plane.origin()));
		}

		bool BillboardGrassShader::getUniformLocations()
		{
			uniformLocations_.matWorld = glGetUniformLocation(program_, "matWorld");
			uniformLocations_.matView = glGetUniformLocation(program_, "matView");
			uniformLocations_.matProj = glGetUniformLocation(program_, "matProj");
			uniformLocations_.minPatchHeight = glGetUniformLocation(program_, "minPatchHeight");
			uniformLocations_.maxPatchHeight = glGetUniformLocation(program_, "maxPatchHeight");
			uniformLocations_.patchSize = glGetUniformLocation(program_, "patchSize");
			uniformLocations_.windStrength = glGetUniformLocation(program_, "windStrength");
			uniformLocations_.fTimePassed = glGetUniformLocation(program_, "fTimePassed");
			uniformLocations_.gSampler = glGetUniformLocation(program_, "gSampler");
			uniformLocations_.vColor = glGetUniformLocation(program_, "vColor");
			uniformLocations_.vEyePosition = glGetUniformLocation(program_, "vEyePosition");
			uniformLocations_.fAlphaTest = glGetUniformLocation(program_, "fAlphaTest");
			uniformLocations_.fAlphaMultiplier = glGetUniformLocation(program_, "fAlphaMultiplier");
			uniformLocations_.clipPlaneNormal = glGetUniformLocation(program_, "clipPlaneNormal");
			uniformLocations_.clipPlaneOrigin = glGetUniformLocation(program_, "clipPlaneOrigin");

			return true;
		}

		void BillboardGrassShader::setVertexAttribPointersInternal()
		{
			glVertexAttribPointer(0u, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
		}

		unsigned int BillboardGrassShader::getNumVertexAttribPointers()
		{
			return 1u;
		}
	}
}