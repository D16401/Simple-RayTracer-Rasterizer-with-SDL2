#include "simple_cg.h"

int main(int argc, char* argv[]){
    try {
        std::filesystem::path baseDir = GetExeBaseDir();
        loader::JsonValue jsonvalue;
        std::string errorMsg;
        if (loader::parseJsonFile(baseDir.string() + "/../assets/test.json", jsonvalue, errorMsg)){
            jsonvalue.dump(0);
        }
        std::cout << errorMsg << std::endl;
    }
    catch (const std::exception& e){
        std::cerr << e.what() << std::endl;
    }
    return 0;
}