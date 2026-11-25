/**
 * @file historia.c
 * @brief Implementa vidas, dicas, recompensas e progressão temporal.
 *
 * Os recursos usam elementos inspirados no universo de Doctor Who:
 * Chave de Fenda Sônica, Arquivos da TARDIS, reversão temporal e regeneração.
 * As funções foram separadas para manter a lógica do jogo simples de testar.
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
#include <stdio.h>
#include "historia.h"

/**
 * @brief Inicializa os recursos do jogador no começo da expedição.
 *
 * O jogador começa com três vidas, duas cargas da Chave de Fenda Sônica,
 * uma consulta aos Arquivos da TARDIS e uma reversão temporal.
 */
void iniciarHistoria(Historia *historia) {
    if (historia == NULL) return;

    historia->vidas = VIDAS_INICIAIS;
    historia->vidas_maximas = VIDAS_MAXIMAS;
    historia->fragmentos = 0;
    historia->regioes_concluidas = 0;

    historia->cargas_sonica = CARGAS_SONICA_INICIAIS;
    historia->cargas_arquivo = CARGAS_ARQUIVO_INICIAIS;
    historia->reversoes_temporais = REVERSOES_INICIAIS;

    historia->regeneracao_disponivel = 1;
    historia->regeneracao_usada = 0;
    historia->regeneracao_em_andamento = 0;
    historia->indice_pergunta_regeneracao = -1;
    historia->game_over = 0;
    historia->mensagem_recompensa[0] = '\0';

    historiaNovaPergunta(historia);
}

/**
 * @brief Limpa os efeitos que pertencem apenas à pergunta anterior.
 */
void historiaNovaPergunta(Historia *historia) {
    if (historia == NULL) return;

    historia->usou_sonica_pergunta = 0;
    historia->usou_arquivo_pergunta = 0;
    historia->mostrar_votos = 0;

    for (int i = 0; i < TOTAL_ALTERNATIVAS; i++) {
        historia->alternativas_visiveis[i] = 1;
        historia->votos_arquivo[i] = 0;
    }
}

/**
 * @brief Elimina uma alternativa incorreta com a Chave de Fenda Sônica.
 * @return 1 se a dica foi usada; 0 se não havia carga ou já foi usada na pergunta.
 */
int historiaUsarSonic(Historia *historia, Pergunta pergunta) {
    int incorretas[TOTAL_ALTERNATIVAS - 1];
    int quantidade = 0;
    int indice_correto = pergunta.resposta_correta - 'A';

    if (historia == NULL || indice_correto < 0 || indice_correto >= TOTAL_ALTERNATIVAS ||
        historia->cargas_sonica <= 0 || historia->usou_sonica_pergunta) {
        return 0;
    }

    /* Guarda as três alternativas erradas. */
    for (int i = 0; i < TOTAL_ALTERNATIVAS; i++) {
        if (i != indice_correto && historia->alternativas_visiveis[i]) {
            incorretas[quantidade] = i;
            quantidade++;
        }
    }

    if (quantidade == 0) {
        return 0;
    }

    /* A sônica identifica e remove uma alternativa incorreta. */
    historia->alternativas_visiveis[incorretas[rand() % quantidade]] = 0;
    historia->cargas_sonica--;
    historia->usou_sonica_pergunta = 1;

    return 1;
}

/**
 * @brief Simula uma consulta aos Arquivos da TARDIS.
 *
 * A votação favorece a alternativa correta, mas não é garantida. Assim, a
 * dica ajuda o jogador sem revelar automaticamente a resposta.
 * @return 1 se a consulta foi feita; 0 se não havia carga ou já foi usada.
 */
int historiaUsarArquivo(Historia *historia, Pergunta pergunta) {
    int pesos[TOTAL_ALTERNATIVAS];
    int soma_pesos = 0;
    int soma_percentuais = 0;
    int indice_correto = pergunta.resposta_correta - 'A';

    if (historia == NULL || indice_correto < 0 || indice_correto >= TOTAL_ALTERNATIVAS ||
        historia->cargas_arquivo <= 0 || historia->usou_arquivo_pergunta) {
        return 0;
    }

    for (int i = 0; i < TOTAL_ALTERNATIVAS; i++) {
        pesos[i] = 10 + rand() % 16;
    }

    /* As informações da TARDIS são mais precisas em perguntas fáceis. */
    pesos[indice_correto] += 65 - ((int)pergunta.dificuldade * 8);

    for (int i = 0; i < TOTAL_ALTERNATIVAS; i++) {
        soma_pesos += pesos[i];
    }

    for (int i = 0; i < TOTAL_ALTERNATIVAS; i++) {
        historia->votos_arquivo[i] = (pesos[i] * 100) / soma_pesos;
        soma_percentuais += historia->votos_arquivo[i];
    }

    /* Corrige o arredondamento para que a soma seja exatamente 100%. */
    historia->votos_arquivo[indice_correto] += 100 - soma_percentuais;
    historia->cargas_arquivo--;
    historia->usou_arquivo_pergunta = 1;
    historia->mostrar_votos = 1;

    return 1;
}

/**
 * @brief Troca a pergunta errada por outra pergunta do mesmo nível.
 *
 * A função procura uma pergunta que ainda não tenha sido respondida na
 * expedição. Se a pergunta sorteada já estiver reservada para uma posição
 * futura, as duas posições são trocadas. Assim, a pergunta antiga continua
 * disponível e nenhuma pergunta é repetida na sequência principal.
 * @return 1 se uma nova pergunta foi sorteada; 0 se não foi possível.
 */
int historiaUsarReversao(Historia *historia, Jogo *jogo, ListaPerguntas *lista) {
    int indice_atual;
    int indice_escolhido = -1;
    int quantidade = 0;
    enum Dificuldade dificuldade;

    if (historia == NULL || jogo == NULL || lista == NULL || lista->dados == NULL ||
        jogo->pergunta_atual < 0 || jogo->pergunta_atual >= jogo->total_perguntas ||
        jogo->pergunta_atual >= TOTAL_QUESTOES_HISTORIA ||
        historia->reversoes_temporais <= 0 || !jogo->respondeu || jogo->acertou ||
        historia->regeneracao_em_andamento) {
        return 0;
    }

    indice_atual = jogo->perguntas_sorteadas[jogo->pergunta_atual];
    if (indice_atual < 0 || indice_atual >= lista->quantidade) return 0;
    dificuldade = lista->dados[indice_atual].dificuldade;

    /* Sorteio por reservatório: não precisa criar outro vetor de índices. */
    for (int i = 0; i < lista->quantidade; i++) {
        int ja_respondida = 0;

        if (i == indice_atual || lista->dados[i].dificuldade != dificuldade) {
            continue;
        }

        /* As posições anteriores já foram respondidas pelo jogador. */
        for (int j = 0; j < jogo->pergunta_atual; j++) {
            if (jogo->perguntas_sorteadas[j] == i) {
                ja_respondida = 1;
                break;
            }
        }

        if (!ja_respondida) {
            quantidade++;
            if (rand() % quantidade == 0) {
                indice_escolhido = i;
            }
        }
    }

    if (indice_escolhido == -1) {
        return 0;
    }

    /* Se a nova pergunta estava reservada para depois, troca os índices. */
    for (int i = jogo->pergunta_atual + 1; i < jogo->total_perguntas; i++) {
        if (jogo->perguntas_sorteadas[i] == indice_escolhido) {
            jogo->perguntas_sorteadas[i] = indice_atual;
            break;
        }
    }

    jogo->perguntas_sorteadas[jogo->pergunta_atual] = indice_escolhido;
    historia->reversoes_temporais--;
    jogo->respondeu = 0;
    jogo->resposta_escolhida = -1;
    jogo->acertou = 0;
    jogo->terminou = 0;
    historiaNovaPergunta(historia);

    return 1;
}

/**
 * @brief Prepara o desafio de regeneração com uma pergunta muito difícil.
 *
 * Primeiro, procura uma pergunta muito difícil que não esteja na sequência
 * de 50 perguntas. Se não houver uma reserva, procura uma pergunta difícil
 * que ainda não tenha sido respondida. Como último recurso, o banco pode
 * reutilizar uma pergunta muito difícil já vista, caso tenha somente as dez
 * perguntas mínimas e o jogador chegue ao protocolo depois de responder todas.
 *
 * O desafio é especial e não ocupa uma das 50 posições da expedição.
 * @return 1 se encontrou uma pergunta; 0 se não existe pergunta muito difícil.
 */
int historiaIniciarRegeneracao(Historia *historia, Jogo *jogo, ListaPerguntas *lista) {
    int indice_escolhido = -1;
    int quantidade = 0;

    if (historia == NULL || jogo == NULL || lista == NULL || lista->dados == NULL ||
        jogo->pergunta_atual < 0 || jogo->pergunta_atual >= jogo->total_perguntas ||
        jogo->total_perguntas > TOTAL_QUESTOES_HISTORIA ||
        !historia->regeneracao_em_andamento || !historia->regeneracao_disponivel) {
        return 0;
    }

    /* Primeira tentativa: escolher uma pergunta que não esteja na sequência. */
    for (int i = 0; i < lista->quantidade; i++) {
        int esta_na_sequencia = 0;

        if (lista->dados[i].dificuldade != MUITO_DIFICIL) {
            continue;
        }

        for (int j = 0; j < jogo->total_perguntas; j++) {
            if (jogo->perguntas_sorteadas[j] == i) {
                esta_na_sequencia = 1;
                break;
            }
        }

        if (!esta_na_sequencia) {
            quantidade++;
            if (rand() % quantidade == 0) {
                indice_escolhido = i;
            }
        }
    }

    /* Segunda tentativa: permite usar uma pergunta reservada para o futuro. */
    if (indice_escolhido == -1) {
        quantidade = 0;

        for (int i = 0; i < lista->quantidade; i++) {
            int ja_respondida = 0;

            if (lista->dados[i].dificuldade != MUITO_DIFICIL) {
                continue;
            }

            for (int j = 0; j <= jogo->pergunta_atual; j++) {
                if (jogo->perguntas_sorteadas[j] == i) {
                    ja_respondida = 1;
                    break;
                }
            }

            if (!ja_respondida) {
                quantidade++;
                if (rand() % quantidade == 0) {
                    indice_escolhido = i;
                }
            }
        }
    }

    /* Último recurso: o banco pode ter exatamente dez perguntas muito difíceis. */
    if (indice_escolhido == -1) {
        quantidade = 0;

        for (int i = 0; i < lista->quantidade; i++) {
            if (lista->dados[i].dificuldade != MUITO_DIFICIL ||
                i == jogo->perguntas_sorteadas[jogo->pergunta_atual]) {
                continue;
            }

            quantidade++;
            if (rand() % quantidade == 0) {
                indice_escolhido = i;
            }
        }
    }

    if (indice_escolhido == -1) {
        return 0;
    }

    historia->indice_pergunta_regeneracao = indice_escolhido;
    historia->regeneracao_disponivel = 0;
    historia->regeneracao_usada = 1;

    /* Limpa a resposta errada anterior para receber a resposta do desafio. */
    jogo->respondeu = 0;
    jogo->resposta_escolhida = -1;
    jogo->acertou = 0;
    jogo->terminou = 0;
    historiaNovaPergunta(historia);

    return 1;
}

/**
 * @brief Registra a conclusão de uma região e concede seu bônus.
 *
 * A região só é considerada concluída depois que as cinco perguntas foram
 * respondidas e o jogador sobreviveu. A comparação evita conceder o bônus
 * duas vezes para a mesma região.
 */
void historiaConcluirRegiao(Historia *historia, int indice_regiao) {
    if (!historia || indice_regiao < 0 ||
        indice_regiao >= TOTAL_REGIOES_HISTORIA ||
        indice_regiao != historia->regioes_concluidas) {
        return;
    }

    historia->regioes_concluidas++;
    historia->fragmentos += 25;
}

/**
 * @brief Entrega a recompensa do portal escolhido depois da região concluída.
 *
 * Quando um recurso já está no limite, o jogo converte a recompensa excedente
 * em fragmentos temporais. Assim, nenhum portal oferece uma recompensa inútil.
 */
void historiaRecompensaPortal(Historia *historia, enum TipoRecompensaPortal tipo, int valor) {
    int quantidade;

    if (!historia || valor <= 0) {
        return;
    }

    switch (tipo) {
        case RECOMPENSA_ARQUIVOS:
            quantidade = historia->cargas_arquivo + valor;
            if (quantidade > CARGAS_ARQUIVO_MAXIMAS) quantidade = CARGAS_ARQUIVO_MAXIMAS;
            if (quantidade == historia->cargas_arquivo) {
                historia->fragmentos += 20;
                snprintf(historia->mensagem_recompensa, sizeof(historia->mensagem_recompensa),
                         "+20 fragmentos: Arquivos da TARDIS ja estavam no limite.");
            } else {
                int recebidas = quantidade - historia->cargas_arquivo;
                historia->cargas_arquivo = quantidade;
                snprintf(historia->mensagem_recompensa, sizeof(historia->mensagem_recompensa),
                         "+%d carga(s) dos Arquivos da TARDIS.", recebidas);
            }
            break;

        case RECOMPENSA_VIDA:
            quantidade = historia->vidas + valor;
            if (quantidade > historia->vidas_maximas) quantidade = historia->vidas_maximas;
            if (quantidade == historia->vidas) {
                historia->fragmentos += 30;
                snprintf(historia->mensagem_recompensa, sizeof(historia->mensagem_recompensa),
                         "+30 fragmentos: suas vidas ja estavam no limite.");
            } else {
                int recebidas = quantidade - historia->vidas;
                historia->vidas = quantidade;
                snprintf(historia->mensagem_recompensa, sizeof(historia->mensagem_recompensa),
                         "+%d vida(s).", recebidas);
            }
            break;

        case RECOMPENSA_SONICA:
            quantidade = historia->cargas_sonica + valor;
            if (quantidade > CARGAS_SONICA_MAXIMAS) quantidade = CARGAS_SONICA_MAXIMAS;
            if (quantidade == historia->cargas_sonica) {
                historia->fragmentos += 20;
                snprintf(historia->mensagem_recompensa, sizeof(historia->mensagem_recompensa),
                         "+20 fragmentos: Chave Sonica ja estava no limite.");
            } else {
                int recebidas = quantidade - historia->cargas_sonica;
                historia->cargas_sonica = quantidade;
                snprintf(historia->mensagem_recompensa, sizeof(historia->mensagem_recompensa),
                         "+%d carga(s) da Chave Sonica.", recebidas);
            }
            break;

        case RECOMPENSA_REVERSAO:
            quantidade = historia->reversoes_temporais + valor;
            if (quantidade > REVERSOES_MAXIMAS) quantidade = REVERSOES_MAXIMAS;
            if (quantidade == historia->reversoes_temporais) {
                historia->fragmentos += 20;
                snprintf(historia->mensagem_recompensa, sizeof(historia->mensagem_recompensa),
                         "+20 fragmentos: Reversoes temporais ja estavam no limite.");
            } else {
                int recebidas = quantidade - historia->reversoes_temporais;
                historia->reversoes_temporais = quantidade;
                snprintf(historia->mensagem_recompensa, sizeof(historia->mensagem_recompensa),
                         "+%d Reversao(oes) Temporal(is).", recebidas);
            }
            break;

        case RECOMPENSA_FRAGMENTOS:
            historia->fragmentos += valor;
            snprintf(historia->mensagem_recompensa, sizeof(historia->mensagem_recompensa),
                     "+%d fragmentos temporais.", valor);
            break;

        default:
            return;
    }

}

/**
 * @brief Retorna a quantidade de fragmentos temporais de uma dificuldade.
 */
int fragmentosPergunta(enum Dificuldade dificuldade) {
    switch (dificuldade) {
        case MUITO_FACIL: return 10;
        case FACIL: return 15;
        case MEDIO: return 25;
        case DIFICIL: return 40;
        case MUITO_DIFICIL: return 60;
        default: return 0;
    }
}

/**
 * @brief Processa o resultado depois que o jogador confirma a tela de feedback.
 *
 * Respostas certas entregam fragmentos temporais. Respostas erradas retiram
 * uma vida. Quando as vidas chegam a zero, começa o desafio especial de
 * regeneração. Acertar a pergunta muito difícil restaura uma vida; errar
 * encerra a expedição.
 */
void historiaProcessarResposta(Historia *historia, Jogo *jogo, Pergunta pergunta) {
    if (historia == NULL || jogo == NULL || !jogo->respondeu) return;

    /* O desafio de regeneração não segue a regra normal de perder uma vida. */
    if (historia->regeneracao_em_andamento) {
        historia->regeneracao_em_andamento = 0;

        if (jogo->acertou) {
            /* Acertar o desafio devolve uma vida e mantém a expedição viva. */
            historia->vidas = 1;
        } else {
            /* Errar o desafio encerra imediatamente a expedição. */
            historia->vidas = 0;
            historia->game_over = 1;
            jogo->terminou = 1;
        }

        return;
    }

    if (jogo->acertou) {
        /* Fragmentos são moeda da expedição e podem ser gastos na loja. */
        historia->fragmentos += fragmentosPergunta(pergunta.dificuldade);
    } else {
        historia->vidas--;

        /* Ao chegar a zero, o jogador recebe uma última pergunta muito difícil. */
        if (historia->vidas <= 0 && historia->regeneracao_disponivel) {
            historia->vidas = 0;
            historia->regeneracao_em_andamento = 1;
        } else if (historia->vidas <= 0) {
            historia->vidas = 0;
            historia->game_over = 1;
            jogo->terminou = 1;
        }
    }
}


/**
 * @brief Acrescenta fragmentos à carteira durante a expedição.
 *
 * A moeda pode ser gasta na Loja da TARDIS para comprar power-ups.
 */
void historiaConcederFragmentos(Historia *historia, int quantidade) {
    if (historia == NULL || quantidade <= 0) {
        return;
    }

    historia->fragmentos += quantidade;
}
