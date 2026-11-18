/**
 * @file portal.h
 * @brief Estrutura e funções dos portais temporais.
 */

#ifndef PORTAL_H
#define PORTAL_H

#include "raylib.h"

#define TOTAL_PORTAIS 3

typedef struct {
    char nome[40];
    char descricao[80];
    Vector2 posicao;
    Color cor;
} Portal;

void iniciarPortais(Portal portais[]);
void desenharPortais(Portal portais[], int portal_proximo);
int portalProximo(Portal portais[], Vector2 centro);

#endif
