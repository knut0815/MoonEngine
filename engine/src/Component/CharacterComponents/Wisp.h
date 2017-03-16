#pragma once

#include "Component/Component.h"
#include "Component/CollisionComponents/BoxCollider.h"
#include "Geometry/Transform.h"

namespace MoonEngine
{
	class Wisp : public Component
	{
	public:
		Wisp();
		void start();

		void update(float dt);

		std::shared_ptr<Component> clone() const;
	private:
		float accumTime;
		glm::vec3 direction;
		float scale;
		std::shared_ptr<GameObject> player;
	};
}