#include <libdragon.h>

#include "../../core.h"
#include "../../minigame.h"

#define FONT_TEXT 1

#define COUNTDOWN_DELAY 3.0f
#define GO_DELAY 1.0f
#define WIN_DELAY 5.0f
#define WIN_SHOW_DELAY 2.0f

#define SCREEN_MARGIN 16

#define GAME_BACKGROUND 0x000000FF

const MinigameDef minigame_def = {.gamename = "Pong",
                                  .developername = "Aaron",
                                  .description = "This is a game of Pong.",
                                  .instructions =
                                      "Move your paddle with the joystick."};

rdpq_font_t *font;

float countdown_timer;
float end_timer;
bool is_ending;

bool has_player_won(PlyNum player) { return true; }

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

  for (size_t i = 0; i < MAXPLAYERS; i++) {
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
  if (can_control()) {
    for (size_t i = 0; i < core_get_playercount(); i++) {
      // For human players, check if physical buttons are pressed
      joypad_buttons_t btn =
          joypad_get_buttons_pressed(core_get_playercontroller(i));

      // handle keypresses
    }
  }

  // Render the UI
  rdpq_attach(display_get(), NULL);
  rdpq_clear(color_from_packed32(GAME_BACKGROUND));

  const int player_screen_coords[MAXPLAYERS][2] = {
      {SCREEN_MARGIN + 320 / 2 - 32, SCREEN_MARGIN},
      {320 - SCREEN_MARGIN - 32, 240 / 2 + SCREEN_MARGIN - 8},
      {SCREEN_MARGIN + 320 / 2 - 32, 240 - SCREEN_MARGIN - 8},
      {SCREEN_MARGIN, 240 / 2 + SCREEN_MARGIN - 8}};

  for (size_t i = 0; i < MAXPLAYERS; i++) {
    int xcur = player_screen_coords[i][0];
    int ycur = player_screen_coords[i][1];

    // Draw player names
    rdpq_set_mode_standard();
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, xcur, ycur, "Player %d",
                     i + 1);
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
