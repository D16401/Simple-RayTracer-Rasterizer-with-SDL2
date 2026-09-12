#include "cg_wd_scene.h"

#include "cg_wd_object.h"
#include "cg_wd_light.h"

using cg::Scene;

Scene::~Scene() = default;
Scene::Scene(SceneBuild build): background(build.background), ambient(build.ambient),
    ObjectPtrs(std::move(build.ObjectPtrs)), LightPtrs(std::move(build.LightPtrs)){}
void Scene::addObjectPtr(std::unique_ptr<Object> ObjectPtr){
    ObjectPtrs.push_back(std::move(ObjectPtr));
};
void Scene::addLightPtr(std::unique_ptr<Light> LightPtr){
    LightPtrs.push_back(std::move(LightPtr));
};