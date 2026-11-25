# Timey Wimey

Jogo de perguntas e respostas com viagens no tempo, desenvolvido em **C (C11)** com a **Raylib**. Projeto da equipe **Wibbly Wobbly**.

O jogador atravessa portais temporais, responde perguntas de vários níveis de dificuldade e usa dicas inspiradas no universo de viagens no tempo para sobreviver até a batalha final contra **O Paradoxo**.

- **Modo História:** 50 perguntas em 8 regiões, loja de power-ups (Loja da TARDIS) e chefe com duas fases.
- **Modo Versus:** disputa de perguntas com dicas 50/50 e Plateia.
- **Banco de perguntas:** 1.000 perguntas em 5 níveis, editáveis pelo gerenciador de terminal.
- **Consulta e exportação:** navegue pelas perguntas por dificuldade e exporte o banco para CSV.

## Sumário

1. [Início rápido](#1-início-rápido)
2. [Instalação no Linux](#2-instalação-no-linux)
3. [Instalação no Windows](#3-instalação-no-windows)
4. [Comandos do jogo](#4-comandos-do-jogo)
5. [Cadastro de perguntas](#5-cadastro-de-perguntas)
6. [Banco de perguntas](#6-banco-de-perguntas)
7. [Estrutura do projeto](#7-estrutura-do-projeto)
8. [Problemas comuns](#8-problemas-comuns)
9. [Créditos e observações](#9-créditos-e-observações)

## 1. Início rápido

Com GCC, Make e Raylib instalados (veja as seções 2 e 3), dentro da pasta `Timey_Wimey_Basico`:

```bash
make run
```

Esse comando compila o projeto (se necessário) e abre o jogo.

| Comando | Função |
|---|---|
| `make` | Compila o jogo e o gerenciador. |
| `make run` | Compila e abre o jogo. |
| `make gerenciar` | Abre o cadastro de perguntas no terminal. |
| `make teste` | Executa os testes automáticos. |
| `make clean` | Remove executáveis, arquivos objeto e temporários dos testes. |
| `make help` | Lista todos os comandos. |

> `make clean` **não** apaga `dados/perguntas.bin`. O banco de perguntas é preservado.

## 2. Instalação no Linux

O projeto usa GCC, Make e Raylib. O desenvolvimento foi feito com a **Raylib 5.5** (a versão dos cabeçalhos em `include/`); versões muito diferentes podem exigir ajustes.

### 2.1. Ferramentas e dependências

```bash
sudo apt update
sudo apt install build-essential git libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev libwayland-dev libxkbcommon-dev
```

### 2.2. Raylib

Se a Raylib já estiver instalada, pule esta etapa. Para instalar a partir do código-fonte, fixando a versão testada:

```bash
cd ~
git clone --depth 1 --branch 5.5 https://github.com/raysan5/raylib.git
cd raylib/src
make PLATFORM=PLATFORM_DESKTOP
sudo make install
sudo ldconfig
```

### 2.3. Compilar e jogar

Extraia o ZIP, entre em `Timey_Wimey_Basico` e execute:

```bash
make
make run
```

## 3. Instalação no Windows

O caminho recomendado é o **MSYS2 (terminal UCRT64)**.

### 3.1. Instalar as dependências

1. Instale o [MSYS2](https://www.msys2.org/) no caminho padrão (`C:\msys64`).
2. Abra o terminal **MSYS2 UCRT64** e execute:

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc make mingw-w64-ucrt-x86_64-raylib
```

### 3.2. Compilar e jogar com os scripts

Com o MSYS2 no caminho padrão, basta dar duplo clique nos scripts da pasta do projeto:

| Script | Função |
|---|---|
| `compilar_windows.bat` | Executa `make clean` e `make`, gerando `timey_wimey.exe` e `gerenciador.exe`. |
| `executar_windows.bat` | Abre o jogo. |
| `executar_gerenciador_windows.bat` | Abre o cadastro de perguntas. |

Se o MSYS2 foi instalado em outra pasta, ajuste a linha `PATH` no início de cada `.bat`.

### 3.3. Compilar pelo terminal

No terminal MSYS2 UCRT64, dentro da pasta do projeto, os comandos da seção 1 funcionam normalmente (`make`, `make run`, ...).

## 4. Comandos do jogo

### 4.1. Menu inicial

| Tecla | Ação |
|---|---|
| `1` ou clique | Iniciar Modo História |
| `2` ou clique | Iniciar Modo Versus |
| `3` ou clique | Consultar perguntas por dificuldade |
| `4` ou clique | Exportar perguntas para CSV |
| `5`, `ESC` ou clique | Sair do jogo |

### 4.2. Movimentação e portais (Modo História)

| Tecla | Ação |
|---|---|
| `W`, `A`, `S`, `D` ou `↑`, `←`, `↓`, `→` | Movimentar o personagem |
| `E` | Entrar no portal quando estiver próximo |
| `L` | Abrir a Loja da TARDIS |
| `ESC` | Voltar ao menu |
| Clique no portal | O personagem caminha até o portal e entra automaticamente |

A cada região são apresentados três portais. Escolha um para entrar na época correspondente. Cada região tem cinco perguntas; ao concluí-las, o jogador volta ao mapa para escolher outro portal.

### 4.3. Perguntas (Modo História e Modo Versus)

| Tecla | Ação |
|---|---|
| `A` ou `1` | Alternativa A |
| `B` ou `2` | Alternativa B |
| `C` ou `3` | Alternativa C |
| `D` ou `4` | Alternativa D |
| Clique na alternativa | Escolher a resposta correspondente |
| `ENTER` ou clique no texto | Avançar após a tela de feedback |

### 4.4. Dicas do Modo História

| Tecla | Dica | Função |
|---|---|---|
| `S` ou clique | Chave de Fenda Sônica | Elimina uma alternativa incorreta. |
| `H` ou clique | Arquivos da TARDIS | Apresenta uma votação simulada para as alternativas. |
| `T` ou clique | Reversão Temporal | Na tela de resposta incorreta, troca a pergunta por outra do mesmo nível, sem retirar uma vida. |
| Automático | Protocolo de Regeneração | Com zero vidas, apresenta uma pergunta especial muito difícil. Acertar devolve uma vida e a pergunta original; errar encerra a expedição. |

A Chave de Fenda Sônica e os Arquivos da TARDIS podem ser usados durante a pergunta, respeitando as cargas disponíveis. A Reversão Temporal aparece somente depois de uma resposta incorreta. O Protocolo de Regeneração não permite dicas.

### 4.5. Dicas do Modo Versus

| Tecla | Dica | Função |
|---|---|---|
| `F` ou clique | 50/50 | Remove duas alternativas incorretas. |
| `P` ou clique | Plateia | Apresenta uma votação simulada. |

Cada dica pode ser usada uma vez por partida.

### 4.6. Loja da TARDIS

Abra com `L` ou clicando no ícone no canto superior esquerdo da tela de portais.

| Tecla | Power-up | Preço | Função |
|---|---|---:|---|
| `1` ou clique | Reparo de Vida | 300 fragmentos | Recupera uma vida (limite de cinco). |
| `2` ou clique | Chave de Fenda Sônica | 75 fragmentos | Adiciona uma carga para eliminar uma alternativa incorreta. |
| `3` ou clique | Arquivos da TARDIS | 100 fragmentos | Adiciona uma consulta com votação simulada. |
| `4` ou clique | Reversão Temporal | 150 fragmentos | Adiciona uma carga para trocar a pergunta depois de errar. |
| `ESC` ou clique | Voltar | n/a | Fecha a loja e retorna ao mapa. |

Os fragmentos temporais são a moeda da expedição: são recebidos ao acertar perguntas e concluir regiões. Não aumentam o nível do jogador nem recuperam vidas automaticamente. As compras são temporárias e não são mantidas entre expedições.

### 4.7. Batalha contra O Paradoxo

O Modo História tem 50 perguntas. As primeiras 40 formam oito regiões de cinco perguntas; as dez últimas acontecem na arena de O Paradoxo.

| Fase | Perguntas | Forma do chefe |
|---|---|---|
| Instabilidade Temporal | 41 a 44 | Primeira forma |
| Colapso Temporal | 45 a 50 | Segunda forma |

A batalha tem cinco rodadas de duas perguntas, e cada rodada representa um núcleo temporal do chefe:

| Rodada | Perguntas |
|---|---|
| 1 | 41 e 42 |
| 2 | 43 e 44 |
| 3 | 45 e 46 |
| 4 | 47 e 48 |
| 5 | 49 e 50 |

Acertar pelo menos uma pergunta da rodada destrói um núcleo; errar as duas mantém o núcleo ativo. Acertar as duas concede 30 fragmentos adicionais. Para vencer, é preciso destruir os cinco núcleos e acertar a pergunta 50. Use `ENTER` ou clique no botão indicado para iniciar a batalha e avançar nas telas de feedback.

### 4.8. Consulta de perguntas

| Tecla | Dificuldade |
|---|---|
| `1` | Muito fácil |
| `2` | Fácil |
| `3` | Médio |
| `4` | Difícil |
| `5` | Muito difícil |

Use `←` e `→` (ou os botões da tela) para navegar. A alternativa correta aparece em verde. `ENTER` ou `ESC` voltam ao menu.

### 4.9. Tela cheia

| Tecla | Ação |
|---|---|
| `F11` | Alternar entre janela e tela cheia |
| `ALT + F4` | Fechar a janela do jogo |

A resolução interna é de 1000 × 650 pixels. Ao redimensionar a janela ou entrar em tela cheia, a interface mantém as proporções.

## 5. Cadastro de perguntas

O cadastro roda separadamente, no terminal (no Windows, use `executar_gerenciador_windows.bat`):

```bash
make gerenciar
```

| Opção | Ação |
|---|---|
| `1` | Inserir pergunta |
| `2` | Listar perguntas |
| `3` | Pesquisar pergunta |
| `4` | Alterar pergunta |
| `5` | Excluir pergunta |
| `6` | Navegar por dificuldade |
| `7` | Exportar para CSV |
| `0` | Salvar e sair |

O cadastro usa vetor dinâmico, então não há limite fixo de perguntas definido no código.

## 6. Banco de perguntas

O banco inicial tem **1.000 perguntas**, 200 por nível: muito fácil, fácil, médio, difícil e muito difícil.

O CSV usa ponto e vírgula (`;`) como separador:

```csv
id;enunciado;alternativa_a;alternativa_b;alternativa_c;alternativa_d;resposta_correta;dificuldade
```

Arquivos em `dados/`:

| Arquivo | Função |
|---|---|
| `perguntas_iniciais.csv` | Banco inicial, carregado na primeira execução. |
| `perguntas.bin` | Banco salvo em binário, usado nas execuções seguintes. |
| `perguntas_exportadas.csv` | Gerado pela opção de exportação. |

Na primeira execução, o programa lê o CSV e grava o `.bin` ao encerrar. Depois, o `.bin` tem prioridade. O formato binário tem controle de versão: se for incompatível com a versão atual do programa, o banco é reconstruído a partir do CSV inicial.

### Restaurar o banco inicial

> **Atenção:** apagar o `.bin` descarta todas as perguntas inseridas ou alteradas depois da instalação. Antes, exporte o banco atual (opção `4` do menu do jogo ou `7` do gerenciador) e guarde o CSV gerado.

Com o jogo e o cadastro fechados:

```bash
rm dados/perguntas.bin
```

Na próxima execução, o programa carrega novamente `dados/perguntas_iniciais.csv`.

## 7. Estrutura do projeto

```text
Timey_Wimey_Basico/
├── src/                   código-fonte (.c)
├── include/               cabeçalhos do projeto e da Raylib
├── dados/                 banco de perguntas (CSV e binário)
├── imagens/
│   ├── fundos/            menu e fundos dos 12 mundos
│   ├── personagem/        quadros de movimentação (4 direções × 4 quadros)
│   ├── portais/           portais temporais
│   ├── dicas/             ícones das três dicas
│   ├── loja/              ícone da Loja da TARDIS
│   └── chefe/             O Paradoxo (duas fases)
├── sons/                  acerto, erro e portal
├── testes/                testes automáticos (make teste)
├── docs/                  documentação de apoio
├── Makefile
└── *_windows.bat          scripts para Windows
```

Os caminhos e nomes dos arquivos de imagem são fixos no código. Para trocar uma arte, mantenha o nome e o tamanho do arquivo original. Detalhes dos fundos dos mundos estão em `imagens/fundos/mundos/README.md`.

## 8. Problemas comuns

| Sintoma | Solução |
|---|---|
| `ERRO: Raylib nao foi encontrada.` | Instale a Raylib (seções 2.2 ou 3.1). No Linux, confirme com `pkg-config --exists raylib && echo ok`. |
| `make: command not found` no Windows | Use o terminal **MSYS2 UCRT64** ou os scripts `.bat`. |
| O `.bat` não acha `gcc` ou `make` | O MSYS2 não está em `C:\msys64`. Ajuste o `PATH` no início do script. |
| Janela abre, mas faltam imagens | Execute o jogo a partir da pasta `Timey_Wimey_Basico`. Os caminhos de `imagens/` e `sons/` são relativos. |
| Faltam perguntas ou o banco parece corrompido | Exporte o banco para CSV e siga a seção 6 para restaurar. |

## 9. Créditos e observações

- Feito em C11, com GCC, Make e Raylib 5.5. A fonte usada é a padrão da Raylib; não é preciso instalar fontes.
- O Makefile tem configuração para Linux e para Windows (MSYS2 UCRT64). Teste a compilação e a execução no sistema em que o jogo será apresentado.
- **Uso de IA:** o banco de perguntas foi elaborado com auxílio do ChatGPT e os fundos dos mundos foram gerados com o Gemini.
- Sprites: Hadassah AI