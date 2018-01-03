#include <util/command/withworldtransformparser.h>
#include <model/withworldtransform.h>
#include <sstream>
#include <string>

namespace util
{
	namespace command
	{
		const std::string WithWorldTransformParser::uuid("WithWorldTransform");

		WithWorldTransformParser::WithWorldTransformParser(
			const Vec3Parser& v3p,
			const QuaternionParser& q
		)
			: vec3_(v3p)
			, quaternion_(q)
		{}

		bool WithWorldTransformParser::setValue(const PropertyCommand& cmd, std::shared_ptr<void> ptr)
		{
			if (cmd.subVals.size() < 1)
			{
				return false;
			}

			auto wwtPtr = std::static_pointer_cast<model::WithWorldTransform>(ptr);

			if (cmd.subVals[0] == "pos")
			{
				glm::vec3 pos = wwtPtr->pos();
				auto nextCmd = cmd;
				auto newPos = std::make_shared<glm::vec3>(pos);
				nextCmd.subVals.erase(nextCmd.subVals.begin());
				if (!vec3_.setValue(nextCmd, newPos))
				{
					return false;
				}
				wwtPtr->pos(*newPos);
			}
			else if (cmd.subVals[0] == "rot")
			{
				glm::quat rot = wwtPtr->rot();
				auto nextCmd = cmd;
				nextCmd.subVals.erase(nextCmd.subVals.begin());
				auto newRot = std::make_shared<glm::quat>(rot);
				if (!quaternion_.setValue(nextCmd, newRot))
				{
					return false;
				}
				wwtPtr->rot(*newRot);
			}
			else if (cmd.subVals[0] == "scl")
			{
				glm::vec3 scl = wwtPtr->scl();
				auto nextCmd = cmd;
				nextCmd.subVals.erase(nextCmd.subVals.begin());
				auto newScl = std::make_shared<glm::vec3>(scl);
				if (!vec3_.setValue(nextCmd, newScl))
				{
					return false;
				}
				wwtPtr->scl(*newScl);
			}
			else
			{
				return false;
			}

			return true;
		}

		std::vector<PropertyCommand> WithWorldTransformParser::serializeValue(std::shared_ptr<void> ptr)
		{
			auto wwtPtr = std::static_pointer_cast<model::WithWorldTransform>(ptr);
			auto posPtr = std::make_shared<glm::vec3>(wwtPtr->pos());
			auto rotPtr = std::make_shared<glm::quat>(wwtPtr->rot());
			auto sclPtr = std::make_shared<glm::vec3>(wwtPtr->scl());

			std::vector<PropertyCommand> tr;

			for (auto posVal : vec3_.serializeValue(posPtr))
			{
				posVal.subVals.emplace(posVal.subVals.begin(), "pos");
				tr.push_back(posVal);
			}
			for (auto rotVal : quaternion_.serializeValue(rotPtr))
			{
				rotVal.subVals.emplace(rotVal.subVals.begin(), "rot");
				tr.push_back(rotVal);
			}
			for (auto sclVal : vec3_.serializeValue(sclPtr))
			{
				sclVal.subVals.emplace(sclVal.subVals.begin(), "scl");
				tr.push_back(sclVal);
			}

			return tr;
		}

		std::string WithWorldTransformParser::getUUID()
		{
			return WithWorldTransformParser::uuid;
		}
	}
}