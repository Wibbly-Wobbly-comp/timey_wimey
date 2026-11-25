/* Implementações vazias para testar a lógica sem abrir uma janela Raylib. */
#include "raylib.h"
#include <string.h>

Texture2D LoadTexture(const char *caminho) {
    Texture2D imagem = {0};
    (void)caminho;
    return imagem;
}

void SetTextureFilter(Texture2D imagem, int filtro) {
    (void)imagem;
    (void)filtro;
}

void UnloadTexture(Texture2D imagem) { (void)imagem; }
void DrawTexturePro(Texture2D imagem, Rectangle origem, Rectangle destino,
                    Vector2 origem_rotacao, float rotacao, Color cor) {
    (void)imagem; (void)origem; (void)destino; (void)origem_rotacao;
    (void)rotacao; (void)cor;
}
void DrawText(const char *texto, int x, int y, int tamanho, Color cor) {
    (void)texto; (void)x; (void)y; (void)tamanho; (void)cor;
}
int MeasureText(const char *texto, int tamanho) {
    (void)texto; (void)tamanho;
    return 0;
}
void DrawCircle(int x, int y, float raio, Color cor) {
    (void)x; (void)y; (void)raio; (void)cor;
}
void DrawCircleLines(int x, int y, float raio, Color cor) {
    (void)x; (void)y; (void)raio; (void)cor;
}

int IsMouseButtonPressed(int botao) { (void)botao; return 0; }
Vector2 GetMousePosition(void) { return (Vector2){-1.0f, -1.0f}; }
int CheckCollisionPointRec(Vector2 ponto, Rectangle area) {
    return ponto.x >= area.x && ponto.x <= area.x + area.width &&
           ponto.y >= area.y && ponto.y <= area.y + area.height;
}
