#ifndef BUS_H
#define BUS_H

#include "cpu.h"

// uint16_t has a max value of 2^16 -1 (highest index)
#define MAX_MEMORY 1 << 16

class Bus {
    public:
        Bus();
        ~Bus();
        // CPU reads and writes from the BUS
        void write(uint16_t adr, uint8_t data);
        uint8_t read(uint16_t adr, bool bReadOnly = false) const;

        // Devices
        Cpu cpu;
        uint8_t ram[MAX_MEMORY]{0x00};
};


#endif