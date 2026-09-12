#include "cg_rz_drawer.h"

#include <iostream>
#include <memory>
#include <algorithm>

namespace cg{

/*
void SamplerDrawer::drawLine(
    RasterizingSampler* output, Vec2 P0, Vec2 P1, uint32_t color
){
    float x0 = P0.getX();
    float x1 = P1.getX();
    float y0 = P0.getY();
    float y1 = P1.getY();
    float dx = x1 - x0;
    float dy = y1 - y0;
    if (abs(dx) > abs(dy)){
        if (x0 > x1){
            std::swap(x0, x1);
            std::swap(y0, y1);
        }
        float k = dy/dx;
        float fy = y0;
        for (int x = static_cast<int>(x0); x <= static_cast<int>(x1); x++){
            int y = static_cast<int>(fy);
            output->writeBufferPixel(x, y, color);
            fy = fy + k;
        }
    }else{
        if (y0 > y1){
            std::swap(x0, x1);
            std::swap(y0, y1);
        }
        float k = dx/dy;
        float fx = x0;
        for (int y = static_cast<int>(y0); y <= static_cast<int>(y1); y++){
            int x = static_cast<int>(fx);
            output->writeBufferPixel(x, y, color);
            fx = fx + k;
        }
    }
}
*/
void SamplerDrawer::drawLine(
    RasterizingSampler* output, Vec2 P0, Vec2 P1, uint32_t color
){
    float x0 = static_cast<int>(P0.getX());
    float x1 = static_cast<int>(P1.getX());
    float y0 = static_cast<int>(P0.getY());
    float y1 = static_cast<int>(P1.getY());
    int dx = abs(x1 - x0);
    int dy = -abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;
    while (true)
    {
        output->writeBufferPixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) {break;}
        int e2 = 2*err;
        if (e2 >= dy){
            if (x0 == x1) {break;}
            err = err + dy;
            x0 = x0 + sx;
        }
        if (e2 <= dx){
            if (y0 == y1) {break;}
            err = err + dx;
            y0 = y0 + sy;
        }
    }
}//Bresenham

void SamplerDrawer::drawTriFrame(
    RasterizingSampler* output, Vec2 P0, Vec2 P1, Vec2 P2, uint32_t color
){
    drawLine(output, P0, P1, color);
    drawLine(output, P1, P2, color);
    drawLine(output, P2, P1, color);
}

float InterpolateX(float given_y, Vec2 P0, Vec2 P1) {
    float dy = P1.getY() - P0.getY();
    if (std::abs(dy) < 1e-6f) {
        // 水平线：直接返回任意端点 x（逻辑上安全）
        return P0.getX();
    }else{
        return P0.getX()
            + (P1.getX() - P0.getX())
            * (given_y - P0.getY())
            / dy;
    }
}

void SamplerDrawer::drawFilledTri(
    RasterizingSampler* output, Vec2 P0, Vec2 P1, Vec2 P2, uint32_t color
){
    float x0 = P0.getX();
    float x1 = P1.getX();
    float x2 = P2.getX();
    float y0 = P0.getY();
    float y1 = P1.getY();
    float y2 = P2.getY();
    int height = output->getHeight();
    if ((static_cast<int>(x0) == static_cast<int>(x1) && static_cast<int>(x0) == static_cast<int>(x2)) ||
        (static_cast<int>(y0) == static_cast<int>(y1) && static_cast<int>(y0) == static_cast<int>(y2))){
        drawLine(output, P0, P2, color);
        drawLine(output, P0, P1, color);
        drawLine(output, P1, P2, color);
        return;
    }
    //sort to make sure P0 is the lowest P & P2 is the highest P
    //y0 < y1 < y2
    if (y1 < y0) { std::swap(y1, y0); std::swap(x1, x0); }
    if (y2 < y0) { std::swap(y2, y0); std::swap(x2, x0); }
    if (y2 < y1) { std::swap(y2, y1); std::swap(x2, x1); }
    int y_min = std::clamp(static_cast<int>(std::ceil(y0)), 0, height - 1);
    int y_max = std::clamp(static_cast<int>(std::floor(y2)), 0, height - 1);
    for (int y = y_min; y <= y_max; ++y){//y由y0递增到y2
        float fx_left = InterpolateX(static_cast<float>(y), P0, P2);;
        float fx_right;
        if (y < y1){
            fx_right = InterpolateX(static_cast<float>(y), P0, P1);
        }else{
            fx_right = InterpolateX(static_cast<float>(y), P1, P2);
        }
        if (fx_left > fx_right){
            std::swap(fx_left, fx_right);
        }
        int x_start = static_cast<int>(std::ceil(fx_left));
        int x_end = static_cast<int>(std::floor(fx_right));
        for (int x = x_start; x <= x_end ; x++){
            if (!output->writeBufferPixel(x, y, color)){
                return;
            };
        }
    }
}
void SamplerDrawer::drawShadedTri(RasterizingSampler* output, Vec3 P0_d, Vec3 P1_d, Vec3 P2_d, uint32_t color){
    float x0 = P0_d.getX();
    float x1 = P1_d.getX();
    float x2 = P2_d.getX();
    float y0 = P0_d.getY();
    float y1 = P1_d.getY();
    float y2 = P2_d.getY();
    float d0 = P0_d.getZ();
    float d1 = P1_d.getZ();
    float d2 = P2_d.getZ();
    int height = output->getHeight();
    //sort by y
    if (y1 < y0) { std::swap(y1, y0); std::swap(x1, x0); std::swap(d1, d0);}
    if (y2 < y0) { std::swap(y2, y0); std::swap(x2, x0); std::swap(d2, d0);}
    if (y2 < y1) { std::swap(y2, y1); std::swap(x2, x1); std::swap(d2, d1);}
    int y_min = std::clamp(static_cast<int>(std::ceil(y0)), 0, height - 1);
    int y_max = std::clamp(static_cast<int>(std::floor(y2)), 0, height - 1);
    for (int y = y_min; y <= y_max; ++y){//y由y0递增到y2
        float fx_left = InterpolateX(static_cast<float>(y), Vec2(x0, y0), Vec2(x2, y2));
        float scaleYleft = ((y - y0)*d2 + (y2 - y)*d0) / (y2 - y0);
        float fx_right;
        float scaleYright;
        if (y < y1){
            fx_right = InterpolateX(static_cast<float>(y), Vec2(x0, y0), Vec2(x1, y1));
            scaleYright = ((y - y0)*d1 + (y1 - y)*d0) / (y1 - y0);
        }else{
            fx_right = InterpolateX(static_cast<float>(y), Vec2(x1, y1), Vec2(x2, y2));
            scaleYright = ((y - y1)*d2 + (y2 - y)*d1) / (y2 - y1);
        }
        if (fx_left > fx_right){
            std::swap(fx_left, fx_right);
            std::swap(scaleYleft, scaleYright);
        }
        int x_start = static_cast<int>(std::ceil(fx_left));
        int x_end = static_cast<int>(std::floor(fx_right));
        for (int x = x_start; x <= x_end ; x++){
            if (x_start > x_end) { continue; }
            float scale = ((x - x_start) * scaleYright + (x_end - x) * scaleYleft) / (x_end - x_start);
            uint32_t shaded_color = colorScale(color, scale);
            if (!output->writeBufferPixel(x, y, shaded_color)){
                return;
            };
        }
    }
}

};