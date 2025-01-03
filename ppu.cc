
#include "ppu.h"
#include "cartridge.h"
#include <cstdint>
#include <iostream>
#include <memory>

Ppu::Ppu() : sprNameTable(2), sprPatternTable(2), frame_complete(false) {
  // color palette for the screen
  palScreen[0x00] = olc::Pixel(84, 84, 84);
  palScreen[0x01] = olc::Pixel(0, 30, 116);
  palScreen[0x02] = olc::Pixel(8, 16, 144);
  palScreen[0x03] = olc::Pixel(48, 0, 136);
  palScreen[0x04] = olc::Pixel(68, 0, 100);
  palScreen[0x05] = olc::Pixel(92, 0, 48);
  palScreen[0x06] = olc::Pixel(84, 4, 0);
  palScreen[0x07] = olc::Pixel(60, 24, 0);
  palScreen[0x08] = olc::Pixel(32, 42, 0);
  palScreen[0x09] = olc::Pixel(8, 58, 0);
  palScreen[0x0A] = olc::Pixel(0, 64, 0);
  palScreen[0x0B] = olc::Pixel(0, 60, 0);
  palScreen[0x0C] = olc::Pixel(0, 50, 60);
  palScreen[0x0D] = olc::Pixel(0, 0, 0);
  palScreen[0x0E] = olc::Pixel(0, 0, 0);
  palScreen[0x0F] = olc::Pixel(0, 0, 0);

  palScreen[0x10] = olc::Pixel(152, 150, 152);
  palScreen[0x11] = olc::Pixel(8, 76, 196);
  palScreen[0x12] = olc::Pixel(48, 50, 236);
  palScreen[0x13] = olc::Pixel(92, 30, 228);
  palScreen[0x14] = olc::Pixel(136, 20, 176);
  palScreen[0x15] = olc::Pixel(160, 20, 100);
  palScreen[0x16] = olc::Pixel(152, 34, 32);
  palScreen[0x17] = olc::Pixel(120, 60, 0);
  palScreen[0x18] = olc::Pixel(84, 90, 0);
  palScreen[0x19] = olc::Pixel(40, 114, 0);
  palScreen[0x1A] = olc::Pixel(8, 124, 0);
  palScreen[0x1B] = olc::Pixel(0, 118, 40);
  palScreen[0x1C] = olc::Pixel(0, 102, 120);
  palScreen[0x1D] = olc::Pixel(0, 0, 0);
  palScreen[0x1E] = olc::Pixel(0, 0, 0);
  palScreen[0x1F] = olc::Pixel(0, 0, 0);

  palScreen[0x20] = olc::Pixel(236, 238, 236);
  palScreen[0x21] = olc::Pixel(76, 154, 236);
  palScreen[0x22] = olc::Pixel(120, 124, 236);
  palScreen[0x23] = olc::Pixel(176, 98, 236);
  palScreen[0x24] = olc::Pixel(228, 84, 236);
  palScreen[0x25] = olc::Pixel(236, 88, 180);
  palScreen[0x26] = olc::Pixel(236, 106, 100);
  palScreen[0x27] = olc::Pixel(212, 136, 32);
  palScreen[0x28] = olc::Pixel(160, 170, 0);
  palScreen[0x29] = olc::Pixel(116, 196, 0);
  palScreen[0x2A] = olc::Pixel(76, 208, 32);
  palScreen[0x2B] = olc::Pixel(56, 204, 108);
  palScreen[0x2C] = olc::Pixel(56, 180, 204);
  palScreen[0x2D] = olc::Pixel(60, 60, 60);
  palScreen[0x2E] = olc::Pixel(0, 0, 0);
  palScreen[0x2F] = olc::Pixel(0, 0, 0);

  palScreen[0x30] = olc::Pixel(236, 238, 236);
  palScreen[0x31] = olc::Pixel(168, 204, 236);
  palScreen[0x32] = olc::Pixel(188, 188, 236);
  palScreen[0x33] = olc::Pixel(212, 178, 236);
  palScreen[0x34] = olc::Pixel(236, 174, 236);
  palScreen[0x35] = olc::Pixel(236, 174, 212);
  palScreen[0x36] = olc::Pixel(236, 180, 176);
  palScreen[0x37] = olc::Pixel(228, 196, 144);
  palScreen[0x38] = olc::Pixel(204, 210, 120);
  palScreen[0x39] = olc::Pixel(180, 222, 120);
  palScreen[0x3A] = olc::Pixel(168, 226, 144);
  palScreen[0x3B] = olc::Pixel(152, 226, 180);
  palScreen[0x3C] = olc::Pixel(160, 214, 228);
  palScreen[0x3D] = olc::Pixel(160, 162, 160);
  palScreen[0x3E] = olc::Pixel(0, 0, 0);
  palScreen[0x3F] = olc::Pixel(0, 0, 0);

  sprScreen = std::make_unique<olc::Sprite>(256, 240);
  sprNameTable[0] = std::make_unique<olc::Sprite>(256, 240);
  sprNameTable[1] = std::make_unique<olc::Sprite>(256, 240);
  sprPatternTable[0] = std::make_unique<olc::Sprite>(128, 128);
  sprPatternTable[1] = std::make_unique<olc::Sprite>(128, 128);
  control.reg = 0x00;
  mask.reg = 0x00;
  status.reg = 0x00;
}
Ppu::~Ppu() {}

// cpu writing to ppu
void Ppu::cpu_write(uint16_t adr, uint8_t val) {
  switch (adr) {
  case 0x0000: // control
    control.reg = val;
    break;
  case 0x0001: // mask
    mask.reg = val;
    break;
  case 0x0002: // Status
    break;
  case 0x0003: // OAM addr
    break;
  case 0x0004: // OAM data
    break;
  case 0x0005: // Scroll
    break;
  case 0x0006: // PPU addr
               // For the addr register of the ppu,
               // the ppu will read the low and high byte one at a time
               // first it reads the low byte then it reads the high byte
               // and it stores it into the ppu addr
    if (latched == 0) {
      // set the high byte first
      ppu_addr = (ppu_addr & 0x00FF) | val;
      latched = 0;
    } else {
      // set the low byte second
      ppu_addr = (ppu_addr & 0xFF00) | val;
      latched = 1;
    }
    break;
  case 0x0007: // PPU data
               //
    ppu_write(ppu_addr, val);
    break;
  }
}

// cpu reading from ppu
uint8_t Ppu::cpu_read(uint16_t adr, bool read) {
  uint8_t data;
  switch (adr) {
  case 0x0000:
    break;
  case 0x0001:
    break;
  case 0x0002:
    latched = 0x00;
    status.vblank = 0x00;
    data = status.reg;
    break;
  case 0x0003:
    break;
  case 0x0004:
    break;
  case 0x0005:
    break;
  case 0x0006:
    break;
  case 0x0007:
    // data register returns data stored
    // AFTER new read.
    data = ppu_data_buffer;
    ppu_data_buffer = ppu_read(adr);

    if (ppu_addr >= 0x3F00 && ppu_addr <= 0x3FFF) {
      data = ppu_data_buffer;
    }
    break;
  }
  return data;
}

void Ppu::ppu_write(uint16_t adr, uint8_t val) {
  adr &= 0x3FFF;

  if (card->ppu_write(adr, val)) {
  } else if (adr >= 0x0000 && adr <= 0x1FFF) {
    // this is the address range for the
    // pattern tables
    // one starts from 0x0000 and the other from 0x1000
    // that bit indicates which pattern table to be accessed
    // background or sprite
    npatterns[adr >> 12][adr & 0x0FFF] = val;
  } else if (adr >= 0x2000 && adr <= 0x2FFF) {
  } else if (adr >= 0x3F00 && adr <= 0x3FFF) {
    // This address space is for the palettes
    // we AND the address because it has mirrors
    // on 0x3F
    adr &= 0x00FF;
    // access through mirrored indexes
    if (adr == 0x0010)
      adr = 0x0000;
    else if (adr == 0x0014)
      adr = 0x0004;
    else if (adr == 0x0018)
      adr = 0x0008;
    else if (adr == 0x001C)
      adr = 0x000C;
    palettes[adr] = val;
  }
}

uint8_t Ppu::ppu_read(uint16_t adr, bool read) {
  adr &= 0x3FFF;
  uint8_t data = 0x00;
  if (read) {
  }
  if (card->ppu_read(adr, data)) {
  } else if (adr >= 0x0000 && adr <= 0x1FFF) {
    // this is the address range for the
    // pattern tables
    // one starts from 0x0000 and the other from 0x1000
    // that bit indicates which pattern table to be accessed
    // background or sprite
    data = npatterns[adr >> 12][adr & 0x0FFF];
  } else if (adr >= 0x2000 && adr <= 0x2FFF) {
  } else if (adr >= 0x3F00 && adr <= 0x3FFF) {
    // This address space is for the palettes
    // we AND the address because it has mirrors
    // on 0x3F
    adr &= 0x00FF;
    // might be unnsecessary, but
    // not certain if the adr can access the background color
    // through mirrored indexes
    if (adr == 0x0010)
      adr = 0x0000;
    else if (adr == 0x0014)
      adr = 0x0004;
    else if (adr == 0x0018)
      adr = 0x0008;
    else if (adr == 0x001C)
      adr = 0x000C;

    data = adr;
  }
  return data;
}

olc::Sprite *Ppu::getScreen() const { return sprScreen.get(); }

// for this function, we'll have to loop through the pattern tables
// and then through the grid of the tile which will allow us
// to get the lsb and msb of the pixels
// we do this by shifting the value of the row of the tile
//
olc::Sprite &Ppu::getpatternTable(uint8_t i, uint8_t palette) {
  for (int pattern_y = 0; pattern_y < 16; pattern_y++) {
    for (int pattern_x = 0; pattern_x < 16; pattern_x++) {
      // offset is in bytes, so calculating 2D index is considering the bytes
      // hence multiplying by 216 and 16
      uint8_t offset = pattern_x * 256 + pattern_y * 16;
      for (int tile_y = 0; tile_y < 8; tile_y++) {
        uint8_t lsb = ppu_read(0x1000 * i + offset + tile_y, true);
        // the +8 is required because the rows of the tile
        // there are 8 rows and every row is stored as a byte
        // the lsb are stored one after the other, so adding 8 brings
        // us directly to the msb
        uint8_t msb = ppu_read(0x1000 * i + offset + tile_y + 8);

        for (int tile_x = 0; tile_x < 8; tile_x++) {
          uint8_t pixel = (msb & 0x01) + (lsb & 0x01);

          lsb = lsb >> 1;
          msb = msb >> 1;

          // we need to scale by 8 since the
          // sprite size is 128 by 128, and our
          // pattern table table has 16 tiles
          // so when we're done with the tile
          // we move on according to pattern_y and pattern_x
          // indicating which pattern we're on
          sprPatternTable[i]->SetPixel(pattern_x * 8 + (7 - tile_x),
                                       pattern_y * 8 + tile_y,
                                       get_palette_color(pixel, palette));
        }
      }
    }
  }
  return *sprPatternTable[i];
}

olc::Pixel Ppu::get_palette_color(uint8_t pixel, uint8_t palette) {
  // we need to multiply by 4 because the palettes have 7 locations where
  // each location stores 4 bytes of types of colors (1 byte for each type)
  // and that gets us the index, then add the pixel to choose which of the 4
  // colors we want
  return palScreen[ppu_read(0x3F00 + (palette * 4) + pixel)];
}

void Ppu::connectCard(Cartridge *c) { card = c; }

// cycles are the horizontal rendering
// scanlines vertical (somewhat like rows)
void Ppu::clock() {
  sprScreen->SetPixel(cycle - 1, scanline,
                      palScreen[(rand() % 2) ? 0x3F : 0x30]);
  if (cycle >= 341) {
    cycle = 0;
    scanline++;
    if (scanline >= 261) {
      scanline = -1;
      frame_complete = true;
    }
  }

  // BUG: this is what I currently believe is right to do for the rendering
  if (scanline >= 0 && scanline <= 239) {
    if (cycle >= 1 && cycle <= 256) {
      // we need to actually render the things
      // increment cycle clock by 2 after each fetching
      // render the thing
    } else if (cycle >= 257 && cycle <= 320) {
      // same thing as previous cycles but we dont render
    } else if (cycle >= 321 && cycle <= 336) {
      // tiles are fetched and loaded into shift registers
      // I believe they're loaded into their shift registers
      // so we will have a sprite shift register and a background shift register
    } else if (cycle >= 337 && cycle <= 340) {
      // random fetches for no reason, so we can just increment cycles ig
    }
  } else if (scanline == 240) {
    ;
  } else if (scanline >= 241 && scanline <= 260) {
    status.vblank = 1;
    control.nmi = 1;
  }
  cycle++;
}

/*
 * The PPU has 8KB of pattern memory (from 0x0000 to 0x1FFF)
 * and th 2KB of memory for nametables (0x2000 to 0x2FFF) the nametables are a
 * 2D array that stores ID's of which pattern to show and the memory from
 * (0x3F00 to 0x3FFF) which is for the Palettes which
 *
 *
 *
 *
 *
 *
 *
 *
 *
 * */
