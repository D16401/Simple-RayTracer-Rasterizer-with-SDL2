#include <iostream>
#include <memory>

#include <SDL2/SDL.h>

#include "cg_math.h"
#include "simple_cg.h"

int main(int argc, char* argv[]){
    std::shared_ptr<Camera> cameraPtr = std::make_shared<Camera>();
    cameraPtr->setCameraMode(CameraMode::VisibilityOnly);

    std::shared_ptr<Scene> scenePtr = std::make_shared<Scene>();
    auto sphereAPtr = std::make_unique<Sphere>();//堆上创建
    sphereAPtr->setColor(0xFF0000FF);
    sphereAPtr->setRadius(0.5f);
    sphereAPtr->setPosition(Vec3(0, 0, -2));
    scenePtr->addObjectPtr(std::move(sphereAPtr));//sphereAPtr现在是nullptr

    std::shared_ptr<RayTracingSampler> samplerPtr = std::make_shared<RayTracingSampler>(1000, 1000);
    samplerPtr->loadCamera(cameraPtr);
    samplerPtr->loadScene(scenePtr);

    SDL_Application app;
    app.loadSampler(samplerPtr);
    app.Init();
    app.Run([&](){});
    return 0;
}