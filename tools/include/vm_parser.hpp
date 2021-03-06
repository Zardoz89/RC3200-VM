/*!
 * \brief       Parameters parser for vm tool (main.cpp)
 * \file        vm_parser.hpp
 * \copyright   LGPL v3
 *
 * Parameters parser for vm tool (main.cpp)
 */
#ifndef __VM_PARSER_HPP_
#define __VM_PARSER_HPP_ 1

#include "types.hpp"

#include <cstring>
#include <cstdlib>
#include <cstdio>

#include <vector>

enum class CpuToUse {
    TR3200,
    DCPU16N
};

enum VmExtentions : unsigned {
    EXT_FULLKEYB = 1
};

struct VmParamaters {

    VmParamaters (const int argc, const char** argv) :
        def_dsk_file("disk.dsk"), def_nvram_file("nvram.data"), ram_size(128*1024), clock(100000), valid_params(true), ask_help(false)   {
        // Default values
        dsk_file = def_dsk_file;
        nvram_file = def_nvram_file;
        rom_file = nullptr;
        exec_vm = false;
        timing_debug = false;
        extentions = 0;

        cpu = CpuToUse::TR3200;

        // Begin to parse
        for (int i = 1; i < argc; i++) {
            const char* arg = argv[i];

            if (arg[0] == '-') { // Parameter
                if (arg[1] == '\0') {
                    valid_params = false;
                    std::fprintf(stderr, "Invalid parameter %s\n", argv[i]);
                    break; // Invalid parameter
                }
                arg++;

                if (strncmp(arg, "r", 1) == 0 || strncmp(arg, "-rom", 4) == 0) {
                    // Rom file parameter
                    i++;
                    arg = argv[i];
                    if (i >= argc || arg[0] == '-') {
                        valid_params = false;
                        std::fprintf(stderr, "Missing or invalid value for parameter %s\n", argv[i-1]);
                        break;
                    }

                    rom_file = arg;

                } else if(strncmp(arg, "-time", 5) == 0) {
                    timing_debug = true;

                } else if(strncmp(arg, "x", 1) == 0 || strncmp(arg, "-exec", 5) == 0) {
                    // Run VM parameter
                    exec_vm = true;

                } else if (strncmp(arg, "d", 1) == 0 || strncmp(arg, "-disk", 5) == 0) {
                    // Disk file parameter
                    i++;
                    arg = argv[i];
                    if (i >= argc || arg[0] == '-') {
                        valid_params = false;
                        std::fprintf(stderr, "Missing or invalid value for parameter %s\n", argv[i-1]);
                        break;
                    }

                    dsk_file = arg;

                } else if (strncmp(arg, "-nvram", 5) == 0) {
                    // NVRAM file parameter
                    i++;
                    arg = argv[i];
                    if (i >= argc || arg[0] == '-') {
                        valid_params = false;
                        std::fprintf(stderr, "Missing or invalid value for parameter %s\n", argv[i-1]);
                        break;
                    }

                    nvram_file = arg;

                } else if (strncmp(arg, "c", 1) == 0 || strncmp(arg, "-cpu", 4) == 0) {
                    // CPU type
                    i++;
                    arg = argv[i];
                    if (i >= argc || arg[0] == '-') {
                        valid_params = false;
                        std::fprintf(stderr, "Missing or invalid value for parameter %s\n", argv[i-1]);
                        break;
                    }

                    if (strncmp(arg, "tr3200", 6) == 0 || strncmp(arg, "TR3200", 6) == 0) {
                        cpu = CpuToUse::TR3200;
                    } else if (strncmp(arg, "DCPU-16N", 8) == 0 || strncmp(arg, "dcpu-16n", 8) == 0) {
                        cpu = CpuToUse::DCPU16N;
                    }

                } else if(strncmp(arg, "m", 1) == 0 || strncmp(arg, "-mem", 4) == 0) {
                    // Total RAM
                    i++;
                    arg = argv[i];
                    if (i >= argc || arg[0] == '-') {
                        valid_params = false;
                        std::fprintf(stderr, "Missing or invalid value for parameter %s\n", argv[i-1]);
                        break;
                    }
                    auto ram = std::strtol(arg, nullptr, 0);
                    if (ram <= 0 || ram > 1024) {
                        std::fprintf(stderr, "Invalid value for parameter %s\nUsing 128KiB\n", argv[i-1]);
                        ram = 128*1024;
                    } else {
                        ram ^= (ram & (32 - 1)); // bitwise round to 1<<5 (32) size blocks
                        if (ram == 0) {
                            ram = 32;
                        }
                    }

                    ram_size = ram * 1024;
                } else if (strncmp(arg, "-clock", 6) == 0 ) {
                    // Clock speed
                    i++;
                    arg = argv[i];
                    if (i >= argc || arg[0] == '-') {
                        valid_params = false;
                        std::fprintf(stderr, "Missing or invalid value for parameter %s\n", argv[i-1]);
                        break;
                    }
                    auto clk = std::atoi(arg);
                    if (clk != 100 && clk != 250 && clk != 500 && clk != 1000) {
                        std::fprintf(stderr, "Invalid value for parameter %s\nUsing 100KHz\n", argv[i-1]);
                        clk = 1100;
                    }

                    clock = clk * 1000;

                } else if (strncmp(arg, "b", 1) == 0 ) {
                    // A breakpoint
                    i++;
                    arg = argv[i];
                    if (i >= argc || arg[0] == '-') {
                        valid_params = false;
                        std::fprintf(stderr, "Missing or invalid value for parameter %s\n", argv[i-1]);
                        break;
                    }
                    trillek::DWord addr = std::strtol(arg, nullptr, 0);
                    breaks.push_back(addr);

                } else if (strncmp(arg, "h", 1) == 0 || strncmp(arg, "-help", 5) == 0) {
                    // Asked for help
                    std::printf("%s%s%s", // 3 parts to the help messages
                    "Virtual Computer toy Emulator\n\n"
                    "Usage:\n\t", argv[0], " -r romfile [other parameters]\n\n"
                    "Parameters:\n"
                    "\t-r file or --rom file : RAW binary file for the ROM (32 KiB Max)\n"
                    "\t-d file or --disk file : Disk file\n"
                    "\t--nvram file : Non volatile RAM file\n"
                    "\t-c val or --cpu val : Sets the CPU to use, from \"tr3200\" or \"dcpu-16n\"\n"
                    "\t-x or --exec : Run the computer in normal mode without asking\n"
                    "\t-m val or --mem val : How much RAM the computer will have, in KiB."
                    " Must be between 32 and 1024 and will be rounded to a multiple of 32\n"
                    "\t--time : Show timing and speed info while running.\n"
                    "\t--clock val : CPU clock speed in Khz. Must be 100, 250, 500 or 1000.\n"
                    "\t-b val : Inserts a breakpoint at address val (could be hexadecimal or decimal).\n"
                    "\t--ext-keys : Allow extra (non-standard) keycodes with virtual keyboard.\n"
                    "\t-h or --help : Shows this help\n");

                    ask_help = true;

                } else if(strncmp(arg, "-ext-keys", 9) == 0) {
                    extentions |= EXT_FULLKEYB;
                }
            }
        }

        if (rom_file == nullptr) {
            valid_params = false;
            std::fprintf(stderr, "Missing ROM file\n");
        }
    }

    const char* def_dsk_file;
    const char* def_nvram_file;

    const char* rom_file;           /// Path to ROM file
    const char* dsk_file;           /// Path to Floppy disk file
    const char* nvram_file;         /// Path to NVRAM file
    unsigned ram_size;              /// Ram size in Bytes
    CpuToUse cpu;                   /// CPU to use (default TR3200)
    unsigned clock;                 /// CPU clock speed (default 100Khz)

    std::vector<trillek::DWord> breaks; /// List of breakpoints

    bool valid_params;              /// Parsed correctly all parameters
    bool ask_help;                  /// User asked by help
    bool exec_vm;                   /// Run computer without asking to use debug mode
    bool timing_debug;              /// Print timing info while running
    unsigned extentions;            /// bit mask of extentions
};

#endif // __VM_PARSER_HPP_

