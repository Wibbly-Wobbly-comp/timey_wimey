/**
 * @file jogo.c
 * @brief Implementa a sequência de perguntas e a pontuação da partida.
 */

#include <stdlib.h>
#include "jogo.h"

/**
 * @brief Retorna o prêmio de acordo com a posição da pergunta.
 */
int valorPergunta(int numero) {
    int premios[TOTAL_QUESTOES_JOGO] = {
        1000, 2000, 5000, 10000, 20000,
        30000, 40000, 50000, 100000, 200000,
        300000, 500000, 600000, 800000, 1000000
    };

    if (numero < 1 || numero > TOTAL_QUESTOES_JOGO) {
        return 0;
    }

    return premios[numero - 1];
}

/**
 * @brief Sorteia perguntas sem repetir e respeita a dificuldade de cada posição.
 * @return 1 se foi possível montar as 15 perguntas; caso contrário retorna 0.
 */
int iniciarJogo(Jogo *jogo, ListaPerguntas *lista) {
    enum Dificuldade niveis[TOTAL_QUESTOES_JOGO] = {
        MUITO_FACIL, MUITO_FACIL,
        FACIL, FACIL,
        MEDIO, MEDIO, MEDIO, MEDIO,
        DIFICIL, DIFICIL, DIFICIL, DIFICIL,
        MUITO_DIFICIL, MUITO_DIFICIL, MUITO_DIFICIL
    };

    if (lista->quantidade < TOTAL_QUESTOES_JOGO) {
        return 0;
    }

    for (int i = 0; i < TOTAL_QUESTOES_JOGO; i++) {
        int quantidade = 0;
        int escolhido;
        int repetida;
        int sorteado;

        /* Conta quantas perguntas deste nível ainda não foram utilizadas. */
        for (int j = 0; j < lista->quantidade; j++) {
            repetida = 0;
            if (lista->dados[j].dificuldade != niveis[i]) {
                continue;
            }
            for (int k = 0; k < i; k++) {
                if (j == jogo->perguntas_sorteadas[k]) {
                    repetida = 1;
                    break;
                }
            }
            if (!repetida) quantidade++;
        }

        if (quantidade == 0) {
            return 0;
        }

        /* Sorteia uma posição entre as perguntas disponíveis desse nível. */
        sorteado = rand() % quantidade;
        escolhido = -1;
        quantidade = 0;

        for (int j = 0; j < lista->quantidade; j++) {
            repetida = 0;
            if (lista->dados[j].dificuldade != niveis[i]) {
                continue;
            }
            for (int k = 0; k < i; k++) {
                if (j == jogo->perguntas_sorteadas[k]) {
                    repetida = 1;
                    break;
                }
            }
            if (!repetida) {
                if (quantidade == sorteado) {
                    escolhido = j;
                    break;
                }
                quantidade++;
            }
        }

        jogo->perguntas_sorteadas[i] = escolhido;
    }

    jogo->pergunta_atual = 0;
    jogo->pontuacao = 0;
    jogo->valor_garantido = 0;
    jogo->respondeu = 0;
    jogo->resposta_escolhida = -1;
    jogo->acertou = 0;
    jogo->terminou = 0;
    jogo->regiao_atual = 0;
    jogo->portal_escolhido = -1;
    for (int i = 0; i < 3; i++) jogo->portal_por_regiao[i] = -1;

    return 1;
}

/**
 * @brief Confere a alternativa escolhida e atualiza a pontuação.
 */
void responderPergunta(Jogo *jogo, Pergunta pergunta, int alternativa) {
    if (jogo->respondeu || alternativa < 0 || alternativa >= TOTAL_ALTERNATIVAS) {
        return;
    }

    jogo->respondeu = 1;
    jogo->resposta_escolhida = alternativa;
    jogo->acertou = (alternativa == pergunta.resposta_correta - 'A');

    if (jogo->acertou) {
        jogo->pontuacao = valorPergunta(jogo->pergunta_atual + 1);

        if (jogo->pergunta_atual == 4 || jogo->pergunta_atual == 9) {
            jogo->valor_garantido = jogo->pontuacao;
        }
    } else {
        jogo->terminou = 1;
    }
}

/**
 * @brief Avança para a próxima pergunta após uma resposta correta.
 */
void proximaPergunta(Jogo *jogo) {
    if (!jogo->respondeu || !jogo->acertou || jogo->terminou) {
        return;
    }

    if (jogo->pergunta_atual == TOTAL_QUESTOES_JOGO - 1) {
        jogo->terminou = 1;
        return;
    }

    jogo->pergunta_atual++;
    jogo->respondeu = 0;
    jogo->resposta_escolhida = -1;
    jogo->acertou = 0;
    jogo->regiao_atual = jogo->pergunta_atual / QUESTOES_POR_REGIAO;
    jogo->portal_escolhido = -1;
}

/**
 * @brief Retorna o valor garantido ou o prêmio final da partida.
 */
int valorFinal(Jogo jogo) {
    if (jogo.acertou && jogo.pergunta_atual == TOTAL_QUESTOES_JOGO - 1) {
        return 1000000;
    }

    return jogo.valor_garantido;
}
