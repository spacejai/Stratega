#pragma once
#include <Configuration/GameConfig.h>

namespace SGA
{
	class GameConfigParser
	{
	public:
		GameConfig parseFromFile(const std::string& filePath) const;

		void parseAgents(const YAML::Node& agentsNode, GameConfig& config) const;
		void parseTileTypes(const YAML::Node& tilesNode, GameConfig& config) const;
		void parseBoardGenerator(const YAML::Node& boardNode, GameConfig& config) const;
		void parseEntities(const YAML::Node& entitiesNode, GameConfig& config) const;
        void parseEntityGroups(const YAML::Node& entityGroupsNode, SGA::GameConfig& config) const;
        void parseActions(const YAML::Node& actionsNode, GameConfig& config) const;
		
	private:
		TargetType parseTargetType(const YAML::Node& node, const GameConfig& config) const;
		
	};
}

namespace YAML
{
    template<>
    struct convert<SGA::ActionSourceType>
    {
        static bool decode(const Node& node, SGA::ActionSourceType& rhs)
        {
            if (!node.IsScalar())
                return false;
            auto value = node.as<std::string>();
            if (value == "EntityAction")
                rhs = SGA::ActionSourceType::Unit;
            else if (value == "PlayerAction")
                rhs = SGA::ActionSourceType::Player;
            else
                return false;

            return true;
        }
    };

    template<>
    struct convert<SGA::TargetType::Type>
    {
        static bool decode(const Node& node, SGA::TargetType::Type& rhs)
        {
            if (!node.IsScalar())
                return false;

            auto value = node.as<std::string>();
            if (value == "Position")
                rhs = SGA::TargetType::Type::Position;
            else if (value == "Entity")
                rhs = SGA::TargetType::Type::Entity;
            else
                return false;

            return true;
        }
    };

    template<>
    struct convert<SGA::ShapeType>
    {
        static bool decode(const Node& node, SGA::ShapeType& rhs)
        {
            if (!node.IsScalar())
                return false;

            auto value = node.as<std::string>();
            if (value == "Circle")
                rhs = SGA::ShapeType::Circle;
            else if (value == "Square")
                rhs = SGA::ShapeType::Square;
            else if (value == "Cross")
                rhs = SGA::ShapeType::Cross;
            else if (value == "Line")
                rhs = SGA::ShapeType::Line;
            else
                return false;

            return true;
        }
    };
}