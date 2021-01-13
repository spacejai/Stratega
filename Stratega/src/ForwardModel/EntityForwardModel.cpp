#include <ForwardModel/EntityForwardModel.h>

namespace SGA
{
	bool EntityForwardModel::canPlayerPlay(const GameState& state, Player& player) const
	{
		if (state.fogOfWarId != -1 && player.id != state.fogOfWarId)
			return true;

		switch (winCondition)
		{
		case WinConditionType::UnitAlive:
		{
			//player.getEntities();
			bool hasKing = false;
			for (auto& unit : state.getPlayerEntities(player.id))
			{
				//Check if player has units
				if (unit->typeID == targetUnitTypeID)
				{
					hasKing = true;
				}
			}

			if (!hasKing)
			{
				return false;
			}

			break;
		}
		case WinConditionType::LastManStanding:
		{
			if (state.getPlayerEntities(player.id).empty())
			{
				return false;
			}
			break;
		}
		}

		return true;
	}

	void EntityForwardModel::executeAction(GameState& state, const Action& action) const
	{
		action.execute(state, *this);

		auto& actionType = state.getActionType(action.actionTypeID);
		if(actionType.sourceType == ActionSourceType::Unit)
		{
			// Remember when the action was executed
			auto& executingEntity = targetToEntity(state, action.targets[0]);
			// ToDo We should probably find a way to avoid this loop
			for(auto& actionInfo : executingEntity.attachedActions)
			{
				if(actionInfo.actionTypeID == action.actionTypeID)
				{
					actionInfo.lastExecutedTick = state.currentTick;
					break;
				}
			}
		}
	}

	void EntityForwardModel::endTick(GameState& state) const
	{
		state.currentTick++;

		// Execute OnTick-trigger
		std::vector<ActionTarget> targets;
		targets.resize(1);
		for(const auto& effect : onTickEffects)
		{
			for (const auto& entity : state.entities)
			{
				targets[0] = entity.id;
				effect->execute(state, *this, targets);
			}
		}
	}
}
