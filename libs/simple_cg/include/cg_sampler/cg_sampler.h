#pragma once

#include <vector>
#include <cstdint>

#include "cg_world.h"

namespace cg{

struct TextureMemory
{
    //    SDL_LockTexture(frameTexture, nullptr, &pixels, &pitch);
    //    uint32_t* dst = static_cast<uint32_t*>(pixels);
    uint32_t* dst;
    int pitch;
    TextureMemory(): dst(nullptr), pitch(0){}
    TextureMemory(uint32_t* dst, int pitch): dst(dst), pitch(pitch){}
    TextureMemory(const TextureMemory& memory): dst(memory.dst), pitch(memory.pitch){}
};
enum class SamplerType : uint8_t {//采样（渲染）模式
    None,
    Raytracer,
    Rasterizer
};
struct SamplerBuild{
    int width = 0;
    int height = 0;
    SamplerType type = SamplerType::None;
    std::shared_ptr<Camera> cameraPtr = nullptr;//绑定的相机
    std::shared_ptr<Scene> scenePtr = nullptr;//绑定的场景
};
class Sampler{
public:
    Sampler() = default;
    Sampler(int w, int h): width(w), height(h){}//设置sampler的宽度和高度，这将决定SDL_Application的window高宽和sampler在世界中的Camera的视口及画布高宽
    Sampler(SamplerBuild build): width(build.width), height(build.height), type(build.type),
        cameraPtr(std::move(build.cameraPtr)), scenePtr(std::move(build.scenePtr)){}
    virtual ~Sampler() = default;
    virtual void UpdateBuffer() = 0;//纯虚函数，根据sampler派生类的对应算法（包括光追和光栅化两类算法）更新渲染缓冲buffer(TextureMemory)。渲染前会对世界进行一次缓冲更新（采样操作）
    void setTextureMemory(uint32_t* dst, int pitch){t_memory.dst = dst, t_memory.pitch = pitch;}//将某块内存关联为sampler的TextureMemory，sampler采集的像素将直接写入TextureMemory
    bool writeBufferPixel(int x, int y, uint32_t color);//将一个RRGGBBAA的uint32_t写入TextureMemory的(x,y)位置
    //getter
    virtual void GetOutputSize(int& w, int& h)const {w = width; h = height;};//引用返回高和宽
    int getWidth()const {return width;}//返回sampler的宽
    int getHeight()const {return height;}//返回sampler的高
    Camera* getCameraPtr()const{return cameraPtr.get();}
    const Scene* getScenePtr()const{return scenePtr.get();}
    //loader
    void loadCamera(std::shared_ptr<Camera> camera_ptr);//关联Raytracing::Camera相机对象到RayTracingSampler对象，同时将相机画布高宽设置为Sampler的高宽
    void loadScene(std::shared_ptr<const Scene> scene_ptr);//关联场景Raytracing::Scene对象到RayTracingSampler对象
    //check
    bool isCameraLoaded()const{return cameraPtr.get();}
    bool isSceneLoaded()const{return scenePtr.get();}
protected:
    int width = 500;
    int height = 500;
    SamplerType type = SamplerType::None;
    TextureMemory t_memory;//储存供api直接读取的像素数据的内存，包括内存起始位置dst和块大小pitch。由api临时提供
    std::shared_ptr<Camera> cameraPtr;//绑定的相机
    std::shared_ptr<const Scene> scenePtr;//绑定的场景
};

};
