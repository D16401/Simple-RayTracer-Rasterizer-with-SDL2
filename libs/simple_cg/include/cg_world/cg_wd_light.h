#pragma once

#include <cstdint>
#include <string>

#include "cg_math.h"

namespace cg{

enum class LightType : uint8_t {
    None,
    Point,          //点光源
    Directional,     //方向光
};
struct LightBuild{
    std::string name;
    float intensity = 0;//光强(0~1)
    LightType type = LightType::None;//光源类型(0~1)
};
class Light{
public:
    Light(float intensity, LightType type): intensity(std::clamp(intensity, 0.f, 1.f)), type(type), name("unknown"){}
    Light(float intensity, LightType type, const char* name): intensity(std::clamp(intensity, 0.f, 1.f)), type(type), name(name){}
    Light(const LightBuild& build): name(build.name), intensity(build.intensity), type(build.type){}
    //getter
    float getIntensity() const { return std::clamp(intensity, 0.f, 1.f); }//返回光源强度(0~1)
    LightType getType() const {return type;}
    //getter(for algorithm)
    virtual Vec3 getHitInDirection(const Vec3& HitPoint) const = 0;//给定某点，返回此光源指向该点的方向
    //setter
    void setIntensity(float i){intensity = std::clamp(i, 0.f, 1.f);}
private:
    std::string name;
    float intensity = 0;//光强(0~1)
    LightType type;//光源类型(0~1)
};
struct PointLightBuild : LightBuild{
    Vec3 position = Vec3(0, 0, 0);//点光源位置，默认世界原点
};
class PointLight: public Light{//Point Light，有位置无方向
public:
    PointLight(float intensity): Light(intensity, LightType::Point){}
    PointLight(float intensity, const char* name): Light(intensity, LightType::Point, name){}
    PointLight(const PointLightBuild build): Light(build), position(build.position){}
    //getter
    Vec3 getPosition() const {return position;}
    //getter(for algorithm)
    Vec3 getHitInDirection(const Vec3& HitPoint)const override {return HitPoint - position;}//给定某点，返回此点光源指向该点的方向
    //setter
    void setPosition(Vec3 P){position = P;}
private:
    Vec3 position = Vec3(0, 0, 0);//点光源位置，默认世界原点
};
struct DirectionalLightBuild : LightBuild{
    Vec3 direction = Vec3(0, 0, 1);//平行光源方向，默认(0,0,1)
};
class DirectionalLight: public Light{//Directional Light，有方向无位置
public:
    DirectionalLight(float intensity): Light(intensity, LightType::Directional){}
    DirectionalLight(float intensity, const char* name): Light(intensity, LightType::Directional, name){}
    DirectionalLight(DirectionalLightBuild build): Light(build), direction(build.direction){}
    //getter
    Vec3 getDirection() const {return direction;}
    //getter(for algorithm)
    Vec3 getHitInDirection(const Vec3& HitPoint)const override {return direction;};//给定某点，返回此平行光源指向该点的方向（恒为平行光的方向）
    //setter
    void setDirection(Vec3 D){direction = D;}
private:
    Vec3 direction = Vec3(0, 0, 1);//平行光源方向，默认(0,0,1)
};

};