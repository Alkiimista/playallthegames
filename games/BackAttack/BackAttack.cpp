#include "BackAttack.h"
#include "Level.h"

#include <blib/ResourceManager.h>
#include <blib/SpriteBatch.h>
#include <blib/StaticModel.h>
#include <blib/Math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../../PlayAllTheGames/Settings.h"

namespace backattack
{
	BackAttack::BackAttack()
	{
		level = NULL;
	}


	std::string BackAttack::getName()
	{
		return "BackAttack";
	}

	std::string BackAttack::getInstructions()
	{
		return "Shoot from behind";
	}

	std::pair<int, int> BackAttack::getPlayerCount()
	{
		return std::pair<int, int>(2, 20);
	}

	void BackAttack::loadResources()
	{
		renderState = spriteBatch->renderState;
		renderState.depthTest = true;
		renderState.activeShader = resourceManager->getResource<blib::Shader>("BackAttack");
		renderState.activeShader->bindAttributeLocation("a_position", 0);
		renderState.activeShader->bindAttributeLocation("a_texcoord", 1);
		renderState.activeShader->bindAttributeLocation("a_normal", 2);
		renderState.activeShader->setUniformName(Uniforms::ProjectionMatrix, "projectionMatrix", blib::Shader::Mat4);
		renderState.activeShader->setUniformName(Uniforms::ModelMatrix, "modelMatrix", blib::Shader::Mat4);
		renderState.activeShader->setUniformName(Uniforms::CameraMatrix, "cameraMatrix", blib::Shader::Mat4);
		renderState.activeShader->setUniformName(Uniforms::s_texture, "s_texture", blib::Shader::Int);
		renderState.activeShader->setUniformName(Uniforms::color, "color", blib::Shader::Vec4);
		renderState.activeShader->finishUniformSetup();
		renderState.activeShader->setUniform(Uniforms::s_texture, 0);
		renderState.cullFaces = blib::RenderState::CullFaces::NONE;
		renderState.activeVbo = NULL;
		renderState.activeVio = NULL;

		Level::loadResources(resourceManager, renderer);
		cart = new blib::StaticModel("assets/games/BackAttack/mine_cart.fbx.json", resourceManager, renderer);
		cube = new blib::StaticModel("assets/games/BackAttack/cube.fbx.json", resourceManager, renderer);
	}

	void BackAttack::start(Difficulty difficulty)
	{
		if (level)
			delete level;
		level = new Level();

		players[0]->position = glm::vec2(0, 0);
		players[1]->position = glm::vec2(0, 9*8);
		players[2]->position = glm::vec2(9 * 8, 9 * 8);
		players[3]->position = glm::vec2(9 * 8, 0);

		for (auto p : players)
			p->angle = -90.0f * p->index;
	}

	float cameraHeight = 0;

	void BackAttack::update(float elapsedTime)
	{
		for (auto p : players)
		{
			if (!p->alive)
				continue;

			if (glm::length(p->joystick.leftStick) > 0.85f)
			{
				glm::vec2 dir = glm::normalize(p->joystick.leftStick);
				p->wishDirection = glm::round(glm::degrees(atan2(dir.y, -dir.x) / 90.0f))*90;
			}


			glm::vec2 newPosition = p->position + blib::math::fromAngle(glm::radians(p->angle)) * elapsedTime * 32.0f;
			bool collided = false;
			glm::ivec2 tile(round(p->position.x / 8), round(p->position.y / 8));

			glm::vec2 diff = glm::vec2(tile) * 8.0f - p->position;
			glm::vec2 newDiff = glm::vec2(tile) * 8.0f - newPosition;
			if (std::signbit(diff.x) != std::signbit(newDiff.x) ||
				std::signbit(diff.y) != std::signbit(newDiff.y))
			{ //reached center!
				float oldAngle = p->angle;
				glm::vec2 v1 = blib::math::fromAngle(glm::radians(p->angle));
				glm::vec2 v2 = blib::math::fromAngle(glm::radians(p->wishDirection));

				if(glm::length(v1 + v2) > 0.1)
					p->angle = p->wishDirection;
				glm::ivec2 nextTile = tile + glm::ivec2(blib::math::fromAngle(glm::radians(p->angle)));
				if (nextTile.x < 0 || nextTile.y < 0 || nextTile.x >= level->width || nextTile.y >= level->height || !level->tiles[nextTile.x][nextTile.y]->isTrack)
				{
					p->angle = oldAngle;
					nextTile = tile + glm::ivec2(blib::math::fromAngle(glm::radians(p->angle)));
					if (nextTile.x < 0 || nextTile.y < 0 || nextTile.x >= level->width || nextTile.y >= level->height || !level->tiles[nextTile.x][nextTile.y]->isTrack)
					{
						p->angle = oldAngle + 90.0f;
						nextTile = tile + glm::ivec2(blib::math::fromAngle(glm::radians(p->angle)));
						if (nextTile.x < 0 || nextTile.y < 0 || nextTile.x >= level->width || nextTile.y >= level->height || !level->tiles[nextTile.x][nextTile.y]->isTrack)
						{
							p->angle = oldAngle - 90.0f;
							nextTile = tile + glm::ivec2(blib::math::fromAngle(glm::radians(p->angle)));
							if (nextTile.x < 0 || nextTile.y < 0 || nextTile.x >= level->width || nextTile.y >= level->height || !level->tiles[nextTile.x][nextTile.y]->isTrack)
								p->angle -= 90.0f;
						}
					}
				}

			}
			newPosition = p->position + blib::math::fromAngle(glm::radians(p->angle)) * elapsedTime * 8.0f;
			p->position = newPosition;


/*			float oldDist = glm::distance(p->position, 8.0f * glm::vec2(oldTile) + glm::vec2(4, 4));
			float newDist = glm::distance(newPosition, 8.0f * glm::vec2(oldTile) + glm::vec2(4, 4));
			if (newDist > oldDist)
			{
				if (newTile.x < 0 || newTile.y < 0 || newTile.x >= level->width || newTile.y >= level->height || !level->tiles[newTile.x][newTile.y]->isTrack)
				{
					p->angle += 90.0f;
					collided = true;
				}
			}
			if (!collided)
				p->position = newPosition;
				*/

		}

	}

	void BackAttack::draw()
	{
		glm::mat4 projectionMatrix = glm::perspective(70.0f, (float)settings->resX / settings->resY, 0.1f, 500.0f);
		glm::mat4 cameraMatrix = glm::lookAt(glm::vec3(level->width * 4 - 4, -85, level->height * 4-20), glm::vec3(level->width * 4 - 4, -16.5, level->height * 4-10.5), glm::vec3(0, -1, 0));
		renderState.activeShader->setUniform(Uniforms::color, glm::vec4(1, 1, 1, 1));


		renderState.activeShader->setUniform(Uniforms::CameraMatrix, cameraMatrix);
		renderState.activeShader->setUniform(Uniforms::ProjectionMatrix, projectionMatrix);
		renderState.activeShader->setUniform(Uniforms::ModelMatrix, glm::mat4());
		renderState.activeFbo = spriteBatch->renderState.activeFbo;
		renderState.activeShader->setUniform(Uniforms::color, glm::vec4(0, 0, 0, 0));



		for (auto p : players)
		{
			if (!p->alive)
				continue;

			glm::mat4 mat;
			mat = glm::translate(mat, glm::vec3(p->position.x, 0, p->position.y));
			renderState.activeShader->setUniform(Uniforms::ModelMatrix, mat);

			cart->draw(renderState, renderer, [this](const blib::Material& material)
			{
				renderState.activeTexture[0] = material.texture;
			});



			bool collided = false;
			glm::ivec2 tile(round(p->position.x / 8), round(p->position.y / 8));
			glm::ivec2 nextTile = tile + glm::ivec2(blib::math::fromAngle(glm::radians(p->angle)));





			mat = glm::mat4();
			mat = glm::translate(mat, glm::vec3(tile.x*8, -1, tile.y*8));
			mat = glm::scale(mat, glm::vec3(8,1,8));
			renderState.activeShader->setUniform(Uniforms::ModelMatrix, mat);
			renderState.activeShader->setUniform(Uniforms::color, glm::vec4(1,0,0,-.25));
			cube->draw(renderState, renderer, [this](const blib::Material& material)
			{
				renderState.activeTexture[0] = material.texture;
			});
			mat = glm::mat4();
			mat = glm::translate(mat, glm::vec3(nextTile.x * 8, -1, nextTile.y * 8));
			mat = glm::scale(mat, glm::vec3(8, 1, 8));
			renderState.activeShader->setUniform(Uniforms::ModelMatrix, mat);
			renderState.activeShader->setUniform(Uniforms::color, glm::vec4(0, 1, 0, -.25));
			cube->draw(renderState, renderer, [this](const blib::Material& material)
			{
				renderState.activeTexture[0] = material.texture;
			});


			renderState.activeShader->setUniform(Uniforms::color, glm::vec4(0, 0, 0, 0));


		}
		level->draw(renderState, renderer);

	}

	blib::Texture* BackAttack::getTitleImage()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

}