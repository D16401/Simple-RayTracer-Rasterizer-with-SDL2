#pragma once

#include <string>
#include <vector>
#include <utility>

namespace loader{

class JsonValue{
public:
    enum class Type {Null, Bool, Number, String, Array, Object};
    JsonValue(): type(Type::Null) {}
    JsonValue(Type type): type(type){}
    JsonValue(std::string s): type(Type::String), str(s){}
    JsonValue(double n): type(Type::Number), number(n){}
    JsonValue(const JsonValue& jsonvalue) =default;
    //check type
    bool isNumber()const{return type == Type::Number;}
    bool isBool()const{return type == Type::Bool;}
    bool isString()const{return type == Type::String;}
    bool isArray()const{return type == Type::Array;}
    bool isObject()const{return type == Type::Object;}
    //read member
    double asNumber(double def = 0.0)const{return (type == Type::Number) ? number : def;}//default默认返回值为0.0
    bool asBool(bool def = false)const{return (type == Type::Bool) ? boolean : def;}//default默认返回值为false
    const std::string asString(const std::string& def = "")const{return (type == Type::String) ? str : def;}//default默认返回值为""（空字符串）
    std::vector<JsonValue> asArray()const{return arr;}
    std::vector<std::pair<std::string, JsonValue>> asObject()const{return obj;}
    //find
    const JsonValue* find(const std::string& key)const;
    //count
    size_t countPairs(){return obj.size();};
    size_t countArr(){return arr.size();};
    //modify
    void setBoolean(bool b){boolean = b;}
    void addPair(const std::string& key, const JsonValue& value);
    void addArr(JsonValue value){arr.push_back(value);}
    //debug
    void dump(int depth)const;
private:
    Type type = Type::Null;
    bool boolean = false;//Bool
    double number = 0.0;//Number
    std::string str;//String
    std::vector<JsonValue> arr;//Array
    std::vector<std::pair<std::string, JsonValue>> obj;//Object（保序）
};

};

