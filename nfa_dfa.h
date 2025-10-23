/**
 * nfa_dfa.h - NFA和DFA数据结构和算法头文件
 * 
 * 实现正规式到NFA的转换、NFA到DFA的确定化、DFA的最简化
 */

#ifndef NFA_DFA_H
#define NFA_DFA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_STATES 100      // 最大状态数
#define MAX_ALPHABET 128    // 字母表最大大小
#define EPSILON -1          // ε转换标记

/* NFA状态转换结构 */
typedef struct {
    int from_state;         // 起始状态
    int to_state;           // 目标状态
    int symbol;             // 转换符号（EPSILON表示ε转换）
} NFATransition;

/* NFA结构 */
typedef struct {
    int num_states;         // 状态数量
    int start_state;        // 初始状态
    bool final_states[MAX_STATES]; // 终态集合
    NFATransition transitions[MAX_STATES * MAX_ALPHABET]; // 转换集合
    int num_transitions;    // 转换数量
} NFA;

/* DFA状态转换表 */
typedef struct {
    int transition[MAX_STATES][MAX_ALPHABET]; // 转换表：[状态][符号] -> 目标状态
    int num_states;         // 状态数量
    int start_state;        // 初始状态
    bool final_states[MAX_STATES]; // 终态集合
    int alphabet_size;      // 字母表大小
    char alphabet[MAX_ALPHABET]; // 字母表
} DFA;

/* 状态集合（用于子集构造法） */
typedef struct {
    int states[MAX_STATES]; // 状态列表
    int count;              // 状态数量
} StateSet;

/* NFA操作函数 */
NFA *create_nfa_for_identifier();
void print_nfa(NFA *nfa);
void free_nfa(NFA *nfa);
StateSet epsilon_closure(NFA *nfa, StateSet states);
StateSet move(NFA *nfa, StateSet states, int symbol);

/* DFA操作函数 */
DFA *nfa_to_dfa(NFA *nfa);
DFA *minimize_dfa(DFA *dfa);
void print_dfa(DFA *dfa);
void print_dfa_transition_matrix(DFA *dfa);
void free_dfa(DFA *dfa);

/* 辅助函数 */
bool state_set_contains(StateSet *set, int state);
void state_set_add(StateSet *set, int state);
bool state_set_equal(StateSet *a, StateSet *b);
int find_state_set_index(StateSet *sets, int num_sets, StateSet *target);

#endif /* NFA_DFA_H */
