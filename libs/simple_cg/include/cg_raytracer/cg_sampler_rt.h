#pragma once 
#include "cg_sampler.h"

#include <memory>
#include "cg_rt_structure.h"


class RayTracingSampler: public Sampler{
public:
    RayTracingSampler(): Sampler(0, 0){}
    RayTracingSampler(int w, int h): Sampler(w, h){}
    ~RayTracingSampler() override = default;
    void loadCamera(std::shared_ptr<const Camera> camera_ptr);
    void loadScene(std::shared_ptr<const Scene> scene_ptr);
    void UpdateBuffer(uint32_t* dst, int pitch);
private:
    std::shared_ptr<const Camera> cameraPtr;
    std::shared_ptr<const Scene> scenePtr;
};