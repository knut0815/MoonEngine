#include "GameObject.h"
#include "Util/Logger.h"
#include "Component/Components.h"

using namespace MoonEngine;

GameObject::GameObject():
    deleted(false),
    defaultBox(glm::vec3(0,0,0),0.5f,0.5f,0.5f),
    useMeshBounds(false),
    useBoxColliderBounds(false)
{

}

GameObject::GameObject(const Transform & t):
    transform(t),
    deleted(false),
    useMeshBounds(false),
    useBoxColliderBounds(false)
{

}

GameObject::~GameObject()
{
    LOG(INFO, "Deallocating game object...");
}

void GameObject::start()
{

    for (Component * c : components)
    {
        c->start();
    }
}

void GameObject::addComponent(Component * component)
{
    component->provideGameObject(this);
    if(dynamic_cast<Mesh *>(component) != nullptr)
    {
        useMeshBounds = true;
    }
    else if(dynamic_cast<BoxCollider *>(component) != nullptr)
    {
        useBoxColliderBounds = true;
    }
    components.push_back(component);
}


Transform & GameObject::getTransform()
{
    return transform;
}

std::vector<Component *> GameObject::getComponents()
{
    return components;
}

void GameObject::update(float dt)
{
    defaultTransformedBox = defaultBox.transform(transform.getMatrix());
    for (Component * c : components)
    {
        c->update(dt);
    }
}


void GameObject::addTag(Tag t)
{
    this->tag = t;
}

Tag GameObject::getTag()
{
    return tag;
}

void GameObject::onCollisionEnter(Collision col)
{
    for (Component * c : components)
    {
        c->onCollisionEnter(col);
    }
}

bool GameObject::isDeleted()
{
    return deleted;
}

void GameObject::setDeleted()
{
    deleted = true;
}

const BoundingBox & GameObject::getBounds()
{
    if(useBoxColliderBounds)
    {
        return (getComponent<BoxCollider>()->getBoundingBox());
    }
    else if(useMeshBounds)
    {
        return (getComponent<Mesh>()->getBoundingBox());
    }
    else
    {
        return defaultTransformedBox;
    }
}