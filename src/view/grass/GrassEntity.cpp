#include <view/grass/GrassEntity.h>
#include <random>

// Grass patches are drawn as a point list - a geometry shader is used to generate
//  the actual grass geometry.

namespace view
{
	namespace grass
	{
		GrassEntity::GrassEntity(glm::vec3 pos, glm::quat rot, glm::vec3 scl)
			: model::WithWorldTransform(pos, rot, scl)
			, isReady_(false)
			, vao_(NULL)
			, vb_(NULL)
			, log(util::DEBUG, util::DEBUG, "[GrassEntity ]")
			, timeElapsed_(0.f)
			, numPoints_(0u)
		{}

		GrassEntity::~GrassEntity()
		{
			release();
		}

		bool GrassEntity::prepare(
			std::shared_ptr<view::grass::GrassShader> shader,
			util::PipelineState& pso,
			std::shared_ptr<model::specialgeo::Heightfield> heightfield,
			std::shared_ptr<view::Texture> grassTexture,
			std::shared_ptr<util::SurfaceMaskBase> mask,
			std::uint32_t seed,
			float patchOffsetMin,
			float patchOffsetMax,
			float grassPatchHeight
		) {
			if (isReady_)
			{
				log << "Already prepared - releasing previous entity" << util::endl;
				release();
			}

			std::srand(seed);

			std::vector<GrassShader::Vertex> verts;
			verts.reserve(1024u);
			float dx = 5.f, dz = 5.f;
			for (float x = -heightfield->width(); x < heightfield->width(); x += (dx = patchOffsetMin + (patchOffsetMax - patchOffsetMin) * float(std::rand() % 1000) * 0.001f))
			{
				for (float z = -heightfield->depth(); z < heightfield->depth(); z += (dz = patchOffsetMin + (patchOffsetMax - patchOffsetMin) * float(std::rand() % 1000) * 0.001f))
				{
					// Fuzz the x and z values
					float finalX = x + ((std::rand() % 1000 - 500) * 0.001f * dx);
					float finalZ = z + ((std::rand() % 1000 - 500) * 0.001f * dz);

					// Skip this one if it's not in the mask.
					if (mask->getMaskValue({ finalX, finalZ }))
					{
						verts.push_back({ { finalX, heightfield->heightAtPos(finalX, finalZ, 1.f), finalZ} });
					}
				}
			}

			std::cout << "Number of vertices: " << verts.size() << std::endl;

			glGenVertexArrays(1, &vao_);
			glBindVertexArray(vao_);

			glGenBuffers(1, &vb_);
			glBindBuffer(GL_ARRAY_BUFFER, vb_);
			glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(GrassShader::Vertex), &verts[0], GL_STATIC_DRAW);
			numPoints_ = verts.size();

			if (!shader->setVertexAttribs(pso))
			{
				return false;
			}

			tex_ = grassTexture;

			glBindVertexArray(NULL);

			isReady_ = true;
		}

		bool GrassEntity::release()
		{
			if (vb_ != NULL)
			{
				glDeleteBuffers(1, &vb_);
				vb_ = NULL;
			}

			if (vao_ != NULL)
			{
				glDeleteVertexArrays(1, &vao_);
				vao_ = NULL;
			}

			return true;
		}

		void GrassEntity::update(float dt)
		{
			timeElapsed_ += dt;
		}

		bool GrassEntity::render(std::shared_ptr<view::grass::GrassShader> shader)
		{
			shader->setWorldMatrix(worldTransform());
			shader->setTimePassed(timeElapsed_);
			shader->setTexture(tex_);
			// TODO SESS: These should be configurable.
			shader->setAlphaTest(0.25f, 1.5f);
			shader->setColor(glm::vec4({ 1.f, 1.f, 1.f, 1.f }));
			shader->setPatchSizeData(0.15f, 0.35f, 0.15f);
			shader->setWind(0.15f);

			glBindVertexArray(vao_);
			glDrawArrays(GL_POINTS, 0, numPoints_);

			return true;
		}
	}
}