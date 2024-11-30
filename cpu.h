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
    CP_absY = 0xCC,
    CP_absX = 0xEC,
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

enum FLAGS {
    C = 1 << 0, // Carry Bit
    Z = 1 << 1, // Zero
    I = 1 << 2, // Disable Interrupts
    D = 1 << 3, // Decimal mode
    B = 1 << 4, // Break
    U = 1 << 5, // Unused
    V = 1 << 6, // Overflow
    N = 1 << 7, // Negative
};

class Bus;  // forward declaration for Bus

// CPU is owned by Bus
struct Cpu {

    uint8_t accumulator{0x00};
    uint8_t x{0x00}; // y register
    uint8_t y{0x00}; // x register

    // check that it starts right
    uint8_t stack_pointer{0xFD}; // top of stack. Stack spans addresses: 0x0100 to 0x01FF
    uint16_t PC{0x0000}; // program counter
    uint8_t status{0x00}; // flags state
    uint8_t fetched{0x00};
    uint8_t cycles{0x00};
    uint8_t opcode{0x00};
    uint16_t adr{0x0000};
    int8_t adr_relative{0x00}; // changed to 8 bits
    int8_t relative_test{0x00};

    // Pointer to Bus it's a part of
    Bus *bus{nullptr};
    Cpu(Bus *bus);
    ~Cpu();
    void write(uint16_t adr, uint8_t val);
    uint8_t read(uint16_t adr) const;

    // get the status of the wanted flag
    uint8_t get_flag(FLAGS flag) const;

    // toggle flag on or off
    void set_flag(FLAGS flag, bool on);
    void clock();

    // sets everything back to default parameters
    void reset();
    // if I flag is 0, irq is triggered at the end of instruction
    void irq();
    void nmi();

    uint8_t fetch();

    // function to verify which instruction to execute
    uint8_t execute_opcode(Opcode opcode);

    // stack functions
    // add element to top of stack
    void push(uint8_t val);
    // remove and return element from top of stack
    uint8_t pull();

    // functions for addressing
    uint8_t zpg();
    uint8_t zpgX();
    uint8_t zpgY();
    uint8_t relative();
    uint8_t absolute();
    uint8_t absoluteX();
    uint8_t absoluteY();
    uint8_t indirect();
    uint8_t ind_X();
    uint8_t ind_Y();

    // general instructions
    uint8_t AND();
    uint8_t BCS();
    uint8_t LDY();
    uint8_t CPX();
    uint8_t CPY();
    uint8_t ORA();
    uint8_t ADC();
    uint8_t SBC();
};

uint16_t convertTo_16_bit(uint8_t high, uint8_t low);

#endif
