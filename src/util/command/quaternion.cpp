#include <util/command/quaternion.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace util
{
	namespace command
	{
		const std::string QuaternionParser::uuid = "QuaternionParser";

		QuaternionParser::QuaternionParser(
			const FloatParser& fp,
			const Vec3Parser& v3p
		)
			: float_(fp)
			, vec3_(v3p)
		{}

		bool QuaternionParser::setValue(const PropertyCommand & cmd, std::shared_ptr<void> ptr)
		{
			// Two properties: "axis" and "angle"
			// Cannot set quaternions directly.
			if (cmd.subVals.size() < 1u)
			{
				return false;
			}

			auto quatPtr = std::reinterpret_pointer_cast<glm::quat>(ptr);
			if (cmd.subVals[0] == "axis")
			{
				glm::vec3 axis = glm::axis(*quatPtr);
				auto axisPtr = std::make_shared<glm::vec3>(axis);
				auto nextCmd = cmd;
				nextCmd.subVals.erase(nextCmd.subVals.begin());
				if (!vec3_.setValue(nextCmd, axisPtr))
				{
					return false;
				}

				auto angle = glm::angle(*quatPtr);
				*quatPtr = glm::angleAxis(angle, *axisPtr);
			}
			else if (cmd.subVals[0] == "angle")
			{
				auto axis = glm::axis(*quatPtr);
				auto angle = std::make_shared<float>();
				auto nextCmd = cmd;
				nextCmd.subVals.erase(nextCmd.subVals.begin());
				if (!float_.setValue(nextCmd, angle))
				{
					return false;
				}
				*quatPtr = glm::angleAxis(*angle, axis);
			}
			else
			{
				return false;
			}

			return true;
		}

		std::vector<PropertyCommand> QuaternionParser::serializeValue(std::shared_ptr<void> ptr)
		{
			auto qp = std::reinterpret_pointer_cast<glm::quat>(ptr);
			auto axisPtr = std::make_shared<glm::vec3>(glm::axis(*qp));
			auto anglePtr = std::make_shared<float>(glm::angle(*qp));
			auto axisVal = vec3_.serializeValue(axisPtr);
			auto angleVal = float_.serializeValue(anglePtr);

			std::vector<PropertyCommand> tr;

			for (auto av : axisVal)
			{
				av.subVals.emplace(av.subVals.begin(), "axis");
				tr.push_back(av);
			}

			for (auto av : angleVal)
			{
				av.subVals.emplace(av.subVals.begin(), "angle");
				tr.push_back(av);
			}

			return tr;
		}

		std::string QuaternionParser::getUUID()
		{
			return QuaternionParser::uuid;
		}
	}
}