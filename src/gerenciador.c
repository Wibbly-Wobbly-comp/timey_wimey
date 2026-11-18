/**
 * @file gerenciador.c
 * @brief Menu de terminal para cadastrar e administrar perguntas.
 * @version 0.1
 * @date 2026-11-17
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "pergunta.h"

#define ARQUIVO_CSV "dados/perguntas_iniciais.csv"
#define ARQUIVO_BIN "dados/perguntas.bin"
#define ARQUIVO_EXPORTADO "dados/perguntas_exportadas.csv"

void limparEntrada();
void lerTexto(char texto[], int tamanho, char mensagem[]);
int lerInteiro(char mensagem[]);
void lerDadosPergunta(Pergunta *pergunta);
void cadastrarPergunta(ListaPerguntas *lista);
void listarPerguntas(ListaPerguntas *lista);
void pesquisarPergunta(ListaPerguntas *lista);
void alterarPergunta(ListaPerguntas *lista);
void removerPergunta(ListaPerguntas *lista);
void navegarDificuldade(ListaPerguntas *lista);

int main() {
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
void limparEntrada() {
    int caractere;
    while ((caractere = getchar()) != '\n' && caractere != EOF) {
    }
}

/**
 * @brief Lê uma linha de texto e remove o caractere de quebra de linha.
 */
void lerTexto(char texto[], int tamanho, char mensagem[]) {
    printf("%s", mensagem);
    if (fgets(texto, tamanho, stdin) != NULL) {
        if (strchr(texto, '\n') == NULL) {
            limparEntrada();
        }
        texto[strcspn(texto, "\n")] = '\0';
    } else {
        texto[0] = '\0';
    }
}

/**
 * @brief Lê um número inteiro digitado pelo usuário.
 */
int lerInteiro(char mensagem[]) {
    char linha[60];
    char *fim;
    long valor;

    while (1) {
        lerTexto(linha, sizeof(linha), mensagem);
        valor = strtol(linha, &fim, 10);

        if (linha[0] != '\0' && *fim == '\0') {
            return (int)valor;
        }

        printf("Digite um numero valido.\n");
    }
}

/**
 * @brief Lê os campos de uma pergunta. Não aceita vírgulas para manter o CSV simples.
 */
void lerDadosPergunta(Pergunta *pergunta) {
    char resposta[20];
    int dificuldade;

    do {
        lerTexto(pergunta->enunciado, TAM_ENUNCIADO, "Enunciado: ");
        if (strchr(pergunta->enunciado, ',') != NULL) {
            printf("Nao use virgulas no enunciado neste modelo simples de CSV.\n");
        }
    } while (pergunta->enunciado[0] == '\0' || strchr(pergunta->enunciado, ',') != NULL);

    for (int i = 0; i < TOTAL_ALTERNATIVAS; i++) {
        do {
            char mensagem[50];
            snprintf(mensagem, sizeof(mensagem), "Alternativa %c: ", 'A' + i);
            lerTexto(pergunta->alternativas[i], TAM_ALTERNATIVA, mensagem);
            if (strchr(pergunta->alternativas[i], ',') != NULL) {
                printf("Nao use virgulas nas alternativas.\n");
            }
        } while (pergunta->alternativas[i][0] == '\0' || strchr(pergunta->alternativas[i], ',') != NULL);
    }

    do {
        lerTexto(resposta, sizeof(resposta), "Letra correta (A, B, C ou D): ");
        if (strlen(resposta) > 0) resposta[0] = (char)toupper((unsigned char)resposta[0]);
    } while (strlen(resposta) != 1 || resposta[0] < 'A' || resposta[0] > 'D');
    pergunta->resposta_correta = resposta[0];

    do {
        dificuldade = lerInteiro("Dificuldade (1 muito facil, 2 facil, 3 medio, 4 dificil, 5 muito dificil): ");
    } while (dificuldade < MUITO_FACIL || dificuldade > MUITO_DIFICIL);
    pergunta->dificuldade = (enum Dificuldade)dificuldade;
}

/**
 * @brief Cadastra uma nova pergunta na lista dinâmica.
 */
void cadastrarPergunta(ListaPerguntas *lista) {
    Pergunta pergunta = {0};

    printf("\n--- NOVA PERGUNTA ---\n");
    lerDadosPergunta(&pergunta);

    if (adicionarPergunta(lista, pergunta)) {
        printf("Pergunta cadastrada com ID %d.\n", lista->dados[lista->quantidade - 1].id);
    } else {
        printf("Nao foi possivel alocar memoria para a pergunta.\n");
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
        lerTexto(termo, sizeof(termo), "Digite uma palavra: ");
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
    lerDadosPergunta(pergunta);
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

        lerTexto(comando, sizeof(comando), "n - proxima | a - anterior | s - sair: ");
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
