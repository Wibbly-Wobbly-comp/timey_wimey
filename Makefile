# Makefile para Timey Wimey - jogo em C usando Raylib.
# Compilacao recomendada no Windows: terminal MSYS2 UCRT64.

# Detectar sistema operacional.
ifdef OS
    SISTEMA := $(strip $(OS))
else
    SISTEMA := $(strip $(shell uname -s))
    ifneq ($(filter MINGW% MSYS% CYGWIN%,$(SISTEMA)),)
        SISTEMA := Windows_NT
    endif
endif

# Configuracoes basicas do projeto.
CC = gcc
CFLAGS = -g -Iinclude -std=c11 -Wall -Wextra -Wpedantic -O2

TARGET = timey_wimey
GERENCIADOR = gerenciador

ARQUIVOS_JOGO = src/main.c src/pergunta.c src/jogo.c src/personagem.c src/portal.c src/versus.c src/historia.c src/chefe.c src/loja.c
ARQUIVOS_GERENCIADOR = src/gerenciador.c src/pergunta.c

OBJETOS_JOGO = $(ARQUIVOS_JOGO:.c=.o)
OBJETOS_GERENCIADOR = $(ARQUIVOS_GERENCIADOR:.c=.o)
CABECALHOS = $(wildcard include/*.h)

# Extensao dos executaveis e comandos usados em cada plataforma.
ifeq ($(SISTEMA),Windows_NT)
    BINEXT = .exe
    EXECUTAR = ./
    RM = rm -f
else
    BINEXT =
    EXECUTAR = ./
    RM = rm -f
endif

PROJETO = $(TARGET)$(BINEXT)
EXECUTAVEL_GERENCIADOR = $(GERENCIADOR)$(BINEXT)
TESTE = testes/teste_core$(BINEXT)

# Tentar localizar a Raylib usando pkg-config primeiro.
PKG_CHECK := $(shell pkg-config --exists raylib 2>/dev/null && echo 0 || echo 1)

ifeq ($(PKG_CHECK),0)
    RAYLIB_CFLAGS := $(shell pkg-config --cflags raylib)
    RAYLIB_LIBS := $(shell pkg-config --libs raylib)
    RAYLIB_STATUS = Usando Raylib via pkg-config.
    RAYLIB_OK = sim
else
    # Caso pkg-config nao encontre a biblioteca, usar os caminhos locais.
    ifeq ($(SISTEMA),Windows_NT)
        ifneq ($(wildcard /ucrt64/include/raylib.h),)
            RAYLIB_PREFIX = /ucrt64
        else
            RAYLIB_PREFIX = /mingw64
        endif

        RAYLIB_CFLAGS = -I$(RAYLIB_PREFIX)/include
        RAYLIB_LIBS = -L$(RAYLIB_PREFIX)/lib -lraylib -lopengl32 -lgdi32 -lwinmm -lm
        RAYLIB_HEADER := $(wildcard $(RAYLIB_PREFIX)/include/raylib.h)
    else
        RAYLIB_CFLAGS = -I/usr/local/include -I/usr/include
        RAYLIB_LIBS = -L/usr/local/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
        RAYLIB_HEADER := $(firstword $(wildcard /usr/local/include/raylib.h /usr/include/raylib.h))
    endif

    RAYLIB_STATUS = Raylib nao encontrada via pkg-config. Usando caminhos locais.

    ifneq ($(RAYLIB_HEADER),)
        RAYLIB_OK = sim
    endif
endif

# Compilar jogo e gerenciador.
.PHONY: all check-raylib run gerenciar teste test clean clear help

all: check-raylib $(PROJETO) $(EXECUTAVEL_GERENCIADOR)
	@echo "Compilacao concluida: $(PROJETO) e $(EXECUTAVEL_GERENCIADOR)"

# Verificar se o cabecalho e as bibliotecas da Raylib estao disponiveis.
check-raylib:
ifeq ($(RAYLIB_OK),)
	@echo "ERRO: Raylib nao foi encontrada."
	@echo "Instale a Raylib ou configure o pkg-config."
	@echo "Linux: sudo apt install libraylib-dev (quando disponivel na distribuicao)."
	@echo "Windows: instale mingw-w64-ucrt-x86_64-raylib pelo MSYS2 UCRT64."
	@exit 1
else
	@echo "$(RAYLIB_STATUS)"
endif

# Compilar o jogo principal.
$(PROJETO): $(OBJETOS_JOGO)
	$(CC) $(CFLAGS) $(RAYLIB_CFLAGS) $(OBJETOS_JOGO) -o $@ $(RAYLIB_LIBS)

# Compilar o cadastro de perguntas.
$(EXECUTAVEL_GERENCIADOR): $(OBJETOS_GERENCIADOR)
	$(CC) $(CFLAGS) $(OBJETOS_GERENCIADOR) -o $@ -lm

# Regra geral: transformar cada arquivo .c em um arquivo .o.
%.o: %.c $(CABECALHOS)
	$(CC) $(CFLAGS) $(RAYLIB_CFLAGS) -c $< -o $@

# Executar o jogo usando a fonte padrao da Raylib.
run: check-raylib $(PROJETO)
	$(EXECUTAR)$(PROJETO)

# Abrir o cadastro de perguntas.
gerenciar: $(EXECUTAVEL_GERENCIADOR)
	$(EXECUTAR)$(EXECUTAVEL_GERENCIADOR)

# Compilar e executar os testes sem precisar abrir a janela da Raylib.
teste:
	$(CC) -Itestes/stub -Iinclude -std=c11 -Wall -Wextra -Wpedantic -O2 \
		testes/teste_core.c testes/raylib_stub.c src/pergunta.c src/jogo.c \
		src/historia.c src/versus.c src/portal.c src/chefe.c src/loja.c -o $(TESTE) -lm
	$(EXECUTAR)$(TESTE)
	$(RM) $(TESTE)

test: teste

# Remover executaveis, arquivos objeto e arquivos temporarios dos testes.
clean:
	$(RM) $(PROJETO) $(EXECUTAVEL_GERENCIADOR) $(OBJETOS_JOGO) $(OBJETOS_GERENCIADOR) $(TESTE)
	$(RM) testes/banco_teste.csv testes/banco_teste.bin testes/csv_invalido.csv

# Apelido para limpeza, caso a equipe utilize o comando clear.
clear: clean

help:
	@echo "Makefile do Timey Wimey"
	@echo ""
	@echo "Comandos disponiveis:"
	@echo "  make          - Compila o jogo e o gerenciador"
	@echo "  make run      - Compila e executa o jogo"
	@echo "  make gerenciar - Abre o cadastro de perguntas"
	@echo "  make teste    - Executa os testes automaticos"
	@echo "  make clean    - Remove os arquivos compilados"
	@echo "  make help     - Mostra esta ajuda"
