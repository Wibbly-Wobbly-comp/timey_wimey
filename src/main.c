/**
 * @file main.c
 * @brief Programa principal do jogo Timey Wimey.
 *
 * O programa possui dois modos:
 * - Historia: oito regioes de exploracao e batalha final nas dez ultimas perguntas.
 * - Versus: disputa de 15 perguntas entre o jogador e o computador.
 *
 * As demais opções permitem consultar e exportar a coleção de perguntas.
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

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "raylib.h"
#include "pergunta.h"
#include "jogo.h"
#include "personagem.h"
#include "portal.h"
#include "versus.h"
#include "historia.h"
#include "chefe.h"
#include "loja.h"

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
    EXPORTACAO,
    ENTRADA_CHEFE,
    LOJA
};

enum ModosJogo {
    MODO_HISTORIA = 1,
    MODO_VERSUS
};

void desenharMenu(int quantidade_perguntas, Texture2D fundo_menu);
void desenharMapa(Personagem personagem, Portal portais[], int portal_proximo, Jogo jogo, Historia historia, Texture2D imagens_portais[], Texture2D imagem_loja);
void desenharPergunta(Pergunta pergunta, Jogo jogo, Portal portais[], int modo, Versus versus, Historia historia, Chefe chefe, Texture2D imagens_chefe[], Texture2D fundos_mundos[], Texture2D imagens_dicas[]);
void desenharResultadoHistoria(Jogo jogo, Historia historia, Chefe chefe, Texture2D imagens_chefe[]);
void desenharFeedbackHistoria(Jogo jogo, Historia historia, Pergunta pergunta, Chefe chefe, Texture2D imagens_chefe[], Texture2D fundos_mundos[], Texture2D imagens_dicas[]);
void desenharEntradaChefe(Chefe chefe, Texture2D imagens_chefe[]);
void desenharVisualChefe(int x, int y, int raio, int fase);
void desenharSpriteChefe(Texture2D imagens_chefe[], int centro_x, int centro_y, float altura, int fase);
void desenharPainelChefe(Chefe chefe, int indice_pergunta, Historia historia, Texture2D imagens_chefe[]);
void desenharResultadoVersus(Versus versus);
void desenharFeedbackVersus(Jogo jogo, Versus versus, Pergunta pergunta);
void desenharConsulta(ListaPerguntas *lista, enum Dificuldade dificuldade, int indice);
void desenharTextoQuebrado(const char texto[], int x, int y, int tamanho, int largura, Color cor);
void desenharLoja(Historia historia, Texture2D imagens_dicas[], Texture2D imagem_loja, const char mensagem[]);
void desenharIconeDica(Texture2D imagem, Rectangle area, Color cor);
Pergunta *perguntaDaDificuldade(ListaPerguntas *lista, enum Dificuldade dificuldade, int indice);
int contarPerguntasVersus(ListaPerguntas *lista);
int contarPerguntasHistoria(ListaPerguntas *lista);
Pergunta perguntaAtual(ListaPerguntas *lista, Jogo jogo, Historia historia);
Vector2 obterMouseVirtual(void);
int mouseSobre(Vector2 mouse, Rectangle area);
void entrarNoPortal(int indice, Portal portais[], Jogo *jogo, Historia *historia,
                    Sound som_portal, int *tela);
Texture2D carregarFundo(const char caminho[]);
void carregarFundosMundos(Texture2D fundos[]);
void liberarFundosMundos(Texture2D fundos[]);
void tocarSom(Sound som);
void desenharFundoMundo(Texture2D fundos[], int id_mundo);
void desenharFundoDaRegiao(Texture2D fundos[], Jogo jogo);

int main(void) {
    ListaPerguntas lista;
    Jogo jogo;
    Versus versus;
    Historia historia;
    Chefe chefe;
    Personagem personagem;
    Portal portais[TOTAL_PORTAIS];
    Texture2D imagens_portais[TOTAL_SPRITES_PORTAIS] = {0};
    Texture2D imagens_chefe[2] = {0};
    Texture2D fundo_menu = {0};
    Texture2D imagem_loja = {0};
    Texture2D fundos_mundos[TOTAL_MODELOS_PORTAIS] = {0};
    Texture2D imagens_dicas[3] = {0};
    RenderTexture2D tela_virtual;

    int tela = MENU;
    int modo_jogo = MODO_HISTORIA;
    int dificuldade_consulta = MUITO_FACIL;
    int indice_consulta = 0;
    int portal_proximo = -1;
    int portal_alvo_mouse = -1;
    int exportacao_ok = 0;
    int carregamento;
    int quantidade_sprites_portais;
    char mensagem_loja[140] = "Escolha um recurso para comprar.";

    Sound som_portal = {0};
    Sound som_acerto = {0};
    Sound som_erro = {0};

    /* Carrega as perguntas do arquivo binário ou, na primeira execução, do CSV. */
    iniciarLista(&lista);
    carregamento = carregarPerguntas(&lista, ARQUIVO_CSV, ARQUIVO_BIN);

    if (carregamento < 0) {
        printf("Erro ao abrir o arquivo CSV de perguntas.\n");
        liberarLista(&lista);
        return 1;
    }

    /* Inicializa a janela redimensionável e a tela virtual do jogo.
       A tela virtual mantém as proporções originais em qualquer resolução. */
    srand((unsigned int)time(NULL));
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(LARGURA_TELA, ALTURA_TELA, "Timey Wimey");

    if (!IsWindowReady()) {
        printf("Erro: nao foi possivel inicializar a janela da Raylib.\n");
        liberarLista(&lista);
        return 1;
    }

    SetWindowMinSize(800, 520);
    tela_virtual = LoadRenderTexture(LARGURA_TELA, ALTURA_TELA);

    if (tela_virtual.id == 0) {
        printf("Erro: nao foi possivel criar a tela virtual do jogo.\n");
        CloseWindow();
        liberarLista(&lista);
        return 1;
    }

    SetTextureFilter(tela_virtual.texture, TEXTURE_FILTER_POINT);
    SetExitKey(KEY_NULL);
    InitAudioDevice();
    if (!IsAudioDeviceReady()) {
        printf("Aviso: dispositivo de audio indisponivel. O jogo continuara sem sons.\n");
    }

    /* So tenta carregar sons quando o dispositivo de audio esta disponivel. */
    if (IsAudioDeviceReady()) {
        som_portal = LoadSound("sons/portal.wav");
        som_acerto = LoadSound("sons/acerto.wav");
        som_erro = LoadSound("sons/erro.wav");
    }
    SetTargetFPS(60);

    /* Carrega o fundo ilustrado do menu principal e o icone opcional da loja. */
    fundo_menu = carregarFundo("imagens/fundos/menu.png");
    imagem_loja = carregarFundo("imagens/loja/loja_tardis.png");

    /* Tenta carregar os doze fundos dos mundos. Os arquivos podem ser
       adicionados depois, sem necessidade de alterar o codigo do jogo. */
    carregarFundosMundos(fundos_mundos);

    /* Carrega as artes das três dicas do Modo História. */
    imagens_dicas[0] = LoadTexture("imagens/dicas/chave_sonica.png");
    imagens_dicas[1] = LoadTexture("imagens/dicas/arquivos_tardis.png");
    imagens_dicas[2] = LoadTexture("imagens/dicas/reversao_temporal.png");
    for (int i = 0; i < 3; i++) {
        if (imagens_dicas[i].id != 0) {
            SetTextureFilter(imagens_dicas[i], TEXTURE_FILTER_POINT);
        } else {
            printf("Aviso: imagem da dica %d nao foi encontrada.\n", i + 1);
        }
    }

    iniciarPersonagem(&personagem);
    iniciarPortais(portais);

    /* Carrega os oito visuais dos portais uma unica vez. */
    quantidade_sprites_portais = carregarSpritesPortais(imagens_portais);
    if (quantidade_sprites_portais < TOTAL_SPRITES_PORTAIS) {
        printf("Aviso: %d de %d sprites de portais foram carregados.\n",
               quantidade_sprites_portais, TOTAL_SPRITES_PORTAIS);
    }

    /* Carrega as duas aparencias oficiais de O Paradoxo.
       A primeira e usada na fase de Instabilidade; a segunda, no Colapso. */
    imagens_chefe[0] = LoadTexture("imagens/chefe/paradoxo_fase1.png");
    imagens_chefe[1] = LoadTexture("imagens/chefe/paradoxo_fase2.png");
    for (int i = 0; i < 2; i++) {
        if (imagens_chefe[i].id != 0) {
            SetTextureFilter(imagens_chefe[i], TEXTURE_FILTER_POINT);
        } else {
            printf("Aviso: nao foi possivel carregar a imagem da fase %d do chefe.\n", i + 1);
        }
    }

    /* A estrutura Versus também é inicializada para evitar valores indefinidos. */
    iniciarVersus(&versus);
    iniciarHistoria(&historia);
    prepararChefe(&chefe);

    /* O laço principal atualiza os controles e desenha a tela atual. */
    while (!WindowShouldClose()) {
        float tempo = GetFrameTime();

        /* F11 alterna entre janela e tela cheia em qualquer tela do jogo. */
        if (IsKeyPressed(KEY_F11)) {
            ToggleFullscreen();
        }

        /* -------------------- MENU PRINCIPAL -------------------- */
        if (tela == MENU) {
            /* As opções também podem ser selecionadas com o botão esquerdo do mouse. */
            Vector2 mouse = obterMouseVirtual();
            int clique = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

            /* Modo História precisa de 10 perguntas de cada dificuldade. */
            if (IsKeyPressed(KEY_ONE) || (clique && mouseSobre(mouse, (Rectangle){55, 200, 430, 76}))) {
                if (contarPerguntasHistoria(&lista) &&
                    iniciarJogo(&jogo, &lista, TOTAL_QUESTOES_HISTORIA)) {
                    modo_jogo = MODO_HISTORIA;
                    iniciarHistoria(&historia);
                    prepararChefe(&chefe);
                    iniciarPortais(portais);
                    personagem.posicao = (Vector2){484.0f, 465.0f};
                    personagem.movendo_mouse = 0;
                    portal_alvo_mouse = -1;
                    tela = MAPA;
                }
            }

            /* Modo Versus mantém a sequência tradicional de 15 perguntas. */
            if (IsKeyPressed(KEY_TWO) || (clique && mouseSobre(mouse, (Rectangle){55, 290, 430, 76}))) {
                if (contarPerguntasVersus(&lista) &&
                    iniciarJogo(&jogo, &lista, TOTAL_QUESTOES_VERSUS)) {
                    modo_jogo = MODO_VERSUS;
                    iniciarVersus(&versus);
                    tela = PERGUNTA_ATUAL;
                }
            }

            /* Consulta permite navegar pelas perguntas de cada dificuldade. */
            if (IsKeyPressed(KEY_THREE) || (clique && mouseSobre(mouse, (Rectangle){55, 385, 205, 50}))) {
                dificuldade_consulta = MUITO_FACIL;
                indice_consulta = 0;
                tela = CONSULTA;
            }

            /* Exporta a coleção atual para um arquivo CSV. */
            if (IsKeyPressed(KEY_FOUR) || (clique && mouseSobre(mouse, (Rectangle){275, 385, 210, 50}))) {
                exportacao_ok = exportarCSV(&lista, ARQUIVO_EXPORTADO);
                tela = EXPORTACAO;
            }

            if (IsKeyPressed(KEY_FIVE) || IsKeyPressed(KEY_ESCAPE) ||
                (clique && mouseSobre(mouse, (Rectangle){55, 450, 430, 50}))) {
                break;
            }
        }

        /* -------------------- MODO HISTÓRIA: MAPA -------------------- */
        else if (tela == MAPA) {
            Vector2 mouse = obterMouseVirtual();
            atualizarPersonagem(&personagem, tempo);
            portal_proximo = portalProximo(portais, centroPersonagem(personagem));

            /*
             * Clicar em um portal faz o personagem caminhar ate ele.
             * Quando chega perto, a entrada acontece automaticamente.
             * O movimento por WASD/setas continua funcionando normalmente.
             */
            /* A loja pode ser aberta sem sair da tela de escolha dos portais. */
            if (IsKeyPressed(KEY_L) ||
                (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
                 mouseSobre(mouse, (Rectangle){24, 132, 240, 72}))) {
                tela = LOJA;
                portal_alvo_mouse = -1;
                personagem.movendo_mouse = 0;
                snprintf(mensagem_loja, sizeof(mensagem_loja),
                         "Escolha um recurso para comprar.");
            }
            /* Cliques fora da loja continuam selecionando um portal. */
            else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                for (int i = 0; i < TOTAL_PORTAIS; i++) {
                    Rectangle area_portal = {
                        portais[i].posicao.x - 68.0f,
                        portais[i].posicao.y - 68.0f,
                        136.0f,
                        136.0f
                    };

                    if (mouseSobre(mouse, area_portal)) {
                        portal_alvo_mouse = i;
                        definirDestinoPersonagem(&personagem,
                            (Vector2){portais[i].posicao.x - 16.0f,
                                      portais[i].posicao.y - 32.0f});
                        break;
                    }
                }
            }

            /* O teclado ainda permite entrar no portal usando E. */
            if (portal_proximo != -1 && IsKeyPressed(KEY_E)) {
                entrarNoPortal(portal_proximo, portais, &jogo, &historia,
                               som_portal, &tela);
                portal_alvo_mouse = -1;
            }
            /* Ao chegar ao portal escolhido com o mouse, entra automaticamente. */
            else if (portal_alvo_mouse != -1 && portal_proximo == portal_alvo_mouse) {
                entrarNoPortal(portal_alvo_mouse, portais, &jogo, &historia,
                               som_portal, &tela);
                portal_alvo_mouse = -1;
            }

            if (IsKeyPressed(KEY_ESCAPE)) {
                tela = MENU;
                portal_alvo_mouse = -1;
            }
        }

        /* -------------------- LOJA DA TARDIS -------------------- */
        else if (tela == LOJA) {
            Vector2 mouse = obterMouseVirtual();
            int clique = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
            int item_escolhido = -1;

            if (IsKeyPressed(KEY_ONE)) item_escolhido = ITEM_VIDA;
            if (IsKeyPressed(KEY_TWO)) item_escolhido = ITEM_SONICA;
            if (IsKeyPressed(KEY_THREE)) item_escolhido = ITEM_ARQUIVOS;
            if (IsKeyPressed(KEY_FOUR)) item_escolhido = ITEM_REVERSAO;

            if (clique) {
                Rectangle areas_itens[TOTAL_ITENS_LOJA] = {
                    {55, 175, 430, 125}, {515, 175, 430, 125},
                    {55, 320, 430, 125}, {515, 320, 430, 125}
                };

                for (int i = 0; i < TOTAL_ITENS_LOJA; i++) {
                    if (mouseSobre(mouse, areas_itens[i])) {
                        item_escolhido = i;
                        break;
                    }
                }
            }

            /* ESC ou o botão inferior retorna ao mapa com os portais atuais. */
            if (IsKeyPressed(KEY_ESCAPE) ||
                (clique && mouseSobre(mouse, (Rectangle){785, 555, 170, 50}))) {
                tela = MAPA;
            } else if (item_escolhido != -1) {
                enum ResultadoCompra resultado = lojaComprar(&historia, item_escolhido);

                if (resultado == COMPRA_SUCESSO) {
                    snprintf(mensagem_loja, sizeof(mensagem_loja),
                             "%s comprado!", lojaNome(item_escolhido));
                    tocarSom(som_acerto);
                } else if (resultado == COMPRA_SEM_FRAGMENTOS) {
                    snprintf(mensagem_loja, sizeof(mensagem_loja),
                             "Fragmentos insuficientes. Voce precisa de %d.",
                             lojaPreco(item_escolhido));
                    tocarSom(som_erro);
                } else if (resultado == COMPRA_LIMITE_RECURSO) {
                    snprintf(mensagem_loja, sizeof(mensagem_loja),
                             "Voce ja atingiu o limite deste recurso.");
                    tocarSom(som_erro);
                } else {
                    snprintf(mensagem_loja, sizeof(mensagem_loja),
                             "Nao foi possivel concluir a compra.");
                    tocarSom(som_erro);
                }
            }
        }

        /* -------------------- PERGUNTA DOS DOIS MODOS -------------------- */
        else if (tela == PERGUNTA_ATUAL) {
            Pergunta pergunta = perguntaAtual(&lista, jogo, historia);
            int alternativa = -1;

            /* Cada modo possui dicas próprias, inspiradas no seu universo. */
            if (modo_jogo == MODO_VERSUS) {
                if (IsKeyPressed(KEY_F)) usarCinquenta(&versus, pergunta);
                if (IsKeyPressed(KEY_P)) usarPlateia(&versus, pergunta);
            } else if (!historia.regeneracao_em_andamento) {
                if (IsKeyPressed(KEY_S)) historiaUsarSonic(&historia, pergunta);
                if (IsKeyPressed(KEY_H)) historiaUsarArquivo(&historia, pergunta);
            }

            /* As dicas também podem ser acionadas clicando em seus nomes. */
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 mouse = obterMouseVirtual();

                if (modo_jogo == MODO_VERSUS) {
                    if (mouseSobre(mouse, (Rectangle){45, 505, 175, 38})) usarCinquenta(&versus, pergunta);
                    if (mouseSobre(mouse, (Rectangle){235, 505, 230, 38})) usarPlateia(&versus, pergunta);
                } else if (!historia.regeneracao_em_andamento) {
                    if (mouseSobre(mouse, (Rectangle){220, 495, 240, 65})) historiaUsarSonic(&historia, pergunta);
                    if (mouseSobre(mouse, (Rectangle){475, 495, 240, 65})) historiaUsarArquivo(&historia, pergunta);
                }
            }

            /* Lê a alternativa escolhida pelo teclado. */
            if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_ONE)) alternativa = 0;
            if (IsKeyPressed(KEY_B) || IsKeyPressed(KEY_TWO)) alternativa = 1;
            if (IsKeyPressed(KEY_C) || IsKeyPressed(KEY_THREE)) alternativa = 2;
            if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_FOUR)) alternativa = 3;

            /* Também é possível escolher uma alternativa clicando em seu cartão. */
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 mouse = obterMouseVirtual();
                for (int i = 0; i < TOTAL_ALTERNATIVAS; i++) {
                    int x = (i % 2 == 0) ? 45 : 515;
                    int y = (i < 2) ? 335 : 425;
                    Rectangle area_alternativa = {(float)x, (float)y, 440.0f, 68.0f};
                    if (mouseSobre(mouse, area_alternativa)) {
                        alternativa = i;
                        break;
                    }
                }
            }

            /* Impede escolher alternativas que uma dica tenha removido. */
            if (alternativa != -1) {
                if (modo_jogo == MODO_VERSUS && !versus.alternativas_visiveis[alternativa]) {
                    alternativa = -1;
                } else if (modo_jogo == MODO_HISTORIA && !historia.alternativas_visiveis[alternativa]) {
                    alternativa = -1;
                }
            }

            if (alternativa != -1) {
                /* No Versus, o erro encerra a disputa. Na História, custa uma vida. */
                responderPergunta(&jogo, pergunta, alternativa, modo_jogo == MODO_VERSUS);

                if (modo_jogo == MODO_VERSUS) {
                    /* O computador tenta acertar usando a chance da dificuldade. */
                    int computador_acertou = computadorAcerta(pergunta.dificuldade);
                    registrarRodadaVersus(&versus, &jogo, pergunta.dificuldade, computador_acertou);
                }

                if (jogo.acertou) {
                    tocarSom(som_acerto);
                } else {
                    tocarSom(som_erro);
                }

                tela = FEEDBACK;
            }
        }

        /* -------------------- ENTRADA NA BATALHA CONTRA O CHEFE -------------------- */
        else if (tela == ENTRADA_CHEFE) {
            Vector2 mouse = obterMouseVirtual();
            if (IsKeyPressed(KEY_ENTER) ||
                (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mouseSobre(mouse, (Rectangle){300, 585, 400, 50}))) {
                historiaNovaPergunta(&historia);
                tela = PERGUNTA_ATUAL;
            }

            if (IsKeyPressed(KEY_ESCAPE)) {
                tela = MENU;
            }
        }

        /* -------------------- FEEDBACK DA RESPOSTA -------------------- */
        else if (tela == FEEDBACK) {
            if (modo_jogo == MODO_VERSUS) {
                Vector2 mouse = obterMouseVirtual();
                if (IsKeyPressed(KEY_ENTER) ||
                    (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mouseSobre(mouse, (Rectangle){230, 495, 540, 100}))) {
                    /* No Versus, a disputa termina quando alguém erra primeiro. */
                    if (versus.terminou) {
                        tela = RESULTADO;
                    } else {
                        proximaPergunta(&jogo);
                        prepararProximaRodadaVersus(&versus);
                        tela = PERGUNTA_ATUAL;
                    }
                }
            } else {
                Pergunta pergunta = perguntaAtual(&lista, jogo, historia);

                /* A reversão troca a pergunta antes de registrar o erro ou atacar o chefe. */
                Vector2 mouse = obterMouseVirtual();
                int y_botao_reversao = (jogo.pergunta_atual >= PERGUNTA_INICIO_CHEFE) ? 465 : 350;
                Rectangle area_reversao = {145, (float)y_botao_reversao, 710, 65};

                if (!historia.regeneracao_em_andamento && !jogo.acertou &&
                    historia.reversoes_temporais > 0 &&
                    (IsKeyPressed(KEY_T) ||
                     (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mouseSobre(mouse, area_reversao))) &&
                    historiaUsarReversao(&historia, &jogo, &lista)) {
                    tela = PERGUNTA_ATUAL;
                }

                if ((IsKeyPressed(KEY_ENTER) ||
                    (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mouseSobre(mouse, (Rectangle){230, 495, 540, 100}))) && tela == FEEDBACK) {
                    int indice_respondido = jogo.pergunta_atual;
                    int era_regeneracao = historia.regeneracao_em_andamento;
                    int resultado_rodada = RODADA_EM_ANDAMENTO;

                    historiaProcessarResposta(&historia, &jogo, pergunta);

                    /*
                     * A pergunta de regeneração é um desafio extra. Se o jogador
                     * acertar, ele volta à pergunta que estava tentando responder.
                     * Assim, não pula uma pergunta da expedição depois de sobreviver.
                     */
                    if (era_regeneracao) {
                        if (historia.game_over) {
                            chefeEncerrarDerrota(&chefe);
                            tela = RESULTADO;
                        } else {
                            jogo.respondeu = 0;
                            jogo.resposta_escolhida = -1;
                            jogo.acertou = 0;
                            jogo.terminou = 0;
                            historiaNovaPergunta(&historia);
                            tela = PERGUNTA_ATUAL;
                        }
                    }
                    /* A última vida inicia o Protocolo de Regeneração. */
                    else if (historia.regeneracao_em_andamento) {
                        if (historiaIniciarRegeneracao(&historia, &jogo, &lista)) {
                            tela = PERGUNTA_ATUAL;
                        } else {
                            historia.game_over = 1;
                            chefeEncerrarDerrota(&chefe);
                            tela = RESULTADO;
                        }
                    } else if (historia.game_over) {
                        chefeEncerrarDerrota(&chefe);
                        tela = RESULTADO;
                    } else if (indice_respondido >= PERGUNTA_INICIO_CHEFE) {
                        /* Cada resposta do chefe entra no cálculo da dupla atual. */
                        resultado_rodada = chefeRegistrarResposta(&chefe,
                            indice_respondido, jogo.acertou);

                        if (resultado_rodada == NUCLEO_DESTRUIDO_PERFEITO) {
                            historiaConcederFragmentos(&historia, BONUS_RODADA_PERFEITA);
                        }

                        /* A pergunta 50 é obrigatoriamente o golpe final. */
                        if (indice_respondido == TOTAL_QUESTOES_HISTORIA - 1) {
                            chefeFinalizar(&chefe, jogo.acertou);
                            jogo.terminou = 1;

                            if (!chefe.venceu) {
                                historia.game_over = 1;
                            }

                            tela = RESULTADO;
                        } else {
                            proximaPergunta(&jogo);
                            historiaNovaPergunta(&historia);
                            tela = PERGUNTA_ATUAL;
                        }
                    } else {
                        proximaPergunta(&jogo);

                        /* As oito regiões de exploração terminam na pergunta 40. */
                        if (jogo.terminou || jogo.pergunta_atual % QUESTOES_POR_REGIAO == 0) {
                            int regiao_concluida = jogo.terminou ?
                                jogo.regiao_atual : jogo.regiao_atual - 1;
                            int indice_portal = -1;

                            if (regiao_concluida >= 0 && regiao_concluida < TOTAL_REGIOES_HISTORIA) {
                                indice_portal = jogo.portal_por_regiao[regiao_concluida];
                                historiaConcluirRegiao(&historia, regiao_concluida);
                            }

                            if (indice_portal >= 0 && indice_portal < TOTAL_PORTAIS) {
                                historiaRecompensaPortal(&historia,
                                    (enum TipoRecompensaPortal)portais[indice_portal].tipo_recompensa,
                                    portais[indice_portal].valor_recompensa);
                            }

                            if (jogo.terminou) {
                                tela = RESULTADO;
                            } else if (jogo.pergunta_atual == PERGUNTA_INICIO_CHEFE) {
                                /* Após oito regiões, a exploração dá lugar à arena do Paradoxo. */
                                iniciarChefe(&chefe);
                                tela = ENTRADA_CHEFE;
                            } else {
                                gerarPortais(portais, jogo.portal_ids_visitados, jogo.regiao_atual);
                                personagem.posicao = (Vector2){484.0f, 465.0f};
                                personagem.movendo_mouse = 0;
                                tela = MAPA;
                            }
                        } else {
                            historiaNovaPergunta(&historia);
                            tela = PERGUNTA_ATUAL;
                        }
                    }
                }
            }
        }

        /* -------------------- RESULTADO -------------------- */
        else if (tela == RESULTADO) {
            Vector2 mouse = obterMouseVirtual();
            if (IsKeyPressed(KEY_ENTER) ||
                (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mouseSobre(mouse, (Rectangle){250, 485, 500, 105}))) {
                tela = MENU;
            }
        }

        /* -------------------- CONSULTA DE PERGUNTAS -------------------- */
        else if (tela == CONSULTA) {
            Vector2 mouse = obterMouseVirtual();
            int clique = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

            if (IsKeyPressed(KEY_ONE) || (clique && mouseSobre(mouse, (Rectangle){100, 115, 145, 42}))) dificuldade_consulta = MUITO_FACIL, indice_consulta = 0;
            if (IsKeyPressed(KEY_TWO) || (clique && mouseSobre(mouse, (Rectangle){250, 115, 115, 42}))) dificuldade_consulta = FACIL, indice_consulta = 0;
            if (IsKeyPressed(KEY_THREE) || (clique && mouseSobre(mouse, (Rectangle){370, 115, 115, 42}))) dificuldade_consulta = MEDIO, indice_consulta = 0;
            if (IsKeyPressed(KEY_FOUR) || (clique && mouseSobre(mouse, (Rectangle){490, 115, 125, 42}))) dificuldade_consulta = DIFICIL, indice_consulta = 0;
            if (IsKeyPressed(KEY_FIVE) || (clique && mouseSobre(mouse, (Rectangle){620, 115, 190, 42}))) dificuldade_consulta = MUITO_DIFICIL, indice_consulta = 0;

            if ((IsKeyPressed(KEY_RIGHT) || (clique && mouseSobre(mouse, (Rectangle){535, 560, 90, 42}))) &&
                indice_consulta < contarDificuldade(&lista, dificuldade_consulta) - 1) {
                indice_consulta++;
            }
            if ((IsKeyPressed(KEY_LEFT) || (clique && mouseSobre(mouse, (Rectangle){375, 560, 90, 42}))) && indice_consulta > 0) {
                indice_consulta--;
            }
            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER) ||
                (clique && mouseSobre(mouse, (Rectangle){700, 560, 180, 42}))) {
                tela = MENU;
            }
        }

        /* -------------------- RESULTADO DA EXPORTAÇÃO -------------------- */
        else if (tela == EXPORTACAO) {
            Vector2 mouse = obterMouseVirtual();
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE) ||
                (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mouseSobre(mouse, (Rectangle){250, 470, 500, 90}))) {
                tela = MENU;
            }
        }

        /* -------------------- DESENHO DA TELA ATUAL -------------------- */
        /* Primeiro desenha tudo na resolução-base de 1000 x 650. */
        BeginTextureMode(tela_virtual);
        ClearBackground((Color){13, 18, 32, 255});

        if (tela == MENU) {
            desenharMenu(lista.quantidade, fundo_menu);
        }
        else if (tela == MAPA) {
            desenharMapa(personagem, portais, portal_proximo, jogo, historia, imagens_portais, imagem_loja);
        }
        else if (tela == LOJA) {
            desenharLoja(historia, imagens_dicas, imagem_loja, mensagem_loja);
        }
        else if (tela == ENTRADA_CHEFE) {
            desenharEntradaChefe(chefe, imagens_chefe);
        }
        else if (tela == PERGUNTA_ATUAL) {
            Pergunta pergunta = perguntaAtual(&lista, jogo, historia);
            desenharPergunta(pergunta, jogo, portais, modo_jogo, versus, historia, chefe, imagens_chefe, fundos_mundos, imagens_dicas);
        }
        else if (tela == FEEDBACK) {
            if (modo_jogo == MODO_VERSUS) {
                desenharFeedbackVersus(jogo, versus, perguntaAtual(&lista, jogo, historia));
            } else {
                Pergunta pergunta = perguntaAtual(&lista, jogo, historia);
                desenharFeedbackHistoria(jogo, historia, pergunta, chefe, imagens_chefe, fundos_mundos, imagens_dicas);
            }
        }
        else if (tela == RESULTADO) {
            if (modo_jogo == MODO_VERSUS) {
                desenharResultadoVersus(versus);
            } else {
                desenharResultadoHistoria(jogo, historia, chefe, imagens_chefe);
            }
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
            DrawText("Clique aqui ou pressione ENTER para voltar", 285, 520, 19, LIGHTGRAY);
        }

        /* Indica o atalho de tela cheia em todas as telas. */
        DrawText("F11 - Alternar tela cheia", 755, 625, 15, (Color){150, 165, 185, 255});
        EndTextureMode();

        /* Ajusta a tela virtual ao tamanho atual da janela/monitor.
           O cálculo mantém a proporção e cria barras laterais ou superiores
           quando a resolução do monitor tem outro formato. */
        BeginDrawing();
        ClearBackground(BLACK);
        {
            float escala_x = (float)GetScreenWidth() / LARGURA_TELA;
            float escala_y = (float)GetScreenHeight() / ALTURA_TELA;
            float escala = escala_x < escala_y ? escala_x : escala_y;
            float largura = LARGURA_TELA * escala;
            float altura = ALTURA_TELA * escala;
            float posicao_x = ((float)GetScreenWidth() - largura) / 2.0f;
            float posicao_y = ((float)GetScreenHeight() - altura) / 2.0f;
            Rectangle origem = {0, 0, LARGURA_TELA, -ALTURA_TELA};
            Rectangle destino = {posicao_x, posicao_y, largura, altura};

            DrawTexturePro(tela_virtual.texture, origem, destino,
                           (Vector2){0, 0}, 0.0f, WHITE);
        }
        EndDrawing();
    }

    /* Salva as alterações e libera os recursos utilizados pelo jogo. */
    salvarBinario(&lista, ARQUIVO_BIN);
    liberarPersonagem(&personagem);
    liberarSpritesPortais(imagens_portais);
    liberarFundosMundos(fundos_mundos);
    for (int i = 0; i < 3; i++) {
        if (imagens_dicas[i].id != 0) {
            UnloadTexture(imagens_dicas[i]);
        }
    }
    if (fundo_menu.id != 0) {
        UnloadTexture(fundo_menu);
    }
    if (imagem_loja.id != 0) {
        UnloadTexture(imagem_loja);
    }
    for (int i = 0; i < 2; i++) {
        if (imagens_chefe[i].id != 0) {
            UnloadTexture(imagens_chefe[i]);
        }
    }
    UnloadRenderTexture(tela_virtual);
    if (IsAudioDeviceReady()) {
        if (som_portal.frameCount > 0) UnloadSound(som_portal);
        if (som_acerto.frameCount > 0) UnloadSound(som_acerto);
        if (som_erro.frameCount > 0) UnloadSound(som_erro);
        CloseAudioDevice();
    }
    CloseWindow();
    liberarLista(&lista);

    return 0;
}



/**
 * @brief Converte a posição do mouse para a resolução virtual do jogo.
 *
 * O jogo é desenhado sempre em 1000 x 650 pixels e depois ajustado ao monitor.
 * Esta função desfaz esse ajuste para que os cliques coincidam com os botões,
 * mesmo quando a janela está redimensionada ou em tela cheia.
 */
/**
 * @brief Toca um efeito sonoro somente quando o audio foi carregado.
 *
 * Se o computador nao possuir dispositivo de audio ou o arquivo estiver
 * ausente, o jogo continua funcionando normalmente.
 */
void tocarSom(Sound som) {
    if (IsAudioDeviceReady() && som.frameCount > 0) {
        PlaySound(som);
    }
}

Vector2 obterMouseVirtual(void) {
    Vector2 mouse = GetMousePosition();
    float escala_x = (float)GetScreenWidth() / LARGURA_TELA;
    float escala_y = (float)GetScreenHeight() / ALTURA_TELA;
    float escala = escala_x < escala_y ? escala_x : escala_y;

    if (escala <= 0.0f) {
        return (Vector2){-1.0f, -1.0f};
    }

    float largura = LARGURA_TELA * escala;
    float altura = ALTURA_TELA * escala;
    float margem_x = ((float)GetScreenWidth() - largura) / 2.0f;
    float margem_y = ((float)GetScreenHeight() - altura) / 2.0f;

    mouse.x = (mouse.x - margem_x) / escala;
    mouse.y = (mouse.y - margem_y) / escala;

    /* Cliques nas barras externas à imagem não correspondem a nenhum botão. */
    if (mouse.x < 0 || mouse.x > LARGURA_TELA || mouse.y < 0 || mouse.y > ALTURA_TELA) {
        mouse.x = -1.0f;
        mouse.y = -1.0f;
    }

    return mouse;
}

/** @brief Verifica se o cursor está dentro de uma área clicável. */
int mouseSobre(Vector2 mouse, Rectangle area) {
    return CheckCollisionPointRec(mouse, area);
}

/** @brief Registra a escolha de portal e inicia a pergunta da região. */
void entrarNoPortal(int indice, Portal portais[], Jogo *jogo, Historia *historia,
                    Sound som_portal, int *tela) {
    if (portais == NULL || jogo == NULL || historia == NULL || tela == NULL ||
        indice < 0 || indice >= TOTAL_PORTAIS ||
        jogo->regiao_atual < 0 || jogo->regiao_atual >= TOTAL_REGIOES_HISTORIA) {
        return;
    }

    jogo->portal_escolhido = indice;
    jogo->portal_por_regiao[jogo->regiao_atual] = indice;
    jogo->portal_ids_visitados[jogo->regiao_atual] = portais[indice].id;
    historiaNovaPergunta(historia);
    tocarSom(som_portal);
    *tela = PERGUNTA_ATUAL;
}

/**
 * @brief Desenha uma representacao geometrica provisoria de O Paradoxo.
 *
 * Esta funcao e usada apenas como alternativa caso os arquivos PNG nao sejam
 * encontrados. Normalmente, o chefe e desenhado pela funcao desenharSpriteChefe.
 */
/**
 * @brief Desenha a arte oficial de O Paradoxo na fase correspondente.
 *
 * A imagem e carregada uma vez no inicio do jogo. Esta funcao calcula a largura
 * a partir da altura desejada, preservando a proporcao original do PNG.
 * Se o arquivo nao for encontrado, utiliza a representacao geometrica antiga.
 */
void desenharSpriteChefe(Texture2D imagens_chefe[], int centro_x, int centro_y,
                         float altura, int fase) {
    int indice = fase == 2 ? 1 : 0;
    Texture2D imagem = imagens_chefe[indice];

    if (imagem.id == 0 || imagem.height == 0) {
        desenharVisualChefe(centro_x, centro_y, (int)(altura / 2.0f), fase);
        return;
    }

    float largura = altura * ((float)imagem.width / (float)imagem.height);
    Rectangle origem = {0.0f, 0.0f, (float)imagem.width, (float)imagem.height};
    Rectangle destino = {
        (float)centro_x - largura / 2.0f,
        (float)centro_y - altura / 2.0f,
        largura,
        altura
    };

    DrawTexturePro(imagem, origem, destino, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
}

void desenharVisualChefe(int x, int y, int raio, int fase) {
    Color cor_energia = fase == 1 ? (Color){55, 190, 240, 255} : (Color){210, 75, 220, 255};
    Color cor_borda = fase == 1 ? (Color){30, 110, 185, 255} : (Color){180, 45, 110, 255};

    DrawCircle(x, y, (float)(raio + 13), (Color){20, 12, 48, 255});
    DrawCircleLines(x, y, (float)(raio + 13), cor_borda);
    DrawCircleLines(x, y, (float)(raio + 5), (Color){75, 95, 190, 255});
    DrawCircle(x, y, (float)(raio - 4), (Color){12, 19, 48, 255});
    DrawCircleLines(x, y, (float)(raio - 4), cor_energia);

    /* Anéis quebrados e linhas simulam a energia temporal instável. */
    DrawLine(x - raio - 4, y - raio / 2, x - raio / 2, y - raio / 3, cor_energia);
    DrawLine(x + raio / 2, y - raio / 3, x + raio + 4, y - raio / 2, cor_energia);
    DrawLine(x - raio - 2, y + raio / 2, x - raio / 2, y + raio / 3, cor_borda);
    DrawLine(x + raio / 2, y + raio / 3, x + raio + 2, y + raio / 2, cor_borda);

    DrawCircle(x, y, (float)(raio / 2), (Color){30, 35, 85, 255});
    DrawCircleLines(x, y, (float)(raio / 2), cor_borda);
    DrawCircle(x, y, (float)(raio / 4), cor_energia);
    DrawCircle(x, y, (float)(raio / 9), (Color){255, 220, 130, 255});

    /* Dois pontos dourados representam os olhos da entidade. */
    DrawCircle(x - raio / 3, y - raio / 7, (float)(raio / 9), (Color){255, 190, 80, 255});
    DrawCircle(x + raio / 3, y - raio / 7, (float)(raio / 9), (Color){255, 190, 80, 255});
}

/**
 * @brief Mostra a entrada cinematográfica da batalha contra O Paradoxo.
 */
void desenharEntradaChefe(Chefe chefe, Texture2D imagens_chefe[]) {
    DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, (Color){7, 8, 25, 255});
    DrawCircleLines(500, 205, 190, (Color){35, 65, 130, 255});
    DrawCircleLines(500, 205, 172, (Color){45, 35, 110, 255});
    desenharSpriteChefe(imagens_chefe, 500, 205, 330.0f, 1);

    DrawText("RUPTURA NA LINHA DO TEMPO", 285, 18, 25, SKYBLUE);
    DrawText("O PARADOXO", 350, 382, 42, (Color){210, 90, 220, 255});
    DrawText("Uma entidade temporal protege cinco nucleos.", 250, 450, 22, RAYWHITE);
    DrawText("As ultimas dez perguntas definirao o destino da linha do tempo.",
             145, 485, 18, LIGHTGRAY);
    DrawText("Acerte ao menos uma pergunta de cada dupla para destruir um nucleo.",
             115, 520, 18, SKYBLUE);
    DrawText("A pergunta 50 sera o golpe final.", 335, 550, 19, GOLD);
    DrawText("ENTER - Iniciar batalha", 365, 605, 21, RAYWHITE);

    DrawText(TextFormat("Nucleos temporais: %d", chefe.nucleos_restantes), 405, 580, 17, LIGHTGRAY);
}

/**
 * @brief Desenha o nome, fase, vida do chefe e os cinco núcleos temporais.
 */
void desenharPainelChefe(Chefe chefe, int indice_pergunta, Historia historia, Texture2D imagens_chefe[]) {
    int fase = chefeFasePergunta(indice_pergunta);
    int nucleos_destruidos = TOTAL_NUCLEOS_CHEFE - chefe.nucleos_restantes;
    Color cor_fase = fase == 1 ? (Color){95, 190, 245, 255} : (Color){225, 95, 220, 255};

    DrawRectangle(0, 0, LARGURA_TELA, 145, (Color){9, 11, 32, 255});
    DrawText("O PARADOXO", 35, 15, 27, cor_fase);
    DrawText(fase == 1 ? "FASE 1 - INSTABILIDADE TEMPORAL" : "FASE 2 - COLAPSO TEMPORAL",
             35, 52, 17, RAYWHITE);
    DrawText(TextFormat("PERGUNTA %d / %d", indice_pergunta + 1, TOTAL_QUESTOES_HISTORIA),
             35, 82, 17, SKYBLUE);
    DrawText(TextFormat("VIDAS: %d | FRAGMENTOS: %d | SONICA: %d | ARQUIVOS: %d | REVERSOES: %d",
             historia.vidas, historia.fragmentos, historia.cargas_sonica,
             historia.cargas_arquivo, historia.reversoes_temporais),
             35, 112, 15, LIGHTGRAY);

    DrawText("NUCLEOS", 405, 48, 16, LIGHTGRAY);
    for (int i = 0; i < TOTAL_NUCLEOS_CHEFE; i++) {
        int x = 430 + i * 42;

        if (i < nucleos_destruidos) {
            DrawCircle(x, 88, 12, (Color){70, 210, 225, 255});
            DrawCircleLines(x, 88, 15, SKYBLUE);
            DrawLine(x - 6, 88, x + 6, 88, (Color){8, 24, 48, 255});
        } else {
            DrawCircle(x, 88, 12, (Color){55, 35, 90, 255});
            DrawCircleLines(x, 88, 15, cor_fase);
        }
    }

    desenharSpriteChefe(imagens_chefe, 855, 82, 118.0f, fase);

    /* Mostra o resultado da dupla anterior apenas na pergunta seguinte. */
    if (chefe.rodadas_concluidas > 0 &&
        indice_pergunta == PERGUNTA_INICIO_CHEFE +
                           chefe.rodadas_concluidas * PERGUNTAS_POR_RODADA_CHEFE) {
        if (chefe.ultima_rodada == NUCLEO_DESTRUIDO_PERFEITO) {
            DrawText("NUCLEO DESTRUIDO - RODADA PERFEITA: +30 FRAGMENTOS",
                     245, 129, 15, GOLD);
        } else if (chefe.ultima_rodada == NUCLEO_DESTRUIDO) {
            DrawText("NUCLEO TEMPORAL DESTRUIDO!", 375, 129, 15, GREEN);
        } else if (chefe.ultima_rodada == NUCLEO_RESISTIU) {
            DrawText("O NUCLEO RESISTIU AOS SEUS ATAQUES!", 330, 129, 15, RED);
        }
    }
}

/**
 * @brief Carrega uma textura de fundo sem gerar erro se o arquivo ainda nao existir.
 *
 * Os fundos dos mundos serao adicionados depois. Por isso, a ausencia de uma
 * imagem e tratada como uma situacao normal e o jogo usa um fundo alternativo.
 */
Texture2D carregarFundo(const char caminho[]) {
    Texture2D imagem = {0};
    FILE *arquivo = fopen(caminho, "rb");

    if (arquivo == NULL) {
        return imagem;
    }

    fclose(arquivo);
    imagem = LoadTexture(caminho);

    if (imagem.id != 0) {
        SetTextureFilter(imagem, TEXTURE_FILTER_POINT);
    }

    return imagem;
}

/**
 * @brief Carrega os doze fundos seguindo o ID de cada destino temporal.
 *
 * Para adicionar um cenário, basta salvar o PNG com o nome correspondente
 * dentro de imagens/fundos/mundos. O restante do código identifica o arquivo.
 */
void carregarFundosMundos(Texture2D fundos[]) {
    const char *caminhos[TOTAL_MODELOS_PORTAIS] = {
        "imagens/fundos/mundos/mundo_01_alexandria.png",
        "imagens/fundos/mundos/mundo_02_atenas.png",
        "imagens/fundos/mundos/mundo_03_roma.png",
        "imagens/fundos/mundos/mundo_04_mosteiro_das_eras.png",
        "imagens/fundos/mundos/mundo_05_florenca_1504.png",
        "imagens/fundos/mundos/mundo_06_londres_1888.png",
        "imagens/fundos/mundos/mundo_07_gallifrey.png",
        "imagens/fundos/mundos/mundo_08_orbita_1969.png",
        "imagens/fundos/mundos/mundo_09_estacao_3000.png",
        "imagens/fundos/mundos/mundo_10_fenda_de_cardiff.png",
        "imagens/fundos/mundos/mundo_11_pompeia_79.png",
        "imagens/fundos/mundos/mundo_12_nova_nova_york.png"
    };

    for (int i = 0; i < TOTAL_MODELOS_PORTAIS; i++) {
        fundos[i] = carregarFundo(caminhos[i]);
    }
}

/** @brief Libera os fundos que foram carregados com sucesso. */
void liberarFundosMundos(Texture2D fundos[]) {
    for (int i = 0; i < TOTAL_MODELOS_PORTAIS; i++) {
        if (fundos[i].id != 0) {
            UnloadTexture(fundos[i]);
            fundos[i].id = 0;
        }
    }
}

/**
 * @brief Desenha o fundo associado ao ID de um mundo.
 *
 * Enquanto a imagem final nao for adicionada, utiliza um fundo escuro com
 * estrelas e uma cor de destaque diferente para cada destino.
 */
void desenharFundoMundo(Texture2D fundos[], int id_mundo) {
    static const Color cores[TOTAL_MODELOS_PORTAIS] = {
        {80, 105, 150, 255}, {55, 125, 130, 255}, {150, 95, 55, 255},
        {105, 80, 140, 255}, {155, 115, 70, 255}, {145, 80, 45, 255},
        {125, 45, 95, 255}, {45, 95, 155, 255}, {40, 105, 130, 255},
        {65, 75, 155, 255}, {150, 75, 45, 255}, {90, 65, 145, 255}
    };

    int indice = id_mundo - 1;

    if (indice < 0 || indice >= TOTAL_MODELOS_PORTAIS) {
        indice = 0;
    }

    if (fundos[indice].id != 0) {
        Rectangle origem = {0.0f, 0.0f, (float)fundos[indice].width,
                            (float)fundos[indice].height};
        Rectangle destino = {0.0f, 0.0f, LARGURA_TELA, ALTURA_TELA};
        DrawTexturePro(fundos[indice], origem, destino, (Vector2){0, 0}, 0.0f, WHITE);
    } else {
        DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, (Color){8, 12, 27, 255});
        DrawCircle(770, 250, 190, (Color){cores[indice].r, cores[indice].g,
                                          cores[indice].b, 75});
        DrawCircle(770, 250, 125, (Color){cores[indice].r, cores[indice].g,
                                          cores[indice].b, 45});

        for (int i = 0; i < 65; i++) {
            int x = (i * 193 + indice * 41 + 37) % LARGURA_TELA;
            int y = (i * 97 + indice * 23 + 19) % ALTURA_TELA;
            DrawCircle(x, y, (i % 5 == 0) ? 2.0f : 1.0f,
                       (Color){160, 190, 225, 150});
        }
    }

    /* O escurecimento preserva a leitura das perguntas sobre a imagem. */
    DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, (Color){4, 8, 23, 125});
}

/** @brief Desenha o fundo da época escolhida na região atual da expedição. */
void desenharFundoDaRegiao(Texture2D fundos[], Jogo jogo) {
    int regiao = jogo.regiao_atual;

    if (regiao < 0 || regiao >= TOTAL_REGIOES_HISTORIA) {
        desenharFundoMundo(fundos, 1);
        return;
    }

    desenharFundoMundo(fundos, jogo.portal_ids_visitados[regiao]);
}

/**
 * @brief Exibe o menu principal sobre a arte oficial do Timey Wimey.
 *
 * O painel escuro fica no lado esquerdo, deixando a máquina temporal e o
 * vórtice visíveis no lado direito da imagem enviada para o projeto.
 */
void desenharMenu(int quantidade_perguntas, Texture2D fundo_menu) {
    Vector2 mouse = obterMouseVirtual();
    Rectangle botoes[] = {
        {55, 200, 430, 76}, {55, 290, 430, 76},
        {55, 385, 205, 50}, {275, 385, 210, 50},
        {55, 450, 430, 50}
    };

    /* Usa a imagem enviada. Se ela estiver ausente, mantém um fundo escuro. */
    if (fundo_menu.id != 0) {
        Rectangle origem = {0.0f, 0.0f, (float)fundo_menu.width, (float)fundo_menu.height};
        Rectangle destino = {0.0f, 0.0f, LARGURA_TELA, ALTURA_TELA};
        DrawTexturePro(fundo_menu, origem, destino, (Vector2){0, 0}, 0.0f, WHITE);
    } else {
        DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, (Color){5, 8, 27, 255});
        for (int i = 0; i < 55; i++) {
            int x = (i * 193 + 37) % LARGURA_TELA;
            int y = (i * 97 + 19) % ALTURA_TELA;
            DrawCircle(x, y, (i % 4 == 0) ? 2.0f : 1.0f, (Color){125, 165, 225, 180});
        }
    }

    /* Escurece somente a área esquerda para dar contraste aos textos. */
    DrawRectangleRounded((Rectangle){28, 24, 475, 600}, 0.035f, 12,
                         (Color){3, 7, 24, 205});
    DrawRectangle(28, 24, 7, 600, (Color){45, 190, 235, 220});

    DrawText("TIMEY WIMEY", 58, 55, 48, (Color){100, 210, 255, 255});
    DrawText("UMA AVENTURA ATRAVES DO TEMPO", 62, 116, 17, RAYWHITE);
    DrawText("Explore epocas, responda perguntas e restaure", 62, 150, 15, LIGHTGRAY);
    DrawText("a linha temporal antes que tudo desapareca.", 62, 171, 15, LIGHTGRAY);

    /* Realça cada opção quando o cursor está sobre ela. */
    for (int i = 0; i < 5; i++) {
        Color fundo = mouseSobre(mouse, botoes[i])
            ? (Color){45, 105, 150, 235}
            : (Color){17, 31, 56, 220};
        DrawRectangleRounded(botoes[i], 0.16f, 8, fundo);
    }

    DrawText("1  MODO HISTORIA", 82, 211, 25, (Color){115, 215, 255, 255});
    DrawText("8 regioes de exploracao + confronto contra O Paradoxo", 82, 247, 15, RAYWHITE);

    DrawText("2  MODO VERSUS", 82, 301, 25, GOLD);
    DrawText("Show do Milhao: jogador contra o computador", 82, 337, 15, RAYWHITE);

    DrawText("3  CONSULTAR", 75, 400, 17, RAYWHITE);
    DrawText("4  EXPORTAR CSV", 290, 400, 17, RAYWHITE);
    DrawText("5  SAIR DO JOGO", 82, 463, 20, RAYWHITE);

    DrawText("F11  Tela cheia", 62, 525, 16, SKYBLUE);
    DrawText(TextFormat("Banco: %d perguntas", quantidade_perguntas), 62, 553, 16, GREEN);
    DrawText("Mouse: clique nas opcoes | Teclado: teclas 1 a 5", 62, 583, 15, LIGHTGRAY);
    DrawText("Cadastro: make gerenciar", 62, 605, 15, LIGHTGRAY);
}

/**
 * @brief Desenha a sala onde o jogador escolhe um dos três portais.
 */
void desenharMapa(Personagem personagem, Portal portais[], int portal_proximo, Jogo jogo, Historia historia, Texture2D imagens_portais[], Texture2D imagem_loja) {
    DrawRectangle(0, 115, LARGURA_TELA, ALTURA_TELA - 115, (Color){24, 37, 48, 255});

    for (int x = 0; x < LARGURA_TELA; x += 40) {
        DrawLine(x, 115, x, ALTURA_TELA, (Color){36, 52, 64, 255});
    }
    for (int y = 115; y < ALTURA_TELA; y += 40) {
        DrawLine(0, y, LARGURA_TELA, y, (Color){36, 52, 64, 255});
    }

    DrawText(TextFormat("SALA TEMPORAL - REGIAO %d DE %d", jogo.regiao_atual + 1, TOTAL_REGIOES_HISTORIA), 25, 18, 26, RAYWHITE);
    DrawText(TextFormat("Proxima pergunta: %d / %d", jogo.pergunta_atual + 1,
                 TOTAL_QUESTOES_HISTORIA), 25, 55, 18, SKYBLUE);
    DrawText(TextFormat("Vidas: %d   Fragmentos: %d   Sonica: %d   Arquivos: %d   Reversoes: %d",
             historia.vidas, historia.fragmentos, historia.cargas_sonica, historia.cargas_arquivo, historia.reversoes_temporais),
             25, 82, 16, LIGHTGRAY);
    DrawText("WASD/setas: mover | Clique em um portal para caminhar ate ele", 25, 112, 16, LIGHTGRAY);
    desenharPortais(portais, portal_proximo, imagens_portais);
    desenharPersonagem(personagem);

    if (portal_proximo != -1) {
        DrawRectangleRounded((Rectangle){270, 555, 460, 55}, 0.2f, 8, (Color){5, 10, 20, 220});
        DrawText(TextFormat("Clique no portal ou pressione E para entrar em %s", portais[portal_proximo].nome), 285, 572, 18, WHITE);
    } else {
        DrawText("Escolha um dos tres portais. Cada um oferece uma recompensa diferente.", 210, 575, 19, LIGHTGRAY);
    }

    /*
     * A entrada da loja fica no canto superior esquerdo da sala.
     * A imagem tem 48 x 48 pixels e é ampliada com filtro POINT para
     * manter os pixels nítidos. O cartão inteiro funciona como botão.
     */
    Rectangle area_loja = {24, 132, 240, 72};
    Color cor_loja = mouseSobre(obterMouseVirtual(), area_loja)
        ? (Color){45, 105, 150, 245} : (Color){12, 25, 43, 235};

    DrawRectangleRounded(area_loja, 0.16f, 8, cor_loja);
    DrawRectangleLinesEx(area_loja, 2.0f,
        mouseSobre(obterMouseVirtual(), area_loja) ? SKYBLUE : (Color){55, 95, 125, 255});

    if (imagem_loja.id != 0) {
        Rectangle origem_loja = {0, 0, (float)imagem_loja.width, (float)imagem_loja.height};
        Rectangle destino_loja = {36, 144, 48, 48};
        DrawTexturePro(imagem_loja, origem_loja, destino_loja,
                       (Vector2){0, 0}, 0.0f, WHITE);
    } else {
        DrawRectangleRounded((Rectangle){36, 144, 48, 48}, 0.12f, 5,
                             (Color){30, 85, 125, 255});
        DrawCircle(60, 168, 11, GOLD);
    }

    DrawText("LOJA DA TARDIS", 98, 145, 18, GOLD);
    DrawText("L - comprar power-ups", 99, 173, 14, RAYWHITE);

    if (historia.mensagem_recompensa[0] != '\0') {
        DrawText(TextFormat("Ultima recompensa: %s", historia.mensagem_recompensa),
                 20, 620, 15, GOLD);
    }

    DrawText("ESC - voltar ao menu", 790, 25, 16, GRAY);
}

/**
 * @brief Desenha um ícone em uma área, preservando sua proporção original.
 */
void desenharIconeDica(Texture2D imagem, Rectangle area, Color cor) {
    if (imagem.id == 0) {
        return;
    }

    float escala_x = area.width / (float)imagem.width;
    float escala_y = area.height / (float)imagem.height;
    float escala = escala_x < escala_y ? escala_x : escala_y;
    float largura = (float)imagem.width * escala;
    float altura = (float)imagem.height * escala;
    Rectangle origem = {0, 0, (float)imagem.width, (float)imagem.height};
    Rectangle destino = {
        area.x + (area.width - largura) / 2.0f,
        area.y + (area.height - altura) / 2.0f,
        largura,
        altura
    };

    DrawTexturePro(imagem, origem, destino, (Vector2){0, 0}, 0.0f, cor);
}

/**
 * @brief Mostra a loja temporária que fica disponível na tela de portais.
 *
 * As compras alteram somente a estrutura Historia atual. Ao iniciar outra
 * expedição, vidas, cargas e fragmentos são reiniciados normalmente.
 */
void desenharLoja(Historia historia, Texture2D imagens_dicas[], Texture2D imagem_loja, const char mensagem[]) {
    Rectangle areas[TOTAL_ITENS_LOJA] = {
        {55, 175, 430, 125}, {515, 175, 430, 125},
        {55, 320, 430, 125}, {515, 320, 430, 125}
    };
    Vector2 mouse = obterMouseVirtual();

    DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, (Color){8, 15, 30, 255});
    DrawRectangleRounded((Rectangle){25, 20, 950, 610}, 0.04f, 12,
                         (Color){18, 31, 52, 255});
    DrawRectangle(25, 20, 7, 610, (Color){45, 190, 235, 255});

    /* A arte opcional da loja deve ser um PNG de 48 x 48 pixels. */
    if (imagem_loja.id != 0) {
        Rectangle origem_loja = {0, 0, (float)imagem_loja.width, (float)imagem_loja.height};
        Rectangle destino_loja = {42, 32, 48, 48};
        DrawTexturePro(imagem_loja, origem_loja, destino_loja,
                       (Vector2){0, 0}, 0.0f, WHITE);
    } else {
        /* Mantem um simbolo simples enquanto a imagem definitiva nao for adicionada. */
        DrawRectangleRounded((Rectangle){42, 32, 48, 48}, 0.12f, 5,
                             (Color){30, 85, 125, 255});
        DrawRectangleLinesEx((Rectangle){42, 32, 48, 48}, 2.0f, SKYBLUE);
        DrawCircle(66, 55, 11, GOLD);
        DrawCircleLines(66, 55, 11, (Color){80, 45, 20, 255});
    }

    DrawText("OFICINA DA TARDIS", 105, 34, 36, (Color){100, 210, 255, 255});
    DrawText("Compre power-ups para esta expedicao usando fragmentos.",
                  107, 80, 18, RAYWHITE);
    DrawText(TextFormat("CARTEIRA: %d FRAGMENTOS", historia.fragmentos),
                  660, 50, 22, GOLD);

    for (int i = 0; i < TOTAL_ITENS_LOJA; i++) {
        Color fundo = mouseSobre(mouse, areas[i])
            ? (Color){45, 75, 105, 255} : (Color){29, 45, 68, 255};
        int indice_icone = i - 1;
        int quantidade = lojaQuantidade(historia, i);

        DrawRectangleRounded(areas[i], 0.12f, 8, fundo);
        DrawRectangleLinesEx(areas[i], 2.0f, (Color){75, 125, 165, 255});

        if (i == ITEM_VIDA) {
            DrawCircle((int)areas[i].x + 48, (int)areas[i].y + 62, 20, (Color){190, 55, 75, 255});
            DrawCircle((int)areas[i].x + 68, (int)areas[i].y + 62, 20, (Color){190, 55, 75, 255});
            DrawTriangle((Vector2){areas[i].x + 29, areas[i].y + 70},
                         (Vector2){areas[i].x + 87, areas[i].y + 70},
                         (Vector2){areas[i].x + 58, areas[i].y + 100},
                         (Color){190, 55, 75, 255});
        } else {
            desenharIconeDica(imagens_dicas[indice_icone],
                (Rectangle){areas[i].x + 18, areas[i].y + 9, 70, 105}, WHITE);
        }

        DrawText(lojaNome(i), (int)areas[i].x + 105, (int)areas[i].y + 17,
                      22, RAYWHITE);
        DrawText(lojaDescricao(i), (int)areas[i].x + 105, (int)areas[i].y + 50,
                      16, LIGHTGRAY);
        DrawText(TextFormat("Atual: %d   Preco: %d fragmentos",
                      quantidade, lojaPreco(i)), (int)areas[i].x + 105,
                      (int)areas[i].y + 82, 17, GOLD);
        DrawText(TextFormat("Comprar: tecla %d ou clique", i + 1),
                      (int)areas[i].x + 105, (int)areas[i].y + 103, 15,
                      (quantidade >= (i == ITEM_VIDA ? historia.vidas_maximas :
                       (i == ITEM_SONICA ? CARGAS_SONICA_MAXIMAS :
                       (i == ITEM_ARQUIVOS ? CARGAS_ARQUIVO_MAXIMAS : REVERSOES_MAXIMAS))))
                       ? GRAY : SKYBLUE);
    }

    DrawRectangleRounded((Rectangle){55, 475, 690, 85}, 0.12f, 8,
                         (Color){12, 24, 42, 255});
    DrawText(mensagem, 75, 495, 19, GOLD);
    DrawText("Dica: fragmentos sao ganhos ao acertar perguntas e concluir regioes.",
                  75, 525, 16, LIGHTGRAY);

    DrawRectangleRounded((Rectangle){785, 555, 170, 50}, 0.18f, 8,
                         mouseSobre(mouse, (Rectangle){785, 555, 170, 50})
                         ? (Color){55, 105, 145, 255} : (Color){31, 55, 78, 255});
    DrawText("VOLTAR [ESC]", 803, 570, 17, RAYWHITE);
}

/**
 * @brief Desenha o enunciado e as quatro alternativas.
 */
void desenharPergunta(Pergunta pergunta, Jogo jogo, Portal portais[], int modo, Versus versus, Historia historia, Chefe chefe, Texture2D imagens_chefe[], Texture2D fundos_mundos[], Texture2D imagens_dicas[]) {
    int numero = jogo.pergunta_atual + 1;

    /* Nas 40 perguntas de exploracao, o destino escolhido fornece o fundo. */
    if (modo == MODO_HISTORIA && jogo.pergunta_atual < PERGUNTA_INICIO_CHEFE) {
        desenharFundoDaRegiao(fundos_mundos, jogo);
    }

    if (modo == MODO_VERSUS) {
        DrawText("MODO VERSUS - JOGADOR X COMPUTADOR", 245, 20, 28, GOLD);
        DrawText(TextFormat("PERGUNTA %d / %d", numero, TOTAL_QUESTOES_VERSUS), 35, 65, 19, SKYBLUE);
        DrawText(TextFormat("Chance de acerto do computador: %d%%", chanceComputador(pergunta.dificuldade)),
                 300, 65, 19, LIGHTGRAY);
        DrawText(TextFormat("Dificuldade: %s", nomeDificuldade(pergunta.dificuldade)), 35, 105, 19, LIGHTGRAY);
        DrawText(TextFormat("Premio atual: R$ %d", valorPergunta(numero)), 700, 105, 19, GOLD);
    } else if (jogo.pergunta_atual >= PERGUNTA_INICIO_CHEFE) {
        /* Nas dez últimas perguntas, a interface passa a mostrar a arena do chefe. */
        desenharPainelChefe(chefe, jogo.pergunta_atual, historia, imagens_chefe);

        if (historia.regeneracao_em_andamento) {
            DrawText("PROTOCOLO DE REGENERACAO - DESAFIO EXTRA", 230, 145, 20, GOLD);
        }
    } else {
        int indice_portal = jogo.portal_por_regiao[jogo.regiao_atual];

        DrawText(TextFormat("MODO HISTORIA - PERGUNTA %d / %d", numero, TOTAL_QUESTOES_HISTORIA), 35, 20, 25, SKYBLUE);
        DrawText(TextFormat("Regiao: %d / %d", jogo.regiao_atual + 1, TOTAL_REGIOES_HISTORIA), 750, 20, 18, GOLD);
        DrawText(TextFormat("Dificuldade: %s", nomeDificuldade(pergunta.dificuldade)), 35, 58, 18, LIGHTGRAY);
        DrawText(TextFormat("Vidas: %d   Fragmentos: %d   Sonica: %d   Arquivos: %d   Reversoes: %d",
                 historia.vidas, historia.fragmentos, historia.cargas_sonica,
                 historia.cargas_arquivo, historia.reversoes_temporais), 35, 95, 15, LIGHTGRAY);

        if (!historia.regeneracao_em_andamento && indice_portal >= 0 && indice_portal < TOTAL_PORTAIS) {
            DrawText(TextFormat("Era: %s", portais[indice_portal].nome), 650, 58, 18, portais[indice_portal].cor);
        }
    }

    DrawRectangleRounded((Rectangle){35, 150, 930, 155}, 0.08f, 8, (Color){29, 42, 62, 238});
    desenharTextoQuebrado(pergunta.enunciado, 60, 185, 25, 880, RAYWHITE);

    /* Desenha as quatro alternativas e destaca aquela sob o cursor. */
    Vector2 mouse = obterMouseVirtual();
    for (int i = 0; i < TOTAL_ALTERNATIVAS; i++) {
        int x = (i % 2 == 0) ? 45 : 515;
        int y = (i < 2) ? 335 : 425;
        Rectangle area = {(float)x, (float)y, 440.0f, 68.0f};
        Color cor_fundo = mouseSobre(mouse, area) ? (Color){65, 100, 140, 245} : (Color){37, 55, 78, 238};
        int alternativa_visivel = 1;

        if (modo == MODO_VERSUS) {
            alternativa_visivel = versus.alternativas_visiveis[i];
        } else {
            alternativa_visivel = historia.alternativas_visiveis[i];
        }

        if (!alternativa_visivel) {
            cor_fundo = (Color){20, 25, 34, 255};
        }

        DrawRectangleRounded((Rectangle){(float)x, (float)y, 440, 68}, 0.12f, 8, cor_fundo);

        if (!alternativa_visivel) {
            DrawText("Alternativa removida pela dica", x + 18, y + 22, 17, DARKGRAY);
        } else {
            char texto_alternativa[180];
            snprintf(texto_alternativa, sizeof(texto_alternativa), "%c) %s", 'A' + i, pergunta.alternativas[i]);
            desenharTextoQuebrado(texto_alternativa, x + 18, y + 9, 19, 400, WHITE);
        }
    }

    if (modo == MODO_VERSUS) {
        DrawText("F - Dica 50/50", 55, 520, 18, versus.usou_cinquenta ? GRAY : SKYBLUE);
        DrawText("P - Consultar plateia", 245, 520, 18, versus.usou_plateia ? GRAY : SKYBLUE);

        if (versus.mostrar_votos) {
            DrawText("Votos da plateia:", 500, 515, 16, LIGHTGRAY);
            for (int i = 0; i < TOTAL_ALTERNATIVAS; i++) {
                DrawText(TextFormat("%c: %d%%", 'A' + i, versus.votos_plateia[i]),
                         640 + i * 75, 515, 16, RAYWHITE);
            }
        }

        DrawText("Clique em uma alternativa ou responda com A, B, C ou D.", 250, 575, 18, LIGHTGRAY);
    } else if (historia.regeneracao_em_andamento) {
        /* O desafio final não permite usar dicas. */
        DrawText("SEM DICAS: acerte para sobreviver; erre e a expedicao termina.", 170, 530, 19, GOLD);
        DrawText("Clique em uma alternativa ou use A, B, C ou D.", 265, 575, 18, LIGHTGRAY);
    } else {
        /* Cada dica recebe seu próprio cartão, ícone e atalho de teclado. */
        Rectangle botoes_dicas[2] = {
            {220, 495, 240, 65}, {475, 495, 240, 65}
        };
        Color cor_sonica = historia.cargas_sonica > 0 && !historia.usou_sonica_pergunta
            ? (Color){35, 75, 105, 245} : (Color){35, 42, 54, 230};
        Color cor_arquivo = historia.cargas_arquivo > 0 && !historia.usou_arquivo_pergunta
            ? (Color){35, 75, 105, 245} : (Color){35, 42, 54, 230};

        DrawRectangleRounded(botoes_dicas[0], 0.16f, 8, cor_sonica);
        DrawRectangleRounded(botoes_dicas[1], 0.16f, 8, cor_arquivo);

        desenharIconeDica(imagens_dicas[0], (Rectangle){228, 503, 48, 48}, WHITE);
        desenharIconeDica(imagens_dicas[1], (Rectangle){483, 503, 48, 48}, WHITE);

        DrawText("S - Chave Sonica", 285, 507, 18,
                 historia.cargas_sonica > 0 && !historia.usou_sonica_pergunta ? SKYBLUE : GRAY);
        DrawText(TextFormat("Cargas: %d", historia.cargas_sonica), 285, 532, 16, LIGHTGRAY);
        DrawText("H - Arquivos TARDIS", 540, 507, 17,
                 historia.cargas_arquivo > 0 && !historia.usou_arquivo_pergunta ? SKYBLUE : GRAY);
        DrawText(TextFormat("Cargas: %d", historia.cargas_arquivo), 540, 532, 16, LIGHTGRAY);

        if (historia.mostrar_votos) {
            DrawText("Leitura dos arquivos:", 735, 505, 15, LIGHTGRAY);
            for (int i = 0; i < TOTAL_ALTERNATIVAS; i++) {
                DrawText(TextFormat("%c: %d%%", 'A' + i, historia.votos_arquivo[i]),
                         735 + (i % 2) * 95, 530 + (i / 2) * 18, 15, RAYWHITE);
            }
        }

        DrawText("Clique em uma alternativa ou use A, B, C ou D.", 275, 585, 17, LIGHTGRAY);
    }
}

/**
 * @brief Exibe o resultado final da expedição do Modo História.
 */
void desenharResultadoHistoria(Jogo jogo, Historia historia, Chefe chefe, Texture2D imagens_chefe[]) {
    if (chefe.venceu) {
        desenharSpriteChefe(imagens_chefe, 500, 112, 150.0f, 2);
        DrawText("O PARADOXO FOI DERROTADO!", 230, 205, 36, GREEN);
        DrawText("A TARDIS estabilizou a linha do tempo.", 270, 260, 23, RAYWHITE);
        DrawText("Voce concluiu as 50 perguntas da expedicao.", 245, 300, 22, LIGHTGRAY);
    } else if (chefe.derrotado) {
        DrawText("O PARADOXO CONSUMIU A LINHA DO TEMPO", 110, 160, 32, RED);
        DrawText("Os nucleos temporais resistiram ao seu ataque.", 220, 225, 22, RAYWHITE);
    } else if (!historia.game_over && jogo.acertou &&
        jogo.pergunta_atual == TOTAL_QUESTOES_HISTORIA - 1) {
        DrawText("LINHA DO TEMPO ESTABILIZADA!", 205, 145, 39, GREEN);
        DrawText("Voce concluiu as 50 perguntas da expedicao.", 240, 220, 23, RAYWHITE);
    } else {
        DrawText("EXPEDICAO INTERROMPIDA", 255, 145, 38, RED);
        DrawText("As anomalias temporais venceram desta vez.", 250, 220, 23, RAYWHITE);
    }

    DrawText(TextFormat("Perguntas respondidas: %d / %d", jogo.pergunta_atual + 1,
             TOTAL_QUESTOES_HISTORIA), 330, 355, 22, LIGHTGRAY);
    DrawText(TextFormat("Regioes de exploracao concluidas: %d / %d", historia.regioes_concluidas,
             TOTAL_REGIOES_HISTORIA), 300, 390, 21, LIGHTGRAY);
    if (chefe.batalha_iniciada) {
        DrawText(TextFormat("Nucleos destruidos: %d / %d", TOTAL_NUCLEOS_CHEFE - chefe.nucleos_restantes,
                 TOTAL_NUCLEOS_CHEFE), 350, 425, 21, SKYBLUE);
    }
    DrawText(TextFormat("Fragmentos acumulados: %d", historia.fragmentos), 330, 460, 21, GOLD);
    DrawText(TextFormat("Vidas restantes: %d", historia.vidas), 385, 490, 20, SKYBLUE);
    DrawText("Pressione ENTER para voltar ao menu", 300, 560, 21, LIGHTGRAY);
}

/**
 * @brief Mostra o resultado da resposta no Modo História.
 *
 * A vida só é retirada quando o jogador pressiona ENTER. Antes disso, ele
 * pode usar a tecla T para substituir a pergunta por outra do mesmo nível.
 */
void desenharFeedbackHistoria(Jogo jogo, Historia historia, Pergunta pergunta, Chefe chefe, Texture2D imagens_chefe[], Texture2D fundos_mundos[], Texture2D imagens_dicas[]) {
    /* Mantem o mesmo cenário enquanto o jogador visualiza o resultado. */
    if (jogo.pergunta_atual < PERGUNTA_INICIO_CHEFE) {
        desenharFundoDaRegiao(fundos_mundos, jogo);
    }

    /* O desafio de regeneração tem um resultado próprio e não usa vidas comuns. */
    if (historia.regeneracao_em_andamento) {
        if (chefe.batalha_iniciada) {
            desenharSpriteChefe(imagens_chefe, 500, 70, 105.0f, chefeFasePergunta(jogo.pergunta_atual));
            DrawText(TextFormat("Nucleos temporais restantes: %d", chefe.nucleos_restantes),
                     345, 112, 18, SKYBLUE);
        }

        if (jogo.acertou) {
            DrawText("REGENERACAO ESTABILIZADA!", 240, 145, 39, GREEN);
            DrawText("Voce acertou a pergunta muito dificil.", 275, 235, 23, RAYWHITE);
            DrawText("A TARDIS restaurara uma vida e a expedicao continuara.", 175, 300, 21, SKYBLUE);
        } else {
            DrawText("FALHA NO PROTOCOLO DE REGENERACAO", 145, 145, 34, RED);
            DrawText(TextFormat("A resposta correta era %c", pergunta.resposta_correta), 350, 235, 23, RAYWHITE);
            DrawText("Voce errou. A expedicao sera encerrada.", 265, 300, 22, RED);
        }

        DrawText("Clique aqui ou pressione ENTER para confirmar", 280, 525, 19, LIGHTGRAY);
        return;
    }

    /* O feedback do chefe explica o ataque e a regra de destruição dos núcleos. */
    if (jogo.pergunta_atual >= PERGUNTA_INICIO_CHEFE) {
        int posicao_rodada = (jogo.pergunta_atual - PERGUNTA_INICIO_CHEFE) % PERGUNTAS_POR_RODADA_CHEFE;
        int acertos_previstos = chefe.acertos_na_rodada + (jogo.acertou ? 1 : 0);

        desenharSpriteChefe(imagens_chefe, 500, 70, 105.0f, chefeFasePergunta(jogo.pergunta_atual));
        DrawText(TextFormat("Nucleos temporais restantes: %d", chefe.nucleos_restantes),
                 345, 112, 18, SKYBLUE);

        if (jogo.acertou) {
            DrawText("ATAQUE EFETIVO!", 350, 155, 38, GREEN);
            DrawText("Seu conhecimento atingiu O Paradoxo.", 280, 225, 23, RAYWHITE);
            DrawText(TextFormat("Fragmentos recebidos: +%d", fragmentosPergunta(pergunta.dificuldade)),
                     330, 275, 21, GOLD);
        } else {
            DrawText("O PARADOXO CONTRA-ATACA!", 245, 155, 36, RED);
            DrawText(TextFormat("A resposta correta era %c.", pergunta.resposta_correta),
                     350, 225, 22, RAYWHITE);
            DrawText("Ao confirmar, voce perdera uma vida.", 300, 275, 21, SKYBLUE);
        }

        if (posicao_rodada == PERGUNTAS_POR_RODADA_CHEFE - 1) {
            if (acertos_previstos > 0) {
                DrawText("Esta dupla vai destruir um nucleo temporal.", 260, 350, 21, GREEN);
                if (acertos_previstos == PERGUNTAS_POR_RODADA_CHEFE) {
                    DrawText(TextFormat("Bonus por acertar as duas: +%d fragmentos", BONUS_RODADA_PERFEITA),
                             290, 390, 19, GOLD);
                }
            } else {
                DrawText("O nucleo resistira: nenhuma pergunta da dupla foi acertada.",
                         160, 350, 19, RED);
            }
        }

        if (jogo.pergunta_atual == TOTAL_QUESTOES_HISTORIA - 1) {
            if (jogo.acertou && acertos_previstos > 0 && chefe.nucleos_restantes == 1) {
                DrawText("GOLPE FINAL! Destrua o ultimo nucleo e vença O Paradoxo.",
                         125, 445, 19, GOLD);
            } else {
                DrawText("A pergunta 50 precisa ser acertada e os cinco nucleos devem cair.",
                         125, 445, 19, GOLD);
            }
        }

        if (!jogo.acertou && historia.reversoes_temporais > 0) {
            DrawRectangleRounded((Rectangle){145, 465, 710, 65}, 0.16f, 8,
                                 (Color){30, 55, 85, 245});
            desenharIconeDica(imagens_dicas[2], (Rectangle){153, 468, 48, 58}, WHITE);
            DrawText("T - Reversao Temporal", 215, 474, 19, SKYBLUE);
            DrawText("Sortear outra pergunta muito dificil", 215, 500, 16, LIGHTGRAY);
        }

        DrawText("Clique aqui ou pressione ENTER para continuar", 275, 555, 19, LIGHTGRAY);
        return;
    }

    if (jogo.acertou) {
        DrawText("RESPOSTA CORRETA!", 325, 145, 38, GREEN);
        DrawText(TextFormat("Fragmentos recebidos: +%d", fragmentosPergunta(pergunta.dificuldade)),
                 320, 235, 25, GOLD);
        DrawText(TextFormat("Vidas preservadas: %d", historia.vidas), 345, 290, 22, SKYBLUE);
        if ((jogo.pergunta_atual + 1) % QUESTOES_POR_REGIAO == 0) {
            DrawText("Ao confirmar: bonus de conclusao + recompensa do portal", 205, 360, 18, GOLD);
        }
        DrawText("Clique aqui ou pressione ENTER para continuar", 280, 525, 19, LIGHTGRAY);
    } else {
        DrawText("RESPOSTA INCORRETA!", 300, 125, 38, RED);
        DrawText(TextFormat("A resposta correta era %c", pergunta.resposta_correta), 335, 205, 23, RAYWHITE);

        if (historia.vidas > 1) {
            DrawText(TextFormat("Vidas apos confirmar: %d", historia.vidas - 1), 350, 275, 22, SKYBLUE);
        } else if (historia.regeneracao_disponivel) {
            DrawText("Protocolo de regeneracao: pergunta muito dificil, tudo ou nada!", 125, 275, 19, GOLD);
        } else {
            DrawText("Esta resposta encerrara a expedicao.", 315, 275, 21, RED);
        }

        if (historia.reversoes_temporais > 0) {
            DrawRectangleRounded((Rectangle){145, 350, 710, 65}, 0.16f, 8,
                                 (Color){30, 55, 85, 245});
            desenharIconeDica(imagens_dicas[2], (Rectangle){153, 353, 48, 58}, WHITE);
            DrawText("T - Reversao Temporal", 215, 359, 19, SKYBLUE);
            DrawText("Sortear outra pergunta do mesmo nivel", 215, 385, 16, LIGHTGRAY);
        }
        if ((jogo.pergunta_atual + 1) % QUESTOES_POR_REGIAO == 0) {
            DrawText("Ao sobreviver a esta pergunta, voce conclui a regiao e recebe a recompensa.",
                     100, 420, 17, GOLD);
        }
        DrawText("Clique aqui ou pressione ENTER para continuar", 280, 525, 19, LIGHTGRAY);
    }
}

/**
 * @brief Mostra o resultado da rodada do Modo Versus.
 *
 * Informa se o jogador e o computador acertaram, a chance utilizada pelo
 * computador e se a disputa continua ou terminou.
 */
void desenharFeedbackVersus(Jogo jogo, Versus versus, Pergunta pergunta) {
    if (jogo.acertou) {
        DrawText("VOCE ACERTOU!", 345, 145, 36, GREEN);
    } else {
        DrawText("VOCE ERROU!", 365, 145, 36, RED);
    }

    if (versus.computador_acertou) {
        DrawText("O COMPUTADOR ACERTOU!", 300, 220, 28, GREEN);
    } else {
        DrawText("O COMPUTADOR ERROU!", 300, 220, 28, RED);
    }

    DrawText(TextFormat("Chance de acerto do computador: %d%%", versus.chance_computador),
             300, 275, 20, LIGHTGRAY);
    if (pergunta.resposta_correta >= 'A' && pergunta.resposta_correta <= 'D') {
        DrawText(TextFormat("Resposta correta: %c) %s", pergunta.resposta_correta,
                 pergunta.alternativas[pergunta.resposta_correta - 'A']), 230, 315, 19, GREEN);
    }
    DrawText(TextFormat("Seu premio: R$ %d", versus.premio_jogador), 260, 370, 23, GOLD);
    DrawText(TextFormat("Premio do computador: R$ %d", versus.premio_computador), 530, 370, 23, SKYBLUE);

    if (versus.terminou) {
        DrawText("A DISPUTA TERMINOU", 345, 435, 27, RAYWHITE);
        DrawText("Clique aqui ou pressione ENTER para ver o resultado", 250, 520, 19, LIGHTGRAY);
    } else {
        DrawText("Os dois continuam na disputa!", 345, 435, 24, RAYWHITE);
        DrawText("Clique aqui ou pressione ENTER para continuar", 260, 520, 19, LIGHTGRAY);
    }

}

/**
 * @brief Exibe o vencedor e os prêmios finais do Modo Versus.
 */
void desenharResultadoVersus(Versus versus) {
    if (versus.vencedor == VERSUS_JOGADOR) {
        DrawText("VOCE VENCEU!", 350, 150, 42, GREEN);
        DrawText("O computador errou antes de voce.", 280, 220, 23, RAYWHITE);
    } else if (versus.vencedor == VERSUS_COMPUTADOR) {
        DrawText("O COMPUTADOR VENCEU!", 245, 150, 42, RED);
        DrawText("Voce errou antes do computador.", 285, 220, 23, RAYWHITE);
    } else {
        DrawText("EMPATE!", 390, 150, 42, GOLD);
        DrawText("Os dois terminaram com o mesmo resultado.", 245, 220, 23, RAYWHITE);
    }

    DrawText(TextFormat("Seu premio: R$ %d", versus.premio_jogador), 250, 330, 27, GOLD);
    DrawText(TextFormat("Premio do computador: R$ %d", versus.premio_computador), 535, 330, 27, SKYBLUE);
    DrawText("Clique aqui ou pressione ENTER para voltar ao menu", 255, 510, 19, LIGHTGRAY);
}

/**
 * @brief Desenha uma pergunta da coleção e permite navegar por dificuldade.
 */
void desenharConsulta(ListaPerguntas *lista, enum Dificuldade dificuldade, int indice) {
    Pergunta *pergunta = perguntaDaDificuldade(lista, dificuldade, indice);
    int total = contarDificuldade(lista, dificuldade);
    Vector2 mouse = obterMouseVirtual();
    const char *nomes_niveis[5] = {"Muito facil", "Facil", "Medio", "Dificil", "Muito dificil"};
    int posicoes_x[5] = {100, 250, 370, 490, 620};
    int larguras[5] = {145, 115, 115, 125, 190};

    DrawText("CONSULTA DE PERGUNTAS", 300, 30, 34, SKYBLUE);
    DrawText("Clique em um nivel para filtrar as perguntas", 300, 82, 18, LIGHTGRAY);

    /* Os cinco níveis são apresentados como botões clicáveis. */
    for (int i = 0; i < 5; i++) {
        Rectangle area = {(float)posicoes_x[i], 115, (float)larguras[i], 42};
        Color fundo = dificuldade == (enum Dificuldade)(i + 1) ? (Color){35, 105, 150, 255} : (Color){29, 42, 62, 255};
        if (mouseSobre(mouse, area)) fundo = (Color){65, 105, 145, 255};
        DrawRectangleRounded(area, 0.18f, 8, fundo);
        DrawText(TextFormat("%d - %s", i + 1, nomes_niveis[i]),
                 posicoes_x[i] + 8, 127, 15, RAYWHITE);
    }

    DrawText(TextFormat("Nivel: %s", nomeDificuldade(dificuldade)), 60, 205, 26, GOLD);
    DrawText(TextFormat("Pergunta %d de %d", total == 0 ? 0 : indice + 1, total), 730, 205, 22, LIGHTGRAY);

    if (pergunta != NULL) {
        DrawRectangleRounded((Rectangle){45, 250, 910, 295}, 0.08f, 8, (Color){29, 42, 62, 255});
        DrawText(TextFormat("ID: %d", pergunta->id), 70, 275, 18, GRAY);
        desenharTextoQuebrado(pergunta->enunciado, 70, 320, 23, 850, WHITE);

        /* A consulta é um modo de estudo: a alternativa correta fica destacada. */
        for (int i = 0; i < TOTAL_ALTERNATIVAS; i++) {
            int y = 410 + i * 25;
            Color cor = (pergunta->resposta_correta == 'A' + i) ? GREEN : RAYWHITE;
            DrawText(TextFormat("%c) %s", 'A' + i, pergunta->alternativas[i]), 75, y, 17, cor);
        }

        if (pergunta->resposta_correta >= 'A' && pergunta->resposta_correta <= 'D') {
            int indice_correto = pergunta->resposta_correta - 'A';
            DrawText(TextFormat("Resposta correta: %c) %s", pergunta->resposta_correta,
                     pergunta->alternativas[indice_correto]), 70, 520, 19, GREEN);
        } else {
            DrawText("Esta pergunta nao possui uma resposta correta valida.", 70, 520, 18, RED);
        }
    } else {
        DrawText("Nao ha perguntas cadastradas neste nivel.", 260, 350, 24, LIGHTGRAY);
    }

    /* Botões de navegação por mouse; as setas do teclado continuam disponíveis. */
    DrawRectangleRounded((Rectangle){375, 560, 90, 42}, 0.18f, 8,
        mouseSobre(mouse, (Rectangle){375, 560, 90, 42}) ? (Color){65, 105, 145, 255} : (Color){29, 42, 62, 255});
    DrawRectangleRounded((Rectangle){535, 560, 90, 42}, 0.18f, 8,
        mouseSobre(mouse, (Rectangle){535, 560, 90, 42}) ? (Color){65, 105, 145, 255} : (Color){29, 42, 62, 255});
    DrawRectangleRounded((Rectangle){700, 560, 180, 42}, 0.18f, 8,
        mouseSobre(mouse, (Rectangle){700, 560, 180, 42}) ? (Color){65, 105, 145, 255} : (Color){29, 42, 62, 255});
    DrawText("ANTERIOR", 386, 573, 15, RAYWHITE);
    DrawText("PROXIMA", 550, 573, 15, RAYWHITE);
    DrawText("VOLTAR", 746, 573, 15, RAYWHITE);
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

    if (lista == NULL || lista->dados == NULL || indice < 0) return NULL;

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
int contarPerguntasVersus(ListaPerguntas *lista) {
    if (lista == NULL || lista->dados == NULL) return 0;

    return contarDificuldade(lista, MUITO_FACIL) >= 2 &&
           contarDificuldade(lista, FACIL) >= 2 &&
           contarDificuldade(lista, MEDIO) >= 4 &&
           contarDificuldade(lista, DIFICIL) >= 4 &&
           contarDificuldade(lista, MUITO_DIFICIL) >= 3;
}


/**
 * @brief Confere se há dez perguntas disponíveis em cada dificuldade.
 *
 * A História precisa de 50 perguntas: dez muito fáceis, dez fáceis,
 * dez médias, dez difíceis e dez muito difíceis.
 */
int contarPerguntasHistoria(ListaPerguntas *lista) {
    if (lista == NULL || lista->dados == NULL) return 0;

    for (int nivel = MUITO_FACIL; nivel <= MUITO_DIFICIL; nivel++) {
        if (contarDificuldade(lista, (enum Dificuldade)nivel) < 10) {
            return 0;
        }
    }

    return 1;
}

/**
 * @brief Retorna a pergunta que deve ser exibida no momento.
 *
 * Normalmente, a pergunta vem da sequência sorteada para as 50 posições.
 * Durante o Protocolo de Regeneração, é utilizada a pergunta especial muito
 * difícil armazenada separadamente na estrutura Historia.
 */
Pergunta perguntaAtual(ListaPerguntas *lista, Jogo jogo, Historia historia) {
    Pergunta pergunta = {0};
    int indice;

    if (lista == NULL || lista->dados == NULL) return pergunta;

    if (historia.regeneracao_em_andamento) {
        indice = historia.indice_pergunta_regeneracao;
    } else {
        if (jogo.pergunta_atual < 0 || jogo.pergunta_atual >= jogo.total_perguntas ||
            jogo.pergunta_atual >= TOTAL_QUESTOES_HISTORIA) {
            return pergunta;
        }
        indice = jogo.perguntas_sorteadas[jogo.pergunta_atual];
    }

    if (indice < 0 || indice >= lista->quantidade) return pergunta;
    return lista->dados[indice];
}
