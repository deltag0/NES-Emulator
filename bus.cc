#include "bus.h"

#include "cpu.h"
#include <iostream>


Bus::Bus(): cpu{this} {}

Bus::~Bus() {}

void Bus::write (uint16_t adr, uint8_t data) {
    if (adr >= 0x0000 && adr <= 0xFFFF) {
        ram[adr] = data;
    }
}

uint8_t Bus::read (uint16_t adr, bool bReadOnly) const {
    if (adr >= 0x0000 && adr <= 0xFFFF) {
        return ram[adr];
    }

    return 0x00;
}
