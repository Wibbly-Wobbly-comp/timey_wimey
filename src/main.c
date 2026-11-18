/**
 * @file main.c
 * @brief Programa principal do jogo Timey Wimey.
 * @version 0.1
 * @date 2026-11-17
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "raylib.h"
#include "pergunta.h"
#include "jogo.h"
#include "personagem.h"
#include "portal.h"

#define LARGURA_TELA 1000
#define ALTURA_TELA 650
#define ARQUIVO_CSV "dados/perguntas_iniciais.csv"
#define ARQUIVO_BIN "dados/perguntas.bin"
#define ARQUIVO_EXPORTADO "dados/perguntas_exportadas.csv"

enum Telas {
    MENU = 0,
    MAPA,
    PERGUNTA_ATUAL,
    FEEDBACK,
    RESULTADO,
    CONSULTA,
    EXPORTACAO
};

void desenharMenu(int quantidade_perguntas);
void desenharMapa(Personagem personagem, Portal portais[], int portal_proximo, Jogo jogo);
void desenharPergunta(Pergunta pergunta, Jogo jogo, Portal portais[]);
void desenharResultado(Jogo jogo);
void desenharConsulta(ListaPerguntas *lista, enum Dificuldade dificuldade, int indice);
void desenharTextoQuebrado(const char texto[], int x, int y, int tamanho, int largura, Color cor);
Pergunta *perguntaDaDificuldade(ListaPerguntas *lista, enum Dificuldade dificuldade, int indice);
int contarPerguntasJogo(ListaPerguntas *lista);

int main() {
    ListaPerguntas lista;
    Jogo jogo;
    Personagem personagem;
    Portal portais[TOTAL_PORTAIS];
    int tela = MENU;
    int dificuldade_consulta = MUITO_FACIL;
    int indice_consulta = 0;
    int portal_proximo = -1;
    int exportacao_ok = 0;
    int carregamento;
    Sound som_portal;
    Sound som_acerto;
    Sound som_erro;

    iniciarLista(&lista);
    carregamento = carregarPerguntas(&lista, ARQUIVO_CSV, ARQUIVO_BIN);

    if (carregamento < 0) {
        printf("Erro ao abrir o arquivo CSV de perguntas.\n");
        liberarLista(&lista);
        return 1;
    }

    srand((unsigned int)time(NULL));
    InitWindow(LARGURA_TELA, ALTURA_TELA, "Timey Wimey");
    SetExitKey(KEY_NULL);
    InitAudioDevice();
    som_portal = LoadSound("sons/portal.wav");
    som_acerto = LoadSound("sons/acerto.wav");
    som_erro = LoadSound("sons/erro.wav");
    SetTargetFPS(60);

    iniciarPersonagem(&personagem);
    iniciarPortais(portais);

    while (!WindowShouldClose()) {
        float tempo = GetFrameTime();

        if (tela == MENU) {
            if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_ENTER)) {
                if (contarPerguntasJogo(&lista)) {
                    if (iniciarJogo(&jogo, &lista)) {
                        personagem.posicao = (Vector2){484.0f, 465.0f};
                        tela = MAPA;
                    }
                }
            }

            if (IsKeyPressed(KEY_TWO)) {
                dificuldade_consulta = MUITO_FACIL;
                indice_consulta = 0;
                tela = CONSULTA;
            }

            if (IsKeyPressed(KEY_THREE)) {
                exportacao_ok = exportarCSV(&lista, ARQUIVO_EXPORTADO);
                tela = EXPORTACAO;
            }

            if (IsKeyPressed(KEY_FOUR) || IsKeyPressed(KEY_ESCAPE)) {
                break;
            }
        }

        else if (tela == MAPA) {
            atualizarPersonagem(&personagem, tempo);
            portal_proximo = portalProximo(portais, centroPersonagem(personagem));

            if (portal_proximo != -1 && IsKeyPressed(KEY_E)) {
                jogo.portal_escolhido = portal_proximo;
                jogo.portal_por_regiao[jogo.regiao_atual] = portal_proximo;
                PlaySound(som_portal);
                tela = PERGUNTA_ATUAL;
            }

            if (IsKeyPressed(KEY_ESCAPE)) {
                tela = MENU;
            }
        }

        else if (tela == PERGUNTA_ATUAL) {
            Pergunta pergunta = lista.dados[jogo.perguntas_sorteadas[jogo.pergunta_atual]];
            int alternativa = -1;

            if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_ONE)) alternativa = 0;
            if (IsKeyPressed(KEY_B) || IsKeyPressed(KEY_TWO)) alternativa = 1;
            if (IsKeyPressed(KEY_C) || IsKeyPressed(KEY_THREE)) alternativa = 2;
            if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_FOUR)) alternativa = 3;

            if (alternativa != -1) {
                responderPergunta(&jogo, pergunta, alternativa);
                if (jogo.acertou) {
                    PlaySound(som_acerto);
                } else {
                    PlaySound(som_erro);
                }
                tela = FEEDBACK;
            }
        }

        else if (tela == FEEDBACK) {
            if (IsKeyPressed(KEY_ENTER)) {
                if (!jogo.acertou) {
                    tela = RESULTADO;
                } else {
                    proximaPergunta(&jogo);

                    if (jogo.terminou) {
                        tela = RESULTADO;
                    } else if (jogo.pergunta_atual % QUESTOES_POR_REGIAO == 0) {
                        personagem.posicao = (Vector2){484.0f, 465.0f};
                        tela = MAPA;
                    } else {
                        tela = PERGUNTA_ATUAL;
                    }
                }
            }
        }

        else if (tela == RESULTADO) {
            if (IsKeyPressed(KEY_ENTER)) {
                tela = MENU;
            }
        }

        else if (tela == CONSULTA) {
            if (IsKeyPressed(KEY_ONE)) dificuldade_consulta = MUITO_FACIL, indice_consulta = 0;
            if (IsKeyPressed(KEY_TWO)) dificuldade_consulta = FACIL, indice_consulta = 0;
            if (IsKeyPressed(KEY_THREE)) dificuldade_consulta = MEDIO, indice_consulta = 0;
            if (IsKeyPressed(KEY_FOUR)) dificuldade_consulta = DIFICIL, indice_consulta = 0;
            if (IsKeyPressed(KEY_FIVE)) dificuldade_consulta = MUITO_DIFICIL, indice_consulta = 0;

            if (IsKeyPressed(KEY_RIGHT) && indice_consulta < contarDificuldade(&lista, dificuldade_consulta) - 1) {
                indice_consulta++;
            }
            if (IsKeyPressed(KEY_LEFT) && indice_consulta > 0) {
                indice_consulta--;
            }
            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER)) {
                tela = MENU;
            }
        }

        else if (tela == EXPORTACAO) {
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {
                tela = MENU;
            }
        }

        BeginDrawing();
        ClearBackground((Color){13, 18, 32, 255});

        if (tela == MENU) {
            desenharMenu(lista.quantidade);
        }
        else if (tela == MAPA) {
            desenharMapa(personagem, portais, portal_proximo, jogo);
        }
        else if (tela == PERGUNTA_ATUAL) {
            Pergunta pergunta = lista.dados[jogo.perguntas_sorteadas[jogo.pergunta_atual]];
            desenharPergunta(pergunta, jogo, portais);
        }
        else if (tela == FEEDBACK) {
            Pergunta pergunta = lista.dados[jogo.perguntas_sorteadas[jogo.pergunta_atual]];
            if (jogo.acertou) {
                DrawText("RESPOSTA CORRETA!", 300, 220, 36, GREEN);
                DrawText(TextFormat("Voce chegou a R$ %d", jogo.pontuacao), 330, 290, 24, RAYWHITE);
                if (jogo.pergunta_atual == 4 || jogo.pergunta_atual == 9) {
                    DrawText("MARCO DE SEGURANCA ALCANCADO!", 260, 350, 22, GOLD);
                }
            } else {
                DrawText("RESPOSTA INCORRETA!", 280, 210, 36, RED);
                DrawText(TextFormat("A resposta correta era %c", pergunta.resposta_correta), 330, 280, 24, RAYWHITE);
                DrawText(TextFormat("Valor garantido: R$ %d", jogo.valor_garantido), 330, 330, 22, GOLD);
            }
            DrawText("Pressione ENTER para continuar", 330, 520, 20, LIGHTGRAY);
        }
        else if (tela == RESULTADO) {
            desenharResultado(jogo);
        }
        else if (tela == CONSULTA) {
            desenharConsulta(&lista, (enum Dificuldade)dificuldade_consulta, indice_consulta);
        }
        else if (tela == EXPORTACAO) {
            DrawText("EXPORTACAO CSV", 340, 180, 36, RAYWHITE);
            if (exportacao_ok) {
                DrawText("Arquivo salvo em dados/perguntas_exportadas.csv", 190, 290, 22, GREEN);
            } else {
                DrawText("Nao foi possivel exportar o arquivo.", 270, 290, 22, RED);
            }
            DrawText("Pressione ENTER para voltar", 330, 500, 20, LIGHTGRAY);
        }

        EndDrawing();
    }

    salvarBinario(&lista, ARQUIVO_BIN);
    liberarPersonagem(&personagem);
    UnloadSound(som_portal);
    UnloadSound(som_acerto);
    UnloadSound(som_erro);
    CloseAudioDevice();
    CloseWindow();
    liberarLista(&lista);
    return 0;
}

/**
 * @brief Exibe as opções do menu principal.
 */
void desenharMenu(int quantidade_perguntas) {
    DrawText("TIMEY WIMEY", 330, 105, 52, SKYBLUE);
    DrawText("Uma pequena aventura atraves do tempo", 285, 175, 22, LIGHTGRAY);
    DrawText("1 - Iniciar expedicao", 360, 285, 28, RAYWHITE);
    DrawText("2 - Consultar perguntas por dificuldade", 300, 340, 24, RAYWHITE);
    DrawText("3 - Exportar perguntas para CSV", 325, 390, 24, RAYWHITE);
    DrawText("4 - Sair", 430, 440, 24, RAYWHITE);
    DrawText(TextFormat("Perguntas carregadas: %d", quantidade_perguntas), 20, 610, 18, GREEN);
    DrawText("Cadastro completo: execute make gerenciar no terminal", 20, 580, 18, LIGHTGRAY);
}

/**
 * @brief Desenha a sala onde o jogador escolhe um dos três portais.
 */
void desenharMapa(Personagem personagem, Portal portais[], int portal_proximo, Jogo jogo) {
    DrawRectangle(0, 115, LARGURA_TELA, ALTURA_TELA - 115, (Color){24, 37, 48, 255});

    for (int x = 0; x < LARGURA_TELA; x += 40) {
        DrawLine(x, 115, x, ALTURA_TELA, (Color){36, 52, 64, 255});
    }
    for (int y = 115; y < ALTURA_TELA; y += 40) {
        DrawLine(0, y, LARGURA_TELA, y, (Color){36, 52, 64, 255});
    }

    DrawText(TextFormat("SALA TEMPORAL - REGIAO %d DE 3", jogo.regiao_atual + 1), 25, 25, 28, RAYWHITE);
    DrawText("Use WASD ou as setas para se mover. Aproxime-se de um portal e pressione E.", 25, 70, 18, LIGHTGRAY);
    desenharPortais(portais, portal_proximo);
    desenharPersonagem(personagem);

    if (portal_proximo != -1) {
        DrawRectangleRounded((Rectangle){270, 555, 460, 55}, 0.2f, 8, (Color){5, 10, 20, 220});
        DrawText(TextFormat("Pressione E para entrar em %s", portais[portal_proximo].nome), 295, 572, 20, WHITE);
    } else {
        DrawText("Escolha um dos tres portais", 380, 575, 20, LIGHTGRAY);
    }

    DrawText("ESC - voltar ao menu", 790, 25, 16, GRAY);
}

/**
 * @brief Desenha o enunciado e as quatro alternativas.
 */
void desenharPergunta(Pergunta pergunta, Jogo jogo, Portal portais[]) {
    int numero = jogo.pergunta_atual + 1;
    int indice_portal = jogo.portal_por_regiao[jogo.regiao_atual];

    DrawText(TextFormat("PERGUNTA %d / 15", numero), 35, 25, 28, SKYBLUE);
    DrawText(TextFormat("Dificuldade: %s", nomeDificuldade(pergunta.dificuldade)), 35, 65, 20, LIGHTGRAY);
    DrawText(TextFormat("Premio: R$ %d", valorPergunta(numero)), 700, 25, 22, GOLD);

    if (indice_portal >= 0 && indice_portal < TOTAL_PORTAIS) {
        DrawText(TextFormat("Regiao: %s", portais[indice_portal].nome), 35, 105, 20, portais[indice_portal].cor);
    }

    DrawRectangleRounded((Rectangle){35, 160, 930, 150}, 0.08f, 8, (Color){29, 42, 62, 255});
    desenharTextoQuebrado(pergunta.enunciado, 60, 195, 25, 880, RAYWHITE);

    for (int i = 0; i < TOTAL_ALTERNATIVAS; i++) {
        int x = (i % 2 == 0) ? 45 : 515;
        int y = (i < 2) ? 355 : 455;
        DrawRectangleRounded((Rectangle){(float)x, (float)y, 440, 72}, 0.12f, 8, (Color){37, 55, 78, 255});
        char texto_alternativa[180];
        snprintf(texto_alternativa, sizeof(texto_alternativa), "%c) %s", 'A' + i, pergunta.alternativas[i]);
        desenharTextoQuebrado(texto_alternativa, x + 18, y + 18, 16, 400, WHITE);
    }

    DrawText("Responda com A, B, C ou D (tambem funciona com 1, 2, 3 ou 4).", 190, 575, 19, LIGHTGRAY);
}

/**
 * @brief Desenha a tela final com o valor conquistado.
 */
void desenharResultado(Jogo jogo) {
    if (jogo.acertou && jogo.pergunta_atual == TOTAL_QUESTOES_JOGO - 1) {
        DrawText("EXPEDICAO CONCLUIDA!", 260, 175, 40, GREEN);
        DrawText("Voce respondeu as 15 perguntas corretamente.", 245, 245, 24, RAYWHITE);
    } else {
        DrawText("FIM DE JOGO", 355, 175, 42, RED);
        DrawText("A viagem temporal foi interrompida.", 285, 245, 24, RAYWHITE);
    }

    DrawText(TextFormat("Valor conquistado: R$ %d", valorFinal(jogo)), 300, 340, 30, GOLD);
    DrawText("Pressione ENTER para voltar ao menu", 300, 500, 21, LIGHTGRAY);
}

/**
 * @brief Desenha uma pergunta da coleção e permite navegar por dificuldade.
 */
void desenharConsulta(ListaPerguntas *lista, enum Dificuldade dificuldade, int indice) {
    Pergunta *pergunta = perguntaDaDificuldade(lista, dificuldade, indice);
    int total = contarDificuldade(lista, dificuldade);

    DrawText("CONSULTA DE PERGUNTAS", 300, 30, 34, SKYBLUE);
    DrawText("Escolha o nivel com as teclas 1 a 5", 330, 90, 20, LIGHTGRAY);
    DrawText("1 Muito facil | 2 Facil | 3 Medio | 4 Dificil | 5 Muito dificil", 145, 130, 18, RAYWHITE);

    DrawText(TextFormat("Nivel: %s", nomeDificuldade(dificuldade)), 60, 205, 26, GOLD);
    DrawText(TextFormat("Pergunta %d de %d", total == 0 ? 0 : indice + 1, total), 730, 205, 22, LIGHTGRAY);

    if (pergunta != NULL) {
        DrawRectangleRounded((Rectangle){45, 250, 910, 260}, 0.08f, 8, (Color){29, 42, 62, 255});
        DrawText(TextFormat("ID: %d", pergunta->id), 70, 275, 18, GRAY);
        desenharTextoQuebrado(pergunta->enunciado, 70, 320, 23, 850, WHITE);

        for (int i = 0; i < TOTAL_ALTERNATIVAS; i++) {
            DrawText(TextFormat("%c) %s", 'A' + i, pergunta->alternativas[i]), 75, 410 + i * 25, 17, RAYWHITE);
        }
    } else {
        DrawText("Nao ha perguntas cadastradas neste nivel.", 260, 350, 24, LIGHTGRAY);
    }

    DrawText("Setas esquerda/direita: navegar | ENTER ou ESC: voltar", 240, 575, 19, LIGHTGRAY);
}

/**
 * @brief Desenha um texto em várias linhas dentro de uma largura definida.
 */
void desenharTextoQuebrado(const char texto[], int x, int y, int tamanho, int largura, Color cor) {
    char copia[TAM_ENUNCIADO];
    char linha[300] = "";
    char *palavra;
    int altura_linha = tamanho + 8;

    snprintf(copia, sizeof(copia), "%s", texto);
    palavra = strtok(copia, " ");

    while (palavra != NULL) {
        char teste[300];
        if (linha[0] == '\0') {
            snprintf(teste, sizeof(teste), "%s", palavra);
        } else {
            snprintf(teste, sizeof(teste), "%s %s", linha, palavra);
        }

        if (MeasureText(teste, tamanho) > largura && linha[0] != '\0') {
            DrawText(linha, x, y, tamanho, cor);
            y += altura_linha;
            snprintf(linha, sizeof(linha), "%s", palavra);
        } else {
            snprintf(linha, sizeof(linha), "%s", teste);
        }

        palavra = strtok(NULL, " ");
    }

    if (linha[0] != '\0') {
        DrawText(linha, x, y, tamanho, cor);
    }
}

/**
 * @brief Retorna a pergunta de uma posição dentro de um nível de dificuldade.
 */
Pergunta *perguntaDaDificuldade(ListaPerguntas *lista, enum Dificuldade dificuldade, int indice) {
    int encontrados = 0;

    for (int i = 0; i < lista->quantidade; i++) {
        if (lista->dados[i].dificuldade == dificuldade) {
            if (encontrados == indice) {
                return &lista->dados[i];
            }
            encontrados++;
        }
    }

    return NULL;
}

/**
 * @brief Confere se a coleção possui perguntas suficientes em cada nível.
 */
int contarPerguntasJogo(ListaPerguntas *lista) {
    return contarDificuldade(lista, MUITO_FACIL) >= 2 &&
           contarDificuldade(lista, FACIL) >= 2 &&
           contarDificuldade(lista, MEDIO) >= 4 &&
           contarDificuldade(lista, DIFICIL) >= 4 &&
           contarDificuldade(lista, MUITO_DIFICIL) >= 3;
}
