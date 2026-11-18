/**
 * @file personagem.c
 * @brief Implementa o carregamento e a movimentação do personagem.
 */

#include <stdio.h>
#include "personagem.h"

/**
 * @brief Carrega os quatro quadros de cada direção.
 */
int iniciarPersonagem(Personagem *personagem) {
    const char *nomes[TOTAL_DIRECOES] = {"frente", "costas", "esquerda", "direita"};
    char caminho[120];
    int carregados = 0;

    personagem->posicao = (Vector2){484.0f, 465.0f};
    personagem->velocidade = 190.0f;
    personagem->tempo_animacao = 0.0f;
    personagem->direcao = FRENTE;
    personagem->quadro_atual = 0;
    personagem->sprites_carregados = 1;

    for (int i = 0; i < TOTAL_DIRECOES; i++) {
        for (int j = 0; j < QUADROS_ANIMACAO; j++) {
            sprintf(caminho, "imagens/personagem/%s/%s_%02d.png", nomes[i], nomes[i], j + 1);
            personagem->quadros[i][j] = LoadTexture(caminho);

            if (personagem->quadros[i][j].id == 0) {
                personagem->sprites_carregados = 0;
            } else {
                SetTextureFilter(personagem->quadros[i][j], TEXTURE_FILTER_POINT);
                carregados++;
            }
        }
    }

    return carregados == TOTAL_DIRECOES * QUADROS_ANIMACAO;
}

/**
 * @brief Atualiza a posição do personagem usando as teclas WASD ou setas.
 */
void atualizarPersonagem(Personagem *personagem, float tempo) {
    float movimento_x = 0.0f;
    float movimento_y = 0.0f;
    float velocidade = personagem->velocidade;

    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        movimento_x = -1.0f;
        personagem->direcao = ESQUERDA;
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        movimento_x = 1.0f;
        personagem->direcao = DIREITA;
    }
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
        movimento_y = -1.0f;
        personagem->direcao = COSTAS;
    }
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
        movimento_y = 1.0f;
        personagem->direcao = FRENTE;
    }

    if (movimento_x != 0.0f && movimento_y != 0.0f) {
        velocidade *= 0.7071f;
    }

    personagem->posicao.x += movimento_x * velocidade * tempo;
    personagem->posicao.y += movimento_y * velocidade * tempo;

    if (personagem->posicao.x < 75.0f) personagem->posicao.x = 75.0f;
    if (personagem->posicao.x > 893.0f) personagem->posicao.x = 893.0f;
    if (personagem->posicao.y < 150.0f) personagem->posicao.y = 150.0f;
    if (personagem->posicao.y > 500.0f) personagem->posicao.y = 500.0f;

    if (movimento_x != 0.0f || movimento_y != 0.0f) {
        personagem->tempo_animacao += tempo;
        if (personagem->tempo_animacao >= 0.13f) {
            personagem->quadro_atual = (personagem->quadro_atual + 1) % QUADROS_ANIMACAO;
            personagem->tempo_animacao = 0.0f;
        }
    } else {
        personagem->quadro_atual = 0;
        personagem->tempo_animacao = 0.0f;
    }
}

/**
 * @brief Desenha o personagem na tela.
 */
void desenharPersonagem(Personagem personagem) {
    if (personagem.sprites_carregados && personagem.quadros[personagem.direcao][personagem.quadro_atual].id != 0) {
        DrawTextureEx(personagem.quadros[personagem.direcao][personagem.quadro_atual],
                      personagem.posicao, 0.0f, ESCALA_PERSONAGEM, WHITE);
    } else {
        DrawRectangle((int)personagem.posicao.x + 8, (int)personagem.posicao.y + 18, 16, 30, BROWN);
        DrawRectangle((int)personagem.posicao.x + 4, (int)personagem.posicao.y + 2, 24, 20, BEIGE);
        DrawRectangle((int)personagem.posicao.x + 7, (int)personagem.posicao.y + 23, 18, 8, RED);
    }
}

/**
 * @brief Libera todas as texturas carregadas.
 */
void liberarPersonagem(Personagem *personagem) {
    for (int i = 0; i < TOTAL_DIRECOES; i++) {
        for (int j = 0; j < QUADROS_ANIMACAO; j++) {
            if (personagem->quadros[i][j].id != 0) {
                UnloadTexture(personagem->quadros[i][j]);
            }
        }
    }
}

/**
 * @brief Retorna o centro aproximado do personagem para detectar portais.
 */
Vector2 centroPersonagem(Personagem personagem) {
    return (Vector2){personagem.posicao.x + 16.0f, personagem.posicao.y + 32.0f};
}
