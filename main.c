/**
 * main.c - C0编译器主程序
 * 
 * 实现命令行参数解析和各个编译阶段的调用
 * 
 * 使用方法：
 *   ./c0compiler -l <source_file>          # 词法分析
 *   ./c0compiler -n                        # 显示NFA
 *   ./c0compiler -d                        # 显示DFA
 *   ./c0compiler -m                        # 显示最简DFA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "lexer.h"
#include "nfa_dfa.h"

/**
 * 打印使用说明
 */
void print_usage(const char *program_name) {
    printf("C0编译器 - 词法分析和自动机工具\n\n");
    printf("使用方法:\n");
    printf("  %s -l <source_file>    词法分析：输出Token序列\n", program_name);
    printf("  %s -n                  显示标识符NFA状态转换图\n", program_name);
    printf("  %s -d                  显示标识符DFA状态转换图\n", program_name);
    printf("  %s -m                  显示最简化DFA状态转换图和转换矩阵\n", program_name);
    printf("  %s -h                  显示帮助信息\n\n", program_name);
    printf("示例:\n");
    printf("  %s -l test.c           # 对test.c进行词法分析\n", program_name);
    printf("  %s -n                  # 显示NFA\n", program_name);
    printf("  %s -d                  # 显示DFA\n", program_name);
    printf("  %s -m                  # 显示最简DFA\n\n", program_name);
}

/**
 * 读取源文件内容
 * @param filename 文件名
 * @return 文件内容字符串（需要调用者释放）
 */
char *read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "错误: 无法打开文件 '%s'\n", filename);
        return NULL;
    }
    
    // 获取文件大小
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // 分配内存并读取文件
    char *content = (char *)malloc(file_size + 1);
    if (!content) {
        fprintf(stderr, "错误: 内存分配失败\n");
        fclose(file);
        return NULL;
    }
    
    size_t bytes_read = fread(content, 1, file_size, file);
    content[bytes_read] = '\0';
    
    fclose(file);
    return content;
}

/**
 * 执行词法分析
 * @param filename 源文件名
 */
void perform_lexical_analysis(const char *filename) {
    printf("\n========================================\n");
    printf("          词法分析结果\n");
    printf("========================================\n\n");
    printf("源文件: %s\n\n", filename);
    
    // 读取源文件
    char *source = read_file(filename);
    if (!source) {
        return;
    }
    
    printf("源代码:\n");
    printf("----------------------------------------\n");
    printf("%s\n", source);
    printf("----------------------------------------\n\n");
    
    // 创建词法分析器
    Lexer *lexer = create_lexer(source);
    
    printf("Token序列（二元组形式）:\n");
    printf("========================================\n");
    
    int token_count = 0;
    int error_count = 0;
    
    // 获取所有Token
    while (1) {
        Token *token = get_next_token(lexer);
        
        if (token->type == TOKEN_EOF) {
            printf("\n<EOF, > (行: %d, 列: %d)\n", token->line, token->column);
            free_token(token);
            break;
        }
        
        // 打印Token
        print_token(token);
        token_count++;
        
        if (token->type == TOKEN_ERROR) {
            error_count++;
        }
        
        free_token(token);
    }
    
    printf("\n========================================\n");
    printf("分析完成！\n");
    printf("共识别 %d 个Token\n", token_count);
    if (error_count > 0) {
        printf("发现 %d 个词法错误\n", error_count);
    }
    printf("========================================\n\n");
    
    // 清理
    free_lexer(lexer);
    free(source);
}

/**
 * 显示NFA
 */
void show_nfa() {
    printf("\n========================================\n");
    printf("    标识符正规式的NFA\n");
    printf("========================================\n");
    printf("\n正规式: letter(letter|digit)*\n");
    printf("其中:\n");
    printf("  letter = [a-zA-Z_]\n");
    printf("  digit  = [0-9]\n\n");
    
    NFA *nfa = create_nfa_for_identifier();
    print_nfa(nfa);
    
    printf("说明:\n");
    printf("- 状态0: 初始状态\n");
    printf("- 状态1: 终态（接受状态）\n");
    printf("- 从状态0到状态1: 必须读取一个字母\n");
    printf("- 状态1的自环: 可以继续读取字母或数字\n\n");
    
    free_nfa(nfa);
}

/**
 * 显示DFA
 */
void show_dfa() {
    printf("\n========================================\n");
    printf("    NFA确定化后的DFA\n");
    printf("========================================\n");
    printf("\n使用子集构造法将NFA转换为DFA\n\n");
    
    NFA *nfa = create_nfa_for_identifier();
    DFA *dfa = nfa_to_dfa(nfa);
    
    print_dfa(dfa);
    
    printf("说明:\n");
    printf("- 通过子集构造法消除了非确定性\n");
    printf("- 每个DFA状态对应NFA状态的一个子集\n");
    printf("- 对每个输入符号，都有唯一确定的转换\n\n");
    
    free_dfa(dfa);
    free_nfa(nfa);
}

/**
 * 显示最简DFA
 */
void show_minimized_dfa() {
    printf("\n========================================\n");
    printf("    DFA最简化结果\n");
    printf("========================================\n");
    printf("\n使用状态等价类划分算法最简化DFA\n\n");
    
    NFA *nfa = create_nfa_for_identifier();
    DFA *dfa = nfa_to_dfa(nfa);
    DFA *min_dfa = minimize_dfa(dfa);
    
    print_dfa(min_dfa);
    print_dfa_transition_matrix(min_dfa);
    
    printf("说明:\n");
    printf("- 合并了等价的状态\n");
    printf("- 得到了最少状态数的DFA\n");
    printf("- 状态转换矩阵展示了部分符号的转换关系\n");
    printf("- '-' 表示该符号没有对应的转换\n\n");
    
    free_dfa(min_dfa);
    free_dfa(dfa);
    free_nfa(nfa);
}

/**
 * 主函数
 */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 0;
    }
    
    const char *option = argv[1];
    
    if (strcmp(option, "-h") == 0 || strcmp(option, "--help") == 0) {
        print_usage(argv[0]);
        return 0;
    }
    else if (strcmp(option, "-l") == 0) {
        // 词法分析
        if (argc < 3) {
            fprintf(stderr, "错误: 缺少源文件参数\n");
            fprintf(stderr, "使用方法: %s -l <source_file>\n", argv[0]);
            return 1;
        }
        perform_lexical_analysis(argv[2]);
    }
    else if (strcmp(option, "-n") == 0) {
        // 显示NFA
        show_nfa();
    }
    else if (strcmp(option, "-d") == 0) {
        // 显示DFA
        show_dfa();
    }
    else if (strcmp(option, "-m") == 0) {
        // 显示最简DFA
        show_minimized_dfa();
    }
    else {
        fprintf(stderr, "错误: 未知选项 '%s'\n", option);
        print_usage(argv[0]);
        return 1;
    }
    
    return 0;
}
