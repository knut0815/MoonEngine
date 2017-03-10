#include "Particle.h"
#include "GameObject/GameObject.h"
#include "GlobalFuncs/GlobalFuncs.h"
#include <iostream>

using namespace MoonEngine;

Particle::Particle() :
	accumTime(0),
	state(INIT)
{

}

void Particle::start()
{
	float a = (float)rand() / RAND_MAX * 6.28f - 3.14;
	float b = (float)rand() / RAND_MAX * 6.28f - 3.14;
	float c = (float)rand() / RAND_MAX * 6.28f - 3.14;
	direction = glm::vec3(a, b, c);
	player = GetWorld()->getPlayer();
	gameObject->getTransform().setScale(0.15f);
}

void Particle::update(float dt)
{
	accumTime += dt;
	if (state == INIT)
	{
		float speed = (0.7 - accumTime);
		gameObject->getTransform().translate(dt * direction * speed);
		if (accumTime >= 0.7f)
		{
			state = GATHER;
		}
	}
	else
	{
		if (player != nullptr)
		{
			direction = glm::normalize(player->getTransform().getPosition()
				- gameObject->getTransform().getPosition());
		}
		
		float scale = (glm::length(player->getTransform().getPosition() - gameObject->getTransform().getPosition()))/4.0f;

		gameObject->getTransform().translate(direction / 3.0f);
		gameObject->getTransform().setScale(std::min(0.15f, scale));
		if (scale <= 0.1f)
		{
			Delete(gameObject);
		}
	}
	gameObject->getTransform().rotate(direction *dt);
		
	
}

std::shared_ptr<Component> Particle::clone() const
{
	return std::make_shared<Particle>(*this);
}
