#pragma once

#include <memory>

#include "cg_ld_parser.h"
#include "sdl_wrapper.h"
#include "cg_sampler.h"
#include "cg_world.h"
#include "cg_raytracer.h"
#include "cg_rasterizer.h"

using cg::SDL_Application;
using cg::SDLappBuild;

namespace loader{

class WorldLoader{
public:
    //Loader顶层入口
    //可用字段: "SDLapp", "sampler", "camera", "scene"
    bool LoadSettings(const JsonValue& settingJson, std::shared_ptr<SDL_Application>& appPtr);

    //Load SDLapp入口
    //可用字段: "title", "canvasH", "canvasW", "fps", "enableCameraMotion", "mouseCaptured"
    bool LoadSDLapp(const JsonValue& SDLappJson, std::shared_ptr<SDL_Application>& appPtr);

    //Load Sampler入口
    //可用公共字段: "type", "width", "height", "scene", "camera"
    //Raytracer专有字段: "samplingmode", "reflectionDepth"
    //Rasterizer专有字段:
    bool LoadSampler(const JsonValue& samplerJson, std::shared_ptr<cg::Sampler>& samplerPtr);
    //可用公共字段: "type", "scene", "camera"
    bool LoadSamplerBuild(const JsonValue& samplerJson, cg::SamplerBuild& samplerbuild, JsonValue& rest);
    //Raytracer专有字段: "samplingmode", "reflectionDepth"
    bool LoadRayTracer(const JsonValue& samplerJson, std::shared_ptr<cg::Sampler>& samplerPtr);
    //Rasterizer专有字段:
    bool LoadRasterizer(const JsonValue& samplerJson, std::shared_ptr<cg::Sampler>& samplerPtr);
    
    //Load Scene入口
    //可用字段: "background", "ambient", "objects", "lights"
    bool LoadScene(const JsonValue& sceneJson, std::shared_ptr<cg::Scene>& scenePtr);
    
    //Load Camera入口
    //可用字段: "position", "forward", "right", "up", "depth", "viewportW", "viewportH", "canvasW", "canvasH"
    bool LoadCamera(const JsonValue& cameraJson, std::shared_ptr<cg::Camera>& cameraPtr);

    //objectArr
    bool LoadObjectArr(const JsonValue& objectArrJson, std::vector<std::unique_ptr<cg::Object>>& ObjectPtrs);
    //Load Object入口
    //可用公共字段: "type", "name", "position", "material", "rtAttr", "rzAttr"
    //material字段: "color", "specular"
    //RtArr字段: "reflectivity"
    //RzArr字段: "segments", "stacks", "size"
    //Sphere专有字段: "radius"
    //Plane专有字段: "normal"
    bool LoadObject(const JsonValue& objectJson, std::unique_ptr<cg::Object>& objPtr);
    //可用公共字段: "type", "name", "position", "material", "rtAttr", "rzAttr"
    bool LoadObjectBuild(const JsonValue& objectJson, cg::ObjectBuild& build, JsonValue& rest);
    //可用字段: "color", "specular"
    bool LoadMaterial(const JsonValue& materialJson, cg::Material& material);
    //RtArr字段: "reflectivity"
    bool LoadRtAttr(const JsonValue& RtAttrJson, cg::RaytracingAttribute& rtAttr);
    //RzArr字段: "segments", "stacks", "size"
    bool LoadRzAttr(const JsonValue& RzAttrJson, cg::RasterizingAttribute& rzAttr);
    //Sphere专有字段: "radius"
    bool LoadSphere(const JsonValue& sphereJson, std::unique_ptr<cg::Object>& objPtr);
    //Plane专有字段: "normal"
    bool LoadPlane(const JsonValue& planeJson, std::unique_ptr<cg::Object>& objPtr);
    //lightArr
    bool LoadLightArr(const JsonValue& lightArrJson, std::vector<std::unique_ptr<cg::Light>>& LightPtrs);
    
    //Load Light入口
    //可用公共字段: "type", "name", "intensity"
    //PointLight专有字段: "position"
    //DirectionalLight: "direction"
    bool LoadLight(const JsonValue& lightJson, std::unique_ptr<cg::Light>& lightPtr);
    bool LoadLightBuild(const JsonValue& lightJson, cg::LightBuild& lightbuild, JsonValue& rest);
    //PointLight专有字段: "position"
    bool LoadPointLight(const JsonValue& plightJson, std::unique_ptr<cg::Light>& lightPtr);
    //DirectionalLight: "direction"
    bool LoadDirectionalLight(const JsonValue& dlightJson, std::unique_ptr<cg::Light>& lightPtr);
    
    std::string GetErr() const { return err; }
private:
    //vaild check
    bool minimumRunCheck(const std::shared_ptr<SDL_Application>& appPtr);
    bool minimumRunCheck(const std::shared_ptr<cg::Sampler>& samplerPtr);
    inline bool checkSDLappBuild(const SDLappBuild& build);
    inline bool checkSamplerBuild(const cg::SamplerBuild& build);
    bool checkSceneBuild(const cg::SceneBuild& build){return true;}//可以全部属性都使用默认值
    inline bool checkCameraBuild(const cg::CameraBuild& build);
    //get attribute
    bool GetBool(const JsonValue& boolJson, bool& boolean);
    bool GetColor(const JsonValue& colorJson, uint32_t& color);
    bool GetNumber(const JsonValue& numberJson, double& number);
    bool GetVec3(const JsonValue& Vec3Json, cg::Vec3& vec3);
    bool GetSamplerType(const JsonValue& samplertypeJson, cg::SamplerType& type);
    bool GetSamplingMode(const JsonValue& samplingmodeJson, cg::SamplingMode& mode);
    bool GetObjectType(const JsonValue& objtypeJson, cg::ObjectType& type);
    bool GetLightType(const JsonValue& lighttypeJson, cg::LightType& type);

    void fail(const std::string& msg);
    std::string err;
};

bool strTocolor(const std::string& str, uint32_t& color);
bool compareLowerLiteral(const std::string& test, const std::string& literal);

};


