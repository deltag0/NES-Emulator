
#include "ppu.h"
#include "bus.h"
#include "cartridge.h"
#include <algorithm>
#include <cstdint>
#include <ios>
#include <iostream>
#include <memory>
#include <winnt.h>

Ppu::Ppu()
    : sprNameTable(2), sprPatternTable(2), oam(256), frame_complete(false) {
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
// For both cpu_write and cpu_read, the address can
// only actually be from 0x0000 to 0x0007
void Ppu::cpu_write(uint16_t adr, uint8_t val) {
  switch (adr) {
  case 0x0000: // control
    control.reg = val;
    status.unused5 = 1;
    t.nametable_high = control.name_table_high;
    t.nametable_low = control.name_table_low;
    break;
  case 0x0001: // mask
    mask.reg = val;
    break;
  case 0x0002: // Status
    break;
  case 0x0003: // OAM
    oam_addr = val;
    break;
  case 0x0004: // OAM data
    oam[oam_addr] = val;

    if (oam_addr == 255) {
      oam_addr = 0x00;
    } else {
      oam_addr++;
    }
    break;
  case 0x0005: // Scroll
    // set coarse x and coarse y
    if (latched == 0) {
      t.coarse_x = val / 8;
      fine_x = val % 8;
      latched = 1;
    }
    else {
      t.coarse_y = val / 8;
      t.fine_y = val % 8;
      latched = 0;
    }
    break;
  case 0x0006: // PPU addr
    if (latched == 0) {
      // set the high byte first
      t.reg = (t.reg & 0x00FF) | (val << 8);
      latched = 1;
    } else {
      // set the low byte second
      t.reg = (t.reg & 0xFF00) | val;
      latched = 0;
      // the v register is set to be the same as the ppu_addr register
      // because when not rendering they are essentially the same
      // when rendering they will both differ from one another.
      v = t.reg;
    }
    break;
  case 0x0007: // PPU data
    ppu_write(v, val);
    if (control.increment)
      v += 32;
    else
      v += 1;
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
    data = status.reg;
    latched = 0x00;
    status.vblank = 0x00;
    break;
  case 0x0003:
    break;
  case 0x0004:
    data = oam[oam_addr];
    break;
  case 0x0005:
    break;
  case 0x0006:
    break;
  case 0x0007:
    if (v >= 0x3F00 && v <= 0x3FFF) {
      data = ppu_read(v);
      ppu_data_buffer = ppu_read(v - 0x1000);
    } else {
      // Normal VRAM read with delay
      data = ppu_data_buffer;
      ppu_data_buffer = ppu_read(v);
    }
    // Increment ppu_addr based on the control flag.
    v += (control.increment ? 32 : 1);
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
    uint8_t name_table_idx = (v & 0x0C00) >> 10;

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
    uint8_t name_table_idx = (v & 0x0C00) >> 10;

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
          uint8_t pixel = ((msb & 0x01) << 1) | (lsb & 0x01);

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

void Ppu::update_render() {
  if (fine_x == 8) {
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
          frame_complete = true;
          // reset coarse_y
          v &= ~0x03E0;

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
  }
}
// cycles are the horizontal rendering
// scanlines vertical (somewhat like rows)
bool Ppu::clock() {
  /* Returning 1 indicates that an NMI was triggered
   * This function handles all the rendering done by the PPU
   * It renders 8 pixels on a scanline all at once and uses buffer cycles to
   * keep the timing fine.
   */

  bool return_val = 0;

  if (scanline >= 0 && scanline <= 239) {
    uint8_t coarse_x = v & 0b0000000000011111;
    uint8_t coarse_y = (v & 0b0000001111100000) >> 5;
    uint8_t fine_y = (v & 0b0111000000000000) >> 12;
    uint8_t curr_render_y = coarse_y * 8 + fine_y;

    if (cycle >= 1 && cycle <= 256) {
      // doing this in less cycles because I wanted to
      if (cycle >= 65 && cycle <= 128) {
        if (cycle == 65) {
          clear_secondary_oam();
        }

        uint8_t sprite_idx = (cycle - 65) * 4;

        if (0 <= curr_render_y - oam[sprite_idx] &&
            curr_render_y - oam[sprite_idx] <= 7 && secondary_oam.size() < 8) {
          secondary_oam.push(sprite_idx);
        }
      }

      if ((cycle - 1) % 8 == 0) {

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
        pattern_table_low = ppu_read((control.bkg_patter_adr * 0x1000) +
                                     ppu_read(tile_adr) * 16 + fine_y);
        pattern_table_high = ppu_read((control.bkg_patter_adr * 0x1000) +
                                      ppu_read(tile_adr) * 16 + 8 + fine_y);

        int x_check = coarse_x % 4;
        int y_check = coarse_y % 4;

        // steps to determine wich palette bits we need from the attribute
        // table so it determines which quadrant we get the palette from
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
      }

      uint8_t bkg_pixel = (((pattern_table_high & 0x80) >> 7) << 1) |
                          ((pattern_table_low & 0x80) >> 7);

      while (sprite_shift.size() > 0 &&
          sprite_shift.front().sprite_x == coarse_x * 8 + fine_x) {
        render_sprites.emplace_back(sprite_shift.front());
        sprite_shift.pop();
      }

      // rendering the pixels for the current scanline (fine_y)
      if (mask.bkg_rendering) {
        sprScreen->SetPixel(coarse_x * 8 + fine_x, curr_render_y,
                            get_palette_color(bkg_pixel, palette_bits));
      }

      pattern_table_high <<= 1;
      pattern_table_low <<= 1;

      if (render_sprites.size() > 0 && mask.sprite_rendering) {
        Sprite &render_sprite = render_sprites.front();
        for (auto c_sprite: render_sprites) {
          render_sprite = c_sprite.idx < render_sprite.idx ? c_sprite : render_sprite;
        }

        for (auto &c_sprite: render_sprites) {
          if (&c_sprite != &render_sprite) {
            move_sprite_pixels(c_sprite);
          }
        }

        bool flip_horz = oam[render_sprite.idx + 2] & 0x40;
        uint8_t pixel{0x00};
        if (flip_horz) {
          pixel = ((render_sprite.sprite_high & 0x01) << 1) |
                  (render_sprite.sprite_low & 0x01);
        } else {
          pixel = (((render_sprite.sprite_high & 0x80) >> 7) << 1) |
                  ((render_sprite.sprite_low & 0x80) >> 7);
        }
        move_sprite_pixels(render_sprite);

        if (pixel != 0) {
          sprScreen->SetPixel(coarse_x * 8 + fine_x, curr_render_y,
                              get_palette_color(pixel, render_sprite.palette));
        }
      }

      while (render_sprites.size() > 0 && coarse_x * 8 + fine_x >= render_sprites.front().sprite_x + 7) {
        render_sprites.pop_front();
      }

      fine_x++;
      if (mask.bkg_rendering || mask.sprite_rendering) {
        update_render();
      }
      cycle++;
      total_cycles += 1;
    } else if (cycle >= 257 && cycle <= 320) {
      // TODO: should only be able to have 8 sprites in the secondary OAM
      // Apparently sprite overflow doesn't seem to be that impportant, so not
      // implemented
      if (cycle == 257) {
        clear_sprite_shift();
        sort_secondary_oam();
        render_sprites.clear();
      }

      if (secondary_oam.size() > 0) {
        uint8_t sprite_addr = secondary_oam.front();
        Sprite sprite;

        sprite.idx = sprite_addr;
        bool flip_vert = oam[sprite.idx + 2] & 0x80;
        sprite.sprite_y = curr_render_y - oam[sprite.idx]- 1;
        sprite.sprite_x = oam[sprite.idx + 3];

        if (flip_vert) {
          sprite.sprite_y = 7 - sprite.sprite_y;
        }

        sprite.sprite_low =
            ppu_read((control.spr_patter_adr * 0x1000) +
                     oam[sprite.idx + 1] * 16 + sprite.sprite_y);
        sprite.sprite_high =
            ppu_read((control.spr_patter_adr * 0x1000) +
                     oam[sprite.idx + 1] * 16 + sprite.sprite_y + 8);

        sprite.palette = (oam[sprite.idx + 2] & 0x03) + 4;

        sprite_shift.push(sprite);
        secondary_oam.pop();
      }

      cycle++;
      oam_addr = 0;
    } else if (cycle >= 321 && cycle <= 336) {
      // Tiles for next scanline are loaded into shift registers
      cycle++;
    } else if (cycle >= 337 && cycle <= 340) {
      // random fetches for no reason, so we can just increment cycles ig
      scanline++;
      cycle = 0;
    } else {
      cycle++;
    }

  } else if (scanline == 240 || scanline == -1) {
    if (cycle == 340) {
      cycle = 0;
      scanline++;
    } else {
      cycle++;
    }
  } else if (scanline >= 241 && scanline <= 260) {
    if (cycle == 340) {
      cycle = 0;
      scanline++;
    } else if (cycle == 1) {
      cycle++;
      status.vblank = 1;
      if (status.vblank && control.nmi) {
        return 1;
      }
    } else
      cycle++;

  } else if (scanline == 261) {
    // Clear vblank flag
    if (cycle == 340) {
      scanline = -1;
      cycle = 0;
    } else if (cycle == 1) {
      if (mask.sprite_rendering || mask.bkg_rendering) {
        v = t.reg;
        fine_x = 0;
      }
      status.reg = 0x00;
      cycle++;
    } else {
      frame_complete = false;
      cycle++;
    }
  }
  /* std::cout << "CYCLE: " << cycle << "\n"; */
  /* std::cout << "CYCLE BUFFER " << buffer_cycles << "\n"; */
  /* std::cout << "SCANLINE " << scanline << "\n"; */
  /* std::cout << "PPU ADDR " << std::hex << v << "\n"; */
  /* std::cout << "COARSE X " << std::hex << (v & 0x001F) << "\n"; */
  /* std::cout << "FINE Y " << (v & 0x7000) << "\n"; */
  /* uint8_t coarse_y = (v & 0b0000001111100000) >> 5; */
  /* uint8_t fine_y = (v & 0b0111000000000000) >> 12; */
  /* std::cout << "SCANLINE: " << static_cast<uint16_t>(scanline) << " " */
  /*           << static_cast<uint16_t>(coarse_y * 8 + fine_y) << "\n"; */
  return return_val;
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
