#ifndef MAPPER_1
#define MAPPER_1
#include "cartridge.h"
#include "mapper.h"
#include <cstdint>

class Mapper_001 : public Mapper {
public:
  bool cpu_read_mapper(uint16_t adr, uint16_t &mapped_adr) override;
  bool cpu_write_mapper(uint16_t adr, uint32_t &mapped_adr) override;
  bool ppu_read_mapper(uint16_t adr, uint32_t &mapped_adr) override;
  bool ppu_write_mapper(uint16_t adr, uint32_t &mapped_adr) override;
  ~Mapper_001();
  private:
  Mapper_001(uint8_t nPRGBanks, uint8_t nCHRBanks);
  uint8_t nPRGBanks;
  uint8_t nCHRBanks;
  friend class Cartridge;
};
#endif
