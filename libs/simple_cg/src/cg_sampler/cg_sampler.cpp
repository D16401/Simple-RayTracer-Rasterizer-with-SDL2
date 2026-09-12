#include "cg_sampler.h"

#include <iostream>
#include <memory>

bool cg::Sampler::writeBufferPixel(int x, int y, uint32_t color){
    if (x < 0 || x >= width || y < 0 || y >= height){
        std::cerr << "invaild access" << std::endl;
        return false;
    }
    t_memory.dst[y * (t_memory.pitch / sizeof(uint32_t)) + x] = color;//直接写入api的texture内存
    return true;
}

void cg::Sampler::loadCamera(std::shared_ptr<cg::Camera> camera_ptr){//关联cg::Camera相机对象到Sampler对象，同时将相机画布高宽设置为Sampler的高宽
    cameraPtr = camera_ptr;
    camera_ptr->setCanvasSize(width, height);
}

void cg::Sampler::loadScene(std::shared_ptr<const cg::Scene> scene_ptr){//关联场景cg::Scene对象到Sampler对象
    scenePtr = scene_ptr;
}

