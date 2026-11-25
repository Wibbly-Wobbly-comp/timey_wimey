/**
 * @file jogo.h
 * @brief Estrutura e funções básicas de uma partida.
 *
 * O Modo História possui 50 perguntas e o Modo Versus mantém as 15
 * perguntas do formato tradicional do Show do Milhão.
 *
 * @author 
 * Giovani Lemos Proença (giovanilemosproenca@alunos.utfpr.edu.br)
 * Hadassah Olívia de Paiva (hadassah@alunos.utfpr.edu.br)
 * João Pedro Arruda (joaosilva.2007@alunos.utfpr.edu.br)
 * @version 1.0
 * @date 2026-11-24
 * @copyright Copyright (c) 2026
 *
 */

#ifndef JOGO_H
#define JOGO_H

#include "pergunta.h"

#define TOTAL_QUESTOES_VERSUS 15
#define TOTAL_QUESTOES_HISTORIA 50
#define QUESTOES_POR_REGIAO 5
/* A exploração ocupa oito regiões; as dez perguntas finais pertencem ao chefe. */
#define TOTAL_REGIOES_HISTORIA 8
#define TOTAL_QUESTOES_EXPLORACAO (TOTAL_REGIOES_HISTORIA * QUESTOES_POR_REGIAO)
#define TOTAL_QUESTOES_CHEFE (TOTAL_QUESTOES_HISTORIA - TOTAL_QUESTOES_EXPLORACAO)
#define PERGUNTA_INICIO_CHEFE TOTAL_QUESTOES_EXPLORACAO

typedef struct {
    int total_perguntas; /* Quantidade de perguntas desta partida: 15 ou 50. */
    int pergunta_atual; /* Posição da pergunta atual, começando em zero. */
    int perguntas_sorteadas[TOTAL_QUESTOES_HISTORIA]; /* Índices das perguntas sorteadas. */
    int pontuacao; /* Prêmio do formato tradicional, usado no Modo Versus. */
    int valor_garantido; /* Prêmio protegido pelos marcos do Modo Versus. */
    int respondeu; /* Indica se o jogador já respondeu à pergunta atual. */
    int resposta_escolhida; /* Índice da alternativa escolhida (0 a 3). */
    int acertou; /* Indica se a resposta do jogador está correta. */
    int terminou; /* Indica se a partida foi encerrada. */
    int regiao_atual; /* Região de exploração; 8 ou 9 durante a batalha final. */
    int portal_escolhido; /* Portal escolhido na região atual. */
    int portal_por_regiao[TOTAL_REGIOES_HISTORIA]; /* Índice do portal escolhido na tela da região. */
    int portal_ids_visitados[TOTAL_REGIOES_HISTORIA]; /* ID de cada destino já visitado. */
} Jogo;

/** Sorteia 15 ou 50 perguntas, respeitando a dificuldade de cada posição. */
int iniciarJogo(Jogo *jogo, ListaPerguntas *lista, int total_perguntas);

/** Confere a resposta e atualiza o estado da partida. */
void responderPergunta(Jogo *jogo, Pergunta pergunta, int alternativa, int encerrar_no_erro);

/** Avança para a próxima pergunta após a resposta ser processada. */
void proximaPergunta(Jogo *jogo);

/** Retorna o prêmio associado ao número da pergunta do Modo Versus. */
int valorPergunta(int numero);

/** Retorna o prêmio final, considerando os marcos de segurança. */
int valorFinal(Jogo jogo);

#endif
