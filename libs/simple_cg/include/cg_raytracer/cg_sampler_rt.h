#pragma once 

#include <cstdint>
#include <memory>

#include "cg_sampler.h"

namespace cg{

enum class SamplingMode : uint8_t {//采样（渲染）模式
    None,//哨兵值
    VisibilityOnly,     // 仅追踪最近交点，返回 albedo / normal
    DirectLighting,     // + Lambertian / Phong 光照
    HardShadows,        // + shadow ray
    RecursiveReflection // + recursive trace()
};
struct RaytracerBuild : SamplerBuild{
    SamplingMode samplingMode = SamplingMode::VisibilityOnly;//采样（渲染）模式
    int reflectionDepth = 1;//光线追踪算法递归深度，默认为1
};
class RayTracingSampler: public Sampler{
public:
    RayTracingSampler(): Sampler(0, 0){}
    RayTracingSampler(int w, int h): Sampler(w, h){}
    RayTracingSampler(RaytracerBuild build): Sampler(build), samplingMode(build.samplingMode), reflectionDepth(build.reflectionDepth){type = SamplerType::Raytracer;}
    ~RayTracingSampler() override = default;
    //getter
    SamplingMode getSamplingMode()const{return samplingMode;}//获取当前采样（渲染）模式
    int getReflectionDepth()const{return reflectionDepth;}//获取当前递归深度
    //setter
    void setSamplingMode(SamplingMode mode){samplingMode = mode;}//设置采样（渲染）模式
    void setReflectionDepth(int d){reflectionDepth = d;}
    //updater
    void UpdateBuffer();//根据光追算法更新buffer中的像素数据
private:
    SamplingMode samplingMode = SamplingMode::VisibilityOnly;//采样（渲染）模式
    int reflectionDepth = 1;//光线追踪算法递归深度，默认为1

};

};