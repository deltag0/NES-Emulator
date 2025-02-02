#ifndef PPU_H
#define PPU_H
#include "cartridge.h"
#include "olcPixelGameEngine.h"
#include <cstdint>
#include <fstream>
#include <memory>
#include <vector>
class Ppu {
public:
  Ppu();
  ~Ppu();

  // PPU will also be able to communicate with the CPU, so it can write and read
  // on the CPU soo we will need these functions
  uint8_t cpu_read(uint16_t adr, bool read = false);
  void cpu_write(uint16_t adr, uint8_t val);

  // of course, the PPU will also need to write and read on its own
  // addressable range
  uint8_t ppu_read(uint16_t adr, bool read = false);
  void ppu_write(uint16_t adr, uint8_t val);

private:
  // PPU also has acces to the cartridge, so we will keep a reference to it
  // it does not own it though, so it has a raw pointer
  // TEMP: LOGGING FOR DEBUG
  std::ofstream ofs{"ppu_pattern.txt"};
  Cartridge *card;
  uint8_t ntables[2][1024]; // vram memory for the nametables 0x2000 to 0x2FFF
  // even though there are 64 color palettes,
  // the palette color only stores an index to which
  // index color we will point to. Size of 32 since 
  // that's the size of 0x3f00 to 0x3f1f
  uint8_t palettes[32];
  // the size of the pattern
  // memory must be 4096 because
  // the tables are 16 x 16 and 
  // the tiles are composed of 2 8 byte
  // planes for the low and the high bytes
  uint8_t npatterns[2][4096];

  // Graphics for PPU
  // Array NES can display
  olc::Pixel palScreen[0x40];
  // FullScreen Output
  std::unique_ptr<olc::Sprite> sprScreen;
  // Name table display
  // tells the PPU how to place the tiles from the pattern table
  std::vector<std::unique_ptr<olc::Sprite>> sprNameTable;
  // Pattern display
  // pattern table is divided into 2 parts of memory
  // first from 0000-0FFFF and second from 1000-1FFF
  // both patterns have 256 tiles (16x16)
  // EACH tile is 8x8 (kind of like a grid)  
  // 01000001
  // 11000010
 //  01000100
  // 01001000
  // 00010000
  // 00100000
  // 01000000 
  // 10000000
  // the 8x8 grid is seperated into the low and the high bit 
  // So the table above could be the low or the high bit thable for a tile
  // and each row is stored as a byte
  // so the first row could be stored as 0x41 
  // makes it easy to figure out what is what
   std::vector<std::unique_ptr<olc::Sprite>> sprPatternTable;

  // for later
  // TODO: remove this, only for debugging (the public thing btw)
public:
  int16_t scanline = 0;
  int16_t cycle = 0;
  int buffer_cycles = 0;
  int total_cycles = 0;
  // tile_adress is the adress needing to be read from the nametable to get the index pointing to the tile
  uint16_t tile_adr = 0x0000;
  // attribute_adress is the adress needing to be read from the attribute table to get the palette of a tile
  uint16_t attribute_adress = 0x0000;
  // index pointing to the tile from the pattern table
  uint8_t pattern_table_low = 0x00;
  uint8_t pattern_table_high = 0x00;
  // palette value where bit 0-1 are the palettes for bottom right
  // the bits 2-3 are the paletters for bottom left
  // 4-5 for top right and 6-7 for top left
  // documentation is very unclear so TODO: test this and make sure this is right
  uint8_t palette_bits = 0x00;

  // registers are how the PPU and CPU communicate together
  // Registers are from 0x2000 to 0x2007
  // 0x2000  ----    -----  ----  -----  ----- ----  0x2007
  // control mask    status             scroll  addr  data
  // 
  // CONTROL determines some things about how to render the game
  // Notably notifies us of VBlank which is when NMI must be switched on
  //
  //

  // internal register of the PPU used for ppu_addr and ppu_status
  uint8_t latched = 0x00;
  // These 2 registries can simply be variables because they change based
  // off the latched variable and they change fully on each fetch 
  uint16_t ppu_addr = 0x0000;
  uint16_t v = 0x0000; // this is an internal register that is used for rendering (more on it in ppu_write 0x2006)
  uint16_t temp_ppu_addr = 0x0000;
  uint8_t fine_x = 0x00;  // fine-x scrolling register (unsure about its usages)
  // this register also fully changes based off a fetch
  uint8_t ppu_data_buffer = 0x00;
  // Since unions store everything on a single memory address
  // adding the 8bit reg variable and the struct allows us
  // to easily manipulate any bit from the registrys
  union PPUCTRL {
    uint8_t reg;
    struct {
      uint8_t name_table_low : 1;
      uint8_t name_table_high : 1;
      uint8_t increment : 1;
      uint8_t spr_patter_adr : 1;
      uint8_t bkg_patter_adr : 1;
      uint8_t sprite_size : 1;
      uint8_t master_select : 1;
      uint8_t nmi : 1;
    };
  } control;

  union PPUMASK {
    uint8_t reg;
    struct {
      uint8_t grey_scale : 1;
      uint8_t bkg_leftmost : 1;
      uint8_t sprite_leftmost : 1;
      uint8_t bkg_rendering : 1;
      uint8_t sprite_rendering : 1;
      uint8_t red : 1;
      uint8_t green : 1;
      uint8_t blue : 1;
    };
  } mask;

  union PPUSTATUS {
    uint8_t reg;
    struct {
      uint8_t unused1 : 1;
      uint8_t unused2 : 1;
      uint8_t unused3 : 1;
      uint8_t unused4 : 1;
      uint8_t unused5 : 1;
      uint8_t sprite_overflow : 1;
      uint8_t sprite_0_hit : 1;
      uint8_t vblank : 1;
    };
  } status;






public:
  // public interface
  void connectCard(Cartridge *c);
  bool clock();

  
  // debugging functions
  olc::Pixel get_palette_color(uint8_t pixel, uint8_t palette);
  olc::Sprite *getScreen() const;
  olc::Sprite &getNameTable(uint8_t i);
  olc::Sprite &getpatternTable(uint8_t i, uint8_t palette);
  bool frame_complete;
};

// Cpu configures the mapper
// to return addressed in our correct address range
// adapts the addresses from PPU AND CPU to write to correct
// address in the Cartridge
#endif
