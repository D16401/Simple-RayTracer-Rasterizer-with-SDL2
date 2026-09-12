#pragma once

#include <memory>

#include "cg_math.h"
#include "cg_sampler_rz.h"

namespace cg{
    
class SamplerDrawer{
public:
    void drawLine(RasterizingSampler* output, Vec2 P0, Vec2 P1, uint32_t color);//给定画布上两点坐标绘制单色直线
    void drawTriFrame(RasterizingSampler* output, Vec2 P0, Vec2 P1, Vec2 P2, uint32_t color);//给定画布上两点坐标绘制单色三角形线框
    void drawFilledTri(RasterizingSampler* output, Vec2 P0, Vec2 P1, Vec2 P2, uint32_t color);//给定画布上两点坐标绘制单色填充实三角形
    void drawShadedTri(RasterizingSampler* output, Vec3 P0_d, Vec3 P1_d, Vec3 P2_d, uint32_t color);//Vec3 P0_d = (x, y, depth)，给定三点及各点阴影权重（0~1）绘制单色阴影实三角形
};

};