#pragma once

#include "cg_sampler_rz.h"
#include "cg_rz_structure.h"

namespace cg{

Vec3 ProjectToViewport(const cg::Camera& camera, const Vec3& worldPos);//物体坐标投影到视口坐标（之后再映射到画布对应位置）
Vec2 ViewportToCanvas(const cg::Sampler& sampler, const cg::Camera& camera, const Vec3& pos);//将视口上的点映射到画布上映射到画布对应位置

void renderMesh(const cg::Camera& camera, const Mesh& mesh);
    
}