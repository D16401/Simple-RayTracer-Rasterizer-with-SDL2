#include <iostream>
#include <memory>
#include "simple_cg.h"

int main(int argc, char* argv[]){
    int width = 500;
    int height = 500;
    std::shared_ptr<cg::RasterizingSampler> samplerPtr = std::make_shared<cg::RasterizingSampler>(width, height);
    SDL_Application app;
    app.loadSampler(samplerPtr);
    app.Init();
    app.Run([&](){
        int tick = app.getTickCount();
        int delay_tick = tick - 300;
        int r = 100;
        Vec2 P0(static_cast<float>(width/2 + r*cos(tick*M_PI/500)), static_cast<float>(height/2 + r*sin(tick*M_PI/500)));
        Vec2 P1(static_cast<float>(width/2 - r*cos(tick*M_PI/500)), static_cast<float>(height/2 - r*sin(tick*M_PI/500)));
        Vec2 P2(static_cast<float>(width/2 + r*cos(delay_tick*M_PI/500)), static_cast<float>(height/2 + r*sin(delay_tick*M_PI/500)));
        Vec2 P3(static_cast<float>(width/2 - r*cos(delay_tick*M_PI/500)), static_cast<float>(height/2 - r*sin(delay_tick*M_PI/500)));
        cg::SamplerDrawer drawer;
        drawer.drawLine(samplerPtr.get(), P0, P1, 0x00FFFFFF);
        drawer.drawLine(samplerPtr.get(), P2, P3, 0x00000FF);
        drawer.drawFilledTri(samplerPtr.get(), Vec2(2, 125), Vec2(2, 250), Vec2(125,125), 0xFF00FFFF);
    });
    return 0;
}