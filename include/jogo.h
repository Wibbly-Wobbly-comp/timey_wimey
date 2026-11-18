/**
 * @file jogo.h
 * @brief Estrutura e funções básicas da partida.
 */

#ifndef JOGO_H
#define JOGO_H

#include "pergunta.h"

#define TOTAL_QUESTOES_JOGO 15
#define QUESTOES_POR_REGIAO 5

typedef struct {
    int pergunta_atual;
    int perguntas_sorteadas[TOTAL_QUESTOES_JOGO];
    int pontuacao;
    int valor_garantido;
    int respondeu;
    int resposta_escolhida;
    int acertou;
    int terminou;
    int regiao_atual;
    int portal_escolhido;
    int portal_por_regiao[3];
} Jogo;

int iniciarJogo(Jogo *jogo, ListaPerguntas *lista);
void responderPergunta(Jogo *jogo, Pergunta pergunta, int alternativa);
void proximaPergunta(Jogo *jogo);
int valorPergunta(int numero);
int valorFinal(Jogo jogo);

#endif
