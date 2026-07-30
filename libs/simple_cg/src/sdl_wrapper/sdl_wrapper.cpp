#include "sdl_wrapper.h"

#include <iostream>
#include <cstdint>
#include <functional>
#include <memory>
#include <cstring>
#include <SDL2/SDL.h>

#include "cg_math.h"

SDL_Application::SDL_Application(const char* Title){
    title = Title;
}
SDL_Application::~SDL_Application() {
    if (renderer) { SDL_DestroyRenderer(renderer); }
    if (window)   { SDL_DestroyWindow(window); }
    SDL_Quit();
}
int SDL_Application::loadSampler(std::shared_ptr<Sampler> sampler_ptr){
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
        title,
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
        SDL_SetRenderDrawColor(renderer, randint(1, 255), randint(1, 255), randint(1, 255), 255);//设置颜色
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
int SDL_Application::HandleEvent(){
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT){
            return 1;
        }else if(event.type == SDL_KEYDOWN){
            switch (event.key.keysym.sym)
            {
                case SDLK_ESCAPE:
                    return 1;
            }
        }
    }
    return 0;
}
int SDL_Application::RenderFrame(){
    void* pixels;
    int pitch;

    SDL_LockTexture(frameTexture, nullptr, &pixels, &pitch);

    uint32_t* dst = static_cast<uint32_t*>(pixels);
    samplerPtr->UpdateBuffer(dst, pitch / sizeof(uint32_t));

    SDL_UnlockTexture(frameTexture);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, frameTexture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
    return 0;
}
int SDL_Application::Run(std::function<void()> UpdateBlock){
    //check 
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
    const Uint64 frameDelay = 1000 / fps;
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


