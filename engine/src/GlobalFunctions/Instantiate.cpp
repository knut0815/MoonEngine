#include "Instantiate.h"
using namespace MoonEngine;
//Private global scene pointer (hacky workaround to call Instantiate from Game Objects)
static Scene * __gActiveScene = nullptr;
/**
 * Create a copy of this game object at the game object's location
 * @param objToCopy the object to copy.
 */
std::shared_ptr<GameObject> MoonEngine::Instantiate(GameObject * objToCopy)
{
	if(__gActiveScene == nullptr)
	{
		assert(!"No scene provided to instantiate!");
	}
	else
	{
		assert(objToCopy != nullptr);
		return __gActiveScene->instantiate(objToCopy, objToCopy->getTransform());
	}

}
/**
 * Create a copy of this game object at a new position and rotation
 */
std::shared_ptr<GameObject> MoonEngine::Instantiate(GameObject * object, glm::vec3 position, glm::vec3 rotation)
{
	if(__gActiveScene == nullptr)
	{
		assert(!"No scene provided to instantiate!");
	}
	else
	{
		assert(object != nullptr);
		Transform newTransform;
		newTransform.setPosition(position);
		newTransform.setRotation(rotation);
		return __gActiveScene->instantiate(object, Transform());
	}
}

	/**
	 * Set the active scene that holds game objects to a scene
	 * @param scene the scene to add game objects to.
	 */
void MoonEngine::SetActiveScene(Scene * scene)
{
	__gActiveScene = scene;
}
