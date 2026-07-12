#include "cg_sampler_raytracing.h"

#include "cg_sampler.h"
#include "cg_interaction.h"

void RayTracingSampler::UpdateBuffer(uint32_t* dst, int pitch){//传入texture的内存位置
    for (int j = 0; j < height; j++){
        for(int i = 0; i < width; i++){
            Vec2 canvasP = Vec2(float(i), float(j));
            Vec3 viewportP = CanvasToViewport(*cameraPtr, canvasP);
            uint32_t color = SimpleRayTracing(*cameraPtr, *scenePtr, viewportP);
            dst[j * pitch + i] = color;//直接写入api的texture内存
        }
    }
}

void RayTracingSampler::loadCamera(std::shared_ptr<const Camera> camera_ptr){
    cameraPtr = camera_ptr;
}

void RayTracingSampler::loadScene(std::shared_ptr<const Scene> scene_ptr){
    scenePtr = scene_ptr;
}
