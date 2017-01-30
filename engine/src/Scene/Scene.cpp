#include "Scene.h"
#include "Component/Components.h"
#include "Util/Logger.h"
using namespace MoonEngine;


Scene::Scene()
{
	_allGameObjects.clear();
	_gameObjects.clear();
	_renderableGameObjects.clear();
	_boxCollisionComponents.clear();
	_components.clear();
}

/**
 * Add the gameobject to the active scene.
 * @param obj [description]
 */
void Scene::addGameObject(std::shared_ptr<GameObject> obj)
{
	_gameObjects.push_back(obj);
	if(obj->getComponent<Material>() != nullptr && 
		obj->getComponent<Mesh>() != nullptr)
	{
		LOG(INFO, "Adding renderable game object");
		_renderableGameObjects.push_back(obj);
	}
	BoxCollider * col = obj->getComponent<BoxCollider>();
	if(col != nullptr)
	{
		LOG(INFO, "Adding collidable game object");
		_boxCollisionComponents.push_back(col);
	}
}

void Scene::runUpdate(float dt)
{
	instantiateNewObjects();

	for(std::shared_ptr<GameObject> go :  _gameObjects)
	{
		go->update(dt);
	}
	runCollisionUpdate();
	if(updateFunctors.size() > 0)
	{
		for(auto & fun : updateFunctors)
		{
			fun(dt);
		}
	}
}

const std::vector<std::shared_ptr<GameObject>> Scene::getGameObjects() const
{
	return _gameObjects;
}
const std::vector<std::shared_ptr<GameObject>> Scene::getRenderableGameObjects() const
{
	return _renderableGameObjects;
}

void Scene::runCollisionUpdate()
{
	glm::vec3 colnormal;
	if(_boxCollisionComponents.size() > 0)
	{
		for(size_t i = 0; i < _boxCollisionComponents.size() - 1; i++)
		{
			for(size_t j = i + 1; j < _boxCollisionComponents.size(); j++)
			{
				//Try collision between i and j
				if(_boxCollisionComponents[i]->intersects(
					_boxCollisionComponents[j], &colnormal))
				{
					//Create a new collision
					Collision c;
					c.normal = colnormal;
					//Forward to both game objects
					c.other = _boxCollisionComponents[i]->getGameObject();
					_boxCollisionComponents[j]->getGameObject()->onCollisionEnter(c);
					c.normal = -colnormal;
					c.other = _boxCollisionComponents[j]->getGameObject();
					_boxCollisionComponents[i]->getGameObject()->onCollisionEnter(c);

				}
			}
		}		
	}
}

std::shared_ptr<GameObject> Scene::instantiate(GameObject * object, const Transform & newPosition)
{
	std::shared_ptr<GameObject>  newObject = std::make_shared<GameObject>();
	//First, create new components for object
	for(Component * c : object->getComponents())
	{
		std::shared_ptr<Component> comp = c->clone();
		_instantiateComponents.push_back(comp);
		newObject->addComponent(comp.get());
	}
	newObject->getTransform() = newPosition;
	_instantiateQueue.push_back(newObject);
	return newObject;

}

void Scene::deleteGameObject(GameObject * object)
{
	//Add object to queue for deletion
	//On frame end, these objects should be deleted.
}


void Scene::runDeleteGameObjects()
{
	//Delete all components on queued gameObjects
	//Also delete the gameObjects
}

void Scene::instantiateNewObjects()
{
	if(_instantiateComponents.size() > 0 || _instantiateQueue.size() > 0)
	{
		LOG(INFO, "Instantiating new objects...");
		_components.insert(_components.end(),_instantiateComponents.begin(),_instantiateComponents.end());
		for(std::shared_ptr<GameObject> obj : _instantiateQueue)
		{
			addGameObject(obj);
			obj->start();
		}
		_instantiateComponents.clear();
		_instantiateQueue.clear();	
	}

}

void Scene::addCustomUpdate(std::function<void(float)> fn)
{
	updateFunctors.push_back(fn);
}
//Naieve implementation
GameObject * Scene::findGameObjectWithTag(Tag  t)
{
	for(auto g : _gameObjects)
	{
		if(g->getTag() == t)
		{
			return g.get();
		}
	}
	return nullptr;
}
//Naieve implementation
bool Scene::castRay(glm::vec3 origin, glm::vec3 direction, float maxDist, Hit * hit)
{
	//Spatial data structure would go here.
	Hit tmpHit;
	if(glm::length(direction) > 0)
	{
		for(size_t i = 0; i < _boxCollisionComponents.size(); i++)
		{
			
			if(_boxCollisionComponents[i]->intersectsRay(origin,direction,&tmpHit))
			{
				LOG(GAME, std::to_string(tmpHit.distance));
				if(maxDist == -1 || tmpHit.distance < maxDist)
				{
					if(hit != nullptr)
					{
						*hit = tmpHit;
					}
					return true;
				}
			}
		}
	}
	return false;
	
}

void Scene::start()
{
	// for(auto & g : _gameObjects)
	// {
	// //	g->start();
	// }
}
