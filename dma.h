# ifndef DMA_H
#define DMA_H
  
#include <cstdint>
class Ppu;
class Bus;

class Dma {
public:
  Dma(Bus *bus, Ppu *ppu);
  void copy_256(uint8_t addr);

private:
  Bus *bus;
  Ppu *ppu;
};

#endif
