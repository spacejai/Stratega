#pragma once
#include <Representation/AbstractGS/EntityType.h>
#include <ForwardModel/AbstractFM/ActionType.h>
#include <Representation/AbstractGS/Entity.h>
#include <Representation/AbstractGS/Player.h>
#include <Representation/Board.h>
#include <ForwardModel/AbstractFM/Precondition.h>

namespace SGA
{
	struct GameState
	{
		GameState()
			: entityTypes(std::make_unique<std::unordered_map<int, EntityType>>()),
			  actionTypes(std::make_unique<std::unordered_map<int, ActionType>>()),
			  tileTypes(std::make_unique<std::unordered_map<int, TileType>>()),
			  isGameOver(false),
			  winnerPlayerID(0),
			  fogOfWarTile(0,0,0),
			  fogOfWarId(0),
			  board(10,10),
			  lastUsedEntityID(0),
			  lastUsedPlayerID(0)
		{
		}

		//Rule of six
		virtual ~GameState() = default;
		GameState(const GameState& other) = default;
		GameState(GameState&& other) noexcept = default;
		GameState& operator=(const GameState& other) = default;
		GameState& operator=(GameState&& other) noexcept = default;

		// Type information
		std::shared_ptr<std::unordered_map<int, EntityType>> entityTypes;
		std::shared_ptr<std::unordered_map<int, ActionType>> actionTypes;
		std::shared_ptr<std::unordered_map<int, TileType>> tileTypes;

		// Game information
		bool isGameOver;
		int winnerPlayerID;
		
		// Board information
		Tile fogOfWarTile;
		int fogOfWarId = -1;
		Board board;

		// Player and unit information
		std::vector<Entity> entities;
		std::vector<Player> players;
		int lastUsedEntityID;
		int lastUsedPlayerID;

		virtual bool canExecuteAction(Entity& entity, ActionType& actionType);

		Entity& getEntity(int entityID)
		{
			auto& entity = entities[entityID];
			return entity;
		}

		const Entity& getEntityConst(int entityID) const 
		{
			const auto& entity = entities[entityID];
			return entity;
		}

		const EntityType& getEntityType(int entityTypeID) const
		{
			auto it = entityTypes->find(entityTypeID);
			if (it != entityTypes->end())
			{
				return it->second;
			}
			else
			{
				std::string s;
				s.append("Tried accessing unknown entity type with ID=");
				s.append(std::to_string(entityTypeID));
				throw std::runtime_error(s);
			}
		}
	};
}