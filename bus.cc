#include "bus.h"

#include "cartridge.h"
#include "cpu.h"
#include "ppu.h"
#include <cstdint>
#include <iostream>
#include <memory>

Bus::Bus() : cpu{this}, ppu() {}

Bus::~Bus() {}

void Bus::Cpu_write(uint16_t adr, uint8_t data) {
  // if ever a cartridge read/write operation interferes with
  // a CPU read/write, the cartridge has priority over the CPU
  if (card->cpu_write(adr, data)) {

  }
  else if (adr >= 0x0000 && adr <= 0x1FFF) { // the CPU has "mirrors until 0x1FF
    cpu_ram[adr & 0x07FF] = data;
  } else if (adr >= 0x2000 && adr <= 0x3FFF) {
    ppu.cpu_write(adr & 0x0007, data);
  }
}

uint8_t Bus::Cpu_read(uint16_t adr, bool bReadOnly) {

  // data that can be read from cartridge as a reference
  uint8_t data{0x00};

  if (card->cpu_read(adr, data)) {
  }
  else if (adr >= 0x0000 && adr <= 0x1FFF) {
    data = cpu_ram[adr & 0x07FF]; // need to do the and operation because of
                                  // mirroring which just allows the NES to
                                  // access a single address from multiple
                                  // different ones (reduces hardware)
  }

  else if (adr >= 0x2000 && adr <= 0x3FFF) {
    return ppu.cpu_read(adr & 0x0007, bReadOnly);
  }
  return data;
}

void Bus::reset() {
  total_clock_count = 0;
  cpu.reset();
}

void Bus::clock() {
  ppu.clock();
  if (total_clock_count % 3 == 0) {
    cpu.clock();
  }
  total_clock_count++;
}

void Bus::insert_card(std::unique_ptr<Cartridge> c) {
  card = std::move(c);
  ppu.connectCard(card.get());
}
