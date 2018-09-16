//
// vr4300/cpu.h: VR4300 processor container.
//
// CEN64: Cycle-Accurate Nintendo 64 Emulator.
// Copyright (C) 2015, Tyler J. Stachecki.
//
// This file is subject to the terms and conditions defined in
// 'LICENSE', which is part of this source code package.
//

#ifndef __vr4300_cpu_h__
#define __vr4300_cpu_h__

#include <infra/types.h>

enum rcp_interrupt_mask {
  MI_INTR_SP = 0x01,
  MI_INTR_SI = 0x02,
  MI_INTR_AI = 0x04,
  MI_INTR_VI = 0x08,
  MI_INTR_PI = 0x10,
  MI_INTR_DP = 0x20
};

struct vr4300;
struct vr4300_stats;
struct memory_map;

// Callback functions to handle reads/writes.
typedef int (*memory_rd_function)(void *, uint32_t, uint32_t *);
typedef int (*memory_wr_function)(void *, uint32_t, uint32_t, uint32_t);

extern "C" {
    
struct memory_map* allocate_memory_map();
void create_memory_map(struct memory_map *map);

int map_address_range(struct memory_map *memory_map,
  uint32_t start, uint32_t length, void *instance,
  memory_rd_function on_read, memory_wr_function on_write);

int memory_read_word(struct memory_map *map, uint32_t address, uint32_t *word);
int memory_write_word(struct memory_map *map, uint32_t address, uint32_t word, uint32_t dqm);
  
void memory_free(struct memory_map *map);

struct vr4300* vr4300_alloc();
void vr4300_free(struct vr4300*);

struct vr4300_stats* vr4300_stats_alloc();
void vr4300_stats_free(struct vr4300_stats*);

int vr4300_init(struct vr4300 *vr4300, struct bus_controller *bus);
void vr4300_cp1_init(struct vr4300 *vr4300);

void vr4300_cycle(struct vr4300 *vr4300);
void vr4300_cycle_extra(struct vr4300 *vr4300, struct vr4300_stats *stats);

uint64 vr4300_get_register(struct vr4300 *vr4300, std::size_t i);
uint64 vr4300_get_pc(struct vr4300 *vr4300);

int read_mi_regs(void *opaque, uint32 address, uint32 *word);
int write_mi_regs(void *opaque, uint32 address, uint32 word, uint32 dqm);

void clear_rcp_interrupt(struct vr4300 *vr4300, enum rcp_interrupt_mask mask);
void signal_rcp_interrupt(struct vr4300 *vr4300, enum rcp_interrupt_mask mask);

void clear_dd_interrupt(struct vr4300 *vr4300);
void signal_dd_interrupt(struct vr4300 *vr4300);

}

#endif

