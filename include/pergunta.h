/**
 * @file pergunta.h
 * @brief Estruturas e funções para o cadastro de perguntas.
 */

#ifndef PERGUNTA_H
#define PERGUNTA_H

#define TAM_ENUNCIADO 240
#define TAM_ALTERNATIVA 120
#define TOTAL_ALTERNATIVAS 4

enum Dificuldade {
    MUITO_FACIL = 1,
    FACIL,
    MEDIO,
    DIFICIL,
    MUITO_DIFICIL
};

typedef struct {
    int id;
    char enunciado[TAM_ENUNCIADO];
    char alternativas[TOTAL_ALTERNATIVAS][TAM_ALTERNATIVA];
    char resposta_correta;
    enum Dificuldade dificuldade;
} Pergunta;

typedef struct {
    Pergunta *dados;
    int quantidade;
    int capacidade;
    int proximo_id;
} ListaPerguntas;

void iniciarLista(ListaPerguntas *lista);
void liberarLista(ListaPerguntas *lista);
int adicionarPergunta(ListaPerguntas *lista, Pergunta pergunta);
Pergunta *buscarPergunta(ListaPerguntas *lista, int id);
int excluirPergunta(ListaPerguntas *lista, int id);
int carregarCSV(ListaPerguntas *lista, const char caminho[]);
int carregarBinario(ListaPerguntas *lista, const char caminho[]);
int carregarPerguntas(ListaPerguntas *lista, const char arquivo_csv[], const char arquivo_bin[]);
int salvarBinario(ListaPerguntas *lista, const char caminho[]);
int exportarCSV(ListaPerguntas *lista, const char caminho[]);
int contarDificuldade(ListaPerguntas *lista, enum Dificuldade dificuldade);
const char *nomeDificuldade(enum Dificuldade dificuldade);
void imprimirPergunta(Pergunta pergunta);

#endif
