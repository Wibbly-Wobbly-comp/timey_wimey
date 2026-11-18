/**
 * @file pergunta.c
 * @brief Implementa o cadastro dinâmico e os arquivos de perguntas.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pergunta.h"

/**
 * @brief Aumenta a capacidade do vetor quando necessário.
 */
static int aumentarLista(ListaPerguntas *lista) {
    Pergunta *novo;
    int nova_capacidade;

    if (lista->quantidade < lista->capacidade) {
        return 1;
    }

    if (lista->capacidade == 0) {
        nova_capacidade = 10;
    } else {
        nova_capacidade = lista->capacidade * 2;
    }

    novo = realloc(lista->dados, nova_capacidade * sizeof(Pergunta));
    if (novo == NULL) {
        return 0;
    }

    lista->dados = novo;
    lista->capacidade = nova_capacidade;
    return 1;
}

/**
 * @brief Inicializa a lista de perguntas.
 */
void iniciarLista(ListaPerguntas *lista) {
    lista->dados = NULL;
    lista->quantidade = 0;
    lista->capacidade = 0;
    lista->proximo_id = 1;
}

/**
 * @brief Libera a memória utilizada pela lista.
 */
void liberarLista(ListaPerguntas *lista) {
    free(lista->dados);
    lista->dados = NULL;
    lista->quantidade = 0;
    lista->capacidade = 0;
    lista->proximo_id = 1;
}

/**
 * @brief Insere uma pergunta no vetor dinâmico.
 */
int adicionarPergunta(ListaPerguntas *lista, Pergunta pergunta) {
    if (!aumentarLista(lista)) {
        return 0;
    }

    if (pergunta.id <= 0) {
        pergunta.id = lista->proximo_id;
    }

    lista->dados[lista->quantidade] = pergunta;
    lista->quantidade++;

    if (pergunta.id >= lista->proximo_id) {
        lista->proximo_id = pergunta.id + 1;
    }

    return 1;
}

/**
 * @brief Procura uma pergunta pelo identificador.
 */
Pergunta *buscarPergunta(ListaPerguntas *lista, int id) {
    for (int i = 0; i < lista->quantidade; i++) {
        if (lista->dados[i].id == id) {
            return &lista->dados[i];
        }
    }

    return NULL;
}

/**
 * @brief Exclui uma pergunta e reorganiza os elementos do vetor.
 */
int excluirPergunta(ListaPerguntas *lista, int id) {
    int indice = -1;

    for (int i = 0; i < lista->quantidade; i++) {
        if (lista->dados[i].id == id) {
            indice = i;
            break;
        }
    }

    if (indice == -1) {
        return 0;
    }

    for (int i = indice; i < lista->quantidade - 1; i++) {
        lista->dados[i] = lista->dados[i + 1];
    }

    lista->quantidade--;

    /* Diminui a capacidade quando muitos elementos forem removidos. */
    if (lista->quantidade == 0) {
        free(lista->dados);
        lista->dados = NULL;
        lista->capacidade = 0;
    } else if (lista->capacidade > 10 && lista->quantidade <= lista->capacidade / 2) {
        int nova_capacidade = lista->capacidade / 2;
        Pergunta *novo = realloc(lista->dados, nova_capacidade * sizeof(Pergunta));
        if (novo != NULL) {
            lista->dados = novo;
            lista->capacidade = nova_capacidade;
        }
    }

    return 1;
}

/**
 * @brief Copia um campo do CSV para uma string com limite de tamanho.
 */
static void copiarCampo(char destino[], int tamanho, char origem[]) {
    snprintf(destino, tamanho, "%s", origem);
}

/**
 * @brief Lê o arquivo CSV e adiciona as perguntas na lista.
 * @return Quantidade de perguntas carregadas ou -1 em caso de erro.
 */
int carregarCSV(ListaPerguntas *lista, const char caminho[]) {
    FILE *arquivo = fopen(caminho, "r");
    char linha[1400];
    int carregadas = 0;

    if (arquivo == NULL) {
        return -1;
    }

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        char *campos[8];
        char *token;
        int quantidade_campos = 0;
        Pergunta pergunta;

        if (strncmp(linha, "id,", 3) == 0) {
            continue;
        }

        token = strtok(linha, ",\r\n");
        while (token != NULL && quantidade_campos < 8) {
            campos[quantidade_campos] = token;
            quantidade_campos++;
            token = strtok(NULL, ",\r\n");
        }

        if (quantidade_campos != 8) {
            continue;
        }

        pergunta.id = atoi(campos[0]);
        copiarCampo(pergunta.enunciado, TAM_ENUNCIADO, campos[1]);
        copiarCampo(pergunta.alternativas[0], TAM_ALTERNATIVA, campos[2]);
        copiarCampo(pergunta.alternativas[1], TAM_ALTERNATIVA, campos[3]);
        copiarCampo(pergunta.alternativas[2], TAM_ALTERNATIVA, campos[4]);
        copiarCampo(pergunta.alternativas[3], TAM_ALTERNATIVA, campos[5]);
        pergunta.resposta_correta = campos[6][0];
        pergunta.dificuldade = (enum Dificuldade)atoi(campos[7]);

        if (pergunta.id <= 0 || pergunta.resposta_correta < 'A' || pergunta.resposta_correta > 'D' ||
            pergunta.dificuldade < MUITO_FACIL || pergunta.dificuldade > MUITO_DIFICIL) {
            continue;
        }

        if (adicionarPergunta(lista, pergunta)) {
            carregadas++;
        }
    }

    fclose(arquivo);
    return carregadas;
}

/**
 * @brief Carrega perguntas salvas no arquivo binário.
 */
int carregarBinario(ListaPerguntas *lista, const char caminho[]) {
    FILE *arquivo = fopen(caminho, "rb");
    int quantidade;
    Pergunta *dados;

    if (arquivo == NULL) {
        return 0;
    }

    if (fread(&quantidade, sizeof(int), 1, arquivo) != 1 || quantidade < 0 || quantidade > 100000) {
        fclose(arquivo);
        return 0;
    }

    if (quantidade == 0) {
        fclose(arquivo);
        return 1;
    }

    dados = malloc(quantidade * sizeof(Pergunta));
    if (dados == NULL) {
        fclose(arquivo);
        return 0;
    }

    if (fread(dados, sizeof(Pergunta), quantidade, arquivo) != (size_t)quantidade) {
        free(dados);
        fclose(arquivo);
        return 0;
    }

    fclose(arquivo);
    lista->dados = dados;
    lista->quantidade = quantidade;
    lista->capacidade = quantidade;
    lista->proximo_id = 1;

    for (int i = 0; i < quantidade; i++) {
        if (dados[i].id >= lista->proximo_id) {
            lista->proximo_id = dados[i].id + 1;
        }
    }

    return 1;
}

/**
 * @brief Tenta carregar o binário; se ele não existir, utiliza o CSV inicial.
 */
int carregarPerguntas(ListaPerguntas *lista, const char arquivo_csv[], const char arquivo_bin[]) {
    if (carregarBinario(lista, arquivo_bin)) {
        return lista->quantidade;
    }

    liberarLista(lista);
    iniciarLista(lista);
    return carregarCSV(lista, arquivo_csv);
}

/**
 * @brief Salva toda a lista atual em um arquivo binário.
 */
int salvarBinario(ListaPerguntas *lista, const char caminho[]) {
    FILE *arquivo = fopen(caminho, "wb");

    if (arquivo == NULL) {
        return 0;
    }

    if (fwrite(&lista->quantidade, sizeof(int), 1, arquivo) != 1) {
        fclose(arquivo);
        return 0;
    }

    if (lista->quantidade > 0 &&
        fwrite(lista->dados, sizeof(Pergunta), lista->quantidade, arquivo) != (size_t)lista->quantidade) {
        fclose(arquivo);
        return 0;
    }

    fclose(arquivo);
    return 1;
}

/**
 * @brief Exporta as perguntas atuais para um arquivo CSV.
 */
int exportarCSV(ListaPerguntas *lista, const char caminho[]) {
    FILE *arquivo = fopen(caminho, "w");

    if (arquivo == NULL) {
        return 0;
    }

    fprintf(arquivo, "id,enunciado,alternativa_a,alternativa_b,alternativa_c,alternativa_d,resposta_correta,dificuldade\n");

    for (int i = 0; i < lista->quantidade; i++) {
        Pergunta pergunta = lista->dados[i];
        fprintf(arquivo, "%d,%s,%s,%s,%s,%s,%c,%d\n",
                pergunta.id,
                pergunta.enunciado,
                pergunta.alternativas[0],
                pergunta.alternativas[1],
                pergunta.alternativas[2],
                pergunta.alternativas[3],
                pergunta.resposta_correta,
                pergunta.dificuldade);
    }

    fclose(arquivo);
    return 1;
}

/**
 * @brief Conta quantas perguntas existem em determinado nível.
 */
int contarDificuldade(ListaPerguntas *lista, enum Dificuldade dificuldade) {
    int total = 0;

    for (int i = 0; i < lista->quantidade; i++) {
        if (lista->dados[i].dificuldade == dificuldade) {
            total++;
        }
    }

    return total;
}

/**
 * @brief Retorna o nome de um nível de dificuldade.
 */
const char *nomeDificuldade(enum Dificuldade dificuldade) {
    switch (dificuldade) {
        case MUITO_FACIL: return "Muito facil";
        case FACIL: return "Facil";
        case MEDIO: return "Medio";
        case DIFICIL: return "Dificil";
        case MUITO_DIFICIL: return "Muito dificil";
        default: return "Desconhecida";
    }
}

/**
 * @brief Exibe uma pergunta no terminal.
 */
void imprimirPergunta(Pergunta pergunta) {
    printf("ID: %d | Dificuldade: %s\n", pergunta.id, nomeDificuldade(pergunta.dificuldade));
    printf("%s\n", pergunta.enunciado);

    for (int i = 0; i < TOTAL_ALTERNATIVAS; i++) {
        printf("%c) %s\n", 'A' + i, pergunta.alternativas[i]);
    }

    printf("Resposta correta: %c\n", pergunta.resposta_correta);
}
