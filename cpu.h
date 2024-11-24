#ifndef CPU_H
#define CPU_H

#include <cstdint>


// all opcodes (possible ways of optimization in future)
enum class Opcode: uint8_t {
    BRK = 0x00,
    BPL = 0x10,
    JSR = 0x20,
    BMI = 0x30,
    RTI = 0x40,
    BVC = 0x50,
    RTS = 0x60,
    BVS = 0x70,
    BCC = 0x90,
    LD_immY = 0xA0,
    BCS = 0xB0,
    CP_immY = 0xC0,
    BNE = 0xD0,
    CP_immX = 0xE0,
    BEQ = 0xF0,
    ORA_indX = 0x01,
    ORA_indY = 0x11,
    AND_indX = 0x21,
    AND_indY = 0x31,
    EOR_indX = 0x41,
    EOR_indY = 0x51,
    ADC_indX = 0x61,
    ADC_indY = 0x71,
    STA_indX = 0x81,
    STA_indY = 0x91,
    LDA_indX = 0xA1,
    LDA_indY = 0xB1,
    CMP_indX = 0xC1,
    CMP_indY = 0xD1,
    SBC_indX = 0xE1,
    SBC_indY = 0xF1,
    LD_immX = 0xA2,
    BIT_zpg = 0x24,
    ST_zpgY = 0x84,
    ST_zpgYX = 0x94,
    LD_zpgY = 0xA4,
    LD_zpgYX = 0xB4,
    CP_zpgY = 0xC4,
    CP_zpgX = 0xE4,
    ORA_zpg = 0x05,
    ORA_zpgX = 0x15,
    AND_zpg = 0x25,
    AND_zpgX = 0x35,
    EOR_zpg = 0x45,
    EOR_zpgX = 0x55,
    ADC_zpg = 0x65,
    ADC_zpgX = 0x75,
    STA_zpg = 0x85,
    STA_zpgX = 0x95,
    LDA_zpg = 0xA5,
    LDA_zpgX = 0xB5,
    CMP_zpg = 0xC5,
    CMP_zpgX = 0xD5,
    SBC_zpg = 0xE5,
    SBC_zpgX = 0xF5,
    ASL_zpg = 0x06,
    ASL_zpgX = 0x16,
    ROL_zpg = 0x26,
    ROL_zpgX = 0x36,
    LSR_spg = 0x46,
    LSR_zpgX = 0x56,
    ROR_zpg = 0x66,
    ROR_zpgX = 0x76,
    STX_zpg = 0x86,
    STX_zpgY = 0x96,
    LDX_zpg = 0xA6,
    LDX_zpgY = 0xB6,
    DEC_zpg = 0xC6,
    DEC_zpgX = 0xD6,
    INC_zpg = 0xE6,
    INC_zpgX = 0xF6,
    PHP_impl = 0x08,
    CLC_impl = 0x18,
    PLP_impl = 0x28,
    SEC_impl = 0x38,
    PHA_impl = 0x48,
    CLI_impl = 0x58,
    PLA_impl = 0x68,
    SEI_impl = 0x78,
    DEY_impl = 0x88,
    TYA_impl = 0x98,
    TAY_impl = 0xA8,
    CLV_impl = 0xB8,
    INY_impl = 0xC8,
    CLD_impl = 0xD8,
    INX_impl = 0xE8,
    SED_impl = 0xF8,
    ORA_imm = 0x09,
    ORA_absY = 0x19,
    AND_imm = 0x29,
    AND_absY = 0x39,
    EOR_imm = 0x49,
    EOR_absY = 0x59,
    ADC_imm = 0x69,
    ADC_absY = 0x79,
    STA_absY = 0x99,
    LDA_imm = 0xA9,
    LDA_absY = 0xB9,
    CMP_imm = 0xC9,
    CMP_absY = 0xD9,
    SBC_imm = 0xE9,
    SBC_absY = 0xF9,
    ASL_A = 0x0A,
    ROL_A = 0x2A,
    LSR_A = 0x4A,
    ROR_A = 0x6A,
    TXA_impl = 0x8A,
    TXS_impl = 0x9A,
    TAX_impl = 0xAA,
    TSX_impl = 0xBA,
    DEX_impl = 0xCA,
    NOP_impl = 0xEA,
    BIT_abs = 0x2C,
    JMP_abs = 0x4C,
    JMP_ind = 0x6C,
    STY_abs = 0x8C,
    LDY_abs = 0xAC,
    LDY_absX = 0xBC,
    CPY_abs = 0xCC,
    CP_absY = 0xEC,
    ORA_abs = 0x0D,
    ORA_absX = 0x1D,
    AND_abs = 0x2D,
    AND_absX = 0x3D,
    EOR_abs = 0x4D,
    EOR_absX = 0x5D,
    ADC_abs = 0x6D,
    ADC_absX = 0x7D,
    STA_abs = 0x8D,
    STA_absX = 0x9D,
    LDA_abs = 0xAD,
    LDA_absX = 0xBD,
    CMP_abs = 0xCD,
    CMP_absX = 0xDD,
    SBC_abs = 0xED,
    SBC_absX = 0xFD,
    ASL_abs = 0x0E,
    ASL_absX = 0x1E,
    ROL_abs = 0x2E,
    ROL_absX = 0x3E,
    LSR_abs = 0x4E,
    LSR_absX = 0x5E,
    ROR_abs = 0x6E,
    ROR_absX = 0x7E,
    STX_abs = 0x8E,
    LDX_abs = 0xAE,
    LDX_absY = 0xBE,
    DEC_abs = 0xCE,
    DEC_absX = 0xDE,
    INC_abs = 0xEE,
    INC_absX = 0xFE
};

class Bus;

// CPU is owned by Bus
struct Cpu {

    uint8_t accumulator;
    uint8_t x;
    uint8_t y;

    uint8_t stack_pointer;
    uint16_t PC;
    uint8_t status;

    // Pointer to Bus it's a part of
    Bus *bus;


};


#endif

/*
switch (opcode) {
    case Opcode::BRK: break;
    case Opcode::BPL: break;
    case Opcode::JSR: break;
    case Opcode::BMI: break;
    case Opcode::RTI: break;
    case Opcode::BVC: break;
    case Opcode::RTS: break;
    case Opcode::BVS: break;
    case Opcode::BCC: break;
    case Opcode::LD_immY: break;
    case Opcode::BCS: break;
    case Opcode::CP_immY: break;
    case Opcode::BNE: break;
    case Opcode::CP_immX: break;
    case Opcode::BEQ: break;
    case Opcode::ORA_indX: break;
    case Opcode::ORA_indY: break;
    case Opcode::AND_indX: break;
    case Opcode::AND_indY: break;
    case Opcode::EOR_indX: break;
    case Opcode::EOR_indY: break;
    case Opcode::ADC_indX: break;
    case Opcode::ADC_indY: break;
    case Opcode::STA_indX: break;
    case Opcode::STA_indY: break;
    case Opcode::LDA_indX: break;
    case Opcode::LDA_indY: break;
    case Opcode::CMP_indX: break;
    case Opcode::CMP_indY: break;
    case Opcode::SBC_indX: break;
    case Opcode::SBC_indY: break;
    case Opcode::LD_immX: break;
    case Opcode::BIT_zpg: break;
    case Opcode::ST_zpgY: break;
    case Opcode::ST_zpgYX: break;
    case Opcode::LD_zpgY: break;
    case Opcode::LD_zpgYX: break;
    case Opcode::CP_zpgY: break;
    case Opcode::CP_zpgX: break;
    case Opcode::ORA_zpg: break;
    case Opcode::ORA_zpgX: break;
    case Opcode::AND_zpg: break;
    case Opcode::AND_zpgX: break;
    case Opcode::EOR_zpg: break;
    case Opcode::EOR_zpgX: break;
    case Opcode::ADC_zpg: break;
    case Opcode::ADC_zpgX: break;
    case Opcode::STA_zpg: break;
    case Opcode::STA_zpgX: break;
    case Opcode::LDA_zpg: break;
    case Opcode::LDA_zpgX: break;
    case Opcode::CMP_zpg: break;
    case Opcode::CMP_zpgX: break;
    case Opcode::SBC_zpg: break;
    case Opcode::SBC_zpgX: break;
    case Opcode::ASL_zpg: break;
    case Opcode::ASL_zpgX: break;
    case Opcode::ROL_zpg: break;
    case Opcode::ROL_zpgX: break;
    case Opcode::LSR_spg: break;
    case Opcode::LSR_zpgX: break;
    case Opcode::ROR_zpg: break;
    case Opcode::ROR_zpgX: break;
    case Opcode::STX_zpg: break;
    case Opcode::STX_zpgY: break;
    case Opcode::LDX_zpg: break;
    case Opcode::LDX_zpgY: break;
    case Opcode::DEC_zpg: break;
    case Opcode::DEC_zpgX: break;
    case Opcode::INC_zpg: break;
    case Opcode::INC_zpgX: break;
    case Opcode::PHP_impl: break;
    case Opcode::CLC_impl: break;
    case Opcode::PLP_impl: break;
    case Opcode::SEC_impl: break;
    case Opcode::PHA_impl: break;
    case Opcode::CLI_impl: break;
    case Opcode::PLA_impl: break;
    case Opcode::SEI_impl: break;
    case Opcode::DEY_impl: break;
    case Opcode::TYA_impl: break;
    case Opcode::TAY_impl: break;
    case Opcode::CLV_impl: break;
    case Opcode::INY_impl: break;
    case Opcode::CLD_impl: break;
    case Opcode::INX_impl: break;
    case Opcode::SED_impl: break;
    case Opcode::ORA_imm: break;
    case Opcode::ORA_absY: break;
    case Opcode::AND_imm: break;
    case Opcode::AND_absY: break;
    case Opcode::EOR_imm: break;
    case Opcode::EOR_absY: break;
    case Opcode::ADC_imm: break;
    case Opcode::ADC_absY: break;
    case Opcode::STA_absY: break;
    case Opcode::LDA_imm: break;
    case Opcode::LDA_absY: break;
    case Opcode::CMP_imm: break;
    case Opcode::CMP_absY: break;
    case Opcode::SBC_imm: break;
    case Opcode::SBC_absY: break;
    case Opcode::ASL_A: break;
    case Opcode::ROL_A: break;
    case Opcode::LSR_A: break;
    case Opcode::ROR_A: break;
    case Opcode::TXA_impl: break;
    case Opcode::TXS_impl: break;
    case Opcode::TAX_impl: break;
    case Opcode::TSX_impl: break;
    case Opcode::DEX_impl: break;
    case Opcode::NOP_impl: break;
    case Opcode::BIT_abs: break;
    case Opcode::JMP_abs: break;
    case Opcode::JMP_ind: break;
    case Opcode::STY_abs: break;
    case Opcode::LDY_abs: break;
    case Opcode::LDY_absX: break;
    case Opcode::CPY_abs: break;
    case Opcode::CP_absY: break;
    case Opcode::ORA_abs: break;
    case Opcode::ORA_absX: break;
    case Opcode::AND_abs: break;
    case Opcode::AND_absX: break;
    case Opcode::EOR_abs: break;
    case Opcode::EOR_absX: break;
    case Opcode::ADC_abs: break;
    case Opcode::ADC_absX: break;
    case Opcode::STA_abs: break;
    case Opcode::STA_absX: break;
    case Opcode::LDA_abs: break;
    case Opcode::LDA_absX: break;
    case Opcode::CMP_abs: break;
    case Opcode::CMP_absX: break;
    case Opcode::SBC_abs: break;
    case Opcode::SBC_absX: break;
    case Opcode::ASL_abs: break;
    case Opcode::ASL_absX: break;
    case Opcode::ROL_abs: break;
    case Opcode::ROL_absX: break;
    case Opcode::LSR_abs: break;
    case Opcode::LSR_absX: break;
    case Opcode::ROR_abs: break;
    case Opcode::ROR_absX: break;
    case Opcode::STX_abs: break;
    case Opcode::LDX_abs: break;
    case Opcode::LDX_absY: break;
    case Opcode::DEC_abs: break;
    case Opcode::DEC_absX: break;
    case Opcode::INC_abs: break;
    case Opcode::INC_absX: break;
}
*/


/*

first Byte (8bits) of input in our CPU provides us with the: Instruction, Size, Duration

To emulate an instruction, we need to emulate its:
- function
- address mode
- cycles

^^^^^^^^^^^^^
All that is still in the first byte




*/