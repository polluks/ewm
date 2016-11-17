// The MIT License (MIT)
//
// Copyright (c) 2015 Stefan Arentz - http://github.com/st3fan/ewm
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>
#include <string.h>

#include "cpu.h"
#include "mem.h"
#include "pia.h"

static uint8_t woz_monitor[256] = {
    0xD8, 0x58, 0xA0, 0x7F, 0x8C, 0x12, 0xD0, 0xA9,
    0xA7, 0x8D, 0x11, 0xD0, 0x8D, 0x13, 0xD0, 0xC9,
    0xDF, 0xF0, 0x13, 0xC9, 0x9B, 0xF0, 0x03, 0xC8,
    0x10, 0x0F, 0xA9, 0xDC, 0x20, 0xEF, 0xFF, 0xA9,
    0x8D, 0x20, 0xEF, 0xFF, 0xA0, 0x01, 0x88, 0x30,
    0xF6, 0xAD, 0x11, 0xD0, 0x10, 0xFB, 0xAD, 0x10,
    0xD0, 0x99, 0x00, 0x02, 0x20, 0xEF, 0xFF, 0xC9,
    0x8D, 0xD0, 0xD4, 0xA0, 0xFF, 0xA9, 0x00, 0xAA,
    0x0A, 0x85, 0x2B, 0xC8, 0xB9, 0x00, 0x02, 0xC9,
    0x8D, 0xF0, 0xD4, 0xC9, 0xAE, 0x90, 0xF4, 0xF0,
    0xF0, 0xC9, 0xBA, 0xF0, 0xEB, 0xC9, 0xD2, 0xF0,
    0x3B, 0x86, 0x28, 0x86, 0x29, 0x84, 0x2A, 0xB9,
    0x00, 0x02, 0x49, 0xB0, 0xC9, 0x0A, 0x90, 0x06,
    0x69, 0x88, 0xC9, 0xFA, 0x90, 0x11, 0x0A, 0x0A,
    0x0A, 0x0A, 0xA2, 0x04, 0x0A, 0x26, 0x28, 0x26,
    0x29, 0xCA, 0xD0, 0xF8, 0xC8, 0xD0, 0xE0, 0xC4,
    0x2A, 0xF0, 0x97, 0x24, 0x2B, 0x50, 0x10, 0xA5,
    0x28, 0x81, 0x26, 0xE6, 0x26, 0xD0, 0xB5, 0xE6,
    0x27, 0x4C, 0x44, 0xFF, 0x6C, 0x24, 0x00, 0x30,
    0x2B, 0xA2, 0x02, 0xB5, 0x27, 0x95, 0x25, 0x95,
    0x23, 0xCA, 0xD0, 0xF7, 0xD0, 0x14, 0xA9, 0x8D,
    0x20, 0xEF, 0xFF, 0xA5, 0x25, 0x20, 0xDC, 0xFF,
    0xA5, 0x24, 0x20, 0xDC, 0xFF, 0xA9, 0xBA, 0x20,
    0xEF, 0xFF, 0xA9, 0xA0, 0x20, 0xEF, 0xFF, 0xA1,
    0x24, 0x20, 0xDC, 0xFF, 0x86, 0x2B, 0xA5, 0x24,
    0xC5, 0x28, 0xA5, 0x25, 0xE5, 0x29, 0xB0, 0xC1,
    0xE6, 0x24, 0xD0, 0x02, 0xE6, 0x25, 0xA5, 0x24,
    0x29, 0x07, 0x10, 0xC8, 0x48, 0x4A, 0x4A, 0x4A,
    0x4A, 0x20, 0xE5, 0xFF, 0x68, 0x29, 0x0F, 0x09,
    0xB0, 0xC9, 0xBA, 0x90, 0x02, 0x69, 0x06, 0x2C,
    0x12, 0xD0, 0x30, 0xFB, 0x8D, 0x12, 0xD0, 0x60,
    0x00, 0x00, 0x00, 0x0F, 0x00, 0xFF, 0x00, 0x00
};

// Apple 1 / 8K RAM / WOZ Monitor

static int setup_apple1(struct cpu_t *cpu) {
   struct pia_t *pia = malloc(sizeof(struct pia_t));
   pia_init(pia);
   pia_trace(pia, 0);
   cpu_add_ram(cpu, 0x0000, 8 * 1024);
   cpu_add_rom_data(cpu, 0xff00, 0x0100, woz_monitor);
   cpu_add_iom(cpu, EWM_A1_PIA6820_ADDR, EWM_A1_PIA6820_LENGTH, pia, pia_read, pia_write);
   return 0;
}

// Replica 1 / 32K RAM / Krusader Assembler & Monitor

static int setup_replica1(struct cpu_t *cpu) {
   struct pia_t *pia = malloc(sizeof(struct pia_t));
   pia_init(pia);
   pia_trace(pia, 0);
   cpu_add_ram(cpu, 0x0000, 32 * 1024);
   cpu_add_rom_file(cpu, 0xe000, "roms/krusader.rom");
   cpu_add_iom(cpu, EWM_A1_PIA6820_ADDR, EWM_A1_PIA6820_LENGTH, pia, pia_read, pia_write);
   return 0;
}

// Apple ][+ / 48K RAM / Original ROMs

static int setup_apple2plus(struct cpu_t *cpu) {
   cpu_add_ram(cpu, 0x0000, 48 * 1024);
   cpu_add_rom_file(cpu, 0xd000, "roms/a2p.rom");
   return 0;
}

// Machine Setup

typedef int (*ewm_machine_setup_f)(struct cpu_t *cpu);

struct ewm_machine_t {
   char *name;
   char *description;
   ewm_machine_setup_f setup;
};

static struct ewm_machine_t machines[] = {
   { "apple1",     "Apple 1",   setup_apple1 },
   { "replica1",   "Replica 1", setup_replica1 },
   { "apple2plus", "Apple ][+", setup_apple2plus },
   { NULL,         NULL,        NULL }
};

static struct option options[] = {
   { "machine", required_argument, NULL, 'm' },
   { NULL,      0,                 NULL, 0   }
};

static struct ewm_machine_t *machine_with_name(char *name) {
   for (struct ewm_machine_t *m = machines; m->name != NULL; m++) {
      if (strcmp(m->name, name) == 0) {
         return m;
      }
   }
   return NULL;
}

int main(int argc, char **argv) {
   struct ewm_machine_t *machine = NULL;

   char ch;
   while ((ch = getopt_long(argc, argv, "m:", options, NULL)) != -1) {
      switch (ch) {
         case 'm':
            machine = machine_with_name(optarg);
            break;
      }
   }

   argc -= optind;
   argv += optind;

   if (machine == NULL) {
      fprintf(stderr, "Usage: ewm --machine apple1|replica1|apple2plus\n");
      exit(1);
   }

   struct cpu_t cpu;
   cpu_init(&cpu);

   (void) setup_replica1(&cpu);

   switch (cpu_boot(&cpu)) {
      case EWM_CPU_ERR_UNIMPLEMENTED_INSTRUCTION:
         fprintf(stderr, "CPU: Exited because of unimplemented instructions 0x%.2x at 0x%.4x\n",
                 mem_get_byte(&cpu, cpu.state.pc), cpu.state.pc);
         break;
      case EWM_CPU_ERR_STACK_OVERFLOW:
         fprintf(stderr, "CPU: Exited because of stack overflow at 0x%.4x\n", cpu.state.pc);
         break;
      case EWM_CPU_ERR_STACK_UNDERFLOW:
         fprintf(stderr, "CPU: Exited because of stack underflow at 0x%.4x\n", cpu.state.pc);
         break;
   }

   return 0;
}
