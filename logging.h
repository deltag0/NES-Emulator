#ifndef LOGGING_H
#define LOGGING_H

#include <cstdint>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>

#include "bus.h"

uint8_t wrap_around(uint8_t val1, uint8_t val2);

void debug_log(std::ofstream out, std::string message, bool debugging) {
  if (!debugging)
    return;

  out << message << "\n";
}

void info_log(std::ofstream out, std::string message) {
  out << message << "\n";
}

void debug_log_cpu(std::ofstream &out, Cpu &cpu, Ppu &ppu, bool debugging) {
  if (!debugging)
    return;

  auto hex = [](uint32_t n, uint8_t d) {
    std::string s(d, '0');
    for (int i = d - 1; i >= 0; i--, n >>= 4)
      s[i] = "0123456789ABCDEF"[n & 0xF];
    return s;
  };

  uint16_t &addr = cpu.PC;
  uint16_t PC_save = cpu.PC;
  uint8_t lo = 0x00;
  uint8_t hi = 0x00;
  uint8_t opcode = cpu.read(cpu.PC);

  std::string sInst = "";
  std::string status = "";
  std::string ppu_cycles = ",";
  std::string ppu_scanlines = " PPU:  ";

  if (ppu.scanline >= 10)
    ppu_scanlines = " PPU: ";
  if (ppu.scanline >= 100)
    ppu_scanlines = " PPU:";
  if (ppu.cycle < 100)
    ppu_cycles += " ";
  if (ppu.cycle < 10)
    ppu_cycles += " ";

  sInst += hex(addr, 4) + "  " + hex(opcode, 2) + " ";
  status += "A:" + hex(cpu.accumulator, 2) + " X:" + hex(cpu.x, 2) +
            " Y:" + hex(cpu.y, 2) + " P:" + hex(cpu.status, 2) +
            " SP:" + hex(cpu.stack_pointer, 2) + ppu_scanlines +
            std::to_string(ppu.scanline) + ppu_cycles +
            std::to_string(ppu.cycle) + " " +
            "CYC:" + std::to_string(cpu.total_cycles);
  addr++;

  if (cpu.lookup[opcode].addrmode == &Cpu::imp) {
    sInst += "       " + cpu.lookup[opcode].name;
  } else if (cpu.lookup[opcode].addrmode == nullptr) {
    uint8_t value = cpu.read(addr);
    sInst += hex(value, 2) + "     " + cpu.lookup[opcode].name + " #$" +
             hex(value, 2);
  } else if (cpu.lookup[opcode].addrmode == &Cpu::zpg) {
    cpu.zpg();
    lo = cpu.adr & 0x00FF;
    sInst += hex(lo, 2) + "     " + cpu.lookup[opcode].name + " $" +
             hex(lo, 2) + " = " + hex(cpu.fetch(), 2);
  } else if (cpu.lookup[opcode].addrmode == &Cpu::zpgX) {
    cpu.zpgX();
    lo = cpu.adr & 0x00FF;
    sInst += hex(lo, 2);
    sInst += "     " + cpu.lookup[opcode].name + "$" + hex(lo, 2) + " = " +
             hex(cpu.fetch(), 2);
  } else if (cpu.lookup[opcode].addrmode == &Cpu::zpgY) {
    cpu.zpgY();
    lo = cpu.adr & 0x00FF;
    sInst += hex(lo, 2);
    sInst += "     " + cpu.lookup[opcode].name + "$" + hex(lo, 2) + " = " +
             hex(cpu.fetch(), 2);
  } else if (cpu.lookup[opcode].addrmode == &Cpu::ind_X) {
    uint16_t val = cpu.read(addr);
    cpu.ind_X();
    cpu.fetch();
    lo = cpu.adr & 0x00FF;
    sInst += hex(val, 2) + "     " + cpu.lookup[opcode].name + " ($" +
             hex(val, 2) + ",X) @ " +
             hex(static_cast<uint16_t>(
                     wrap_around(val, static_cast<uint16_t>(cpu.x))),
                 2) +
             " = " + hex(cpu.adr, 4) + " = " + hex(cpu.fetched, 2);
  } else if (cpu.lookup[opcode].addrmode == &Cpu::ind_Y) {
    uint16_t val = cpu.read(addr);
    cpu.ind_Y();
    cpu.fetch();
    sInst += hex(val, 2) + "     " + cpu.lookup[opcode].name + " ($" +
             hex(val, 2) + "),Y = " + hex(cpu.adr - cpu.y, 4) + " @ " +
             hex(cpu.adr, 4) + " = " + hex(cpu.fetched, 2);
  } else if (cpu.lookup[opcode].addrmode == &Cpu::absolute) {
    lo = cpu.bus->Cpu_read(addr++, true);
    hi = cpu.bus->Cpu_read(addr++, true);
    uint16_t val = (hi << 8) | lo;
    sInst += hex(lo, 2) + " " + hex(hi, 2) + "  " + cpu.lookup[opcode].name +
             " $" + hex(val, 4);
    if (cpu.lookup[opcode].name != "JSR" && cpu.lookup[opcode].name != "JMP")
      sInst += " = " + hex(cpu.read(val), 2);
  } else if (cpu.lookup[opcode].addrmode == &Cpu::absoluteX) {
    lo = cpu.bus->Cpu_read(addr++, true);
    hi = cpu.bus->Cpu_read(addr++, true);
    uint16_t val = (hi << 8) | lo;
    sInst += hex(lo, 2) + " " + hex(hi, 2) + "  " + cpu.lookup[opcode].name +
             " $" + hex(val, 4) + ",X @ " + hex(val + cpu.x, 4) + " = " +
             hex(cpu.read(val + cpu.x), 2);
  } else if (cpu.lookup[opcode].addrmode == &Cpu::absoluteY) {
    lo = cpu.bus->Cpu_read(addr++, true);
    hi = cpu.bus->Cpu_read(addr++, true);
    uint16_t val = (hi << 8) | lo;
    sInst += hex(lo, 2) + " " + hex(hi, 2) + "  " + cpu.lookup[opcode].name +
             " $" + hex(val, 4) + ",Y @ " + hex(val + cpu.y, 4) + " = " +
             hex(cpu.read(val + cpu.y), 2);
  } else if (cpu.lookup[opcode].addrmode == &Cpu::indirect) {
    lo = cpu.bus->Cpu_read(addr++, true);
    hi = cpu.bus->Cpu_read(addr++, true);
    sInst += hex(lo, 2) + " " + hex(hi, 2);
    sInst += "  " + cpu.lookup[opcode].name + " $" + hex((hi << 8) | lo, 4);
  } else if (cpu.lookup[opcode].addrmode == &Cpu::relative) {
    uint8_t val = cpu.read(addr);
    cpu.relative();
    sInst += hex(val, 2);
    sInst += "     " + cpu.lookup[opcode].name + " $" +
             hex(cpu.PC + cpu.adr_relative, 4);
  }
  out << sInst << std::setw(48 - sInst.length()) << "" << std::left
      << std::setw(30 + std::to_string(cpu.total_cycles).size()) << status
      << "\n";
  /* std::cout << sInst << std::setw(48 - sInst.length()) << "" << std::left */
  /*     << std::setw(30 + std::to_string(cpu.total_cycles).size()) << status */
  /*     << "\n"; */

  cpu.PC = PC_save;
}
#endif
