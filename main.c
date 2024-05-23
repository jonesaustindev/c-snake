#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//
// Data
//
#define GRID_ROWS 20
#define GRID_COLUMNS 20
#define GRID_SIZE 30
#define DEFAULT_SPEED_INCREMENT 0.125
#define SPEED_INCREMENT 0.005

Color background_color = {71, 49, 68, 255};
int screen_width = 800;
int screen_height = 800;

int grid_width = GRID_COLUMNS * GRID_SIZE;
int grid_height = GRID_ROWS * GRID_SIZE;

//
// Objects
//
typedef enum { LEFT, RIGHT, UP, DOWN } Direction;

typedef struct {
  Vector2 current;
  Vector2 last;
} Position;

typedef struct {
  Position position;
  Direction direction;
} Player;

typedef struct {
  bool left;
  bool right;
  bool up;
  bool down;
  bool enter;
} InputManager;

typedef enum { MENU, PLAY, PAUSE, GAME_OVER } Scene;

typedef struct {
  Player *player;
  Position body[100];
  int body_length;
  Scene current_scene;
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

bool is_outside_grid(Vector2 position) {
  Vector2 start_pos = get_start_pos();
  int end_x = start_pos.x + grid_width;
  int end_y = start_pos.y + grid_height;

  if (position.x < start_pos.x || position.x >= end_x ||
      position.y < start_pos.y || position.y >= end_y) {
    return true;
  } else {
    return false;
  }
}

bool is_spawn_on_body(Vector2 position, GameState *state) {
  for (int i = 0; i < state->body_length; i++) {
    if (position.x == state->body[i].current.x &&
        position.y == state->body[i].current.y) {
      return true;
    }
  }

  return false;
}

void reset_input(InputManager *input) {
  input->up = false;
  input->down = false;
  input->left = false;
  input->right = false;
  input->enter = false;
}

Vector2 get_random_pos(GameState *state) {
  Vector2 start_pos = get_start_pos();
  Vector2 random_pos;

  do {
    int x = rand() % GRID_COLUMNS;
    int y = rand() % GRID_ROWS;

    random_pos.x = start_pos.x + x * GRID_SIZE;
    random_pos.y = start_pos.y + y * GRID_SIZE;
  } while (is_outside_grid(random_pos) && is_spawn_on_body(random_pos, state));

  return random_pos;
}

Player *setup_player() {
  Player *player = (Player *)malloc(sizeof(Player));

  if (player == NULL) {
    printf("player could not be allocated\n");
    return 0;
  }

  player->position.current.x = (float)GRID_COLUMNS / 2;
  player->position.current.y = (float)GRID_ROWS / 2;
  player->direction = RIGHT;

  return player;
}

//
// Update logic
//
void handle_input(InputManager *input) {
  input->left = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A);
  input->right = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D);
  input->up = IsKeyDown(KEY_UP) || IsKeyDown(KEY_W);
  input->down = IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S);
  input->enter = IsKeyDown(KEY_ENTER);
}

void hit_detection(GameState *state) {
  Vector2 pixel_position = {
      get_start_pos().x + state->player->position.current.x * GRID_SIZE,
      get_start_pos().y + state->player->position.current.y * GRID_SIZE};

  // snake ate the food
  // set to not alive to be respawned
  // speed up movement
  if (pixel_position.x == state->food.position.x &&
      pixel_position.y == state->food.position.y) {
    state->food.is_alive = false;
    state->body_length += 1;

    if (state->body_length > 0) {
      state->body[state->body_length - 1] = state->player->position;
    } else {
      state->body[state->body_length - 1] = state->body[state->body_length - 2];
    }

    state->move_rate -= SPEED_INCREMENT;
  }
}

void update(GameState *state, InputManager *input) {
  if (state->current_scene == PLAY) {
    state->player->position.last = state->player->position.current;

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
        if (state->player->position.current.x - 1 < GRID_ROWS) {
        }
        state->player->position.current.x -= 1;
        break;
      case RIGHT:
        state->player->position.current.x += 1;
        break;
      case UP:
        state->player->position.current.y -= 1;
        break;
      case DOWN:
        state->player->position.current.y += 1;
        break;
      }

      // snake body
      for (int i = 0; i < state->body_length; i++) {
        if (i == 0) {
          state->body[i].last = state->body[i].current;
          state->body[i].current = state->player->position.last;
        } else {
          state->body[i].last = state->body[i].current;
          state->body[i].current = state->body[i - 1].last;
        }
      }

      accumulator = 0;
    }

    hit_detection(state);
  }

  if (state->current_scene == GAME_OVER) {
    if (input->enter) {
      state->current_scene = PLAY;

      free(state->player);
      state->player = setup_player();

      state->food.is_alive = false;
      state->body_length = 0;
      state->move_rate = DEFAULT_SPEED_INCREMENT;
    }
  }
}

//
// Draw
//
void draw_debug() {
  char fps_str[32];
  sprintf(fps_str, "FPS: %d", GetFPS());
  DrawText(fps_str, 10, 10, 20, LIGHTGRAY);
}

void draw_game_over() {
  static char game_over_str[32];
  static char subtitle_str[64];
  sprintf(game_over_str, "Game Over");
  sprintf(subtitle_str, "Press 'Enter' to play again");

  int font_size = 72;
  int subtitle_font_size = 24;

  int game_over_width = MeasureText(game_over_str, font_size);
  int game_over_height = font_size;

  int subtitle_width = MeasureText(subtitle_str, subtitle_font_size);
  int subtitle_height = subtitle_font_size;

  Vector2 game_over_position;
  game_over_position.x = (float)(screen_width - game_over_width) / 2;
  game_over_position.y = (float)(screen_height - game_over_height) / 2;

  Vector2 subtitle_position;
  subtitle_position.x = (float)(screen_width - subtitle_width) / 2;
  subtitle_position.y = game_over_position.y + game_over_height + 20;

  DrawText(game_over_str, game_over_position.x, game_over_position.y, font_size,
           LIGHTGRAY);
  DrawText(subtitle_str, subtitle_position.x, subtitle_position.y,
           subtitle_font_size, LIGHTGRAY);
}

void draw_grid(GameState *state) {
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

void draw_food(GameState *state) {
  Vector2 size = {.x = GRID_SIZE, .y = GRID_SIZE};

  if (!state->food.is_alive) {
    state->food.position = get_random_pos(state);
    state->food.is_alive = true;
  }

  DrawRectangleV(state->food.position, size, BLUE);
}

void draw(GameState *state) {
  draw_grid(state);

  draw_food(state);

  if (state->current_scene == PLAY) {
    // player
    Vector2 size = {.x = GRID_SIZE, .y = GRID_SIZE};
    Vector2 start_pos = get_start_pos();
    Vector2 player_position = {
        start_pos.x + state->player->position.current.x * GRID_SIZE,
        start_pos.y + state->player->position.current.y * GRID_SIZE};

    if (is_outside_grid(player_position)) {
      state->current_scene = GAME_OVER;
    }

    DrawRectangleV(player_position, size, WHITE);

    // snake body
    for (int i = 0; i < state->body_length; i++) {
      Vector2 size = {.x = GRID_SIZE, .y = GRID_SIZE};
      Vector2 start_pos = get_start_pos();
      Vector2 pixel_position = {
          start_pos.x + state->body[i].current.x * GRID_SIZE,
          start_pos.y + state->body[i].current.y * GRID_SIZE};
      DrawRectangleV(pixel_position, size, WHITE);
    }
  }

  if (state->current_scene == GAME_OVER) {
    draw_game_over();
  }
}

//
// Main
//
int main(void) {
  srand(time(NULL));

  GameState game_state = {.player = setup_player(),
                          .body_length = 0,
                          .current_scene = PLAY,
                          .food = {.is_alive = false},
                          .move_rate = 0.125};

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
    draw(&game_state);

    EndDrawing();
  }

  // memory cleanup
  free(game_state.player);

  CloseWindow();

  return 0;
}
