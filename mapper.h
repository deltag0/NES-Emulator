#ifndef MAPPER
#define MAPPER

#include <cstdint>
#include <utility>
#define SWITCH1 std::pair<uint16_t, uint16_t>{0x8000, 0xBFFF}
#define SWITCH2 std::pair<uint16_t, uint16_t>{0xC000, 0xFFFF}
#define BANK_SIZE 0x4000

// Parent class for the different kind of mappers on the NES
class Mapper {
  public:
  virtual bool cpu_read_mapper(uint16_t adr, uint16_t &mapped_adr);
  virtual bool cpu_write_mapper(uint16_t adr, uint32_t &mapped_adr, uint8_t data);
  virtual bool ppu_read_mapper(uint16_t adr, uint32_t &mapped_adr);
  virtual bool ppu_write_mapper(uint16_t adr, uint32_t &mapped_adr);

  virtual ~Mapper() = 0;
  
};

// Mapper (000) since there are multiple kind of mappers
// has 2 types -> 32KB or 16KB
// When CPU communicates with cartridge, its address is mapped to the correct
// address on the cartridge such that it starts at the right place We must pick
// the correct value to mask them for the 32KB, we mask the raw CPU address by
// using 0x7FFF -> 32KB and 16KB with 0x3FFF -> 16KB map (we count 0, so we
// aren't off by one)

#endif
