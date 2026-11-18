CC = gcc
CFLAGS = -Iinclude -std=c11 -Wall -Wextra -Wpedantic -O2
LIBS = -L/usr/local/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

PROJETO = timey_wimey
GERENCIADOR = gerenciador

ARQUIVOS_JOGO = src/main.c src/pergunta.c src/jogo.c src/personagem.c src/portal.c
ARQUIVOS_GERENCIADOR = src/gerenciador.c src/pergunta.c

all: $(PROJETO) $(GERENCIADOR)

$(PROJETO): $(ARQUIVOS_JOGO)
	$(CC) $(CFLAGS) $(ARQUIVOS_JOGO) -o $(PROJETO) $(LIBS)

$(GERENCIADOR): $(ARQUIVOS_GERENCIADOR)
	$(CC) $(CFLAGS) $(ARQUIVOS_GERENCIADOR) -o $(GERENCIADOR)

run: $(PROJETO)
	./$(PROJETO)

gerenciar: $(GERENCIADOR)
	./$(GERENCIADOR)

clean:
	rm -f $(PROJETO) $(GERENCIADOR)

.PHONY: all run gerenciar clean
