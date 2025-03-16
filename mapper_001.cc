#include "mapper_001.h"
#include <cstdint>

Mapper_001::Mapper_001(uint8_t nPRGBanks, uint8_t nCHRBanks): nPRGBanks{nPRGBanks}, nCHRBanks{nCHRBanks} {}
Mapper_001::~Mapper_001() {}

bool Mapper_001::cpu_read_mapper(uint16_t adr, uint16_t &mapped_adr) {

}

bool Mapper_001::cpu_write_mapper(uint16_t adr, uint32_t &mapped_adr) {

}

bool Mapper_001::ppu_read_mapper(uint16_t adr, uint32_t &mapped_adr) {

}

bool Mapper_001::ppu_write_mapper(uint16_t adr, uint32_t &mapped_adr) {

}
