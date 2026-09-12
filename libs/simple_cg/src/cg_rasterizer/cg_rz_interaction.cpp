#include "cg_rz_interaction.h"

using cg::Vec3;
using cg::Vec2;
using cg::Camera;
using cg::Sampler;

Vec3 cg::ProjectToViewport(const Camera& camera, const Vec3& pos){
    //假定相机位置(0,0,0)，方向朝向(0,0,-1)
    float viewportDepth = camera.getDepth();
    float projectX = pos.getX() * viewportDepth / pos.getZ();
    float projectY = pos.getY() * viewportDepth / pos.getZ();
    return Vec3(projectX, projectY, -viewportDepth);
};

Vec2 cg::ViewportToCanvas(const Sampler& sampler, const Camera& camera, const Vec3& pos){
    //假定相机位置(0,0,0),方向朝向(0,0,-1)，视口深度depth
    float viewportX = pos.getX();
    float viewportY = pos.getY();
    float canvasX = viewportX / camera.getViewportW() * sampler.getWidth();
    float canvasY = viewportY / camera.getViewportH() * sampler.getHeight();
    return Vec2(canvasX, canvasY);
}