#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include "bus.h"
#include "cartridge.h"
#include "cpu.h"
#define OLC_PGE_APPLICATION
#define OLC_ENABLE_EXPERIMENTATION
#include "olcPixelGameEngine.h"

class Debugger : public olc::PixelGameEngine {
public:
  Debugger() { sAppName = "Debugger"; }

  Bus nes; // Bus is basically the NES
  std::map<uint16_t, std::string> mapAsm;

  // if run_emulation is true, debugging mode is off
  bool run_emulation = false;
  float fResidualTime = 0.0f;

  std::string hex(uint32_t n, uint8_t d) {
    std::string s(d, '0');
    for (int i = d - 1; i >= 0; i--, n >>= 4)
      s[i] = "0123456789ABCDEF"[n & 0xF];
    return s;
  };

  void DrawRam(int x, int y, uint16_t nAddr, int nRows, int nColumns) {
    int nRamX = x, nRamY = y;
    for (int row = 0; row < nRows; row++) {
      std::string sOffset = "$" + hex(nAddr, 4) + ":";
      for (int col = 0; col < nColumns; col++) {
        sOffset += " " + hex(nes.Cpu_read(nAddr, true), 2);
        nAddr += 1;
      }
      DrawString(nRamX, nRamY, sOffset);
      nRamY += 10;
    }
  }

  void DrawCpu(int x, int y) {
    std::string status = "STATUS: ";
    DrawString(x, y, "STATUS:", olc::WHITE);
    DrawString(x + 64, y, "N",
               nes.cpu.status & FLAGS::N ? olc::GREEN : olc::RED);
    DrawString(x + 80, y, "V",
               nes.cpu.status & FLAGS::V ? olc::GREEN : olc::RED);
    DrawString(x + 96, y, "-",
               nes.cpu.status & FLAGS::U ? olc::GREEN : olc::RED);
    DrawString(x + 112, y, "B",
               nes.cpu.status & FLAGS::B ? olc::GREEN : olc::RED);
    DrawString(x + 128, y, "D",
               nes.cpu.status & FLAGS::D ? olc::GREEN : olc::RED);
    DrawString(x + 144, y, "I",
               nes.cpu.status & FLAGS::I ? olc::GREEN : olc::RED);
    DrawString(x + 160, y, "Z",
               nes.cpu.status & FLAGS::Z ? olc::GREEN : olc::RED);
    DrawString(x + 178, y, "C",
               nes.cpu.status & FLAGS::C ? olc::GREEN : olc::RED);
    DrawString(x, y + 10, "PC: $" + hex(nes.cpu.PC, 4));
    DrawString(x, y + 20,
               "A: $" + hex(nes.cpu.accumulator, 2) + "  [" +
                   std::to_string(nes.cpu.accumulator) + "]");
    DrawString(x, y + 30,
               "X: $" + hex(nes.cpu.x, 2) + "  [" + std::to_string(nes.cpu.x) +
                   "]");
    DrawString(x, y + 40,
               "Y: $" + hex(nes.cpu.y, 2) + "  [" + std::to_string(nes.cpu.y) +
                   "]");
    DrawString(x, y + 50, "Stack P: $" + hex(nes.cpu.stack_pointer, 4));
  }

  void DrawCode(int x, int y, int nLines) {
    auto it_a = mapAsm.find(nes.cpu.PC);
    int nLineY = (nLines >> 1) * 10 + y;
    if (it_a != mapAsm.end()) {
      DrawString(x, nLineY, (*it_a).second, olc::CYAN);
      while (nLineY < (nLines * 10) + y) {
        nLineY += 10;
        if (++it_a != mapAsm.end()) {
          DrawString(x, nLineY, (*it_a).second);
        }
      }
    }

    it_a = mapAsm.find(nes.cpu.PC);
    nLineY = (nLines >> 1) * 10 + y;
    if (it_a != mapAsm.end()) {
      while (nLineY > y) {
        nLineY -= 10;
        if (--it_a != mapAsm.end()) {
          DrawString(x, nLineY, (*it_a).second);
        }
      }
    }
  }

  bool OnUserCreate() {
    auto card = std::make_unique<Cartridge>("duck.nes");
    nes.insert_card(std::move(card));

    mapAsm = nes.cpu.disassemble(0x0000, 0xFFFF);

    nes.reset();

    return true;
  }

  bool OnUserUpdate(float fElapsedTime) {
    Clear(olc::DARK_BLUE);
    // if emulation is playing without debug mode
    if (run_emulation) {
      // run the emulation, clocking as fast as possible\
      // and updating frames as fast as possible
      // ResidualTime is reset at each frame, and
      // if ResidualTime > 0, we still need time until
      // displaying the next frame
      if (fResidualTime > 0.0f)
        fResidualTime -= fElapsedTime;
      else {
        fResidualTime =
            (1.0f / 60.0f) - fElapsedTime; // Substract ElapsedTime for accuracy
        do {

          nes.clock();
        } while (!nes.ppu.frame_complete);
        nes.ppu.frame_complete = false;
      }
    }

    else {
      if (GetKey(olc::Key::C).bPressed) {
        // clock the NES until we have completed an instruction
        do {
          nes.clock();
        } while (!nes.cpu.complete());

        // set up CPU for next instruction
        // because its clock is slower, it might have more cycles
        do {
          nes.clock();
        } while (nes.cpu.complete());
      }
      if (GetKey(olc::Key::F).bPressed) {
        // complete a frame
        do {
          nes.clock();
        } while (!nes.ppu.frame_complete);

        // set up CPU for next instruction
        // because its clock is slower, it might have more cycles
        do {
          nes.clock();
        } while (nes.cpu.complete());
      }
    }

    if (GetKey(olc::Key::R).bPressed)
      nes.reset();

    if (GetKey(olc::Key::SPACE).bPressed)
      run_emulation = !run_emulation;

    DrawCpu(516, 2);

    DrawCode(516, 76, 26);

    DrawSprite(0, 0, nes.ppu.getScreen(), 2);
    return true;
  }
};

int main() {
  Debugger demo;
  demo.Construct(680, 480, 2, 2);
  demo.Start();
  return 0;
}
