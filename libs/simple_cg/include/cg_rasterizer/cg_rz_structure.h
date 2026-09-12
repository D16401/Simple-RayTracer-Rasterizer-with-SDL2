#pragma once

#include <vector>

#include "cg_math.h"

namespace cg{

class Camera{
public:
    float getDepth() const {return depth;}
    float getViewportW()const {return viewportH;}
    float getViewportH()const {return viewportW;}
    //TODO
    //int getCanvasW() const {return canvasW;}
    //int getCanvasH() const {return canvasH;}

    Vec3 getPosition() const {return position;}
    Vec3 getForward() const {return forward;}
    Vec3 getUp() const {return up;}
    Vec3 getRight() const {return right;}
private:
    //sample setting
    float depth = 1.0f;//视口深度
    float viewportW = 1.0f;//视口宽
    float viewportH = 1.0f;//视口高
    //右手坐标系
    Vec3 position = Vec3(0.0f, 0.0f, 0.0f);//默认原点(0,0,0)
    Vec3 forward = Vec3(0.0f, 0.0f, -1.0f);//默认前朝向(0，0，-1)
    Vec3 right = Vec3(1.0f, 0.0f, 0.0f);//默认右朝向(1,0,0)
    Vec3 up = Vec3(0.0f, 1.0f, 0.0f);//默认上朝向(0,1,0)，满足叉乘式forward.cross(right)
};


struct Vertex{
    Vertex(Vec3 pos, uint32_t color): position(pos), color(color){}
    Vec3 position;
    uint32_t color;
};


class Triangle{
public:
    Vec3 getA(){return A;}
    Vec3 getB(){return B;}
    Vec3 getC(){return C;}
    uint32_t getColor(){return color;}
private:
    Vec3 A;
    Vec3 B;
    Vec3 C;
    uint32_t color;
};

class Mesh{
public:
    Mesh(Triangle tri){
        vertexs.push_back(Vertex(tri.getA(), tri.getColor()));
        vertexs.push_back(Vertex(tri.getB(), tri.getColor()));
        vertexs.push_back(Vertex(tri.getC(), tri.getColor()));
    };
private:
    std::vector<Vertex> vertexs;
    std::vector<uint32_t> metaMesh;
};


};