#ifndef RAYLIB_H
#define RAYLIB_H
#include <stdarg.h>
typedef struct Vector2 { float x,y; } Vector2;
typedef struct Color { unsigned char r,g,b,a; } Color;
typedef struct Rectangle { float x,y,width,height; } Rectangle;
typedef struct Texture2D { unsigned int id; int width,height,mipmaps,format; } Texture2D;
typedef struct Sound { unsigned int frameCount; void *stream; } Sound;
typedef struct RenderTexture2D { unsigned int id; Texture2D texture; } RenderTexture2D;
#define WHITE ((Color){255,255,255,255})
#define BROWN ((Color){165,42,42,255})
#define BEIGE ((Color){245,245,220,255})
#define RED ((Color){255,0,0,255})
#define RAYWHITE ((Color){245,245,245,255})
#define SKYBLUE ((Color){102,191,255,255})
#define LIGHTGRAY ((Color){200,200,200,255})
#define GRAY ((Color){130,130,130,255})
#define DARKGRAY ((Color){80,80,80,255})
#define GREEN ((Color){0,228,48,255})
#define GOLD ((Color){255,203,0,255})
#define BLACK ((Color){0,0,0,255})
#define FLAG_WINDOW_RESIZABLE 4
#define FLAG_VSYNC_HINT 64
#define KEY_F11 300
#define TEXTURE_FILTER_BILINEAR 1
#define KEY_NULL 0
#define KEY_A 65
#define KEY_B 66
#define KEY_C 67
#define KEY_D 68
#define KEY_E 69
#define KEY_F 70
#define KEY_H 72
#define KEY_L 76
#define KEY_P 80
#define KEY_S 83
#define KEY_T 84
#define KEY_W 87
#define KEY_ONE 49
#define KEY_TWO 50
#define KEY_THREE 51
#define KEY_FOUR 52
#define KEY_FIVE 53
#define KEY_ENTER 257
#define KEY_ESCAPE 256
#define KEY_UP 265
#define KEY_DOWN 264
#define KEY_LEFT 263
#define KEY_RIGHT 262
#define TEXTURE_FILTER_POINT 0
#define TEXTURE_FILTER_BILINEAR 1
#define MOUSE_BUTTON_LEFT 0
void SetConfigFlags(unsigned int); void InitWindow(int,int,const char*); int IsWindowReady(void); void SetWindowMinSize(int,int); void ToggleFullscreen(void); int GetScreenWidth(void); int GetScreenHeight(void); RenderTexture2D LoadRenderTexture(int,int); void UnloadRenderTexture(RenderTexture2D); void BeginTextureMode(RenderTexture2D); void EndTextureMode(void);
int IsAudioDeviceReady(void);
void InitWindow(int,int,const char*); void SetExitKey(int); void InitAudioDevice(void); void CloseAudioDevice(void); void CloseWindow(void); int WindowShouldClose(void); void SetTargetFPS(int); float GetFrameTime(void);
int IsKeyPressed(int); int IsKeyDown(int);
int IsMouseButtonPressed(int); Vector2 GetMousePosition(void);
int CheckCollisionPointRec(Vector2, Rectangle);
Sound LoadSound(const char*); void UnloadSound(Sound); void PlaySound(Sound);
Texture2D LoadTexture(const char*); void SetTextureFilter(Texture2D,int); void UnloadTexture(Texture2D);
void DrawTexturePro(Texture2D, Rectangle, Rectangle, Vector2, float, Color);
void BeginDrawing(void); void EndDrawing(void); void ClearBackground(Color);
void DrawText(const char*,int,int,int,Color); int MeasureText(const char*,int);
const char *TextFormat(const char*,...);
void DrawRectangle(int,int,int,int,Color); void DrawRectangleRounded(Rectangle,float,int,Color);
void DrawRectangleLinesEx(Rectangle,float,Color); void DrawTriangle(Vector2,Vector2,Vector2,Color); void DrawLine(int,int,int,int,Color); void DrawCircle(int,int,float,Color); void DrawCircleLines(int,int,float,Color); void DrawTextureEx(Texture2D,Vector2,float,float,Color);
#endif
