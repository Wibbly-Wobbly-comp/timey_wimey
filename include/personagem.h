/**
 * @file personagem.h
 * @brief Estrutura e funções do personagem controlado pelo jogador.
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
    Vector2 destino_mouse;
    int movendo_mouse;
    float velocidade;
    float tempo_animacao;
    int direcao;
    int quadro_atual;
    Texture2D quadros[TOTAL_DIRECOES][QUADROS_ANIMACAO];
    int sprites_carregados;
} Personagem;

int iniciarPersonagem(Personagem *personagem);
void atualizarPersonagem(Personagem *personagem, float tempo);
void definirDestinoPersonagem(Personagem *personagem, Vector2 destino);
void desenharPersonagem(Personagem personagem);
void liberarPersonagem(Personagem *personagem);
Vector2 centroPersonagem(Personagem personagem);

#endif
