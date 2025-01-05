#ifndef BUS_H
#define BUS_H

#include "cartridge.h"
#include "cpu.h"
#include "ppu.h"
#include <cmath>
#include <memory>

// uint16_t has a max value of 2^16 -1 (highest index)
#define MAX_MEMORY 2048

class Bus {
public:
  Bus();
  ~Bus();
  // CPU reads and writes from the BUS
  void Cpu_write(uint16_t adr, uint8_t data);
  uint8_t Cpu_read(uint16_t adr, bool bReadOnly = false);

  // Devices
  Cpu cpu;
  Ppu ppu;
  uint8_t cpu_ram[MAX_MEMORY]{0x00};
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
