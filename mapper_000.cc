#include "mapper_000.h"
#include <cstdint>
#include <iostream>

Mapper_000::Mapper_000(uint8_t prg_banks, uint8_t chr_banks)
    : nPRGBanks(prg_banks), nCHRBanks(chr_banks) {}

Mapper_000::~Mapper_000() {}

// checks if the CPU can read from the PRG ROM on the cartridge, and maps it
// to the correct address, masking it
bool Mapper_000::cpu_read_mapper(uint16_t adr, uint32_t &mapped_adr) {
  if (adr >= 0x8000 && adr <= 0xFFFF) {
    mapped_adr &= (nPRGBanks > 1 ? 0x7FFF : 0x3FFF);
    return true;
  }
  return false;
}

bool Mapper_000::cpu_write_mapper(uint16_t adr, uint32_t &mapped_adr) {
  if (adr >= 0x8000 && adr <= 0xFFFF) {
    mapped_adr &= (nPRGBanks > 1 ? 0x7FFF : 0x3FFF);
    return true;
  }
  return false;
}

// Checks if the PPU can read from the CHR ROM on the cartridge, and maps it
// to the correct address
bool Mapper_000::ppu_read_mapper(uint16_t adr, uint32_t &mapped_adr) {
  if (adr >= 0x0000 && adr <= 0x1FFF) {
    mapped_adr = adr;
    return true;
  }
  return false;
}

// PPU shouldn't be able to write to the CHR ROM, but may need to add changes
// if there is CHR RAM
bool Mapper_000::ppu_write_mapper(uint16_t adr, uint32_t &mapped_adr) {
  return false;
}
