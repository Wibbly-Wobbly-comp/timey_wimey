/**
 * @file chefe.c
 * @brief Regras de progressão e dano da batalha contra O Paradoxo.
 *
 * @author 
 * Giovani Lemos Proença (giovanilemosproenca@alunos.utfpr.edu.br)
 * Hadassah Olívia de Paiva (hadassah@alunos.utfpr.edu.br)
 * João Pedro Arruda (joaosilva.2007@alunos.utfpr.edu.br)
 * 
 * @version 1.0
 * @date 2026-11-24
 * @copyright Copyright (c) 2026
 *
 */

#include <string.h>
#include "chefe.h"

/**
 * @brief Prepara o chefe para uma nova expedição.
 *
 * O jogador precisa superar cinco rodadas, cada uma formada por duas
 * perguntas. Acertar pelo menos uma pergunta da dupla destrói um núcleo.
 */
void prepararChefe(Chefe *chefe) {
    if (chefe == NULL) return;

    memset(chefe, 0, sizeof(Chefe));

    chefe->fase = 1;
    chefe->nucleos_restantes = TOTAL_NUCLEOS_CHEFE;
    chefe->ultima_rodada = RODADA_EM_ANDAMENTO;
}

void iniciarChefe(Chefe *chefe) {
    if (chefe == NULL) return;

    prepararChefe(chefe);
    chefe->batalha_iniciada = 1;
    chefe->ativo = 1;
}

/**
 * @brief Define a fase de acordo com a pergunta atual.
 *
 * As perguntas 41 a 44 pertencem à primeira fase. As perguntas 45 a 50
 * pertencem à segunda fase. Assim, cada fase muda entre rodadas completas.
 */
int chefeFasePergunta(int indice_pergunta) {
    if (indice_pergunta < PERGUNTA_INICIO_CHEFE ||
        indice_pergunta >= TOTAL_QUESTOES_HISTORIA) {
        return 0;
    }

    /* A fase 1 ocupa as duas primeiras rodadas (perguntas 41 a 44).
       A fase 2 começa na rodada 3 (pergunta 45), sem cortar uma dupla. */
    if (indice_pergunta < PERGUNTA_INICIO_CHEFE + 4) {
        return 1;
    }

    return 2;
}

/**
 * @brief Registra uma resposta e resolve cada dupla de perguntas.
 *
 * Uma resposta correta atinge o chefe. Ao confirmar a segunda pergunta da
 * dupla, pelo menos um acerto destrói um núcleo. Dois acertos também geram
 * um bônus de fragmentos temporais.
 *
 * @return Um valor de enum ResultadoRodadaChefe.
 */
int chefeRegistrarResposta(Chefe *chefe, int indice_pergunta, int acertou) {
    int resultado;

    int indice_esperado;

    if (chefe == NULL || !chefe->batalha_iniciada || !chefe->ativo ||
        indice_pergunta < PERGUNTA_INICIO_CHEFE ||
        indice_pergunta >= TOTAL_QUESTOES_HISTORIA) {
        return RODADA_EM_ANDAMENTO;
    }

    /* Impede registrar duas vezes ou fora de ordem a mesma pergunta. */
    indice_esperado = PERGUNTA_INICIO_CHEFE +
                      chefe->rodadas_concluidas * PERGUNTAS_POR_RODADA_CHEFE +
                      chefe->respostas_na_rodada;
    if (indice_pergunta != indice_esperado) {
        return RODADA_EM_ANDAMENTO;
    }

    chefe->fase = chefeFasePergunta(indice_pergunta);
    chefe->respostas_na_rodada++;

    if (acertou) {
        chefe->acertos_na_rodada++;
    }

    /* A primeira pergunta da dupla apenas registra o resultado. */
    if (chefe->respostas_na_rodada < PERGUNTAS_POR_RODADA_CHEFE) {
        return RODADA_EM_ANDAMENTO;
    }

    chefe->rodadas_concluidas++;

    if (chefe->acertos_na_rodada > 0 && chefe->nucleos_restantes > 0) {
        chefe->nucleos_restantes--;

        if (chefe->acertos_na_rodada == PERGUNTAS_POR_RODADA_CHEFE) {
            resultado = NUCLEO_DESTRUIDO_PERFEITO;
        } else {
            resultado = NUCLEO_DESTRUIDO;
        }
    } else {
        /* Sem acertos na dupla, o núcleo resiste aos ataques. */
        resultado = NUCLEO_RESISTIU;
    }

    chefe->ultima_rodada = resultado;
    chefe->respostas_na_rodada = 0;
    chefe->acertos_na_rodada = 0;

    return resultado;
}

/**
 * @brief Verifica se o golpe final derrotou O Paradoxo.
 *
 * A pergunta 50 precisa estar correta e todos os cinco núcleos precisam ter
 * sido destruídos. Caso contrário, a linha temporal não é estabilizada.
 */
void chefeFinalizar(Chefe *chefe, int acertou_pergunta_final) {
    if (chefe == NULL || !chefe->batalha_iniciada || !chefe->ativo) {
        return;
    }

    if (acertou_pergunta_final && chefe->nucleos_restantes == 0 &&
        chefe->rodadas_concluidas == TOTAL_NUCLEOS_CHEFE) {
        chefe->venceu = 1;
    } else {
        chefe->derrotado = 1;
    }

    chefe->ativo = 0;
}

/** Encerra a batalha quando o jogador perde todas as vidas ou falha na regeneração. */
void chefeEncerrarDerrota(Chefe *chefe) {
    if (chefe == NULL || !chefe->batalha_iniciada || chefe->venceu) {
        return;
    }

    chefe->ativo = 0;
    chefe->derrotado = 1;
}
