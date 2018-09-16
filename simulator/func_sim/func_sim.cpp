/*
 * func_sim.cpp - extremely simple simulator
 * Copyright 2018 MIPT-MIPS
 */

#include "func_sim.h"

template <typename ISA>
FuncSim<ISA>::FuncSim( bool log) : Simulator( log), mem( new Memory) { }

template <typename ISA>
void FuncSim<ISA>::update_and_check_nop_counter( const typename FuncSim<ISA>::FuncInstr& instr)
{
    if ( instr.is_nop())
        ++nops_in_a_row;
    else
        nops_in_a_row = 0;

    if (nops_in_a_row > 10)
        throw BearingLost();
}

template <typename ISA>
typename FuncSim<ISA>::FuncInstr FuncSim<ISA>::step()
{
    // fetch instruction
    FuncInstr instr = mem->fetch_instr( PC);

    // set sequence_id
    instr.set_sequence_id(sequence_id);
    sequence_id++;

    // read sources
    rf.read_sources( &instr);

    // execute
    instr.execute();

    // load/store
    mem->load_store( &instr);

    // writeback
    rf.write_dst( instr);

    // trap check
    instr.check_trap();

    // PC update
    PC = instr.get_new_PC();

    // Check whether we execute nops
    update_and_check_nop_counter( instr);

    // dump
    return instr;
}

template <typename ISA>
Simulator::StopReason FuncSim<ISA>::run( uint64 instrs_to_run)
{
    if (!binary_file_loaded)
        throw NoBinaryFile();
    for ( uint64 i = 0; i < instrs_to_run; ++i) {
        const auto& instr = step();
        sout << instr << std::endl;

        if ( instrs_to_run == 1)
            return StopReason::SingleStep;
        if ( instr.is_break())
            return StopReason::BreakpointHit;
        if ( instr.is_halt())
            break; /* Cant't return Halted here because of 'control reaches end of non-void' */
    }
    return StopReason::Halted;
}

template <typename ISA>
void FuncSim<ISA>::load_binary_file( const std::string &tr) {
    mem->load_elf_file( tr);
    binary_file_loaded = true;
}

template <typename ISA>
void FuncSim<ISA>::prepare_to_run() {
    PC = mem->startPC();
    nops_in_a_row = 0;
}

template <typename ISA>
size_t FuncSim<ISA>::mem_read( Addr addr, unsigned char *buf, size_t length) const {
    return mem->memcpy_guest_to_host( reinterpret_cast<Byte *>(buf), addr, length);
}

template <typename ISA>
size_t FuncSim<ISA>::mem_read_noexcept( Addr addr, unsigned char *buf, size_t length) const noexcept try {
    return mem_read( addr, buf, length);
}
catch (...) {
    return 0;
}

template <typename ISA>
size_t FuncSim<ISA>::mem_write( Addr addr, const unsigned char *buf, size_t length) {
    return mem->memcpy_host_to_guest( addr, reinterpret_cast<const Byte *>(buf), length);
}

template <typename ISA>
size_t FuncSim<ISA>::mem_write_noexcept( Addr addr, const unsigned char *buf, size_t length) noexcept try {
    return mem_write( addr, buf, length);
}
catch (...) {
    return 0;
}

template<typename ISA>
uint32 FuncSim<ISA>::read_uint32(Addr addr) const {
    return mem->template read<uint32>(addr);
}

template<typename ISA>
void FuncSim<ISA>::write_uint32(Addr addr, uint32 word) {
    mem->template write<uint32>(word, addr);
}

#include <mips/mips.h>
#include <risc_v/risc_v.h>

template class FuncSim<MIPSI>;
template class FuncSim<MIPSII>;
template class FuncSim<MIPSIII>;
template class FuncSim<MIPSIV>;
template class FuncSim<MIPS32>;
template class FuncSim<MIPS64>;
template class FuncSim<RISCV32>;
template class FuncSim<RISCV64>;
template class FuncSim<RISCV128>;

