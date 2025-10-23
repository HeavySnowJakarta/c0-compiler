/**
 * lexer.c - 词法分析器实现
 * 
 * 实现词法分析的核心功能，包括：
 * - Token扫描和识别
 * - 关键字识别
 * - 标识符识别（支持下划线）
 * - 整型常量识别（10进制和16进制）
 * - 浮点常量识别（支持科学计数法）
 * - 字符串常量识别
 * - 注释识别（单行和多行注释）
 * - 词法错误处理
 */

#include "lexer.h"

/**
 * 创建词法分析器
 * @param source 源代码字符串
 * @return 新创建的词法分析器指针
 */
Lexer *create_lexer(const char *source) {
    Lexer *lexer = (Lexer *)malloc(sizeof(Lexer));
    if (!lexer) {
        fprintf(stderr, "内存分配失败: create_lexer\n");
        exit(1);
    }
    
    lexer->source = source;
    lexer->pos = 0;
    lexer->length = strlen(source);
    lexer->line = 1;
    lexer->column = 1;
    lexer->current_char = source[0];
    
    return lexer;
}

/**
 * 释放词法分析器内存
 * @param lexer 词法分析器指针
 */
void free_lexer(Lexer *lexer) {
    if (lexer) {
        free(lexer);
    }
}

/**
 * 前进到下一个字符
 * @param lexer 词法分析器指针
 */
void advance(Lexer *lexer) {
    if (lexer->pos < lexer->length) {
        if (lexer->current_char == '\n') {
            lexer->line++;
            lexer->column = 1;
        } else {
            lexer->column++;
        }
        lexer->pos++;
        if (lexer->pos < lexer->length) {
            lexer->current_char = lexer->source[lexer->pos];
        } else {
            lexer->current_char = '\0';
        }
    }
}

/**
 * 查看下一个字符但不前进
 * @param lexer 词法分析器指针
 * @return 下一个字符
 */
char peek(Lexer *lexer) {
    if (lexer->pos + 1 < lexer->length) {
        return lexer->source[lexer->pos + 1];
    }
    return '\0';
}

/**
 * 跳过空白字符
 * @param lexer 词法分析器指针
 */
void skip_whitespace(Lexer *lexer) {
    while (lexer->current_char != '\0' && isspace(lexer->current_char)) {
        advance(lexer);
    }
}

/**
 * 跳过单行注释 (//)
 * @param lexer 词法分析器指针
 */
void skip_line_comment(Lexer *lexer) {
    // 跳过 '//'
    advance(lexer);
    advance(lexer);
    
    // 读取到行尾或文件结束
    while (lexer->current_char != '\0' && lexer->current_char != '\n') {
        advance(lexer);
    }
    
    // 如果是换行符，跳过它
    if (lexer->current_char == '\n') {
        advance(lexer);
    }
}

/**
 * 跳过多行注释
 * @param lexer 词法分析器指针
 */
void skip_block_comment(Lexer *lexer) {
    // 跳过 '/*'
    advance(lexer);
    advance(lexer);
    
    // 读取到 '*/' 或文件结束
    while (lexer->current_char != '\0') {
        if (lexer->current_char == '*' && peek(lexer) == '/') {
            advance(lexer); // 跳过 '*'
            advance(lexer); // 跳过 '/'
            break;
        }
        advance(lexer);
    }
}

/**
 * 读取标识符或关键字
 * 标识符：以字母或下划线开头，由字母、数字和下划线组成
 * @param lexer 词法分析器指针
 * @return Token指针
 */
Token *read_identifier(Lexer *lexer) {
    int start_line = lexer->line;
    int start_column = lexer->column;
    size_t start_pos = lexer->pos;
    
    // 读取标识符：字母、数字、下划线
    while (lexer->current_char != '\0' && 
           (isalnum(lexer->current_char) || lexer->current_char == '_')) {
        advance(lexer);
    }
    
    // 提取标识符字符串
    size_t length = lexer->pos - start_pos;
    char *identifier = (char *)malloc(length + 1);
    strncpy(identifier, lexer->source + start_pos, length);
    identifier[length] = '\0';
    
    // 检查是否为关键字
    TokenType type = lookup_keyword(identifier);
    Token *token = create_token(type, identifier, start_line, start_column);
    
    free(identifier);
    return token;
}

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
    int start_line = lexer->line;
    int start_column = lexer->column;
    size_t start_pos = lexer->pos;
    int is_float = 0;
    int is_hex = 0;
    
    // 检查是否为16进制数 (0x 或 0X)
    if (lexer->current_char == '0' && (peek(lexer) == 'x' || peek(lexer) == 'X')) {
        is_hex = 1;
        advance(lexer); // 跳过 '0'
        advance(lexer); // 跳过 'x' 或 'X'
        
        // 读取16进制数字
        while (lexer->current_char != '\0' && isxdigit(lexer->current_char)) {
            advance(lexer);
        }
    } else {
        // 读取10进制数字
        while (lexer->current_char != '\0' && isdigit(lexer->current_char)) {
            advance(lexer);
        }
        
        // 检查小数点
        if (lexer->current_char == '.' && isdigit(peek(lexer))) {
            is_float = 1;
            advance(lexer); // 跳过 '.'
            
            // 读取小数部分
            while (lexer->current_char != '\0' && isdigit(lexer->current_char)) {
                advance(lexer);
            }
        }
        
        // 检查指数部分 (e 或 E)
        if (lexer->current_char == 'e' || lexer->current_char == 'E') {
            is_float = 1;
            advance(lexer); // 跳过 'e' 或 'E'
            
            // 检查可选的正负号
            if (lexer->current_char == '+' || lexer->current_char == '-') {
                advance(lexer);
            }
            
            // 读取指数部分
            while (lexer->current_char != '\0' && isdigit(lexer->current_char)) {
                advance(lexer);
            }
        }
    }
    
    // 提取数字字符串
    size_t length = lexer->pos - start_pos;
    char *number_str = (char *)malloc(length + 1);
    strncpy(number_str, lexer->source + start_pos, length);
    number_str[length] = '\0';
    
    // 创建Token
    Token *token;
    if (is_float) {
        token = create_token(TOKEN_DOUBLE_CONST, number_str, start_line, start_column);
        token->value.double_value = atof(number_str);
    } else {
        token = create_token(TOKEN_INT_CONST, number_str, start_line, start_column);
        if (is_hex) {
            token->value.int_value = strtoll(number_str, NULL, 16);
        } else {
            token->value.int_value = atoll(number_str);
        }
    }
    
    free(number_str);
    return token;
}

/**
 * 读取字符串常量
 * 支持转义字符：\n, \t, \r, \\, \"
 * @param lexer 词法分析器指针
 * @return Token指针
 */
Token *read_string(Lexer *lexer) {
    int start_line = lexer->line;
    int start_column = lexer->column;
    size_t start_pos = lexer->pos;
    
    advance(lexer); // 跳过开头的引号
    
    // 读取字符串内容，直到遇到结束引号或文件结束
    while (lexer->current_char != '\0' && lexer->current_char != '"') {
        if (lexer->current_char == '\\') {
            advance(lexer); // 跳过转义符
            if (lexer->current_char != '\0') {
                advance(lexer); // 跳过被转义的字符
            }
        } else {
            advance(lexer);
        }
    }
    
    // 检查是否正常结束
    if (lexer->current_char != '"') {
        // 字符串未正常结束
        Token *token = create_token(TOKEN_ERROR, "未结束的字符串", 
                                   start_line, start_column);
        return token;
    }
    
    advance(lexer); // 跳过结束引号
    
    // 提取字符串（包括引号）
    size_t length = lexer->pos - start_pos;
    char *string = (char *)malloc(length + 1);
    strncpy(string, lexer->source + start_pos, length);
    string[length] = '\0';
    
    Token *token = create_token(TOKEN_STRING_CONST, string, start_line, start_column);
    
    free(string);
    return token;
}

/**
 * 读取字符常量
 * 例如：'a', '\n', '\t'
 * @param lexer 词法分析器指针
 * @return Token指针
 */
Token *read_char(Lexer *lexer) {
    int start_line = lexer->line;
    int start_column = lexer->column;
    size_t start_pos = lexer->pos;
    
    advance(lexer); // 跳过开头的单引号
    
    char char_value = '\0';
    
    if (lexer->current_char == '\\') {
        // 转义字符
        advance(lexer);
        switch (lexer->current_char) {
            case 'n': char_value = '\n'; break;
            case 't': char_value = '\t'; break;
            case 'r': char_value = '\r'; break;
            case '\\': char_value = '\\'; break;
            case '\'': char_value = '\''; break;
            case '0': char_value = '\0'; break;
            default: char_value = lexer->current_char; break;
        }
        advance(lexer);
    } else if (lexer->current_char != '\0' && lexer->current_char != '\'') {
        // 普通字符
        char_value = lexer->current_char;
        advance(lexer);
    }
    
    // 检查结束单引号
    if (lexer->current_char != '\'') {
        Token *token = create_token(TOKEN_ERROR, "未结束的字符常量", 
                                   start_line, start_column);
        return token;
    }
    
    advance(lexer); // 跳过结束单引号
    
    // 提取字符常量（包括单引号）
    size_t length = lexer->pos - start_pos;
    char *char_str = (char *)malloc(length + 1);
    strncpy(char_str, lexer->source + start_pos, length);
    char_str[length] = '\0';
    
    Token *token = create_token(TOKEN_CHAR_CONST, char_str, start_line, start_column);
    token->value.char_value = char_value;
    
    free(char_str);
    return token;
}

/**
 * 获取下一个Token
 * 主要的词法分析函数，返回下一个识别的Token
 * @param lexer 词法分析器指针
 * @return Token指针
 */
Token *get_next_token(Lexer *lexer) {
    while (lexer->current_char != '\0') {
        int start_line = lexer->line;
        int start_column = lexer->column;
        
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
        
        // 标识符或关键字（以字母或下划线开头）
        if (isalpha(lexer->current_char) || lexer->current_char == '_') {
            return read_identifier(lexer);
        }
        
        // 数字常量
        if (isdigit(lexer->current_char)) {
            return read_number(lexer);
        }
        
        // 字符串常量
        if (lexer->current_char == '"') {
            return read_string(lexer);
        }
        
        // 字符常量
        if (lexer->current_char == '\'') {
            return read_char(lexer);
        }
        
        // 运算符和分隔符
        char current = lexer->current_char;
        char next = peek(lexer);
        
        // 双字符运算符
        if (current == '=' && next == '=') {
            advance(lexer); advance(lexer);
            return create_token(TOKEN_EQ, "==", start_line, start_column);
        }
        if (current == '!' && next == '=') {
            advance(lexer); advance(lexer);
            return create_token(TOKEN_NE, "!=", start_line, start_column);
        }
        if (current == '<' && next == '=') {
            advance(lexer); advance(lexer);
            return create_token(TOKEN_LE, "<=", start_line, start_column);
        }
        if (current == '>' && next == '=') {
            advance(lexer); advance(lexer);
            return create_token(TOKEN_GE, ">=", start_line, start_column);
        }
        if (current == '&' && next == '&') {
            advance(lexer); advance(lexer);
            return create_token(TOKEN_AND, "&&", start_line, start_column);
        }
        if (current == '|' && next == '|') {
            advance(lexer); advance(lexer);
            return create_token(TOKEN_OR, "||", start_line, start_column);
        }
        
        // 单字符运算符和分隔符
        advance(lexer);
        switch (current) {
            case '+': return create_token(TOKEN_PLUS, "+", start_line, start_column);
            case '-': return create_token(TOKEN_MINUS, "-", start_line, start_column);
            case '*': return create_token(TOKEN_MULTIPLY, "*", start_line, start_column);
            case '/': return create_token(TOKEN_DIVIDE, "/", start_line, start_column);
            case '%': return create_token(TOKEN_MODULO, "%", start_line, start_column);
            case '=': return create_token(TOKEN_ASSIGN, "=", start_line, start_column);
            case '<': return create_token(TOKEN_LT, "<", start_line, start_column);
            case '>': return create_token(TOKEN_GT, ">", start_line, start_column);
            case '!': return create_token(TOKEN_NOT, "!", start_line, start_column);
            case ';': return create_token(TOKEN_SEMICOLON, ";", start_line, start_column);
            case ',': return create_token(TOKEN_COMMA, ",", start_line, start_column);
            case '(': return create_token(TOKEN_LPAREN, "(", start_line, start_column);
            case ')': return create_token(TOKEN_RPAREN, ")", start_line, start_column);
            case '{': return create_token(TOKEN_LBRACE, "{", start_line, start_column);
            case '}': return create_token(TOKEN_RBRACE, "}", start_line, start_column);
            case '[': return create_token(TOKEN_LBRACKET, "[", start_line, start_column);
            case ']': return create_token(TOKEN_RBRACKET, "]", start_line, start_column);
            default: {
                char error_msg[64];
                snprintf(error_msg, sizeof(error_msg), "非法字符: '%c'", current);
                return create_token(TOKEN_ERROR, error_msg, start_line, start_column);
            }
        }
    }
    
    // 文件结束
    return create_token(TOKEN_EOF, "", lexer->line, lexer->column);
}

/**
 * 打印Token信息（二元组形式）
 * @param token Token指针
 */
void print_token(Token *token) {
    printf("<%s, %s>", token_type_to_string(token->type), token->lexeme);
    
    // 对于常量，额外打印值
    if (token->type == TOKEN_INT_CONST) {
        printf(" [值: %lld]", token->value.int_value);
    } else if (token->type == TOKEN_DOUBLE_CONST) {
        printf(" [值: %g]", token->value.double_value);
    } else if (token->type == TOKEN_CHAR_CONST) {
        printf(" [值: '%c']", token->value.char_value);
    }
    
    printf(" (行: %d, 列: %d)\n", token->line, token->column);
}
