#pragma once

#include <iostream>
#include <cstdint>
#include <functional>
#include <memory>
#include <SDL2/SDL.h>

#include "cg_math.h"
#include "cg_sampler.h"

namespace cg{

struct SDLappBuild{
    std::string title = "Title";
    int canvasW = 500;//画布（窗口）宽度由绑定到app上的Sampler决定
    int canvasH = 500;//画布（窗口）高度由绑定到app上的Sampler决定
    int fps = 30;//每秒渲染的帧数
    bool enableCameraMotion = false;//相机运动开关
    bool mouseCaptured = false;//鼠标捕获开关
    std::shared_ptr<Sampler> samplerPtr = nullptr;//绑定的sampler
};
class SDL_Application{
public:
    SDL_Application() = default;
    SDL_Application(const char* title);
    SDL_Application(const SDL_Application&) = delete;
    SDL_Application(const SDLappBuild& build): title(build.title), canvasH(build.canvasH), canvasW(build.canvasW),
        fps(build.fps), enableCameraMotion(build.enableCameraMotion), mouseCaptured(build.mouseCaptured),
        samplerPtr(std::move(build.samplerPtr)){}
    SDL_Application& operator=(const SDL_Application&) = delete;
    ~SDL_Application();
    //main rendering program
    int loadSampler(std::shared_ptr<Sampler> sampler_ptr);//载入sampler，接受一个shared_ptr对象
    int Init(bool enableTestWindow = false);//根据载入的sampler设置，初始化窗口window和渲染器renderer
    int Run(std::function<void()> UpdateBlock = [](){});//根据
    int RenderFrame();//让renderer根据texture管理的内存所存储的像素数据进行一次单帧渲染
    int getTickCount(){return tickCount;}//返回运行时刻
    //run check
    bool isSamplerLoaded()const{return samplerPtr.get();}
    //input detect
    int HandleEvent();//处理事件，包括基础的窗口关闭和esc按下事件
    bool isKeyDown(SDL_Scancode sc)const;
    Vec2 getMousedelta()const;
    void setEnableCameraMotion(bool isEnable);
    void setMouseCapture(bool capture);
    void UpdateCameraMotion(float deltaSeconds);
private:
    //settings member
    std::string title = "Title";
    int canvasW = 500;//画布（窗口）宽度由绑定到app上的Sampler决定
    int canvasH = 500;//画布（窗口）高度由绑定到app上的Sampler决定
    int fps = 30;//每秒渲染的帧数
    bool enableCameraMotion = false;//相机运动开关
    bool mouseCaptured = false;//鼠标捕获开关
    //owned object
    SDL_Window* window = nullptr;//窗口对象SDL_Window
    SDL_Renderer* renderer = nullptr;//渲染器对象SDL_Renderer
    SDL_Texture* frameTexture = nullptr;//材质对象SDL_Texture，储存画面像素数据，作为renderer的渲染缓冲buffer
    std::shared_ptr<Sampler> samplerPtr;//sampler分为Raytracer和Rasterizer两者
    //interaction variable
    int tickCount = 0;//某次Run()内部维护的时刻
    int runtimes = 0;//一次Init初始化后Run()的执行次数
    Vec2 mouseDelta = Vec2(0,0);//每帧鼠标的相对位移
};
  
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

