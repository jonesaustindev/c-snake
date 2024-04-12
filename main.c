#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>

#define GRID_ROWS 50
#define GRID_COLUMNS 50

Color background_color = {71, 49, 68, 255};

typedef enum Direction { LEFT, RIGHT, UP, DOWN } Direction;

typedef struct Player {
  Vector2 position;
  float speed;
  int snake_length;
  Direction direction;
} Player;

typedef struct InputManager {
  bool left;
  bool right;
  bool up;
  bool down;
} InputManager;

typedef enum Scene { MENU, PLAY, PAUSE, GAME_OVER } Scene;

typedef struct GameState {
  Player *player;
  Scene current_scene;
  int grid[GRID_ROWS][GRID_COLUMNS];
} GameState;

void reset_input(InputManager *input) {
  input->up = false;
  input->down = false;
  input->left = false;
  input->right = false;
}

void draw_debug() {
  char fps_str[32];
  sprintf(fps_str, "FPS: %d", GetFPS());
  DrawText(fps_str, 10, 10, 20, LIGHTGRAY);
}

void handle_input(InputManager *input) {
  input->left = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A);
  input->right = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D);
  input->up = IsKeyDown(KEY_UP) || IsKeyDown(KEY_W);
  input->down = IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S);
}

void update(GameState *state, InputManager *input) {
  if (input->left) {
    state->player->direction = LEFT;
  }
  if (input->right) {
    state->player->direction = RIGHT;
  }
  if (input->up) {
    state->player->direction = UP;
  }
  if (input->down) {
    state->player->direction = DOWN;
  }

  switch (state->player->direction) {
  case LEFT:
    state->player->position.x -= 10;
    break;
  case RIGHT:
    state->player->position.x += 10;
    break;
  case UP:
    state->player->position.y -= 10;
    break;
  case DOWN:
    state->player->position.y += 10;
    break;
  }
}

void render(GameState *state) {
  Vector2 size = {.x = 20, .y = 20};
  DrawRectangleV(state->player->position, size, WHITE);
}

int main(void) {
  Player player = {.position.x = 25,
                   .position.y = 25,
                   .speed = 10.0f,
                   .snake_length = 1,
                   .direction = RIGHT};

  GameState game_state = {
      .player = &player,
      .current_scene = PLAY,
  };

  InputManager input_manager = {
      .left = false, .right = false, .up = false, .down = false};

  InitWindow(1280, 720, "Snake");
  SetWindowState(FLAG_WINDOW_RESIZABLE);
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    reset_input(&input_manager);

    BeginDrawing();
    ClearBackground(background_color);

    draw_debug();

    handle_input(&input_manager);

    update(&game_state, &input_manager);
    render(&game_state);

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
