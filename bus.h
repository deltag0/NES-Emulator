#ifndef BUS_H
#define BUS_H

#include "cartridge.h"
#include "controller.h"
#include "cpu.h"
#include "ppu.h"
#include "dma.h"
#include <cstdint>
#include <fstream>
#include <cmath>
#include <memory>
#include <fstream>

// uint16_t has a max value of 2^16 -1 (highest index)
#define MAX_MEMORY 2048
#define MAX_CARTRIDGE_RAM 0x2000

class Bus {
public:
  std::ofstream debug_out{"debug.txt"};
  Bus();
  ~Bus();
  // CPU reads and writes from the BUS
  void Cpu_write(uint16_t adr, uint8_t data);
  uint8_t Cpu_read(uint16_t adr, bool bReadOnly = false);
  void oamdma(uint8_t adr);

  // Devices
  Cpu cpu;
  Ppu ppu;
  Controller controller;
  Dma dma;
  uint8_t cpu_ram[MAX_MEMORY]{0x00};
  uint8_t cartridge_ram[MAX_CARTRIDGE_RAM]{0x00};
  std::unique_ptr<Cartridge> card;
  // Interface
  // this function was with a shared_ptr reference, but I don't really see the
  // need, so I've kept it like this for now
  void insert_card(const std::unique_ptr<Cartridge> card);
  void reset();
  void clock();

private:
  int total_clock_count{0};
};

#endif
