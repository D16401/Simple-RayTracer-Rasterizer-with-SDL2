#include <iostream>
#include <memory>

#include <SDL2/SDL.h>

#include "cg_math.h"
#include "simple_cg.h"

int main(int argc, char* argv[]){
    std::shared_ptr<Camera> cameraPtr = std::make_shared<Camera>();
    cameraPtr->setCameraMode(CameraMode::RecursiveReflection);
    cameraPtr->setReflectionDepth(2);
    
    std::shared_ptr<Scene> scenePtr = std::make_shared<Scene>();
    scenePtr->setAmbientLight(0.2f);
    scenePtr->setBackground(0x000000FF);

    auto sphereAPtr = std::make_unique<Sphere>();//堆上创建
    sphereAPtr->setColor(0xFF0000FF);
    sphereAPtr->setRadius(0.5f);
    sphereAPtr->setPosition(Vec3(-0.6f, 0, -2.0f));
    sphereAPtr->setSpecular(100);
    sphereAPtr->setReflectivity(0.2f);
    scenePtr->addObjectPtr(std::move(sphereAPtr));//sphereAPtr现在是nullptr

    auto sphereBPtr = std::make_unique<Sphere>();//堆上创建
    sphereBPtr->setColor(0x0000FFFF);
    sphereBPtr->setRadius(0.5f);
    sphereBPtr->setPosition(Vec3(0.6f, 0, -2.0f));
    sphereBPtr->setSpecular(100);
    sphereBPtr->setReflectivity(0.2f);
    scenePtr->addObjectPtr(std::move(sphereBPtr));//sphereBPtr现在是nullptr

    auto planePtr = std::make_unique<Plane>();
    planePtr->setColor(0x88AA88FF);
    planePtr->setNormal(Vec3(0, 2, 0));
    planePtr->setPosition(Vec3(0, -1.3f, -2.0f));
    planePtr->setSpecular(1000);
    planePtr->setReflectivity(0.f);
    scenePtr->addObjectPtr(std::move(planePtr));

    Vec3 L = Vec3(0,1,-1);
    Vec3 L_v = cameraPtr->getForward() * L.dot(cameraPtr->getForward())/(cameraPtr->getForward().length() * cameraPtr->getForward().length());
    Vec3 h = L - L_v;
    float r = h.length();

    auto PLightPtr = std::make_unique<PointLight>(0.8f);
    PLightPtr->setPosition(L);
    scenePtr->addLightPtr(std::move(PLightPtr));


    std::shared_ptr<RayTracingSampler> samplerPtr = std::make_shared<RayTracingSampler>(500, 500);
    samplerPtr->loadCamera(cameraPtr);
    samplerPtr->loadScene(scenePtr);

    SDL_Application app;
    app.loadSampler(samplerPtr);
    app.Init(false);
    app.Run([&]()
        {
            Vec3 newPosition = (
                L_v + 
                Vec3(static_cast<float>(r*cos(app.getTickCount()*2*M_PI/400)),
                    static_cast<float>(r*sin(app.getTickCount()*2*M_PI/400)),
                    0));
            auto* light = scenePtr->getLightPtrs()[0].get();
            if (!light) {
                // 空指针保护
                std::cerr << "Fail to modify the light. Nullptr error!" << std::endl;
                return;
            }
            if (auto* pointLight = dynamic_cast<PointLight*>(light)) {
                pointLight->setPosition(newPosition);
            }
        }
    );
    return 0;
}