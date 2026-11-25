/**
 * @file pergunta.c
 * @brief Implementa o cadastro dinâmico e os arquivos de perguntas.
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
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

#ifdef _WIN32
    /* MoveFileEx permite substituir o banco existente no Windows. */
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <windows.h>
#endif

#include "pergunta.h"

/* Valida os campos antes de inserir, importar ou salvar uma pergunta. */
static int perguntaValida(const Pergunta *pergunta);

/**
 * @brief Aumenta a capacidade do vetor quando necessário.
 * @return 1 quando a memoria foi alocada; 0 em caso de erro.
 */
static int aumentarLista(ListaPerguntas *lista) {
    Pergunta *novo;
    int nova_capacidade;

    if (lista == NULL) return 0;

    if (lista->quantidade < lista->capacidade) {
        return 1;
    }

    if (lista->capacidade == 0) {
        nova_capacidade = 10;
    } else {
        if (lista->capacidade > INT_MAX / 2) return 0;
        nova_capacidade = lista->capacidade * 2;
    }

    if ((size_t)nova_capacidade > SIZE_MAX / sizeof(Pergunta)) {
        return 0;
    }

    novo = realloc(lista->dados, (size_t)nova_capacidade * sizeof(Pergunta));
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
    if (lista == NULL) return;

    lista->dados = NULL;
    lista->quantidade = 0;
    lista->capacidade = 0;
    lista->proximo_id = 1;
}

/**
 * @brief Libera a memória utilizada pela lista.
 */
void liberarLista(ListaPerguntas *lista) {
    if (lista == NULL) return;

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
    if (lista == NULL || !perguntaValida(&pergunta) ||
        lista->quantidade < 0 || lista->quantidade > lista->capacidade) {
        return 0;
    }

    /* IDs precisam ser únicos para que busca, alteração e exclusão funcionem. */
    if (pergunta.id <= 0) {
        if (lista->proximo_id <= 0) return 0;
        pergunta.id = lista->proximo_id;
    } else if (buscarPergunta(lista, pergunta.id) != NULL) {
        return 0;
    }

    if (!aumentarLista(lista)) {
        return 0;
    }

    lista->dados[lista->quantidade] = pergunta;
    lista->quantidade++;

    /* Atualiza o próximo ID somente depois que a pergunta foi inserida. */
    if (pergunta.id == INT_MAX) {
        lista->proximo_id = 0;
    } else if (pergunta.id >= lista->proximo_id) {
        lista->proximo_id = pergunta.id + 1;
    }

    return 1;
}

/**
 * @brief Procura uma pergunta pelo identificador.
 */
Pergunta *buscarPergunta(ListaPerguntas *lista, int id) {
    if (lista == NULL || lista->dados == NULL || id <= 0) return NULL;

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

    if (lista == NULL || lista->dados == NULL || id <= 0) return 0;

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
        Pergunta *novo = realloc(lista->dados, (size_t)nova_capacidade * sizeof(Pergunta));
        if (novo != NULL) {
            lista->dados = novo;
            lista->capacidade = nova_capacidade;
        }
    }

    return 1;
}

/**
 * @brief Remove espaços e quebras de linha das extremidades de um texto.
 */
static char *removerEspacos(char *texto) {
    char *fim;

    while (*texto && isspace((unsigned char)*texto)) texto++;
    fim = texto + strlen(texto);

    while (fim > texto && isspace((unsigned char)fim[-1])) {
        fim--;
    }

    *fim = '\0';
    return texto;
}

/**
 * @brief Separa uma linha CSV em oito campos, respeitando aspas.
 *
 * O separador do projeto é ponto e vírgula. Um campo entre aspas pode conter
 * ponto e vírgula e aspas duplicadas representam uma aspa literal.
 */
static int separarLinhaCSV(const char linha[], char campos[8][300]) {
    const char *cursor = linha;
    int campo = 0;

    memset(campos, 0, 8 * 300);

    while (*cursor && campo < 8) {
        int entre_aspas = 0;
        int iniciou_aspas = 0;
        int tamanho = 0;

        if (*cursor == '"') {
            entre_aspas = 1;
            iniciou_aspas = 1;
            cursor++;
        }

        while (*cursor) {
            if (entre_aspas) {
                if (*cursor == '"') {
                    if (cursor[1] == '"') {
                        if (tamanho >= 299) return 0;
                        campos[campo][tamanho++] = '"';
                        cursor += 2;
                    } else {
                        entre_aspas = 0;
                        cursor++;
                        break;
                    }
                } else {
                    if (tamanho >= 299) return 0;
                    campos[campo][tamanho++] = *cursor;
                    cursor++;
                }
            } else {
                if (*cursor == ';' || *cursor == '\n' || *cursor == '\r') break;
                if (*cursor == '"' && iniciou_aspas) return 0;
                if (tamanho >= 299) return 0;
                campos[campo][tamanho++] = *cursor;
                cursor++;
            }
        }

        if (entre_aspas) return 0;
        campos[campo][tamanho] = '\0';

        if (!iniciou_aspas) {
            char *limpo = removerEspacos(campos[campo]);
            if (limpo != campos[campo]) {
                memmove(campos[campo], limpo, strlen(limpo) + 1);
            }
        }

        campo++;

        if (*cursor == ';') {
            cursor++;
        } else {
            break;
        }
    }

    while (*cursor == '\n' || *cursor == '\r' || *cursor == ' ' || *cursor == '\t') {
        cursor++;
    }

    return campo == 8 && *cursor == '\0';
}

/**
 * @brief Copia um campo sem cortar o texto silenciosamente.
 * @return 1 quando o campo cabe no destino; 0 caso contrário.
 */
static int copiarCampo(char destino[], size_t tamanho, const char origem[]) {
    size_t comprimento = strlen(origem);

    if (comprimento >= tamanho) return 0;
    memcpy(destino, origem, comprimento + 1);
    return 1;
}

/**
 * @brief Confere os campos essenciais de uma pergunta antes de salvá-la.
 */
static int textoValido(const char texto[], size_t tamanho) {
    size_t i;
    int tem_conteudo = 0;

    for (i = 0; i < tamanho; i++) {
        if (texto[i] == '\0') break;
        if (texto[i] == '\n' || texto[i] == '\r') return 0;
        if (!isspace((unsigned char)texto[i])) tem_conteudo = 1;
    }

    return i < tamanho && tem_conteudo;
}

static int perguntaValida(const Pergunta *pergunta) {
    if (pergunta == NULL) return 0;

    if (!textoValido(pergunta->enunciado, sizeof(pergunta->enunciado))) {
        return 0;
    }

    for (int i = 0; i < TOTAL_ALTERNATIVAS; i++) {
        if (!textoValido(pergunta->alternativas[i], sizeof(pergunta->alternativas[i]))) {
            return 0;
        }
    }

    return pergunta->resposta_correta >= 'A' && pergunta->resposta_correta <= 'D' &&
           pergunta->dificuldade >= MUITO_FACIL && pergunta->dificuldade <= MUITO_DIFICIL;
}

/**
 * @brief Converte um campo numerico sem aceitar caracteres extras.
 */
static int converterInteiro(const char texto[], int *valor) {
    char *fim;
    long numero;

    if (texto == NULL || valor == NULL || texto[0] == '\0') return 0;

    errno = 0;
    numero = strtol(texto, &fim, 10);

    if (errno == ERANGE || fim == texto || *fim != '\0' ||
        numero < INT_MIN || numero > INT_MAX) {
        return 0;
    }

    *valor = (int)numero;
    return 1;
}

/** Cabeçalho usado para identificar e validar o arquivo binário. */
typedef struct {
    char identificador[8];
    uint32_t versao;
    uint32_t tamanho_registro;
    uint32_t quantidade;
} CabecalhoPerguntas;

#define IDENTIFICADOR_BINARIO "TWPREG01"
#define VERSAO_BINARIO 4U
#define LIMITE_PERGUNTAS_BINARIO 100000U

/**
 * @brief Substitui o arquivo de destino pelo arquivo temporario.
 *
 * No Windows, rename() nao substitui de forma confiavel um arquivo que ja
 * existe. MoveFileExA resolve esse caso sem apagar primeiro o banco valido.
 * No Linux e em outros sistemas POSIX, rename() ja realiza essa substituicao.
 */
static int substituirArquivo(const char origem[], const char destino[]) {
#ifdef _WIN32
    return MoveFileExA(origem, destino,
                       MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
#else
    return rename(origem, destino) == 0;
#endif
}

/**
 * @brief Lê o CSV inicial, aceitando campos entre aspas.
 * @return Quantidade carregada ou -1 quando o arquivo não pode ser aberto.
 */
int carregarCSV(ListaPerguntas *lista, const char caminho[]) {
    FILE *arquivo;
    char linha[2048];
    int carregadas = 0;
    int primeira_linha = 1;

    if (lista == NULL || caminho == NULL) return -1;

    arquivo = fopen(caminho, "r");
    if (arquivo == NULL) return -1;

    liberarLista(lista);
    iniciarLista(lista);

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        char campos[8][300];
        Pergunta pergunta = {0};
        size_t tamanho = strlen(linha);
        int id;
        int dificuldade;

        /* Remove o BOM UTF-8, caso o arquivo tenha sido salvo pelo Excel. */
        if (primeira_linha) {
            primeira_linha = 0;
            if ((unsigned char)linha[0] == 0xEF &&
                (unsigned char)linha[1] == 0xBB &&
                (unsigned char)linha[2] == 0xBF) {
                memmove(linha, linha + 3, tamanho - 2);
                tamanho -= 3;
            }
        }

        /* Descarta linhas maiores que o buffer para nao misturar registros. */
        if (tamanho == sizeof(linha) - 1 && linha[tamanho - 1] != '\n') {
            int caractere;

            while ((caractere = fgetc(arquivo)) != '\n' && caractere != EOF) {
            }

            /* Se houve erro ou fim do arquivo, nao tenta ler outra linha. */
            if (caractere == EOF) {
                if (ferror(arquivo)) {
                    fclose(arquivo);
                    return -1;
                }
                break;
            }
            continue;
        }

        if (strncmp(linha, "id;", 3) == 0 || linha[0] == '\n' || linha[0] == '\r') {
            continue;
        }

        if (!separarLinhaCSV(linha, campos)) continue;
        if (!converterInteiro(campos[0], &id) ||
            !converterInteiro(campos[7], &dificuldade)) {
            continue;
        }

        pergunta.id = id;
        if (!copiarCampo(pergunta.enunciado, sizeof(pergunta.enunciado), campos[1]) ||
            !copiarCampo(pergunta.alternativas[0], sizeof(pergunta.alternativas[0]), campos[2]) ||
            !copiarCampo(pergunta.alternativas[1], sizeof(pergunta.alternativas[1]), campos[3]) ||
            !copiarCampo(pergunta.alternativas[2], sizeof(pergunta.alternativas[2]), campos[4]) ||
            !copiarCampo(pergunta.alternativas[3], sizeof(pergunta.alternativas[3]), campos[5])) {
            continue;
        }

        if (strlen(campos[6]) != 1) continue;
        pergunta.resposta_correta = (char)toupper((unsigned char)campos[6][0]);
        pergunta.dificuldade = (enum Dificuldade)dificuldade;

        if (pergunta.id <= 0 || !perguntaValida(&pergunta) ||
            buscarPergunta(lista, pergunta.id) != NULL) {
            continue;
        }

        if (adicionarPergunta(lista, pergunta)) {
            carregadas++;
        }
    }

    if (ferror(arquivo)) {
        fclose(arquivo);
        return -1;
    }

    fclose(arquivo);
    return carregadas;
}

/**
 * @brief Carrega um banco binário após validar assinatura, versão e tamanho.
 */
int carregarBinario(ListaPerguntas *lista, const char caminho[]) {
    FILE *arquivo;
    CabecalhoPerguntas cabecalho;
    Pergunta *dados = NULL;

    if (lista == NULL || caminho == NULL) return 0;

    arquivo = fopen(caminho, "rb");
    if (arquivo == NULL) return 0;

    if (fread(&cabecalho, sizeof(cabecalho), 1, arquivo) != 1 ||
        memcmp(cabecalho.identificador, IDENTIFICADOR_BINARIO, 8) != 0 ||
        cabecalho.versao != VERSAO_BINARIO ||
        cabecalho.tamanho_registro != sizeof(Pergunta) ||
        cabecalho.quantidade > LIMITE_PERGUNTAS_BINARIO ||
        cabecalho.quantidade > (uint32_t)INT_MAX) {
        fclose(arquivo);
        return 0;
    }

    if (cabecalho.quantidade > 0) {
        dados = malloc((size_t)cabecalho.quantidade * sizeof(Pergunta));
        if (dados == NULL ||
            fread(dados, sizeof(Pergunta), cabecalho.quantidade, arquivo) != cabecalho.quantidade) {
            free(dados);
            fclose(arquivo);
            return 0;
        }

        for (uint32_t i = 0; i < cabecalho.quantidade; i++) {
            if (dados[i].id <= 0 || !perguntaValida(&dados[i])) {
                free(dados);
                fclose(arquivo);
                return 0;
            }

            /* O banco binario tambem precisa manter IDs unicos. */
            for (uint32_t j = 0; j < i; j++) {
                if (dados[i].id == dados[j].id) {
                    free(dados);
                    fclose(arquivo);
                    return 0;
                }
            }
        }
    }

    /* Dados extras indicam um arquivo fora do formato esperado. */
    if (fgetc(arquivo) != EOF || ferror(arquivo)) {
        free(dados);
        fclose(arquivo);
        return 0;
    }

    fclose(arquivo);
    liberarLista(lista);
    lista->dados = dados;
    lista->quantidade = (int)cabecalho.quantidade;
    lista->capacidade = (int)cabecalho.quantidade;
    lista->proximo_id = 1;

    for (int i = 0; i < lista->quantidade; i++) {
        if (lista->dados[i].id == INT_MAX) {
            lista->proximo_id = 0;
        } else if (lista->proximo_id > 0 && lista->dados[i].id >= lista->proximo_id) {
            lista->proximo_id = lista->dados[i].id + 1;
        }
    }

    return 1;
}

/**
 * @brief Tenta carregar o binário; se falhar, reconstrói o banco pelo CSV.
 */
int carregarPerguntas(ListaPerguntas *lista, const char arquivo_csv[], const char arquivo_bin[]) {
    if (lista == NULL || arquivo_csv == NULL || arquivo_bin == NULL) return -1;

    if (carregarBinario(lista, arquivo_bin)) {
        return lista->quantidade;
    }

    liberarLista(lista);
    iniciarLista(lista);
    return carregarCSV(lista, arquivo_csv);
}

/**
 * @brief Salva o banco em um arquivo temporário antes de substituir o binário.
 *
 * A assinatura, a versão e o tamanho da estrutura permitem detectar arquivos
 * criados por versões incompatíveis do programa. O arquivo temporário evita
 * substituir o banco válido por uma gravação incompleta.
 */
int salvarBinario(ListaPerguntas *lista, const char caminho[]) {
    FILE *arquivo;
    CabecalhoPerguntas cabecalho = {{0}, VERSAO_BINARIO, sizeof(Pergunta), 0};
    char temporario[512];
    int sucesso = 1;

    if (lista == NULL || caminho == NULL || lista->quantidade < 0 ||
        lista->quantidade > lista->capacidade ||
        lista->quantidade > (int)LIMITE_PERGUNTAS_BINARIO ||
        (lista->quantidade > 0 && lista->dados == NULL)) return 0;

    for (int i = 0; i < lista->quantidade; i++) {
        if (lista->dados[i].id <= 0 || !perguntaValida(&lista->dados[i])) return 0;
        for (int j = 0; j < i; j++) {
            if (lista->dados[i].id == lista->dados[j].id) return 0;
        }
    }

    cabecalho.quantidade = (uint32_t)lista->quantidade;
    memcpy(cabecalho.identificador, IDENTIFICADOR_BINARIO, 8);

    if (snprintf(temporario, sizeof(temporario), "%s.tmp", caminho) >= (int)sizeof(temporario)) {
        return 0;
    }

    arquivo = fopen(temporario, "wb");
    if (arquivo == NULL) return 0;

    if (fwrite(&cabecalho, sizeof(cabecalho), 1, arquivo) != 1) sucesso = 0;
    if (sucesso && lista->quantidade > 0 &&
        fwrite(lista->dados, sizeof(Pergunta), (size_t)lista->quantidade, arquivo) != (size_t)lista->quantidade) {
        sucesso = 0;
    }

    if (fclose(arquivo) != 0) sucesso = 0;

    if (!sucesso || !substituirArquivo(temporario, caminho)) {
        remove(temporario);
        return 0;
    }

    return 1;
}

/**
 * @brief Escreve um campo CSV e protege separadores, aspas e quebras de linha.
 */
static int escreverCampoCSV(FILE *arquivo, const char texto[]) {
    int usar_aspas = 0;

    for (const char *p = texto; *p; p++) {
        if (*p == ';' || *p == '"' || *p == '\n' || *p == '\r') {
            usar_aspas = 1;
            break;
        }
    }

    if (usar_aspas && fputc('"', arquivo) == EOF) return 0;

    for (const char *p = texto; *p; p++) {
        if (*p == '"' && fputc('"', arquivo) == EOF) return 0;
        if (fputc(*p, arquivo) == EOF) return 0;
    }

    if (usar_aspas && fputc('"', arquivo) == EOF) return 0;
    return 1;
}

/**
 * @brief Exporta o banco atual para CSV sem perder separadores dentro dos textos.
 */
int exportarCSV(ListaPerguntas *lista, const char caminho[]) {
    FILE *arquivo;

    if (!lista || !caminho || !(arquivo = fopen(caminho, "w"))) return 0;

    if (fprintf(arquivo, "id;enunciado;alternativa_a;alternativa_b;alternativa_c;alternativa_d;resposta_correta;dificuldade\n") < 0) {
        fclose(arquivo);
        return 0;
    }

    for (int i = 0; i < lista->quantidade; i++) {
        Pergunta pergunta = lista->dados[i];
        const char *campos[6] = {
            pergunta.enunciado,
            pergunta.alternativas[0],
            pergunta.alternativas[1],
            pergunta.alternativas[2],
            pergunta.alternativas[3],
            NULL
        };

        if (fprintf(arquivo, "%d;", pergunta.id) < 0) {
            fclose(arquivo);
            return 0;
        }

        for (int j = 0; j < 5; j++) {
            if (!escreverCampoCSV(arquivo, campos[j]) || fputc(';', arquivo) == EOF) {
                fclose(arquivo);
                return 0;
            }
        }

        if (fprintf(arquivo, "%c;%d\n", pergunta.resposta_correta, pergunta.dificuldade) < 0) {
            fclose(arquivo);
            return 0;
        }
    }

    return fclose(arquivo) == 0;
}

/**
 * @brief Conta quantas perguntas existem em determinado nível.
 */
int contarDificuldade(ListaPerguntas *lista, enum Dificuldade dificuldade) {
    int total = 0;

    if (lista == NULL || lista->dados == NULL) return 0;

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
