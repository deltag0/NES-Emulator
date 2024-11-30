#include <stdexcept>

#include "cpu.h"
#include "bus.h"

uint8_t wrap_around(uint8_t val1, uint8_t val2);
uint8_t get_high(uint16_t val);
uint8_t get_low(uint16_t val);

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
        if (!on) status = status & ~mask;
        else status = status | mask;
    }
    case FLAGS::Z: {
        uint8_t mask = 1 << 1;
        if (!on) status = status & ~mask;
        else status = status | mask;
    }
    case FLAGS::I: {
        uint8_t mask = 1 << 2;
        if (!on) status = status & ~mask;
        else status = status | mask;
    }
    case FLAGS::D: {
        uint8_t mask = 1 << 3;
        if (!on) status = status & ~mask;
        else status = status | mask;
    }
    case FLAGS::B: {
        uint8_t mask = 1 << 4;
        if (!on) status = status & ~mask;
        else status = status | mask;
    }
    case FLAGS::U: {
        uint8_t mask = 1 << 5;
        if (!on) status = status & ~mask;
        else status = status | mask;
    }
    case FLAGS::V: {
        uint8_t mask = 1 << 6;
        if (!on) status = status & ~mask;
        else status = status | mask;
    }
    case FLAGS::N: {
        uint8_t mask = 1 << 7;
        if (!on) status = status & ~mask;
        else status = status | mask;
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
        PC++;

        // can have 1 or 0 additional cycle
        uint8_t additional_cycle =  execute_opcode(static_cast<Opcode>(opcode));

        cycles += additional_cycle;
    }

    cycles--;
}

uint8_t Cpu::execute_opcode(Opcode opcode) {
    uint8_t additional = 0x00;
    uint8_t op_result = 0x00;
    switch (opcode) {
        case Opcode::BRK: {
            PC++;
            cycles = 7;
            set_flag(FLAGS::B, true);
            set_flag(FLAGS::I, true);
            push(get_high(PC));
            push(get_low(PC));
            push(status);
            uint16_t new_adr = convertTo_16_bit(read(0xFFFF), read(0xFFFE));
            PC = new_adr;
            break;
        }
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

            additional = relative();
            op_result = BCS();
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
            additional = ind_X();
            op_result = AND();
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

    return op_result & additional;
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

    PC = (high << 8) | low;

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
    uint16_t adr = convertTo_16_bit(0x01, stack_pointer);

    write(adr, val);
    stack_pointer--;
}

uint8_t Cpu::pull() {
    // UNSURE about correctness
    uint16_t adr = convertTo_16_bit(0x01, stack_pointer);

    uint8_t val = read(adr);
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
    adr = read(PC++) + x;
    adr &= 0x00FF;

    return 0;
}

uint8_t Cpu::zpgY() {
    adr = read(PC++) + y;
    adr &= 0x00FF;

    return 0;
}

uint8_t Cpu::relative() {
    adr_relative = read(PC++);
    if (FLAGS::V & adr_relative) {
        adr_relative |= 0xFF00;
    }
    return 0;
}

uint16_t Cpu::absolute() {
    uint8_t low = read(PC++);
    uint8_t high = read(PC++);

    adr = (high << 8) | low;

    return 0;
}

uint16_t Cpu::absoluteX() {
    uint8_t low = read(PC++);
    uint8_t high = read(PC++);
    adr = (high << 8) | low;

    adr += x;
    
    if ((adr & 0xFF00) != (high << 8)) return 1;
    else return 0;
}

uint16_t Cpu::absoluteY() {
    uint8_t low = read(PC++);
    uint8_t high = read(PC++);
    adr = (high << 8) | low;

    adr += y;
    
    if ((adr & 0xFF00) != (high << 8)) return 1;
    else return 0;
}

uint8_t Cpu::indirect() {
    uint16_t eff_adr = absolute();

    adr = (read(eff_adr + 1) << 8) | read(eff_adr);

    return 0;
}

uint16_t Cpu::ind_X() {
    uint8_t adr = read(PC++) + x;

    uint8_t low = read(static_cast<uint16_t>(adr));
    uint8_t high = read(static_cast<uint16_t>(adr + 1));

    uint16_t eff_adr = (high << 8) | low;

    adr = eff_adr;
    return 0;
}

uint16_t Cpu::ind_Y() {
    uint8_t adr = zpg();

    uint8_t low = read(static_cast<uint16_t>(adr));
    uint8_t high = read(static_cast<uint16_t>(adr + 1));

    uint16_t eff_adr = ((high << 8) | low) + y;

    adr = eff_adr;

    if ((adr & 0xFF00) != (high << 8)) return 1;
    else return 0;
}

uint8_t Cpu::AND() {
    uint8_t val = fetch();
    accumulator = accumulator & val;
    set_flag(FLAGS::N, accumulator >> 7 == 1);
    set_flag(FLAGS::Z, 0x00 == accumulator);

    return 1;
}

uint8_t Cpu::BCS() {
    if (get_flag(FLAGS::C) == 1) {
        cycles++;

        adr = PC + adr_relative;

        // if high bytes are affected we add a cycle (not sure why)
        if ((adr & 0xFF00) != (PC & 0xFF0)) cycles++;

        PC = adr;
    }

    return 0;
}


uint8_t wrap_around(uint8_t val1, uint8_t val2) {
    return (val1 + val2) & 0xFF;
}

uint8_t sign_extend(uint8_t x, int bit_count) {
    if ((x >> (bit_count - 1)) & 1) {
        x |= (0xFF << bit_count);
    }
    return x;
}

uint8_t get_low(uint16_t val) {
    return 0x00FF & val;
}

uint8_t get_high(uint16_t val) {
    return val >> 8;
}

/*
Addressing modes:

bytes include opcode

implicit: always 1 byte, no extra address anywhere

accumulator: Also 1 byte, do smth with accumulator, not sure

immediate: 2 bytes, and we can specify a constant (not address) in the instruction

zero paging: 2 bytes, second byte is an address

zero paging X: 2 bytes, second byte is address & we add it to value in X register

zero paging Y: 2 bytes, second byte is address & we add it to value in Y register

Relative: 2 bytes. After instruction executes, check if 0 flag is set. If it is, we jump to new address

Absolute: 3 bytes. The 2 bytes after the first are the full address

AbsoluteX: 3 bytes. The 2 bytes after the first are the full address & we add address from X register

AbsoluteY: 3 bytes. The 2 bytes after the first are the full address & we add address from Y register

Indirect: 3 bytes. First byte after the first is the least significant byte, and one after is most significant

i.e: If we have 0xAB, followed by 0xFF, address is 0xFFAB

Indexed Indirect (ind_X): 2 bytes. Add address from Y with wraparound (probably will need function for that): This is an address. Value of that address is Low byte. Value of address right after is high byte. Combine them, and retrieve the value from memory

Indirect Indexed: 2 bytes. Value from second byte -> low byte. Value of address right after second byte -> high byte. Combine them to get an address. Add Y to the address with wraparound to the lower byte

*/