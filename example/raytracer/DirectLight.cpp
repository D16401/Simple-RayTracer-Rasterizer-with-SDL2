#include <iostream>
#include <memory>

#include "simple_cg.h"

int main(int argc, char* argv[]){
    using cg::Vec3;
    std::shared_ptr<cg::Camera> cameraPtr = std::make_shared<cg::Camera>();

    std::shared_ptr<cg::Scene> scenePtr = std::make_shared<cg::Scene>();
    scenePtr->setAmbient(0.2f);

    auto sphereAPtr = std::make_unique<cg::Sphere>();//堆上创建
    sphereAPtr->setColor(0xFF0000FF);
    sphereAPtr->setRadius(0.5f);
    sphereAPtr->setPosition(Vec3(0, 0, -2));
    scenePtr->addObjectPtr(std::move(sphereAPtr));//sphereAPtr现在是nullptr
    auto planePtr = std::make_unique<cg::Plane>();
    planePtr->setColor(0xFFFF00FF);
    planePtr->setNormal(Vec3(0, 2, 0));
    planePtr->setPosition(Vec3(0, -1.3f, -2));
    scenePtr->addObjectPtr(std::move(planePtr));

    Vec3 L = Vec3(0,1,-1);
    Vec3 L_v = cameraPtr->getForward() * L.dot(cameraPtr->getForward())/(cameraPtr->getForward().length() * cameraPtr->getForward().length());
    Vec3 h = L - L_v;
    float r = h.length();

    auto DLightPtr = std::make_unique<cg::DirectionalLight>(0.8f);
    DLightPtr->setDirection(L);
    scenePtr->addLightPtr(std::move(DLightPtr));

    std::shared_ptr<cg::RayTracingSampler> samplerPtr = std::make_shared<cg::RayTracingSampler>(500, 500);
    samplerPtr->setSamplingMode(cg::SamplingMode::DirectLighting);
    samplerPtr->loadCamera(cameraPtr);
    samplerPtr->loadScene(scenePtr);

    SDL_Application app;
    app.loadSampler(samplerPtr);
    app.Init();
    app.setEnableCameraMotion(true);
    app.setMouseCapture(true);
    app.Run([&]() 
        {
            Vec3 newDirection = (
                L_v + 
                Vec3(static_cast<float>(r*cos(app.getTickCount()*2*M_PI/20)),
                    static_cast<float>(r*sin(app.getTickCount()*2*M_PI/20)),
                    0));
            auto* light = scenePtr->getLightPtrs()[0].get();
            if (!light) {
                std::cerr << "Fail to modify the light. Nullptr error!" << std::endl;
                return;
            }
            if (auto* directionalLight = dynamic_cast<cg::DirectionalLight*>(light)) {
                directionalLight->setDirection(newDirection);
            }
        }
    );
    return 0;
}