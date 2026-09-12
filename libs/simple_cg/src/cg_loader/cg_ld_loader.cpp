#include "cg_ld_loader.h"

#include <cctype>
#include <algorithm>

using cg::Vec3;
using loader::WorldLoader;

void WorldLoader::fail(const std::string& msg){
    err = err + "\n" + msg;
}
bool WorldLoader::LoadSettings(const JsonValue& settingJson, std::shared_ptr<SDL_Application>& appPtr){
    if (!settingJson.isObject()){
        fail("LoadSettings: Mismatch JsonValue type for LoadSettings, expected type: Object");
        return false;
    }
    auto pairs = settingJson.asObject();
    std::shared_ptr<cg::Sampler> samplerPtr;
    std::shared_ptr<cg::Scene> scenePtr;
    std::shared_ptr<cg::Camera> cameraPtr;
    for (size_t i = 0; i < pairs.size(); i++){
        std::string& key = pairs[i].first;
        JsonValue& value = pairs[i].second;
        if (compareLowerLiteral(key, "SDLapp")){
            if (!LoadSDLapp(value, appPtr)){
                fail("LoadSettings: Fail to load SDLapp");
                return false;
            }
        }else if (compareLowerLiteral(key, "sampler")){
            if (!LoadSampler(value, samplerPtr)){
                fail("LoadSettings: Fail to load sampler");
                return false;
            }
        }else if (compareLowerLiteral(key, "camera")){
            if (!LoadCamera(value, cameraPtr)){
                fail("LoadSettings:Fail to load Camera(cover)");
                return false;
            }
        }else if (compareLowerLiteral(key, "scene")){
            if (!LoadScene(value, scenePtr)){
                fail("LoadSettings: Fail to load Scene(cover)");
                return false;
            }
        }else{
            fail("LoadSettings: Unknown Settings attribute key: " + key);
            return false;
        }
    }
    if (!samplerPtr.get()){
        fail("LoadSettings: Missing sampler key");
        return false;
    }
    if (cameraPtr.get()){
        //若setting中有指定相机Camera，则覆盖sampler配置json中指定的相机Camera
        //否则保持sampler配置json中指定的相机Camera
        samplerPtr->loadCamera(std::move(cameraPtr));
    }
    if (scenePtr.get()){
        //若setting中有指定场景Scene，则覆盖sampler配置json中指定的场景Scene
        //否则保持sampler配置json中指定的场景Scene
        samplerPtr->loadScene(std::move(scenePtr));
    }
    if (!minimumRunCheck(samplerPtr)){
        fail("LoadSettings: Incomplete sampler");
        return false;
    }
    if (!appPtr.get()){
        fail("LoadSettings: Missing SDLapp key");
        return false;
    }
    appPtr->loadSampler(std::move(samplerPtr));
    if (!minimumRunCheck(appPtr)){
        fail("LoadSettings: Incomplete SDLapp");
        return false;
    }
    return true;
}
bool WorldLoader::LoadSDLapp(const JsonValue& SDLappJson, std::shared_ptr<SDL_Application>& appPtr){
    if (!SDLappJson.isObject()){
        fail("LoadSDLapp: Mismatch JsonValue type for LoadWorld, expected type: Object");
        return false;
    }
    auto pairs = SDLappJson.asObject();
    SDLappBuild SDLappbuild;
    for (size_t i = 0; i < pairs.size(); i++){
        std::string& key = pairs[i].first;
        JsonValue& value = pairs[i].second;
        if (compareLowerLiteral(key, "title")){
            if (!value.isString()){
                fail("LoadSDLapp: Fail to load window title, expected type: String");
                return false;
            }
            SDLappbuild.title = value.asString().c_str();
        }else if (compareLowerLiteral(key, "canvasH")){
            double canvasH;
            if (!GetNumber(value, canvasH)){
                fail("LoadSDLapp: Fail to load canvas height");
                return false;
            }
            SDLappbuild.canvasH = static_cast<int>(canvasH);
        }else if (compareLowerLiteral(key, "canvasW")){
            double canvasW;
            if (!GetNumber(value, canvasW)){
                fail("LoadSDLapp: Fail to load canvas width");
                return false;
            }
            SDLappbuild.canvasW = static_cast<int>(canvasW);
        }else if (compareLowerLiteral(key, "fps")){
            double fps;
            if (!GetNumber(value, fps)){
                fail("LoadSDLapp: Fail to load fps(render frame per second)");
                return false;
            }
            SDLappbuild.fps = static_cast<int>(fps);
        }else if (compareLowerLiteral(key, "enableCameraMotion")){
            if (!GetBool(value, SDLappbuild.enableCameraMotion)){
                fail("LoadSDLapp: Fail to load SDLapp option \"enableCameraMotion\"");
                return false;
            }
        }else if (compareLowerLiteral(key, "mouseCaptured")){
            if (!GetBool(value, SDLappbuild.mouseCaptured)){
                fail("Fail to load SDLapp otption \"mouseCaptured\"");
                return false;
            }
        }else{
            fail("Unknown SDLapp attribute key: " + key);
            return false;
        }

        
        
    }
    if (!checkSDLappBuild(SDLappbuild)){
        fail("Fail to load SDLapp: Invaild attribute list");
        return false;
    }
    appPtr = std::make_shared<SDL_Application>(SDLappbuild);
    return true;
}

bool WorldLoader::LoadSampler(const JsonValue& samplerJson, std::shared_ptr<cg::Sampler>& samplerPtr){
    if (!samplerJson.isObject()){
        fail("LoadSampler: Mismatch JsonValue type for LoadSampler, expected type: Object");
        return false;
    }
    const JsonValue* typeValue = samplerJson.find("type");
    if (!typeValue){
        fail("LoadSampler: Fail to load Sampler: missing SamplerType");
        return false;        
    }
    cg::SamplerType type;
    if (!GetSamplerType(*typeValue, type)){
        fail("LoadSampler: Fail to get SamplerType");
        return false;        
    }
    switch (type){
        case cg::SamplerType::Raytracer:
            if (!LoadRayTracer(samplerJson, samplerPtr)){
                fail("LoadSampler: Fail to load Raytracer");
                return false;
            }
            break;
        case cg::SamplerType::Rasterizer:
            if (!LoadRasterizer(samplerJson, samplerPtr)){
                fail("LoadSampler: Fail to load Rasterizer");
                return false;
            }
            break;
        default:
            fail("LoadSampler: Fail to load Sampler: unkonwn SamplerType:" + typeValue->asString());
            return false;            
    }
    return true;

}
bool WorldLoader::LoadSamplerBuild(const JsonValue& samplerJson, cg::SamplerBuild& samplerbuild, JsonValue& rest){
    if (!samplerJson.isObject()){
        fail("LoadSamplerBuild: Mismatch JsonValue type for LoadSamplerBuild, expected JsonValue type: Object");
        return false;
    }
    auto pairs = samplerJson.asObject();
    for (size_t i = 0; i < pairs.size(); i++){
        std::string& key = pairs[i].first;
        JsonValue& value = pairs[i].second;
        if (compareLowerLiteral(key, "type")){
            continue;
        }else if (compareLowerLiteral(key, "width")){
            double tmpWidth;
            if (!GetNumber(value, tmpWidth)){
                fail("LoadSamplerBuild: Fail to load scene");
                return false;
            }
            samplerbuild.width = static_cast<int>(tmpWidth);
        }else if (compareLowerLiteral(key, "height")){
            double tmpHeight;
            if (!GetNumber(value, tmpHeight)){
                fail("LoadSamplerBuild: Fail to load scene");
                return false;
            }
            samplerbuild.height = static_cast<int>(tmpHeight);
        }else if (compareLowerLiteral(key, "scene")){
            if (!LoadScene(value, samplerbuild.scenePtr)){
                fail("LoadSamplerBuild: Fail to load scene");
                return false;
            }
        }else if (compareLowerLiteral(key, "camera")){
            if (!LoadCamera(value, samplerbuild.cameraPtr)){
                fail("LoadSamplerBuild: Fail to load camera");
                return false;
            }
        }else{
            rest.addPair(key, value);
        }
    }
    return true;
}
bool WorldLoader::LoadRayTracer(const JsonValue& samplerJson, std::shared_ptr<cg::Sampler>& samplerPtr){
    if (!samplerJson.isObject()){
        fail("LoadRayTracer: Mismatch JsonValue type for LoadRayTracer, expected JsonValue type: Object");
        return false;
    }
    cg::RaytracerBuild raytracerbuild;
    raytracerbuild.type = cg::SamplerType::Raytracer;
    JsonValue rest = JsonValue(JsonValue::Type::Object);
    if (!LoadSamplerBuild(samplerJson, raytracerbuild, rest)){
        fail("LoadRayTracer: Fail to load SamplerBuild");
        return false;
    }
    auto pairs = rest.asObject();
    for (size_t i = 0; i < pairs.size(); i++){
        std::string key = pairs[i].first;
        JsonValue value = pairs[i].second;
        if (compareLowerLiteral(key, "samplingmode")){
            if (!GetSamplingMode(value, raytracerbuild.samplingMode)){
                fail("LoadRayTracer: Fail to load RayTracer SamplingMode");
                return false;
            }
        }else if (compareLowerLiteral(key,"reflectionDepth")){
            double tmpReflectionDepth;
            if (!GetNumber(value, tmpReflectionDepth)){
                fail("LoadRayTracer: Fail to load RayTracer reflectionDepth");
                return false;
            }
            raytracerbuild.reflectionDepth = static_cast<int>(tmpReflectionDepth);
        }else{
            fail("LoadRayTracer: Unknown RayTracer attribute key: " + key);
            return false;
        }
    }
    if (!checkSamplerBuild(raytracerbuild)){
        fail("LoadRayTracer: Fail to load Sampler, invaild attribute list");
        return false;
    }
    samplerPtr = std::make_shared<cg::RayTracingSampler>(raytracerbuild);
    return true;
}
bool WorldLoader::LoadRasterizer(const JsonValue& samplerJson, std::shared_ptr<cg::Sampler>& samplerPtr){
    if (!samplerJson.isObject()){
        fail("LoadRasterizer: Mismatch JsonValue type for LoadRasterizer, expected JsonValue type: Object");
        return false;
    }
    cg::RasterizerBuild rasterizerbuild;
    rasterizerbuild.type = cg::SamplerType::Rasterizer;
    JsonValue rest = JsonValue(JsonValue::Type::Object);
    if (!LoadSamplerBuild(samplerJson, rasterizerbuild, rest)){
        fail("LoadRasterizer: Fail to load SamplerBuild");
        return false;
    }
    auto pairs = rest.asObject();
    for (size_t i = 0; i < pairs.size(); i++){
        std::string key = pairs[i].first;
        JsonValue value = pairs[i].second;
        if (false){
            continue;
        }else{
            fail("LoadRasterizer: Unknown Rasterizer attribute key: " + key);
            return false;
        }
    }
    if (!checkSamplerBuild(rasterizerbuild)){
        fail("LoadRasterizer: Fail to load Sampler, invaild attribute list");
        return false;
    }
    samplerPtr = std::make_shared<cg::RasterizingSampler>(rasterizerbuild);
    return true;
}

bool WorldLoader::LoadScene(const JsonValue& worldJson, std::shared_ptr<cg::Scene>& scenePtr){
    if (!worldJson.isObject()){
        fail("LoadScene: Mismatch JsonValue type for LoadWorld, expected type: Object");
        return false;
    } 
    auto pairs = worldJson.asObject();
    cg::SceneBuild scenebuild;
    for (size_t i = 0; i < pairs.size(); i++){
        std::string& key = pairs[i].first;
        JsonValue& value = pairs[i].second;
        if (compareLowerLiteral(key, "background")){
            if (!GetColor(value, scenebuild.background)){
                fail("LoadScene: Fail to load scene background color");
                return false;
            }
        }else if (compareLowerLiteral(key, "ambient")){
            double number;
            if (!GetNumber(value, number)){
                fail("LoadScene: Fail to load scene ambient light intensity");
                return false;
            }
            scenebuild.ambient = std::clamp(static_cast<float>(value.asNumber()), 0.f, 1.f);
        }else if (compareLowerLiteral(key, "objects")){
            if (!LoadObjectArr(value, scenebuild.ObjectPtrs)){
                fail("LoadScene: Fail to load scene objects");
                return false;
            }
        }else if (compareLowerLiteral(key, "lights")){
            if (!LoadLightArr(value, scenebuild.LightPtrs)){
                fail("LoadScene: Fail to load scene lights");
                return false;
            }
        }else{
            fail("LoadScene: Unknown Scene attribute key: " + key);
            return false;
        }
    }
    if (!checkSceneBuild(scenebuild)){
        fail("LoadScene: Fail to load Scene: Invaild attribute list");
        return false;
    }
    scenePtr = std::make_shared<cg::Scene>(std::move(scenebuild));//build里面有uniquePtr，必须move
    return true;
}
bool WorldLoader::LoadCamera(const JsonValue& cameraJson, std::shared_ptr<cg::Camera>& cameraPtr){
    if (!cameraJson.isObject()){
        fail("LoadCamera: Mismatch JsonValue type for LoadCamera, expected type: Object");
        return false;
    }
    auto pairs = cameraJson.asObject();
    cg::CameraBuild build;
    for (size_t i = 0; i < pairs.size(); i++){
        std::string& key = pairs[i].first;
        JsonValue& value = pairs[i].second;
        if (compareLowerLiteral(key, "position")){
            if (!GetVec3(value, build.position)){
                fail("LoadCamera: Fail to load camera position");
                return false;
            }
        }else if (compareLowerLiteral(key, "forward")){
            if (!GetVec3(value, build.forward)){
                fail("LoadCamera: Fail to load camera forward");
                return false;
            }
        }else if (compareLowerLiteral(key, "right")){
            if (!GetVec3(value, build.right)){
                fail("LoadCamera: Fail to load camera right");
                return false;
            }
        }else if (compareLowerLiteral(key, "up")){
            if (!GetVec3(value, build.up)){
                fail("LoadCamera: Fail to load camera up");
                return false;
            }
        }else if(compareLowerLiteral(key, "depth")){
            double depth;
            if (!GetNumber(value, depth)){
                fail("LoadCamera: Fail to load camera depth");
                return false;
            }
            build.depth = static_cast<float>(depth);
        }else if(compareLowerLiteral(key, "viewportW")){
            double viewportW;
            if (!GetNumber(value, viewportW)){
                fail("LoadCamera: Fail to load camera viewportW");
                return false;
            }
            build.viewportW = static_cast<float>(viewportW);
        }else if(compareLowerLiteral(key, "viewportH")){
            double viewportH;
            if (!GetNumber(value, viewportH)){
                fail("LoadCamera: Fail to load camera viewportH");
                return false;
            }
            build.viewportH = static_cast<float>(viewportH);
        }else if(compareLowerLiteral(key, "canvasW")){
            double canvasW;
            if (!GetNumber(value, canvasW)){
                fail("LoadCamera: Fail to load camera canvasW");
                return false;
            }
            build.canvasW = static_cast<int>(canvasW);
        }else if(compareLowerLiteral(key, "canvasH")){
            double canvasH;
            if (!GetNumber(value, canvasH)){
                fail("LoadCamera: Fail to load camera canvasH");
                return false;
            }
            build.canvasH = static_cast<int>(canvasH);
        }else{
            fail("LoadCamera: Unknown Camera attribute key: " + key);
            return false;
        }
    }
    if (! checkCameraBuild(build)){
        fail("LoadCamera: Fail to load Camera: Invaild attribute list");
        return false;
    }
    cameraPtr = std::make_shared<cg::Camera>(build);
    return true;
}

bool WorldLoader::LoadObjectArr(const loader::JsonValue& objectArrJson, std::vector<std::unique_ptr<cg::Object>>& ObjectPtrs){
    if (!objectArrJson.isArray()){
        fail("LoadObjectArr: Mismatch JsonValue type for LoadObjectArr, expected type: Array");
        return false;
    }
    auto arr = objectArrJson.asArray();
    for (size_t i = 0; i < arr.size(); i++){
        std::unique_ptr<cg::Object> objPtr;
        if (!LoadObject(arr[i], objPtr) ){
            fail("LoadObjectArr: Fail to load Object");
            return false;
        }
        ObjectPtrs.push_back(std::move(objPtr));
    }
    return true;
}
bool WorldLoader::LoadObject(const loader::JsonValue& objectJson, std::unique_ptr<cg::Object>& objPtr){
    if (!objectJson.isObject()){
        fail("LoadObject: Mismatch JsonValue type for LoadObject, expected JsonValue type: Object");
        return false;
    }
    const JsonValue* typeValue = objectJson.find("type");
    if (!typeValue){
        fail("LoadObject: Fail to load Object: missing ObjectType");
        return false;
    }
    cg::ObjectType type;
    if (!GetObjectType(*typeValue, type)){
        fail("LoadObject: Fail to get ObjectType");
        return false;
    }
    switch (type){
        case cg::ObjectType::Sphere:
            if (!LoadSphere(objectJson, objPtr)){
                fail("LoadObject: Fail to load Sphere");
                return false;
            }
            break;
        case cg::ObjectType::Plane:
            if (!LoadPlane(objectJson, objPtr)){
                fail("LoadObject: Fail to load Plane");
                return false;
            }
            break;
        default:
            fail("LoadObject: Fail to load Object: unkonwn ObjectType:" + typeValue->asString());
            return false;
    }
    return true;
}
bool WorldLoader::LoadObjectBuild(const JsonValue& objectJson, cg::ObjectBuild& objBuild, JsonValue& rest){
    if (!objectJson.isObject()){
        fail("LoadObjectBuild: Mismatch JsonValue type for LoadObjectBuild, expected JsonValue type: Object");
        return false;
    }
    auto pairs = objectJson.asObject();
    for (size_t i = 0; i < pairs.size(); i++){
        std::string key = pairs[i].first;
        JsonValue value = pairs[i].second;
        if (compareLowerLiteral(key, "type")){
            continue;
        }else if (compareLowerLiteral(key, "name")){
            if (!value.isString()){
                fail("LoadObjectBuild: Fail to load Object name: expected JsonValue type: String");
                return false;
            }
            objBuild.name = value.asString();
        }else if (compareLowerLiteral(key, "position")){
            if (!GetVec3(value, objBuild.position)){
                fail("LoadObjectBuild: Fail to load Object position");
                return false;
            }
        }else if (compareLowerLiteral(key, "material")){
            if (!LoadMaterial(value, objBuild.material)){
                fail("LoadObjectBuild: Fail to load Object Meterial");
                return false;
            }
        }else if (compareLowerLiteral(key, "rtAttr")){
            if (!LoadRtAttr(value, objBuild.rtAttr)){
                fail("LoadObjectBuild: Fail to load Object RaytracingAttribute");
                return false;
            }
        }else if (compareLowerLiteral(key, "rzAttr")){
            if (!LoadRzAttr(value, objBuild.rzAttr)){
                fail("LoadObjectBuild: Fail to load Object RasterizingAttribute");
                return false;
            }
        }else{
            rest.addPair(key, value);
        }
    }
    return true;
}
bool WorldLoader::LoadMaterial(const JsonValue& materialJson, cg::Material& material){
    if (!materialJson.isObject()){
        fail("LoadMaterial: Mismatch JsonValue type for LoadMaterial, expected JsonValue type: Object");
        return false;
    }
    auto pairs = materialJson.asObject();
    for (size_t i = 0; i < pairs.size(); i++){
        std::string key = pairs[i].first;
        JsonValue value = pairs[i].second;
        if (compareLowerLiteral(key, "color")){
            if (!GetColor(value, material.color)){
                fail("LoadMaterial: Fail to load material color");
                return false;
            }
        }else if (compareLowerLiteral(key, "specular")){
            double tmpSpecular;
            if (!GetNumber(value, tmpSpecular)){
                fail("LoadMaterial: Fail to load material specular");
                return false;
            }
            material.specular = static_cast<int>(tmpSpecular);
        }else{
            fail("LoadMaterial: Unknown Material attribute key: " + key);
            return false;
        }
    }
    return true;
}
bool WorldLoader::LoadRtAttr(const JsonValue& RtAttrJson, cg::RaytracingAttribute& rtAttr){
    if (!RtAttrJson.isObject()){
        fail("LoadRtAttr: Mismatch JsonValue type for LoadRtAttr, expected JsonValue type: Object");
        return false;
    }
    auto pairs = RtAttrJson.asObject();
    for (size_t i = 0; i < pairs.size(); i++){
        std::string key = pairs[i].first;
        JsonValue value = pairs[i].second;
        if (compareLowerLiteral(key, "reflectivity")){
            double tmpReflectivity;
            if (!GetNumber(value, tmpReflectivity)){
                fail("LoadRtAttr: Fail to load RaytracingAttribute reflectivity");
                return false;
            }
            rtAttr.reflectivity = static_cast<float>(tmpReflectivity);
        }else{
            fail("LoadRtAttr: Unknown RaytracingAttribute attribute key: " + key);
            return false;
        }
    }
    return true;
}
bool WorldLoader::LoadRzAttr(const JsonValue& RzAttrJson, cg::RasterizingAttribute& rzAttr){
    if (!RzAttrJson.isObject()){
        fail("LoadRzAttr: Mismatch JsonValue type for LoadRzAttr, expected JsonValue type: Object");
        return false;
    }
    auto pairs = RzAttrJson.asObject();
    for (size_t i = 0; i < pairs.size(); i++){
        std::string key = pairs[i].first;
        JsonValue value = pairs[i].second;
        if (compareLowerLiteral(key, "segments")){
            double tmpSegments;
            if (!GetNumber(value, tmpSegments)){
                fail("LoadRzAttr: Fail to load RasterizingAttribute segments");
                return false;
            }
            rzAttr.segments = static_cast<int>(tmpSegments);
        }else if (compareLowerLiteral(key, "stacks")){
            double tmpStacks;
            if (!GetNumber(value, tmpStacks)){
                fail("LoadRzAttr: Fail to load RasterizingAttribute stacks");
                return false;
            }
            rzAttr.stacks = static_cast<int>(tmpStacks);
        }else if (compareLowerLiteral(key, "size")){
            double tmpSize;
            if (!GetNumber(value, tmpSize)){
                fail("LoadRzAttr: Fail to load RasterizingAttribute size");
                return false;
            }
            rzAttr.size = static_cast<float>(tmpSize);
        }else{
            fail("LoadRzAttr: Unknown RasterizingAttribute attribute key: " + key);
            return false;
        }
    }
    return true;
}
bool WorldLoader::LoadSphere(const JsonValue& sphereJson, std::unique_ptr<cg::Object>& objPtr){
    if (!sphereJson.isObject()){
        fail("LoadSphere: Mismatch JsonValue type for LoadSphere, expected JsonValue type: Object");
        return false;
    }
    cg::SphereBuild spherebuild;
    spherebuild.type = cg::ObjectType::Sphere;
    JsonValue rest = JsonValue(JsonValue::Type::Object);
    if (!LoadObjectBuild(sphereJson, spherebuild, rest)){
        fail("LoadSphere: Fail to load ObjectBuild");
        return false;
    }
    auto pairs = rest.asObject();
    for (size_t i = 0; i < pairs.size(); i++){
        std::string key = pairs[i].first;
        JsonValue value = pairs[i].second;
        if (compareLowerLiteral(key, "radius")){
            double tmpRadius;
            if (!GetNumber(value, tmpRadius)){
                fail("Fail to load Sphere radius");
                return false;
            }
            spherebuild.radius = static_cast<float>(tmpRadius);
        }else{
            fail("LoadSphere: Unknown Sphere attribute key: " + key);
            return false;
        }
    }
    objPtr = std::make_unique<cg::Sphere>(spherebuild);
    return true;
}
bool WorldLoader::LoadPlane(const JsonValue& planeJson, std::unique_ptr<cg::Object>& objPtr){
    if (!planeJson.isObject()){
        fail("LoadPlane: Mismatch JsonValue type for LoadPlane, expected JsonValue type: Object");
        return false;
    }
    cg::PlaneBuild planebuild;
    planebuild.type = cg::ObjectType::Plane;
    JsonValue rest = JsonValue(JsonValue::Type::Object);
    if (!LoadObjectBuild(planeJson, planebuild, rest)){
        fail("LoadPlane: Fail to load ObjectBuild");
        return false;
    }
    auto pairs = rest.asObject();
    for (size_t i = 0; i < pairs.size(); i++){
        std::string key = pairs[i].first;
        JsonValue value = pairs[i].second;
        if (compareLowerLiteral(key, "normal")){
            if (!GetVec3(value, planebuild.normal)){
                fail("LoadPlane: Fail to load Plane normal");
                return false;
            }
        }else{
            fail("LoadPlane: Unknown Plane attribute key: " + key);
            return false;
        }
    }
    objPtr = std::make_unique<cg::Plane>(planebuild);
    return true;
}
bool WorldLoader::LoadLightArr(const JsonValue& lightArrJson, std::vector<std::unique_ptr<cg::Light>>& LightPtrs){
    if (!lightArrJson.isArray()){
        fail("LoadLightArr: Mismatch JsonValue type for LoadLightArr, expected type: Array");
        return false;
    }
    auto arr = lightArrJson.asArray();
    for (size_t i = 0; i < arr.size(); i++){
        std::unique_ptr<cg::Light> lightPtr;
        if (!LoadLight(arr[i], lightPtr) ){
            fail("LoadLightArr: Fail to load Light");
            return false;
        }
        LightPtrs.push_back(std::move(lightPtr));
    }
    return true;
}
bool WorldLoader::LoadLight(const JsonValue& lightJson, std::unique_ptr<cg::Light>& lightPtr){
    if (!lightJson.isObject()){
        fail("LoadLight: Mismatch JsonValue type for LoadLight, expected type: Object");
        return false;
    }
    const JsonValue* typeValue = lightJson.find("type");
    if (!typeValue){
        fail("LoadLight: Fail to load Light: missing Light type");
        return false;
    }
    cg::LightType type;
    if (!GetLightType(*typeValue, type)){
        fail("LoadLight: Fail to get lightType");
        return false;
    }
    switch (type){
        case cg::LightType::Point:
            if (!LoadPointLight(lightJson, lightPtr)){
                fail("LoadLight: Fail to load PointLight");
                return false;
            }
            break;
        case cg::LightType::Directional:
            if (!LoadDirectionalLight(lightJson, lightPtr)){
                fail("LoadLight: Fail to load DirectionalLight");
                return false;
            }
            break;
        default:
            fail("LoadLight: Fail to load Light: unkonwn Light type:" + typeValue->asString());
        return false;
    }
    return true;
}
bool WorldLoader::LoadLightBuild(const JsonValue& lightJson, cg::LightBuild& lightbuild, JsonValue& rest){
    if (!lightJson.isObject()){
        fail("LoadLightBuild: Mismatch JsonValue type for LoadLightBuild, expected JsonValue type: Object");
        return false;
    }
    auto pairs = lightJson.asObject();
    for (size_t i = 0; i < pairs.size(); i++){
        std::string key = pairs[i].first;
        JsonValue value = pairs[i].second;
        if (compareLowerLiteral(key, "type")){
            continue;
        }else if (compareLowerLiteral(key, "name")){
            if (!value.isString()){
                fail("LoadLightBuild: Fail to load Light name: expected JsonValue type: String");
                return false;
            }
            lightbuild.name = value.asString();
        }else if (compareLowerLiteral(key, "intensity")){
            double tmpIntensity;
            if (!GetNumber(value, tmpIntensity)){
                fail("LoadLightBuild: Fail to load Light intensity");
                return false;
            }
            lightbuild.intensity = static_cast<float>(tmpIntensity);
        }else{
            rest.addPair(key, value);
        }
    }
    return true;
}
bool WorldLoader::LoadPointLight(const JsonValue& plightJson, std::unique_ptr<cg::Light>& lightPtr){
    if (!plightJson.isObject()){
        fail("LoadPointLight: Mismatch JsonValue type for LoadPointLight, expected JsonValue type: Object");
        return false;
    }
    cg::PointLightBuild plightbuild;
    plightbuild.type = cg::LightType::Point;
    JsonValue rest = JsonValue(JsonValue::Type::Object);
    if (!LoadLightBuild(plightJson, plightbuild, rest)){
        fail("LoadPointLight: Fail to load LightBuild");
        return false;
    }
    auto pairs = rest.asObject();
    for (size_t i = 0; i < pairs.size(); i++){
        std::string key = pairs[i].first;
        JsonValue value = pairs[i].second;
        if (compareLowerLiteral(key, "position")){
            if (!GetVec3(value, plightbuild.position)){
                fail("LoadPointLight: Fail to load light position");
                return false;
            }
        }else{
            fail("LoadPointLight: Unknown PointLight attribute key: " + key);
            return false;
        }
    }
    lightPtr = std::make_unique<cg::PointLight>(plightbuild);
    return true;
}
bool WorldLoader::LoadDirectionalLight(const JsonValue& dlightJson, std::unique_ptr<cg::Light>& lightPtr){
    if (!dlightJson.isObject()){
        fail("LoadDirectionalLight: Mismatch JsonValue type for LoadDirectionalLight, expected JsonValue type: Object");
        return false;
    }
    cg::DirectionalLightBuild dlightbuild;
    dlightbuild.type = cg::LightType::Directional;
    JsonValue rest = JsonValue(JsonValue::Type::Object);
    if (!LoadLightBuild(dlightJson, dlightbuild, rest)){
        fail("LoadDirectionalLight: Fail to load LightBuild");
        return false;
    }
    auto pairs = rest.asObject();
    for (size_t i = 0; i < pairs.size(); i++){
        std::string key = pairs[i].first;
        JsonValue value = pairs[i].second;
        if (compareLowerLiteral(key, "direction")){
            if (!GetVec3(value, dlightbuild.direction)){
                fail("LoadDirectionalLight: Fail to load light direction");
                return false;
            }
        }else{
            fail("LoadDirectionalLight: Unknown DirectionalLight attribute key: " + key);
            return false;
        }
    }
    lightPtr = std::make_unique<cg::DirectionalLight>(dlightbuild);
    return true;
}

bool WorldLoader::minimumRunCheck(const std::shared_ptr<SDL_Application>& appPtr){
    if (!appPtr->isSamplerLoaded()){
        fail("minimumRunCheck: SDLapp missing loaded sampler");
        return false;
    }
    return true;
}
bool WorldLoader::minimumRunCheck(const std::shared_ptr<cg::Sampler>& samplerPtr){
    if (!samplerPtr->isCameraLoaded()){
        fail("minimumRunCheck: sampler missing loaded camera");
        return false;
    }
    if (!samplerPtr->isSceneLoaded()){
        fail("minimumRunCheck: sampler missing loaded scene");
        return false;
    }
    return true;
}

bool WorldLoader::checkSamplerBuild(const cg::SamplerBuild& build){
    return true;
}
bool WorldLoader::checkCameraBuild(const cg::CameraBuild& build){
    if ((build.right - build.forward.cross(build.up)).length() > CGMath_EPS){
        fail("checkCameraBuild Fail to load camera basis: Expected orthogonal basis");
        return false;
    }
    return true;
}
bool WorldLoader::checkSDLappBuild(const SDLappBuild& build){
    return true;
}


bool WorldLoader::GetBool(const JsonValue& boolJson, bool& boolean){
    if (!boolJson.isBool()){
        fail("GetBool: Mismatch JsonValue type for GetBool, expected type: Bool");
        return false;
    }
    boolean = boolJson.asBool();
    return true;
}
bool WorldLoader::GetColor(const JsonValue& colorJson, uint32_t& color){
    if (!colorJson.isString()){
        fail("GetColor: Mismatch JsonValue type for GetColor, expected type: String");
        return false;
    }
    if (!strTocolor(colorJson.asString(), color)){
        fail("GetColor: Invaild color format: \"" + colorJson.asString() + "\"");
        return false;
    }
    return true;
}
bool WorldLoader::GetNumber(const loader::JsonValue& numberJson, double& number){
    if (!numberJson.isNumber()){
        fail("GetNumber: Mismatch JsonValue type for GetNumber, expected type: Number");
        return false;
    }
    number = numberJson.asNumber();
    return true;
}
bool WorldLoader::GetVec3(const JsonValue& Vec3Json, Vec3& vec3){
    if (!Vec3Json.isArray()){
        fail("GetVec3: Mismatch JsonValue type for GetVec3, expected type: Array");
        return false;
    }
    if (Vec3Json.asArray().size() != 3){
        fail("GetVec3: Unexpected Array[" +  std::to_string(Vec3Json.asArray().size()) + "] for GetVec3, expected Array format: Array[3]");
        return false;
    } 
    float x, y, z;
    for (size_t i = 0; i < 3; i++){
        double number;
        if (!GetNumber(Vec3Json.asArray()[i], number)){
            fail("GetVec3: Fail to get Vec3: Array[" + std::to_string(i) +"]");
            return false;
        }
        switch (i){
            case 0:
                x = static_cast<float>(number);
                break;
            case 1:
                y = static_cast<float>(number);
                break;
            case 2:
                z = static_cast<float>(number);
                break;
        }
    }
    vec3 = Vec3(x, y, z);
    return true;
}
bool WorldLoader::GetSamplerType(const JsonValue& samplertypeJson, cg::SamplerType& type){
    if (!samplertypeJson.isString()){
        fail("GetSamplerType: Mismatch JsonValue type for GetSamplerType, expected type: String");
        return false;
    }
    if (compareLowerLiteral(samplertypeJson.asString(), "raytracer")){
        type = cg::SamplerType::Raytracer;
    }else if (compareLowerLiteral(samplertypeJson.asString(), "rasterizer")){
        type = cg::SamplerType::Rasterizer;
    }else{
        fail("GetSamplerType: Fail to get SamplerType, unknown attribute:" + samplertypeJson.asString());
        type = cg::SamplerType::None;
        return false;
    }
    return true;
}
bool WorldLoader::GetSamplingMode(const JsonValue& samplingmodeJson, cg::SamplingMode& mode){
    if (!samplingmodeJson.isString()){
        fail("GetSamplingMode: Mismatch JsonValue type for GetSamplingMode, expected type: String");
        return false;
    }
    if (compareLowerLiteral(samplingmodeJson.asString(), "VisibilityOnly")){
        mode = cg::SamplingMode::VisibilityOnly;
    }else if (compareLowerLiteral(samplingmodeJson.asString(), "DirectLighting")){
        mode = cg::SamplingMode::DirectLighting;
    }else if (compareLowerLiteral(samplingmodeJson.asString(), "HardShadows")){
        mode = cg::SamplingMode::HardShadows;
    }else if (compareLowerLiteral(samplingmodeJson.asString(), "RecursiveReflection")){
        mode = cg::SamplingMode::RecursiveReflection;
    }else{
        fail("GetSamplingMode: Fail to get SamplingMode, unknown attribute:" + samplingmodeJson.asString());
        mode = cg::SamplingMode::None;
        return false;
    }
    return true;
}
bool WorldLoader::GetObjectType(const JsonValue& objtypeJson, cg::ObjectType& type){
    if (!objtypeJson.isString()){
        fail("GetObjectType: Mismatch JsonValue type for GetObjectType, expected type: String");
        return false;
    }
    if (compareLowerLiteral(objtypeJson.asString(), "sphere")){
        type = cg::ObjectType::Sphere;
    }else if (compareLowerLiteral(objtypeJson.asString(), "plane")){
        type = cg::ObjectType::Plane;
    }else{
        fail("GetObjectType: Fail to get ObjectType, unknown attribute:" + objtypeJson.asString());
        type = cg::ObjectType::None;
        return false;
    }
    return true;
}
bool WorldLoader::GetLightType(const JsonValue& lighttypeJson, cg::LightType& type){
    if (!lighttypeJson.isString()){
        fail("GetLightType: Mismatch JsonValue type for GetLightType, expected type: String");
        return false;
    }
    if (compareLowerLiteral(lighttypeJson.asString(), "point")){
        type = cg::LightType::Point;
    }else if (compareLowerLiteral(lighttypeJson.asString(), "directional")){
        type = cg::LightType::Directional;
    }else{
        fail("GetLightType: Fail to get LightType, unknown attribute:" + lighttypeJson.asString());
        type = cg::LightType::None;
        return false;
    }
    return true;
}

bool loader::strTocolor(const std::string& str, uint32_t& color){
    if (str.size() != 9 || str[0] != '#') return false;
    for (size_t i = 1; i < 9; i++){
        if (!std::isxdigit(static_cast<unsigned char>(str[i]))){
            return false;
        }
    }
    try{
        color = static_cast<uint32_t>(std::stoul(str.substr(1), nullptr, 16));
    }
    catch (...){
        return false;
    }
    return true;
}
bool loader::compareLowerLiteral(const std::string& test, const std::string& literal) {
    if (test.size() != literal.size())
        return false;

    return std::equal(test.begin(), test.end(), literal.begin(),
        [](unsigned char a, unsigned char b) {
            return std::tolower(a) == std::tolower(b);
        });
}
