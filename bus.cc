#include "bus.h"
#include "logging.h"
#include "cartridge.h"
#include "controller.h"
#include "cpu.h"
#include "ppu.h"
#include "dma.h"
#include <cstdint>
#include <iostream>
#include <memory>

#define CONTROLLER_POLL 0x4016
#define CONTROLLER_POLL2 0x4016
#define TRIGGER_OAM 0x4014

Bus::Bus() : cpu{this}, ppu{}, dma{this, &ppu}  {}

Bus::~Bus() {}

void Bus::Cpu_write(uint16_t adr, uint8_t data) {
  // if ever a cartridge read/write operation interferes with
  // a CPU read/write, the cartridge has priority over the CPU
  if (card->cpu_write(adr, data)) {
  }
  else if (adr >= 0x0000 && adr <= 0x1FFF) { // the CPU has "mirrors until
    cpu_ram[adr & 0x07FF] = data;
  } else if (adr >= 0x2000 && adr <= 0x3FFF) {
    ppu.cpu_write(adr & 0x0007, data);
  }
  // special register of the CPU called OAMDMA
  // the data it writes to this register (val)
  // will be the top 2 bytes of where the copying
  // from OAM starts. So if data = 0x02, we copy the data
  // from 0x0200 to 0x02FF (256 bytes or entire OAM always)
  else if (adr == TRIGGER_OAM) {
    oamdma(data);
    cpu.oam = true;
  }
  else if (adr == CONTROLLER_POLL) {
    bool strobe = data & 0x01;

    controller.detect_input();
    if (!strobe && controller.prev_strobe) {
      controller.shifted_count = 0;
    }
    controller.prev_strobe = strobe;
  }
  else if (adr >= 0x6000 && adr <= 0x7FFF) {
    cartridge_ram[adr - 0x6000] = data;
  }

}

uint8_t Bus::Cpu_read(uint16_t adr, bool bReadOnly) {

  // data that can be read from cartridge as a reference
  uint8_t data{0x00};

  if (card->cpu_read(adr, data)) {
  } else if (adr >= 0x0000 && adr <= 0x1FFF) {
    data = cpu_ram[adr & 0x07FF]; // need to do the and operation because of
                                  // mirroring which just allows the NES to
                                  // access a single address from multiple
                                  // different ones (reduces hardware)
  }

  // cpu accessing PPU registers to communicate with it
  else if (adr >= 0x2000 && adr <= 0x3FFF) {
    return ppu.cpu_read(adr & 0x0007, bReadOnly);
  }
  else if (adr == CONTROLLER_POLL) {

    if (controller.prev_strobe) {
      data = controller.input.a_button;
    }
    else if (controller.shifted_count >= 8) {
      return 1;
    }
    else if (!controller.prev_strobe) {
      data = 0x01 & controller.input.reg;
      controller.shifted_count++;
      controller.input.reg >>= 1;
    }
  }
  else if (adr >= 0x6000 && adr <= 0x7FFF) {
    data = cartridge_ram[adr - 0x6000];
  }

  return data;
}

void Bus::oamdma(uint8_t addr) {
  dma.copy_256(addr);
}

void Bus::reset() {
  total_clock_count = 0;
  cpu.reset();
}

void Bus::clock() {
  cpu.clock();
  for (int i = 0; i < 3; i++) {
    if (ppu.clock()) cpu.nmi();
    total_clock_count++;
  }
}

void Bus::insert_card(std::unique_ptr<Cartridge> c) {
  card = std::move(c);
  ppu.connectCard(card.get());
}
