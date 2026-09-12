#include "cg_wd_camera.h"

using cg::Camera;

void Camera::rotateYaw(float radians){//以世界的Y轴为轴旋转（而不是相机的up方向）forward和right，输入正角度视角向左转
    forward = rotateAroundAxis(forward, Vec3(0,1,0), radians).normalize();
    right = rotateAroundAxis(right, Vec3(0,1,0), radians).normalize();
    up = right.cross(forward).normalize();
};
void Camera::rotatePitch(float radians){//以right为轴上下旋转forward和up，输入正角度视角向上转
    forward = rotateAroundAxis(forward, right, radians).normalize();
    up = rotateAroundAxis(up, right, radians).normalize();
    right = forward.cross(up).normalize();
};