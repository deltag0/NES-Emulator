#ifndef MAPPER0
#define MAPPER0

#include "mapper.h"
#include <cstdint>


class Cartridge;

class Mapper_000 : public Mapper {

public:
  bool cpu_read_mapper(uint16_t adr, uint32_t &mapped_adr) override;
  bool cpu_write_mapper(uint16_t adr, uint32_t &mapped_adr, uint8_t data) override;
  bool ppu_read_mapper(uint16_t adr, uint32_t &mapped_adr) override;
  bool ppu_write_mapper(uint16_t adr, uint32_t &mapped_adr) override;

  ~Mapper_000();

private:
  Mapper_000(uint8_t nPRGBanks, uint8_t nCHRBanks);

  uint8_t nPRGBanks;
  uint8_t nCHRBanks;

  friend class Cartridge;
};

#endif
