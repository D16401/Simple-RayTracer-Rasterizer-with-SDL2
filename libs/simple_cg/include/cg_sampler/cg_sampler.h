#pragma once

#include <vector>
#include <cstdint>

class Sampler{
public:
    Sampler() = default;
    Sampler(int w, int h): width(w), height(h){}
    virtual ~Sampler() = default;
    virtual void GetOutputSize(int& w, int& h)const {w = width; h = height;};
    virtual void UpdateBuffer(uint32_t* dst, int pitch) = 0;
protected:
    int width = 0;
    int height = 0;
};

