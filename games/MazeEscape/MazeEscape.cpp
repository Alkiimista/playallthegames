#include "MazeEscape.h"

#include <blib/SpriteBatch.h>
#include <blib/LineBatch.h>
#include <blib/ResourceManager.h>
#include <blib/Util.h>
#include <blib/Math.h>
#include <blib/Color.h>

#include "../../PlayAllTheGames/Settings.h"
#include "../../PlayAllTheGames/Participant.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <glm/gtc/matrix_transform.hpp>

namespace mazeescape
{

	std::string MazeEscape::getName()
	{
		return "MazeEscape";
	}

	std::string MazeEscape::getInstructions()
	{
		return "Escape";
	}

	void MazeEscape::loadResources()
	{
		backSprite = resourceManager->getResource<blib::Texture>("assets/games/MazeEscape/back.png");
		playerSprite = resourceManager->getResource<blib::Texture>("assets/games/MazeEscape/player.png");
		font = resourceManager->getResource<blib::Font>("menu");

	}

	void MazeEscape::start(Difficulty difficulty)
	{
		for (auto p : players)
		{
			p->position = glm::vec2(1920 / 2, 1080 / 2) + 10.0f * blib::util::fromAngle(p->index / (float)players.size() * 2 * (float)M_PI);
			p->rotation = 0;
		}

		maze.clear(); // TODO: delete

		int lastCellCount = 1;
		for (int i = 0; i < 16; i++)
		{
			std::vector<Cell*> row;
			int cellCount = 64;
			if (i < 1)
				cellCount = 1;
			else if (i < 2)
				cellCount = 8;
			else if (i < 4)
				cellCount = 16;
			else if (i < 10)
				cellCount = 32;
			for (int ii = 0; ii < cellCount; ii++)
				row.push_back(new Cell());


			for (int ii = 0; ii < row.size(); ii++)
			{
				row[ii]->neighbours.push_back(std::pair<bool, Cell*>(false, row[(ii + 1) % row.size()]));
				row[ii]->neighbours.push_back(std::pair<bool, Cell*>(false, row[(ii + row.size() - 1) % row.size()]));

				if (i != 0)
				if (cellCount == lastCellCount)
					row[ii]->neighbours.push_back(std::pair<bool, Cell*>(false, maze[i - 1][ii]));
				else
				{
					int fac = cellCount / lastCellCount;
					row[ii]->neighbours.push_back(std::pair<bool, Cell*>(false, maze[i - 1][ii / fac]));
				}
			}
			lastCellCount = cellCount;
			maze.push_back(row);
		}
		for (size_t i = 0; i < maze.size()-1; i++)
			for (size_t ii = 0; ii < maze[i].size(); ii++)
				for (size_t iii = 0; iii < maze[i+1].size(); iii++)
					if (blib::linq::contains(maze[i + 1][iii]->neighbours, [this, i, ii](const std::pair<bool, Cell*> &v) { return v.second == maze[i][ii]; }))
							maze[i][ii]->neighbours.push_back(std::pair<bool, Cell*>(false, maze[i + 1][iii]));



		///build maze
		std::list<Cell*> nodes;
		maze[15][0]->visited = true;
		nodes.push_back(maze[15][0]);
		for (int iii = 0; iii < 10000; iii++)
		{
			Cell* current = nodes.back();
			std::vector<std::pair<int, Cell*>> available;
			for (size_t i = 0; i < current->neighbours.size(); i++)
				if (!current->neighbours[i].second->visited)
					available.push_back(std::pair<int, Cell*>(i, current->neighbours[i].second));
			if (available.empty())
			{
				nodes.pop_back();
				if (nodes.empty())
					break;
			}
			else
			{
				int nextIndex = rand() % available.size();
				nodes.push_back(available[nextIndex].second);
				current->neighbours[available[nextIndex].first].first = true;
				blib::linq::foreach(available[nextIndex].second->neighbours, [current](std::pair<bool, Cell*> &p) { if (p.second == current) { p.first = true; } });
				available[nextIndex].second->visited = true;
			}
		}

	}

	void MazeEscape::update(float elapsedTime)
	{
		blib::math::Rectangle screenRect(0, 0, 1920, 1080);
		for (auto p : players)
		{
		}

	}

	void MazeEscape::draw()
	{
		spriteBatch->begin(settings->scaleMatrix);
		spriteBatch->draw(backSprite, glm::mat4());



		spriteBatch->end();


		lineBatch->begin(glm::mat4(), 8.0f);

		double m = blib::math::pi / 32;
		for (int i = 32; i < 544 - 32; i += 32)
		{
			for (int d = 0; d < 64; d++)
			{
				int row = i / 32;
				int index = d;

				if (row == 0)
					index /= 64;
				else if (row < 2)
					index /= 8;
				else if (row < 4)
					index /= 4;
				else if (row < 10)
					index /= 2;


				if (!maze[row][index]->neighbours[2].first)
					lineBatch->draw(glm::vec2(1920 / 2 + i * cos(d*m), 1080 / 2 + i * sin(d*m)),
					glm::vec2(1920 / 2 + i * cos((d + 1)*m), 1080 / 2 + i * sin((d + 1)*m)), blib::Color::black);

				if (i < 64 && (d / 4) % 2 == 1)
					continue;
				if (i < 100 && (d / 2) % 2 == 1)
					continue;
				if (i < 300 && d % 2 == 1)
					continue;
				if (i >= 544 - 32)
					continue;

				if (!maze[row][index]->neighbours[1].first)
					lineBatch->draw(glm::vec2(1920 / 2 + i * cos(d*m), 1080 / 2 + i * sin(d*m)),
					glm::vec2(1920 / 2 + (i + 32)*cos(d*m), 1080 / 2 + (i + 32) * sin(d*m)), blib::Color::black);
			}
		}
		lineBatch->end();

		lineBatch->begin(glm::mat4(), 6.0f);
		for (int i = 32; i < 544-32; i += 32)
		{
			for (int d = 0; d < 64; d++)
			{
				int row = i / 32;
				int index = d;

				if (row == 0)
					index /= 64;
				else if (row < 2)
					index /= 8;
				else if (row < 4)
					index /= 4;
				else if (row < 10)
					index /= 2;


				if (!maze[row][index]->neighbours[2].first)
					lineBatch->draw(glm::vec2(1920 / 2 + i * cos(d*m), 1080 / 2 + i * sin(d*m)),
									glm::vec2(1920 / 2 + i * cos((d + 1)*m), 1080 / 2 + i * sin((d + 1)*m)), blib::Color::white);

				if (i < 64 && (d / 4) % 2 == 1)
					continue;
				if (i < 100 && (d / 2) % 2 == 1)
					continue;
				if (i < 300 && d % 2 == 1)
					continue;
				if (i >= 544 - 32)
					continue;

				if (!maze[row][index]->neighbours[1].first)
					lineBatch->draw(glm::vec2(1920 / 2 + i * cos(d*m), 1080 / 2 + i * sin(d*m)),
									glm::vec2(1920 / 2 + (i + 32)*cos(d*m), 1080 / 2 + (i + 32) * sin(d*m)), blib::Color::white);
			}
		}
		lineBatch->end();

	}

	blib::Texture* MazeEscape::getTitleImage()
	{
		return NULL;
	}

	bool MazeEscape::hasWinner()
	{
		return false;
	}

	std::list<Player*> MazeEscape::getWinners()
	{
		return std::list<Player*>();
	}

}