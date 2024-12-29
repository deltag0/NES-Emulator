#include "cartridge.h"
#include "mapper.h"
#include "mapper_000.h"
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#define PRG_SIZE 16 * 1024
#define CHR_SIZE 8 * 1024

Cartridge::~Cartridge() {}

Cartridge::Cartridge(const std::string &file) {
  std::ifstream ifs{file, std::ios::binary};
  sHeader header;

  if (!ifs) {
    throw std::runtime_error("File does not exist.\n");
  }

  ifs >> header.name[0] >> header.name[1] >> header.name[2] >> header.name[3];
  ifs >> header.prg_rom_chunks >> header.chr_rom_chunks >> header.mapper1 >>
      header.mapper2 >> header.prg_ram_size >> header.tv_system1 >>
      header.tv_system2 >> header.unused[0] >> header.unused[1] >>
      header.unused[2] >> header.unused[3] >> header.unused[4];

  if (header.mapper1 & 0x04)
    ifs.seekg(512, std::ios::cur);

  nMapperID = ((header.mapper2 >> 4) << 4) | (header.mapper1 >> 4);

  // TEMP ADDITION
  // NOTE: the hardcoded elements are all temporary
  nMapperID = 0;
  uint8_t file_type = 1;
  header.chr_rom_chunks = 2;

  if (file_type == 0) {
    // TODO:
  }

  else if (file_type == 1) {
    // resizing program and character memory to be right size,
    // and reading the data from our file
    nPRGBanks = header.prg_rom_chunks;
    nCHRBanks = header.chr_rom_chunks;
    vPRGMemory.resize(nPRGBanks * PRG_SIZE);
    vCHRMemory.resize(nCHRBanks * CHR_SIZE);
    for (int i = 0; i < nPRGBanks * PRG_SIZE; i++) {
      ifs >> vPRGMemory[i];
    }
    for (int i = 0; i < nCHRBanks * CHR_SIZE; i++) {
      ifs >> vCHRMemory[i];
    }
  }

  if (file_type == 2) {
    // TODO:
  }
  switch (nMapperID) {
  case 0:
    std::cout << "HELLO\n";
    mapper = std::move(
        std::unique_ptr<Mapper_000>{new Mapper_000{nPRGBanks, nCHRBanks}});
    break;
  case 1:
    // TODO:
    break;
  case 2:
    // TODO:
    break;
  }
  assert(mapper.get());
}

// cpu_read will read from the cartridge program memory using the mapper
bool Cartridge::cpu_read(uint16_t adr, uint8_t &data) {

  uint32_t mapped_adr{0};
  if (mapper->cpu_read_mapper(adr, mapped_adr)) {
    data = vPRGMemory[mapped_adr];
    return true;
  }

  return false;
}

// cpu_write will write to the cartridge program memory using the mapper
bool Cartridge::cpu_write(uint16_t adr, uint8_t data) {
  uint32_t mapped_adr{0};
  if (mapper->cpu_write_mapper(adr, mapped_adr)) {
    vPRGMemory[mapped_adr] = data;
    return true;
  }
  return false;
}

// ppu_read will read from the cartridge character memory using the mapper
bool Cartridge::ppu_read(uint16_t adr, uint8_t &data) {
  uint32_t mapped_adr{0};
  if (mapper->ppu_read_mapper(adr, mapped_adr)) {
    data = vCHRMemory[mapped_adr];
    return 1;
  }
  return 0;
}

bool Cartridge::ppu_write(uint16_t adr, uint8_t val) {
  uint32_t mapped_adr{0};
  if (mapper->ppu_write_mapper(adr, mapped_adr)) {
    return 1;
  }
  return 0;
}
