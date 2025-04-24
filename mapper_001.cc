#include "mapper_001.h"
#include "mapper.h"
#include <cstdint>
#include <exception>
#include <ios>
#include <iostream>
#include <map>
#include <stdexcept>
#include <utility>

const Arangement Mapper_001::get_name_tbl_argmt() const { return argmt; }

void Mapper_001::set_program_mode() {
  prg_bank_mode = (control.prg_bank_high << 1) | control.prg_bank_low;
  if (prg_bank_mode == 0 || prg_bank_mode == 1) {
    double_block_mode = true;
  } else {
    double_block_mode = false;
  }
}

uint32_t
Mapper_001::find_prg_mapped_addr(std::pair<uint16_t, uint16_t> &switch_range,
                                 uint16_t addr) {
  uint32_t mapped_addr = 0;
  auto [low, high] = switch_range;

  prg_bank_selected = double_block_mode ? prg.reg & 0x0E : prg.reg & 0x0F;
  if (double_block_mode) {
    prg_bank_selected >>= 1;
  }

  if (!double_block_mode) {
    if (low <= addr && addr <= high) {
      mapped_addr = (addr - low) + PRG_BANK_SIZE * prg_bank_selected;
    } else if (prg_bank_mode == 2) {
      mapped_addr = addr - PRG_SWITCH1.first;
    } else if (prg_bank_mode == 3) {
      mapped_addr =
          (addr - PRG_SWITCH2.first) + (nPRGBanks - 1) * PRG_BANK_SIZE;
    } else {
      throw std::runtime_error(
          "Trying to switch individual bank in double block mode");
    }
  } else {
    mapped_addr =
        (addr - PRG_SWITCH1.first) + (PRG_BANK_SIZE * 2) * prg_bank_selected;
  }

  return mapped_addr;
}

uint32_t Mapper_001::find_chr_mapped_addr(uint16_t addr) {
  if (chr_bank_mode == 0x01) {
    if (CHR_SWITCH1.first <= addr && addr <= CHR_SWITCH1.second) {
      return (addr - CHR_SWITCH1.first) +
             (chr_bank_0.reg & 0x1F) * CHR_BANK_SIZE;
    } else if (CHR_SWITCH2.first <= addr && addr <= CHR_SWITCH2.second) {
      return (addr - CHR_SWITCH2.first) +
             (chr_bank_1.reg & 0x1F) * CHR_BANK_SIZE;
    } else {
      std::cout << "Address should not be out of the range of the switch "
                   "intervals for CHR\n";
      // TODO: throw error?
      return 0;
    }
  } else {
    return (addr - CHR_SWITCH1.first) + (chr_bank_0.reg & 0x1E) * CHR_BANK_SIZE;
  }
}

void Mapper_001::write_to_register(uint16_t adr, uint8_t val) {
  // load bank registers bit by bit using bit shifting
  // if the last bit in the value is set, we must reset the register
  if (val & 0x80) {
    control.reg |= 0x0C;
    cnt = 0;
    shift.reg = 0x00;

    chr_bank_mode = control.chr_bank;
    set_program_mode();
    find_argmt();
    return;
  }
  uint8_t bit = val & 0x01;

  switch (cnt) {
  case 0x00:
    shift.bit1 = bit;
    break;
  case 0x01:
    shift.bit2 = bit;
    break;
  case 0x02:
    shift.bit3 = bit;
    break;
  case 0x03:
    shift.bit4 = bit;
    break;
  case 0x04:
    shift.bit5 = bit;

    uint8_t dest_reg = (adr >> 13) & 0x03;

    if (dest_reg == 0) {
      control.reg = shift.reg;
      chr_bank_mode = control.chr_bank;

      set_program_mode();
      find_argmt();
    } else if (dest_reg == 1) {
      chr_bank_0.reg = shift.reg;
    } else if (dest_reg == 2) {
      chr_bank_1.reg = shift.reg;
    } else if (dest_reg == 3) {
      prg.reg = shift.reg;
    }

    cnt = 0;
    shift.reg = 0x00;
    return;
  }
  cnt++;
}

Mapper_001::Mapper_001(uint8_t nPRGBanks, uint8_t nCHRBanks, uint8_t argmt)
    : nPRGBanks{nPRGBanks}, nCHRBanks{nCHRBanks} {
  control.reg = 0x0C;

  switch (argmt) {
  case 0:
    this->argmt = Arangement::VERTICAL;
    break;
  case 1:
    this->argmt = Arangement::HORIZONTAL;
    break;
  }
}

bool Mapper_001::cpu_read_mapper(uint16_t adr, uint32_t &mapped_adr) {
  if (adr >= 0x6000 && adr <= 0x7FFF) {
    std::runtime_error("NOT implemented yet\n");
  }

  if (adr >= 0x8000) {
    std::pair<uint16_t, uint16_t> range =
        prg_bank_mode == 2 ? PRG_SWITCH2 : PRG_SWITCH1;
    mapped_adr = find_prg_mapped_addr(range, adr);

    return true;
  }

  return false;
}

bool Mapper_001::cpu_write_mapper(uint16_t adr, uint32_t &mapped_adr,
                                  uint8_t data) {
  if (adr >= 0x6000 && adr <= 0x7FFF) {
    std::runtime_error("NOT implemented yet\n");
  }

  if (prev_written && !(0x80 & data))
    return false;

  if (adr >= 0x8000) {
    if (count % 10000 == 0) {
      /* std::cout << "CONTROL " << std::hex << static_cast<uint16_t>(control.reg) */
      /*           << "\n"; */
      /* std::cout << "PRG " << std::hex << static_cast<uint16_t>(prg.reg) */
      /*           << *"\n"; */
      /* std::cout << "CHR0 " << std::hex << static_cast<uint16_t>(chr_bank_1.reg) */
      /*           << "\n"; */
      /* std::cout << "CHR1 " << std::hex << static_cast<uint16_t>(chr_bank_0.reg) */
      /*           << "\n"; */
    }
    count++;
    write_to_register(adr, data);

    std::pair<uint16_t, uint16_t> range =
        prg_bank_mode == 2 ? PRG_SWITCH2 : PRG_SWITCH1;
    mapped_adr = find_prg_mapped_addr(range, adr);

    return true;
  }

  return false;
}

bool Mapper_001::ppu_read_mapper(uint16_t adr, uint32_t &mapped_adr) {
  if (0x1FFF < adr || adr < 0x0000)
    return false;
  mapped_adr = find_chr_mapped_addr(adr);

  return true;
}

bool Mapper_001::ppu_write_mapper(uint16_t adr, uint32_t &mapped_adr) {
  return false;
}

void Mapper_001::find_argmt() {
  uint8_t new_argmt = (control.nametable_high << 1) | control.nametable_low;
  switch (new_argmt) {
  case 0:
    argmt = Arangement::LOWER;
    break;
  case 1:
    argmt = Arangement::HIGHER;
    break;
  case 2:
    argmt = Arangement::HORIZONTAL;
    break;
  case 3:
    argmt = Arangement::VERTICAL;
    break;
  }
}
