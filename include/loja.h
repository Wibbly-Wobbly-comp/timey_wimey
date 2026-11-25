/**
 * @file loja.h
 * @brief Produtos e regras da Loja da TARDIS.
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

#ifndef LOJA_H
#define LOJA_H

#include "historia.h"

#define TOTAL_ITENS_LOJA 4

enum ItemLoja {
    ITEM_VIDA = 0,
    ITEM_SONICA,
    ITEM_ARQUIVOS,
    ITEM_REVERSAO
};

enum ResultadoCompra {
    COMPRA_SUCESSO = 1,
    COMPRA_SEM_FRAGMENTOS = 0,
    COMPRA_LIMITE_RECURSO = -1,
    COMPRA_ITEM_INVALIDO = -2
};

/** Retorna o preço de um item da loja. */
int lojaPreco(int item);

/** Retorna o nome de um item da loja. */
const char *lojaNome(int item);

/** Retorna uma descrição curta do efeito do item. */
const char *lojaDescricao(int item);

/** Retorna a quantidade atual do recurso comprado. */
int lojaQuantidade(Historia historia, int item);

/** Compra um power-up, descontando fragmentos e respeitando os limites. */
enum ResultadoCompra lojaComprar(Historia *historia, int item);

#endif
