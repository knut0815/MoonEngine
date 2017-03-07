#pragma once

#include "Component/Component.h"
#include "Component/CollisionComponents/BoxCollider.h"
#include "Geometry/Transform.h"

namespace MoonEngine
{
	enum ParticleState
	{
		INIT,
		GATHER,
		END
	};
	class Particle : public Component
	{
	public:

		Particle();
		
		void start();

		void update(float dt);

		std::shared_ptr<Component> clone() const;
	private:
		float accumTime;
		glm::vec3 direction;
		ParticleState state;
		std::shared_ptr<GameObject> player;
	};
}