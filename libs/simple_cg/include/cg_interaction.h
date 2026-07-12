#pragma once

#include "cg_structure.h"

Vec3 CanvasToViewport(const Camera& camera, const Vec2& canvasP);//坐标转换，画布到视口

size_t FindClosestIntersection(const Scene& scene, const Ray& intersectRay, float& closest_distance);
float DiffuseFactor(const Vec3& HitPointNormal, const Vec3& HitInDirection);
float SpecularFactor(int specular, const Vec3& HitPointNormal, const Vec3& HitInDirection, const Vec3& ViewDirection);
float ComputeLighting(const Ray& ray, const Scene& scene, const Vec3& HitPoint,
                      const Vec3& HitPointNormal, int SurfaceSpecular, bool enableOcclusionTest);
uint32_t SimpleRayTracing(const Camera& camera, const Scene& scene, const Vec3& viewportP);
template<typename ShadingFunc>
uint32_t TraceRay(const Ray& ray, const Scene& scene, int ReflectiveDepth,
                  size_t& closest_objPtr_index, float& closest_distance,
                  const ShadingFunc& Shading){
    closest_objPtr_index = FindClosestIntersection(scene, ray, closest_distance);
    uint32_t local_color = scene.getBackgroundColor();
    if (closest_objPtr_index != static_cast<size_t>(-1)){
        local_color = scene.getObjectPtrs()[closest_objPtr_index]->getColor();
        uint32_t color = Shading(local_color, ray);
        ReflectiveDepth--;
        if (ReflectiveDepth <= 0){
            return color;
        }else{
            Vec3 HitPoint = ray.getOrigin() + ray.getDirection() * closest_distance;
            Vec3 HitInDirection = ray.getDirection();
            Vec3 Normal = scene.getObjectPtrs()[closest_objPtr_index]->getNormal(HitPoint).normalize();
            Vec3 ReflectDirection = HitInDirection - Normal * HitInDirection.dot(Normal) * 2;
            float reflectivity = scene.getObjectPtrs()[closest_objPtr_index]->getReflectivity();
            Ray NewRay(HitPoint + ReflectDirection * CGMath_EPS * 40, ReflectDirection);
            closest_objPtr_index = static_cast<size_t>(-1);
            closest_distance = CGMATH_INF;
            uint32_t reflected_color = TraceRay(NewRay, scene, ReflectiveDepth, closest_objPtr_index, closest_distance, Shading);
            color = blendByReflectivity(color, reflected_color, reflectivity);
            return color;
        }
    }else{
        return local_color;//无物体命中，返回背景色
    }
}

template<typename ShadingFunc>
uint32_t Shading();


inline std::uint32_t blendByReflectivity(std::uint32_t c1, std::uint32_t c2, float reflectivity){
    reflectivity = std::clamp(reflectivity, 0.0f, 1.0f);
    float ir = 1.0f - reflectivity;
    // RRGGBBAA
    std::uint32_t r = static_cast<std::uint32_t>(
        ((c1 >> 24) & 0xFF) * ir +
        ((c2 >> 24)  & 0xFF) * reflectivity + 0.5f);
    std::uint32_t g = static_cast<std::uint32_t>(
        ((c1 >> 16) & 0xFF) * ir +
        ((c2 >> 16) & 0xFF) * reflectivity + 0.5f);
    std::uint32_t b = static_cast<std::uint32_t>(
        ((c1 >> 8)  & 0xFF) * ir +
        ((c2 >> 8)  & 0xFF) * reflectivity + 0.5f);
    std::uint32_t a = static_cast<std::uint32_t>(
        ((c1 >> 0)  & 0xFF) * ir +
        ((c2 >> 0)  & 0xFF) * reflectivity + 0.5f);
    return (r << 24) | (g << 16) | (b << 8) | (a << 0);
}
inline std::uint32_t colorScale(std::uint32_t color, float weight)
{
    weight = std::clamp(weight, 0.0f, 1.0f);

    std::uint32_t r = static_cast<std::uint32_t>(
        ((color >> 24) & 0xFF) * weight + 0.5f);
    std::uint32_t g = static_cast<std::uint32_t>(
        ((color >> 16) & 0xFF) * weight + 0.5f);
    std::uint32_t b = static_cast<std::uint32_t>(
        ((color >> 8)  & 0xFF) * weight + 0.5f);
    std::uint32_t a = static_cast<std::uint32_t>(
        ((color >> 0)  & 0xFF) * weight + 0.5f);

    return (r << 24) | (g << 16) | (b << 8) | a;
}
