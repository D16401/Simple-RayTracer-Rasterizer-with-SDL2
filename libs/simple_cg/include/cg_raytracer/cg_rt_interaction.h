#pragma once

#include "cg_sampler_rt.h"
#include "cg_world.h"

namespace cg{

Vec3 CanvasToViewport(const Camera& camera, const Vec2& canvasP);//坐标转换，画布到视口

bool IntersectTest(const Object& obj, const Ray& ray, float& return_distance);
bool IntersectTest(const Sphere& sphere, const Ray& ray, float& return_distance);
bool IntersectTest(const Plane& plane, const Ray& ray, float& return_distance);

bool OcclusionTest(const Light& light, const Scene& scene, const Vec3& HitPoint);
bool OcclusionTest(const PointLight& light, const Scene& scene, const Vec3& HitPoint);
bool OcclusionTest(const DirectionalLight& light, const Scene& scene, const Vec3& HitPoint);

size_t FindClosestIntersection(const Scene& scene, const Ray& intersectRay, float& closest_distance);
float DiffuseFactor(const Vec3& HitPointNormal, const Vec3& HitInDirection);
float SpecularFactor(int specular, const Vec3& HitPointNormal, const Vec3& HitInDirection, const Vec3& ViewDirection);
float ComputeLighting(const Ray& ray, const Scene& scene, const Vec3& HitPoint,
                      const Vec3& HitPointNormal, int SurfaceSpecular, bool enableOcclusionTest);

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
            float reflectivity = scene.getObjectPtrs()[closest_objPtr_index]->getRtAttr().reflectivity;
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
};

template <SamplingMode Mode>
struct SamplingModeTraits;
template <>
struct SamplingModeTraits<SamplingMode::DirectLighting>{
    static constexpr bool enableOcclusionTrait = false;
    static int getReflectionDepth(const RayTracingSampler& RTsampler){
        return 1;
    }
};
template <>
struct SamplingModeTraits<SamplingMode::HardShadows>{
    static constexpr bool enableOcclusionTrait = true;
    static int getReflectionDepth(const RayTracingSampler& RTsampler){
        return 1;
    }
};
template <>
struct SamplingModeTraits<SamplingMode::RecursiveReflection>{
    static constexpr bool enableOcclusionTrait = true;
    static int getReflectionDepth(const RayTracingSampler& RTsampler){
        return RTsampler.getReflectionDepth();
    }
};
template <SamplingMode Mode>
uint32_t SimpleRayTracingTemplate(const RayTracingSampler& RTsampler, const Scene& scene, const Vec3& viewportP);

uint32_t SimpleRayTracing(const RayTracingSampler& RTsampler, const Scene& scene, const Vec3& viewportP);
    
}
