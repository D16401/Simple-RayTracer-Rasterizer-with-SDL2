#include "cg_rt_interaction.h"

#include <vector>
#include <memory>
#include <iostream>
#include <stdexcept>

#include "cg_math.h"

Vec3 CanvasToViewport(const Camera& camera, const Vec2& canvasP){
    float v_w = ((canvasP.getX() + 0.5f) / camera.getCanvasW() - 0.5f) * camera.getViewportW();
    float v_h = -((canvasP.getY() + 0.5f) / camera.getCanvasH() - 0.5f) * camera.getViewportH();
    Vec3 ralativeBias = (camera.getForward() * camera.getDepth()) + (camera.getUp() * v_h) + (camera.getRight() *  v_w);
    return camera.getPosition() + ralativeBias;
};

bool Sphere::intersectTest(const Ray& ray, float& return_distance) const {
    const Vec3& D = ray.getDirection();
    const Vec3& O = ray.getOrigin();
    const Vec3& C = this->getPosition();
    float r = radius;
    float a = D.dot(D);
    float b = 2 * D.dot(O - C);
    float c = (O-C).dot(O-C) - r*r;
    float x1,x2;
    int n_root = solveQuadratic(a, b, c, x1, x2);
    if (n_root != 0){
        if (x1 > 0){
            return_distance = x1;
            return true;
        }else if (x2 > 0){
            return_distance = x2;
            return true;
        }else{
            return false;
        }
    }else{
        return false;
    }
};
bool Plane::intersectTest(const Ray& ray, float& return_distance)const {
    const Vec3& D = ray.getDirection();
    const Vec3& N = normal;
    float D_dot_N = D.dot(N);
    if (abs(D_dot_N) < CGMath_EPS){
        return_distance = CGMATH_INF;
        return false;
    }else{
        Vec3 OP = this->getPosition() - ray.getOrigin();
        //h = abs((OP).dot(N) / N.length()) ;
        return_distance = (OP).dot(N) / D_dot_N;
        if (return_distance > 0.f){
            return true;
        }else{
            return false;
        }

    }   
};

bool PointLight::OcclusionTest(const Scene& scene, const Vec3& HitPoint) const {
    Vec3 L = this->getHitInDirection(HitPoint)*-1.0f;
    Vec3 O = HitPoint + L * CGMath_EPS * 40;//偏移量足够大才不会自交
    Ray ray(O, L);
    float closest_distance = CGMATH_INF; 
    size_t closest_objPtr_index = FindClosestIntersection(scene, ray, closest_distance);
    if (closest_objPtr_index == static_cast<size_t>(-1)){
        return false;
    }else{
        if (closest_distance > (HitPoint - position).length()){
            return false;
        }else{
            return true;
        }
    }
};
bool DirectionalLight::OcclusionTest(const Scene& scene, const Vec3& HitPoint) const {
    Vec3 L = this->getHitInDirection(HitPoint)*-1.0f;
    Vec3 O = HitPoint + L * CGMath_EPS * 40;
    Ray ray(O, L);
    float closest_distance = CGMATH_INF;
    size_t closest_objPtr_index = FindClosestIntersection(scene, ray, closest_distance);
    if (closest_objPtr_index == static_cast<size_t>(-1)){
        return false;
    }else{
        return true;
    }
};

size_t FindClosestIntersection(const Scene& scene, const Ray& intersectRay, float& closest_distance){
    size_t closest_objPtr_index = static_cast<size_t>(-1);//哨兵值
    size_t n_object = scene.getObjectPtrs().size();
    for (size_t objPtr_idx = 0; objPtr_idx < n_object; objPtr_idx++){
        float distance;
        if (scene.getObjectPtrs()[objPtr_idx]->intersectTest(intersectRay, distance)){
            if (distance < closest_distance){
                closest_distance = distance;
                closest_objPtr_index = objPtr_idx;
            }
        }
    }
    return closest_objPtr_index;
};

float DiffuseFactor(const Vec3& HitPointNormal, const Vec3& HitInDirection){
    float N_dot_L = HitPointNormal.dot(HitInDirection*-1.0f);
    if( N_dot_L > 0){
        float Factor = N_dot_L / (1  * HitInDirection.length());//HitPointNormal.length() = 1
        return Factor;
    }else{
        return 0;
    }
};
float SpecularFactor(int specular, const Vec3& HitPointNormal, const Vec3& HitInDirection, const Vec3& ViewDirection){
    if (specular <= 0){
        return 0;
    }else{
        float N_dot_L = HitPointNormal.dot(HitInDirection*-1.0f);
        Vec3 ReflectOutRay = HitPointNormal* 2 * N_dot_L + HitInDirection;
        float R_dot_V = ReflectOutRay.dot(ViewDirection); 
        if (R_dot_V > 0){
            float Factor = static_cast<float>(pow(R_dot_V / ReflectOutRay.length(), specular));//(R_dot_V / (ReflectOutRay.length() * ViewDirection.length())) = R_dot_V / (ReflectOutRay.length() / 1
            return Factor;
        }else{
            return 0;
        }
    }
};
float ComputeLighting(const Ray& ray, const Scene& scene, const Vec3& HitPoint, const Vec3& HitPointNormal, int SurfaceSpecular, bool enableOcclusionTest){
    bool OcclusionTest = enableOcclusionTest;
    float total_intensity = 0;
    Vec3 viewDirection = ray.getDirection() * -1;
    size_t n_Lights = scene.getLightPtrs().size();
    for (size_t idx = 0; idx < n_Lights; idx++){
        if (OcclusionTest && scene.getLightPtrs()[idx]->OcclusionTest(scene, HitPoint)){
            continue;
        }
        Vec3 HitInDirection = scene.getLightPtrs()[idx]->getHitInDirection(HitPoint);
        float IntensityFactor = DiffuseFactor(HitPointNormal, HitInDirection) + SpecularFactor(SurfaceSpecular, HitPointNormal, HitInDirection, viewDirection);
        total_intensity = total_intensity + scene.getLightPtrs()[idx]->getIntensity() * IntensityFactor;
    }
    return total_intensity;
};
template <CameraMode Mode>
uint32_t SimpleRayTracingTemplate(const Camera& camera, const Scene& scene, const Vec3& viewportP){
    Vec3 cameraPos = camera.getPosition();
    Ray ray(cameraPos, viewportP - cameraPos);
    size_t closest_objPtr_index = static_cast<size_t>(-1);
    float closest_distance = CGMATH_INF;
    constexpr bool enableOcclusion = CameraModeTraits<Mode>::enableOcclusionTrait;
    int reflectionDepth = CameraModeTraits<Mode>::getReflectionDepth(camera);
    auto shadingFunc = [&scene, &closest_objPtr_index, &closest_distance, enableOcclusion]
    (uint32_t local_color, const Ray& currentRay){
        float total_intensity = 0;
        if (closest_objPtr_index != static_cast<size_t>(-1)){
            Vec3 Hitpoint = currentRay.getOrigin() + currentRay.getDirection() * closest_distance;
            Vec3 HitpointNormal = scene.getObjectPtrs()[closest_objPtr_index]->getNormal(Hitpoint).normalize();
            int specular = scene.getObjectPtrs()[closest_objPtr_index]->getSpecular();
            total_intensity = ComputeLighting(currentRay, scene, Hitpoint, HitpointNormal, specular, enableOcclusion);
        }
        return colorScale(local_color, (scene.getAmbientLight() + total_intensity));
    };
    uint32_t local_color = TraceRay(ray, scene, reflectionDepth, closest_objPtr_index, closest_distance, shadingFunc);
    return local_color;
};
template <>
inline uint32_t SimpleRayTracingTemplate<CameraMode::VisibilityOnly>
(const Camera& camera, const Scene& scene, const Vec3& viewportP){
    Vec3 cameraPos = camera.getPosition();
    Ray ray(cameraPos, viewportP - cameraPos);
    size_t closest_objPtr_index = static_cast<size_t>(-1);
    float closest_distance = CGMATH_INF;
    auto VisibilityOnly = [](uint32_t local_color, const Ray&) {
        return local_color;
    };
    return TraceRay(ray, scene, 1, closest_objPtr_index, closest_distance, VisibilityOnly);
};


uint32_t SimpleRayTracing(const Camera& camera, const Scene& scene, const Vec3& viewportP) {
    switch (camera.getCameraMode()) {
        case CameraMode::VisibilityOnly:
            return SimpleRayTracingTemplate<CameraMode::VisibilityOnly>(camera, scene, viewportP);
        case CameraMode::DirectLighting:
            return SimpleRayTracingTemplate<CameraMode::DirectLighting>(camera, scene, viewportP);
        case CameraMode::HardShadows:
            return SimpleRayTracingTemplate<CameraMode::HardShadows>(camera, scene, viewportP);
        case CameraMode::RecursiveReflection:
            return SimpleRayTracingTemplate<CameraMode::RecursiveReflection>(camera, scene, viewportP);
        default:
            throw std::runtime_error("Unhandled CameraMode!");
            return scene.getBackgroundColor();
    }
};
template uint32_t SimpleRayTracingTemplate<CameraMode::DirectLighting>(const Camera& camera, const Scene& scene, const Vec3& viewportP);
template uint32_t SimpleRayTracingTemplate<CameraMode::HardShadows>(const Camera& camera, const Scene& scene, const Vec3& viewportP);
template uint32_t SimpleRayTracingTemplate<CameraMode::RecursiveReflection>(const Camera& camera, const Scene& scene, const Vec3& viewportP);