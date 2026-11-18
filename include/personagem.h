/**
 * @file personagem.h
 * @brief Estrutura e funções do personagem controlado pelo jogador.
 */

#ifndef PERSONAGEM_H
#define PERSONAGEM_H

#include "raylib.h"

#define TOTAL_DIRECOES 4
#define QUADROS_ANIMACAO 4
#define TAM_QUADRO 16
#define ALTURA_QUADRO 32
#define ESCALA_PERSONAGEM 2.0f

enum Direcao {
    FRENTE = 0,
    COSTAS,
    ESQUERDA,
    DIREITA
};

typedef struct {
    Vector2 posicao;
    float velocidade;
    float tempo_animacao;
    int direcao;
    int quadro_atual;
    Texture2D quadros[TOTAL_DIRECOES][QUADROS_ANIMACAO];
    int sprites_carregados;
} Personagem;

int iniciarPersonagem(Personagem *personagem);
void atualizarPersonagem(Personagem *personagem, float tempo);
void desenharPersonagem(Personagem personagem);
void liberarPersonagem(Personagem *personagem);
Vector2 centroPersonagem(Personagem personagem);

#endif
