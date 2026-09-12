#include <iostream>
#include <memory>

#include "simple_cg.h"

int main(int argc, char* argv[]){
    using cg::Vec3;
    
    std::shared_ptr<cg::Camera> cameraPtr = std::make_shared<cg::Camera>();

    std::shared_ptr<cg::Scene> scenePtr = std::make_shared<cg::Scene>();
    auto sphereAPtr = std::make_unique<cg::Sphere>();//堆上创建
    sphereAPtr->setColor(0xFF0000FF);
    sphereAPtr->setRadius(0.5f);
    sphereAPtr->setPosition(Vec3(0, 0, -2));
    scenePtr->addObjectPtr(std::move(sphereAPtr));//sphereAPtr现在是nullptr

    std::shared_ptr<cg::RayTracingSampler> samplerPtr = std::make_shared<cg::RayTracingSampler>(500, 500);
    samplerPtr->setSamplingMode(cg::SamplingMode::VisibilityOnly);
    samplerPtr->loadCamera(cameraPtr);
    samplerPtr->loadScene(scenePtr);

    SDL_Application app;
    app.loadSampler(samplerPtr);
    app.Init();
    app.setEnableCameraMotion(true);
    app.setMouseCapture(true);
    app.Run([&](){});
    return 0;
}