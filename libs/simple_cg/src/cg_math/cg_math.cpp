#include <cmath>
#include <utility>
#include <random>

#include "cg_math.h"


Vec3 Vec3::operator+(const Vec3& other) const{
    return Vec3(x+other.x, y+other.y, z+other.z);
}
Vec3 Vec3::operator-(const Vec3& other) const{
    return Vec3(x-other.x, y-other.y, z-other.z);
}
Vec3 Vec3::operator*(float scalar) const{
    return Vec3(x*scalar, y*scalar, z*scalar);
}
Vec3 Vec3::operator/(float scalar) const{
    return Vec3(x/scalar, y/scalar, z/scalar);
}
float Vec3::dot(const Vec3& other) const{
    return x*other.x + y*other.y + z*other.z;
}
Vec3 Vec3::cross(const Vec3& other) const{
    return Vec3(y*other.z - z*other.y, z*other.x - x*other.z, x*other.y - y*other.x);
}
float Vec3::length() const{
    return sqrt(x*x + y*y + z*z);
}
Vec3 Vec3::normalize() const{
    float len = length();
    return Vec3(x/len, y/len, z/len);
}


Vec2 Vec2::operator+(const Vec2& other) const{
    return Vec2(x + other.x, y+other.y);
}
Vec2 Vec2::operator-(const Vec2& other) const{
    return Vec2(x-other.x, y-other.y);
}
Vec2 Vec2::operator*(float scalar) const{// 数乘
    return Vec2(x*scalar, y*scalar);
}
Vec2 Vec2::operator/(float scalar) const{
    return Vec2(x/scalar, y/scalar);
};
float Vec2::dot(const Vec2& other) const{// 点乘
    return (x*other.x + y*other.y);
}
float Vec2::cross(const Vec2& other) const{//  伪叉乘
    return (x*other.y - y*other.x);
}
float Vec2::length() const{// 长度
    return sqrt(x*x + y*y);
}
Vec2 Vec2::normalize() const{// 归一化
    float len = length();
    return Vec2(x/len, y/len);
}

int solveQuadratic(float a, float b, float c, float& x1, float& x2){
    //求解二次方程，返回值为根的个数：0无实根，1有重根，2有两不同实根
    if (abs(a) < CGMath_EPS){
        if (abs(b) < CGMath_EPS){
            x1 = CGMATH_INF;
            x2 = x1;
            return 0;//a=b=0，不是一个方程，无解
        }else{
            x1 = -c/b;
            x2 = x1;
            return 1; //a=0，b!=0，是一元一次方程，有一个解
        }
    }else{
        float delta = b*b - 4.0f * a * c;
        if (delta < -CGMath_EPS){
            x1 = CGMATH_INF;
            x2 = x1;
            return 0;//二次曲线无交点，二次方程无实根
        }else if(delta < CGMath_EPS){
            x1 = -b / (2.0f *a);
            x2 = x1;
            return 1;//有重根
        }else{
            float q = -0.5f * (b+ copysign(sqrt(delta), b));
            x1 = q / a;
            x2 = c / q;
            if (x1 > x2){
                std::swap(x1, x2);//保证x1 < x2
            }
            return 2;//二次方程有两不同实根
        }
    }
}

int randint(int l, int r) {
    static std::mt19937 g{std::random_device{}()};
    return std::uniform_int_distribution<>(l, r)(g);
}
