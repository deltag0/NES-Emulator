#ifndef LOGGING_H
#define LOGGING_H


#ifdef LOGGING
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>

class Cpu;
class Ppu;

uint8_t wrap_around(uint8_t val1, uint8_t val2);

void debug_log_cpu(std::ostream &out, Cpu &cpu, Ppu *ppu, bool debugging);
#endif

#endif
