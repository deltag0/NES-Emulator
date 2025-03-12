#include <SDL2/SDL.h>
#include <cstdint>
#include <iostream>

#include "controller.h"

void Controller::detect_input() {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0) {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    return;
  }

  SDL_Joystick *joystick = SDL_JoystickOpen(0);
  if (!joystick) {
    SDL_Quit();
    return;
  }

  SDL_JoystickUpdate(); // Ensure input is updated

  for (int i = 0; i < SDL_JoystickNumButtons(joystick); i++) {
    uint8_t state = SDL_JoystickGetButton(joystick, i);
    switch (i) {
    case 0:
      input.a_button = state;
      break;
    case 1:
      input.b_button = state;
      break;
    case 4:
      input.select = state;
      break;
    case 6:
      input.start = state;
      break;
    case 11:
      input.up = state;
      break;
    case 12:
      input.down = state;
      break;
    case 13:
      input.left = state;
      break;
    case 14:
      input.right = state;
      break;
    }
  }
}
void Controller::detect_input_keyboard() {
  // Initialize SDL for video (keyboard input)
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    return;
  }

  // Pump events so the keyboard state is updated
  SDL_PumpEvents();

  // Get the current keyboard state array
  const Uint8* keyState = SDL_GetKeyboardState(NULL);

  // Map the keys to your input structure
  // Use WASD for directional input:
  input.up       = keyState[SDL_SCANCODE_W] ? 1 : 0;
  input.down     = keyState[SDL_SCANCODE_S] ? 1 : 0;
  input.left     = keyState[SDL_SCANCODE_A] ? 1 : 0;
  input.right    = keyState[SDL_SCANCODE_D] ? 1 : 0;

  // Map Spacebar to the A button (typically jump)
  input.a_button = keyState[SDL_SCANCODE_SPACE] ? 1 : 0;

  // Map the [ key to the B button
  input.b_button = keyState[SDL_SCANCODE_LEFTBRACKET] ? 1 : 0;

  // Map Enter to the Start button
  input.start    = keyState[SDL_SCANCODE_RETURN] ? 1 : 0;

  // Optionally, map another key for Select if desired:
  // input.select = keyState[SDL_SCANCODE_X] ? 1 : 0;
}
