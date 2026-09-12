#include "simple_cg.h"

int main(int argc, char* argv[]){
    loader::JsonValue json;
    std::string errorMsg;
    std::filesystem::path baseDir = GetExeBaseDir();
    std::string filename;
    if (!loader::getJsonFileInFolder((baseDir.string() + "/../assets/JsonSettings/current/"), filename, errorMsg)){
        WriteErrorLog(errorMsg);
        return 1;
    }
    if (!loader::parseJsonFile(filename, json, errorMsg)){
        WriteErrorLog(errorMsg);
        return 1;
    }
    std::shared_ptr<cg::SDL_Application> appPtr;
    loader::WorldLoader worldloader;
    if (!worldloader.LoadSettings(json, appPtr)){
        WriteErrorLog(worldloader.GetErr());
        return 1;
    }   
    appPtr->Init();
    appPtr->Run();
    return 0;
}