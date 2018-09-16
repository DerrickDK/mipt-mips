/*
 * simulator.h - interface for simulator
 * Copyright 2018 MIPT-MIPS
 */

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <infra/log.h>
#include <infra/target.h>
#include <infra/types.h>
#include <infra/exception.h>
 
#include <memory>

struct BearingLost final : Exception {
    BearingLost() : Exception("Bearing lost", "10 nops in a row") { }
};

struct NoBinaryFile final : Exception {
    NoBinaryFile() : Exception("No binary file loaded", "can't run simulator without binary file") { }
};

class Simulator : public Log {
public:
    explicit Simulator( bool log = false) : Log( log) {}

    enum class StopReason : int { Halted, BreakpointHit, SingleStep };

    virtual void load_binary_file( const std::string &tr) = 0;
    virtual void prepare_to_run() = 0;
    virtual void init( const std::string& tr) = 0;
    virtual StopReason run( uint64 instrs_to_run) = 0;
    void run_no_limit( const std::string& tr) { init( tr); run( MAX_VAL64); }
    virtual void set_target( const Target& target) = 0;
    virtual uint64 get_register(size_t) const { return 0; }
    virtual uint64 get_pc() const { return 0; }
    virtual uint32 read_uint32(Addr addr) const = 0;
    virtual void write_uint32(uint32 value, Addr addr) = 0;

    static std::unique_ptr<Simulator> create_simulator( const std::string& isa, bool functional_only, bool log);
    static std::unique_ptr<Simulator> create_configured_simulator();

    virtual size_t mem_read( Addr addr, unsigned char *buf, size_t length) const = 0;
    virtual size_t mem_read_noexcept( Addr addr, unsigned char *buf, size_t length) const noexcept = 0;
    virtual size_t mem_write( Addr addr, const unsigned char *buf, size_t length) = 0;
    virtual size_t mem_write_noexcept( Addr addr, const unsigned char *buf, size_t length) noexcept = 0;

protected:
    bool binary_file_loaded = false;
};

class CycleAccurateSimulator : public Simulator {
public:
    explicit CycleAccurateSimulator( bool log = false) : Simulator( log) {}
    virtual void clock() = 0;
    virtual void halt() = 0;
    static std::unique_ptr<CycleAccurateSimulator> create_simulator(const std::string& isa, bool log);
};

#endif // SIMULATOR_H
