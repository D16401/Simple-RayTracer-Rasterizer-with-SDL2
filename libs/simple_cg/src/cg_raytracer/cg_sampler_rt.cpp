#include "cg_sampler_rt.h"

#include "cg_sampler.h"
#include "cg_rt_interaction.h"

void cg::RayTracingSampler::UpdateBuffer(){//根据光追算法更新buffer中的像素数据
    for (int j = 0; j < height; j++){
        for(int i = 0; i < width; i++){
            Vec2 canvasP = Vec2(float(i), float(j));
            Vec3 viewportP = CanvasToViewport(*cameraPtr, canvasP);//将二维画布坐标转换到世界视口的三维坐标
            uint32_t color = SimpleRayTracing(*this, *scenePtr, viewportP);//光线追踪计算世界视口的三维坐标处看到的颜色
            writeBufferPixel(i, j, color);//直接写入texture内存，供api根据内存中的数据渲染画面
        }
    }
}
