#ifndef MAPPER_1
#define MAPPER_1
#include "mapper.h"
#include <cstdint>

class Cartridge; // Forward declaration
class TestMapper_001;

class Mapper_001 : public Mapper {
public:
  bool cpu_read_mapper(uint16_t adr, uint32_t &mapped_adr) override;
  bool cpu_write_mapper(uint16_t adr, uint32_t &mapped_adr,
                        uint8_t data) override;
  bool ppu_read_mapper(uint16_t adr, uint32_t &mapped_adr) override;
  bool ppu_write_mapper(uint16_t adr, uint32_t &mapped_adr) override;
  const Arangement get_name_tbl_argmt() const override;
  virtual ~Mapper_001() {}

  // NOTE: this is temporary, and just for testing
  // It should be private
  Mapper_001(uint8_t nPRGBanks, uint8_t nCHRBanks, uint8_t argmt);
  union BANK {
    uint8_t reg;
    struct {
      uint8_t bit1 : 1;
      uint8_t bit2 : 1;
      uint8_t bit3 : 1;
      uint8_t bit4 : 1;
      uint8_t bit5 : 1;
      uint8_t unused : 3;
    };
  } chr_bank_0;

  BANK prg;

  union CTRL {
    uint8_t reg;
    struct {
      uint8_t nametable_low : 1;
      uint8_t nametable_high : 1;
      uint8_t prg_bank_low : 1;
      uint8_t prg_bank_high : 1;
      uint8_t chr_bank : 1;
      uint8_t unused : 3;
    };
  } control;

private:
  Arangement argmt;
  uint8_t nPRGBanks;
  uint8_t nCHRBanks;

  // The unused bits for CTRL and BANK are used for counting how many
  // writes have occured for each area of memory. When 5 writes occur,
  // it is wrapped back to 0, indicating the first bit will be overwritten


  BANK chr_bank_1;

  BANK shift;
  int cnt = 0;

  int count;
  bool prev_written = false;

  // Double block mode is just if we're using the 2 blocks of memory in the 32KB
  // of the RAM (i.e. bank mode set to 0/1)
  bool double_block_mode = false;
  // From 0-3
  uint8_t prg_bank_mode = 3;
  // 32 max banks possible, but for now, we'll have less
  uint8_t prg_bank_selected = 0x00;

  // 1 (switch 2 seperate 4 KB banks) or 0 (switch 1 8 KB bank at a time)
  uint8_t chr_bank_mode = 0x00;

  void write_to_register(uint16_t adr, uint8_t bit);
  void write_to_control_register(uint8_t value);
  void set_program_mode();
  uint32_t find_prg_mapped_addr(std::pair<uint16_t, uint16_t> &switch_range,
                                uint16_t addr);
  uint32_t find_chr_mapped_addr(uint16_t addr);
  void find_argmt();

  friend class Cartridge;
  friend class TestMapper_001;
  friend class Cpu;
};
#endif
