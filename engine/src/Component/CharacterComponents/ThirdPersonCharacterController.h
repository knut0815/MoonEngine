#pragma once

#include "Component/Component.h"
#include "Component/CollisionComponents/BoxCollider.h"
#include "Geometry/Transform.h"
/**
 * Controls a character in a first-person matter
 */
namespace MoonEngine
{

	enum PlayerState
	{
		JUMPING,
		GROUND,
		FALLING
	};

	class ThirdPersonCharacterController : public Component
	{
	public:
		ThirdPersonCharacterController(float playerSpeed = 1);
		void start();
		void update(float dt);
		void onCollisionEnter(Collision col);

		std::shared_ptr<Component> clone() const;

	private:
		void handleMove(float dt);
		void handleJump(float dt);
		void checkIfShouldFall();
		Transform * transform;
		float playerSpeed;
		float jumpSpeed;
		float jumpForce;
		float jumpTime;
		float _curJumpForce;
		float _jumpTime;
		float gravity;
		PlayerState state;
		float radius;
		GameObject * mainCamera;
		BoxCollider * bbox;


	};
}