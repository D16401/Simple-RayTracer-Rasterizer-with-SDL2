#include "cg_ld_parser.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <windows.h>
#include <direct.h>

using loader::Parser;

bool Parser::parse(JsonValue& out){
    skipWhiteSpace();
    if (eof()){
        fail("Empty input");
        return false;
    }
    if (!parseValue(out)) return false;
    skipWhiteSpace();
    if (!eof()){
        fail("Unexpected input");
        return false;
    }
    return true;
};
bool Parser::skipWhiteSpace(){
    while(peek() == ' '  || peek() == '\t' ||
          peek() == '\n' || peek() == '\r'){
        moveOn();
    }
    return true;
}
bool Parser::parseValue(JsonValue& out){
    skipWhiteSpace();
    if (pos >= text.size()){
        fail("Unexpected end of input");
        return false;
    }
    char c = peek();
    switch (c){
        case '{':
            return parseObject(out);
            break;
        case '[':
            return parseArray(out);
            break;
        case '"':
            return parseString(out);
            break;
        case 't':
            return parseLiteral("true", out);
            break;
        case 'f':
            return parseLiteral("false", out);
            break;
        case 'n':
            return parseLiteral("null", out);
            break;
        case '-':
            return parseNumber(out);
            break;
        case 'l':
            return parseLink(out);
            break;
        default:
            if (std::isdigit(c)){
                return parseNumber(out);
            }else{
                fail("Unexpected character: " + std::string(1, c));
                return false;
            }
    }
    return true;
}
bool Parser::parseObject(JsonValue& out){
    if (!expect('{')) return false;
    out = JsonValue(JsonValue::Type::Object);
    while(true){
        skipWhiteSpace();
        if (pos < text.size() && text[pos] == '}') break;
        if (out.countPairs() > 0){
            if (!expect(',')){
                return false;
            }else{
                skipWhiteSpace();
                if (pos < text.size() && text[pos] == '}'){
                    fail("Unexpected comma");
                    return false;
                }
            }
        }
        JsonValue stringJson;
        if (!parseString(stringJson)) return false;
        std::string key = stringJson.asString();
        skipWhiteSpace();
        if (!expect(':')) return false;
        JsonValue value;
        if (!parseValue(value)) return false;
        out.addPair(key, value);
    }
    return expect('}');
}
bool Parser::parseArray(JsonValue& out){
    if (!expect('[')) return false;
    out = JsonValue(JsonValue::Type::Array);
    while (true){
        skipWhiteSpace();
        if (pos < text.size() && text[pos] == ']') break;
        if (out.countArr() > 0){
            if (!expect(',')){
                return false;
            }else{
                skipWhiteSpace();
                if (pos < text.size() && text[pos] == ']'){
                    fail("Unexpected comma");
                    return false;
                }
            }
        }

        JsonValue value;
        if (!parseValue(value)) return false;
        out.addArr(value);
    }
    return expect(']');
}
bool Parser::parseString(JsonValue& out){
    if (!expect('"')) return false;
    std::string str = "";
    while(pos < text.size() && text[pos] != '"'){
        //不跳空
        if (text[pos] == '\\'){
            moveOn();
            if (pos >= text.size()){
                fail("Unexpected end in escape");
                return false;
            }
            switch (text[pos]){
                case '\\':str += '\\'; break;
                case '"': str += '"';  break;
                case '/': str += '/';  break;
                case 'b': str += '\b'; break;
                case 'f': str += '\f'; break;
                case 'n': str += '\n'; break;
                case 'r': str += '\r'; break;
                case 't': str += '\t'; break;
                case 'u':
                    fail("u escape not supported");
                    return false;
                default:
                    fail(std::string("Invalid escape '\\") + text[pos] + "'");
                    return false;
            }
        }else{
            if (static_cast<unsigned char>(text[pos]) < 0x20){
                fail("Unescaped control character");
                return false;
            }
            str += text[pos];
        }
        moveOn();
    }
    if (pos >= text.size()){
        fail("Unterminated string");
        return false;
    }
    out = JsonValue(std::move(str));
    return expect('"');
}
bool Parser::parseNumber(JsonValue& out){
    skipWhiteSpace();
    size_t start = pos;
    //符号
    if (pos < text.size() && text[pos] == '-') moveOn();
    //整数部分
    size_t intStart = pos;
    if (text[pos] == '0' && std::isdigit(text[pos+1])){
        fail("Unexpected number with leading zero");
        return false;
    }
    while (pos < text.size() && std::isdigit(text[pos])) moveOn();
    if (pos -intStart < 1){
        fail("Unexpected number missing integer part");
        return false;
    }
    //小数部分
    if (pos < text.size() && text[pos] == '.'){
        moveOn();
        size_t fracStart = pos;
        while (pos < text.size() && std::isdigit(text[pos])) moveOn();
        if (pos - fracStart < 1){
            fail("Unexpected number missing fractional part after '.'");
            return false;
        }
    }
    if (pos < text.size() && (text[pos] == 'e' || text[pos] == 'E')){
        moveOn();
        if (pos < text.size() && (text[pos] == '+' || text[pos] == '-')) moveOn();
        size_t expStart = pos;
        while (pos < text.size() && std::isdigit(text[pos])) moveOn();
        if (pos - expStart < 1){
            fail("Unexpected number missing exponent part after 'e' or 'E'");
            return false;
        }
    }
    double number = std::stod(text.substr(start, pos - start));
    out = JsonValue(number);
    return true;
}
bool Parser::parseLiteral(const std::string& literal, JsonValue& out){
    skipWhiteSpace();
    if (text.compare(pos, literal.size(), literal) != 0){//比较pos后若干个字符和literral是否相同
        fail("Invalid literal, expected " + literal);
        return false;
    }
    pos += literal.size();
    char next = (pos < text.size()) ? text[pos] : '\0';
    if (next != '\0' && !std::isspace(next) &&
        next != ',' && next != '}' && next != ']'){
        fail("Invalid characters after literal " + literal);
        return false;// 拦住 truex / null1
    }
    if (literal == "true"){
        out = JsonValue(JsonValue::Type::Bool);
        out.setBoolean(true);
    }else if(literal == "false"){
        out = JsonValue(JsonValue::Type::Bool);
        out.setBoolean(false);
    }else {
        out = JsonValue(JsonValue::Type::Null);
    }
    return true;
}//true/false/null
bool Parser::parseLink(JsonValue& out){
    skipWhiteSpace();
    if (text.compare(pos, 4, "link") != 0){
        fail("Invalid character, you mean \"link\"?");
        return false;
    }
    pos += 4;
    skipWhiteSpace();
    if (peek() != '\"'){
        fail("Invalid input after \"link\", expect String");
        return false;
    }
    JsonValue pathStrJson;
    if (!parseString(pathStrJson)){
        fail("Fail to load link path string");
        return false;
    }
    std::string path = baseDir + "\\" + pathStrJson.asString();
    if (!parseJsonFile(path, out, err)){
        fail("Fail to load json from relative path:\""+ pathStrJson.asString() +"\"");
        return false;
    }
    return true;
}
void Parser::fail(const std::string& msg){
    int n_line = 0;
    int n_char = 0;
    for(int i = 0; i < pos; i++){
        n_char++;
        if (text[i] == '\n'){
            n_line++;
            n_char = 0;
        }
    }
    std::ostringstream oss;
    oss << "JSON Parse Erorr <" + baseDir + "\\" + filename + ">: (l" 
    << n_line << ", c" << n_char << ") " << msg << "\n";
    err = err + oss.str();
}
char Parser::peek()const{
    if (pos < text.size()){
        return text[pos];
    }else{
        return '\0';
    }

}
char Parser::get(){//获取当前字符并前进
    char c = peek();
    pos = pos + 1;
    return c;
}
bool Parser::moveOn(){
    if (pos < text.size()){
        pos += 1;
        return true;
    }else{
        return false;
    }
}
bool Parser::expect(char c){
    skipWhiteSpace();
    if (pos >= text.size() || text[pos] != c){
        fail("Unexpected character " + std::string(1, text[pos]) + ", expected " + std::string(1, c));
        return false;
    }
    moveOn();
    return true;
}
bool Parser::eof()const{
    if (pos == text.size()){
        return true;
    }else{
        return false;
    }
};

bool readFile(const std::string& fileName, std::string& text, std::string& err){
    std::ifstream ifs(fileName, std::ios::in | std::ios::binary);
    if (!ifs){
        err = err + ("\nloadFile: No such file or directory or access denied.");
        return false;
    }
    ifs.seekg(0, std::ifstream::end);//移动文件指针到文件结尾
    size_t size = ifs.tellg();//获取当前位置，从而计算文件大小
    ifs.seekg(0, std::ifstream::beg);//移动文件指针到文件开头
    std::string content(size, '\0');//创建一个大小为size的字符串，并以'\0'初始化所有字符
    ifs.read(&content[0], size);//读取size个字节到字符串content中
    text = content;
    return true;
}
bool loader::parseJsontxt(const std::string& text, JsonValue& out, std::string& error){
    Parser parser(text);
    bool returnValue = parser.parse(out);
    error = parser.GetErr();
    return returnValue;
}
bool loader::parseJsonFile(const std::string& path, JsonValue& out, std::string& error){
    std::string text;
    std::string fileErr;
    if (!readFile(path, text, fileErr)){
        return false;
    }
    Parser parser(text, path);
    bool returnValue = parser.parse(out);
    error = fileErr.append(parser.GetErr());
    return returnValue;
}
std::string GetExeBaseDir() {
    char exePath[MAX_PATH] = {0};
    GetModuleFileNameA(nullptr, exePath, MAX_PATH);

    std::string path(exePath);
    size_t pos = path.find_last_of("\\/");
    if (pos != std::string::npos)
        return path.substr(0, pos);
    return ".";
}
void splitPath(const std::string& path, std::string& baseDir, std::string& filename) {
    std::filesystem::path p(path);
    baseDir = p.parent_path().string();  // 获取目录部分
    filename = p.filename().string();    // 获取文件名部分
}
bool loader::getJsonFileInFolder(const std::string& path, std::string& file, std::string& error) {
    namespace fs = std::filesystem;

    if (!fs::exists(path) || !fs::is_directory(path)) {
        error = "Path does not exist or is not a directory: " + path;
        return false;
    }

    std::vector<fs::path> jsons;
    for (const auto& entry : fs::directory_iterator(path)) {
        if (!entry.is_regular_file()) continue;

        auto ext = entry.path().extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (ext == ".json") {
            jsons.push_back(entry.path());
        }
    }

    if (jsons.empty()) {
        error = "No json file found in " + path;
        return false;
    }

    std::sort(jsons.begin(), jsons.end(),
        [](const fs::path& a, const fs::path& b) {
            return a.filename().string() < b.filename().string();
        });

    file = jsons.front().string();
    return true;
}
void WriteErrorLog(const std::string& errorMsg) {
    std::string baseDir = GetExeBaseDir();

    // exe 文件名（不带扩展名）
    std::string exeName = baseDir.substr(baseDir.find_last_of("\\/") + 1);
    std::string baseName = exeName.substr(0, exeName.find_last_of('.'));

    // log 目录
    std::string logDir = baseDir + "\\log";
    CreateDir(logDir);

    // 日志文件名
    std::string timeStr = GetCurrentTimeForFile();
    std::string logFile = logDir + "\\" + baseName + "_" + timeStr + ".log";

    // 写入日志
    std::ofstream ofs(logFile, std::ios::app);
    if (!ofs.is_open())
        return;

    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
    localtime_s(&tm, &time);

    ofs << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "] "
        << errorMsg << std::endl;
}
std::string GetCurrentTimeForFile() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
    localtime_s(&tm, &time);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y%m%d_%H%M%S");
    return oss.str();
}
bool CreateDir(const std::string& path) {
    return _mkdir(path.c_str()) == 0 || errno == EEXIST;
}