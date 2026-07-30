#include "cg_rt_structure.h"

//Scene
void Scene::addObjectPtr(std::unique_ptr<Object> ObjectPtr){
    ObjectPtrs.push_back(std::move(ObjectPtr));
}
void Scene::addLightPtr(std::unique_ptr<Light> LightPtr){
    LightPtrs.push_back(std::move(LightPtr));
}