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
#include "cpu.h"

void debug_log(std::ofstream out, std::string message, bool debugging) {
  if (!debugging)
    return;

  out << message << "\n";
}

void info_log(std::ofstream out, std::string message) {
  out << message << "\n";
}

void debug_log_cpu(std::ofstream &out, Cpu &cpu, bool debugging) {
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
  sInst += hex(addr, 4) + "  " + hex(opcode, 2) + " ";
  status += "A:" + hex(cpu.accumulator, 2) + " X:" + hex(cpu.x, 2) +
            " Y:" + hex(cpu.y, 2) + " SP:" + hex(cpu.stack_pointer, 2) +
            " PPU:  " + "0, " + "" + " " +
            "CYC:" + std::to_string(cpu.total_cycles);
  addr++;

  if (cpu.lookup[opcode].addrmode == &Cpu::imp) {
    sInst += "      " + cpu.lookup[opcode].name;
  } else if (cpu.lookup[opcode].addrmode == nullptr) {
    uint8_t value = cpu.read(addr);
    sInst += hex(value, 2) + "    " + cpu.lookup[opcode].name + " #$" +
             hex(value, 2);
  } else if (cpu.lookup[opcode].addrmode == &Cpu::zpg) {
    cpu.zpg();
    lo = cpu.adr & 0x00FF;
    sInst += hex(lo, 2) + "    " + cpu.lookup[opcode].name + "$" + hex(lo, 2) +
             " = " + hex(cpu.fetch(), 2);
  } else if (cpu.lookup[opcode].addrmode == &Cpu::zpgX) {
    cpu.zpgX();
    lo = cpu.adr & 0x00FF;
    sInst += hex(lo, 2);
    sInst += "    " + cpu.lookup[opcode].name + "$" + hex(lo, 2) + " = " +
             hex(cpu.fetch(), 2);
  } else if (cpu.lookup[opcode].addrmode == &Cpu::zpgY) {
    cpu.zpgY();
    lo = cpu.adr & 0x00FF;
    sInst += hex(lo, 2);
    sInst += "    " + cpu.lookup[opcode].name + "$" + hex(lo, 2) + " = " +
             hex(cpu.fetch(), 2);
  } else if (cpu.lookup[opcode].addrmode == &Cpu::ind_X) {
    uint8_t val = cpu.read(addr);
    cpu.ind_X();
    hi = cpu.adr >> 8;
    sInst += hex(val, 2);
    sInst += "    " + cpu.lookup[opcode].name + "$ " + hex(val, 2) + ",X @ " +
             hex(hi, 2) + " = " + hex(cpu.fetched, 2);
  } else if (cpu.lookup[opcode].addrmode == &Cpu::ind_Y) {
    uint8_t val = cpu.read(addr);
    cpu.ind_Y();
    sInst += hex(val, 2) + "    " + cpu.lookup[opcode].name + "($" +
             hex(val, 2) + ",Y = " + hex(cpu.adr, 4) + " = " +
             hex(cpu.fetch(), 2);
  } else if (cpu.lookup[opcode].addrmode == &Cpu::absolute) {
    lo = cpu.bus->Cpu_read(addr++, true);
    hi = cpu.bus->Cpu_read(addr++, true);
    uint16_t val = (hi << 8) | lo;
    sInst += hex(lo, 2) + " " + hex(hi, 2) + " " + cpu.lookup[opcode].name +
             " $" + hex(val, 4);
    if (cpu.lookup[opcode].name != "JSR")
      sInst += " = " + hex(cpu.read(val), 2);
  } else if (cpu.lookup[opcode].addrmode == &Cpu::absoluteX) {
    lo = cpu.bus->Cpu_read(addr++, true);
    hi = cpu.bus->Cpu_read(addr++, true);
    uint16_t val = (hi << 8) | lo;
    sInst += hex(lo, 2) + " " + hex(hi, 2) + " " + cpu.lookup[opcode].name +
             " $" + hex(val, 4) + ",X @ " + hex(val + cpu.x, 4) + " = " +
             hex(cpu.read(val + cpu.x), 2);
  } else if (cpu.lookup[opcode].addrmode == &Cpu::absoluteY) {
    lo = cpu.bus->Cpu_read(addr++, true);
    hi = cpu.bus->Cpu_read(addr++, true);
    uint16_t val = (hi << 8) | lo;
    sInst += hex(lo, 2) + " " + hex(hi, 2) + " " + cpu.lookup[opcode].name +
             " $" + hex(val, 4) + ",Y @ " + hex(val + cpu.y, 4) +
             " = " + hex(cpu.read(val + cpu.y), 2);
  } else if (cpu.lookup[opcode].addrmode == &Cpu::indirect) {
    lo = cpu.bus->Cpu_read(addr++, true);
    hi = cpu.bus->Cpu_read(addr++, true);
    sInst += hex(lo, 2) + " " + hex(hi, 2);
    sInst += "  " + cpu.lookup[opcode].name + " $" + hex((hi << 8) | lo, 4);
  } else if (cpu.lookup[opcode].addrmode == &Cpu::relative) {
    uint8_t val = cpu.read(addr);
    cpu.relative();
    sInst += hex(val, 2);
    sInst += "    " + cpu.lookup[opcode].name + " $" +
             hex(cpu.PC + cpu.adr_relative, 2);
  }
  out << sInst << std::setw(40 - sInst.length()) << "" << std::left
      << std::setw(30 + std::to_string(cpu.total_cycles).size()) << status
      << "\n";
  cpu.PC = PC_save;
}
#endif
