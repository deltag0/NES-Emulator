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
    std::cerr << "No joystick found." << std::endl;
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
