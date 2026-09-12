#pragma once

#include "cg_math.h"

namespace cg{

struct CameraBuild{
    //viewportAttr
    float depth = 1.0f;
    float viewportW = 1.0f;
    float viewportH = 1.0f;
    float ratio = 1.0f;
    //outputAttr
    int canvasW = 500;
    int canvasH = 500;
    //cameraAttr，相机观察基
    Vec3 position = Vec3(0,0,0);
    Vec3 forward = Vec3(0,0,-1);
    Vec3 right = Vec3(1,0,0);
    Vec3 up = Vec3(0,1,0);
};

class Camera{
public:
    Camera() = default;
    Camera(const CameraBuild& build): depth(build.depth), viewportW(build.viewportW), viewportH(build.viewportH),
        ratio(build.ratio), canvasW(build.canvasW), canvasH(build.canvasH), position(build.position), 
        forward(build.forward.normalize()), right(build.right.normalize()), up(build.up.normalize()){}
    //getter(viewportAttr)
    float getDepth() const{return depth;}
    float getViewportW() const{return viewportW;}
    float getViewportH() const{return viewportH;}
    //getter(outputAttr)
    int getCanvasW() const{return canvasW;}
    int getCanvasH() const{return canvasH;}
    //cameraAttr，相机观察基
    Vec3 getPosition() const{return position;}
    Vec3 getForward() const{return forward;}
    Vec3 getUp() const{return up;}
    Vec3 getRight() const{return right;}
    //setter
    void setCanvasSize(int w, int h){canvasW = w; canvasH = h; ratio = static_cast<float>(w)/h;}
    void setPosition(const Vec3& pos){position = pos;}
    //camera motion
    void moveForward(float distant){position = position + forward*distant;}
    void moveRight(float distant){position = position + right*distant;}
    void moveUp(float distant){position = position + up*distant;}
    void rotateYaw(float radians);//以世界的Y轴为轴旋转（而不是相机的up方向）forward和right，输入正角度视角向左转
    void rotatePitch(float radians);//以right为轴上下旋转forward和up，输入正角度视角向上转
    void lookAt(const Vec3& target);//将forward变为当前位置指向target的单位向量，并修改right和up以重建对应的右手坐标系
private:
    //viewportAttr
    float depth = 1.0f;
    float viewportW = 1.0f;
    float viewportH = 1.0f;
    float ratio = 1.0f;
    //outputAttr
    int canvasW = 500;
    int canvasH = 500;
    //cameraAttr，相机观察基
    Vec3 position = Vec3(0,0,0);
    Vec3 forward = Vec3(0,0,-1);
    Vec3 right = Vec3(1,0,0);
    Vec3 up = Vec3(0,1,0);
};

};