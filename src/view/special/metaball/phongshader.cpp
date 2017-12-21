#include <glad/glad.h>

#include <view/special/metaball/phongshader.h>
#include <fstream>
#include <resources.h>
#include <util/io.h>
#include <glm/gtc/type_ptr.hpp>

namespace view
{
	namespace special
	{
		namespace metaball
		{
			PhongShader::PhongShader()
				: isLoaded_(false)
				, program_(0u)
				, log(util::DEBUG, util::WARN, "[pg:PhongMetaball] ")
			{}

			bool PhongShader::initialize()
			{
				if (isLoaded_)
				{
					log.warn << "Attempted to initialize an already loaded shader" << util::endl;
					return false;
				}

				auto vsTextFuture = util::readFileAsync(SHADER_PATH("metaball-phong.vert"));
				auto fsTextFuture = util::readFileAsync(SHADER_PATH("metaball-phong.frag"));

				std::string vsText = vsTextFuture.get();
				// TODO SESS: Continue here!
			}
		}
	}
}