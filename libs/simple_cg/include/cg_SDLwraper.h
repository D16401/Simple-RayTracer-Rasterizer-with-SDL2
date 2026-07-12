#include <iostream>
#include <cstdint>
#include <functional>
#include <memory>
#include <SDL2/SDL.h>

#include "cg_math.h"
#include "cg_sampler.h"

class SDL_Application{
public:
    SDL_Application() = default;
    SDL_Application(const char* title);
    SDL_Application(const SDL_Application&) = delete;
    SDL_Application& operator=(const SDL_Application&) = delete;
    ~SDL_Application(); 
    int loadSampler(std::shared_ptr<Sampler> sampler_ptr);
    int Init(bool enableTestWindow = false);//根据载入的相机设置，初始化窗口和渲染器
    int Run(std::function<void()> UpdateBlock = [](){});    
    int HandleEvent();//处理事件，窗口关闭和esc按下事件
    int RenderFrame();
    int getTickCount(){return tickCount;}
private:
    const char* title = "Title";
    int canvasW = 500;
    int canvasH = 500;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* frameTexture = nullptr;
    std::shared_ptr<Sampler> samplerPtr;
    int tickCount = 0;
    int fps = 30;
    int runtimes = 0;
};

/*
inline SDL_Color ConvertToSDL_Color(uint32_t packed) {
    SDL_Color c;
    c.r = (packed >> 24) & 0xFF;  // RR
    c.g = (packed >> 16) & 0xFF;  // GG
    c.b = (packed >> 8)  & 0xFF;  // BB
    c.a =  packed        & 0xFF;  // AA
    return c;
}
*/

