#ifndef RAMULATOR_MEM_CTRLS_H_
#define RAMULATOR_MEM_CTRLS_H_

#include <map>
#include <deque>
#include <string>
#include "g_std/g_string.h"
#include "memory_hierarchy.h"
#include "pad.h"
#include "stats.h"
#include <functional>

namespace ramulator {
    class Gem5Wrapper;
    class Config;
    class Request;
}

class RamulatorAccEvent;

/* Simple memory (or memory bank), has a fixed latency */
class RamulatorMemory : public MemObject {
    private:
        g_string name;
        uint32_t minLatency;
        uint32_t domain;

        ramulator::Gem5Wrapper* wrapper;

        std::multimap<uint64_t, RamulatorAccEvent*> inflightRequests;

        uint32_t stat_interval;

        double cpu_ns;
        double mem_ns;
        double diff_ns;
        
        uint64_t curCycle;
        std::deque<ramulator::Request> waiting_req;
        // R/W stats
        PAD();
        Counter profReads;
        Counter profWrites;
        Counter profTotalRdLat;
        Counter profTotalWrLat;
        PAD();

    public:
        RamulatorMemory(std::string& _ramulator_config, uint32_t _ramulator_stat_interval, std::string& _statsPrefix, 
                uint64_t _cpuFreqHz, uint32_t _minLatency, uint32_t _domain, const g_string& _name);

        const char* getName() {return name.c_str();}
        void initStats(AggregateStat* parentStat);
        uint64_t access(MemReq& req);
        uint32_t tick(uint64_t cycle);
        void enqueue(RamulatorAccEvent* ev, uint64_t cycle);

    private:
        void DRAM_return_cb(uint32_t id, uint64_t addr, uint64_t returnCycle);
        void readComplete(ramulator::Request& req);
        std::function<void(ramulator::Request&)> cb_func;
};

#endif  // MEM_CTRLS_H_
