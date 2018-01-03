#include <util/command/directionallightparser.h>
#include <model/light/directionallight.h>

namespace util
{
	namespace command
	{
		const std::string DirectionalLightParser::uuid("DirectionalLight");

		DirectionalLightParser::DirectionalLightParser(
			const Vec3Parser& vp
		)
			: vec3_(vp)
		{}

		bool DirectionalLightParser::setValue(const PropertyCommand& cmd, std::shared_ptr<void> ptr)
		{
			if (cmd.subVals.size() < 1u)
			{
				return false;
			}

			auto pass = cmd;
			auto prop = pass.subVals[0];
			pass.subVals.erase(pass.subVals.begin());

			auto lightPtr = std::static_pointer_cast<model::light::DirectionalLight>(ptr);
			if (prop == "direction")
			{
				auto v = std::make_shared<glm::vec3>();
				if (!vec3_.setValue(pass, v))
				{
					return false;
				}
				lightPtr->direction(*v);
			}
			else if (prop == "diffuse")
			{
				auto v = std::make_shared<glm::vec3>();
				if (!vec3_.setValue(pass, v))
				{
					return false;
				}
				lightPtr->diffuse(*v);
			}
			else if (prop == "ambient")
			{
				auto v = std::make_shared<glm::vec3>();
				if (!vec3_.setValue(pass, v))
				{
					return false;
				}
				lightPtr->ambient(*v);
			}
			else if (prop == "specular")
			{
				auto v = std::make_shared<glm::vec3>();
				if (!vec3_.setValue(pass, v))
				{
					return false;
				}
				lightPtr->specular(*v);
			}
			else
			{
				return false;
			}
		}

		std::vector<PropertyCommand> DirectionalLightParser::serializeValue(std::shared_ptr<void> ptr)
		{
			auto lightPtr = std::static_pointer_cast<model::light::DirectionalLight>(ptr);
			
			auto ambientPtr = std::make_shared<glm::vec3>(lightPtr->ambient());
			auto diffusePtr = std::make_shared<glm::vec3>(lightPtr->diffuse());
			auto directionPtr = std::make_shared<glm::vec3>(lightPtr->direction());
			auto specularPtr = std::make_shared<glm::vec3>(lightPtr->specular());

			std::vector<PropertyCommand> tr;

			for (auto as : vec3_.serializeValue(ambientPtr))
			{
				as.subVals.emplace(as.subVals.begin(), "ambient");
				tr.push_back(as);
			}

			for (auto diffs : vec3_.serializeValue(diffusePtr))
			{
				diffs.subVals.emplace(diffs.subVals.begin(), "diffuse");
				tr.push_back(diffs);
			}

			for (auto ss : vec3_.serializeValue(specularPtr))
			{
				ss.subVals.emplace(ss.subVals.begin(), "specular");
				tr.push_back(ss);
			}

			for (auto ds : vec3_.serializeValue(directionPtr))
			{
				ds.subVals.emplace(ds.subVals.begin(), "direction");
				tr.push_back(ds);
			}

			return tr;
		}

		std::string DirectionalLightParser::getUUID()
		{
			return DirectionalLightParser::uuid;
		}
	}
}