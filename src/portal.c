/**
 * @file portal.c
 * @brief Implementa os destinos, as imagens e a interação com os portais.
 *
 * Os modelos ficam separados da tela. Assim, o jogo pode sortear novos
 * destinos sem precisar alterar a lógica de desenho ou movimentação.
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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "portal.h"
#include "jogo.h"

/* Cada modelo representa um destino que pode aparecer durante a História. */
typedef struct {
    int id;
    int indice_sprite;
    int tipo_recompensa;
    int valor_recompensa;
    const char *nome;
    const char *descricao;
    const char *recompensa;
    Color cor;
} ModeloPortal;

static const ModeloPortal MODELOS_PORTAIS[TOTAL_MODELOS_PORTAIS] = {
    {1, 0, RECOMPENSA_ARQUIVOS, 1, "Biblioteca de Alexandria",
     "Manuscritos e conhecimentos do mundo antigo", "+1 carga dos Arquivos da TARDIS", {70, 175, 255, 255}},
    {2, 1, RECOMPENSA_FRAGMENTOS, 30, "Atenas Classica",
     "Filosofia, logica e pensamento critico", "+30 fragmentos temporais", {70, 210, 190, 255}},
    {3, 2, RECOMPENSA_SONICA, 1, "Roma Imperial",
     "Engenharia, politica e expansao do imperio", "+1 carga da Chave Sonica", {235, 175, 105, 255}},
    {4, 3, RECOMPENSA_REVERSAO, 1, "Mosteiro das Eras",
     "Um arquivo isolado guarda conhecimentos esquecidos", "+1 Reversao Temporal", {165, 135, 235, 255}},
    {5, 4, RECOMPENSA_ARQUIVOS, 1, "Florença, 1504",
     "Arte, anatomia e novas formas de investigar", "+1 carga dos Arquivos da TARDIS", {225, 180, 110, 255}},
    {6, 5, RECOMPENSA_VIDA, 1, "Londres, 1888",
     "A era do vapor e das grandes transformacoes", "+1 vida", {235, 145, 85, 255}},
    {7, 6, RECOMPENSA_FRAGMENTOS, 60, "Frente de Gallifrey",
     "Uma fratura instavel na Guerra do Tempo", "+60 fragmentos temporais", {190, 80, 125, 255}},
    {8, 7, RECOMPENSA_SONICA, 1, "Orbita Terrestre, 1969",
     "A humanidade se prepara para alcancar a Lua", "+1 carga da Chave Sonica", {90, 170, 230, 255}},
    {9, 0, RECOMPENSA_VIDA, 1, "Estacao Temporal 3000",
     "Uma estacao abandonada fora da linha do tempo", "+1 vida", {85, 190, 220, 255}},
    {10, 2, RECOMPENSA_REVERSAO, 1, "Fenda de Cardiff",
     "Uma anomalia temporal atravessa a cidade", "+1 Reversao Temporal", {90, 125, 235, 255}},
    {11, 4, RECOMPENSA_FRAGMENTOS, 45, "Pompeia, 79 d.C.",
     "Uma cidade diante de um acontecimento historico", "+45 fragmentos temporais", {225, 125, 85, 255}},
    {12, 7, RECOMPENSA_ARQUIVOS, 1, "Nova Nova York",
     "Uma metropole futurista cheia de informacoes", "+1 carga dos Arquivos da TARDIS", {125, 100, 230, 255}}
};

static const Vector2 POSICOES_PORTAIS[TOTAL_PORTAIS] = {
    {240.0f, 310.0f},
    {500.0f, 310.0f},
    {760.0f, 310.0f}
};

/**
 * @brief Verifica se um destino já foi escolhido em uma região anterior.
 */
static int portalJaVisitado(int id, const int ids_visitados[], int total_visitados) {
    if (ids_visitados == NULL || total_visitados <= 0) return 0;

    for (int i = 0; i < total_visitados; i++) {
        if (ids_visitados[i] == id) {
            return 1;
        }
    }

    return 0;
}

/**
 * @brief Copia as informações de um modelo para o portal desenhado na tela.
 */
static void copiarModelo(Portal *portal, const ModeloPortal *modelo, int posicao) {
    portal->id = modelo->id;
    portal->indice_sprite = modelo->indice_sprite;
    portal->tipo_recompensa = modelo->tipo_recompensa;
    portal->valor_recompensa = modelo->valor_recompensa;
    snprintf(portal->nome, sizeof(portal->nome), "%s", modelo->nome);
    snprintf(portal->descricao, sizeof(portal->descricao), "%s", modelo->descricao);
    snprintf(portal->recompensa, sizeof(portal->recompensa), "%s", modelo->recompensa);
    portal->posicao = POSICOES_PORTAIS[posicao];
    portal->cor = modelo->cor;
}

/**
 * @brief Gera três opções aleatórias, evitando os destinos já escolhidos.
 *
 * O jogo possui doze modelos e dez regiões. Como apenas um destino é
 * escolhido em cada região, é possível terminar a expedição sem repetir
 * uma época já visitada.
 */
void gerarPortais(Portal portais[], const int ids_visitados[], int total_visitados) {
    int candidatos[TOTAL_MODELOS_PORTAIS];
    int quantidade = 0;

    if (portais == NULL) return;
    if (ids_visitados == NULL || total_visitados < 0) total_visitados = 0;
    if (total_visitados > TOTAL_REGIOES_HISTORIA) {
        total_visitados = TOTAL_REGIOES_HISTORIA;
    }

    for (int i = 0; i < TOTAL_MODELOS_PORTAIS; i++) {
        if (!portalJaVisitado(MODELOS_PORTAIS[i].id, ids_visitados, total_visitados)) {
            candidatos[quantidade] = i;
            quantidade++;
        }
    }

    /* Em uma situação inesperada, permite repetir destinos para não travar o jogo. */
    if (quantidade < TOTAL_PORTAIS) {
        quantidade = 0;
        for (int i = 0; i < TOTAL_MODELOS_PORTAIS; i++) {
            candidatos[quantidade] = i;
            quantidade++;
        }
    }

    /* Embaralha os candidatos e utiliza os três primeiros. */
    for (int i = quantidade - 1; i > 0; i--) {
        int sorteado = rand() % (i + 1);
        int temporario = candidatos[i];
        candidatos[i] = candidatos[sorteado];
        candidatos[sorteado] = temporario;
    }

    for (int i = 0; i < TOTAL_PORTAIS; i++) {
        copiarModelo(&portais[i], &MODELOS_PORTAIS[candidatos[i]], i);
    }
}

/**
 * @brief Sorteia os três destinos disponíveis no início de uma expedição.
 */
void iniciarPortais(Portal portais[]) {
    if (portais == NULL) return;
    gerarPortais(portais, NULL, 0);
}

/**
 * @brief Carrega as oito imagens que compõem o conjunto de portais.
 *
 * As imagens são carregadas uma única vez, durante a inicialização do jogo.
 * O filtro POINT mantém os pixels definidos quando o sprite é ampliado.
 */
int carregarSpritesPortais(Texture2D imagens[]) {
    int carregadas = 0;

    if (imagens == NULL) return 0;

    for (int i = 0; i < TOTAL_SPRITES_PORTAIS; i++) {
        char caminho[80];
        snprintf(caminho, sizeof(caminho), "imagens/portais/portal_%02d.png", i + 1);
        imagens[i] = LoadTexture(caminho);

        if (imagens[i].id != 0) {
            SetTextureFilter(imagens[i], TEXTURE_FILTER_POINT);
            carregadas++;
        }
    }

    return carregadas;
}

/**
 * @brief Libera as texturas dos portais antes de fechar a janela.
 */
void liberarSpritesPortais(Texture2D imagens[]) {
    if (imagens == NULL) return;

    for (int i = 0; i < TOTAL_SPRITES_PORTAIS; i++) {
        if (imagens[i].id != 0) {
            UnloadTexture(imagens[i]);
            imagens[i].id = 0;
        }
    }
}

/**
 * @brief Desenha os portais e destaca aquele que está mais próximo.
 */
void desenharPortais(Portal portais[], int portal_proximo, Texture2D imagens[]) {
    if (portais == NULL || imagens == NULL) return;

    for (int i = 0; i < TOTAL_PORTAIS; i++) {
        float x = portais[i].posicao.x;
        float y = portais[i].posicao.y;
        Color cor = portais[i].cor;
        int sprite = portais[i].indice_sprite;

        DrawCircle((int)x, (int)y, (i == portal_proximo) ? 72.0f : 66.0f,
                   (Color){cor.r, cor.g, cor.b, 35});

        if (sprite >= 0 && sprite < TOTAL_SPRITES_PORTAIS && imagens[sprite].id != 0) {
            Rectangle origem = {0.0f, 0.0f,
                                (float)imagens[sprite].width,
                                (float)imagens[sprite].height};
            Rectangle destino = {x - 63.0f, y - 62.0f, 126.0f, 124.0f};
            DrawTexturePro(imagens[sprite], origem, destino,
                           (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
        } else {
            /* Desenho alternativo se algum arquivo de imagem não for encontrado. */
            DrawCircle((int)x, (int)y, 48.0f, (Color){cor.r, cor.g, cor.b, 90});
            DrawCircleLines((int)x, (int)y, 48.0f, cor);
            DrawCircleLines((int)x, (int)y, 38.0f, cor);
        }

        if (i == portal_proximo) {
            DrawCircleLines((int)x, (int)y, 69.0f, WHITE);
            DrawCircleLines((int)x, (int)y, 73.0f, cor);
        }

        DrawText(portais[i].nome,
                 (int)(x - (float)MeasureText(portais[i].nome, 16) / 2.0f),
                 (int)y + 65, 16, RAYWHITE);
        DrawText(portais[i].descricao,
                 (int)(x - (float)MeasureText(portais[i].descricao, 11) / 2.0f),
                 (int)y + 85, 11, LIGHTGRAY);
        DrawText(portais[i].recompensa,
                 (int)(x - (float)MeasureText(portais[i].recompensa, 12) / 2.0f),
                 (int)y + 101, 12, GOLD);
    }
}

/**
 * @brief Retorna o índice do portal mais próximo dentro do alcance.
 */
int portalProximo(Portal portais[], Vector2 centro) {
    int escolhido = -1;
    float menor_distancia = 65.0f;

    if (portais == NULL) return -1;

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
