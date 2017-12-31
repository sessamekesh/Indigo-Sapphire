#include <util/command/staticcameraparser.h>
#include <util/camera/StaticCamera.h>

namespace util
{
	namespace command
	{
		const std::string StaticCameraParser::uuid = "StaticCameraParser";

		StaticCameraParser::StaticCameraParser(const Vec3Parser& vp)
			: vec3_(vp)
		{}

		bool StaticCameraParser::setValue(const PropertyCommand& cmd, std::shared_ptr<void> ptr)
		{
			if (cmd.subVals.size() == 0u)
			{
				return false;
			}

			auto pass = cmd;
			auto prop = pass.subVals[0];
			pass.subVals.erase(pass.subVals.begin(), pass.subVals.begin() + 1u);

			auto camera = std::reinterpret_pointer_cast<util::camera::StaticCamera>(ptr);
			if (prop == "pos")
			{
				auto v = std::make_shared<glm::vec3>(camera->pos());
				if (!vec3_.setValue(pass, v))
				{
					return false;
				}

				camera->pos(*v);
			}
			else if (prop == "lookAt")
			{
				auto v = std::make_shared<glm::vec3>(camera->lookAt());
				if (!vec3_.setValue(pass, v))
				{
					return false;
				}
				camera->lookAt(*v);
			}
			else if (prop == "up")
			{
				auto v = std::make_shared<glm::vec3>(camera->up());
				if (!vec3_.setValue(pass, v))
				{
					return false;
				}
				camera->up(*v);
			}
			else
			{
				return false;
			}

			return true;
		}

		std::vector<PropertyCommand> StaticCameraParser::serializeValue(std::shared_ptr<void> ptr)
		{
			auto camera = std::reinterpret_pointer_cast<util::camera::StaticCamera>(ptr);
			auto pos = camera->pos();
			auto lookAt = camera->lookAt();
			auto up = camera->up();

			std::vector<PropertyCommand> tr;
			
			auto ppos = std::shared_ptr<glm::vec3>(&pos, [](glm::vec3*) {});
			auto pla = std::shared_ptr<glm::vec3>(&lookAt, [](glm::vec3*) {});
			auto pup = std::shared_ptr<glm::vec3>(&up, [](glm::vec3*) {});

			auto posSerialization = vec3_.serializeValue(ppos);
			auto lookAtSerialization = vec3_.serializeValue(pla);
			auto upSerialization = vec3_.serializeValue(pup);

			for (auto& ps : posSerialization)
			{
				ps.subVals.emplace(ps.subVals.begin(), "pos");
				tr.push_back(ps);
			}
			for (auto&& las : lookAtSerialization)
			{
				las.subVals.emplace(las.subVals.begin(), "lookAt");
				tr.push_back(las);
			}
			for (auto&& us : upSerialization)
			{
				us.subVals.emplace(us.subVals.begin(), "up");
				tr.push_back(us);
			}

			return tr;
		}

		std::string StaticCameraParser::getUUID()
		{
			return StaticCameraParser::uuid;
		}
	}
}