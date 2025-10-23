/**
 * token.c - Token操作函数实现
 * 
 * 实现Token的创建、销毁和辅助函数
 */

#define _POSIX_C_SOURCE 200809L
#include "token.h"

/**
 * 创建一个新的Token
 * @param type Token类型
 * @param lexeme Token的词素(原始字符串)
 * @param line 行号
 * @param column 列号
 * @return 新创建的Token指针
 */
Token *create_token(TokenType type, const char *lexeme, int line, int column) {
    Token *token = (Token *)malloc(sizeof(Token));
    if (!token) {
        fprintf(stderr, "内存分配失败: create_token\n");
        exit(1);
    }
    
    token->type = type;
    token->lexeme = strdup(lexeme);
    token->line = line;
    token->column = column;
    
    return token;
}

/**
 * 释放Token占用的内存
 * @param token 要释放的Token指针
 */
void free_token(Token *token) {
    if (token) {
        if (token->lexeme) {
            free(token->lexeme);
        }
        free(token);
    }
}

/**
 * 将Token类型转换为字符串表示
 * @param type Token类型
 * @return Token类型的字符串表示
 */
const char *token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_CONST: return "CONST";
        case TOKEN_INT: return "INT";
        case TOKEN_DOUBLE: return "DOUBLE";
        case TOKEN_CHAR: return "CHAR";
        case TOKEN_VOID: return "VOID";
        case TOKEN_IF: return "IF";
        case TOKEN_ELSE: return "ELSE";
        case TOKEN_WHILE: return "WHILE";
        case TOKEN_FOR: return "FOR";
        case TOKEN_RETURN: return "RETURN";
        case TOKEN_BREAK: return "BREAK";
        case TOKEN_CONTINUE: return "CONTINUE";
        case TOKEN_STRUCT: return "STRUCT";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_INT_CONST: return "INT_CONST";
        case TOKEN_DOUBLE_CONST: return "DOUBLE_CONST";
        case TOKEN_CHAR_CONST: return "CHAR_CONST";
        case TOKEN_STRING_CONST: return "STRING_CONST";
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_MULTIPLY: return "MULTIPLY";
        case TOKEN_DIVIDE: return "DIVIDE";
        case TOKEN_MODULO: return "MODULO";
        case TOKEN_ASSIGN: return "ASSIGN";
        case TOKEN_EQ: return "EQ";
        case TOKEN_NE: return "NE";
        case TOKEN_LT: return "LT";
        case TOKEN_LE: return "LE";
        case TOKEN_GT: return "GT";
        case TOKEN_GE: return "GE";
        case TOKEN_AND: return "AND";
        case TOKEN_OR: return "OR";
        case TOKEN_NOT: return "NOT";
        case TOKEN_SEMICOLON: return "SEMICOLON";
        case TOKEN_COMMA: return "COMMA";
        case TOKEN_LPAREN: return "LPAREN";
        case TOKEN_RPAREN: return "RPAREN";
        case TOKEN_LBRACE: return "LBRACE";
        case TOKEN_RBRACE: return "RBRACE";
        case TOKEN_LBRACKET: return "LBRACKET";
        case TOKEN_RBRACKET: return "RBRACKET";
        case TOKEN_EOF: return "EOF";
        case TOKEN_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

/**
 * 查找关键字
 * @param str 要查找的字符串
 * @return 如果是关键字，返回对应的TokenType，否则返回TOKEN_IDENTIFIER
 */
TokenType lookup_keyword(const char *str) {
    for (int i = 0; keywords[i].keyword != NULL; i++) {
        if (strcmp(str, keywords[i].keyword) == 0) {
            return keywords[i].type;
        }
    }
    return TOKEN_IDENTIFIER;
}
