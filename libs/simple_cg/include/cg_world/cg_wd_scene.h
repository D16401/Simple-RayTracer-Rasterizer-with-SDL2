#pragma once

#include <cstdint>
#include <algorithm>
#include <vector>
#include <memory>

#include "cg_wd_object.h"
#include "cg_wd_light.h"

namespace cg{

class Object;
class Light;

struct SceneBuild{
    uint32_t background = 0x000000FF;//不透明黑色
    float ambient= 0;//Ambient Light环境光强
    std::vector<std::unique_ptr<Object>> ObjectPtrs;//物体指针列表
    std::vector<std::unique_ptr<Light>> LightPtrs;//光源指针列表
};

class Scene{
public:
    Scene(){}
    Scene(SceneBuild build);
    ~Scene();
    //getter
    uint32_t getBackgroundColor() const {return background;}//返回Scene实例的背景色
    float getAmbientLight() const {return std::clamp(ambient, 0.f, 1.f);}//返回当前scene的环境光强
    const std::vector<std::unique_ptr<Object>>& getObjectPtrs() const {return ObjectPtrs;}//返回物体指针列表(vector<unique_ptr<Object>>)
    const std::vector<std::unique_ptr<Light>>& getLightPtrs() const {return LightPtrs;}//返回光源指针列表(vector<unique_ptr<Object>>)
    //setter
    void setAmbient(float intensity){ambient = intensity;};//设置环境光强(0-1, float)
    void setBackground(uint32_t color){background = color;}//设置背景颜色(#RRGGBBAA)
    //adder
    void addObjectPtr(std::unique_ptr<Object> ObjectPtr);//为Scene实例添加物体（unique_ptr）//unique_ptr, #include <memory>
    void addLightPtr(std::unique_ptr<Light> LightPtr);//为Scene实例添加光源
private:
    uint32_t background = 0x000000FF;//不透明黑色
    float ambient= 0;//Ambient Light环境光强
    std::vector<std::unique_ptr<Object>> ObjectPtrs;//物体指针列表
    std::vector<std::unique_ptr<Light>> LightPtrs;//光源指针列表
};

};
