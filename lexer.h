/**
 * lexer.h - 词法分析器头文件
 * 
 * 定义词法分析器的数据结构和接口函数
 */

#ifndef LEXER_H
#define LEXER_H

#include "token.h"

/* 词法分析器状态 */
typedef enum {
    STATE_START,          // 初始状态
    STATE_IDENTIFIER,     // 标识符状态
    STATE_NUMBER,         // 数字状态
    STATE_HEX_NUMBER,     // 十六进制数字状态
    STATE_FLOAT,          // 浮点数状态
    STATE_EXPONENT,       // 指数状态
    STATE_STRING,         // 字符串状态
    STATE_CHAR,           // 字符状态
    STATE_COMMENT_LINE,   // 单行注释状态
    STATE_COMMENT_BLOCK,  // 多行注释状态
    STATE_OPERATOR,       // 运算符状态
    STATE_ERROR           // 错误状态
} LexerState;

/* 词法分析器结构 */
typedef struct {
    const char *source;   // 源代码字符串
    size_t pos;           // 当前位置
    size_t length;        // 源代码长度
    int line;             // 当前行号
    int column;           // 当前列号
    char current_char;    // 当前字符
} Lexer;

/* 词法分析器函数声明 */
Lexer *create_lexer(const char *source);
void free_lexer(Lexer *lexer);
Token *get_next_token(Lexer *lexer);
void print_token(Token *token);

/* 辅助函数声明 */
void advance(Lexer *lexer);
char peek(Lexer *lexer);
void skip_whitespace(Lexer *lexer);
Token *read_identifier(Lexer *lexer);
Token *read_number(Lexer *lexer);
Token *read_string(Lexer *lexer);
Token *read_char(Lexer *lexer);
void skip_line_comment(Lexer *lexer);
void skip_block_comment(Lexer *lexer);

#endif /* LEXER_H */
