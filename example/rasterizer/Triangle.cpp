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
        cg::SamplerDrawer drawer;
        drawer.drawShadedTri(samplerPtr.get(), Vec3(250, 50, 1), Vec3(75, 425, 0), Vec3(425, 425, 0), 0xFFFF00FF);
    });
    return 0;
}