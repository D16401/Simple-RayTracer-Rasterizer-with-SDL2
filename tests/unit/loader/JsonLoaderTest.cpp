// JsonTest.cpp — JSON 解析器验收测试（链接 simple_cg 即可编译）
// 全部通过返回 0，任一失败返回 1
#include "cg_ld_parser.h"
#include <iostream>
#include <string>

static int failures = 0;

struct Case {
    const char* json;
    bool shouldOk;   // true=期望解析成功, false=期望解析失败
    const char* note;
};

static void check(const Case& c){
    loader::JsonValue value;
    std::string err;
    bool ok = loader::parseJsontxt(c.json, value, err);
    bool pass = (ok == c.shouldOk);
    if (!pass) failures++;
    std::cout << (pass ? "[PASS] " : "[FAIL] ")
              << (ok ? "OK   " : "ERR  ")
              << "\"" << c.json << "\"  期望" << (c.shouldOk ? "成功" : "失败")
              << "  (" << c.note << ")";
    if (!pass){
        std::string first = err.substr(0, err.find('\n'));
        std::cout << "\n       解析器报错: " << first;
    }
    std::cout << std::endl;
}

int main(){
    // —— 合法输入：应解析成功 ——
    const Case okCases[] = {
        {"{}", true, "空对象"},
        {"[]", true, "空数组"},
        {"{\"a\":1,\"b\":2}", true, "多成员对象"},
        {"{\"a\":true,\"b\":2}", true, "对象内含布尔值"},
        {"[true]", true, "数组内含布尔值"},
        {"[false,1]", true, "布尔值后跟数字"},
        {"{\"x\":null}", true, "对象内含 null"},
        {"[null]", true, "数组内含 null"},
        {"true", true, "顶层布尔值"},
        {"null", true, "顶层 null"},
        {"[1,2,3]", true, "数字数组"},
        {"{\"a\":{\"b\":[1,2,3]}}", true, "多层嵌套"},
        {"-0", true, "负零"},
        {"0.5", true, "小数"},
        {"1e5", true, "指数"},
        {"1E+2", true, "指数带符号"},
        {"\"a b\"", true, "字符串内空格"},
        {"\"a\\\"b\\\\c\\/d\\b\\f\\n\\r\\t\"", true, "全部转义字符"},
        {"  { \"a\" : [ 1 , 2 ] }  ", true, "任意位置空白"},
        {"{\"a\":1}", true, "单成员对象"},
    };
    // —— 非法输入：应解析失败 ——
    const Case errCases[] = {
        {"{\"a\":1,}", false, "对象尾逗号"},
        {"[1,]", false, "数组尾逗号"},
        {"[1,2,]", false, "数组尾逗号2"},
        {"1e", false, "指数缺数字"},
        {"1e+", false, "指数只有符号"},
        {"\"abc", false, "字符串未闭合"},
        {"{\"a\" 1}", false, "缺冒号"},
        {"{\"a\":}", false, "缺值"},
        {"{", false, "对象未闭合"},
        {"[1 2]", false, "缺逗号"},
        {"01", false, "前导零"},
        {"1.", false, "小数点后无数字"},
        {"+1", false, "数字前正号"},
        {"tru", false, "残缺字面量"},
        {"truex", false, "字面量后跟字母"},
        {"{} garbage", false, "尾随垃圾"},
        {"", false, "空输入"},
    };
    std::cout << "===== 合法输入（应全部 OK）=====" << std::endl;
    for (const auto& c : okCases) check(c);
    std::cout << "===== 非法输入（应全部 ERR）=====" << std::endl;
    for (const auto& c : errCases) check(c);
    std::cout << "===== 结果: "
              << (failures == 0 ? "ALL PASS" : (std::to_string(failures) + " FAILURES"))
              << " =====" << std::endl;
    return failures != 0;
}