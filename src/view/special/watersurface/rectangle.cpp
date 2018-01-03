#include <view/special/watersurface/rectangle.h>

namespace view
{
	namespace special
	{
		namespace watersurface
		{
			Rectangle::Rectangle(std::shared_ptr<model::geo::Rectangle> model)
				: model_(model)
				, isReady_(false)
				, vao_(0u)
				, vertexBuffer_(0u)
			{}

			Rectangle::~Rectangle()
			{
				if (isReady_)
				{
					release();
				}
			}

			bool Rectangle::prepare(std::shared_ptr<WaterSurfaceShader> shader, util::PipelineState& pso)
			{
				if (isReady_)
				{
					return false;
				}

				glGenVertexArrays(1, &vao_);
				glBindVertexArray(vao_);

				glGenBuffers(1, &vertexBuffer_);
				glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
				
				auto&& verts = getVerts();
				glBufferStorage(
					GL_ARRAY_BUFFER,
					verts.size() * sizeof(Vertex),
					&verts[0],
					0x00
				);
				shader->setVertexAttribPointers(pso);

				glBindVertexArray(NULL);
				glBindBuffer(GL_ARRAY_BUFFER, NULL);

				isReady_ = true;
				return true;
			}

			bool Rectangle::release()
			{
				if (vertexBuffer_)
				{
					glDeleteBuffers(1, &vertexBuffer_);
					vertexBuffer_ = 0u;
				}

				if (vao_)
				{
					glDeleteVertexArrays(1, &vao_);
					vao_ = 0u;
				}

				isReady_ = false;
				return true;
			}

			void Rectangle::render(std::shared_ptr<WaterSurfaceShader> shader)
			{
				shader->setWorldMatrix(model_->worldTransform());
				glBindVertexArray(vao_);
				glDrawArrays(GL_TRIANGLES, 0, 6);
				glBindVertexArray(NULL);
			}

			std::vector<Vertex> Rectangle::getVerts()
			{
				std::vector<Vertex> tr;

				tr.push_back({ { -model_->width(), 0.f, -model_->height() } });
				tr.push_back({ { -model_->width(), 0.f,  model_->height() } });
				tr.push_back({ {  model_->width(), 0.f, -model_->height() } });

				tr.push_back({ { -model_->width(), 0.f,  model_->height() } });
				tr.push_back({ {  model_->width(), 0.f, -model_->height() } });
				tr.push_back({ {  model_->width(), 0.f,  model_->height() } });

				return tr;
			}
		}
	}
}