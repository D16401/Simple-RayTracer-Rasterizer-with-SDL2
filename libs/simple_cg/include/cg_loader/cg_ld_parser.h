#pragma once

#include <string>
#include <filesystem>

#include "cg_ld_jsonvalue.h"

bool readFile(const std::string& fileName, std::string& text, std::string& error);
std::string GetExeBaseDir();
void splitPath(const std::string& path, std::string& baseDir, std::string& filename);
void WriteErrorLog(const std::string& errorMsg);
std::string GetCurrentTimeForFile();
bool CreateDir(const std::string& path);

namespace loader{

class Parser{
public:
    Parser(const std::string& text) : text(text) {}
    Parser(const std::string& text, const std::string& path) : text(text){
        splitPath(path, baseDir, filename);
    }
    bool parse(JsonValue& out);//顶层入口
    const std::string& GetErr()const{return err;}
private:
    const std::string& text;//被解析的文本
    size_t pos = 0;//当前解析字符位置
    std::string err;//错误信息
    std::string baseDir;
    std::string filename;
    //parse method
    bool skipWhiteSpace();
    bool parseValue(JsonValue& out);
    bool parseObject(JsonValue& out);
    bool parseArray(JsonValue& out);
    bool parseString(JsonValue& out);
    bool parseNumber(JsonValue& out);
    bool parseLiteral(const std::string& literal, JsonValue& out);//true/false/null
    bool parseLink(JsonValue& out);
    void fail(const std::string& msg);//生成带行号的错误信息
    char peek()const;//当前字符，越界返回'\0'
    char get();//获取当前字符并前进
    bool moveOn();//前进
    bool expect(char c);//消费当前位置的期望字符，若非期望则报错
    bool eof()const;
};


bool parseJsontxt(const std::string& text, JsonValue& out, std::string& error);
bool parseJsonFile(const std::string& path, JsonValue& out, std::string& error);
bool getJsonFileInFolder(const std::string& path, std::string& file, std::string& error);

};
