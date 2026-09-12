#include "sdl_wrapper.h"

#include <iostream>
#include <cstdint>
#include <functional>
#include <memory>
#include <cstring>
#include <SDL2/SDL.h>

#include "cg_math.h"

namespace cg{

SDL_Application::SDL_Application(const char* Title){
    title = Title;
}
SDL_Application::~SDL_Application() {
    if (renderer) { SDL_DestroyRenderer(renderer); }
    if (window)   { SDL_DestroyWindow(window); }
    SDL_Quit();
}
int SDL_Application::loadSampler(std::shared_ptr<cg::Sampler> sampler_ptr){
    samplerPtr = sampler_ptr;
    if (!samplerPtr){
        std::cerr << "Fail to load the sampler or a nullptr error." << std::endl;
        return -1;
    }
    int w, h;
    samplerPtr->GetOutputSize(w, h);
    canvasW = w;
    canvasH = h;
    return 0;
}
int SDL_Application::Init(bool enableTestWindow){
    //Init video module
    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cerr << "Fail to initialize SDL:" << SDL_GetError() << std::endl;
        return -1;
    }
    //Init window
    window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        canvasW,
        canvasH,
        SDL_WINDOW_SHOWN
    );
    if(!window){
        std::cerr << "Fail to create the window:" << SDL_GetError() << std::endl;
        return -1;
    }
    //Init renderer
    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if(!renderer){
	    std::cerr << "Fail to create the window:" << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    //optional test window rendering
    bool testWindow = enableTestWindow;
    if (testWindow) {
        std::cout << "SDL initialization test mode. Press ESC to continue main program..." << std::endl;
    }
    while(testWindow){
        SDL_SetRenderDrawColor(renderer, cg::randint(1, 255), cg::randint(1, 255), cg::randint(1, 255), 255);//设置颜色
        SDL_RenderClear(renderer);//绘制颜色
        SDL_RenderPresent(renderer);//显示（黑色）屏幕
        if(HandleEvent() == 1){
            testWindow = false;
            std::cout << "Test window is terminated by the user." << std::endl;
            return 0;
        }
        SDL_Delay(1000/fps);
    };
    frameTexture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        canvasW,
        canvasH
    );
    if(!frameTexture){
	    std::cerr << "Fail to create the texture:" << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    tickCount = 0;
    runtimes = 0;
    //end init
    return 0;
}
bool SDL_Application::isKeyDown(SDL_Scancode sc) const{
    const Uint8* state = SDL_GetKeyboardState(nullptr);//轮询键盘状态，返回以scancode为下标的数组
    return state != nullptr && state[sc] != 0;
}
Vec2 SDL_Application::getMousedelta() const{
    return mouseDelta;//返回HandleEvent每帧采样的值
}
void SDL_Application::setEnableCameraMotion(bool isEnable){
    enableCameraMotion = isEnable;
    if (SDL_SetRelativeMouseMode(enableCameraMotion ? SDL_TRUE : SDL_FALSE) < 0){
        std::cerr << "Fail to set CameraMotion: " << SDL_GetError() << std::endl;
    }
}
void SDL_Application::setMouseCapture(bool capture){
    mouseCaptured = capture;
    if (SDL_SetRelativeMouseMode(capture ? SDL_TRUE : SDL_FALSE) < 0){
        std::cerr << "Fail to set relative mouse mode: " << SDL_GetError() << std::endl;
    }
}

int SDL_Application::HandleEvent(){
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT){
            return 1;
        }else if(event.type == SDL_KEYDOWN){
            if (enableCameraMotion && !event.key.repeat &&
                (event.key.keysym.scancode == SDL_SCANCODE_LSHIFT ||
                 event.key.keysym.scancode == SDL_SCANCODE_RSHIFT)){
                setMouseCapture(!mouseCaptured);//切换鼠标捕捉/释放状态
            }
            switch (event.key.keysym.sym)
            {
                case SDLK_ESCAPE:
                    return 1;
            }
        }
    }
    int dx = 0, dy = 0;
    SDL_GetRelativeMouseState(&dx, &dy);
    mouseDelta = Vec2(static_cast<float>(dx), static_cast<float>(dy));
    return 0;
}
int SDL_Application::RenderFrame(){
    void* pixels;
    int pitch;

    SDL_LockTexture(frameTexture, nullptr, &pixels, &pitch);

    uint32_t* dst = static_cast<uint32_t*>(pixels);
    samplerPtr->setTextureMemory(dst, pitch);
    samplerPtr->UpdateBuffer();

    SDL_UnlockTexture(frameTexture);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, frameTexture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
    return 0;
}
int SDL_Application::Run(std::function<void()> UpdateBlock){
    //check sampler&texture buffer
    if (!samplerPtr){
        std::cerr << "No sampler has been loaded!" << std::endl;
        return -1;
    }
    if (!frameTexture) {
        std::cerr << "No frameTexture has been initialized!" << std::endl;
        SDL_Log("frameTexture is null!");
        return -1;
    }
    //prepare for tick count
    bool isrunning = true;
    Uint64 lastTime = SDL_GetTicks64();
    const Uint64 frameDelay = 1000 / fps;//1000ms除以fps
    double lag = 0.0;
    //固定30hz的updateblock，渲染越快越好
    while (isrunning)
    {
        //fps match
        Uint64 currentTime = SDL_GetTicks64();
        Uint64 elapsed = currentTime - lastTime;
        lastTime = currentTime;
        lag += elapsed;
        if(HandleEvent() == 1){//handle event
            isrunning = false;
            std::cout << "Terminated by the user." << std::endl;
            break;
        }

        while (lag >= frameDelay){
            tickCount++;
            if (enableCameraMotion){
                UpdateCameraMotion(static_cast<float>(elapsed) / 1000.0f);
            }
            UpdateBlock();
            lag -= frameDelay;
        }
        //render
        int renderStatus = RenderFrame();
        //render status
        if (renderStatus == -1){
            isrunning = false;
            std::cerr << "No sampler has been loaded!" << std::endl;
        }else if(renderStatus == 0){//do nothing
        }
    }
    tickCount = 0;
    runtimes++;
    return 0;
}

void SDL_Application::UpdateCameraMotion(float deltaSeconds){
    auto cameraPtr = samplerPtr.get()->getCameraPtr();
    if (!cameraPtr){//未绑定相机则跳过
        return;
    }
    const float speed = 0.5f * deltaSeconds;//平移速度：5单位/秒，乘deltaTime保证帧率无关
    if (isKeyDown(SDL_SCANCODE_W))     { cameraPtr->moveForward(speed); }//前
    if (isKeyDown(SDL_SCANCODE_S))     { cameraPtr->moveForward(-speed); }//后
    if (isKeyDown(SDL_SCANCODE_A))     { cameraPtr->moveRight(-speed); }//左
    if (isKeyDown(SDL_SCANCODE_D))     { cameraPtr->moveRight(speed); }//右
    if (isKeyDown(SDL_SCANCODE_SPACE)) { cameraPtr->moveUp(speed); }//上（空格）
    if (isKeyDown(SDL_SCANCODE_LCTRL) || isKeyDown(SDL_SCANCODE_RCTRL)) { cameraPtr->moveUp(-speed); }//下（Ctrl）
    //视角旋转：仅在鼠标捕捉（相对鼠标模式）下生效
    if (mouseCaptured){
        Vec2 m = getMousedelta();
        //符号按你Camera的约定：rotateYaw正=右转、rotatePitch正=抬头
        cameraPtr->rotateYaw(-m.getX() * 0.003f);   //鼠标右移(dx>0)→右转头
        cameraPtr->rotatePitch(-m.getY() * 0.003f);//鼠标上移(dy<0)→抬头
    }
}
    
}