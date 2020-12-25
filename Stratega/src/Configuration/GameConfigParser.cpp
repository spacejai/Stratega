#include <Configuration/GameConfigParser.h>
#include <Configuration/BoardGenerator.h>
#include <Agent/AgentFactory.h>

#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/yaml.h>

namespace SGA
{
	GameConfig GameConfigParser::parseFromFile(const std::string& filePath) const
	{
		auto configNode = YAML::LoadFile(filePath);
        GameConfig config;
        config.gameType = configNode["GameConfig"]["Type"].as<std::string>();
        config.roundLimit = configNode["GameConfig"]["RoundLimit"].as<int>(config.roundLimit);
        config.numPlayers = configNode["GameConfig"]["PlayerCount"].as<int>(config.numPlayers);

		// Parse complex structures
		// Order is important, only change if you are sure that a function doesn't depend on something parsed before it
        parseAgents(configNode["Agents"], config);
        parseTileTypes(configNode["Tiles"], config);
        parseBoardGenerator(configNode["Board"], config);
        parseEntities(configNode["Entities"], config);
        parseEntityGroups(configNode["EntityGroups"], config);
        parseActions(configNode["Actions"], config);
		// ToDo Parse ForwardModelConfig in an seperate function
		if(configNode["ForwardModel"].IsDefined())
		{
            config.forwardModelConfig = configNode["ForwardModel"].as<ForwardModelConfig>();
		}
        else
        {
            throw std::runtime_error("Cannot find a definition for the ForwardModel");
        }

		// Assign actions to entities
        auto types = configNode["Entities"].as<std::map<std::string, YAML::Node>>();
        for (auto& type : config.entityTypes)
        {
            auto actions = types[type.second.name]["Actions"].as<std::vector<std::string>>(std::vector<std::string>());
            for (const auto& actionName : actions)
            {
                type.second.actionIds.emplace_back(config.getActionID(actionName));
            }
        }
		
        return config;
	}

	void GameConfigParser::parseAgents(const YAML::Node& agentsNode, GameConfig& config) const
	{
        if(!agentsNode.IsDefined())
        {
            throw std::runtime_error("Cannot find definition for Agents");
        }

        for (const auto& agentNode : agentsNode)
        {
            if (agentNode.IsScalar())
            {
                config.agentParams.emplace_back(agentNode.as<std::string>(), YAML::Null);
            }
            else
            {
                auto map = agentNode.as<std::map<std::string, YAML::Node>>();
                const auto& firstEntry = *map.begin();
                config.agentParams.emplace_back(firstEntry);
            }
        }
	}

    void GameConfigParser::parseTileTypes(const YAML::Node& tilesNode, GameConfig& config) const
	{
        if(!tilesNode.IsDefined())
        {
            throw std::runtime_error("Cannot find definition for Tiles");
        }
		
        auto tileConfigs = tilesNode.as<std::map<std::string, YAML::Node>>();
        std::unordered_map<std::string, TileType> types;
        auto idCounter = 0;
		for(const auto& nameConfigPair : tileConfigs)
		{
            TileType type;
            type.id = idCounter++;
            type.name = nameConfigPair.first;
			type.isWalkable = nameConfigPair.second["IsWalkable"].as<bool>(type.isWalkable);
			type.symbol = nameConfigPair.second["Symbol"].as<char>();
            config.tileTypes.emplace(type.id, std::move(type));
		}
	}

    void GameConfigParser::parseBoardGenerator(const YAML::Node& boardNode, GameConfig& config) const
	{
		if(!boardNode.IsDefined())
		{
            throw std::runtime_error("Cannot find definition for the Board");
		}

        if (boardNode["GenerationType"].as<std::string>() == "Manual")
        {
            auto layout = boardNode["Layout"].as<std::string>();
            // Extract rows
            std::vector<std::string> rows;
            size_t last = 0, next;
            while ((next = layout.find(' ', last)) != std::string::npos)
            {
                rows.emplace_back(layout.substr(last, next - last));
                last = next + 1;
            }
            rows.push_back(layout.substr(last, layout.size() - last));

            // Initialize the generator
            std::unordered_map<char, TileType> tileMapping;
            for (const auto& nameTypePar : config.tileTypes)
            {
                tileMapping.emplace(nameTypePar.second.symbol, nameTypePar.second);
            }
            config.boardGenerator = std::make_unique<BoardGenerator>(std::move(rows), std::move(tileMapping));
        }
        else
        {
            throw std::runtime_error("Unknown board-generation type " + boardNode["GenerationType"].as<std::string>());
        }
	}

    void GameConfigParser::parseEntities(const YAML::Node& entitiesNode, GameConfig& config) const
    {
        if(!entitiesNode.IsDefined())
        {
            throw std::runtime_error("Cannot find definition for Entities");
        }
		
        auto types = entitiesNode.as<std::map<std::string, YAML::Node>>();
        for (const auto& nameTypePair : types)
        {
            EntityType type;
            type.name = nameTypePair.first;
            type.id = config.entityTypes.size();
            for (const auto& nameParamPair : nameTypePair.second["Parameters"].as<std::map<std::string, double>>())
            {
                // Assign IDs to parameters that do not exist yet
                if (config.parameters.find(nameParamPair.first) == config.parameters.end())
                {
                    config.parameters.insert({ nameParamPair.first, config.parameters.size() });
                }

                // Construct the parameter
                Parameter param;
                param.name = nameParamPair.first;
                param.minValue = 0;
                param.maxValue = nameParamPair.second;
                param.defaultValue = param.maxValue;
                param.index = type.parameters.size();
                type.parameters.insert({ config.parameters.at(nameParamPair.first), std::move(param) });
            }

            config.entityTypes.emplace(type.id, std::move(type));
        }
    }

	void GameConfigParser::parseEntityGroups(const YAML::Node& entityGroupsNode, GameConfig& config) const
    {
        if (entityGroupsNode.IsDefined())
        {
            auto groups = entityGroupsNode.as<std::map<std::string, std::vector<std::string>>>();
            for (const auto& nameGroupPair : groups)
            {
                config.entityGroups.emplace(nameGroupPair.first, std::vector<int>());
                for (const auto& entityName : nameGroupPair.second)
                {
                    config.entityGroups[nameGroupPair.first].emplace_back(config.getEntityID(entityName));
                }
            }
        }

        // Group that contains all entities
        config.entityGroups.emplace("All", std::vector<int>());
        for (const auto& idEntityPair : config.entityTypes)
        {
            config.entityGroups.at("All").emplace_back(idEntityPair.first);
        }
    }

    void GameConfigParser::parseActions(const YAML::Node& actionsNode, GameConfig& config) const
    {
        if(!actionsNode.IsDefined())
        {
            throw std::runtime_error("Cannot find definition for Actions");
        }

        // ToDo Is this necessary? - Hardcode EndTurn
        ActionType actionType;
        actionType.id = 0;
        actionType.name = "EndTurn";
        actionType.sourceType = ActionSourceType::Player;
        config.actionTypes.emplace(0, std::move(actionType));

        FunctionParser parser;
        auto types = actionsNode.as<std::map<std::string, YAML::Node>>();
        for (const auto& nameTypePair : types)
        {
            ActionType type;
            type.id = config.actionTypes.size();
            type.name = nameTypePair.first;
            type.actionTargets = parseTargetType(nameTypePair.second["Target"], config);
            type.sourceType = nameTypePair.second["Type"].as<ActionSourceType>();
            std::unordered_map<std::string, int> actionTargetIds;
            actionTargetIds.emplace("Source", 0);
            actionTargetIds.emplace("Target", 1);

            // Parse preconditions
        	auto preconditions = nameTypePair.second["Preconditions"].as<std::vector<std::string>>(std::vector<std::string>());
            parser.parseFunctions(preconditions, type.preconditions, actionTargetIds, config.parameters);

            // Parse effects
            auto effects = nameTypePair.second["Effects"].as<std::vector<std::string>>(std::vector<std::string>());
            parser.parseFunctions(effects, type.effects, actionTargetIds, config.parameters);

            config.actionTypes.emplace(type.id, std::move(type));
        }
    }

    TargetType GameConfigParser::parseTargetType(const YAML::Node& node, const GameConfig& config) const
    {
        TargetType targetType;
        targetType.type = node["Type"].as<TargetType::Type>();
        if (targetType.type == TargetType::Position)
        {
            targetType.shapeType = node["Shape"].as<ShapeType>();
            targetType.shapeSize = node["Size"].as<int>();
        }
        else if (targetType.type == TargetType::Entity)
        {
            auto groupName = node["ValidTargets"].as<std::string>("All");
            const auto& group = config.entityGroups.at(groupName);
            targetType.groupEntityTypes.insert(group.begin(), group.end());
        }
        return targetType;
    }
}