# Makefile for C0 Compiler
# 编译C0编译器的Makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = c0compiler
OBJS = main.o token.o lexer.o nfa_dfa.o

# 默认目标：编译整个项目
all: $(TARGET)

# 链接目标文件生成可执行文件
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)
	@echo "编译完成！可执行文件: $(TARGET)"

# 编译各个源文件
main.o: main.c token.h lexer.h nfa_dfa.h
	$(CC) $(CFLAGS) -c main.c

token.o: token.c token.h
	$(CC) $(CFLAGS) -c token.c

lexer.o: lexer.c lexer.h token.h
	$(CC) $(CFLAGS) -c lexer.c

nfa_dfa.o: nfa_dfa.c nfa_dfa.h
	$(CC) $(CFLAGS) -c nfa_dfa.c

# 清理编译产物
clean:
	rm -f $(OBJS) $(TARGET)
	@echo "清理完成！"

# 清理并重新编译
rebuild: clean all

# 运行测试
test: $(TARGET)
	@echo "运行词法分析测试..."
	./$(TARGET) -l test_input.c

# 显示NFA
show-nfa: $(TARGET)
	./$(TARGET) -n

# 显示DFA
show-dfa: $(TARGET)
	./$(TARGET) -d

# 显示最简DFA
show-min-dfa: $(TARGET)
	./$(TARGET) -m

# 显示帮助信息
help:
	@echo "C0编译器 Makefile"
	@echo ""
	@echo "可用目标:"
	@echo "  make                - 编译项目"
	@echo "  make clean          - 清理编译产物"
	@echo "  make rebuild        - 清理并重新编译"
	@echo "  make test           - 运行测试"
	@echo "  make show-nfa       - 显示NFA状态转换图"
	@echo "  make show-dfa       - 显示DFA状态转换图"
	@echo "  make show-min-dfa   - 显示最简DFA状态转换图"
	@echo "  make help           - 显示此帮助信息"

.PHONY: all clean rebuild test show-nfa show-dfa show-min-dfa help
