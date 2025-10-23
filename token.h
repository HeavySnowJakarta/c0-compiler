/**
 * token.h - Token类型定义和词法分析相关数据结构
 * 
 * 本文件定义了C0编译器的Token类型、Token结构和相关枚举
 */

#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Token类型枚举 - 定义所有可能的Token类型 */
typedef enum {
    /* 关键字 (Keywords) */
    TOKEN_CONST,      // const
    TOKEN_INT,        // int
    TOKEN_DOUBLE,     // double
    TOKEN_CHAR,       // char
    TOKEN_VOID,       // void
    TOKEN_IF,         // if
    TOKEN_ELSE,       // else
    TOKEN_WHILE,      // while
    TOKEN_FOR,        // for
    TOKEN_RETURN,     // return
    TOKEN_BREAK,      // break
    TOKEN_CONTINUE,   // continue
    TOKEN_STRUCT,     // struct
    
    /* 标识符和常量 (Identifiers and Constants) */
    TOKEN_IDENTIFIER, // 标识符 (以字母或下划线开头)
    TOKEN_INT_CONST,  // 整型常量 (10进制或16进制)
    TOKEN_DOUBLE_CONST, // 双精度浮点常量 (可带指数)
    TOKEN_CHAR_CONST, // 字符常量
    TOKEN_STRING_CONST, // 字符串常量
    
    /* 运算符 (Operators) */
    TOKEN_PLUS,       // +
    TOKEN_MINUS,      // -
    TOKEN_MULTIPLY,   // *
    TOKEN_DIVIDE,     // /
    TOKEN_MODULO,     // %
    TOKEN_ASSIGN,     // =
    TOKEN_EQ,         // ==
    TOKEN_NE,         // !=
    TOKEN_LT,         // <
    TOKEN_LE,         // <=
    TOKEN_GT,         // >
    TOKEN_GE,         // >=
    TOKEN_AND,        // &&
    TOKEN_OR,         // ||
    TOKEN_NOT,        // !
    
    /* 分隔符 (Delimiters) */
    TOKEN_SEMICOLON,  // ;
    TOKEN_COMMA,      // ,
    TOKEN_LPAREN,     // (
    TOKEN_RPAREN,     // )
    TOKEN_LBRACE,     // {
    TOKEN_RBRACE,     // }
    TOKEN_LBRACKET,   // [
    TOKEN_RBRACKET,   // ]
    
    /* 特殊Token */
    TOKEN_EOF,        // 文件结束
    TOKEN_ERROR       // 词法错误
} TokenType;

/* Token结构 - 存储单个Token的信息 */
typedef struct {
    TokenType type;      // Token类型
    char *lexeme;        // Token的词素(原始字符串)
    int line;            // Token所在行号
    int column;          // Token所在列号
    
    /* 存储常量值的联合体 */
    union {
        long long int_value;    // 整型常量值
        double double_value;    // 浮点常量值
        char char_value;        // 字符常量值
    } value;
} Token;

/* 关键字表 - 用于查找关键字 */
typedef struct {
    const char *keyword;
    TokenType type;
} KeywordEntry;

/* 关键字表定义 */
static const KeywordEntry keywords[] = {
    {"const", TOKEN_CONST},
    {"int", TOKEN_INT},
    {"double", TOKEN_DOUBLE},
    {"char", TOKEN_CHAR},
    {"void", TOKEN_VOID},
    {"if", TOKEN_IF},
    {"else", TOKEN_ELSE},
    {"while", TOKEN_WHILE},
    {"for", TOKEN_FOR},
    {"return", TOKEN_RETURN},
    {"break", TOKEN_BREAK},
    {"continue", TOKEN_CONTINUE},
    {"struct", TOKEN_STRUCT},
    {NULL, TOKEN_EOF}  // 结束标记
};

/* Token操作函数声明 */
Token *create_token(TokenType type, const char *lexeme, int line, int column);
void free_token(Token *token);
const char *token_type_to_string(TokenType type);
TokenType lookup_keyword(const char *str);

#endif /* TOKEN_H */
