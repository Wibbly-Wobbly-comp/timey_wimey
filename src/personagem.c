/**
 * @file personagem.c
 * @brief Implementa o carregamento e a movimentação do personagem.
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

#include <stdio.h>
#include <math.h>
#include "personagem.h"

/**
 * @brief Carrega os quatro quadros de cada direção.
 */
int iniciarPersonagem(Personagem *personagem) {
    const char *nomes[TOTAL_DIRECOES] = {"frente", "costas", "esquerda", "direita"};
    char caminho[120];
    int carregados = 0;

    if (personagem == NULL) return 0;

    personagem->posicao = (Vector2){484.0f, 465.0f};
    personagem->destino_mouse = personagem->posicao;
    personagem->movendo_mouse = 0;
    personagem->velocidade = 190.0f;
    personagem->tempo_animacao = 0.0f;
    personagem->direcao = FRENTE;
    personagem->quadro_atual = 0;
    personagem->sprites_carregados = 1;

    for (int i = 0; i < TOTAL_DIRECOES; i++) {
        for (int j = 0; j < QUADROS_ANIMACAO; j++) {
            snprintf(caminho, sizeof(caminho), "imagens/personagem/%s/%s_%02d.png", nomes[i], nomes[i], j + 1);
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
    float velocidade;

    if (personagem == NULL) return;
    if (tempo < 0.0f) tempo = 0.0f;
    if (tempo > 0.1f) tempo = 0.1f;
    velocidade = personagem->velocidade;

    /* O teclado tem prioridade e cancela o destino definido pelo mouse. */
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) movimento_x = -1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) movimento_x = 1.0f;
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) movimento_y = -1.0f;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) movimento_y = 1.0f;

    if (movimento_x != 0.0f || movimento_y != 0.0f) {
        personagem->movendo_mouse = 0;
        if (movimento_x < 0) personagem->direcao = ESQUERDA;
        if (movimento_x > 0) personagem->direcao = DIREITA;
        if (movimento_y < 0) personagem->direcao = COSTAS;
        if (movimento_y > 0) personagem->direcao = FRENTE;
    } else if (personagem->movendo_mouse) {
        /* Calcula a direção até o ponto selecionado pelo jogador. */
        float dx = personagem->destino_mouse.x - personagem->posicao.x;
        float dy = personagem->destino_mouse.y - personagem->posicao.y;
        float distancia = sqrtf(dx * dx + dy * dy);

        if (distancia <= 3.0f) {
            personagem->posicao = personagem->destino_mouse;
            personagem->movendo_mouse = 0;
        } else {
            movimento_x = dx / distancia;
            movimento_y = dy / distancia;
            if (fabsf(dx) > fabsf(dy)) {
                personagem->direcao = dx < 0 ? ESQUERDA : DIREITA;
            } else {
                personagem->direcao = dy < 0 ? COSTAS : FRENTE;
            }
        }
    }

    if (movimento_x != 0.0f && movimento_y != 0.0f) {
        velocidade *= 0.7071f;
    }

    /* Evita ultrapassar o ponto escolhido pelo mouse em um unico quadro. */
    if (personagem->movendo_mouse && (movimento_x != 0.0f || movimento_y != 0.0f)) {
        float dx = personagem->destino_mouse.x - personagem->posicao.x;
        float dy = personagem->destino_mouse.y - personagem->posicao.y;
        float distancia = sqrtf(dx * dx + dy * dy);
        float passo = velocidade * tempo;

        if (passo >= distancia) {
            personagem->posicao = personagem->destino_mouse;
            personagem->movendo_mouse = 0;
            movimento_x = 0.0f;
            movimento_y = 0.0f;
        }
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
 * @brief Define o ponto do mapa para onde o personagem deve caminhar.
 * @param personagem Personagem controlado pelo jogador.
 * @param destino Posição final em coordenadas da tela virtual.
 */
void definirDestinoPersonagem(Personagem *personagem, Vector2 destino) {
    if (personagem == NULL) return;

    if (destino.x < 75.0f) destino.x = 75.0f;
    if (destino.x > 893.0f) destino.x = 893.0f;
    if (destino.y < 150.0f) destino.y = 150.0f;
    if (destino.y > 500.0f) destino.y = 500.0f;

    personagem->destino_mouse = destino;
    personagem->movendo_mouse = 1;
}

/**
 * @brief Desenha o personagem na tela.
 */
void desenharPersonagem(Personagem personagem) {
    if (personagem.direcao < 0 || personagem.direcao >= TOTAL_DIRECOES ||
        personagem.quadro_atual < 0 || personagem.quadro_atual >= QUADROS_ANIMACAO) {
        personagem.direcao = FRENTE;
        personagem.quadro_atual = 0;
    }

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
    if (personagem == NULL) return;

    for (int i = 0; i < TOTAL_DIRECOES; i++) {
        for (int j = 0; j < QUADROS_ANIMACAO; j++) {
            if (personagem->quadros[i][j].id != 0) {
                UnloadTexture(personagem->quadros[i][j]);
                personagem->quadros[i][j].id = 0;
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
