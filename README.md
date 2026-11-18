# Timey Wimey — instalação e comandos

## 1. Instalação no Linux

O projeto precisa de GCC, Make e Raylib.

### 1.1. Instalar as ferramentas e dependências

Abra o terminal e execute:

```bash
sudo apt update
sudo apt install build-essential git libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev libwayland-dev libxkbcommon-dev
```

### 1.2. Instalar a Raylib

Caso a Raylib já esteja instalada, pule esta etapa. Para instalar a partir do código-fonte:

```bash
cd ~
git clone --depth 1 https://github.com/raysan5/raylib.git
cd raylib/src
make PLATFORM=PLATFORM_DESKTOP
sudo make install
sudo ldconfig
```

### 1.3. Compilar o Timey Wimey

Extraia o arquivo ZIP do projeto. No terminal, entre na pasta `Timey_Wimey_Basico` e execute:

```bash
make
```

Para abrir o jogo:

```bash
make run
```

Para abrir o cadastro de perguntas no terminal:

```bash
make gerenciar
```

Para apagar os executáveis gerados:

```bash
make clean
```

## 2. Comandos do jogo

### Menu inicial

| Tecla | Ação |
|---|---|
| `1` ou `ENTER` | Iniciar expedição |
| `2` | Consultar perguntas por dificuldade |
| `3` | Exportar perguntas para CSV |
| `4` ou `ESC` | Sair do jogo |

### Movimentação e portais

| Tecla | Ação |
|---|---|
| `W`, `A`, `S`, `D` | Movimentar o personagem |
| `↑`, `←`, `↓`, `→` | Movimentar o personagem |
| `E` | Entrar no portal quando estiver próximo |
| `ESC` | Voltar ao menu |

Aproxime o personagem de um dos três portais. Quando aparecer a indicação na tela, pressione `E` para entrar na região escolhida. Cada região apresenta cinco perguntas.

### Perguntas

| Tecla | Resposta |
|---|---|
| `A` ou `1` | Alternativa A |
| `B` ou `2` | Alternativa B |
| `C` ou `3` | Alternativa C |
| `D` ou `4` | Alternativa D |
| `ENTER` | Avançar após a tela de resposta ou retornar ao menu na tela de resultado |

A expedição possui 15 perguntas. Após responder corretamente, pressione `ENTER` para continuar. A cada cinco perguntas, o personagem retorna ao mapa para escolher outro portal.

### Consulta de perguntas

Na tela de consulta, escolha a dificuldade:

| Tecla | Dificuldade |
|---|---|
| `1` | Muito fácil |
| `2` | Fácil |
| `3` | Médio |
| `4` | Difícil |
| `5` | Muito difícil |

Use `←` e `→` para navegar entre as perguntas do nível selecionado. Pressione `ENTER` ou `ESC` para voltar ao menu.

## 3. Cadastro de perguntas

O cadastro é executado separadamente pelo terminal:

```bash
make gerenciar
```

Dentro do cadastro, utilize:

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

## 4. Arquivos de perguntas

- `dados/perguntas_iniciais.csv`: perguntas carregadas na primeira execução.
- `dados/perguntas.bin`: dados salvos automaticamente ao sair do jogo ou do cadastro.
- `dados/perguntas_exportadas.csv`: arquivo gerado pela opção de exportação.

O arquivo binário tem prioridade nas execuções seguintes. Para carregar novamente a coleção inicial, feche o programa e apague `dados/perguntas.bin`; na próxima execução, o CSV inicial será carregado.


### Observações:
Perguntas feitas pelo ChatGPT.
Makefile ainda falta compatibi1lidade com o Windows.
Sprites e fundos ainda em produção.
