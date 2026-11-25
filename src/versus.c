/**
 * @file versus.c
 * @brief Implementa a disputa entre o jogador e o computador.
 *
 * O computador não escolhe uma alternativa de texto. Para deixar o código
 * simples, ele utiliza uma chance de acerto definida pela dificuldade da
 * pergunta. O sorteio é feito uma vez por rodada.
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
#include "versus.h"

/**
 * @brief Prepara as dicas, a pontuação e o estado inicial da disputa.
 *
 * Todas as alternativas começam visíveis. As duas dicas ficam disponíveis
 * uma única vez durante a partida.
 */
void iniciarVersus(Versus *versus) {
    if (versus == NULL) return;

    versus->usou_cinquenta = 0;
    versus->usou_plateia = 0;
    versus->mostrar_votos = 0;
    versus->chance_computador = 0;
    versus->computador_acertou = 0;
    versus->vencedor = VERSUS_EM_ANDAMENTO;
    versus->terminou = 0;
    versus->premio_jogador = 0;
    versus->premio_computador = 0;
    versus->garantia_computador = 0;

    for (int i = 0; i < TOTAL_ALTERNATIVAS; i++) {
        versus->alternativas_visiveis[i] = 1;
        versus->votos_plateia[i] = 0;
    }
}

/**
 * @brief Retorna a chance de acerto do computador para cada dificuldade.
 *
 * Os valores estão em porcentagem. Por exemplo, 90 significa 90 chances
 * em 100 de o computador acertar uma pergunta muito fácil.
 */
int chanceComputador(enum Dificuldade dificuldade) {
    switch (dificuldade) {
        case MUITO_FACIL:  return 90;
        case FACIL:        return 80;
        case MEDIO:        return 60;
        case DIFICIL:      return 40;
        case MUITO_DIFICIL: return 20;
        default:           return 0;
    }
}

/**
 * @brief Sorteia se o computador acertou a pergunta.
 * @return 1 se acertou ou 0 se errou.
 */
int computadorAcerta(enum Dificuldade dificuldade) {
    int chance = chanceComputador(dificuldade);
    int sorteio = rand() % 100;

    return sorteio < chance;
}

/**
 * @brief Aplica a dica 50/50, escondendo duas alternativas incorretas.
 *
 * A alternativa correta sempre continua visível. As duas alternativas
 * removidas são escolhidas aleatoriamente entre as três incorretas.
 * @return 1 se a dica foi aplicada ou 0 se já foi utilizada.
 */
int usarCinquenta(Versus *versus, Pergunta pergunta) {
    int incorretas[3];
    int quantidade = 0;
    int indice_correto = pergunta.resposta_correta - 'A';

    if (versus == NULL || indice_correto < 0 || indice_correto >= TOTAL_ALTERNATIVAS ||
        versus->usou_cinquenta) {
        return 0;
    }

    /* Guarda os índices das três alternativas incorretas. */
    for (int i = 0; i < TOTAL_ALTERNATIVAS; i++) {
        if (i != indice_correto) {
            incorretas[quantidade] = i;
            quantidade++;
        }
    }

    /* Esconde duas alternativas incorretas, sorteadas sem repetição. */
    int primeira = rand() % 3;
    int segunda;
    do {
        segunda = rand() % 3;
    } while (segunda == primeira);

    versus->alternativas_visiveis[incorretas[primeira]] = 0;
    versus->alternativas_visiveis[incorretas[segunda]] = 0;
    versus->usou_cinquenta = 1;

    return 1;
}

/**
 * @brief Gera a votação da plateia e apresenta os percentuais ao jogador.
 *
 * A votação tende a favorecer a alternativa correta, mas não é infalível.
 * Em perguntas mais difíceis, essa vantagem é menor.
 * @return 1 se a dica foi gerada ou 0 se já foi utilizada.
 */
int usarPlateia(Versus *versus, Pergunta pergunta) {
    int pesos[TOTAL_ALTERNATIVAS];
    int soma_pesos = 0;
    int soma_percentuais = 0;
    int indice_correto = pergunta.resposta_correta - 'A';

    if (versus == NULL || indice_correto < 0 || indice_correto >= TOTAL_ALTERNATIVAS ||
        versus->usou_plateia) {
        return 0;
    }

    /* Cada alternativa recebe uma quantidade aleatória de votos. */
    for (int i = 0; i < TOTAL_ALTERNATIVAS; i++) {
        pesos[i] = 10 + rand() % 16;
    }

    /* A plateia tende a conhecer melhor as perguntas mais fáceis. */
    pesos[indice_correto] += 65 - ((int)pergunta.dificuldade * 8);

    for (int i = 0; i < TOTAL_ALTERNATIVAS; i++) {
        soma_pesos += pesos[i];
    }

    /* Converte os pesos em porcentagens inteiras. */
    for (int i = 0; i < TOTAL_ALTERNATIVAS; i++) {
        versus->votos_plateia[i] = (pesos[i] * 100) / soma_pesos;
        soma_percentuais += versus->votos_plateia[i];
    }

    /* Corrige eventual diferença causada pelo arredondamento. */
    versus->votos_plateia[indice_correto] += 100 - soma_percentuais;
    versus->mostrar_votos = 1;
    versus->usou_plateia = 1;

    return 1;
}

/**
 * @brief Atualiza a pontuação e verifica o resultado de uma rodada.
 *
 * O jogador e o computador respondem à mesma pergunta. Se apenas um acertar,
 * esse participante vence a disputa. Se ambos errarem, ocorre empate.
 * Quando os dois acertam a pergunta 15, a partida também termina empatada.
 */
void registrarRodadaVersus(Versus *versus, Jogo *jogo, enum Dificuldade dificuldade, int computador_acertou) {
    int numero_pergunta;
    int jogador_acertou;

    if (versus == NULL || jogo == NULL || !jogo->respondeu ||
        jogo->pergunta_atual < 0 || jogo->pergunta_atual >= TOTAL_QUESTOES_VERSUS) return;

    numero_pergunta = jogo->pergunta_atual + 1;
    jogador_acertou = jogo->acertou;

    versus->computador_acertou = computador_acertou;
    versus->chance_computador = chanceComputador(dificuldade);

    /* Atualiza o prêmio do jogador e mantém os marcos de segurança do jogo. */
    if (jogador_acertou) {
        versus->premio_jogador = jogo->pontuacao;
    } else {
        versus->premio_jogador = jogo->valor_garantido;
    }

    /* Atualiza separadamente a pontuação do computador. */
    if (computador_acertou) {
        versus->premio_computador = valorPergunta(numero_pergunta);

        if (numero_pergunta == 5 || numero_pergunta == 10) {
            versus->garantia_computador = versus->premio_computador;
        }
    } else {
        versus->premio_computador = versus->garantia_computador;
    }

    /* Decide o vencedor quando os resultados dos dois participantes diferem. */
    if (jogador_acertou && !computador_acertou) {
        versus->vencedor = VERSUS_JOGADOR;
        versus->terminou = 1;
    } else if (!jogador_acertou && computador_acertou) {
        versus->vencedor = VERSUS_COMPUTADOR;
        versus->terminou = 1;
    } else if (!jogador_acertou && !computador_acertou) {
        versus->vencedor = VERSUS_EMPATE;
        versus->terminou = 1;
    } else if (numero_pergunta == TOTAL_QUESTOES_VERSUS) {
        /* Ambos acertaram as quinze perguntas. */
        versus->vencedor = VERSUS_EMPATE;
        versus->terminou = 1;
    }
}

/**
 * @brief Prepara as alternativas e a votação para a próxima pergunta.
 *
 * As dicas já utilizadas continuam marcadas como utilizadas, mas os efeitos
 * visuais da pergunta anterior são removidos.
 */
void prepararProximaRodadaVersus(Versus *versus) {
    if (versus == NULL) return;

    versus->mostrar_votos = 0;

    for (int i = 0; i < TOTAL_ALTERNATIVAS; i++) {
        versus->alternativas_visiveis[i] = 1;
        versus->votos_plateia[i] = 0;
    }
}
