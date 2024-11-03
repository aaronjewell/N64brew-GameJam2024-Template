#include <libdragon.h>

#include "../../core.h"
#include "../../minigame.h"
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>

#define FONT_TEXT 1

#define COUNTDOWN_DELAY 3.0f
#define GO_DELAY 1.0f
#define WIN_DELAY 5.0f
#define WIN_SHOW_DELAY 2.0f

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define HALF_SCREEN_WIDTH 160
#define HALF_SCREEN_HEIGHT 120
#define SCREEN_MARGIN 16
#define PADDLE_WIDTH 16
#define HALF_PADDLE_WIDTH 8
#define PADDLE_HEIGHT 4
#define HALF_PADDLE_HEIGHT 2

#define GAME_BACKGROUND 0x000000FF
#define PADDLE_COLOR 0xFFFFFFFF

const MinigameDef minigame_def = {.gamename = "Pong",
                                  .developername = "Aaron",
                                  .description = "This is a game of Pong.",
                                  .instructions =
                                      "Move your paddle with the joystick."};

int player_screen_coords[MAXPLAYERS][2] = {
    {HALF_SCREEN_WIDTH - HALF_PADDLE_WIDTH, SCREEN_MARGIN},
    {SCREEN_WIDTH - SCREEN_MARGIN - PADDLE_HEIGHT,
      HALF_SCREEN_HEIGHT - HALF_PADDLE_WIDTH},
    {HALF_SCREEN_WIDTH - HALF_PADDLE_WIDTH,
      SCREEN_HEIGHT - SCREEN_MARGIN - PADDLE_HEIGHT},
    {SCREEN_MARGIN, HALF_SCREEN_HEIGHT - HALF_PADDLE_WIDTH}};

rdpq_font_t *font;

float countdown_timer;
float end_timer;
bool is_ending;

bool has_player_won(PlyNum player) { return false; }

bool is_countdown() { return countdown_timer > 0; }

bool can_control() { return !is_ending && !is_countdown(); }

void minigame_init() {
  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE,
               FILTERS_RESAMPLE);

  // load font
  font = rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_VAR);
  rdpq_text_register_font(FONT_TEXT, font);

  countdown_timer = COUNTDOWN_DELAY;
}

void minigame_fixedloop(float deltatime) {
  bool could_control = can_control();
  if (countdown_timer > -GO_DELAY) {
    float prevtime = countdown_timer;
    countdown_timer -= deltatime;
    if ((int)prevtime != (int)countdown_timer && countdown_timer > 0) {
      // Do counting down stuff
    }
  }

  if (is_ending) {
    float prevendtime = end_timer;
    end_timer += deltatime;
    if ((int)prevendtime != (int)end_timer &&
        (int)end_timer == WIN_SHOW_DELAY) {
      // Do winner stuff
    }

    if (end_timer > WIN_DELAY) {
      minigame_end();
    }
  }

  if (!can_control()) {
    return;
  }

  if (!could_control && can_control()) {
    // Do game started stuff like sound
  }

  T3DVec3 newDir = {0};

  for (size_t i = 0; i < MAXPLAYERS; i++) {
    if (can_control()) {
      if (i < core_get_playercount()) {
        // For human players, check if physical buttons are pressed
        joypad_inputs_t joypad =
            joypad_get_inputs(core_get_playercontroller(i));

        newDir.v[0] = (float)joypad.stick_x * 0.05f;
        newDir.v[2] = -(float)joypad.stick_y * 0.05f;
      } else {
        newDir.v[0] = 0.0f;
        newDir.v[2] = 0.0f;
        // AI movement
      }

      // restrict axix movement
      bool is_horizontal = i % 2 == 0;

      if (is_horizontal) {
        player_screen_coords[i][0] += newDir.v[0]; 
      } else {
        player_screen_coords[i][1] += newDir.v[2];
      }

      // clamp to screen bounds
      if (player_screen_coords[i][0] < SCREEN_MARGIN) player_screen_coords[i][0] = SCREEN_MARGIN;
      if (player_screen_coords[i][0] > SCREEN_WIDTH - SCREEN_MARGIN) player_screen_coords[i][0] = SCREEN_WIDTH - SCREEN_MARGIN;
      if (player_screen_coords[i][1] < SCREEN_MARGIN) player_screen_coords[i][1] = SCREEN_MARGIN;
      if (player_screen_coords[i][1] > SCREEN_HEIGHT - SCREEN_MARGIN) player_screen_coords[i][1] = SCREEN_HEIGHT - SCREEN_MARGIN;
    }

    // Do player stuff including AI
  }

  for (size_t i = 0; i < MAXPLAYERS; i++) {
    // Check for win condition
    if (has_player_won(i)) {
      core_set_winner(i);
      is_ending = true;
    }
  }
}

void minigame_loop(float deltatime) {

  // Render the UI
  rdpq_attach(display_get(), NULL);
  rdpq_clear(color_from_packed32(GAME_BACKGROUND));

  for (size_t i = 0; i < MAXPLAYERS; i++) {
    int xcur = player_screen_coords[i][0];
    int ycur = player_screen_coords[i][1];

    bool horizontal = i % 2 == 0;

    rdpq_set_mode_standard();

    // Draw paddles
    rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
    rdpq_set_prim_color(color_from_packed32(PADDLE_COLOR));

    if (horizontal)
      rdpq_fill_rectangle(xcur, ycur, xcur + PADDLE_WIDTH,
                          ycur + PADDLE_HEIGHT);
    else
      rdpq_fill_rectangle(xcur, ycur, xcur + PADDLE_HEIGHT,
                          ycur + PADDLE_WIDTH);
  }

  rdpq_set_mode_standard();

  if (is_countdown()) {
    // Draw countdown
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 155, 100, "%d",
                     (int)ceilf(countdown_timer));
  } else if (countdown_timer > -GO_DELAY) {
    // For a short time after countdown is over, draw "GO!"
    rdpq_text_print(NULL, FONT_BUILTIN_DEBUG_MONO, 150, 100, "GO!");
  } else if (is_ending && end_timer >= WIN_SHOW_DELAY) {
    // Draw winner announcement (There might be multiple winners)
    int ycur = 100;
    for (size_t i = 0; i < MAXPLAYERS; i++) {
      if (!has_player_won(i))
        continue;
      ycur += rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, 120, ycur,
                               "Player %d wins!\n", i + 1)
                  .advance_y;
    }
  }

  rdpq_detach_show();
}

void minigame_cleanup() {
  display_close();
  rdpq_text_unregister_font(FONT_TEXT);
  rdpq_font_free(font);
}
