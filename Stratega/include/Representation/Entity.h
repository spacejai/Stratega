#pragma once
#include <vector>
#include <Representation/Vector2.h>
#include "Representation/Path.h"

namespace SGA
{
	struct Entity
	{
		Entity():

			actionCooldown(0),
			maxActionCooldown(1),
			movementSpeed(2),
			collisionRadius(0.5),
			position(),

			intendedAction(),
			executingAction(),

			path()
		{
		}
		int typeID;
		int id;
		int owner;
		Vector2f position;
		std::vector<int> actionTypeIds;

		std::vector<double> parameters;

		bool shouldRemove=false;//Death?

		//TODO depends on the type...
		bool canExecuteAction;

		//RTS Stuff
		Action intendedAction;
		Action executingAction;
		Path path;
		double actionCooldown;
		double maxActionCooldown;
		double movementSpeed;
		float collisionRadius;
	};
}