/**
 * @file loja.c
 * @brief Implementa as compras temporárias da Loja da TARDIS.
 *
 * Os fragmentos funcionam como moeda apenas durante a expedição atual.
 * Quando uma nova expedição começa, a carteira volta a zero.
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

#include <stddef.h>
#include "loja.h"

/* Preços dos quatro power-ups, na mesma ordem do enum ItemLoja. */
static const int PRECOS[TOTAL_ITENS_LOJA] = {300, 75, 100, 150};

static const char *NOMES[TOTAL_ITENS_LOJA] = {
    "Reparo de Vida",
    "Carga da Chave Sonica",
    "Pacote dos Arquivos",
    "Reversao Temporal"
};

static const char *DESCRICOES[TOTAL_ITENS_LOJA] = {
    "Recupera 1 vida (limite: 5).",
    "Elimina uma alternativa incorreta.",
    "Mostra votos simulados da plateia.",
    "Troca a pergunta apos um erro."
};

int lojaPreco(int item) {
    if (item < 0 || item >= TOTAL_ITENS_LOJA) {
        return 0;
    }

    return PRECOS[item];
}

const char *lojaNome(int item) {
    if (item < 0 || item >= TOTAL_ITENS_LOJA) {
        return "Item desconhecido";
    }

    return NOMES[item];
}

const char *lojaDescricao(int item) {
    if (item < 0 || item >= TOTAL_ITENS_LOJA) {
        return "";
    }

    return DESCRICOES[item];
}

int lojaQuantidade(Historia historia, int item) {
    switch (item) {
        case ITEM_VIDA: return historia.vidas;
        case ITEM_SONICA: return historia.cargas_sonica;
        case ITEM_ARQUIVOS: return historia.cargas_arquivo;
        case ITEM_REVERSAO: return historia.reversoes_temporais;
        default: return 0;
    }
}

enum ResultadoCompra lojaComprar(Historia *historia, int item) {
    int limite;
    int *quantidade;

    if (historia == NULL || item < 0 || item >= TOTAL_ITENS_LOJA) {
        return COMPRA_ITEM_INVALIDO;
    }

    /* Escolhe o recurso que será aumentado e seu limite por expedição. */
    switch (item) {
        case ITEM_VIDA:
            quantidade = &historia->vidas;
            limite = historia->vidas_maximas;
            break;
        case ITEM_SONICA:
            quantidade = &historia->cargas_sonica;
            limite = CARGAS_SONICA_MAXIMAS;
            break;
        case ITEM_ARQUIVOS:
            quantidade = &historia->cargas_arquivo;
            limite = CARGAS_ARQUIVO_MAXIMAS;
            break;
        case ITEM_REVERSAO:
            quantidade = &historia->reversoes_temporais;
            limite = REVERSOES_MAXIMAS;
            break;
        default:
            return COMPRA_ITEM_INVALIDO;
    }

    /* Não cobra fragmentos quando o jogador já atingiu o limite do item. */
    if (*quantidade >= limite) {
        return COMPRA_LIMITE_RECURSO;
    }

    if (historia->fragmentos < lojaPreco(item)) {
        return COMPRA_SEM_FRAGMENTOS;
    }

    historia->fragmentos -= lojaPreco(item);
    (*quantidade)++;

    return COMPRA_SUCESSO;
}
