#pragma once
#include "../../PlayAllTheGames/AlivePlayer.h"

namespace backattack
{
	class Player : public AlivePlayer
	{
	public:
		glm::vec2 position;
		float angle;

		float wishDirection;
		float speed;

		Player(int index) : AlivePlayer(index)
		{
			angle = 0;
			wishDirection = 0;
			speed = 32;
		}
	};
}
