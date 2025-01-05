#include <cstdint>
#include <ios>
#include <iostream> // for debugging
#include <stdexcept>

#include "bus.h"
#include "cpu.h"

Cpu::Cpu(Bus *bus) : bus{bus} {
  using a = Cpu;

  lookup = {
      {"BRK", nullptr},       {"ORA", &a::ind_X},     {"???", &a::imp},
      {"???", &a::imp},       {"???", &a::imp},       {"ORA", &a::zpg},
      {"ASL", &a::zpg},       {"???", &a::imp},       {"PHP", &a::imp},
      {"ORA", nullptr},       {"ASL", &a::imp},       {"???", &a::imp},
      {"???", &a::imp},       {"ORA", &a::absolute},  {"ASL", &a::absolute},
      {"???", &a::imp},       {"BPL", &a::relative},  {"ORA", &a::ind_Y},
      {"???", &a::imp},       {"???", &a::imp},       {"???", &a::imp},
      {"ORA", &a::zpgX},      {"ASL", &a::zpgX},      {"???", &a::imp},
      {"CLC", &a::imp},       {"ORA", &a::absoluteY}, {"???", &a::imp},
      {"???", &a::imp},       {"???", &a::imp},       {"ORA", &a::absoluteX},
      {"ASL", &a::absoluteX}, {"???", &a::imp},       {"JSR", &a::absolute},
      {"AND", &a::ind_X},     {"???", &a::imp},       {"???", &a::imp},
      {"BIT", &a::zpg},       {"AND", &a::zpg},       {"ROL", &a::zpg},
      {"???", &a::imp},       {"PLP", &a::imp},       {"AND", nullptr},
      {"ROL", &a::imp},       {"???", &a::imp},       {"BIT", &a::absolute},
      {"AND", &a::absolute},  {"ROL", &a::absolute},  {"???", &a::imp},
      {"BMI", &a::relative},  {"AND", &a::ind_Y},     {"???", &a::imp},
      {"???", &a::imp},       {"???", &a::imp},       {"AND", &a::zpgX},
      {"ROL", &a::zpgX},      {"???", &a::imp},       {"SEC", &a::imp},
      {"AND", &a::absoluteY}, {"???", &a::imp},       {"???", &a::imp},
      {"???", &a::imp},       {"AND", &a::absoluteX}, {"ROL", &a::absoluteX},
      {"???", &a::imp},       {"RTI", &a::imp},       {"EOR", &a::ind_X},
      {"???", &a::imp},       {"???", &a::imp},       {"???", &a::imp},
      {"EOR", &a::zpg},       {"LSR", &a::zpg},       {"???", &a::imp},
      {"PHA", &a::imp},       {"EOR", nullptr},       {"LSR", &a::imp},
      {"???", &a::imp},       {"JMP", &a::absolute},  {"EOR", &a::absolute},
      {"LSR", &a::absolute},  {"???", &a::imp},       {"BVC", &a::relative},
      {"EOR", &a::ind_Y},     {"???", &a::imp},       {"???", &a::imp},
      {"???", &a::imp},       {"EOR", &a::zpgX},      {"LSR", &a::zpgX},
      {"???", &a::imp},       {"CLI", &a::imp},       {"EOR", &a::absoluteY},
      {"???", &a::imp},       {"???", &a::imp},       {"???", &a::imp},
      {"EOR", &a::absoluteX}, {"LSR", &a::absoluteX}, {"???", &a::imp},
      {"RTS", &a::imp},       {"ADC", &a::ind_X},     {"???", &a::imp},
      {"???", &a::imp},       {"???", &a::imp},       {"ADC", &a::zpg},
      {"ROR", &a::zpg},       {"???", &a::imp},       {"PLA", &a::imp},
      {"ADC", nullptr},       {"ROR", &a::imp},       {"???", &a::imp},
      {"JMP", &a::indirect},  {"ADC", &a::absolute},  {"ROR", &a::absolute},
      {"???", &a::imp},       {"BVS", &a::relative},  {"ADC", &a::ind_Y},
      {"???", &a::imp},       {"???", &a::imp},       {"???", &a::imp},
      {"ADC", &a::zpgX},      {"ROR", &a::zpgX},      {"???", &a::imp},
      {"SEI", &a::imp},       {"ADC", &a::absoluteY}, {"???", &a::imp},
      {"???", &a::imp},       {"???", &a::imp},       {"ADC", &a::absoluteX},
      {"ROR", &a::absoluteX}, {"???", &a::imp},       {"???", &a::imp},
      {"STA", &a::ind_X},     {"???", &a::imp},       {"???", &a::imp},
      {"STY", &a::zpg},       {"STA", &a::zpg},       {"STX", &a::zpg},
      {"???", &a::imp},       {"DEY", &a::imp},       {"???", &a::imp},
      {"TXA", &a::imp},       {"???", &a::imp},       {"STY", &a::absolute},
      {"STA", &a::absolute},  {"STX", &a::absolute},  {"???", &a::imp},
      {"BCC", &a::relative},  {"STA", &a::ind_Y},     {"???", &a::imp},
      {"???", &a::imp},       {"STY", &a::zpgX},      {"STA", &a::zpgX},
      {"STX", &a::zpgY},      {"???", &a::imp},       {"TYA", &a::imp},
      {"STA", &a::absoluteY}, {"TXS", &a::imp},       {"???", &a::imp},
      {"???", &a::imp},       {"STA", &a::absoluteX}, {"???", &a::imp},
      {"???", &a::imp},       {"LDY", nullptr},       {"LDA", &a::ind_X},
      {"LDX", nullptr},       {"???", &a::imp},       {"LDY", &a::zpg},
      {"LDA", &a::zpg},       {"LDX", &a::zpg},       {"???", &a::imp},
      {"TAY", &a::imp},       {"LDA", nullptr},       {"TAX", &a::imp},
      {"???", &a::imp},       {"LDY", &a::absolute},  {"LDA", &a::absolute},
      {"LDX", &a::absolute},  {"???", &a::imp},       {"BCS", &a::relative},
      {"LDA", &a::ind_Y},     {"???", &a::imp},       {"???", &a::imp},
      {"LDY", &a::zpgX},      {"LDA", &a::zpgX},      {"LDX", &a::zpgY},
      {"???", &a::imp},       {"CLV", &a::imp},       {"LDA", &a::absoluteY},
      {"TSX", &a::imp},       {"???", &a::imp},       {"LDY", &a::absoluteX},
      {"LDA", &a::absoluteX}, {"LDX", &a::absoluteY}, {"???", &a::imp},
      {"CPY", nullptr},       {"CMP", &a::ind_X},     {"???", &a::imp},
      {"???", &a::imp},       {"CPY", &a::zpg},       {"CMP", &a::zpg},
      {"DEC", &a::zpg},       {"???", &a::imp},       {"INY", &a::imp},
      {"CMP", nullptr},       {"DEX", &a::imp},       {"???", &a::imp},
      {"CPY", &a::absolute},  {"CMP", &a::absolute},  {"DEC", &a::absolute},
      {"???", &a::imp},       {"BNE", &a::relative},  {"CMP", &a::ind_Y},
      {"???", &a::imp},       {"???", &a::imp},       {"???", &a::imp},
      {"CMP", &a::zpgX},      {"DEC", &a::zpgX},      {"???", &a::imp},
      {"CLD", &a::imp},       {"CMP", &a::absoluteY}, {"NOP", &a::imp},
      {"???", &a::imp},       {"???", &a::imp},       {"CMP", &a::absoluteX},
      {"DEC", &a::absoluteX}, {"???", &a::imp},       {"CPX", nullptr},
      {"SBC", &a::ind_X},     {"???", &a::imp},       {"???", &a::imp},
      {"CPX", &a::zpg},       {"SBC", &a::zpg},       {"INC", &a::zpg},
      {"???", &a::imp},       {"INX", &a::imp},       {"SBC", nullptr},
      {"NOP", &a::imp},       {"???", &a::imp},       {"CPX", &a::absolute},
      {"SBC", &a::absolute},  {"INC", &a::absolute},  {"???", &a::imp},
      {"BEQ", &a::relative},  {"SBC", &a::ind_Y},     {"???", &a::imp},
      {"???", &a::imp},       {"???", &a::imp},       {"SBC", &a::zpgX},
      {"INC", &a::zpgX},      {"???", &a::imp},       {"SED", &a::imp},
      {"SBC", &a::absoluteY}, {"NOP", &a::imp},       {"???", &a::imp},
      {"???", &a::imp},       {"SBC", &a::absoluteX}, {"INC", &a::absoluteX},
      {"???", &a::imp},
  };
}

Cpu::~Cpu() {}

void Cpu::write(uint16_t adr, uint8_t val) { bus->Cpu_write(adr, val); }

uint8_t Cpu::read(uint16_t adr) const { return bus->Cpu_read(adr); }

// get the flag at the specific bit from status
uint8_t Cpu::get_flag(FLAGS flag) const {
  switch (flag) {
  case FLAGS::C:
    return FLAGS::C & status;
  case FLAGS::Z:
    return FLAGS::Z & status;
  case FLAGS::I:
    return FLAGS::I & status;
  case FLAGS::D:
    return FLAGS::D & status;
  case FLAGS::B:
    return FLAGS::B & status;
  case FLAGS::U:
    return FLAGS::U & status;
  case FLAGS::V:
    return FLAGS::V & status;
  case FLAGS::N:
    return FLAGS::N & status;
  default:
    throw std::runtime_error("Invalid flag");
  }
}

// set bit corresponding to the flag in our status to 0 if on == false
// or 1 otherwise
void Cpu::set_flag(FLAGS flag, bool on) {
  switch (flag) {
  case FLAGS::C: {
    uint8_t mask = 1;
    if (!on)
      status = status & ~mask;
    else
      status = status | mask;
    break;
  }
  case FLAGS::Z: {
    uint8_t mask = 1 << 1;
    if (!on)
      status = status & ~mask;
    else
      status = status | mask;
    break;
  }
  case FLAGS::I: {
    uint8_t mask = 1 << 2;
    if (!on)
      status = status & ~mask;
    else
      status = status | mask;
    break;
  }
  case FLAGS::D: {
    uint8_t mask = 1 << 3;
    if (!on)
      status = status & ~mask;
    else
      status = status | mask;
    break;
  }
  case FLAGS::B: {
    uint8_t mask = 1 << 4;
    if (!on)
      status = status & ~mask;
    else
      status = status | mask;
    break;
  }
  case FLAGS::U: {
    uint8_t mask = 1 << 5;
    if (!on)
      status = status & ~mask;
    else
      status = status | mask;
    break;
  }
  case FLAGS::V: {
    uint8_t mask = 1 << 6;
    if (!on)
      status = status & ~mask;
    else
      status = status | mask;
    break;
  }
  case FLAGS::N: {
    uint8_t mask = 1 << 7;
    if (!on)
      status = status & ~mask;
    else
      status = status | mask;
    break;
  }
  default:
    throw std::runtime_error("Invalid flag");
  }
}

uint8_t Cpu::fetch() {
  fetched = read(adr);
  return fetched;
}

void Cpu::clock() {
  // if we finished the previous instruction, execute new one
  // unlike real hardware, we finish the instruction in a single cycle
  // then wait out the cycles until they reach 0
  if (cycles == 0) {
    opcode = read(PC);

    set_flag(FLAGS::U, 1);
    PC++;

    // can have 1 or 0 additional cycle
    uint8_t additional_cycle = execute_opcode(static_cast<Opcode>(opcode));

    cycles += additional_cycle;
    set_flag(FLAGS::U, 1);
  }

  clock_count++;
  cycles--;
}

bool Cpu::complete() const { return cycles == 0; }

uint8_t Cpu::execute_opcode(Opcode opcode) {
  uint8_t additional = 0x00;
  uint8_t op_result = 0x00;
  switch (opcode) {
  case Opcode::BRK: {
    // PC++; // not sure about this
    cycles = 7;
    set_flag(FLAGS::B, true);
    push(get_high(PC));
    push(get_low(PC));
    push(status);
    uint16_t new_adr = convertTo_16_bit(read(0xFFFF), read(0xFFFE));
    PC = new_adr;
    break;
  }
  case Opcode::BPL: {
    cycles = 2;
    additional = relative();

    if (get_flag(FLAGS::N) == 0) {
      cycles++;

      adr = PC + adr_relative;

      // if high bytes are affected we add a cycle (not sure why)
      if ((adr & 0xFF00) != (PC & 0xFF0))
        cycles++;

      PC = adr;
    }

    op_result = 0;
    break;
  }
  case Opcode::JSR: {
    cycles = 6;
    additional = absolute();
    push(get_high(PC));
    push(get_low(PC));
    fetch();
    PC = fetched;
    break;
  }
  case Opcode::BMI: {
    cycles = 2;
    additional = relative();

    if (get_flag(FLAGS::N) == 1) {
      cycles++;

      adr = PC + adr_relative;

      // if high bytes are affected we add a cycle (not sure why)
      if ((adr & 0xFF00) != (PC & 0xFF0))
        cycles++;

      PC = adr;
    }

    op_result = 0;
    break;
  }
  case Opcode::RTI: {
    cycles = 6;
    status = pull();

    status &= ~FLAGS::B;
    status &= ~FLAGS::U;

    uint8_t low = pull();
    uint8_t high = pull();

    PC = convertTo_16_bit(high, low);
    break;
  }
  case Opcode::BVC: {
    cycles = 2;
    additional = relative();

    if (get_flag(FLAGS::V) == 0) {
      cycles++;

      adr = PC + adr_relative;

      // if high bytes are affected we add a cycle (not sure why)
      if ((adr & 0xFF00) != (PC & 0xFF0))
        cycles++;

      PC = adr;
    }

    op_result = 0;
    break;
  }
  case Opcode::RTS: {
    cycles = 6;
    uint8_t low = pull();
    uint8_t high = pull();

    PC = convertTo_16_bit(high, low);
    break;
  }
  case Opcode::BVS: {
    cycles = 2;
    additional = relative();

    if (get_flag(FLAGS::V) == 1) {
      cycles++;

      adr = PC + adr_relative;

      // if high bytes are affected we add a cycle (not sure why)
      if ((adr & 0xFF00) != (PC & 0xFF0))
        cycles++;

      PC = adr;
    }

    op_result = 0;
    break;
  }
  case Opcode::BCC: {
    cycles = 2;
    additional = relative();

    if (get_flag(FLAGS::C) == 0) {
      cycles++;

      adr = PC + adr_relative;

      // if high bytes are affected we add a cycle (not sure why)
      if ((adr & 0xFF00) != (PC & 0xFF0))
        cycles++;

      PC = adr;
    }

    op_result = 0;
    break;
  }
  case Opcode::LD_immY: {
    cycles = 2;

    uint8_t val = read(PC++);

    y = val;

    update_y_flags();
    return 0;
  }
  case Opcode::BCS: {
    cycles = 2;

    additional = relative();
    op_result = BCS();
    break;
  }
  case Opcode::CP_immY: {
    cycles = 2;
    uint8_t val = read(PC++);

    uint8_t res = y - val;

    set_flag(FLAGS::Z, res == 0x00);
    set_flag(FLAGS::C, y >= val);
    set_flag(FLAGS::N, res >> 7 == 1);
    break;
  }
  case Opcode::BNE: {
    cycles = 2;
    additional = relative();

    if (get_flag(FLAGS::Z) == 0) {
      cycles++;

      adr = PC + adr_relative;

      // if high bytes are affected we add a cycle (not sure why)
      if ((adr & 0xFF00) != (PC & 0xFF0))
        cycles++;

      PC = adr;
    }

    op_result = 0;
    break;
  }
  case Opcode::CP_immX: {
    cycles = 2;
    uint8_t val = read(PC++);

    uint8_t res = x - val;

    set_flag(FLAGS::Z, res == 0x00);
    set_flag(FLAGS::C, x >= val);
    set_flag(FLAGS::N, res >> 7 == 1);
    break;
  }
  case Opcode::BEQ: {
    cycles = 2;
    additional = relative();

    if (get_flag(FLAGS::Z) == 1) {
      cycles++;

      adr = PC + adr_relative;

      // if high bytes are affected we add a cycle (not sure why)
      if ((adr & 0xFF00) != (PC & 0xFF0))
        cycles++;

      PC = adr;
    }

    op_result = 0;
    break;
  }
  case Opcode::ORA_indX:
    cycles = 6;
    additional = ind_X();
    op_result = ORA();
    break;
  case Opcode::ORA_indY:
    cycles = 5;
    additional = ind_Y();
    op_result = ORA();
    break;
  case Opcode::AND_indX:
    cycles = 6;
    additional = ind_X();
    op_result = AND();
    break;
  case Opcode::AND_indY:
    cycles = 5;
    additional = ind_Y();
    op_result = AND();
    break;
  case Opcode::EOR_indX:
    additional = ind_X();
    op_result = EOR();
    cycles = 6;
    break;
  case Opcode::EOR_indY:
    cycles = 5;
    additional = ind_Y();
    op_result = EOR();
    break;
  case Opcode::ADC_indX:
    cycles = 6;
    additional = ind_X();
    op_result = ADC();
    break;
  case Opcode::ADC_indY:
    cycles = 5;
    additional = ind_Y();
    op_result = ADC();
    break;
  case Opcode::STA_indX:
    cycles = 6;
    additional = ind_X();
    op_result = STA();
    break;
  case Opcode::STA_indY:
    cycles = 6;
    additional = ind_Y();
    op_result = STA();
    break;
  case Opcode::LDA_indX:
    cycles = 6;
    additional = ind_X();
    op_result = LDA();
    break;
  case Opcode::LDA_indY:
    cycles = 5;
    additional = ind_Y();
    op_result = LDA();
    break;
  case Opcode::CMP_indX:
    cycles = 6;
    additional = ind_X();
    op_result = CMP();
    break;
  case Opcode::CMP_indY:
    cycles = 5;
    additional = ind_Y();
    op_result = CMP();
    break;
  case Opcode::SBC_indX:
    cycles = 6;
    additional = ind_X();
    op_result = SBC();
    break;
  case Opcode::SBC_indY:
    cycles = 5;
    additional = ind_Y();
    op_result = SBC();
    break;
  case Opcode::LD_immX: {
    cycles = 2;
    uint8_t val = read(PC++);

    x = val;

    update_x_flags();

    return 0;
  }
  case Opcode::BIT_zpg:
    cycles = 3;
    additional = zpg();
    op_result = BIT();
    break;
  case Opcode::ST_zpgY:
    cycles = 3;
    additional = zpg();
    op_result = STY();
    break;
  case Opcode::ST_zpgYX:
    cycles = 4;
    additional = zpgX();
    op_result = STY();
    break;
  case Opcode::LD_zpgY:
    cycles = 3;
    additional = zpg();
    op_result = LDY();
    break;
  case Opcode::LD_zpgYX:
    cycles = 4;
    additional = zpgX();
    op_result = LDY();
    break;
  case Opcode::CP_zpgY:
    cycles = 3;
    additional = zpg();
    op_result = CPY();
    break;
  case Opcode::CP_zpgX:
    cycles = 3;
    additional = zpg();
    op_result = CPX();
    break;
  case Opcode::ORA_zpg:
    cycles = 3;
    additional = zpg();
    op_result = ORA();
    break;
  case Opcode::ORA_zpgX:
    cycles = 4;
    additional = zpgX();
    op_result = ORA();
    break;
  case Opcode::AND_zpg:
    cycles = 3;
    additional = zpg();
    op_result = AND();
    break;
  case Opcode::AND_zpgX:
    cycles = 4;
    additional = zpgX();
    op_result = AND();
    break;
  case Opcode::EOR_zpg:
    cycles = 3;
    additional = zpg();
    op_result = EOR();
    break;
  case Opcode::EOR_zpgX:
    cycles = 4;
    additional = zpgX();
    op_result = EOR();
    break;
  case Opcode::ADC_zpg:
    cycles = 3;
    additional = zpg();
    op_result = ADC();
    break;
  case Opcode::ADC_zpgX:
    cycles = 4;
    additional = zpgX();
    op_result = ADC();
    break;
  case Opcode::STA_zpg:
    cycles = 3;
    additional = zpg();
    op_result = STA();
    break;
  case Opcode::STA_zpgX:
    cycles = 4;
    additional = zpgX();
    op_result = STA();
    break;
  case Opcode::LDA_zpg:
    cycles = 3;
    additional = zpg();
    op_result = LDA();
    break;
  case Opcode::LDA_zpgX:
    cycles = 4;
    additional = zpgX();
    op_result = LDA();
    break;
  case Opcode::CMP_zpg:
    cycles = 3;
    additional = zpg();
    op_result = CMP();
    break;
  case Opcode::CMP_zpgX:
    cycles = 4;
    additional = zpgX();
    op_result = CMP();
    break;
  case Opcode::SBC_zpg:
    cycles = 3;
    additional = zpg();
    op_result = SBC();
    break;
  case Opcode::SBC_zpgX:
    cycles = 4;
    additional = zpgX();
    op_result = SBC();
    break;
  case Opcode::ASL_zpg:
    cycles = 5;
    additional = zpg();
    op_result = ASL();
    break;
  case Opcode::ASL_zpgX:
    cycles = 6;
    additional = zpgX();
    op_result = ASL();
    break;
  case Opcode::ROL_zpg:
    cycles = 5;
    additional = zpg();
    op_result = ROL();
    break;
  case Opcode::ROL_zpgX:
    cycles = 6;
    additional = zpgX();
    op_result = ROL();
    break;
  case Opcode::LSR_zpg:
    cycles = 5;
    additional = zpg();
    op_result = LSR();
    break;
  case Opcode::LSR_zpgX:
    cycles = 6;
    additional = zpgX();
    op_result = LSR();
    break;
  case Opcode::ROR_zpg:
    cycles = 5;
    additional = zpg();
    op_result = ROR();
    break;
  case Opcode::ROR_zpgX:
    cycles = 6;
    additional = zpgX();
    op_result = ROR();
    break;
  case Opcode::STX_zpg:
    cycles = 3;
    additional = zpg();
    op_result = STX();
    break;
  case Opcode::STX_zpgY:
    cycles = 4;
    additional = zpgY();
    op_result = STX();
    break;
  case Opcode::LDX_zpg:
    cycles = 3;
    additional = zpg();
    op_result = LDX();
    break;
  case Opcode::LDX_zpgY:
    cycles = 4;
    additional = zpgY();
    op_result = LDX();
    break;
  case Opcode::DEC_zpg:
    cycles = 5;
    additional = zpg();
    op_result = DEC();
    break;
  case Opcode::DEC_zpgX:
    cycles = 6;
    additional = zpgX();
    op_result = DEC();
    break;
  case Opcode::INC_zpg:
    cycles = 5;
    additional = zpg();
    op_result = INC();
    break;
  case Opcode::INC_zpgX:
    cycles = 6;
    additional = zpgX();
    op_result = INC();
    break;
  case Opcode::PHP_impl:
    cycles = 3;
    push(status);
    break;
  case Opcode::CLC_impl:
    cycles = 2;
    set_flag(FLAGS::C, 0);
    break;
  case Opcode::PLP_impl: {
    cycles = 4;
    uint8_t new_status = pull();
    status = new_status;
    break;
  }
  case Opcode::SEC_impl:
    cycles = 2;
    set_flag(FLAGS::C, 1);
    break;
  case Opcode::PHA_impl:
    cycles = 3;
    push(accumulator);
    break;
  case Opcode::CLI_impl:
    cycles = 2;
    set_flag(FLAGS::I, 0);
    break;
  case Opcode::PLA_impl:
    cycles = 4;
    accumulator = pull();
    update_accumulator_flags();
    break;
  case Opcode::SEI_impl:
    cycles = 2;
    set_flag(FLAGS::I, 1);
    break;
  case Opcode::DEY_impl:
    cycles = 2;
    y -= 1;

    update_y_flags();
    return 0;
  case Opcode::TYA_impl:
    cycles = 2;
    accumulator = y;
    update_accumulator_flags();
    break;
  case Opcode::TAY_impl:
    cycles = 2;
    y = accumulator;
    update_y_flags();
    break;
  case Opcode::CLV_impl:
    cycles = 2;
    set_flag(FLAGS::V, 0);
    break;
  case Opcode::INY_impl:
    cycles = 2;
    y += 1;
    update_y_flags();
    break;
  case Opcode::CLD_impl:
    cycles = 2;
    set_flag(FLAGS::D, 0);
    break;
  case Opcode::INX_impl:
    cycles = 2;
    x += 1;
    update_x_flags();
    break;
  case Opcode::SED_impl:
    cycles = 2;
    set_flag(FLAGS::D, 1);
    break;
  case Opcode::ORA_imm: {
    cycles = 2;
    uint8_t val = read(PC++);

    accumulator |= val;

    update_accumulator_flags();

    return 0;
  }
  case Opcode::ORA_absY: {
    cycles = 4;
    additional = absoluteY();
    op_result = ORA();
    break;
  }
  case Opcode::AND_imm: {
    cycles = 2;
    uint8_t val = read(PC++);
    accumulator = accumulator & val;
    update_accumulator_flags();

    return 0;
    break;
  }
  case Opcode::AND_absY:
    cycles = 4;
    additional = absoluteY();
    op_result = AND();
    break;
  case Opcode::EOR_imm: {
    cycles = 2;
    accumulator = accumulator ^ read(PC++);
    update_accumulator_flags();
    break;
  }
  case Opcode::EOR_absY:
    cycles = 4;
    additional = absoluteY();
    op_result = EOR();
    break;
  case Opcode::ADC_imm: {
    cycles = 2;
    uint8_t val = read(PC++);

    uint16_t temp = static_cast<uint16_t>(val) +
                    static_cast<uint16_t>(accumulator) + get_flag(FLAGS::C);

    set_flag(FLAGS::C, temp > 255);
    set_flag(FLAGS::Z, (temp & 0x00FF) == 0);
    set_flag(FLAGS::N, temp & 0x80);
    set_flag(
        FLAGS::V,
        (~(static_cast<uint16_t>(accumulator) ^ static_cast<uint16_t>(val)) &
         (static_cast<uint16_t>(accumulator) ^ static_cast<uint16_t>(temp))) &
            0x0080);

    accumulator = temp & 0x00FF;

    return 0;
  }
  case Opcode::ADC_absY:
    cycles = 4;
    additional = absoluteY();
    op_result = ADC();
    break;
  case Opcode::STA_absY:
    cycles = 5;
    additional = absoluteY();
    op_result = STA();
    break;
  case Opcode::LDA_imm: {
    cycles = 2;
    accumulator = read(PC++);
    update_accumulator_flags();

    return 0;
  }
  case Opcode::LDA_absY:
    cycles = 4;
    additional = absoluteY();
    op_result = LDA();
    break;
  case Opcode::CMP_imm: {
    cycles = 2;
    uint8_t val = read(PC++);

    set_flag(FLAGS::Z, accumulator == val);
    set_flag(FLAGS::C, accumulator > val);
    set_flag(FLAGS::N, accumulator < val);
    return 0;
  }
  case Opcode::CMP_absY:
    cycles = 4;
    additional = absoluteY();
    op_result = CMP();
    break;
  case Opcode::SBC_imm: {
    cycles = 2;
    uint16_t val = read(PC++) ^ 0xFF;

    uint16_t temp = static_cast<uint16_t>(val) +
                    static_cast<uint16_t>(accumulator) + get_flag(FLAGS::C);

    set_flag(FLAGS::C, temp > 255);
    set_flag(FLAGS::Z, (temp & 0x00FF) == 0);
    set_flag(FLAGS::N, temp & 0x80);
    set_flag(
        FLAGS::V,
        (static_cast<uint16_t>(accumulator) ^ static_cast<uint16_t>(val)) &
            (static_cast<uint16_t>(accumulator) ^ static_cast<uint16_t>(temp)) &
            0x0080);

    accumulator = temp & 0x00FF;

    return 0;
  }
  case Opcode::SBC_absY:
    cycles = 4;
    additional = absoluteY();
    op_result = SBC();
    break;
  case Opcode::ASL_A: {
    cycles = 2;

    set_flag(FLAGS::C, accumulator >> 7);
    accumulator = accumulator << 1;

    update_accumulator_flags();

    return 0;
  }
  case Opcode::ROL_A: {
    cycles = 2;

    uint8_t temp = get_flag(FLAGS::C);
    set_flag(FLAGS::C, accumulator >> 7);
    accumulator = accumulator << 1;

    if (temp)
      accumulator |= 0x01;
    else
      accumulator &= 0xFE;

    update_accumulator_flags();

    return 0;
  }
  case Opcode::LSR_A: {
    cycles = 2;

    set_flag(FLAGS::C, accumulator & 0x01);
    accumulator = accumulator >> 1;

    fetch();
    set_flag(FLAGS::N, 0);
    set_flag(FLAGS::Z, fetched == 0x00);
    return 0;
  }
  case Opcode::ROR_A: {
    cycles = 2;

    uint8_t temp = get_flag(FLAGS::C);
    set_flag(FLAGS::C,
             accumulator << 7); // compiler gives warning, but code is correct
    accumulator = accumulator >> 1;
    if (temp)
      accumulator |= 0x80;
    else
      accumulator &= 0x7F;

    set_flag(FLAGS::Z, accumulator == 0);
    set_flag(FLAGS::N, temp);

    return 0;
  }
  case Opcode::TXA_impl:
    cycles = 2;
    accumulator = x;
    update_accumulator_flags();
    break;
  case Opcode::TXS_impl:
    cycles = 2;
    stack_pointer = x;
    break;
  case Opcode::TAX_impl:
    cycles = 2;
    x = accumulator;
    update_x_flags();
    break;
  case Opcode::TSX_impl:
    cycles = 2;
    x = stack_pointer;
    update_x_flags();
    break;
  case Opcode::DEX_impl:
    cycles = 2;
    x -= 1;

    update_x_flags();
    return 0;
  case Opcode::NOP_impl:
    cycles = 2;

    // TODO
    break;
  case Opcode::BIT_abs:
    cycles = 4;
    additional = absolute();
    op_result = BIT();
    break;
  case Opcode::JMP_abs:
    cycles = 3;
    additional = absolute();
    op_result = JMP();
    break;
  case Opcode::JMP_ind:
    cycles = 5;
    additional = indirect();
    op_result = JMP();
    break;
  case Opcode::STY_abs:
    cycles = 4;
    additional = absolute();
    op_result = STY();
    break;
  case Opcode::LDY_abs:
    cycles = 4;
    additional = absolute();
    op_result = LDY();
    break;
  case Opcode::LDY_absX:
    cycles = 4;
    additional = absoluteX();
    op_result = LDY();
    break;
  case Opcode::CP_absY:
    cycles = 4;
    additional = absolute();
    op_result = CPY();
    break;
  case Opcode::CP_absX:
    cycles = 4;
    additional = absolute();
    op_result = CPX();
    break;
  case Opcode::ORA_abs:
    cycles = 4;
    additional = absolute();
    op_result = ORA();
    break;
  case Opcode::ORA_absX:
    cycles = 4;
    additional = absoluteX();
    op_result = ORA();
    break;
  case Opcode::AND_abs:
    cycles = 4;
    additional = absolute();
    op_result = AND();
    break;
  case Opcode::AND_absX:
    cycles = 4;
    additional = absoluteX();
    op_result = AND();
    break;
  case Opcode::EOR_abs:
    cycles = 4;
    additional = absolute();
    op_result = EOR();
    break;
  case Opcode::EOR_absX:
    cycles = 4;
    additional = absoluteX();
    op_result = EOR();
    break;
  case Opcode::ADC_abs:
    cycles = 4;
    additional = absolute();
    op_result = ADC();
    break;
  case Opcode::ADC_absX:
    cycles = 4;
    additional = absoluteX();
    op_result = ADC();
    break;
  case Opcode::STA_abs:
    cycles = 4;
    additional = absolute();
    op_result = STA();
    break;
  case Opcode::STA_absX:
    cycles = 5;
    additional = absoluteX();
    op_result = STA();
    break;
  case Opcode::LDA_abs:
    cycles = 4;
    additional = absolute();
    op_result = LDA();
    break;
  case Opcode::LDA_absX:
    cycles = 4;
    additional = absoluteX();
    op_result = LDA();
    break;
  case Opcode::CMP_abs:
    cycles = 4;
    additional = absolute();
    op_result = CMP();
    break;
  case Opcode::CMP_absX:
    cycles = 4;
    additional = absoluteX();
    op_result = CMP();
    break;
  case Opcode::SBC_abs:
    cycles = 4;
    additional = absolute();
    op_result = SBC();
    break;
  case Opcode::SBC_absX:
    cycles = 4;
    additional = absoluteX();
    op_result = SBC();
    break;
  case Opcode::ASL_abs:
    cycles = 6;
    additional = absolute();
    op_result = ASL();
    break;
  case Opcode::ASL_absX:
    cycles = 7;
    additional = absoluteX();
    op_result = ASL();
    break;
  case Opcode::ROL_abs:
    cycles = 6;
    additional = absolute();
    op_result = ROL();
    break;
  case Opcode::ROL_absX:
    cycles = 7;
    additional = absoluteX();
    op_result = ROL();
    break;
  case Opcode::LSR_abs:
    cycles = 6;
    additional = absolute();
    op_result = LSR();
    break;
  case Opcode::LSR_absX:
    cycles = 7;
    additional = absoluteX();
    op_result = LSR();
    break;
  case Opcode::ROR_abs:
    cycles = 6;
    additional = absolute();
    op_result = ROR();
    break;
  case Opcode::ROR_absX:
    cycles = 7;
    additional = absoluteX();
    op_result = ROR();
    break;
  case Opcode::STX_abs:
    cycles = 4;
    additional = absolute();
    op_result = STX();
    break;
  case Opcode::LDX_abs:
    cycles = 4;
    additional = absolute();
    op_result = LDX();
    break;
  case Opcode::LDX_absY:
    cycles = 4;
    additional = absoluteY();
    op_result = LDX();
    break;
  case Opcode::DEC_abs:
    cycles = 6;
    additional = absolute();
    op_result = DEC();
    break;
  case Opcode::DEC_absX:
    cycles = 7;
    additional = absoluteX();
    op_result = DEC();
    break;
  case Opcode::INC_abs:
    cycles = 6;
    additional = absolute();
    op_result = INC();
    break;
  case Opcode::INC_absX:
    cycles = 7;
    additional = absoluteX();
    op_result = INC();
    break;
  }

  return op_result & additional;
}

void Cpu::irq() {
  // interrupt vector is stored at address 0xFFFE and 0xFFFF
  // Since the ram stores 2 8bit addresses, ^lower byte, ^higher byte
  // to make up an address on the RAM for PC

  uint8_t pc_low = PC & 0x00FF;
  uint8_t pc_high = PC >> 8;

  push(pc_high);
  push(pc_low);

  set_flag(FLAGS::I, 1);
  set_flag(FLAGS::B, 0);
  set_flag(FLAGS::U, 1);
  push(status);

  uint8_t low = read(0xFFFE);
  uint8_t high = read(0xFFFF);

  PC = (high << 8) | low;

  cycles = 7;
}

void Cpu::nmi() {
  // NMI vector is at FFFA
  uint8_t pc_low = PC & 0x00FF;
  uint8_t pc_high = PC >> 8;

  push(pc_high);
  push(pc_low);

  set_flag(FLAGS::I, 1);
  set_flag(FLAGS::B, 0);
  set_flag(FLAGS::U, 1);
  push(status);

  uint8_t low = read(0xFFFA);
  uint8_t high = read(0xFFFA + 1);

  PC = (high << 8) | low;

  cycles = 8;
}

void Cpu::reset() {
  stack_pointer = 0xFD; // might change
  accumulator = 0x00;
  x = 0x00;
  y = 0x00;

  status = 0x00 | FLAGS::U;
  // reset vector is hard coded at FFFC
  adr = 0xFFFC;
  uint8_t low = read(adr);
  uint8_t high = read(adr + 1);
  PC = convertTo_16_bit(0xC0, 0x00);

  fetched = 0x00;
  adr = 0x0000;
  adr_relative = 0x00;

  cycles = 8;
}

void Cpu::push(uint8_t val) {
  // UNSURE about correctness
  uint16_t adr = convertTo_16_bit(0x01, stack_pointer);

  write(adr, val);
  stack_pointer--;
}

uint8_t Cpu::pull() {
  // UNSURE about correctness
  uint16_t adr = convertTo_16_bit(0x01, stack_pointer);

  uint8_t val = fetch();
  write(adr, 0x00);
  stack_pointer++;

  return val;
}

uint16_t convertTo_16_bit(uint8_t high, uint8_t low) {
  return (high << 8) | low;
}

uint8_t Cpu::zpg() {
  adr = read(PC++);
  adr &= 0x00FF;

  return 0;
}

uint8_t Cpu::zpgX() {
  adr = wrap_around(read(PC++), x);

  return 0;
}

uint8_t Cpu::zpgY() {
  adr = wrap_around(read(PC++), y);

  return 0;
}

uint8_t Cpu::relative() {
  adr_relative = read(PC++);

  // check if first bit is 1 (negative number)
  if (0x80 & adr_relative) {
    adr_relative |= 0xFF00;
  }
  return 0;
}

uint8_t Cpu::absolute() {
  uint8_t low = read(PC++);
  uint8_t high = read(PC++);

  adr = (high << 8) | low;

  return 0;
}

uint8_t Cpu::absoluteX() {
  uint8_t low = read(PC++);
  uint8_t high = read(PC++);
  adr = (high << 8) | low;

  adr += x;

  if ((adr & 0xFF00) != (high << 8))
    return 1;
  else
    return 0;
}

uint8_t Cpu::absoluteY() {
  uint8_t low = read(PC++);
  uint8_t high = read(PC++);
  adr = (high << 8) | low;

  adr += y;

  if ((adr & 0xFF00) != (high << 8))
    return 1;
  else
    return 0;
}

uint8_t Cpu::indirect() {
  uint8_t low = read(PC++);
  uint8_t high = read(PC++);

  uint16_t ptr = (high << 8) | low;

  // this simulates a bug that was found in the NES hardware
  if (low == 0x00FF) {
    adr = (read(ptr & 0xFF00) << 8) | read(ptr);
  } else { // actual normal behaviour
    adr = (read(ptr + 1) << 8) | read(ptr);
  }
  return 0;
}

uint8_t Cpu::ind_X() {
  uint8_t tmp = wrap_around(read(PC++), x);

  uint8_t low = read(static_cast<uint16_t>(tmp));
  uint8_t high = read(static_cast<uint16_t>(wrap_around(tmp, 1)));

  uint16_t eff_adr = (high << 8) | low;

  adr = eff_adr;
  return 0;
}

uint8_t Cpu::ind_Y() {
  uint8_t adr = zpg();

  uint8_t low = read(static_cast<uint16_t>(adr));
  uint8_t high = read(static_cast<uint16_t>(wrap_around(adr, 1)));

  uint16_t eff_adr = ((high << 8) | low) + y;

  adr = eff_adr;

  if ((adr & 0xFF00) != (high << 8))
    return 1;
  else
    return 0;
}

uint8_t Cpu::AND() {
  uint8_t val = fetch();
  accumulator = accumulator & val;
  update_accumulator_flags();

  return 1;
}

uint8_t Cpu::BCS() {
  if (get_flag(FLAGS::C) == 1) {
    cycles++;

    adr = PC + adr_relative;

    // if high bytes are affected we add a cycle (not sure why)
    if ((adr & 0xFF00) != (PC & 0xFF0))
      cycles++;

    PC = adr;
  }

  return 0;
}

uint8_t Cpu::LDY() {
  uint8_t val = fetch();

  y = val;

  update_y_flags();
  return 1;
}

uint8_t Cpu::LDX() {
  uint8_t val = fetch();

  x = val;

  update_x_flags();
  return 1;
}

uint8_t Cpu::BIT() {
  fetch();

  uint8_t temp = accumulator & fetched;

  set_flag(FLAGS::Z, temp == 0x00);
  set_flag(FLAGS::N, fetched >> 7);
  set_flag(FLAGS::V, fetched & 0x40);

  return 0;
}

uint8_t Cpu::CPX() {
  uint8_t val = fetch();

  set_flag(FLAGS::Z, x == val);
  set_flag(FLAGS::C, x > val);
  set_flag(FLAGS::N, x < val);

  return 0;
}

uint8_t Cpu::CMP() {
  uint8_t val = fetch();

  set_flag(FLAGS::Z, accumulator == val);
  set_flag(FLAGS::C, accumulator > val);
  set_flag(FLAGS::N, accumulator < val);

  return 1;
}

uint8_t Cpu::CPY() {
  uint8_t val = fetch();

  set_flag(FLAGS::Z, y == val);
  set_flag(FLAGS::C, y > val);
  set_flag(FLAGS::N, y < val);

  return 0;
}

uint8_t Cpu::ORA() {
  uint8_t val = fetch();

  accumulator |= val;

  update_accumulator_flags();

  return 1;
}

uint8_t Cpu::ADC() {
  uint8_t val = fetch();

  uint16_t temp = static_cast<uint16_t>(val) +
                  static_cast<uint16_t>(accumulator) + get_flag(FLAGS::C);

  set_flag(FLAGS::C, temp > 255);
  set_flag(FLAGS::Z, (temp & 0x00FF) == 0);
  set_flag(FLAGS::N, temp & 0x80);
  set_flag(
      FLAGS::V,
      (~(static_cast<uint16_t>(accumulator) ^ static_cast<uint16_t>(val)) &
       (static_cast<uint16_t>(accumulator) ^ static_cast<uint16_t>(temp))) &
          0x0080);

  accumulator = temp & 0x00FF;

  return 1;
}

uint8_t Cpu::SBC() {
  fetch();

  uint16_t val = fetched ^ 0xFF;

  uint16_t temp = static_cast<uint16_t>(val) +
                  static_cast<uint16_t>(accumulator) + get_flag(FLAGS::C);

  set_flag(FLAGS::C, temp > 255);
  set_flag(FLAGS::Z, (temp & 0x00FF) == 0);
  set_flag(FLAGS::N, temp & 0x80);
  set_flag(
      FLAGS::V,
      (static_cast<uint16_t>(accumulator) ^ static_cast<uint16_t>(val)) &
          (static_cast<uint16_t>(accumulator) ^ static_cast<uint16_t>(temp)) &
          0x0080);

  accumulator = temp & 0x00FF;

  return 1;
}

uint8_t Cpu::EOR() {
  fetch();
  accumulator = accumulator ^ fetched;
  update_accumulator_flags();

  return 1;
}

uint8_t Cpu::STA() {
  write(adr, accumulator);

  return 0;
}

uint8_t Cpu::STY() {
  write(adr, y);

  return 0;
}

uint8_t Cpu::STX() {
  write(adr, x);

  return 0;
}

uint8_t Cpu::ASL() {
  fetch();

  set_flag(FLAGS::C, fetched >> 7);
  write(adr, fetched << 1);

  fetch();
  set_flag(FLAGS::N, fetched >> 7);
  set_flag(FLAGS::Z, fetched == 0x00);
  return 0;
}

uint8_t Cpu::DEC() {
  write(adr, fetch() - 1);
  fetch();

  set_flag(FLAGS::Z, fetched == 0);
  set_flag(FLAGS::N, fetched >> 7);
  return 0;
}

uint8_t Cpu::INC() {
  write(adr, fetch() + 1);
  fetch();

  set_flag(FLAGS::Z, fetched == 0);
  set_flag(FLAGS::N, fetched >> 7);
  return 0;
}

uint8_t Cpu::ROL() {
  fetch();

  uint8_t temp = get_flag(FLAGS::C);
  set_flag(FLAGS::C, fetched >> 7);
  fetched = fetched << 1;

  if (temp)
    fetched |= 0x01;
  else
    fetched &= 0xFE;

  write(adr, fetched);
  set_flag(FLAGS::N, fetched >> 7);
  set_flag(FLAGS::Z, fetched == 0x00);
  return 0;
}

uint8_t Cpu::ROR() {
  fetch();

  uint8_t temp = get_flag(FLAGS::C);
  set_flag(FLAGS::C,
           fetched << 7); // compiler gives warning, but code is correct
  fetched = fetched >> 1;
  if (temp)
    fetched |= 0x80;
  else
    fetched &= 0x7F;

  set_flag(FLAGS::Z, fetched == 0);
  set_flag(FLAGS::N, temp);

  return 0;
}

uint8_t Cpu::LSR() {
  fetch();

  set_flag(FLAGS::C, fetched & 0x01);
  write(adr, fetched >> 1);

  fetch();
  set_flag(FLAGS::N, 0);
  set_flag(FLAGS::Z, fetched == 0x00);
  return 0;
}

uint8_t Cpu::JMP() {
  fetch();
  PC = fetched;

  return 0;
}

uint8_t Cpu::LDA() {
  fetch();

  accumulator = fetched;
  update_accumulator_flags();

  return 1;
}

void Cpu::update_accumulator_flags() {
  set_flag(FLAGS::Z, accumulator == 0x00);
  set_flag(FLAGS::N, accumulator >> 7 == 1);
}

void Cpu::update_y_flags() {
  set_flag(FLAGS::Z, y == 0x00);
  set_flag(FLAGS::N, y >> 7 == 1);
}

void Cpu::update_x_flags() {
  set_flag(FLAGS::Z, x == 0x00);
  set_flag(FLAGS::N, x >> 7 == 1);
}

uint8_t wrap_around(uint8_t val1, uint8_t val2) { return (val1 + val2) & 0xFF; }

uint8_t sign_extend(uint8_t x, int bit_count) {
  if ((x >> (bit_count - 1)) & 1) {
    x |= (0xFF << bit_count);
  }
  return x;
}

uint8_t get_low(uint16_t val) { return 0x00FF & val; }

uint8_t get_high(uint16_t val) { return val >> 8; }

/*
Addressing modes:

bytes include opcode

implicit: always 1 byte, no extra address anywhere

accumulator: Also 1 byte, do smth with accumulator, not sure

immediate: 2 bytes, and we can specify a constant (not address) in the
instruction

zero paging: 2 bytes, second byte is an address

zero paging X: 2 bytes, second byte is address & we add it to value in X
register

zero paging Y: 2 bytes, second byte is address & we add it to value in Y
register

Relative: 2 bytes. After instruction executes, check if 0 flag is set. If it is,
we jump to new address

Absolute: 3 bytes. The 2 bytes after the first are the full address

AbsoluteX: 3 bytes. The 2 bytes after the first are the full address & we add
address from X register

AbsoluteY: 3 bytes. The 2 bytes after the first are the full address & we add
address from Y register

Indirect: 3 bytes. First byte after the first is the least significant byte, and
one after is most significant

i.e: If we have 0xAB, followed by 0xFF, address is 0xFFAB

Indexed Indirect (ind_X): 2 bytes. Add address from Y with wraparound (probably
will need function for that): This is an address. Value of that address is Low
byte. Value of address right after is high byte. Combine them, and retrieve the
value from memory

Indirect Indexed: 2 bytes. Value from second byte -> low byte. Value of address
right after second byte -> high byte. Combine them to get an address. Add Y to
the address with wraparound to the lower byte

*/
