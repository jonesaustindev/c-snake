#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//
// Data
//
#define GRID_ROWS 50
#define GRID_COLUMNS 50
#define GRID_SIZE 20

Color background_color = {71, 49, 68, 255};
int screen_width = 1280;
int screen_height = 720;

int grid_width = GRID_COLUMNS * GRID_SIZE;
int grid_height = GRID_ROWS * GRID_SIZE;

//
// Objects
//
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
  struct {
    Vector2 position;
    bool is_alive;
  } food;
  float move_rate;
} GameState;

//
// Utils
//
Vector2 get_start_pos() {
  int start_x = (screen_width - grid_width) / 2;
  int start_y = (screen_height - grid_height) / 2;

  Vector2 start_pos = {.x = start_x, .y = start_y};

  return start_pos;
}

Vector2 get_random_pos() {
  int x = rand() % GRID_COLUMNS;
  int y = rand() % GRID_ROWS;

  // ugly but my math isn't great right now
  if (x < 0) {
    x *= -1;
  }
  if (y < 0) {
    y *= -1;
  }

  Vector2 random_pos = {.x = x * GRID_SIZE, .y = y * GRID_SIZE};
  return random_pos;
}

void reset_input(InputManager *input) {
  input->up = false;
  input->down = false;
  input->left = false;
  input->right = false;
}

//
// Update logic
//
void handle_input(InputManager *input) {
  input->left = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A);
  input->right = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D);
  input->up = IsKeyDown(KEY_UP) || IsKeyDown(KEY_W);
  input->down = IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S);
}

void update(GameState *state, InputManager *input) {
  if (input->left && state->player->direction != RIGHT) {
    state->player->direction = LEFT;
  } else if (input->right && state->player->direction != LEFT) {
    state->player->direction = RIGHT;
  } else if (input->up && state->player->direction != DOWN) {
    state->player->direction = UP;
  } else if (input->down && state->player->direction != UP) {
    state->player->direction = DOWN;
  }

  static float accumulator = 0;
  accumulator += GetFrameTime();

  if (accumulator >= state->move_rate) {
    switch (state->player->direction) {
    case LEFT:
      state->player->position.x -= 1;
      break;
    case RIGHT:
      state->player->position.x += 1;
      break;
    case UP:
      state->player->position.y -= 1;
      break;
    case DOWN:
      state->player->position.y += 1;
      break;
    }
    accumulator = 0;
  }
}

//
// Render
//
void draw_debug() {
  char fps_str[32];
  sprintf(fps_str, "FPS: %d", GetFPS());
  DrawText(fps_str, 10, 10, 20, LIGHTGRAY);
}

void render_grid(GameState *state) {
  int grid_width = GRID_COLUMNS * GRID_SIZE;
  int grid_height = GRID_ROWS * GRID_SIZE;

  int start_x = (screen_width - grid_width) / 2;
  int start_y = (screen_height - grid_height) / 2;

  for (int j = 0; j < GRID_ROWS; j++) {
    for (int k = 0; k < GRID_COLUMNS; k++) {
      Vector2 pos = {.x = start_x + k * GRID_SIZE,
                     .y = start_y + j * GRID_SIZE};
      Vector2 size = {.x = GRID_SIZE, .y = GRID_SIZE};

      DrawRectangleLines(pos.x, pos.y, size.x, size.y, BLACK);
    }
  }
}

void render_food(GameState *state) {
  Vector2 size = {.x = GRID_SIZE, .y = GRID_SIZE};

  if (!state->food.is_alive) {
    Vector2 pos = get_random_pos();
    Vector2 start_pos = get_start_pos();
    state->food.position.x = start_pos.x + pos.x;
    state->food.position.y = start_pos.y + pos.y;
    state->food.is_alive = true;
  }

  DrawRectangleV(state->food.position, size, WHITE);
}

void render(GameState *state) {
  // grid
  render_grid(state);

  // food
  render_food(state);

  // player
  Vector2 size = {.x = GRID_SIZE, .y = GRID_SIZE};
  Vector2 start_pos = get_start_pos();
  Vector2 pixel_position = {start_pos.x + state->player->position.x * GRID_SIZE,
                            start_pos.y +
                                state->player->position.y * GRID_SIZE};
  DrawRectangleV(pixel_position, size, WHITE);
}

//
// Main
//
int main(void) {
  srand(time(0));
  Vector2 start_pos = get_start_pos();
  Player player = {.position.x = (float)GRID_COLUMNS / 2,
                   .position.y = (float)GRID_ROWS / 2,
                   .speed = 10.0f,
                   .snake_length = 1,
                   .direction = RIGHT};

  GameState game_state = {.player = &player,
                          .current_scene = PLAY,
                          .food = {.is_alive = false},
                          .move_rate = 0.1};

  InputManager input_manager = {
      .left = false, .right = false, .up = false, .down = false};

  InitWindow(screen_width, screen_height, "Snake");
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
