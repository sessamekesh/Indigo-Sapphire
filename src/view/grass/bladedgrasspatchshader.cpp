#include <view/grass/bladedgrasspatchshader.h>
#include <resources.h>
#include <glm/gtc/type_ptr.hpp>

namespace view
{
	namespace grass
	{
		BladedGrassPatchShader::BladedGrassPatchShader()
			: view::Shader("[BladedGrassPatchShader ]", SHADER_PATH("bladedgrass.vert"), SHADER_PATH("bladedgrass.frag"), SHADER_PATH("bladedgrass.geom"))
			, uniformLocations_({})
			, perSceneBlockG_()
			, perFrameBlockG_()
			, perInstanceBlockG_()
			, perSceneBlockF_()
			, perFrameBlockF_()
		{}

		BladedGrassPatchShader::~BladedGrassPatchShader()
		{
			Shader::~Shader();
			perSceneBlockG_.release();
			perFrameBlockG_.release();
			perInstanceBlockG_.release();
			perSceneBlockF_.release();
			perFrameBlockF_.release();
		}

		bool BladedGrassPatchShader::getUniformLocations()
		{
			uniformLocations_.PerSceneBlockG = glGetUniformBlockIndex(program_, "PerSceneBlockG");
			uniformLocations_.PerFrameBlockG = glGetUniformBlockIndex(program_, "PerFrameBlockG");
			uniformLocations_.PerInstanceBlockG = glGetUniformBlockIndex(program_, "PerInstanceBlockG");
			uniformLocations_.PerSceneBlockF = glGetUniformBlockIndex(program_, "PerSceneBlockF");
			uniformLocations_.PerFrameBlockF = glGetUniformBlockIndex(program_, "PerFrameBlockF");
			uniformLocations_.texGrassBlade = glGetUniformLocation(program_, "texGrassBlade");

			glUniformBlockBinding(program_, uniformLocations_.PerSceneBlockG, 0);
			glUniformBlockBinding(program_, uniformLocations_.PerFrameBlockG, 1);
			glUniformBlockBinding(program_, uniformLocations_.PerInstanceBlockG, 2);
			glUniformBlockBinding(program_, uniformLocations_.PerSceneBlockF, 3);
			glUniformBlockBinding(program_, uniformLocations_.PerFrameBlockF, 4);

			return true;
		}

		void BladedGrassPatchShader::setVertexAttribPointersInternal()
		{
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
			glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
			glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(4 * sizeof(float)));
			glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5 * sizeof(float)));
			glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
			glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(7 * sizeof(float)));
		}

		unsigned int BladedGrassPatchShader::getNumVertexAttribPointers()
		{
			return 6u;
		}

		bool BladedGrassPatchShader::initializeInternal()
		{
			if (!perSceneBlockG_.prepare()) return false;
			if (!perFrameBlockG_.prepare()) return false;
			if (!perInstanceBlockG_.prepare()) return false;
			if (!perSceneBlockF_.prepare()) return false;
			if (!perFrameBlockF_.prepare()) return false;

			return true;
		}

		void BladedGrassPatchShader::setWorldMatrix(const glm::mat4& w)
		{
			// We are assuming that this will always change, so skip the comparison here.
			//  That might be an incorrect assumption.
			perInstanceBlockG_.data.matWorld = w;
			perInstanceBlockG_.markDirty();
		}

		void BladedGrassPatchShader::setViewMatrix(const glm::mat4& v)
		{
			if (perFrameBlockG_.data.matView != v)
			{
				perFrameBlockG_.data.matView = v;
				perFrameBlockG_.markDirty();
			}
		}

		void BladedGrassPatchShader::setProjMatrix(const glm::mat4& p)
		{
			if (perSceneBlockG_.data.matProj != p)
			{
				perSceneBlockG_.data.matProj = p;
				perSceneBlockG_.markDirty();
			}
		}

		void BladedGrassPatchShader::setWind(float windStrength)
		{
			if (perSceneBlockG_.data.windStrength != windStrength)
			{
				perSceneBlockG_.data.windStrength = windStrength;
				perSceneBlockG_.markDirty();
			}
		}

		void BladedGrassPatchShader::setTimeElapsed(float timeElapsed)
		{
			if (perFrameBlockG_.data.fTimeElapsed != timeElapsed)
			{
				perFrameBlockG_.data.fTimeElapsed = timeElapsed;
				perFrameBlockG_.markDirty();
			}
		}

		void BladedGrassPatchShader::setGrassBladeTexture(std::shared_ptr<view::Texture> tex)
		{
			glUniform1i(uniformLocations_.texGrassBlade, 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex->texture());
		}

		void BladedGrassPatchShader::setLight(const model::light::DirectionalLight& light)
		{
			if (perSceneBlockF_.data.lightAmbient != glm::vec4(light.ambient(), 1.0))
			{
				perSceneBlockF_.data.lightAmbient = glm::vec4(light.ambient(), 1.0);
				perSceneBlockF_.markDirty();
			}

			if (perSceneBlockF_.data.lightDiffuse != glm::vec4(light.diffuse(), 1.0))
			{
				perSceneBlockF_.data.lightDiffuse = glm::vec4(light.diffuse(), 1.0);
				perSceneBlockF_.markDirty();
			}

			if (perSceneBlockF_.data.lightSpecular != glm::vec4(light.specular(), 1.0))
			{
				perSceneBlockF_.data.lightSpecular = glm::vec4(light.diffuse(), 1.0);
				perSceneBlockF_.markDirty();
			}

			if (perSceneBlockF_.data.lightDirection != light.direction())
			{
				perSceneBlockF_.data.lightDirection = light.direction();
				perSceneBlockF_.markDirty();
			}
		}

		void BladedGrassPatchShader::setCameraPosition(const glm::vec3& pos)
		{
			if (perFrameBlockF_.data.vCameraPos != pos)
			{
				perFrameBlockF_.data.vCameraPos = pos;
				perFrameBlockF_.markDirty();
			}
		}

		void BladedGrassPatchShader::setClipPlane(const model::geo::Plane& plane)
		{
			if (perFrameBlockG_.data.clipPlaneNormal != plane.normal())
			{
				perFrameBlockG_.data.clipPlaneNormal = plane.normal();
				perFrameBlockG_.markDirty();
			}

			if (perFrameBlockG_.data.clipPlaneOrigin != plane.origin())
			{
				perFrameBlockG_.data.clipPlaneOrigin = plane.origin();
				perFrameBlockG_.markDirty();
			}
		}

		void BladedGrassPatchShader::setBladeSpecularColor(const glm::vec4& c)
		{
			if (perSceneBlockF_.data.cBladeSpecular != c)
			{
				perSceneBlockF_.data.cBladeSpecular = c;
				perSceneBlockF_.markDirty();
			}
		}

		void BladedGrassPatchShader::submitUBOs()
		{
			perSceneBlockG_.sync();
			perFrameBlockG_.sync();
			perInstanceBlockG_.sync();
			perSceneBlockF_.sync();
			perFrameBlockF_.sync();

			glBindBufferBase(GL_UNIFORM_BUFFER, 0, perSceneBlockG_.ubo());
			glBindBufferBase(GL_UNIFORM_BUFFER, 1, perFrameBlockG_.ubo());
			glBindBufferBase(GL_UNIFORM_BUFFER, 2, perInstanceBlockG_.ubo());
			glBindBufferBase(GL_UNIFORM_BUFFER, 3, perSceneBlockF_.ubo());
			glBindBufferBase(GL_UNIFORM_BUFFER, 4, perFrameBlockF_.ubo());
		}
	}
}