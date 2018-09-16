//
// vr4300/cpu.c: VR4300 processor container.
//
// CEN64: Cycle-Accurate Nintendo 64 Emulator.
// Copyright (C) 2015, Tyler J. Stachecki.
//
// This file is subject to the terms and conditions defined in
// 'LICENSE', which is part of this source code package.
//

#include "cen64_intf.h"
#include <simulator.h>
#include "bus/controller.h"

struct memory_map {
    std::unique_ptr<CycleAccurateSimulator> sim;
};

struct vr4300 {
    CycleAccurateSimulator* sim;
};
   
struct memory_map* allocate_memory_map() {
    return new memory_map();
}

void create_memory_map(struct memory_map *map) {
    map->sim = CycleAccurateSimulator::create_simulator("mips32", false);
}

int map_address_range(struct memory_map *,
  uint32_t, uint32_t, void *,
  memory_rd_function, memory_wr_function) { return 0; }

int memory_read_word(struct memory_map *map, uint32_t address, uint32_t *word) {
    *word = map->sim->read_uint32(address);
    return 0;
}

int memory_write_word(struct memory_map *map, uint32_t address, uint32_t word, uint32_t dqm) {
    map->sim->write_uint32(word & dqm, address);
    return 0;
}
  
void memory_free(struct memory_map *map) {
    delete map;
}

struct vr4300* vr4300_alloc() {
    return new vr4300();
}

void vr4300_free(struct vr4300* ptr) {
    delete ptr;
}

struct vr4300_stats* vr4300_stats_alloc() {
   return nullptr;
}

void vr4300_stats_free(struct vr4300_stats*) {}

int vr4300_init(struct vr4300 *vr4300, struct bus_controller * bus)
{
    vr4300->sim = bus->map->sim.get();
    return 0;
}

void vr4300_cp1_init(struct vr4300 *) {}

void vr4300_cycle(struct vr4300 * vr4300)
{
    vr4300->sim->clock();
}

void vr4300_cycle_extra(struct vr4300*, struct vr4300_stats *)
{

}

uint64_t vr4300_get_register(struct vr4300 *vr4300, size_t i)
{
    return vr4300->sim->get_register(i);
}

uint64_t vr4300_get_pc(struct vr4300 *vr4300)
{
    return vr4300->sim->get_pc();
}

int read_mi_regs(void * /* opaque */, uint32_t /* address */, uint32_t * /* word */) { return 0; }
int write_mi_regs(void * /* opaque */, uint32_t /* address */, uint32_t /* word */, uint32_t /* dqm */) { return 0; }

void clear_rcp_interrupt(struct vr4300 *, enum rcp_interrupt_mask) {}
void signal_rcp_interrupt(struct vr4300 *, enum rcp_interrupt_mask) {}

void clear_dd_interrupt(struct vr4300 *) {}
void signal_dd_interrupt(struct vr4300 *) {}

