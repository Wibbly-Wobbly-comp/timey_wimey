/**
 * @file teste_core.c
 * @brief Testes automáticos das principais regras do Timey Wimey.
 *
 * Estes testes não abrem a janela gráfica. Eles verificam banco de perguntas,
 * sorteio, dicas, regeneração, recompensas e escolha de portais.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pergunta.h"
#include "jogo.h"
#include "historia.h"
#include "versus.h"
#include "portal.h"
#include "chefe.h"
#include "loja.h"

static void testarBanco(ListaPerguntas *lista) {
    ListaPerguntas exportada;
    Pergunta extra = {0};

    iniciarLista(&exportada);
    assert(carregarCSV(lista, "dados/perguntas_iniciais.csv") == 1000);

    /* O novo banco possui exatamente 200 perguntas em cada dificuldade. */
    for (int nivel = 1; nivel <= 5; nivel++) {
        assert(contarDificuldade(lista, (enum Dificuldade)nivel) == 200);
    }

    /* Verifica também textos que contêm o separador e aspas. */
    extra.id = lista->proximo_id;
    snprintf(extra.enunciado, sizeof(extra.enunciado), "Pergunta de teste; com \"aspas\"");
    snprintf(extra.alternativas[0], sizeof(extra.alternativas[0]), "Resposta A");
    snprintf(extra.alternativas[1], sizeof(extra.alternativas[1]), "Resposta; B");
    snprintf(extra.alternativas[2], sizeof(extra.alternativas[2]), "Resposta C");
    snprintf(extra.alternativas[3], sizeof(extra.alternativas[3]), "Resposta D");
    extra.resposta_correta = 'B';
    extra.dificuldade = MUITO_FACIL;
    assert(adicionarPergunta(lista, extra));
    /* O cadastro nao pode aceitar dois registros com o mesmo ID. */
    assert(!adicionarPergunta(lista, extra));

    assert(exportarCSV(lista, "testes/banco_teste.csv"));
    assert(carregarCSV(&exportada, "testes/banco_teste.csv") == lista->quantidade);
    assert(strcmp(exportada.dados[exportada.quantidade - 1].enunciado,
                   extra.enunciado) == 0);
    assert(strcmp(exportada.dados[exportada.quantidade - 1].alternativas[1],
                   extra.alternativas[1]) == 0);

    assert(salvarBinario(lista, "testes/banco_teste.bin"));
    /* Salvar novamente no mesmo destino testa a substituicao do binario existente. */
    assert(salvarBinario(lista, "testes/banco_teste.bin"));
    assert(carregarBinario(&exportada, "testes/banco_teste.bin"));
    assert(exportada.quantidade == lista->quantidade);

    remove("testes/banco_teste.csv");
    remove("testes/banco_teste.bin");
    liberarLista(&exportada);
}


static void testarCSVInvalido(void) {
    ListaPerguntas lista;
    FILE *arquivo;

    iniciarLista(&lista);
    arquivo = fopen("testes/csv_invalido.csv", "w");
    assert(arquivo != NULL);

    fprintf(arquivo, "id;enunciado;alternativa_a;alternativa_b;alternativa_c;alternativa_d;resposta_correta;dificuldade\n");
    fprintf(arquivo, "7;Pergunta valida;A;B;C;D;A;1\n");
    fprintf(arquivo, "7;ID repetido;A;B;C;D;A;1\n");
    fprintf(arquivo, "8x;ID com letras;A;B;C;D;A;1\n");
    fprintf(arquivo, "9;Dificuldade invalida;A;B;C;D;A;2x\n");
    fclose(arquivo);

    assert(carregarCSV(&lista, "testes/csv_invalido.csv") == 1);
    assert(lista.quantidade == 1);
    assert(lista.dados[0].id == 7);

    remove("testes/csv_invalido.csv");
    liberarLista(&lista);
}

static void testarSorteio(ListaPerguntas *lista) {
    Jogo jogo;
    int quantidades[5] = {0};

    assert(iniciarJogo(&jogo, lista, TOTAL_QUESTOES_HISTORIA));

    for (int i = 0; i < TOTAL_QUESTOES_HISTORIA; i++) {
        int indice = jogo.perguntas_sorteadas[i];
        enum Dificuldade nivel = lista->dados[indice].dificuldade;
        assert(nivel >= MUITO_FACIL && nivel <= MUITO_DIFICIL);
        quantidades[nivel - 1]++;

        for (int j = 0; j < i; j++) {
            assert(jogo.perguntas_sorteadas[j] != indice);
        }
    }

    for (int i = 0; i < 5; i++) assert(quantidades[i] == 10);

    assert(iniciarJogo(&jogo, lista, TOTAL_QUESTOES_VERSUS));
    assert(lista->dados[jogo.perguntas_sorteadas[0]].dificuldade == MUITO_FACIL);
    assert(lista->dados[jogo.perguntas_sorteadas[1]].dificuldade == MUITO_FACIL);
    assert(lista->dados[jogo.perguntas_sorteadas[2]].dificuldade == FACIL);
    assert(lista->dados[jogo.perguntas_sorteadas[3]].dificuldade == FACIL);
    assert(lista->dados[jogo.perguntas_sorteadas[14]].dificuldade == MUITO_DIFICIL);
}

static void testarReversao(ListaPerguntas *lista) {
    Jogo jogo;
    Historia historia;
    Pergunta *atual;
    int antiga;

    assert(iniciarJogo(&jogo, lista, TOTAL_QUESTOES_HISTORIA));
    iniciarHistoria(&historia);
    atual = &lista->dados[jogo.perguntas_sorteadas[0]];
    antiga = jogo.perguntas_sorteadas[0];

    responderPergunta(&jogo, *atual, (atual->resposta_correta - 'A' + 1) % TOTAL_ALTERNATIVAS, 0);
    assert(jogo.respondeu && !jogo.acertou);
    assert(historiaUsarReversao(&historia, &jogo, lista));
    assert(jogo.perguntas_sorteadas[0] != antiga);
    assert(lista->dados[jogo.perguntas_sorteadas[0]].dificuldade == atual->dificuldade);
    assert(historia.reversoes_temporais == REVERSOES_INICIAIS - 1);

    /* Nenhum índice pode aparecer duas vezes depois da troca. */
    for (int i = 0; i < TOTAL_QUESTOES_HISTORIA; i++) {
        for (int j = i + 1; j < TOTAL_QUESTOES_HISTORIA; j++) {
            assert(jogo.perguntas_sorteadas[i] != jogo.perguntas_sorteadas[j]);
        }
    }
}

static void testarRegeneracao(ListaPerguntas *lista) {
    Jogo jogo;
    Historia historia;
    Pergunta *pergunta;

    assert(iniciarJogo(&jogo, lista, TOTAL_QUESTOES_HISTORIA));
    iniciarHistoria(&historia);
    historia.vidas = 1;

    pergunta = &lista->dados[jogo.perguntas_sorteadas[0]];
    responderPergunta(&jogo, *pergunta,
        (pergunta->resposta_correta - 'A' + 1) % TOTAL_ALTERNATIVAS, 0);
    historiaProcessarResposta(&historia, &jogo, *pergunta);
    assert(historia.regeneracao_em_andamento);
    assert(historiaIniciarRegeneracao(&historia, &jogo, lista));

    pergunta = &lista->dados[historia.indice_pergunta_regeneracao];
    assert(pergunta->dificuldade == MUITO_DIFICIL);
    responderPergunta(&jogo, *pergunta, pergunta->resposta_correta - 'A', 0);
    historiaProcessarResposta(&historia, &jogo, *pergunta);
    assert(historia.vidas == 1);
    assert(!historia.game_over);

    /* Segunda tentativa: errar o desafio encerra a expedição. */
    assert(iniciarJogo(&jogo, lista, TOTAL_QUESTOES_HISTORIA));
    iniciarHistoria(&historia);
    historia.vidas = 1;
    pergunta = &lista->dados[jogo.perguntas_sorteadas[0]];
    responderPergunta(&jogo, *pergunta,
        (pergunta->resposta_correta - 'A' + 1) % TOTAL_ALTERNATIVAS, 0);
    historiaProcessarResposta(&historia, &jogo, *pergunta);
    assert(historiaIniciarRegeneracao(&historia, &jogo, lista));
    pergunta = &lista->dados[historia.indice_pergunta_regeneracao];
    responderPergunta(&jogo, *pergunta,
        (pergunta->resposta_correta - 'A' + 1) % TOTAL_ALTERNATIVAS, 0);
    historiaProcessarResposta(&historia, &jogo, *pergunta);
    assert(historia.game_over);
    assert(historia.vidas == 0);
}

static void testarVersus(void) {
    Versus versus;
    Pergunta pergunta = {0};

    assert(chanceComputador(MUITO_FACIL) == 90);
    assert(chanceComputador(FACIL) == 80);
    assert(chanceComputador(MEDIO) == 60);
    assert(chanceComputador(DIFICIL) == 40);
    assert(chanceComputador(MUITO_DIFICIL) == 20);

    pergunta.resposta_correta = 'C';
    iniciarVersus(&versus);
    assert(usarCinquenta(&versus, pergunta));
    assert(versus.alternativas_visiveis[2]);
    {
        int escondidas = 0;
        for (int i = 0; i < TOTAL_ALTERNATIVAS; i++) {
            if (!versus.alternativas_visiveis[i]) escondidas++;
        }
        assert(escondidas == 2);
    }
    assert(!usarCinquenta(&versus, pergunta));
    assert(usarPlateia(&versus, pergunta));
    assert(versus.mostrar_votos);
    assert(versus.votos_plateia[0] + versus.votos_plateia[1] +
           versus.votos_plateia[2] + versus.votos_plateia[3] == 100);
}


static void testarRecompensas(void) {
    Historia historia;
    int fragmentos_antes;

    iniciarHistoria(&historia);
    historiaConcluirRegiao(&historia, 0);
    assert(historia.regioes_concluidas == 1);
    assert(historia.fragmentos == 25);

    /* O Modo História possui oito regiões, não dez. */
    historia.regioes_concluidas = TOTAL_REGIOES_HISTORIA;
    historiaConcluirRegiao(&historia, TOTAL_REGIOES_HISTORIA);
    assert(historia.regioes_concluidas == TOTAL_REGIOES_HISTORIA);
    assert(historia.fragmentos == 25);
    historia.regioes_concluidas = 1;

    /* A mesma região não pode pagar o bônus duas vezes. */
    historiaConcluirRegiao(&historia, 0);
    assert(historia.regioes_concluidas == 1);
    assert(historia.fragmentos == 25);

    historiaRecompensaPortal(&historia, RECOMPENSA_VIDA, 1);
    assert(historia.vidas == VIDAS_INICIAIS + 1);

    /* Recompensa de recurso no limite vira fragmentos, em vez de ser perdida. */
    historia.cargas_sonica = CARGAS_SONICA_MAXIMAS;
    fragmentos_antes = historia.fragmentos;
    historiaRecompensaPortal(&historia, RECOMPENSA_SONICA, 1);
    assert(historia.fragmentos == fragmentos_antes + 20);
}




static void testarFragmentosComoMoeda(void) {
    Historia historia;
    Jogo jogo = {0};
    Pergunta pergunta = {0};

    iniciarHistoria(&historia);
    historia.vidas = 1;
    pergunta.dificuldade = MUITO_FACIL;
    jogo.acertou = 1;
    jogo.respondeu = 1;

    /* Acertar aumenta a carteira, mas não concede nível nem vida automática. */
    historiaProcessarResposta(&historia, &jogo, pergunta);
    assert(historia.fragmentos == fragmentosPergunta(MUITO_FACIL));
    assert(historia.vidas == 1);

    /* Uma nova expedição inicia uma nova carteira temporária. */
    iniciarHistoria(&historia);
    assert(historia.fragmentos == 0);
}

static void testarLoja(void) {
    Historia historia;
    int saldo_inicial;

    iniciarHistoria(&historia);
    assert(lojaPreco(ITEM_VIDA) == 300);
    assert(lojaPreco(ITEM_REVERSAO) == 150);
    assert(lojaPreco(ITEM_SONICA) == 75);
    assert(lojaPreco(ITEM_ARQUIVOS) == 100);
    assert(strlen(lojaDescricao(ITEM_VIDA)) > 10);
    assert(strlen(lojaDescricao(ITEM_SONICA)) > 10);
    assert(strlen(lojaDescricao(ITEM_ARQUIVOS)) > 10);
    assert(strlen(lojaDescricao(ITEM_REVERSAO)) > 10);

    historiaConcederFragmentos(&historia, 400);
    saldo_inicial = historia.fragmentos;

    /* A compra reduz a carteira e aumenta a carga da dica correspondente. */
    assert(lojaComprar(&historia, ITEM_SONICA) == COMPRA_SUCESSO);
    assert(historia.fragmentos == saldo_inicial - lojaPreco(ITEM_SONICA));
    assert(historia.cargas_sonica == CARGAS_SONICA_INICIAIS + 1);

    /* Uma compra sem saldo não altera a carteira nem o recurso. */
    historia.fragmentos = 0;
    saldo_inicial = historia.cargas_arquivo;
    assert(lojaComprar(&historia, ITEM_ARQUIVOS) == COMPRA_SEM_FRAGMENTOS);
    assert(historia.cargas_arquivo == saldo_inicial);

    /* O jogador não pode comprar acima do limite de um recurso. */
    historia.fragmentos = 500;
    historia.cargas_sonica = CARGAS_SONICA_MAXIMAS;
    saldo_inicial = historia.fragmentos;
    assert(lojaComprar(&historia, ITEM_SONICA) == COMPRA_LIMITE_RECURSO);
    assert(historia.fragmentos == saldo_inicial);

    /* Compra de vida também respeita o limite máximo da expedição. */
    historia.vidas = historia.vidas_maximas - 1;
    assert(lojaComprar(&historia, ITEM_VIDA) == COMPRA_SUCESSO);
    assert(historia.vidas == historia.vidas_maximas);
    assert(lojaComprar(&historia, ITEM_VIDA) == COMPRA_LIMITE_RECURSO);

    /* Os dois outros itens aumentam suas cargas e descontam o preço correto. */
    historia.fragmentos = 300;
    saldo_inicial = historia.fragmentos;
    assert(lojaComprar(&historia, ITEM_ARQUIVOS) == COMPRA_SUCESSO);
    assert(historia.cargas_arquivo == CARGAS_ARQUIVO_INICIAIS + 1);
    assert(historia.fragmentos == saldo_inicial - lojaPreco(ITEM_ARQUIVOS));

    saldo_inicial = historia.fragmentos;
    assert(lojaComprar(&historia, ITEM_REVERSAO) == COMPRA_SUCESSO);
    assert(historia.reversoes_temporais == REVERSOES_INICIAIS + 1);
    assert(historia.fragmentos == saldo_inicial - lojaPreco(ITEM_REVERSAO));

    assert(lojaComprar(&historia, -1) == COMPRA_ITEM_INVALIDO);
}

static void testarChefe(void) {
    Chefe chefe;
    int resultado;

    assert(TOTAL_QUESTOES_EXPLORACAO == 40);
    assert(TOTAL_QUESTOES_CHEFE == 10);
    assert(TOTAL_REGIOES_HISTORIA == 8);

    prepararChefe(&chefe);
    chefeEncerrarDerrota(&chefe);
    assert(!chefe.derrotado);
    assert(chefe.nucleos_restantes == TOTAL_NUCLEOS_CHEFE);

    prepararChefe(NULL);
    iniciarChefe(NULL);

    iniciarChefe(&chefe);
    assert(chefe.batalha_iniciada);
    assert(chefe.ativo);
    assert(chefe.nucleos_restantes == TOTAL_NUCLEOS_CHEFE);
    assert(chefe.fase == 1);

    /* A ordem das perguntas nao pode ser repetida ou alterada. */
    assert(chefeRegistrarResposta(&chefe, 41, 1) == RODADA_EM_ANDAMENTO);
    assert(chefe.respostas_na_rodada == 0);

    /* Rodada 1: um acerto e um erro destroem o primeiro núcleo. */
    resultado = chefeRegistrarResposta(&chefe, 40, 1);
    assert(resultado == RODADA_EM_ANDAMENTO);
    resultado = chefeRegistrarResposta(&chefe, 41, 0);
    assert(resultado == NUCLEO_DESTRUIDO);
    assert(chefe.nucleos_restantes == 4);

    /* Rodada 2: dois acertos destroem o núcleo e indicam bônus perfeito. */
    chefeRegistrarResposta(&chefe, 42, 1);
    resultado = chefeRegistrarResposta(&chefe, 43, 1);
    assert(resultado == NUCLEO_DESTRUIDO_PERFEITO);
    assert(chefe.nucleos_restantes == 3);

    /* A fase 2 começa na pergunta 45, entre rodadas completas. */
    assert(chefeFasePergunta(39) == 0);
    assert(chefeFasePergunta(40) == 1);
    assert(chefeFasePergunta(43) == 1);
    assert(chefeFasePergunta(44) == 2);
    assert(chefeFasePergunta(49) == 2);
    assert(chefeFasePergunta(50) == 0);

    /* Rodada 3: um acerto. */
    chefeRegistrarResposta(&chefe, 44, 1);
    assert(chefeRegistrarResposta(&chefe, 45, 0) == NUCLEO_DESTRUIDO);

    /* Rodada 4: dois acertos. */
    chefeRegistrarResposta(&chefe, 46, 1);
    assert(chefeRegistrarResposta(&chefe, 47, 1) == NUCLEO_DESTRUIDO_PERFEITO);

    /* Rodada 5: a última dupla deve acertar ao menos uma pergunta. */
    chefeRegistrarResposta(&chefe, 48, 0);
    assert(chefeRegistrarResposta(&chefe, 49, 1) == NUCLEO_DESTRUIDO);
    assert(chefe.nucleos_restantes == 0);

    chefeFinalizar(&chefe, 1);
    assert(chefe.venceu);
    assert(!chefe.ativo);

    /* Se um núcleo resistir, acertar a pergunta 50 não basta para vencer. */
    iniciarChefe(&chefe);
    chefeRegistrarResposta(&chefe, 40, 0);
    assert(chefeRegistrarResposta(&chefe, 41, 0) == NUCLEO_RESISTIU);
    for (int indice = 42; indice < 50; indice += 2) {
        chefeRegistrarResposta(&chefe, indice, 1);
        chefeRegistrarResposta(&chefe, indice + 1, 1);
    }
    assert(chefe.nucleos_restantes == 1);
    chefeFinalizar(&chefe, 1);
    assert(chefe.derrotado);
    assert(!chefe.venceu);
}

static void testarPortais(void) {
    Portal portais[TOTAL_PORTAIS];
    int visitados[TOTAL_REGIOES_HISTORIA];

    for (int regiao = 0; regiao < TOTAL_REGIOES_HISTORIA; regiao++) {
        gerarPortais(portais, visitados, regiao);

        /* Os três portais mostrados na mesma região devem ser diferentes. */
        assert(portais[0].id != portais[1].id);
        assert(portais[0].id != portais[2].id);
        assert(portais[1].id != portais[2].id);

        /* O destino escolhido não pode ter aparecido em região anterior. */
        for (int i = 0; i < regiao; i++) {
            for (int j = 0; j < TOTAL_PORTAIS; j++) {
                assert(portais[j].id != visitados[i]);
            }
        }

        visitados[regiao] = portais[regiao % TOTAL_PORTAIS].id;
    }
}

int main(void) {
    ListaPerguntas lista;

    srand(12345);
    iniciarLista(&lista);

    testarBanco(&lista);
    testarCSVInvalido();
    testarSorteio(&lista);
    testarReversao(&lista);
    testarRegeneracao(&lista);
    testarVersus();
    testarRecompensas();
    testarFragmentosComoMoeda();
    testarLoja();
    testarChefe();
    testarPortais();

    liberarLista(&lista);
    puts("Todos os testes do Timey Wimey passaram.");
    return 0;
}
