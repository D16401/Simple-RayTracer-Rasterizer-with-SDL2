#include <iostream>
#include <memory>

#include <SDL2/SDL.h>

#include "cg_math.h"
#include "simple_cg.h"

int main(int argc, char* argv[]){
    std::shared_ptr<Camera> cameraPtr = std::make_shared<Camera>();
    cameraPtr->setCameraMode(CameraMode::DirectLighting);
    std::shared_ptr<Scene> scenePtr = std::make_shared<Scene>();
    scenePtr->setAmbientLight(0.2f);

    auto sphereAPtr = std::make_unique<Sphere>();//堆上创建
    sphereAPtr->setColor(0xFF0000FF);
    sphereAPtr->setRadius(0.5f);
    sphereAPtr->setPosition(Vec3(0, 0, -2));
    scenePtr->addObjectPtr(std::move(sphereAPtr));//sphereAPtr现在是nullptr


    Vec3 L = Vec3(0,1,-1);
    Vec3 L_v = cameraPtr->getForward() * L.dot(cameraPtr->getForward())/(cameraPtr->getForward().length() * cameraPtr->getForward().length());
    Vec3 h = L - L_v;
    float r = h.length();

    auto DLightPtr = std::make_unique<DirectionalLight>(0.8f);
    DLightPtr->setDirection(L);
    scenePtr->addLightPtr(std::move(DLightPtr));

    std::shared_ptr<RayTracingSampler> samplerPtr = std::make_shared<RayTracingSampler>(500, 500);
    samplerPtr->loadCamera(cameraPtr);
    samplerPtr->loadScene(scenePtr);

    SDL_Application app;
    app.loadSampler(samplerPtr);
    app.Init();
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
            if (auto* directionalLight = dynamic_cast<DirectionalLight*>(light)) {
                directionalLight->setDirection(newDirection);
            }
        }
    );
    return 0;
}