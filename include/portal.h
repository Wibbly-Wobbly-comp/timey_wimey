/**
 * @file portal.h
 * @brief Estrutura e funções dos portais temporais.
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

#ifndef PORTAL_H
#define PORTAL_H

#include "raylib.h"
#include "recompensa.h"

#define TOTAL_PORTAIS 3
#define TOTAL_SPRITES_PORTAIS 8
#define TOTAL_MODELOS_PORTAIS 12

typedef struct {
    int id; /* Identificador do destino, usado para evitar visitas repetidas. */
    int indice_sprite; /* Qual dos oito visuais será utilizado. */
    int tipo_recompensa; /* Um dos valores definidos em recompensa.h. */
    int valor_recompensa; /* Quantidade de vidas, cargas ou fragmentos. */
    char nome[48];
    char descricao[100];
    char recompensa[80];
    Vector2 posicao;
    Color cor;
} Portal;

/* Sorteia três destinos para a primeira região. */
void iniciarPortais(Portal portais[]);

/* Gera três destinos diferentes dos já visitados pelo jogador. */
void gerarPortais(Portal portais[], const int ids_visitados[], int total_visitados);

/* Carrega os oito sprites individuais usados na tela de escolha de portais. */
int carregarSpritesPortais(Texture2D imagens[]);

/* Libera as texturas dos portais antes de fechar a janela. */
void liberarSpritesPortais(Texture2D imagens[]);

/* Desenha os três portais com os visuais definidos em cada destino. */
void desenharPortais(Portal portais[], int portal_proximo, Texture2D imagens[]);

/* Retorna o índice do portal mais próximo do personagem ou -1. */
int portalProximo(Portal portais[], Vector2 centro);

#endif
