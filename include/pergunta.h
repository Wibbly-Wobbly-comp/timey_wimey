/**
 * @file pergunta.h
 * @brief Estruturas e funções para o cadastro de perguntas.
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

#ifndef PERGUNTA_H
#define PERGUNTA_H

#define TAM_ENUNCIADO 260
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
    int id; /* Identificador único utilizado na busca e na alteração. */
    char enunciado[TAM_ENUNCIADO]; /* Texto apresentado ao jogador. */
    char alternativas[TOTAL_ALTERNATIVAS][TAM_ALTERNATIVA]; /* Opções A, B, C e D. */
    char resposta_correta; /* Letra da resposta correta: A, B, C ou D. */
    enum Dificuldade dificuldade; /* Nível de dificuldade de 1 a 5. */
} Pergunta;

typedef struct {
    Pergunta *dados; /* Endereço do vetor alocado dinamicamente. */
    int quantidade; /* Quantas perguntas estão cadastradas no momento. */
    int capacidade; /* Quantas perguntas cabem no espaço já reservado. */
    int proximo_id; /* Próximo identificador que será atribuído. */
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
