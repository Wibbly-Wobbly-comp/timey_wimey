/**
 * @file jogo.c
 * @brief Implementa o sorteio, a sequência e a pontuação das partidas.
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

#include <stdlib.h>
#include "jogo.h"

/**
 * @brief Retorna o prêmio de acordo com a posição da pergunta.
 *
 * Esta tabela pertence ao Modo Versus, que segue o formato tradicional
 * de 15 perguntas. O Modo História utiliza fragmentos temporais.
 */
int valorPergunta(int numero) {
    int premios[TOTAL_QUESTOES_VERSUS] = {
        1000, 2000, 5000, 10000, 20000,
        30000, 40000, 50000, 100000, 200000,
        300000, 500000, 600000, 800000, 1000000
    };

    if (numero < 1 || numero > TOTAL_QUESTOES_VERSUS) {
        return 0;
    }

    return premios[numero - 1];
}

/**
 * @brief Sorteia perguntas sem repetir e monta a dificuldade da partida.
 *
 * No Versus, a distribuição original é mantida: 2 muito fáceis, 2 fáceis,
 * 4 médias, 4 difíceis e 3 muito difíceis.
 *
 * Na História, são sorteadas 10 perguntas de cada dificuldade, totalizando
 * 50 perguntas. Cada bloco de 10 aumenta um nível de dificuldade.
 */
int iniciarJogo(Jogo *jogo, ListaPerguntas *lista, int total_perguntas) {
    enum Dificuldade niveis[TOTAL_QUESTOES_HISTORIA];
    int quantidade_nivel[5];
    int indice_nivel = 0;

    if (jogo == NULL || lista == NULL || lista->dados == NULL || lista->quantidade <= 0 ||
        (total_perguntas != TOTAL_QUESTOES_VERSUS &&
         total_perguntas != TOTAL_QUESTOES_HISTORIA)) {
        return 0;
    }

    if (total_perguntas == TOTAL_QUESTOES_VERSUS) {
        quantidade_nivel[0] = 2;
        quantidade_nivel[1] = 2;
        quantidade_nivel[2] = 4;
        quantidade_nivel[3] = 4;
        quantidade_nivel[4] = 3;
    } else {
        for (int i = 0; i < 5; i++) {
            quantidade_nivel[i] = 10;
        }
    }

    /* Monta a sequência de dificuldade que será utilizada na partida. */
    for (int nivel = 0; nivel < 5; nivel++) {
        for (int i = 0; i < quantidade_nivel[nivel]; i++) {
            niveis[indice_nivel] = (enum Dificuldade)(nivel + 1);
            indice_nivel++;
        }
    }

    /* Confere se existe quantidade suficiente de perguntas em cada nível. */
    for (int nivel = 0; nivel < 5; nivel++) {
        int disponiveis = 0;

        for (int i = 0; i < lista->quantidade; i++) {
            if (lista->dados[i].dificuldade == (enum Dificuldade)(nivel + 1)) {
                disponiveis++;
            }
        }

        if (disponiveis < quantidade_nivel[nivel]) {
            return 0;
        }
    }

    /* Sorteia uma pergunta de cada vez, sem repetir índices. */
    for (int i = 0; i < total_perguntas; i++) {
        int quantidade = 0;
        int sorteado;
        int escolhido = -1;

        /* Conta as perguntas disponíveis para a dificuldade desta posição. */
        for (int j = 0; j < lista->quantidade; j++) {
            int repetida = 0;

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
                quantidade++;
            }
        }

        sorteado = rand() % quantidade;
        quantidade = 0;

        /* Encontra a pergunta correspondente à posição sorteada. */
        for (int j = 0; j < lista->quantidade; j++) {
            int repetida = 0;

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

        if (escolhido == -1) {
            return 0;
        }

        jogo->perguntas_sorteadas[i] = escolhido;
    }

    /* Reinicia todos os dados da partida. */
    jogo->total_perguntas = total_perguntas;
    jogo->pergunta_atual = 0;
    jogo->pontuacao = 0;
    jogo->valor_garantido = 0;
    jogo->respondeu = 0;
    jogo->resposta_escolhida = -1;
    jogo->acertou = 0;
    jogo->terminou = 0;
    jogo->regiao_atual = 0;
    jogo->portal_escolhido = -1;

    for (int i = 0; i < TOTAL_REGIOES_HISTORIA; i++) {
        jogo->portal_por_regiao[i] = -1;
        jogo->portal_ids_visitados[i] = -1;
    }

    return 1;
}

/**
 * @brief Confere a alternativa escolhida e atualiza a pontuação.
 *
 * O parâmetro encerrar_no_erro permite que cada modo tenha sua própria regra:
 * no Versus, o erro encerra a disputa; na História, o erro retira uma vida,
 * mas a decisão de continuar fica a cargo do sistema de vidas.
 */
void responderPergunta(Jogo *jogo, Pergunta pergunta, int alternativa, int encerrar_no_erro) {
    if (jogo == NULL || jogo->respondeu ||
        alternativa < 0 || alternativa >= TOTAL_ALTERNATIVAS ||
        pergunta.resposta_correta < 'A' || pergunta.resposta_correta > 'D') {
        return;
    }

    jogo->respondeu = 1;
    jogo->resposta_escolhida = alternativa;
    jogo->acertou = (alternativa == pergunta.resposta_correta - 'A');

    if (jogo->acertou && jogo->total_perguntas == TOTAL_QUESTOES_VERSUS) {
        jogo->pontuacao = valorPergunta(jogo->pergunta_atual + 1);

        if (jogo->pergunta_atual == 4 || jogo->pergunta_atual == 9) {
            jogo->valor_garantido = jogo->pontuacao;
        }
    } else if (!jogo->acertou && encerrar_no_erro) {
        jogo->terminou = 1;
    }
}

/**
 * @brief Avança para a próxima pergunta depois que a resposta foi confirmada.
 *
 * No Modo História, uma resposta errada também pode avançar caso o jogador
 * ainda tenha vidas. No Versus, o erro já deixa a partida encerrada.
 */
void proximaPergunta(Jogo *jogo) {
    if (jogo == NULL || jogo->total_perguntas <= 0 ||
        jogo->pergunta_atual < 0 || jogo->pergunta_atual >= jogo->total_perguntas ||
        !jogo->respondeu || jogo->terminou) {
        return;
    }

    if (jogo->pergunta_atual == jogo->total_perguntas - 1) {
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
 * @brief Retorna o valor garantido ou o prêmio final do Modo Versus.
 */
int valorFinal(Jogo jogo) {
    if (jogo.acertou && jogo.pergunta_atual == TOTAL_QUESTOES_VERSUS - 1) {
        return 1000000;
    }

    return jogo.valor_garantido;
}
