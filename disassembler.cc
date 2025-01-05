#include <cmath>
#include <cstdint>
#include <ios>
#include <iostream>
#include <map>
#include <string>

#include "bus.h"
#include "cpu.h"

// Because the code is structured with switch statements,
// the cpu.cc would've been way too long, so the dissassembler
// is seperate. While the code is more efficient, it is much longer.

// Starting at the specified address we Cpu_read an instruction
// byte, which in  information from the lookup table
// as to how many  to Cpu_read and what the
// addressing mode is. I need this info to assemble human Cpu_readable
// syntax, which is different depending upon the addressing mode

// As the instruction is decoded, a std::string is assembled
// with the Cpu_readable output
std::map<uint16_t, std::string> Cpu::disassemble(uint16_t nStart,
                                                 uint16_t nStop) {
  uint32_t addr = static_cast<uint32_t>(nStart);
  uint8_t value = 0x00, lo = 0x00, hi = 0x00;
  std::map<uint16_t, std::string> mapLines;
  uint16_t line_addr = 0;

  auto hex = [](uint32_t n, uint8_t d) {
    std::string s(d, '0');
    for (int i = d - 1; i >= 0; i--, n >>= 4)
      s[i] = "0123456789ABCDEF"[n & 0xF];
    return s;
  };

  // A convenient utility to convert variables into
  // hex strings because "modern C++"'s method with
  // streams is atrocious
  while (addr <= (uint32_t)nStop) {

    line_addr = addr;

    // Prefix line with instruction address
    std::string sInst = "$" + hex(addr, 4) + ": ";

    // Cpu_read instruction, and get its Cpu_readable name
    uint8_t opcode = bus->Cpu_read(line_addr, true);
    addr++;
    sInst += lookup[opcode].name + " ";

    // Get oprands from desired locations, and form the
    // instruction based upon its addressing mode. These
    // routines mimmick the actual fetch routine of the
    // 6502 in order to get accurate data as part of the
    // instruction
    if (lookup[opcode].addrmode == &Cpu::imp) {
      sInst += " {IMP}";
    } else if (lookup[opcode].addrmode == nullptr) {
      value = bus->Cpu_read(addr, true);
      addr++;
      sInst += "#$" + hex(value, 2) + " {IMM}";
    } else if (lookup[opcode].addrmode == &Cpu::zpg) {
      lo = bus->Cpu_read(addr, true);
      addr++;
      hi = 0x00;
      sInst += "$" + hex(lo, 2) + " {ZP0}";
    } else if (lookup[opcode].addrmode == &Cpu::zpgX) {
      lo = bus->Cpu_read(addr, true);
      addr++;
      hi = 0x00;
      sInst += "$" + hex(lo, 2) + ", X {ZPX}";
    } else if (lookup[opcode].addrmode == &Cpu::zpgY) {
      lo = bus->Cpu_read(addr, true);
      addr++;
      hi = 0x00;
      sInst += "$" + hex(lo, 2) + ", Y {ZPY}";
    } else if (lookup[opcode].addrmode == &Cpu::ind_X) {
      lo = bus->Cpu_read(addr, true);
      addr++;
      hi = 0x00;
      sInst += "($" + hex(lo, 2) + ", X) {IZX}";
    } else if (lookup[opcode].addrmode == &Cpu::ind_Y) {
      lo = bus->Cpu_read(addr, true);
      addr++;
      hi = 0x00;
      sInst += "($" + hex(lo, 2) + "), Y {IZY}";
    } else if (lookup[opcode].addrmode == &Cpu::absolute) {
      lo = bus->Cpu_read(addr, true);
      addr++;
      hi = bus->Cpu_read(addr, true);
      addr++;
      sInst += "$" + hex((uint16_t)(hi << 8) | lo, 4) + " {ABS}";
    } else if (lookup[opcode].addrmode == &Cpu::absoluteX) {
      lo = bus->Cpu_read(addr, true);
      addr++;
      hi = bus->Cpu_read(addr, true);
      addr++;
      sInst += "$" + hex((uint16_t)(hi << 8) | lo, 4) + ", X {ABX}";
    } else if (lookup[opcode].addrmode == &Cpu::absoluteY) {
      lo = bus->Cpu_read(addr, true);
      addr++;
      hi = bus->Cpu_read(addr, true);
      addr++;
      sInst += "$" + hex((uint16_t)(hi << 8) | lo, 4) + ", Y {ABY}";
    } else if (lookup[opcode].addrmode == &Cpu::indirect) {
      lo = bus->Cpu_read(addr, true);
      addr++;
      hi = bus->Cpu_read(addr, true);
      addr++;
      sInst += "($" + hex((uint16_t)(hi << 8) | lo, 4) + ") {IND}";
    } else if (lookup[opcode].addrmode == &Cpu::relative) {
      value = bus->Cpu_read(addr, true);
      addr++;
      sInst += "$" + hex(value, 2) + " [$" + hex(addr + value, 4) + "] {REL}";
    }

    // Add the formed string to a std::map, using the instruction's
    // address as the key. This makes it convenient to look for later
    // as the instructions are variable in length, so a straight up
    // incremental index is not sufficient.
    mapLines[line_addr] = sInst;
  }

  return mapLines;
}
