#ifndef CARD_H
#define CARD_H

#include "mapper.h"
#include "mapper_000.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

struct sHeader {
  char name[4];
  uint8_t prg_rom_chunks;
  uint8_t chr_rom_chunks;
  uint8_t mapper1;
  uint8_t mapper2;
  uint8_t prg_ram_size;
  uint8_t tv_system1;
  uint8_t tv_system2;
  char unused[5];
};

class Cartridge {

public:
  Cartridge(const std::string &file);
  ~Cartridge();

  // we need all these read an write functions because in the NES, the cartridge
  // is connected to both CPU and PPU. CPU and PPU can read and write to the
  // cartridge
  bool cpu_read(uint16_t adr, uint8_t &data);
  bool cpu_write(uint16_t adr, uint8_t data);
  bool ppu_read(uint16_t adr, uint8_t &data);
  bool ppu_write(uint16_t adr, uint8_t val);

  // NOTE: vPRGMemory should be private
  std::vector<uint8_t> vPRGMemory; // 16 KB

  // Cartridge is connected to CPU and PPU through a NOTE: mapper
  // The mapper is set up by the CPU and
private:
  std::vector<uint8_t> vCHRMemory; // 8KB

  std::unique_ptr<Mapper> mapper;
  uint8_t nMapperID = 0;
  uint8_t nPRGMemoryID = 0;
  uint8_t nPRGBanks = 0;
  uint8_t nCHRBanks = 0;
};

#endif
