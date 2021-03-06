#pragma once
#include <unordered_set>
#include <yaml-cpp/node/node.h>
#include <Representation/UnitType.h>
#include <ForwardModel/ActionType.h>

namespace SGA
{
	struct UnitConfig
	{
        int health;
        int attackDamage;
        int healAmount;
        int movementRange;
        int actionRange;
        int actionsPerTurn;
        bool canRepeatActions;
        int lineOfSightRange;
        float collisionRadius;
        std::unordered_set<ActionType> actions;
		
        UnitConfig():
	        health(0),
	        attackDamage(0),
	        healAmount(0),
	        movementRange(0),
			actionRange(0),
			actionsPerTurn(1),
			canRepeatActions(false),
	        lineOfSightRange(0),
			collisionRadius(0.5)
        {
        }
	};

    UnitType unitTypeFromConfig(const UnitConfig& config, int id);
}

namespace YAML
{
    template<>
	struct convert<SGA::ActionType>
    {
	    static bool decode(const Node& node, SGA::ActionType& rhs)
	    {
            if (!node.IsScalar())
                return false;

            auto type = node.as<std::string>();
            if (type == "Move")
            {
                rhs = SGA::ActionType::Move;
            }
            else if (type == "Attack")
            {
                rhs = SGA::ActionType::Attack;
            }
            else if (type == "Heal")
            {
                rhs = SGA::ActionType::Heal;
            }
            else if (type == "Push")
            {
                rhs = SGA::ActionType::Push;
            }
            else
            {
                return false;
            }

            return true;
	    }
    };
	
    template<>
    struct convert<SGA::UnitConfig>
    {
        static bool decode(const Node& node, SGA::UnitConfig& rhs)
        {
            if (!node.IsMap())
                return false;

            rhs.health = node["Health"].as<int>();
            rhs.lineOfSightRange = node["LineOfSightRange"].as<int>();
            rhs.actionRange = node["ActionRange"].as<int>();
            rhs.actionsPerTurn = node["ActionsPerTurn"].as<int>(rhs.actionsPerTurn);
            rhs.canRepeatActions = node["RepeatableActions"].as<bool>(rhs.canRepeatActions);
            rhs.collisionRadius = node["CollisionRadius"].as<float>(rhs.collisionRadius);
        	
        	auto actionVector = node["Actions"].as<std::vector<SGA::ActionType>>(std::vector<SGA::ActionType>());
            rhs.actions.insert(actionVector.begin(), actionVector.end());
        	if(rhs.actions.find(SGA::ActionType::Move) != rhs.actions.end())
        	{
                rhs.movementRange = node["MovementRange"].as<int>();
        	}
        	if(rhs.actions.find(SGA::ActionType::Attack) != rhs.actions.end())
        	{
                rhs.attackDamage = node["AttackDamage"].as<int>();
        	}
        	if(rhs.actions.find(SGA::ActionType::Heal) != rhs.actions.end())
        	{
                rhs.healAmount = node["HealAmount"].as<int>();
        	}

            return true;
        }
    };
}
