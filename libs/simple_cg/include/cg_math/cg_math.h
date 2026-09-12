#pragma once

#include <cmath>
#include <cstdint>
#include <algorithm>

namespace cg{

#define CGMath_EPS 1e-7f
#define CGMATH_INF 1e7f

class Vec3{
private:
    float x, y, z;
public:
    Vec3(float x, float y, float z): x(x), y(y), z(z){}
    Vec3():x(0), y(0), z(0){}
    Vec3(float scalar):x(scalar), y(scalar), z(scalar){}
    Vec3(float x, float y):x(x), y(y), z(0){}
    
    Vec3 operator+(const Vec3& other) const;
    Vec3 operator-(const Vec3& other) const;
    Vec3 operator*(float scalar) const;   // 数乘
    Vec3 operator/(float scalar) const;
    
    float dot(const Vec3& other) const;   // 点乘
    Vec3 cross(const Vec3& other) const;  // 叉乘
    float length() const;                 // 长度
    Vec3 normalize() const;               // 归一化
    
    float getX() const{return x;}  // 获取x坐标
    float getY() const{return y;}  // 获取y坐标
    float getZ() const{return z;}  // 获取z坐标
    void print()const;
};

class Vec2{
private:
    float x, y;
public:
    Vec2(float x, float y):x(x), y(y){}
    Vec2():x(0), y(0){}
    Vec2(float scalar):x(scalar), y(scalar){}

    Vec2 operator+(const Vec2& other) const;
    Vec2 operator-(const Vec2& other) const;
    Vec2 operator*(float scalar) const;   // 数乘
    Vec2 operator/(float scalar) const;
    
    float dot(const Vec2& other) const;   // 点乘
    float cross(const Vec2& other) const;  // 伪叉乘
    float length() const;                 // 长度
    Vec2 normalize() const;               // 归一化
    
    float getX() const{return x;}  // 获取x坐标
    float getY() const{return y;}  // 获取y坐标
};

class Ray{
public:
    Ray(){}
    Ray(Vec3 origin, Vec3 direction): origin(origin), direction(direction){}
    //getter
    Vec3 getOrigin() const {return origin;}
    Vec3 getDirection() const {return direction;}
    //setter
    void setDirection(Vec3 vector_D){direction = vector_D;}
private:
    Vec3 origin = Vec3(0.0f, 0.0f, 0.0f);
    Vec3 direction = Vec3(0.0f, 0.0f, 1.0f);
};

int solveQuadratic(float a, float b, float c, float& x1, float& x2);
//求解二次方程，返回值为根的个数：0无实根，1有重根，2有两不同实根

Vec3 rotateAroundAxis(Vec3 vector, Vec3 axis, float angle);

int randint(int l, int r);//输入最小值l和最大值r，生成闭区间[l, r]内的随机整数

inline std::uint32_t blendByReflectivity(std::uint32_t c1, std::uint32_t c2, float reflectivity){
    reflectivity = std::clamp(reflectivity, 0.0f, 1.0f);
    float ir = 1.0f - reflectivity;
    // RRGGBBAA
    std::uint32_t r = static_cast<std::uint32_t>(
        ((c1 >> 24) & 0xFF) * ir +
        ((c2 >> 24)  & 0xFF) * reflectivity + 0.5f);
    std::uint32_t g = static_cast<std::uint32_t>(
        ((c1 >> 16) & 0xFF) * ir +
        ((c2 >> 16) & 0xFF) * reflectivity + 0.5f);
    std::uint32_t b = static_cast<std::uint32_t>(
        ((c1 >> 8)  & 0xFF) * ir +
        ((c2 >> 8)  & 0xFF) * reflectivity + 0.5f);
    std::uint32_t a = static_cast<std::uint32_t>(
        ((c1 >> 0)  & 0xFF) * ir +
        ((c2 >> 0)  & 0xFF) * reflectivity + 0.5f);
    return (r << 24) | (g << 16) | (b << 8) | (a << 0);
};
inline std::uint32_t colorScale(std::uint32_t color, float weight)
{
    weight = std::clamp(weight, 0.0f, 1.0f);

    std::uint32_t r = static_cast<std::uint32_t>(
        ((color >> 24) & 0xFF) * weight + 0.5f);
    std::uint32_t g = static_cast<std::uint32_t>(
        ((color >> 16) & 0xFF) * weight + 0.5f);
    std::uint32_t b = static_cast<std::uint32_t>(
        ((color >> 8)  & 0xFF) * weight + 0.5f);
    std::uint32_t a = static_cast<std::uint32_t>(
        ((color >> 0)  & 0xFF) * weight + 0.5f);

    return (r << 24) | (g << 16) | (b << 8) | a;
};
    
}