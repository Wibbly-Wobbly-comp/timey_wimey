/**
 * @file gerenciador.c
 * @brief Menu de terminal para cadastrar e administrar perguntas.
 * @version 1.0
 * @date 2026-11-24
 *
 * @author 
 * Giovani Lemos Proença (giovanilemosproenca@alunos.utfpr.edu.br)
 * Hadassah Olívia de Paiva (hadassah@alunos.utfpr.edu.br)
 * João Pedro Arruda (joaosilva.2007@alunos.utfpr.edu.br)
 * @copyright Copyright (c) 2026
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include "pergunta.h"

#define ARQUIVO_CSV "dados/perguntas_iniciais.csv"
#define ARQUIVO_BIN "dados/perguntas.bin"
#define ARQUIVO_EXPORTADO "dados/perguntas_exportadas.csv"

void limparEntrada(void);
int lerTexto(char texto[], int tamanho, const char mensagem[]);
int lerInteiro(const char mensagem[]);
int lerDadosPergunta(Pergunta *pergunta);
void cadastrarPergunta(ListaPerguntas *lista);
void listarPerguntas(ListaPerguntas *lista);
void pesquisarPergunta(ListaPerguntas *lista);
void alterarPergunta(ListaPerguntas *lista);
void removerPergunta(ListaPerguntas *lista);
void navegarDificuldade(ListaPerguntas *lista);

int main(void) {
    ListaPerguntas lista;
    int opcao;
    int carregadas;

    iniciarLista(&lista);
    carregadas = carregarPerguntas(&lista, ARQUIVO_CSV, ARQUIVO_BIN);

    if (carregadas < 0) {
        printf("Nao foi possivel abrir o arquivo de perguntas.\n");
        liberarLista(&lista);
        return 1;
    }

    printf("Perguntas carregadas: %d\n", lista.quantidade);

    do {
        printf("\n========== CADASTRO DE PERGUNTAS ==========\n");
        printf("1 - Inserir pergunta\n");
        printf("2 - Listar perguntas\n");
        printf("3 - Pesquisar pergunta\n");
        printf("4 - Alterar pergunta\n");
        printf("5 - Excluir pergunta\n");
        printf("6 - Navegar por dificuldade\n");
        printf("7 - Exportar para CSV\n");
        printf("0 - Salvar e sair\n");
        opcao = lerInteiro("Escolha uma opcao: ");

        switch (opcao) {
            case 1:
                cadastrarPergunta(&lista);
                break;
            case 2:
                listarPerguntas(&lista);
                break;
            case 3:
                pesquisarPergunta(&lista);
                break;
            case 4:
                alterarPergunta(&lista);
                break;
            case 5:
                removerPergunta(&lista);
                break;
            case 6:
                navegarDificuldade(&lista);
                break;
            case 7:
                if (exportarCSV(&lista, ARQUIVO_EXPORTADO)) {
                    printf("Arquivo exportado em %s\n", ARQUIVO_EXPORTADO);
                } else {
                    printf("Erro ao exportar o arquivo.\n");
                }
                break;
            case 0:
                if (salvarBinario(&lista, ARQUIVO_BIN)) {
                    printf("Dados salvos com sucesso.\n");
                } else {
                    printf("Erro ao salvar os dados.\n");
                }
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
        }
    } while (opcao != 0);

    liberarLista(&lista);
    return 0;
}

/**
 * @brief Descarta os caracteres restantes da linha digitada.
 */
void limparEntrada(void) {
    int caractere;
    while ((caractere = getchar()) != '\n' && caractere != EOF) {
    }
}

/**
 * @brief Lê uma linha de texto e remove o caractere de quebra de linha.
 */
int lerTexto(char texto[], int tamanho, const char mensagem[]) {
    printf("%s", mensagem);

    if (fgets(texto, tamanho, stdin) == NULL) {
        texto[0] = '\0';
        return 0;
    }

    if (strchr(texto, '\n') == NULL && !feof(stdin)) {
        limparEntrada();
        printf("Aviso: o texto foi limitado ao tamanho do campo.\n");
    }

    texto[strcspn(texto, "\n")] = '\0';
    return 1;
}

/**
 * @brief Lê um número inteiro digitado pelo usuário.
 */
int lerInteiro(const char mensagem[]) {
    char linha[60];
    char *fim;
    long valor;

    while (1) {
        if (!lerTexto(linha, sizeof(linha), mensagem)) return 0;
        errno = 0;
        valor = strtol(linha, &fim, 10);

        if (linha[0] != '\0' && fim != linha && *fim == '\0' &&
            errno != ERANGE && valor >= INT_MIN && valor <= INT_MAX) {
            return (int)valor;
        }

        printf("Digite um numero valido.\n");
    }
}

/**
 * @brief Lê os campos de uma pergunta.
 *
 * O CSV utiliza ponto e vírgula como separador e protege campos entre aspas.
 * Por isso, vírgulas e ponto e vírgulas podem aparecer nos textos.
 */
int lerDadosPergunta(Pergunta *pergunta) {
    char resposta[20];
    int dificuldade;

    /* O enunciado e as quatro alternativas nao podem ficar vazios. */
    do {
        if (!lerTexto(pergunta->enunciado, TAM_ENUNCIADO, "Enunciado: ")) return 0;
        if (pergunta->enunciado[0] == '\0') {
            printf("O enunciado nao pode ficar vazio.\n");
        }
    } while (pergunta->enunciado[0] == '\0');

    for (int i = 0; i < TOTAL_ALTERNATIVAS; i++) {
        do {
            char mensagem[50];
            snprintf(mensagem, sizeof(mensagem), "Alternativa %c: ", 'A' + i);
            if (!lerTexto(pergunta->alternativas[i], TAM_ALTERNATIVA, mensagem)) return 0;
            if (pergunta->alternativas[i][0] == '\0') {
                printf("A alternativa nao pode ficar vazia.\n");
            }
        } while (pergunta->alternativas[i][0] == '\0');
    }

    /* A resposta precisa ser uma unica letra entre A e D. */
    do {
        if (!lerTexto(resposta, sizeof(resposta), "Letra correta (A, B, C ou D): ")) return 0;
        if (strlen(resposta) > 0) resposta[0] = (char)toupper((unsigned char)resposta[0]);
        if (strlen(resposta) != 1 || resposta[0] < 'A' || resposta[0] > 'D') {
            printf("Digite apenas A, B, C ou D.\n");
        }
    } while (strlen(resposta) != 1 || resposta[0] < 'A' || resposta[0] > 'D');
    pergunta->resposta_correta = resposta[0];

    /* A dificuldade deve estar entre os cinco niveis disponiveis. */
    do {
        dificuldade = lerInteiro("Dificuldade (1 muito facil, 2 facil, 3 medio, 4 dificil, 5 muito dificil): ");
        if (dificuldade < MUITO_FACIL || dificuldade > MUITO_DIFICIL) {
            printf("Dificuldade invalida. Escolha um valor de 1 a 5.\n");
        }
        if (feof(stdin)) return 0;
    } while (dificuldade < MUITO_FACIL || dificuldade > MUITO_DIFICIL);
    pergunta->dificuldade = (enum Dificuldade)dificuldade;

    return 1;
}

/**
 * @brief Cadastra uma nova pergunta na lista dinâmica.
 */
void cadastrarPergunta(ListaPerguntas *lista) {
    Pergunta pergunta = {0};

    printf("\n--- NOVA PERGUNTA ---\n");
    if (!lerDadosPergunta(&pergunta)) {
        printf("Cadastro cancelado.\n");
        return;
    }

    if (adicionarPergunta(lista, pergunta)) {
        printf("Pergunta cadastrada com ID %d.\n", lista->dados[lista->quantidade - 1].id);
    } else {
        printf("Nao foi possivel cadastrar a pergunta. Verifique os dados ou a memoria disponivel.\n");
    }
}

/**
 * @brief Lista todas as perguntas cadastradas.
 */
void listarPerguntas(ListaPerguntas *lista) {
    if (lista->quantidade == 0) {
        printf("Nenhuma pergunta cadastrada.\n");
        return;
    }

    for (int i = 0; i < lista->quantidade; i++) {
        printf("\n--------------------------------------------\n");
        imprimirPergunta(lista->dados[i]);
    }
}

/**
 * @brief Pesquisa perguntas pelo ID ou por uma palavra do enunciado.
 */
void pesquisarPergunta(ListaPerguntas *lista) {
    int tipo;
    int id;
    char termo[TAM_ENUNCIADO];
    int encontrados = 0;

    printf("1 - Pesquisar por ID\n");
    printf("2 - Pesquisar por palavra do enunciado\n");
    tipo = lerInteiro("Tipo de pesquisa: ");

    if (tipo == 1) {
        id = lerInteiro("Digite o ID: ");
        Pergunta *pergunta = buscarPergunta(lista, id);
        if (pergunta != NULL) {
            imprimirPergunta(*pergunta);
        } else {
            printf("Pergunta nao encontrada.\n");
        }
    } else if (tipo == 2) {
        if (!lerTexto(termo, sizeof(termo), "Digite uma palavra: ")) return;
        if (termo[0] == '\0') {
            printf("Digite ao menos uma letra para pesquisar.\n");
            return;
        }
        for (int i = 0; i < lista->quantidade; i++) {
            if (strstr(lista->dados[i].enunciado, termo) != NULL) {
                printf("\n--------------------------------------------\n");
                imprimirPergunta(lista->dados[i]);
                encontrados++;
            }
        }
        if (encontrados == 0) {
            printf("Nenhuma pergunta encontrada com esse termo.\n");
        }
    } else {
        printf("Tipo de pesquisa invalido.\n");
    }
}

/**
 * @brief Altera os dados de uma pergunta existente.
 */
void alterarPergunta(ListaPerguntas *lista) {
    int id = lerInteiro("Digite o ID da pergunta que deseja alterar: ");
    Pergunta *pergunta = buscarPergunta(lista, id);

    if (pergunta == NULL) {
        printf("Pergunta nao encontrada.\n");
        return;
    }

    printf("Pergunta atual:\n");
    imprimirPergunta(*pergunta);
    printf("\nDigite os novos dados.\n");
    {
        Pergunta original = *pergunta;
        if (!lerDadosPergunta(pergunta)) {
            *pergunta = original;
            printf("Alteracao cancelada. Os dados anteriores foram mantidos.\n");
            return;
        }
    }
    printf("Pergunta alterada.\n");
}

/**
 * @brief Exclui uma pergunta pelo ID.
 */
void removerPergunta(ListaPerguntas *lista) {
    int id = lerInteiro("Digite o ID da pergunta que deseja excluir: ");

    if (excluirPergunta(lista, id)) {
        printf("Pergunta excluida.\n");
    } else {
        printf("Pergunta nao encontrada.\n");
    }
}

/**
 * @brief Navega por perguntas de um mesmo nível usando n, a e s.
 */
void navegarDificuldade(ListaPerguntas *lista) {
    int dificuldade;
    int indice = 0;
    int total;
    char comando[20];

    do {
        dificuldade = lerInteiro("Nivel (1 muito facil, 2 facil, 3 medio, 4 dificil, 5 muito dificil): ");
        if (feof(stdin)) return;
    } while (dificuldade < MUITO_FACIL || dificuldade > MUITO_DIFICIL);

    total = contarDificuldade(lista, (enum Dificuldade)dificuldade);
    if (total == 0) {
        printf("Nao existem perguntas nesse nivel.\n");
        return;
    }

    while (1) {
        int encontrados = 0;
        printf("\nPergunta %d de %d - %s\n", indice + 1, total, nomeDificuldade((enum Dificuldade)dificuldade));

        for (int i = 0; i < lista->quantidade; i++) {
            if (lista->dados[i].dificuldade == (enum Dificuldade)dificuldade) {
                if (encontrados == indice) {
                    imprimirPergunta(lista->dados[i]);
                    break;
                }
                encontrados++;
            }
        }

        if (!lerTexto(comando, sizeof(comando), "n - proxima | a - anterior | s - sair: ")) {
            break;
        }
        if (comando[0] == 'n' && indice < total - 1) {
            indice++;
        } else if (comando[0] == 'a' && indice > 0) {
            indice--;
        } else if (comando[0] == 's') {
            break;
        } else {
            printf("Comando invalido ou limite da lista alcancado.\n");
        }
    }
}
