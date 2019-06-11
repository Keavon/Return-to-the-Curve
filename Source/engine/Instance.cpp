
#include "Instance.h"
#include "Prefab.h"

shared_ptr<PhysicsObject> makePhysicsObject(Prefab& definition)
{
    if (definition.name == "Marble") return make_shared<Ball>(vec3(0, 0, 0), quat(1, 0, 0, 0), definition.model, 1.0f);

    return make_shared<PhysicsObject>(vec3(0, 0, 0), definition.model);
}

Instance::Instance(Prefab& definition) : definition(definition)
{
    this->material = definition.material;
    this->physicsObject = makePhysicsObject(definition);
}
