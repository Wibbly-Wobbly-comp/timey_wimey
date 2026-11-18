/**
 * @file portal.c
 * @brief Implementa a apresentação e a detecção de proximidade dos portais.
 */

#include <math.h>
#include <stdio.h>
#include "portal.h"

/**
 * @brief Define os três portais disponíveis em cada etapa.
 */
void iniciarPortais(Portal portais[]) {
    snprintf(portais[0].nome, sizeof(portais[0].nome), "Alexandria Antiga");
    snprintf(portais[0].descricao, sizeof(portais[0].descricao), "Biblioteca e conhecimentos antigos");
    portais[0].posicao = (Vector2){240.0f, 310.0f};
    portais[0].cor = (Color){70, 175, 255, 255};

    snprintf(portais[1].nome, sizeof(portais[1].nome), "Era Industrial");
    snprintf(portais[1].descricao, sizeof(portais[1].descricao), "Maquinas e revolucao tecnologica");
    portais[1].posicao = (Vector2){500.0f, 310.0f};
    portais[1].cor = (Color){255, 166, 74, 255};

    snprintf(portais[2].nome, sizeof(portais[2].nome), "Futuro Distante");
    snprintf(portais[2].descricao, sizeof(portais[2].descricao), "Uma civilizacao muito avancada");
    portais[2].posicao = (Vector2){760.0f, 310.0f};
    portais[2].cor = (Color){176, 115, 255, 255};
}

/**
 * @brief Desenha os portais e destaca o mais próximo do personagem.
 */
void desenharPortais(Portal portais[], int portal_proximo) {
    for (int i = 0; i < TOTAL_PORTAIS; i++) {
        Color cor = portais[i].cor;
        int raio = (i == portal_proximo) ? 48 : 42;

        DrawCircle((int)portais[i].posicao.x, (int)portais[i].posicao.y, (float)raio, (Color){cor.r, cor.g, cor.b, 45});
        DrawCircleLines((int)portais[i].posicao.x, (int)portais[i].posicao.y, (float)raio, cor);
        DrawCircleLines((int)portais[i].posicao.x, (int)portais[i].posicao.y, (float)(raio - 9), cor);
        DrawText(portais[i].nome, (int)portais[i].posicao.x - 75, (int)portais[i].posicao.y + 58, 18, RAYWHITE);
        DrawText(portais[i].descricao, (int)portais[i].posicao.x - 100, (int)portais[i].posicao.y + 82, 12, LIGHTGRAY);
    }
}

/**
 * @brief Retorna o índice do portal mais próximo dentro do alcance.
 */
int portalProximo(Portal portais[], Vector2 centro) {
    int escolhido = -1;
    float menor_distancia = 65.0f;

    for (int i = 0; i < TOTAL_PORTAIS; i++) {
        float dx = centro.x - portais[i].posicao.x;
        float dy = centro.y - portais[i].posicao.y;
        float distancia = sqrtf(dx * dx + dy * dy);

        if (distancia < menor_distancia) {
            menor_distancia = distancia;
            escolhido = i;
        }
    }

    return escolhido;
}
