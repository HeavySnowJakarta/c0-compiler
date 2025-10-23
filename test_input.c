// 测试C0编译器的词法分析功能
// This is a test file for C0 compiler

/* 多行注释测试
   包含关键字、标识符、常量等 */

int main() {
    // 声明变量
    int count = 10;
    double pi = 3.14159;
    double sci_num = 1.23e-5;
    char letter = 'A';
    const int MAX_SIZE = 100;
    
    // 标识符测试（包含下划线）
    int my_variable = 42;
    int _private_var = 0;
    int test123 = 0;
    
    // 十六进制数测试
    int hex_value = 0xFF;
    int another_hex = 0x1A2B;
    
    // 字符串常量测试
    char *message = "Hello, World!";
    char *escaped = "Line1\nLine2\tTabbed";
    
    // 运算符测试
    if (count >= 10 && count <= 20) {
        count = count + 1;
    }
    
    while (count > 0) {
        count--;
    }
    
    for (int i = 0; i < MAX_SIZE; i++) {
        // 循环体
    }
    
    // 条件运算
    int result = (count == 0) ? 1 : 0;
    bool flag = (pi != 3.14) || (letter == 'A');
    
    return 0;
}

// 结构体定义测试
struct Point {
    double x;
    double y;
};

void test_function(int param1, double param2) {
    // 函数体
    if (param1 > 0) {
        return;
    }
}
