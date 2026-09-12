#include "cg_ld_jsonvalue.h"

#include <iostream>

using loader::JsonValue;

const JsonValue* JsonValue::find(const std::string& key)const{
    for (int i = 0; i < obj.size(); i++){
        if (obj[i].first == key){
            return &(obj[i].second);
        }
    }
    return nullptr;
}
void JsonValue::addPair(const std::string& key, const JsonValue& value){
    obj.push_back(std::pair<std::string, JsonValue>(key, value));
}
void JsonValue::dump(int depth)const{
    switch(type){
        case Type::Null:
            std::cout << "null";
            break;
        case Type::Bool:
            if (boolean){
                std::cout << "true";
            }else{
                std::cout << "false";
            }
            break;
        case Type::Number:
            std::cout << number;
            break;
        case Type::String:
            std::cout << "\"" << str << "\"";
            break;
        case Type::Object:
            std::cout << "{" << "\n";
            depth++;
            for (int i = 0; i < obj.size(); i++){
                for (int t = 0; t < depth; t++){
                    std::cout << "    ";
                }
                std::cout << "\"" << obj[i].first << "\"" << ":";
                obj[i].second.dump(depth);
                if(i != obj.size()-1){
                    std::cout << ",";
                }
                std::cout << "\n";
            }
            depth--;
            for (int t = 0; t < depth; t++){
                std::cout << "    ";
            }
            std::cout << "}";
            break;
        case Type::Array:
            std::cout << "[" << "\n";
            depth++;
            for (int i = 0; i < arr.size(); i++){
                for (int t = 0; t < depth; t++){
                    std::cout << "    ";
                }
                arr[i].dump(depth);
                if(i != arr.size()-1){
                    std::cout << ",";
                }
                std::cout << "\n";
            }
            depth--;
            for (int t = 0; t < depth; t++){
                std::cout << "    ";
            }
            std::cout << "]";
            break;
    }
}

