#pragma once

#include <glad/glad.h>
#include <model/geo/rectangle.h>
#include <memory>
#include <view/special/watersurface/watersurfaceshader.h>
#include <vector>

namespace view
{
	namespace special
	{
		namespace watersurface
		{
			class Rectangle
			{
			public:
				Rectangle(
					std::shared_ptr<model::geo::Rectangle> model
				);
				~Rectangle();
				Rectangle(const Rectangle&) = delete;

				bool prepare(std::shared_ptr<WaterSurfaceShader> shader, util::PipelineState& pso);
				bool release();

				void render(std::shared_ptr<WaterSurfaceShader> shader);

			private:
				std::vector<Vertex> getVerts();

			private:
				std::shared_ptr<model::geo::Rectangle> model_;
				bool isReady_;
				GLuint vao_;
				GLuint vertexBuffer_;
			};
		}
	}
}