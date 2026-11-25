/**
 * @file chefe.h
 * @brief Estrutura e regras da batalha contra O Paradoxo.
 *
 * A batalha ocupa as dez últimas perguntas do Modo História. O inimigo possui
 * cinco núcleos temporais; cada dupla de perguntas pode destruir um núcleo.
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

#ifndef CHEFE_H
#define CHEFE_H

#include "jogo.h"

#define TOTAL_NUCLEOS_CHEFE 5
#define PERGUNTAS_POR_RODADA_CHEFE 2
#define BONUS_RODADA_PERFEITA 30

enum ResultadoRodadaChefe {
    NUCLEO_RESISTIU = -1,
    RODADA_EM_ANDAMENTO = 0,
    NUCLEO_DESTRUIDO = 1,
    NUCLEO_DESTRUIDO_PERFEITO = 2
};

typedef struct {
    int batalha_iniciada; /* Indica que o jogador já entrou na arena do chefe. */
    int ativo; /* Indica que O Paradoxo ainda está em combate. */
    int fase; /* Fase 1 nas perguntas 41 a 44; fase 2 nas perguntas 45 a 50. */
    int nucleos_restantes; /* Quantidade de núcleos que ainda protegem o chefe. */
    int respostas_na_rodada; /* Respostas já confirmadas na dupla atual. */
    int acertos_na_rodada; /* Acertos obtidos na dupla atual. */
    int rodadas_concluidas; /* Quantidade de duplas de perguntas finalizadas. */
    int ultima_rodada; /* Resultado da última dupla, para a interface. */
    int venceu; /* Indica que o chefe foi derrotado. */
    int derrotado; /* Indica que a batalha terminou sem vitória. */
} Chefe;

/** Prepara os dados do chefe sem iniciar a batalha. */
void prepararChefe(Chefe *chefe);

/** Inicia o confronto com cinco núcleos e na primeira fase. */
void iniciarChefe(Chefe *chefe);

/** Retorna a fase correspondente ao índice da pergunta (começando em zero). */
int chefeFasePergunta(int indice_pergunta);

/** Registra uma resposta e resolve a rodada a cada duas perguntas. */
int chefeRegistrarResposta(Chefe *chefe, int indice_pergunta, int acertou);

/** Confirma a vitória somente se a pergunta 50 acertar e todos os núcleos caírem. */
void chefeFinalizar(Chefe *chefe, int acertou_pergunta_final);

/** Encerra a batalha por derrota, sem alterar o resultado do Modo Versus. */
void chefeEncerrarDerrota(Chefe *chefe);

#endif
