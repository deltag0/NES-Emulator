
#include "ppu.h"
#include "cartridge.h"
#include <cstdint>
#include <ios>
#include <iostream>
#include <memory>
#include <winnt.h>

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
    // TODO: in the debugger this flag gets activated when the val is 0x10
    // not sure why that's the case...
    status.unused5 = 1;
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
      ppu_addr = (ppu_addr & 0x00FF) | (val << 8);
      latched = 1;
    } else {
      // set the low byte second
      ppu_addr = (ppu_addr & 0xFF00) | val;
      latched = 0;
      // the v register is set to be the same as the ppu_addr register
      // because when not rendering they are essentially the same
      // when rendering they will both differ from one another.
      v = ppu_addr;
    }
    break;
  case 0x0007: // PPU data
    ppu_write(ppu_addr, val);
    if (control.increment)
      ppu_addr += 32;
    else
      ppu_addr += 1;
    break;
  }
}

// cpu reading from ppu
uint8_t Ppu::cpu_read(uint16_t adr, bool read) {
  uint8_t data{0x00};
  switch (adr) {
  case 0x0000:
    break;
  case 0x0001:
    break;
  case 0x0002: // Status register
    // setting the status vblank to 1 is temporary
    /* status.vblank = 1; */
    data = status.reg;
    latched = 0x00;
    status.vblank = 0x00;
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
    ppu_data_buffer = ppu_read(ppu_addr);

    // For address range of palettes, there is no delay
    // for reading data from ppu
    if (ppu_addr >= 0x3F00 && ppu_addr <= 0x3FFF) {
      data = ppu_data_buffer;
    }
    if (control.increment)
      ppu_addr += 32;
    else
      ppu_addr += 1;
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
    npatterns[(adr & 0x1000) >> 12][adr & 0x0FFF] = val;
  } else if (adr >= 0x2000 && adr <= 0x2FFF) {
    int name_table_idx = 0;
    if (adr > 0x2400) {
      name_table_idx = 1;
    }
    ntables[name_table_idx][(adr & 0x23FF) - 0x2000] = val;

  } else if (adr >= 0x3F00 && adr <= 0x3FFF) {
    // This address space is for the palettes
    // we AND the address because it has mirrors
    // on 0x3F
    adr &= 0x001F;
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
    data = npatterns[(adr & 0x1000) >> 12][adr & 0x0FFF];
  } else if (adr >= 0x2000 && adr <= 0x2FFF) {
    int name_table_idx = 0;
    if (adr > 0x2400) {
      name_table_idx = 1;
    }
    data = ntables[name_table_idx][(adr & 0x23FF) - 0x2000];
  } else if (adr >= 0x3F00 && adr <= 0x3FFF) {
    // This address space is for the palettes
    // we AND the address because it has mirrors
    // on 0x3F
    adr &= 0x001F;
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

    data = palettes[adr];
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
  for (int pattern_x = 0; pattern_x < 16; pattern_x++) {
    for (int pattern_y = 0; pattern_y < 16; pattern_y++) {
      // offset is in bytes, so55 calculating 2D index is considering the bytes
      // hence multiplying by 256 and 16
      // using the formula: y * width + x
      uint16_t offset = pattern_x * 256 + pattern_y * 16;
      for (int tile_x = 0; tile_x < 8; tile_x++) {
        uint8_t lsb = ppu_read(0x1000 * i + offset + tile_x, true);
        // the +8 is required because the rows of the tile
        // there are 8 rows and every row is stored as a byte
        // the lsb are stored one after the other, so adding 8 brings
        // us directly to the msb
        uint8_t msb = ppu_read(0x1000 * i + offset + tile_x + 8, true);

        for (int tile_y = 0; tile_y < 8; tile_y++) {
          uint8_t pixel = ((msb & 0x01) << 1) || (lsb & 0x01);

          lsb = lsb >> 1;
          msb = msb >> 1;

          // we need to scale by 8 since the
          // sprite size is 128 by 128, and our
          // pattern table table has 16 tiles
          // so when we're done with the tile
          // we move on according to pattern_y and pattern_x
          // indicating which pattern we're on
          sprPatternTable[i]->SetPixel(pattern_y * 8 + (7 - tile_y),
                                       pattern_x * 8 + tile_x,
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
  return palScreen[ppu_read(0x3F00 + (palette << 2) + pixel) & 0x3F];
}

void Ppu::connectCard(Cartridge *c) { card = c; }

// cycles are the horizontal rendering
// scanlines vertical (somewhat like rows)
bool Ppu::clock() {
  /* Returning 1 indicates that an NMI was triggered
   * This function handles all the rendering done by the PPU
   * It renders 8 pixels on a scanline all at once and uses buffer cycles to
   * keep the timing fine.
   */
  if (buffer_cycles > 0) {
    // increment fine_x
    buffer_cycles--;
    fine_x++;

    if (buffer_cycles == 0) {
      fine_x = 0;
      // TODO: figure out if we need to switch nametables because
      // we might need to?

      if ((v & 0x001F) == 31) {

        // reset coarse_x
        v &= ~0x001F;

        if (((v & 0x7000) >> 12) == 7) {
          // reset fine_y
          v &= ~0x7000;

          if (((v & 0x03E0) >> 5) == 29) {
            // TODO: this is temporary since scanline needs to be reset to -1
            // also there's a problem with the scanlines since it only gets to
            // 210
            frame_complete = true;
            // reset coarse_y
            v &= ~0x03E0;

            for (int row = 0; row < 240; row++) {
              for (int col = 0; col < 256; col++) {
                sprScreen->SetPixel(col, row, 0x00);
              }
            }
          } else {
            // increment coarse_y
            v += 0x0020;
          }
        } else {
          // increment fine_y
          v += 0x1000;
        }
      } else {
        // increment coarse_x
        v++;
      }

      // I render the entire tile row at once because it's simpler to emulate
      // this way so we can just update the coarse_x when we run out of buffer
      // cycles and reset fine_x
    }

    return 0;
  }

  // this part of code was only temporary for getting a static screen
  /* sprScreen->SetPixel(cycle - 1, scanline, */
  /*                     palScreen[(rand() % 2) ? 0x3F : 0x30]); */
  /* if (cycle >= 341) { */
  /*   cycle = 0; */
  /*   scanline++; */
  /*   if (scanline >= 261) { */
  /*     scanline = -1; */
  /*     frame_complete = true; */
  /*   } */
  /* } */

  if (scanline >= 0 && scanline <= 239) {

    if (cycle >= 1 && cycle <= 256) {
      // we need to actually render the things
      // increment cycle clock by 2 after each fetching
      // render the thing
      // These are the addresses that must be read from the nametable or
      // attribute table to get the necessary info
      tile_adr = 0x2000 | (v & 0x0FFF);
      attribute_adress =
          0x23C0 | (v & 0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07);

      // can be reduced to 4 reads like the "real" NES, but this is fine too
      // memmory accesses by the ppu to get information to render (8 cycles
      // total)
      palette_bits = ppu_read(attribute_adress);

      // variables to help determine the quandrant and to render the pixels
      uint8_t coarse_x = v & 0b0000000000011111;
      uint8_t coarse_y = (v & 0b0000001111100000) >> 5;
      uint8_t fine_y = (v & 0b0111000000000000) >> 12;
      pattern_table_low = ppu_read((control.bkg_patter_adr * 0x1000) +
                                   ppu_read(tile_adr) * 16 + fine_y);
      pattern_table_high = ppu_read((control.bkg_patter_adr * 0x1000) +
                                    ppu_read(tile_adr) * 16 + 8 + fine_y);

      int x_check = coarse_x % 4;
      int y_check = coarse_y % 4;

      // steps to determine wich palette bits we need from the attribute table
      // so it determines which quadrant we get the palette from
      if ((x_check == 0 || x_check == 1) && (y_check == 0 || y_check == 1)) {
        // Top left quadrant
        palette_bits &= 0b00000011;
      } else if ((x_check == 0 || x_check == 1) &&
                 (y_check == 2 || y_check == 3)) {
        // Bottom left quadrant
        palette_bits &= 0b00110000;
        palette_bits = palette_bits >> 4;
      } else if ((y_check == 0 || y_check == 1) &&
                 (x_check == 2 || x_check == 3)) {
        // Top right quadrant
        palette_bits &= 0b00001100;
        palette_bits = palette_bits >> 2;
      } else {
        // Bottom right quadrant
        palette_bits &= 0b11000000;
        palette_bits = palette_bits >> 6;
      }

      // rendering the pixels for the current scanline (fine_y)
      if (mask.bkg_rendering) {
        for (int i = 0; i < 8; i++) {
          uint8_t pixel =
              ((pattern_table_high & 0x01) << 1) | (pattern_table_low & 0x01);
          pattern_table_high = pattern_table_high >> 1;
          pattern_table_low = pattern_table_low >> 1;
          sprScreen->SetPixel(coarse_x * 8 + (7 - i), coarse_y * 8 + fine_y,
                              get_palette_color(pixel, palette_bits));
        }
      }

      buffer_cycles += 8;
      cycle += 8;
      total_cycles += 8;
    } else if (cycle >= 257 && cycle <= 320) {
      // TODO: This is important for rendering the sprites, so for now I'll just
      // simulate adding cycles
      cycle = 321;
    } else if (cycle >= 321 && cycle <= 336) {
      // Tiles for next scanline are loaded into shift registers
      // TODO: I think we won't need to worry about this since we can retrieve
      // data immediately in our emulation, so dummy cycles for now
      cycle = 337;
    } else if (cycle >= 337 && cycle <= 340) {
      // random fetches for no reason, so we can just increment cycles ig
      scanline++;
      cycle = 0;
    } else {
      cycle++;
    }
  } else if (scanline == 240) {
    if (cycle == 340) {
      cycle = 0;
      scanline++;
    }
    cycle++;
  } else if (scanline >= 241 && scanline <= 260) {
    if (cycle == 340) {
      cycle = 0;
      scanline++;
    } else
      cycle++;
    status.vblank = 1;
    if (status.vblank && control.nmi) {
      // trigger NMI
      return 1;
    }
  } else if (scanline == 261) {
    // Clear vblank flag
    status.vblank = 0;
    scanline = 0;
    frame_complete = false;
  }
  /* std::cout << "CYCLE: " << cycle << "\n"; */
  /* std::cout << "CYCLE BUFFER " << buffer_cycles << "\n"; */
  /* std::cout << "SCANLINE " << scanline << "\n"; */
  /* std::cout << "PPU ADDR " << std::hex << v << "\n"; */
  /* std::cout << "COARSE X " << std::hex << (v & 0x001F) << "\n"; */
  /* std::cout << "FINE Y " << (v & 0x7000) << "\n"; */
  return 0;
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
