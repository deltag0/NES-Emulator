#include <stdexcept>

#include "cpu.h"
#include "bus.h"


Cpu::Cpu(Bus *bus): bus{bus} {}

Cpu::~Cpu() {}

void Cpu::write(uint16_t adr, uint8_t val) {
    bus->write(adr, val);
}

uint8_t Cpu::read(uint16_t adr) const {
    return bus->read(adr);
}

// get the flag at the specific bit from status
uint8_t Cpu::get_flag(FLAGS flag) const {
    switch (flag)
    {
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
    switch (flag)
    {
    case FLAGS::C: {
        uint8_t mask = 1;
        if (!flag) status = status & ~mask;
        else status = status | mask;
    }
    case FLAGS::Z: {
        uint8_t mask = 1 << 1;
        if (!flag) status = status & ~mask;
        else status = status | mask;
    }
    case FLAGS::I: {
        uint8_t mask = 1 << 2;
        if (!flag) status = status & ~mask;
        else status = status | mask;
    }
    case FLAGS::D: {
        uint8_t mask = 1 << 3;
        if (!flag) status = status & ~mask;
        else status = status | mask;
    }
    case FLAGS::B: {
        uint8_t mask = 1 << 4;
        if (!flag) status = status & ~mask;
        else status = status | mask;
    }
    case FLAGS::U: {
        uint8_t mask = 1 << 5;
        if (!flag) status = status & ~mask;
        else status = status | mask;
    }
    case FLAGS::V: {
        uint8_t mask = 1 << 6;
        if (!flag) status = status & ~mask;
        else status = status | mask;
    }
    case FLAGS::N: {
        uint8_t mask = 1 << 7;
        if (!flag) status = status & ~mask;
        else status = status | mask;
    }
    default:
        throw std::runtime_error("Invalid flag");
    }
}

uint8_t Cpu::fetch() {
    fetched = read(PC);
    return fetched;
}

void Cpu::clock() {
    // if we finished the previous instruction, execute new one
    if (cycles == 0) {
        opcode = read(PC);
        PC++;

        // can have 1 or 0 additional cycle
        uint8_t additional_cycle =  execute_opcode(static_cast<Opcode>(opcode));

        cycles += additional_cycle;
    }
}

uint8_t Cpu::execute_opcode(Opcode opcode) {
    switch (opcode) {
        case Opcode::BRK: 
            cycles = 7; 
            break;
        case Opcode::BPL: 
            cycles = 2; 
            break;
        case Opcode::JSR:
            cycles = 6; 
            break;
        case Opcode::BMI:
            cycles = 2; 
            break;
        case Opcode::RTI:
            cycles = 6; 
            break;
        case Opcode::BVC:
            cycles = 2; 
            break;
        case Opcode::RTS:
            cycles = 6; 
            break;
        case Opcode::BVS:
            cycles = 2; 
            break;
        case Opcode::BCC:
            cycles = 2; 
            break;
        case Opcode::LD_immY:
            cycles = 2; 
            break;
        case Opcode::BCS:
            cycles = 2; 
            break;
        case Opcode::CP_immY:
            cycles = 2; 
            break;
        case Opcode::BNE:
            cycles = 2; 
            break;
        case Opcode::CP_immX:
            cycles = 2; 
            break;
        case Opcode::BEQ:
            cycles = 2; 
            break;
        case Opcode::ORA_indX:
            cycles = 6; 
            break;
        case Opcode::ORA_indY:
            cycles = 5; 
            break;
        case Opcode::AND_indX:
            cycles = 6; 
            break;
        case Opcode::AND_indY:
            cycles = 5; 
            break;
        case Opcode::EOR_indX:
            cycles = 6; 
            break;
        case Opcode::EOR_indY:
            cycles = 5; 
            break;
        case Opcode::ADC_indX:
            cycles = 6; 
            break;
        case Opcode::ADC_indY:
            cycles = 5; 
            break;
        case Opcode::STA_indX:
            cycles = 6; 
            break;
        case Opcode::STA_indY:
            cycles = 6; 
            break;
        case Opcode::LDA_indX:
            cycles = 6; 
            break;
        case Opcode::LDA_indY:
            cycles = 5; 
            break;
        case Opcode::CMP_indX:
            cycles = 6; 
            break;
        case Opcode::CMP_indY:
            cycles = 5; 
            break;
        case Opcode::SBC_indX:
            cycles = 6; 
            break;
        case Opcode::SBC_indY:
            cycles = 5; 
            break;
        case Opcode::LD_immX:
            cycles = 2; 
            break;
        case Opcode::BIT_zpg:
            cycles = 3; 
            break;
        case Opcode::ST_zpgY:
            cycles = 3; 
            break;
        case Opcode::ST_zpgYX:
            cycles = 4; 
            break;
        case Opcode::LD_zpgY:
            cycles = 3; 
            break;
        case Opcode::LD_zpgYX:
            cycles = 4; 
            break;
        case Opcode::CP_zpgY:
            cycles = 3; 
            break;
        case Opcode::CP_zpgX:
            cycles = 3; 
            break;
        case Opcode::ORA_zpg:
            cycles = 3; 
            break;
        case Opcode::ORA_zpgX:
            cycles = 4; 
            break;
        case Opcode::AND_zpg:
            cycles = 3; 
            break;
        case Opcode::AND_zpgX:
            cycles = 4; 
            break;
        case Opcode::EOR_zpg:
            cycles = 3; 
            break;
        case Opcode::EOR_zpgX:
            cycles = 4; 
            break;
        case Opcode::ADC_zpg:
            cycles = 3; 
            break;
        case Opcode::ADC_zpgX:
            cycles = 4; 
            break;
        case Opcode::STA_zpg:
            cycles = 3; 
            break;
        case Opcode::STA_zpgX:
            cycles = 4; 
            break;
        case Opcode::LDA_zpg:
            cycles = 3; 
            break;
        case Opcode::LDA_zpgX:
            cycles = 4; 
            break;
        case Opcode::CMP_zpg:
            cycles = 3; 
            break;
        case Opcode::CMP_zpgX:
            cycles = 4; 
            break;
        case Opcode::SBC_zpg:
            cycles = 3; 
            break;
        case Opcode::SBC_zpgX:
            cycles = 4; 
            break;
        case Opcode::ASL_zpg:
            cycles = 5; 
            break;
        case Opcode::ASL_zpgX:
            cycles = 6; 
            break;
        case Opcode::ROL_zpg:
            cycles = 5; 
            break;
        case Opcode::ROL_zpgX:
            cycles = 6; 
            break;
        case Opcode::LSR_spg:
            cycles = 5; 
            break;
        case Opcode::LSR_zpgX:
            cycles = 6; 
            break;
        case Opcode::ROR_zpg:
            cycles = 5; 
            break;
        case Opcode::ROR_zpgX:
            cycles = 6; 
            break;
        case Opcode::STX_zpg:
            cycles = 3; 
            break;
        case Opcode::STX_zpgY:
            cycles = 4; 
            break;
        case Opcode::LDX_zpg:
            cycles = 3; 
            break;
        case Opcode::LDX_zpgY:
            cycles = 4; 
            break;
        case Opcode::DEC_zpg:
            cycles = 5; 
            break;
        case Opcode::DEC_zpgX:
            cycles = 6; 
            break;
        case Opcode::INC_zpg:
            cycles = 5; 
            break;
        case Opcode::INC_zpgX:
            cycles = 6; 
            break;
        case Opcode::PHP_impl:
            cycles = 3; 
            break;
        case Opcode::CLC_impl:
            cycles = 2; 
            break;
        case Opcode::PLP_impl:
            cycles = 4; 
            break;
        case Opcode::SEC_impl:
            cycles = 2; 
            break;
        case Opcode::PHA_impl:
            cycles = 3; 
            break;
        case Opcode::CLI_impl:
            cycles = 2; 
            break;
        case Opcode::PLA_impl:
            cycles = 4; 
            break;
        case Opcode::SEI_impl:
            cycles = 2; 
            break;
        case Opcode::DEY_impl:
            cycles = 2; 
            break;
        case Opcode::TYA_impl:
            cycles = 2; 
            break;
        case Opcode::TAY_impl:
            cycles = 2; 
            break;
        case Opcode::CLV_impl:
            cycles = 2; 
            break;
        case Opcode::INY_impl:
            cycles = 2; 
            break;
        case Opcode::CLD_impl:
            cycles = 2; 
            break;
        case Opcode::INX_impl:
            cycles = 2; 
            break;
        case Opcode::SED_impl:
            cycles = 2; 
            break;
        case Opcode::ORA_imm:
            cycles = 2; 
            break;
        case Opcode::ORA_absY:
            cycles = 4; 
            break;
        case Opcode::AND_imm:
            cycles = 2; 
            break;
        case Opcode::AND_absY:
            cycles = 4; 
            break;
        case Opcode::EOR_imm:
            cycles = 2; 
            break;
        case Opcode::EOR_absY:
            cycles = 4; 
            break;
        case Opcode::ADC_imm:
            cycles = 2; 
            break;
        case Opcode::ADC_absY:
            cycles = 4; 
            break;
        case Opcode::STA_absY:
            cycles = 5; 
            break;
        case Opcode::LDA_imm:
            cycles = 2; 
            break;
        case Opcode::LDA_absY:
            cycles = 4; 
            break;
        case Opcode::CMP_imm:
            cycles = 2; 
            break;
        case Opcode::CMP_absY:
            cycles = 4; 
            break;
        case Opcode::SBC_imm:
            cycles = 2; 
            break;
        case Opcode::SBC_absY:
            cycles = 4; 
            break;
        case Opcode::ASL_A:
            cycles = 2; 
            break;
        case Opcode::ROL_A:
            cycles = 2; 
            break;
        case Opcode::LSR_A:
            cycles = 2; 
            break;
        case Opcode::ROR_A:
            cycles = 2; 
            break;
        case Opcode::TXA_impl:
            cycles = 2; 
            break;
        case Opcode::TXS_impl:
            cycles = 2; 
            break;
        case Opcode::TAX_impl:
            cycles = 2; 
            break;
        case Opcode::TSX_impl:
            cycles = 2; 
            break;
        case Opcode::DEX_impl:
            cycles = 2; 
            break;
        case Opcode::NOP_impl: 
            cycles = 2; 
            break;
        case Opcode::BIT_abs: 
            cycles = 4;
            break;
        case Opcode::JMP_abs: 
            cycles = 3;
            break;
        case Opcode::JMP_ind: 
            cycles = 5;
            break;
        case Opcode::STY_abs: 
            cycles = 4;
            break;
        case Opcode::LDY_abs: 
            cycles = 4;
            break;
        case Opcode::LDY_absX: 
            cycles = 4;
            break;
        case Opcode::CP_absY: 
            cycles = 4;
            break;
        case Opcode::CP_absX: 
            cycles = 4;
            break;
        case Opcode::ORA_abs: 
            cycles = 4;
            break;
        case Opcode::ORA_absX: 
            cycles = 4;
            break;
        case Opcode::AND_abs: 
            cycles = 4;
            break;
        case Opcode::AND_absX: 
            cycles = 4;
            break;
        case Opcode::EOR_abs: 
            cycles = 4;
            break;
        case Opcode::EOR_absX: 
            cycles = 4;
            break;
        case Opcode::ADC_abs: 
            cycles = 4;
            break;
        case Opcode::ADC_absX: 
            cycles = 4;
            break;
        case Opcode::STA_abs: 
            cycles = 4;
            break;
        case Opcode::STA_absX: 
            cycles = 5;
            break;
        case Opcode::LDA_abs: 
            cycles = 4;
            break;
        case Opcode::LDA_absX: 
            cycles = 4;
            break;
        case Opcode::CMP_abs: 
            cycles = 4;
            break;
        case Opcode::CMP_absX: 
            cycles = 4;
            break;
        case Opcode::SBC_abs: 
            cycles = 4;
            break;
        case Opcode::SBC_absX: 
            cycles = 4;
            break;
        case Opcode::ASL_abs: 
            cycles = 6;
            break;
        case Opcode::ASL_absX: 
            cycles = 7;
            break;
        case Opcode::ROL_abs: 
            cycles = 6;
            break;
        case Opcode::ROL_absX: 
            cycles = 7;
            break;
        case Opcode::LSR_abs: 
            cycles = 6;
            break;
        case Opcode::LSR_absX: 
            cycles = 7;
            break;
        case Opcode::ROR_abs: 
            cycles = 6;
            break;
        case Opcode::ROR_absX: 
            cycles = 7;
            break;
        case Opcode::STX_abs: 
            cycles = 4;
            break;
        case Opcode::LDX_abs: 
            cycles = 4;
            break;
        case Opcode::LDX_absY: 
            cycles = 4;
            break;
        case Opcode::DEC_abs: 
            cycles = 6;
            break;
        case Opcode::DEC_absX: 
            cycles = 7;
            break;
        case Opcode::INC_abs: 
            cycles = 6;
            break;
        case Opcode::INC_absX: 
            cycles = 7;
            break;
    }
}

void Cpu::irq() {
    // interrupt vector is stored at address 0xFFFE and 0xFFFF
    // Since the ram stores 2 8bit addresses, ^lower byte, ^higher byte 
    // to make up an address on the RAM for PC

    uint8_t pc_low = PC & 0x00FF; // make sure this is right
    uint8_t pc_high = PC >> 8; // make sure this is right

    push(pc_high);
    push(pc_low);
    push(status);

    uint8_t low = read(0xFFFE);
    uint8_t high = read(0xFFFF);

    PC = high << 8 | low;

    set_flag(FLAGS::I, false);
}

void Cpu::reset() {
    PC = 0x0000; // might change

    stack_pointer = 0x00; // might change
    accumulator = 0x00;
    x = 0x00;
    y = 0x00;

    status = 0x00;

    fetched = 0x00;

    cycles = 0x00;
}

void Cpu::push(uint8_t val) {
    // UNSURE about correctness
    uint16_t adr = convertTo_16_bit(0x10, stack_pointer);

    write(adr, val);
    stack_pointer--;
}

uint8_t Cpu::pull() {
    // UNSURE about correctness
    uint16_t adr = convertTo_16_bit(0x10, stack_pointer);

    uint8_t val = read(adr);
    write(adr, 0x00);
    stack_pointer++;

    return val;
}

uint16_t convertTo_16_bit(uint8_t high, uint8_t low) {
    return high << 8 | low;
}