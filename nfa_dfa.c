/**
 * nfa_dfa.c - NFA和DFA算法实现
 * 
 * 实现以下功能：
 * 1. 构造标识符的NFA（字母开头，由字母和数字构成）
 * 2. NFA到DFA的转换（子集构造法）
 * 3. DFA的最简化（Hopcroft算法）
 * 4. 状态转换图的输出
 */

#include "nfa_dfa.h"

/**
 * 创建标识符的NFA
 * 正规式：letter(letter|digit)*
 * letter = [a-zA-Z_]
 * digit = [0-9]
 * 
 * 状态设计：
 * 状态0: 初始状态
 * 状态1: 读取了一个字母（也是终态）
 * 
 * @return NFA指针
 */
NFA *create_nfa_for_identifier() {
    NFA *nfa = (NFA *)malloc(sizeof(NFA));
    if (!nfa) {
        fprintf(stderr, "内存分配失败: create_nfa_for_identifier\n");
        exit(1);
    }
    
    // 初始化NFA
    nfa->num_states = 2;
    nfa->start_state = 0;
    nfa->num_transitions = 0;
    
    // 初始化终态集合（状态1是终态）
    for (int i = 0; i < MAX_STATES; i++) {
        nfa->final_states[i] = false;
    }
    nfa->final_states[1] = true;
    
    // 添加转换：状态0 -> 状态1（字母）
    // a-z
    for (char c = 'a'; c <= 'z'; c++) {
        nfa->transitions[nfa->num_transitions].from_state = 0;
        nfa->transitions[nfa->num_transitions].to_state = 1;
        nfa->transitions[nfa->num_transitions].symbol = c;
        nfa->num_transitions++;
    }
    // A-Z
    for (char c = 'A'; c <= 'Z'; c++) {
        nfa->transitions[nfa->num_transitions].from_state = 0;
        nfa->transitions[nfa->num_transitions].to_state = 1;
        nfa->transitions[nfa->num_transitions].symbol = c;
        nfa->num_transitions++;
    }
    // 下划线
    nfa->transitions[nfa->num_transitions].from_state = 0;
    nfa->transitions[nfa->num_transitions].to_state = 1;
    nfa->transitions[nfa->num_transitions].symbol = '_';
    nfa->num_transitions++;
    
    // 添加转换：状态1 -> 状态1（字母或数字）
    // a-z
    for (char c = 'a'; c <= 'z'; c++) {
        nfa->transitions[nfa->num_transitions].from_state = 1;
        nfa->transitions[nfa->num_transitions].to_state = 1;
        nfa->transitions[nfa->num_transitions].symbol = c;
        nfa->num_transitions++;
    }
    // A-Z
    for (char c = 'A'; c <= 'Z'; c++) {
        nfa->transitions[nfa->num_transitions].from_state = 1;
        nfa->transitions[nfa->num_transitions].to_state = 1;
        nfa->transitions[nfa->num_transitions].symbol = c;
        nfa->num_transitions++;
    }
    // 0-9
    for (char c = '0'; c <= '9'; c++) {
        nfa->transitions[nfa->num_transitions].from_state = 1;
        nfa->transitions[nfa->num_transitions].to_state = 1;
        nfa->transitions[nfa->num_transitions].symbol = c;
        nfa->num_transitions++;
    }
    // 下划线
    nfa->transitions[nfa->num_transitions].from_state = 1;
    nfa->transitions[nfa->num_transitions].to_state = 1;
    nfa->transitions[nfa->num_transitions].symbol = '_';
    nfa->num_transitions++;
    
    return nfa;
}

/**
 * 打印NFA状态转换图
 * @param nfa NFA指针
 */
void print_nfa(NFA *nfa) {
    printf("\n=== NFA 状态转换图 ===\n");
    printf("状态数量: %d\n", nfa->num_states);
    printf("初始状态: %d\n", nfa->start_state);
    
    printf("终态集合: {");
    bool first = true;
    for (int i = 0; i < nfa->num_states; i++) {
        if (nfa->final_states[i]) {
            if (!first) printf(", ");
            printf("%d", i);
            first = false;
        }
    }
    printf("}\n");
    
    printf("\n状态转换:\n");
    printf("起始状态 -> 目标状态 [符号]\n");
    printf("--------------------------------\n");
    
    for (int i = 0; i < nfa->num_transitions; i++) {
        NFATransition *t = &nfa->transitions[i];
        printf("    %d    ->    %d     ", t->from_state, t->to_state);
        if (t->symbol == EPSILON) {
            printf("[ε]\n");
        } else if (t->symbol >= 32 && t->symbol <= 126) {
            printf("['%c']\n", t->symbol);
        } else {
            printf("[ASCII:%d]\n", t->symbol);
        }
    }
    printf("\n");
}

/**
 * 释放NFA内存
 * @param nfa NFA指针
 */
void free_nfa(NFA *nfa) {
    if (nfa) {
        free(nfa);
    }
}

/**
 * 检查状态集合是否包含某个状态
 * @param set 状态集合
 * @param state 状态
 * @return 是否包含
 */
bool state_set_contains(StateSet *set, int state) {
    for (int i = 0; i < set->count; i++) {
        if (set->states[i] == state) {
            return true;
        }
    }
    return false;
}

/**
 * 向状态集合添加状态
 * @param set 状态集合
 * @param state 要添加的状态
 */
void state_set_add(StateSet *set, int state) {
    if (!state_set_contains(set, state) && set->count < MAX_STATES) {
        set->states[set->count++] = state;
    }
}

/**
 * 比较两个状态集合是否相等
 * @param a 状态集合a
 * @param b 状态集合b
 * @return 是否相等
 */
bool state_set_equal(StateSet *a, StateSet *b) {
    if (a->count != b->count) {
        return false;
    }
    
    for (int i = 0; i < a->count; i++) {
        if (!state_set_contains(b, a->states[i])) {
            return false;
        }
    }
    return true;
}

/**
 * 计算ε闭包
 * @param nfa NFA指针
 * @param states 状态集合
 * @return ε闭包后的状态集合
 */
StateSet epsilon_closure(NFA *nfa, StateSet states) {
    StateSet closure = states;
    bool changed = true;
    
    // 迭代添加通过ε转换可达的状态
    while (changed) {
        changed = false;
        int old_count = closure.count;
        
        for (int i = 0; i < closure.count; i++) {
            int state = closure.states[i];
            
            // 查找从当前状态出发的ε转换
            for (int j = 0; j < nfa->num_transitions; j++) {
                if (nfa->transitions[j].from_state == state &&
                    nfa->transitions[j].symbol == EPSILON) {
                    state_set_add(&closure, nfa->transitions[j].to_state);
                }
            }
        }
        
        if (closure.count > old_count) {
            changed = true;
        }
    }
    
    return closure;
}

/**
 * 计算move操作
 * 从状态集合出发，经过某个符号到达的状态集合
 * @param nfa NFA指针
 * @param states 状态集合
 * @param symbol 转换符号
 * @return 目标状态集合
 */
StateSet move(NFA *nfa, StateSet states, int symbol) {
    StateSet result;
    result.count = 0;
    
    for (int i = 0; i < states.count; i++) {
        int state = states.states[i];
        
        // 查找从当前状态出发，经过symbol的转换
        for (int j = 0; j < nfa->num_transitions; j++) {
            if (nfa->transitions[j].from_state == state &&
                nfa->transitions[j].symbol == symbol) {
                state_set_add(&result, nfa->transitions[j].to_state);
            }
        }
    }
    
    return result;
}

/**
 * 在状态集合列表中查找某个状态集合
 * @param sets 状态集合列表
 * @param num_sets 集合数量
 * @param target 目标集合
 * @return 索引（找不到返回-1）
 */
int find_state_set_index(StateSet *sets, int num_sets, StateSet *target) {
    for (int i = 0; i < num_sets; i++) {
        if (state_set_equal(&sets[i], target)) {
            return i;
        }
    }
    return -1;
}

/**
 * NFA到DFA的转换（子集构造法）
 * @param nfa NFA指针
 * @return DFA指针
 */
DFA *nfa_to_dfa(NFA *nfa) {
    DFA *dfa = (DFA *)malloc(sizeof(DFA));
    if (!dfa) {
        fprintf(stderr, "内存分配失败: nfa_to_dfa\n");
        exit(1);
    }
    
    // 初始化DFA
    for (int i = 0; i < MAX_STATES; i++) {
        for (int j = 0; j < MAX_ALPHABET; j++) {
            dfa->transition[i][j] = -1; // -1表示没有转换
        }
        dfa->final_states[i] = false;
    }
    
    // 构建字母表（字母和数字、下划线）
    dfa->alphabet_size = 0;
    for (char c = 'a'; c <= 'z'; c++) {
        dfa->alphabet[dfa->alphabet_size++] = c;
    }
    for (char c = 'A'; c <= 'Z'; c++) {
        dfa->alphabet[dfa->alphabet_size++] = c;
    }
    for (char c = '0'; c <= '9'; c++) {
        dfa->alphabet[dfa->alphabet_size++] = c;
    }
    dfa->alphabet[dfa->alphabet_size++] = '_';
    
    // 状态集合列表（DFA的每个状态对应NFA的一个状态集合）
    StateSet dfa_states[MAX_STATES];
    int num_dfa_states = 0;
    
    // 计算初始状态的ε闭包
    StateSet initial_set;
    initial_set.count = 1;
    initial_set.states[0] = nfa->start_state;
    initial_set = epsilon_closure(nfa, initial_set);
    
    dfa_states[num_dfa_states++] = initial_set;
    dfa->start_state = 0;
    
    // 工作队列
    int unmarked[MAX_STATES];
    int unmarked_count = 1;
    unmarked[0] = 0;
    
    // 子集构造算法
    while (unmarked_count > 0) {
        // 取出一个未标记的状态
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
                    // 新状态
                    next_dfa_state = num_dfa_states;
                    dfa_states[num_dfa_states++] = next_set;
                    unmarked[unmarked_count++] = next_dfa_state;
                }
                
                // 添加转换
                dfa->transition[current_dfa_state][(int)symbol] = next_dfa_state;
            }
        }
    }
    
    dfa->num_states = num_dfa_states;
    
    // 确定终态（包含NFA终态的DFA状态）
    for (int i = 0; i < num_dfa_states; i++) {
        for (int j = 0; j < dfa_states[i].count; j++) {
            if (nfa->final_states[dfa_states[i].states[j]]) {
                dfa->final_states[i] = true;
                break;
            }
        }
    }
    
    return dfa;
}

/**
 * DFA的最简化（状态等价类划分）
 * 使用Hopcroft算法的简化版本
 * @param dfa 原DFA
 * @return 最简化的DFA
 */
DFA *minimize_dfa(DFA *dfa) {
    // 简化实现：将终态和非终态作为初始划分
    int partition[MAX_STATES];     // 每个状态所属的划分
    int num_partitions = 2;        // 划分数量
    
    // 初始划分：0 - 非终态，1 - 终态
    for (int i = 0; i < dfa->num_states; i++) {
        partition[i] = dfa->final_states[i] ? 1 : 0;
    }
    
    // 迭代细化划分
    bool changed = true;
    while (changed) {
        changed = false;
        int new_partition[MAX_STATES];
        memcpy(new_partition, partition, sizeof(partition));
        int next_partition_id = num_partitions;
        
        // 对每个划分
        for (int p = 0; p < num_partitions; p++) {
            // 收集该划分中的状态
            int states_in_partition[MAX_STATES];
            int count = 0;
            for (int i = 0; i < dfa->num_states; i++) {
                if (partition[i] == p) {
                    states_in_partition[count++] = i;
                }
            }
            
            if (count <= 1) continue;
            
            // 检查这些状态是否可区分
            for (int i = 1; i < count; i++) {
                int s1 = states_in_partition[0];
                int s2 = states_in_partition[i];
                bool distinguishable = false;
                
                // 检查所有输入符号
                for (int j = 0; j < dfa->alphabet_size; j++) {
                    char symbol = dfa->alphabet[j];
                    int next1 = dfa->transition[s1][(int)symbol];
                    int next2 = dfa->transition[s2][(int)symbol];
                    
                    if ((next1 == -1) != (next2 == -1)) {
                        distinguishable = true;
                        break;
                    }
                    
                    if (next1 != -1 && next2 != -1 && 
                        partition[next1] != partition[next2]) {
                        distinguishable = true;
                        break;
                    }
                }
                
                if (distinguishable) {
                    new_partition[s2] = next_partition_id++;
                    changed = true;
                }
            }
        }
        
        memcpy(partition, new_partition, sizeof(partition));
        num_partitions = next_partition_id;
    }
    
    // 构造最简DFA
    DFA *min_dfa = (DFA *)malloc(sizeof(DFA));
    if (!min_dfa) {
        fprintf(stderr, "内存分配失败: minimize_dfa\n");
        exit(1);
    }
    
    // 初始化
    for (int i = 0; i < MAX_STATES; i++) {
        for (int j = 0; j < MAX_ALPHABET; j++) {
            min_dfa->transition[i][j] = -1;
        }
        min_dfa->final_states[i] = false;
    }
    
    min_dfa->num_states = num_partitions;
    min_dfa->start_state = partition[dfa->start_state];
    min_dfa->alphabet_size = dfa->alphabet_size;
    memcpy(min_dfa->alphabet, dfa->alphabet, dfa->alphabet_size);
    
    // 建立转换
    for (int i = 0; i < dfa->num_states; i++) {
        int p = partition[i];
        
        if (dfa->final_states[i]) {
            min_dfa->final_states[p] = true;
        }
        
        for (int j = 0; j < dfa->alphabet_size; j++) {
            char symbol = dfa->alphabet[j];
            int next = dfa->transition[i][(int)symbol];
            if (next != -1) {
                min_dfa->transition[p][(int)symbol] = partition[next];
            }
        }
    }
    
    return min_dfa;
}

/**
 * 打印DFA状态转换图
 * @param dfa DFA指针
 */
void print_dfa(DFA *dfa) {
    printf("\n=== DFA 状态转换图 ===\n");
    printf("状态数量: %d\n", dfa->num_states);
    printf("初始状态: %d\n", dfa->start_state);
    
    printf("终态集合: {");
    bool first = true;
    for (int i = 0; i < dfa->num_states; i++) {
        if (dfa->final_states[i]) {
            if (!first) printf(", ");
            printf("%d", i);
            first = false;
        }
    }
    printf("}\n");
    
    printf("\n状态转换:\n");
    printf("起始状态 -> 目标状态 [符号]\n");
    printf("--------------------------------\n");
    
    for (int i = 0; i < dfa->num_states; i++) {
        for (int j = 0; j < dfa->alphabet_size; j++) {
            char symbol = dfa->alphabet[j];
            int next = dfa->transition[i][(int)symbol];
            if (next != -1) {
                printf("    %d    ->    %d     ['%c']\n", i, next, symbol);
            }
        }
    }
    printf("\n");
}

/**
 * 打印DFA状态转换矩阵
 * @param dfa DFA指针
 */
void print_dfa_transition_matrix(DFA *dfa) {
    printf("\n=== DFA 状态转换矩阵 ===\n");
    printf("初始状态: %d\n", dfa->start_state);
    
    printf("终态集合: {");
    bool first = true;
    for (int i = 0; i < dfa->num_states; i++) {
        if (dfa->final_states[i]) {
            if (!first) printf(", ");
            printf("%d", i);
            first = false;
        }
    }
    printf("}\n\n");
    
    // 打印表头
    printf("状态\\符号 |");
    // 只显示部分有代表性的符号
    printf("  a  |  z  |  A  |  Z  |  0  |  9  |  _  |\n");
    printf("----------|-----|-----|-----|-----|-----|-----|-----|\n");
    
    // 打印每个状态的转换
    for (int i = 0; i < dfa->num_states; i++) {
        printf("    %d     |", i);
        char test_symbols[] = {'a', 'z', 'A', 'Z', '0', '9', '_'};
        for (int j = 0; j < 7; j++) {
            int next = dfa->transition[i][(int)test_symbols[j]];
            if (next != -1) {
                printf("  %d  |", next);
            } else {
                printf("  -  |");
            }
        }
        printf("\n");
    }
    printf("\n");
}

/**
 * 释放DFA内存
 * @param dfa DFA指针
 */
void free_dfa(DFA *dfa) {
    if (dfa) {
        free(dfa);
    }
}
