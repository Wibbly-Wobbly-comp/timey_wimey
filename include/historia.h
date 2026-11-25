/**
 * @file historia.h
 * @brief Estrutura e funções de progressão do Modo História.
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

#ifndef HISTORIA_H
#define HISTORIA_H

#include "jogo.h"
#include "recompensa.h"

#define VIDAS_INICIAIS 3
#define VIDAS_MAXIMAS 5
#define CARGAS_SONICA_INICIAIS 2
#define CARGAS_ARQUIVO_INICIAIS 1
#define CARGAS_SONICA_MAXIMAS 5
#define CARGAS_ARQUIVO_MAXIMAS 3
#define REVERSOES_INICIAIS 1
#define REVERSOES_MAXIMAS 2

typedef struct {
    int vidas; /* Quantidade atual de vidas do jogador. */
    int vidas_maximas; /* Limite de vidas que podem ser acumuladas. */
    int fragmentos; /* Moeda temporaria usada para comprar power-ups na expedicao. */
    int regioes_concluidas; /* Quantidade de regiões completadas. */

    int cargas_sonica; /* Usos disponíveis da Chave de Fenda Sônica. */
    int cargas_arquivo; /* Usos disponíveis dos Arquivos da TARDIS. */
    int reversoes_temporais; /* Quantidade de reversões de tempo disponíveis. */

    int regeneracao_disponivel; /* Indica se o desafio de regeneração ainda pode ser usado. */
    int regeneracao_usada; /* Indica se o jogador já iniciou o desafio de regeneração. */
    int regeneracao_em_andamento; /* Indica que o jogador está respondendo ao desafio. */
    int indice_pergunta_regeneracao; /* Índice da pergunta muito difícil do desafio. */
    int game_over; /* Indica que a expedição foi encerrada por uma derrota. */

    int usou_sonica_pergunta; /* Impede usar a sônica várias vezes na mesma pergunta. */
    int usou_arquivo_pergunta; /* Impede usar o arquivo várias vezes na mesma pergunta. */
    int alternativas_visiveis[TOTAL_ALTERNATIVAS]; /* Alternativas removidas pela sônica. */
    int votos_arquivo[TOTAL_ALTERNATIVAS]; /* Votação simulada do arquivo da TARDIS. */
    int mostrar_votos; /* Controla a exibição da votação. */
    char mensagem_recompensa[120]; /* Última recompensa recebida ao concluir uma região. */
} Historia;

/** Prepara vidas, cargas de dicas e progresso inicial da expedição. */
void iniciarHistoria(Historia *historia);

/** Reinicia os efeitos temporários quando uma nova pergunta começa. */
void historiaNovaPergunta(Historia *historia);

/** Usa a Chave de Fenda Sônica para eliminar uma alternativa incorreta. */
int historiaUsarSonic(Historia *historia, Pergunta pergunta);

/** Usa os Arquivos da TARDIS para apresentar uma votação simulada. */
int historiaUsarArquivo(Historia *historia, Pergunta pergunta);

/** Troca a pergunta errada por outra pergunta ainda não respondida do mesmo nível. */
int historiaUsarReversao(Historia *historia, Jogo *jogo, ListaPerguntas *lista);

/** Sorteia a pergunta muito difícil do desafio de regeneração. */
int historiaIniciarRegeneracao(Historia *historia, Jogo *jogo, ListaPerguntas *lista);

/** Concede o bônus de conclusão de uma região, uma única vez. */
void historiaConcluirRegiao(Historia *historia, int indice_regiao);

/** Aplica o tipo e a quantidade de recompensa do portal concluído. */
void historiaRecompensaPortal(Historia *historia, enum TipoRecompensaPortal tipo, int valor);

/** Processa uma resposta normal ou o desafio especial de regeneração. */
void historiaProcessarResposta(Historia *historia, Jogo *jogo, Pergunta pergunta);

/** Retorna a quantidade de fragmentos recebidos por uma resposta correta. */
int fragmentosPergunta(enum Dificuldade dificuldade);

/** Acrescenta fragmentos extras à carteira do jogador. */
void historiaConcederFragmentos(Historia *historia, int quantidade);

#endif
