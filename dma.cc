#include "dma.h"
#include "bus.h"
#include "ppu.h"
#include <cstdint>

Dma::Dma(Bus *bus, Ppu *ppu):  bus{bus}, ppu{ppu} {}

void Dma::copy_256(uint8_t adr) {
  // hardcoded 256, cuz, well, it only ever copies 256 bytes
  uint16_t copy_adr = adr << 8;
  for (int i = 0; i < 256; i++) {
    ppu->oam[i] = bus->cpu_ram[copy_adr];
    copy_adr++;
  }
}
