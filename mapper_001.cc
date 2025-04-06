#include "mapper_001.h"
#include "mapper.h"
#include <cstdint>
#include <exception>
#include <map>
#include <utility>
#include <iostream>

// TODO: I think I previously finished the mapping for CPU, but Not tested yet, so do that ya dumb sonofabitch, then PPU mapping and nametables

void Mapper_001::set_program_mode() {
  prg_bank_mode = (control.prg_bank_high << 1) | control.prg_bank_low;
  if (prg_bank_mode == 0 || prg_bank_mode == 1) {
    double_block_mode = true;
  }
}

uint32_t
Mapper_001::find_prg_mapped_addr(std::pair<uint16_t, uint16_t> &switch_range,
                             uint16_t addr) {
  uint32_t mapped_addr = 0;
  auto [low, high] = switch_range;
  if (low <= addr && addr <= high) {
    mapped_addr = (low & addr) + PRG_BANK_SIZE * prg_bank_selected;
  } else if (prg_bank_mode == 2) {
    mapped_addr = low & addr;
  } else if (prg_bank_mode == 3) {
    mapped_addr = (low & addr) + nPRGBanks * PRG_BANK_SIZE;
  }
  return mapped_addr;
}

uint32_t Mapper_001::find_chr_mapped_addr(uint16_t addr) {
  if (chr_bank_mode == 0x01) {
    if (CHR_SWITCH1.first <= addr && addr <= CHR_SWITCH1.second) {
      return (addr & CHR_SWITCH1.first) + (chr_bank_0.reg & 0x1F) * CHR_BANK_SIZE;
    }
    else if (CHR_SWITCH2.first <= addr && addr <= CHR_SWITCH2.second) {
      return (addr & CHR_SWITCH2.first) + (chr_bank_1.reg & 0x1F) * CHR_BANK_SIZE;
    }
    else {
      std::cout << "Address should not be out of the range of the switch intervals for CHR\n";
    }
  }
  else {
    return (addr & CHR_SWITCH1.first) + (chr_bank_0.reg & 0x1E) * CHR_BANK_SIZE;
  }
}

void Mapper_001::write_to_register(BANK bank, uint8_t val) {
  // load bank registers bit by bit using bit shifting
  // if the last bit in the value is set, we must reset the register
  if (val & 0x80) {
    bank.reg = 0x0C;
    bank.unused = 0;
    return;
  }
  uint8_t bit = val & 0x01;

  switch (bank.unused) {
  case 0x01:
    bank.bit1 = bit;
    break;
  case 0x02:
    bank.bit2 = bit;
    break;
  case 0x03:
    bank.bit3 = bit;
    break;
  case 0x04:
    bank.bit4 = bit;
    break;
  case 0x05:
    bank.bit5 = bit;
    bank.unused = 0;
    return;
  }
  bank.unused++;
}

void Mapper_001::write_to_control_register(uint8_t value) {
  // If bit 7 is set, reset the shift register.
  if (value & 0x80) {
    // The MMC1 resets the control register to 0x0C.
    control.reg = 0x0C;
    control.unused = 0;
    return;
  }
  // Extract only the LSB (the only bit that matters for shifting).
  uint8_t bit = value & 0x01;

  // Use the 'unused' field as a counter for how many bits have been written.
  switch (control.unused) {
  case 0:
    control.nametable_low = bit;
    break;
  case 1:
    control.nametable_high = bit;
    break;
  case 2: {
    control.prg_bank_low = bit;
    set_program_mode();
    break;
  }
  case 3:
    control.prg_bank_high = bit;
    set_program_mode();
    break;
  case 4:
    control.chr_bank = bit;
    chr_bank_mode = control.chr_bank;
    // Reset the counter after the fifth bit is written.
    control.unused = 0;
    return;
  }
  control.unused++;
}

Mapper_001::Mapper_001(uint8_t nPRGBanks, uint8_t nCHRBanks)
    : nPRGBanks{nPRGBanks}, nCHRBanks{nCHRBanks} {}
Mapper_001::~Mapper_001() {}

bool Mapper_001::cpu_read_mapper(uint16_t adr, uint16_t &mapped_adr) {
  if (0xFFFF < adr || adr < 0x8000) return false;
  if (!double_block_mode) {
    std::pair<uint16_t, uint16_t> range =
        prg_bank_mode == 2 ? PRG_SWITCH2 : PRG_SWITCH1;
    mapped_adr = find_prg_mapped_addr(range, adr);
  }
  else {
    mapped_adr = (adr & PRG_SWITCH1.first) + PRG_BANK_SIZE * prg_bank_selected;
  }
  return true;
}


bool Mapper_001::cpu_write_mapper(uint16_t adr, uint32_t &mapped_adr,
                                  uint8_t data) {
  bool mapper_range = false;

  if (0x8000 <= adr && adr <= 0x9FFF) {
    write_to_control_register(data);
    mapper_range = true;
  } else if (0xA000 <= adr && adr <= 0xBFFF) {
    write_to_register(chr_bank_0, data);
    mapper_range = true;
  } else if (0xC000 <= adr && adr <= 0xDFFF) {
    write_to_register(chr_bank_1, data);
    mapper_range = true;
  } else if (0xE000 <= adr && adr <= 0xFFFF) {
    write_to_register(prg, data);
    prg_bank_selected = double_block_mode == true ? prg.reg & 0x0E : prg.reg & 0x0F;
    mapper_range = true;
  }

  if (!double_block_mode) {
    std::pair<uint16_t, uint16_t> range =
        prg_bank_mode == 2 ? PRG_SWITCH2 : PRG_SWITCH1;
    mapped_adr = find_prg_mapped_addr(range, adr);
  }
  else {
    mapped_adr = (adr & PRG_SWITCH1.first) + PRG_BANK_SIZE * prg_bank_selected;
  }

  return mapper_range;
}

bool Mapper_001::ppu_read_mapper(uint16_t adr, uint32_t &mapped_adr) {
  if (0x1FFF < adr || adr < 0x0000) return false;
  mapped_adr = find_chr_mapped_addr(adr);


  return true;
}

bool Mapper_001::ppu_write_mapper(uint16_t adr, uint32_t &mapped_adr) {
  return false;
}


