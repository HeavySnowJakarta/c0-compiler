# C0编译器实验报告

## 一、实验目的

1. 理解编译原理中词法分析的基本原理和实现方法
2. 掌握正规式、NFA和DFA的构造与转换算法
3. 实现一个能够识别C0语言基本词法元素的词法分析器
4. 实现NFA到DFA的确定化和DFA的最简化算法

## 二、实验环境

- 编程语言：C (C99标准)
- 编译器：GCC
- 操作系统：Linux
- 开发工具：Make, Git

## 三、设计思想

### 3.1 总体架构

C0编译器采用模块化设计，主要分为以下几个模块：

```
c0-compiler/
├── token.h/token.c         # Token定义和操作
├── lexer.h/lexer.c         # 词法分析器
├── nfa_dfa.h/nfa_dfa.c     # NFA/DFA构造和转换
└── main.c                  # 主程序和命令行接口
```

### 3.2 词法分析器设计

词法分析器采用**有限状态自动机**的设计思想，通过状态转换来识别不同类型的Token。

#### 主要状态：
- `STATE_START`: 初始状态
- `STATE_IDENTIFIER`: 标识符识别状态
- `STATE_NUMBER`: 数字识别状态
- `STATE_HEX_NUMBER`: 十六进制数识别状态
- `STATE_FLOAT`: 浮点数识别状态
- `STATE_EXPONENT`: 指数识别状态
- `STATE_STRING`: 字符串识别状态
- `STATE_CHAR`: 字符识别状态
- `STATE_COMMENT_LINE`: 单行注释状态
- `STATE_COMMENT_BLOCK`: 多行注释状态

#### Token类型：
词法分析器能够识别以下类型的Token：
- **关键字**：const, int, double, char, void, if, else, while, for, return, break, continue, struct
- **标识符**：以字母或下划线开头，由字母、数字、下划线组成
- **常量**：
  - 整型常量：10进制（123）、16进制（0xFF）
  - 浮点常量：小数（3.14）、科学计数法（1.23e-5）
  - 字符常量：'a', '\n'
  - 字符串常量："Hello"
- **运算符**：+, -, *, /, %, =, ==, !=, <, <=, >, >=, &&, ||, !
- **分隔符**：;, ,, (, ), {, }, [, ]

### 3.3 NFA/DFA构造设计

#### 标识符的正规式
```
identifier = letter(letter|digit)*
letter = [a-zA-Z_]
digit = [0-9]
```

#### NFA设计
NFA包含2个状态：
- 状态0：初始状态
- 状态1：接受状态（终态）

转换规则：
- 状态0 --[letter]--> 状态1
- 状态1 --[letter|digit]--> 状态1

#### NFA到DFA转换
采用**子集构造法（Subset Construction）**：
1. 计算初始状态的ε闭包
2. 对每个未处理的DFA状态和每个输入符号：
   - 计算move操作
   - 计算ε闭包
   - 创建或查找对应的DFA状态
3. 标记包含NFA终态的DFA状态为终态

#### DFA最简化
采用**状态等价类划分算法**：
1. 初始划分：将状态分为终态和非终态两类
2. 迭代细化划分：
   - 对每个划分中的状态对，检查是否可区分
   - 如果对相同输入转移到不同划分，则可区分
   - 将可区分的状态分到新的划分
3. 根据最终划分构造最简DFA

## 四、关键算法实现

### 4.1 词法分析核心算法

```c
Token *get_next_token(Lexer *lexer) {
    while (lexer->current_char != '\0') {
        // 跳过空白字符
        if (isspace(lexer->current_char)) {
            skip_whitespace(lexer);
            continue;
        }
        
        // 处理注释
        if (lexer->current_char == '/') {
            if (peek(lexer) == '/') {
                skip_line_comment(lexer);
                continue;
            } else if (peek(lexer) == '*') {
                skip_block_comment(lexer);
                continue;
            }
        }
        
        // 识别标识符或关键字
        if (isalpha(lexer->current_char) || lexer->current_char == '_') {
            return read_identifier(lexer);
        }
        
        // 识别数字常量
        if (isdigit(lexer->current_char)) {
            return read_number(lexer);
        }
        
        // 识别字符串和字符常量
        if (lexer->current_char == '"') {
            return read_string(lexer);
        }
        if (lexer->current_char == '\'') {
            return read_char(lexer);
        }
        
        // 识别运算符和分隔符
        // ...
    }
    
    return create_token(TOKEN_EOF, "", lexer->line, lexer->column);
}
```

### 4.2 数字识别算法

支持三种数字格式：
1. **10进制整数**：连续的数字
2. **16进制整数**：0x或0X开头，后跟16进制数字
3. **浮点数**：包含小数点或指数部分（e/E）

```c
Token *read_number(Lexer *lexer) {
    int is_float = 0;
    int is_hex = 0;
    
    // 检查16进制
    if (lexer->current_char == '0' && (peek(lexer) == 'x' || peek(lexer) == 'X')) {
        is_hex = 1;
        advance(lexer); // 跳过'0'
        advance(lexer); // 跳过'x'
        // 读取16进制数字
        while (isxdigit(lexer->current_char)) {
            advance(lexer);
        }
    } else {
        // 读取10进制数字
        while (isdigit(lexer->current_char)) {
            advance(lexer);
        }
        
        // 检查小数点
        if (lexer->current_char == '.' && isdigit(peek(lexer))) {
            is_float = 1;
            advance(lexer);
            while (isdigit(lexer->current_char)) {
                advance(lexer);
            }
        }
        
        // 检查指数部分
        if (lexer->current_char == 'e' || lexer->current_char == 'E') {
            is_float = 1;
            advance(lexer);
            if (lexer->current_char == '+' || lexer->current_char == '-') {
                advance(lexer);
            }
            while (isdigit(lexer->current_char)) {
                advance(lexer);
            }
        }
    }
    
    // 创建相应类型的Token
    // ...
}
```

### 4.3 NFA到DFA转换算法（子集构造法）

```c
DFA *nfa_to_dfa(NFA *nfa) {
    // 初始化DFA
    DFA *dfa = (DFA *)malloc(sizeof(DFA));
    
    // 计算初始状态的ε闭包
    StateSet initial_set;
    initial_set.count = 1;
    initial_set.states[0] = nfa->start_state;
    initial_set = epsilon_closure(nfa, initial_set);
    
    dfa_states[0] = initial_set;
    dfa->start_state = 0;
    
    // 工作队列
    int unmarked[MAX_STATES];
    int unmarked_count = 1;
    unmarked[0] = 0;
    
    // 子集构造算法
    while (unmarked_count > 0) {
        int current_dfa_state = unmarked[--unmarked_count];
        StateSet current_set = dfa_states[current_dfa_state];
        
        // 对字母表中的每个符号
        for (int i = 0; i < dfa->alphabet_size; i++) {
            char symbol = dfa->alphabet[i];
            
            // 计算move和ε闭包
            StateSet next_set = move(nfa, current_set, symbol);
            if (next_set.count > 0) {
                next_set = epsilon_closure(nfa, next_set);
                
                // 查找或创建新的DFA状态
                int next_dfa_state = find_state_set_index(dfa_states, num_dfa_states, &next_set);
                if (next_dfa_state == -1) {
                    next_dfa_state = num_dfa_states++;
                    dfa_states[next_dfa_state] = next_set;
                    unmarked[unmarked_count++] = next_dfa_state;
                }
                
                // 添加转换
                dfa->transition[current_dfa_state][(int)symbol] = next_dfa_state;
            }
        }
    }
    
    // 确定终态
    // ...
    
    return dfa;
}
```

### 4.4 DFA最简化算法

```c
DFA *minimize_dfa(DFA *dfa) {
    // 初始划分：终态和非终态
    int partition[MAX_STATES];
    for (int i = 0; i < dfa->num_states; i++) {
        partition[i] = dfa->final_states[i] ? 1 : 0;
    }
    
    // 迭代细化划分
    bool changed = true;
    while (changed) {
        changed = false;
        
        // 对每个划分中的状态对，检查是否可区分
        for (int p = 0; p < num_partitions; p++) {
            // 收集该划分中的状态
            // ...
            
            // 检查这些状态是否可区分
            for (int i = 1; i < count; i++) {
                bool distinguishable = false;
                
                // 检查所有输入符号
                for (int j = 0; j < dfa->alphabet_size; j++) {
                    // 如果转移到不同的划分，则可区分
                    if (partition[next1] != partition[next2]) {
                        distinguishable = true;
                        break;
                    }
                }
                
                if (distinguishable) {
                    // 分到新的划分
                    new_partition[s2] = next_partition_id++;
                    changed = true;
                }
            }
        }
    }
    
    // 构造最简DFA
    // ...
    
    return min_dfa;
}
```

## 五、实验结果

### 5.1 词法分析测试

#### 测试输入（test_input.c片段）：
```c
int main() {
    int count = 10;
    double pi = 3.14159;
    double sci_num = 1.23e-5;
    int hex_value = 0xFF;
    char *message = "Hello, World!";
}
```

#### 输出结果：
```
<INT, int> (行: 1, 列: 1)
<IDENTIFIER, main> (行: 1, 列: 5)
<LPAREN, (> (行: 1, 列: 9)
<RPAREN, )> (行: 1, 列: 10)
<LBRACE, {> (行: 1, 列: 12)
<INT, int> (行: 2, 列: 5)
<IDENTIFIER, count> (行: 2, 列: 9)
<ASSIGN, => (行: 2, 列: 15)
<INT_CONST, 10> [值: 10] (行: 2, 列: 17)
<SEMICOLON, ;> (行: 2, 列: 19)
<DOUBLE, double> (行: 3, 列: 5)
<IDENTIFIER, pi> (行: 3, 列: 12)
<ASSIGN, => (行: 3, 列: 15)
<DOUBLE_CONST, 3.14159> [值: 3.14159] (行: 3, 列: 17)
<SEMICOLON, ;> (行: 3, 列: 24)
<DOUBLE, double> (行: 4, 列: 5)
<IDENTIFIER, sci_num> (行: 4, 列: 12)
<ASSIGN, => (行: 4, 列: 21)
<DOUBLE_CONST, 1.23e-5> [值: 1.23e-05] (行: 4, 列: 23)
<SEMICOLON, ;> (行: 4, 列: 30)
<INT, int> (行: 5, 列: 5)
<IDENTIFIER, hex_value> (行: 5, 列: 9)
<ASSIGN, => (行: 5, 列: 20)
<INT_CONST, 0xFF> [值: 255] (行: 5, 列: 22)
<SEMICOLON, ;> (行: 5, 列: 26)
<CHAR, char> (行: 6, 列: 5)
<MULTIPLY, *> (行: 6, 列: 10)
<IDENTIFIER, message> (行: 6, 列: 11)
<ASSIGN, => (行: 6, 列: 19)
<STRING_CONST, "Hello, World!"> (行: 6, 列: 21)
<SEMICOLON, ;> (行: 6, 列: 36)
<RBRACE, }> (行: 7, 列: 1)
```

### 5.2 NFA状态转换图

```
=== NFA 状态转换图 ===
状态数量: 2
初始状态: 0
终态集合: {1}

状态转换:
起始状态 -> 目标状态 [符号]
--------------------------------
    0    ->    1     ['a']
    0    ->    1     ['b']
    ...
    0    ->    1     ['z']
    0    ->    1     ['A']
    ...
    0    ->    1     ['Z']
    0    ->    1     ['_']
    1    ->    1     ['a']
    ...
    1    ->    1     ['0']
    ...
    1    ->    1     ['9']
    1    ->    1     ['_']
```

### 5.3 DFA状态转换图

```
=== DFA 状态转换图 ===
状态数量: 2
初始状态: 0
终态集合: {1}

状态转换:
起始状态 -> 目标状态 [符号]
--------------------------------
（与NFA相同，因为该NFA已经是确定化的）
```

### 5.4 最简DFA状态转换矩阵

```
=== DFA 状态转换矩阵 ===
初始状态: 0
终态集合: {1}

状态\符号 |  a  |  z  |  A  |  Z  |  0  |  9  |  _  |
----------|-----|-----|-----|-----|-----|-----|-----|
    0     |  1  |  1  |  1  |  1  |  -  |  -  |  1  |
    1     |  1  |  1  |  1  |  1  |  1  |  1  |  1  |
```

## 六、测试用例

### 6.1 标识符测试（test_simple.c）
```c
int a;
int b123;
int _test;
int my_variable_name;
int __private;
int CamelCase;
```
**结果**：所有标识符均被正确识别。

### 6.2 数字常量测试（test_numbers.c）
```c
int dec = 123;           // 10进制
int hex = 0xFF;          // 16进制
double f = 3.14159;      // 浮点数
double e = 1.23e-5;      // 科学计数法
```
**结果**：
- 10进制整数：正确识别，值为123
- 16进制整数：正确识别，值为255
- 浮点数：正确识别，值为3.14159
- 科学计数法：正确识别，值为0.0000123

### 6.3 字符串测试（test_strings.c）
```c
char c = 'a';                    // 字符常量
char *s = "Hello";               // 字符串常量
char *e = "Line1\nLine2\t";      // 转义字符
```
**结果**：所有字符和字符串常量均被正确识别，转义字符处理正确。

### 6.4 注释测试（test_comments.c）
```c
// 单行注释
int a;
/* 多行注释 */
int b;
```
**结果**：单行和多行注释均被正确跳过，不产生Token。

## 七、代码注释质量

本项目所有源文件都包含详细的中文注释：
1. **文件头注释**：说明文件的功能和用途
2. **函数注释**：说明函数的功能、参数和返回值
3. **算法注释**：关键算法步骤都有详细说明
4. **数据结构注释**：每个结构体和重要变量都有说明

示例：
```c
/**
 * 读取数字常量（整数或浮点数）
 * 支持：
 * - 10进制整数：123, 456
 * - 16进制整数：0x1A, 0xFF
 * - 浮点数：123.456
 * - 科学计数法：1.23e10, 1.23E-5
 * @param lexer 词法分析器指针
 * @return Token指针
 */
Token *read_number(Lexer *lexer) {
    // 实现代码...
}
```

## 八、实验总结

### 8.1 完成情况

本实验完整实现了C0编译器的要求：

1. ✅ **词法分析器**
   - 识别所有要求的词法元素
   - 支持关键字、标识符（含下划线）
   - 支持10进制和16进制整数
   - 支持浮点数和科学计数法
   - 支持字符串和字符常量
   - 支持单行和多行注释
   - 能够检测和报告词法错误

2. ✅ **NFA/DFA构造**
   - 实现标识符正规式的NFA
   - 实现NFA到DFA的转换（子集构造法）
   - 实现DFA的最简化（状态等价类划分）
   - 能够输出状态转换图和转换矩阵

3. ✅ **代码质量**
   - 所有代码都有详细的中文注释
   - 模块化设计，结构清晰
   - 使用标准C99，无外部依赖
   - 提供Makefile便于编译

### 8.2 技术要点

1. **有限状态自动机**：词法分析器的核心是状态机，通过状态转换识别不同类型的Token。

2. **子集构造法**：将NFA转换为DFA的标准算法，通过计算ε闭包和move操作实现。

3. **状态等价类划分**：DFA最简化的核心思想，通过迭代细化划分合并等价状态。

4. **错误处理**：能够检测并报告词法错误，如未结束的字符串、非法字符等。

### 8.3 改进方向

1. **扩展Token类型**：可以添加更多的运算符（如三目运算符?:）和关键字。

2. **优化DFA最简化**：可以实现更高效的Hopcroft算法。

3. **错误恢复**：增强错误处理，在遇到错误后能够继续分析。

4. **Unicode支持**：当前只支持ASCII字符，可以扩展支持Unicode标识符。

### 8.4 心得体会

通过本次实验，深入理解了编译原理中词法分析的基本原理和实现方法。词法分析是编译器的第一个阶段，虽然概念相对简单，但实现起来需要考虑很多细节，如转义字符处理、注释跳过、错误恢复等。

NFA和DFA的构造与转换是自动机理论的核心内容，通过编程实现这些算法，对理论知识有了更深刻的理解。特别是子集构造法，虽然理论上简单，但实现时需要仔细处理状态集合的比较和查找。

整体而言，本实验达到了预期目标，实现了一个功能完整、代码规范的C0编译器词法分析模块。

## 九、参考资料

1. 《编译原理》（龙书）- Alfred V. Aho等著
2. 《编译原理及实践》- Kenneth C. Louden著
3. C99标准文档
4. 正规式与有限自动机相关理论

---

**实验完成日期**：2025年10月

**代码仓库**：https://github.com/HeavySnowJakarta/c0-compiler
