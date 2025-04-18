#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "Mapper_001.h" // Include your actual header file
#include "doctest.h"
#include <cstdint>
#include <utility>

// Using the actual macros as provided
#define PRG_SWITCH1 std::pair<uint16_t, uint16_t>{0x8000, 0xBFFF}
#define PRG_SWITCH2 std::pair<uint16_t, uint16_t>{0xC000, 0xFFFF}
#define CHR_SWITCH1 std::pair<uint16_t, uint16_t>{0x0000, 0x0FFF}
#define CHR_SWITCH2 std::pair<uint16_t, uint16_t>{0x1000, 0x1FFF}
#define PRG_BANK_SIZE 0x4000
#define CHR_BANK_SIZE 0x1000

class TestMapper_001 : public Mapper_001 {
public:
  TestMapper_001()
      : Mapper_001(4, 4) {} // Initialize with 4 PRG banks and 2 CHR banks

  // Expose private members for testing
  using Mapper_001::chr_bank_0;
  using Mapper_001::chr_bank_1;
  using Mapper_001::chr_bank_mode;
  using Mapper_001::control;
  using Mapper_001::double_block_mode;
  using Mapper_001::find_prg_mapped_addr;
  using Mapper_001::nCHRBanks;
  using Mapper_001::nPRGBanks;
  using Mapper_001::prg;
  using Mapper_001::prg_bank_mode;
  using Mapper_001::prg_bank_selected;
  using Mapper_001::write_to_control_register;
  using Mapper_001::write_to_register;
  virtual ~TestMapper_001() {}

  // Helper function to manually calculate expected mapped address
  uint32_t calculate_expected_mapped_addr(uint16_t addr, uint8_t prg_bank,
                                          uint8_t mode, bool double_block) {
    if (double_block) {
      return (addr & PRG_SWITCH1.first) + PRG_BANK_SIZE * prg_bank;
    } else {
      std::pair<uint16_t, uint16_t> range =
          mode == 2 ? PRG_SWITCH2 : PRG_SWITCH1;
      uint32_t mapped_addr = 0;
      auto [low, high] = range;
      if (low <= addr && addr <= high) {
        mapped_addr = (low & addr) + PRG_BANK_SIZE * prg_bank;
      } else if (mode == 2) {
        mapped_addr = low & addr;
      } else if (mode == 3) {
        mapped_addr = (low & addr) + nPRGBanks * PRG_BANK_SIZE;
      }
      return mapped_addr;
    }
  }

  // Reset the mapper to known state
  void reset() {
    control.reg = 0;
    chr_bank_0.reg = 0;
    chr_bank_1.reg = 0;
    prg.reg = 0;
    double_block_mode = false;
    prg_bank_mode = 0;
    prg_bank_selected = 0;
    chr_bank_mode = 0;
  }
};

TEST_CASE("Mapper_001::cpu_write_mapper - Control Register Write") {
  TestMapper_001 mapper;
  mapper.reset();

  SUBCASE("Write to Control Register (0x8000-0x9FFF)") {
    uint32_t mapped_adr = 0;
    uint16_t test_addr = 0x8000; // Address in control register range
    bool result{false};
    result = mapper.cpu_write_mapper(test_addr, mapped_adr, 0x01);
    result = mapper.cpu_write_mapper(test_addr, mapped_adr, 0x00);
    result = mapper.cpu_write_mapper(test_addr, mapped_adr, 0x00);
    result = mapper.cpu_write_mapper(test_addr, mapped_adr, 0x01);
    result = mapper.cpu_write_mapper(test_addr, mapped_adr, 0x01);
    result = mapper.cpu_write_mapper(test_addr, mapped_adr, 0x00);
    CHECK(mapper.prg_bank_mode == 2);
    CHECK((mapper.control.reg & 0x1F) == 0x18);

    CHECK(mapper.double_block_mode == false);
    // Choose 2rd bank at 0xC000
    result = mapper.cpu_write_mapper(0xE000, mapped_adr, 0x00);
    result = mapper.cpu_write_mapper(0xE000, mapped_adr, 0x01);
    result = mapper.cpu_write_mapper(0xE000, mapped_adr, 0x00);
    result = mapper.cpu_write_mapper(0xE000, mapped_adr, 0x00);
    result = mapper.cpu_write_mapper(0xE000, mapped_adr, 0x00);
    // Supposed to wrap around

    CHECK((mapper.prg.reg & 0x1F) == 0x02);
    CHECK(mapper.prg_bank_selected == 2);
    CHECK(mapper.prg_bank_mode == 2);
    uint32_t check_adr{0x0000};
    result = mapper.cpu_read_mapper(0x8005, check_adr);
    CHECK(check_adr == 5);
    result = mapper.cpu_read_mapper(0xC005, check_adr);
    CHECK(check_adr == 0x8005);

    CHECK(result == true); // Should return true for mapper range

    result = mapper.cpu_write_mapper(test_addr, mapped_adr, 0x00);
    result = mapper.cpu_write_mapper(test_addr, mapped_adr, 0x01);
    result = mapper.cpu_write_mapper(test_addr, mapped_adr, 0x01);
    result = mapper.cpu_write_mapper(test_addr, mapped_adr, 0x01);
    CHECK(mapper.prg_bank_mode == 3);
    
    result = mapper.cpu_read_mapper(0xBF03, check_adr);
    CHECK(check_adr == 0xBF03);
    result = mapper.cpu_read_mapper(0xC005, check_adr);
    CHECK(mapper.nPRGBanks == 4);
    CHECK(check_adr == 0xC005);

    result = mapper.cpu_write_mapper(test_addr, mapped_adr, 0x00);
    result = mapper.cpu_write_mapper(test_addr, mapped_adr, 0x00);
    result = mapper.cpu_write_mapper(test_addr, mapped_adr, 0x01);
    result = mapper.cpu_write_mapper(test_addr, mapped_adr, 0x00);
    result = mapper.cpu_write_mapper(test_addr, mapped_adr, 0x00);

    result = mapper.cpu_write_mapper(0xE000, mapped_adr, 0x01);
    result = mapper.cpu_write_mapper(0xE000, mapped_adr, 0x00);
    result = mapper.cpu_write_mapper(0xE000, mapped_adr, 0x00);
    result = mapper.cpu_write_mapper(0xE000, mapped_adr, 0x00);
    result = mapper.cpu_write_mapper(0xE000, mapped_adr, 0x00);
    result = mapper.cpu_read_mapper(0x8005, check_adr);
    CHECK(mapper.double_block_mode == true);
    CHECK(check_adr == 5);


  mapper.reset();
  // Test CHR switch 1 8KB at a time
  result = mapper.cpu_write_mapper(0xA000, mapped_adr, 0x00);
  result = mapper.cpu_write_mapper(0xA000, mapped_adr, 0x01);
  result = mapper.cpu_write_mapper(0xA000, mapped_adr, 0x00);
  result = mapper.cpu_write_mapper(0xA000, mapped_adr, 0x00);
  result = mapper.cpu_write_mapper(0xA000, mapped_adr, 0x00);
  // Testing the wrap around
  result = mapper.cpu_write_mapper(0xA000, mapped_adr, 0x01);

  result = mapper.ppu_read_mapper(0x0001, mapped_adr);
  CHECK(mapped_adr == 8193);
  result = mapper.ppu_read_mapper(0x1001, mapped_adr);
  CHECK(mapped_adr == 12289);

  result = mapper.cpu_write_mapper(test_addr, mapped_adr, 0x00);
  result = mapper.cpu_write_mapper(test_addr, mapped_adr, 0x00);
  result = mapper.cpu_write_mapper(test_addr, mapped_adr, 0x00);
  result = mapper.cpu_write_mapper(test_addr, mapped_adr, 0x00);
  result = mapper.cpu_write_mapper(test_addr, mapped_adr, 0x01);
  result = mapper.ppu_read_mapper(0x0001, mapped_adr);
  CHECK(mapped_adr == 12289);
  result = mapper.ppu_read_mapper(0x1001, mapped_adr);
  CHECK(mapped_adr == 0x001);
  }
}

TEST_CASE("Mapper_001::cpu_write_mapper - CHR Bank 0 Register Write") {
  TestMapper_001 mapper;
  mapper.reset();

  uint32_t mapped_adr = 0;
  uint16_t test_addr = 0xA000;    // Address in CHR Bank 0 range
  uint8_t test_data = 0b00011111; // Set all 5 bits

  SUBCASE("Write to CHR Bank 0 Register (0xA000-0xBFFF)") {
    bool result = mapper.cpu_write_mapper(test_addr, mapped_adr, test_data);

    CHECK(result == true); // Should return true for mapper range

    // Calculate expected mapped address
    uint32_t expected = mapper.calculate_expected_mapped_addr(
        test_addr, mapper.prg_bank_selected, mapper.prg_bank_mode,
        mapper.double_block_mode);
    CHECK(mapped_adr == expected);
  }
}

TEST_CASE("Mapper_001::cpu_write_mapper - CHR Bank 1 Register Write") {
  TestMapper_001 mapper;
  mapper.reset();

  uint32_t mapped_adr = 0;
  uint16_t test_addr = 0xC000;    // Address in CHR Bank 1 range
  uint8_t test_data = 0b00010101; // Set some bits

  SUBCASE("Write to CHR Bank 1 Register (0xC000-0xDFFF)") {
    bool result = mapper.cpu_write_mapper(test_addr, mapped_adr, test_data);

    CHECK(result == true); // Should return true for mapper range

    // Calculate expected mapped address
    uint32_t expected = mapper.calculate_expected_mapped_addr(
        test_addr, mapper.prg_bank_selected, mapper.prg_bank_mode,
        mapper.double_block_mode);
    CHECK(mapped_adr == expected);
  }
}

TEST_CASE("Mapper_001::cpu_write_mapper - PRG Bank Register Write") {
  TestMapper_001 mapper;
  mapper.reset();

  uint32_t mapped_adr = 0;
  uint16_t test_addr = 0xE000;    // Address in PRG Bank range
  uint8_t test_data = 0b00001111; // Set lower 4 bits

  SUBCASE("Write to PRG Bank Register (0xE000-0xFFFF)") {
    bool result{false};
    for (int i = 0; i < 4; i++) {
      result = mapper.cpu_write_mapper(test_addr, mapped_adr, test_data);
    }

    CHECK(result == true); // Should return true for mapper range
    // PRG bank selection should be updated
    CHECK(mapper.prg_bank_selected == 0x0F); // When double_block_mode is false

    // Calculate expected mapped address
    uint32_t expected = mapper.calculate_expected_mapped_addr(
        test_addr, mapper.prg_bank_selected, mapper.prg_bank_mode,
        mapper.double_block_mode);
    CHECK(mapped_adr == expected);
  }

  SUBCASE("Write to PRG Bank Register with double_block_mode = true") {
    mapper.double_block_mode = true;
    mapper.prg_bank_selected = 0; // Reset to ensure we see the change
    bool result{false};

    for (int i = 0; i < 4; i++) {
      result = mapper.cpu_write_mapper(test_addr, mapped_adr, test_data);
    }

    CHECK(result == true);
    CHECK(mapper.prg_bank_selected ==
          0x0E); // When double_block_mode is true, mask with 0x0E

    // In double block mode, the mapped address is calculated directly
    uint32_t expected = (test_addr & PRG_SWITCH1.first) +
                        PRG_BANK_SIZE * mapper.prg_bank_selected;
    CHECK(mapped_adr == expected);
  }
}

TEST_CASE("Mapper_001::cpu_write_mapper - Different PRG Bank Modes") {
  TestMapper_001 mapper;
  mapper.reset();

  uint32_t mapped_adr = 0;
  uint16_t test_addr = 0x9000;  // Address in control register range
  mapper.prg_bank_selected = 3; // Set to a known value

  SUBCASE("PRG Bank Mode 2") {
    // Set PRG bank mode to 2
    mapper.prg_bank_mode = 2;

    // Write to any address to trigger address mapping
    bool result = mapper.cpu_write_mapper(test_addr, mapped_adr, 0);

    CHECK(result == true);

    // Calculate expected address using PRG_SWITCH2
    std::pair<uint16_t, uint16_t> range = PRG_SWITCH2;
    uint32_t expected = 0;
    auto [low, high] = range;
    if (low <= test_addr && test_addr <= high) {
      expected = (low & test_addr) + PRG_BANK_SIZE * mapper.prg_bank_selected;
    } else {
      expected = low & test_addr;
    }
    CHECK(mapped_adr == expected);
  }

  SUBCASE("PRG Bank Mode 3") {
    mapper.prg_bank_mode = 3;

    bool result = mapper.cpu_write_mapper(test_addr, mapped_adr, 0);

    CHECK(result == true);

    // Calculate expected address for mode 3
    std::pair<uint16_t, uint16_t> range = PRG_SWITCH1;
    uint32_t expected = 0;
    auto [low, high] = range;
    if (low <= test_addr && test_addr <= high) {
      expected = (low & test_addr) + PRG_BANK_SIZE * mapper.prg_bank_selected;
    } else {
      expected = (low & test_addr) + mapper.nPRGBanks * PRG_BANK_SIZE;
    }
    CHECK(mapped_adr == expected);
  }
}

TEST_CASE("Mapper_001::cpu_write_mapper - Outside Mapper Range") {
  TestMapper_001 mapper;
  mapper.reset();

  uint32_t mapped_adr = 0;
  uint16_t test_addr = 0x7FFF; // Address outside mapper range
  uint8_t test_data = 0x42;

  SUBCASE("Write to Address Outside Mapper Range") {
    bool result = mapper.cpu_write_mapper(test_addr, mapped_adr, test_data);

    CHECK(result == false); // Should return false for non-mapper range

    // Calculate expected mapped address
    uint32_t expected = mapper.calculate_expected_mapped_addr(
        test_addr, mapper.prg_bank_selected, mapper.prg_bank_mode,
        mapper.double_block_mode);
    CHECK(mapped_adr == expected);
  }
}

TEST_CASE(
    "Mapper_001::cpu_write_mapper - PRG Bank Mapping with Double Block Mode") {
  TestMapper_001 mapper;
  mapper.reset();

  uint32_t mapped_adr = 0;
  uint16_t test_addr = 0xA000; // Any valid address will work

  SUBCASE("Standard PRG Bank Mapping") {
    mapper.double_block_mode = false;
    mapper.prg_bank_mode = 1;
    mapper.prg_bank_selected = 2; // Set to known value

    bool result = mapper.cpu_write_mapper(test_addr, mapped_adr, 0x0F);

    CHECK(result == true);

    // Calculate expected mapped address with PRG_SWITCH1
    std::pair<uint16_t, uint16_t> range = PRG_SWITCH1;
    uint32_t expected = 0;
    auto [low, high] = range;
    if (low <= test_addr && test_addr <= high) {
      expected = (low & test_addr) + PRG_BANK_SIZE * mapper.prg_bank_selected;
    }
    CHECK(mapped_adr == expected);
  }

  SUBCASE("Double Block Mode PRG Bank Mapping") {
    mapper.double_block_mode = true;
    mapper.prg_bank_selected = 5; // Set to a known value

    bool result = mapper.cpu_write_mapper(test_addr, mapped_adr, 0);

    CHECK(result == true);
    // In double block mode, mapped_adr should be calculated directly
    uint32_t expected = (test_addr & PRG_SWITCH1.first) + PRG_BANK_SIZE * 5;
    CHECK(mapped_adr == expected);
  }
}

// Test covering all address boundaries
TEST_CASE("Mapper_001::cpu_write_mapper - Address Boundary Tests") {
  TestMapper_001 mapper;
  mapper.reset();
  mapper.prg_bank_selected = 1; // Set to known value

  uint32_t mapped_adr = 0;

  // Boundary tests for each register range
  SUBCASE("Control Register Boundaries") {
    // Lower boundary
    bool result = mapper.cpu_write_mapper(0x8000, mapped_adr, 0x55);
    CHECK(result == true);
    uint32_t expected = mapper.calculate_expected_mapped_addr(
        0x8000, mapper.prg_bank_selected, mapper.prg_bank_mode,
        mapper.double_block_mode);
    CHECK(mapped_adr == expected);

    // Upper boundary
    result = mapper.cpu_write_mapper(0x9FFF, mapped_adr, 0x55);
    CHECK(result == true);
    expected = mapper.calculate_expected_mapped_addr(
        0x9FFF, mapper.prg_bank_selected, mapper.prg_bank_mode,
        mapper.double_block_mode);
    CHECK(mapped_adr == expected);

    // Just outside lower boundary
    result = mapper.cpu_write_mapper(0x7FFF, mapped_adr, 0x55);
    CHECK(result == false);
    expected = mapper.calculate_expected_mapped_addr(
        0x7FFF, mapper.prg_bank_selected, mapper.prg_bank_mode,
        mapper.double_block_mode);
    CHECK(mapped_adr == expected);
  }

  SUBCASE("CHR Bank 0 Register Boundaries") {
    // Lower boundary
    bool result = mapper.cpu_write_mapper(0xA000, mapped_adr, 0x55);
    CHECK(result == true);
    uint32_t expected = mapper.calculate_expected_mapped_addr(
        0xA000, mapper.prg_bank_selected, mapper.prg_bank_mode,
        mapper.double_block_mode);
    CHECK(mapped_adr == expected);

    // Upper boundary
    result = mapper.cpu_write_mapper(0xBFFF, mapped_adr, 0x55);
    CHECK(result == true);
    expected = mapper.calculate_expected_mapped_addr(
        0xBFFF, mapper.prg_bank_selected, mapper.prg_bank_mode,
        mapper.double_block_mode);
    CHECK(mapped_adr == expected);
  }
}

// Tests for the Register and Control Register writing functions
TEST_CASE("Mapper_001::write_to_register") {
  TestMapper_001 mapper;
  mapper.reset();

  SUBCASE("Reset register when bit 7 is set") {
    mapper.chr_bank_0.reg = 0x1F; // Set some bits
    mapper.chr_bank_0.unused = 3; // Set counter

    mapper.write_to_register(mapper.chr_bank_0, 0x80); // Set bit 7

    CHECK(mapper.chr_bank_0.reg == 0x0C); // Should reset to 0x0C
    CHECK(static_cast<int>(mapper.chr_bank_0.unused) ==
          0); // Counter should reset
  }

  SUBCASE("Shift in bits correctly") {
    // Initial state
    mapper.chr_bank_0.reg = 0;
    mapper.chr_bank_0.unused = 0;

    // Shift in 5 bits (10101)
    mapper.write_to_register(mapper.chr_bank_0, 0x01); // Bit 1
    mapper.write_to_register(mapper.chr_bank_0, 0x00); // Bit 2
    mapper.write_to_register(mapper.chr_bank_0, 0x01); // Bit 3
    mapper.write_to_register(mapper.chr_bank_0, 0x00); // Bit 4
    mapper.write_to_register(mapper.chr_bank_0, 0x01); // Bit 5

    // The register should now have bits set: 10101 in bit positions 1-5
    CHECK(static_cast<int>(mapper.chr_bank_0.bit1) == 1);
    CHECK(static_cast<int>(mapper.chr_bank_0.bit2) == 0);
    CHECK(static_cast<int>(mapper.chr_bank_0.bit3) == 1);
    CHECK(static_cast<int>(mapper.chr_bank_0.bit4) == 0);
    CHECK(static_cast<int>(mapper.chr_bank_0.bit5) == 1);
    CHECK(static_cast<int>(mapper.chr_bank_0.unused) ==
          0); // Counter should reset after 5th bit
  }
}

TEST_CASE("Mapper_001::write_to_control_register") {
  TestMapper_001 mapper;
  mapper.reset();

  SUBCASE("Reset control register when bit 7 is set") {
    mapper.control.reg = 0x1F; // Set some bits
    mapper.control.unused = 3; // Set counter

    mapper.write_to_control_register(0x80); // Set bit 7

    CHECK(mapper.control.reg == 0x0C);                   // Should reset to 0x0C
    CHECK(static_cast<int>(mapper.control.unused) == 0); // Counter should reset
  }

  SUBCASE("Shift in bits correctly") {
    // Initial state
    mapper.control.reg = 0;
    mapper.control.unused = 0;

    // Shift in 5 bits (10101)
    mapper.write_to_control_register(0x01); // nametable_low
    mapper.write_to_control_register(0x00); // nametable_high
    mapper.write_to_control_register(0x01); // prg_bank_low
    mapper.write_to_control_register(0x00); // prg_bank_high
    mapper.write_to_control_register(0x01); // chr_bank

    // The control register should now have corresponding bits set
    CHECK(static_cast<int>(mapper.control.nametable_low) == 1);
    CHECK(static_cast<int>(mapper.control.nametable_high) == 0);
    CHECK(static_cast<int>(mapper.control.prg_bank_low) == 1);
    CHECK(static_cast<int>(mapper.control.prg_bank_high) == 0);
    CHECK(static_cast<int>(mapper.control.chr_bank) == 1);
    CHECK(static_cast<int>(mapper.control.unused) ==
          0); // Counter should reset after 5th bit
    CHECK(static_cast<int>(mapper.chr_bank_mode) ==
          1); // chr_bank_mode should be updated
  }
}
