#pragma once

#include <cstdint>
#include <string>

#include "cg_math.h"

namespace cg{

struct Material{//物体基础材质参数
    uint32_t color = 0x000000FF;//物体固有色，默认不透明黑色0x000000FF
    int specular = -1;//镜面反射系数，默认-1
};

struct RaytracingAttribute{//光追渲染参数
    float reflectivity = 0.00f;//反射度
};
struct RasterizingAttribute{//光栅渲染参数
    int segments = 24;//球体经线数（烘焙细分）
    int stacks = 16;//球体纬线数（烘焙细分）
    float size = 5.0f;//平面有界patch边长（烘焙）
};

//物体分类枚举
enum class ObjectType : uint8_t {
    None,           //未指定的物体类型
    Sphere,         //球体
    Plane,          //无穷平面
    OneSideQuad,    //单面矩形
};

struct ObjectBuild{
    std::string name;//物体名称
    Vec3 position = Vec3(0.0f, 0.0f, 0.0f); //物体位置，默认为世界原点
    ObjectType type = ObjectType::None;     //物体类别，未指定时默认为NONE
    Material material;              //物体基础材质参数
    RaytracingAttribute rtAttr;     //光追渲染参数
    RasterizingAttribute rzAttr;    //光栅渲染参数
};
inline bool checkObjectBuild(const ObjectBuild& build){return build.type != ObjectType::None;}
//物体抽象基类
class Object{
public:
    Object(ObjectType type): type(type), name("unknown"){}
    Object(ObjectType type, const char* name): type(type), name(name){}
    Object(const ObjectBuild& build): type(build.type), name(build.name), position(build.position),
        material(build.material), rtAttr(build.rtAttr), rzAttr(build.rzAttr){}
    //getter(regular attribute)
    std::string getName() const {return name;}
    Vec3 getPosition() const {return position;}
    ObjectType getType() const {return type;}
    Material& getMaterial() {return material;}              //修改物体基础材质参数
    const Material& getMaterial() const {return material;}  //获取物体基础材质参数
    uint32_t getColor() const {return material.color;}
    RaytracingAttribute& getRtAttr() {return rtAttr;}       //修改光追渲染参数
    const RaytracingAttribute& getRtAttr() const {return rtAttr;}//获取光追渲染参数
    RasterizingAttribute& getRzAttr() {return rzAttr;}      //修改光栅渲染参数
    const RasterizingAttribute& getRzAttr() const {return rzAttr;}//获取光栅渲染参数
    //getter(for algorithm)
    virtual Vec3 getNormal(const Vec3& Hitpoint) const = 0;
    //setter
    void setName(const std::string& n){name = n;}
    void setPosition(Vec3 pos){position = pos;}
    void setColor(uint32_t color){material.color = color;}
    void setSpecular(int specular){material.specular = specular;}
private:
    std::string name;//物体名称
    Vec3 position = Vec3(0.0f, 0.0f, 0.0f);//物体位置，默认为世界原点
    ObjectType type = ObjectType::None;//物体类别，未指定时默认为NONE
    Material material;//物体基础材质参数
    RaytracingAttribute rtAttr;//光追渲染参数
    RasterizingAttribute rzAttr;//光栅渲染参数
};
struct SphereBuild: ObjectBuild{
    float radius = 1.0;
};
inline bool checkSphereBuild(const SphereBuild& build){return checkObjectBuild(build);}
//派生类：球体
class Sphere: public Object{
public:
    Sphere(): Object(ObjectType::Sphere){}
    Sphere(const char* name): Object(ObjectType::Sphere, name){}
    Sphere(const SphereBuild& build): Object(build), radius(build.radius){}
    //getter(regular attribute)
    float getRadius() const {return radius;}
    //getter(for algorithm)
    Vec3 getNormal(const Vec3& Hitpoint) const override {return (Hitpoint - this->getPosition());}
    //setter
    void setRadius(float Sphere_radius){radius = Sphere_radius;}
private:
    float radius = 1.0;
};
struct PlaneBuild :ObjectBuild{
    Vec3 normal = Vec3(0,1,0);
};
inline bool checkPlaneBuild(const PlaneBuild& build){return checkObjectBuild(build);}
//派生类：无限平面
class Plane: public Object{
public:
    Plane(): Object(ObjectType::Plane){}
    Plane(const char* name): Object(ObjectType::Plane, name){}
    Plane(const PlaneBuild& build): Object(build), normal(build.normal){}
    //getter(for algorithm)
    Vec3 getNormal() const {return normal;}
    Vec3 getNormal(const Vec3& Hitpoint) const override {return normal;}
    //setter
    void setNormal(Vec3 N) {normal = N;}
private:
    Vec3 normal = Vec3(0,1,0);
};

//派生类：
class OneSideQuad: public Object{

};

};