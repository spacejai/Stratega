#pragma once
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <Representation/AbstractGS/Parameter.h>

namespace SGA
{
	typedef int ParameterID;
	
	struct EntityType
	{
		int id;
		std::string name;
		std::unordered_map<ParameterID, Parameter> parameters;

		const Parameter& getParameter(ParameterID id) const
		{
			auto it = parameters.find(id);
			if (it != parameters.end())
			{
				return it->second;
			}
			else
			{
				std::string s;
				s.append("Tried accessing unknown parameter ID ");
				s.append(std::to_string(id));
				s.append("in entityType ");
				s.append(name);
				throw std::runtime_error(s);
			}
		}
	};
}
