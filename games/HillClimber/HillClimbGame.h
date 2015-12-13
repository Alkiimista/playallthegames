#pragma once

#include "../../PlayAllTheGames/AliveGame.h"

#include "HillClimbPlayer.h"
#include <Box2D/Box2D.h>

namespace blib { class Texture; };

namespace hillclimb
{


	class HillClimb : public AliveGame<HillClimbPlayer>
	{
		b2World* world;


		b2Body* floorBody[2];
		int floorCounter;

		blib::Texture* whitePixel;

		blib::Texture* tireSprite;
		blib::Texture* grassSprite;
		blib::Texture* skySprite;
		blib::Texture* carSprite;
		blib::Texture* carOverlaySprite;
		blib::Texture* shadowSprite;

	public:

		HillClimb();
		virtual ~HillClimb();
		virtual std::string getName() override;
		virtual std::string getInstructions() override;
		virtual std::pair<int, int> getPlayerCount() override;
		virtual void loadResources() override;
		virtual void start(Difficulty difficulty) override;
		virtual void update(float elapsedTime) override;
		virtual void draw() override;
		virtual blib::Texture* getTitleImage() override;
		void buildFloor();
	};


}