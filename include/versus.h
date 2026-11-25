/**
 * @file versus.h
 * @brief Estrutura e funções do modo Versus.
 *
 * Neste modo, o jogador enfrenta um computador que responde às mesmas
 * perguntas. A chance de acerto do computador diminui conforme a dificuldade.
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

#ifndef VERSUS_H
#define VERSUS_H

#include "jogo.h"

/* Identifica quem venceu a disputa. */
enum VencedorVersus {
    VERSUS_EM_ANDAMENTO = 0,
    VERSUS_JOGADOR,
    VERSUS_COMPUTADOR,
    VERSUS_EMPATE
};

typedef struct {
    int usou_cinquenta; /* 1 quando a dica 50/50 já foi utilizada. */
    int usou_plateia; /* 1 quando a dica da plateia já foi utilizada. */
    int alternativas_visiveis[TOTAL_ALTERNATIVAS]; /* 1 mostra; 0 esconde. */
    int votos_plateia[TOTAL_ALTERNATIVAS]; /* Percentual de votos de A, B, C e D. */
    int mostrar_votos; /* Controla a exibição dos percentuais na tela. */

    int chance_computador; /* Chance, em porcentagem, da pergunta atual. */
    int computador_acertou; /* Resultado sorteado para o computador. */
    int vencedor; /* Um dos valores definidos em VencedorVersus. */
    int terminou; /* Indica que a disputa não terá novas perguntas. */

    int premio_jogador; /* Prêmio do jogador, respeitando os marcos de segurança. */
    int premio_computador; /* Prêmio atual do computador. */
    int garantia_computador; /* Último marco de segurança alcançado pelo computador. */
} Versus;

void iniciarVersus(Versus *versus);
int chanceComputador(enum Dificuldade dificuldade);
int computadorAcerta(enum Dificuldade dificuldade);
int usarCinquenta(Versus *versus, Pergunta pergunta);
int usarPlateia(Versus *versus, Pergunta pergunta);
void registrarRodadaVersus(Versus *versus, Jogo *jogo, enum Dificuldade dificuldade, int computador_acertou);
void prepararProximaRodadaVersus(Versus *versus);

#endif
