# Makefile - arvore-binaria-busca-c
# Uso: make | make run | make test | make test SANITIZE=1 | make clean

CC      ?= cc
CFLAGS  = -std=c11 -Wall -Wextra -Wpedantic -O2
CPPFLAGS += -Iinclude
LDFLAGS ?=

# Com SANITIZE=1 os binarios sao instrumentados com AddressSanitizer e
# UndefinedBehaviorSanitizer (vazamentos, acessos invalidos e UB abortam).
ifeq ($(SANITIZE),1)
CFLAGS  += -fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer -g
LDFLAGS += -fsanitize=address,undefined
endif

BUILD   := build
LIB_SRC := src/arvore.c
HEADER  := include/arvore.h
DEMO    := $(BUILD)/demo
TESTES  := $(BUILD)/test_arvore

.PHONY: all run test clean

all: $(DEMO) $(TESTES)

$(BUILD):
	mkdir -p $@

$(DEMO): $(LIB_SRC) src/main.c $(HEADER) | $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LIB_SRC) src/main.c -o $@ $(LDFLAGS)

$(TESTES): $(LIB_SRC) tests/test_arvore.c $(HEADER) | $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LIB_SRC) tests/test_arvore.c -o $@ $(LDFLAGS)

run: $(DEMO)
	./$(DEMO)

test: $(TESTES)
	./$(TESTES)

clean:
	rm -rf $(BUILD)
