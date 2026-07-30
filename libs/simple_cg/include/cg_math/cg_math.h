#pragma once

#include <cmath>

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

int solveQuadratic(float a, float b, float c, float& x1, float& x2);
//求解二次方程，返回值为根的个数：0无实根，1有重根，2有两不同实根

int randint(int l, int r);