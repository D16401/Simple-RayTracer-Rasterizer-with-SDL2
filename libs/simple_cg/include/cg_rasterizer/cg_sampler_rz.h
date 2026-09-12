#pragma once

#include "cg_math.h"
#include "cg_sampler.h"


namespace cg{

struct RasterizerBuild : SamplerBuild{
    int a;
};
class RasterizingSampler : public cg::Sampler{
public:
    RasterizingSampler(): Sampler(0, 0){}
    RasterizingSampler(int w, int h): Sampler(w, h){}
    RasterizingSampler(RasterizerBuild build): Sampler(build){}
    ~RasterizingSampler() override = default;
    void UpdateBuffer();
};

};

