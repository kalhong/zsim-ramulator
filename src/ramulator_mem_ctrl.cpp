
#include "ramulator_mem_ctrl.h"
#include <map>
#include <string>
#include "event_recorder.h"
#include "tick_event.h"
#include "timing_event.h"
#include "zsim.h"

#ifdef _WITH_RAMULATOR_
#include "src/Gem5Wrapper.h"
#include "src/Config.h"
#include "src/Request.h"
#include <cassert>
#include <iostream>

using namespace ramulator;


class RamulatorAccEvent : public TimingEvent {
    private:
        RamulatorMemory* dram;
        bool write;
        Address addr;

    public:
        uint64_t sCycle;

        RamulatorAccEvent(RamulatorMemory* _dram, bool _write, Address _addr, int32_t domain) :  TimingEvent(0, 0, domain), dram(_dram), write(_write), addr(_addr) {}

        bool isWrite() const {
            return write;
        }

        Address getAddr() const {
            return addr;
        }

        void simulate(uint64_t startCycle) {
            sCycle = startCycle;
            dram->enqueue(this, startCycle);
        }
};

RamulatorMemory::RamulatorMemory(string& _ramulator_config, uint32_t _ramulator_stat_interval, string& _statsPrefix,
        uint64_t _cpuFreqHz, uint32_t _minLatency, uint32_t _domain, const g_string& _name)
{
    curCycle = 0;
    minLatency = _minLatency;
    stat_interval = _ramulator_stat_interval;

    Config configs(_ramulator_config);
    configs.set_core_num(zinfo->numCores);

    wrapper = new Gem5Wrapper(configs, _statsPrefix, (int)64);

    cpu_ns = (1000000000.0/_cpuFreqHz);
    mem_ns = wrapper->tCK;
    diff_ns = 0;
    assert(mem_ns > cpu_ns);

    //callback
    cb_func = std::bind(&RamulatorMemory::readComplete, this, std::placeholders::_1);

    domain = _domain;
    TickEvent<RamulatorMemory>* tickEv = new TickEvent<RamulatorMemory>(this, domain);
    tickEv->queue(0);  // start the sim at time 0

    name = _name;
}

void RamulatorMemory::initStats(AggregateStat* parentStat) {
    AggregateStat* memStats = new AggregateStat();
    memStats->init(name.c_str(), "Memory controller stats");
    profReads.init("rd", "Read requests"); memStats->append(&profReads);
    profWrites.init("wr", "Write requests"); memStats->append(&profWrites);
    profTotalRdLat.init("rdlat", "Total latency experienced by read requests"); memStats->append(&profTotalRdLat);
    profTotalWrLat.init("wrlat", "Total latency experienced by write requests"); memStats->append(&profTotalWrLat);
    parentStat->append(memStats);
}

uint64_t RamulatorMemory::access(MemReq& req) {
    switch (req.type) {
        case PUTS:
        case PUTX:
            *req.state = I;
            break;
        case GETS:
            *req.state = req.is(MemReq::NOEXCL)? S : E;
            break;
        case GETX:
            *req.state = M;
            break;

        default: panic("!?");
    }

    uint64_t respCycle = req.cycle + minLatency;
    assert(respCycle > req.cycle);

    if ((req.type != PUTS /*discard clean writebacks*/) && zinfo->eventRecorders[req.srcId]) {
        Address addr = req.lineAddr << lineBits;
        bool isWrite = (req.type == PUTX);
        RamulatorAccEvent* memEv = new (zinfo->eventRecorders[req.srcId]) RamulatorAccEvent(this, isWrite, addr, domain);
        memEv->setMinStartCycle(req.cycle);
        TimingRecord tr = {addr, req.cycle, respCycle, req.type, memEv, memEv};
        zinfo->eventRecorders[req.srcId]->pushRecord(tr);
    }
    return respCycle;
}

uint32_t RamulatorMemory::tick(uint64_t cycle) {
    curCycle++;
    diff_ns += cpu_ns;
    if(diff_ns >= mem_ns) {
        wrapper->tick();
        diff_ns -= mem_ns;

        if(waiting_req.size()) {
            auto prev_req = waiting_req.begin();
            if(wrapper->send(*prev_req)) {
                waiting_req.pop_front();
            }
        }
    }

    if(curCycle%stat_interval == 0) {
        wrapper->finish();
        wrapper->printall();
    }

    return 1;
}

void RamulatorMemory::readComplete(ramulator::Request& req) {
    std::multimap<uint64_t, RamulatorAccEvent*>::iterator it = inflightRequests.find(uint64_t(req.addr));
    assert((it != inflightRequests.end()));
    RamulatorAccEvent* ev = it->second;
    
    uint32_t lat = curCycle+1 - ev->sCycle;
    if (ev->isWrite()) {
        profWrites.inc();
        profTotalWrLat.inc(lat);
    } else {
        profReads.inc();
        profTotalRdLat.inc(lat);
    }
    ev->release();
    ev->done(curCycle+1);
    inflightRequests.erase(it);
}

void RamulatorMemory::enqueue(RamulatorAccEvent* ev, uint64_t cycle) {
    Request::Type req_type;
    if(ev->isWrite())
        req_type = Request::Type::WRITE;
    else
        req_type = Request::Type::READ;

    // to use 2GB ramulator memory, to prevent the uint64_t to long overflow
    uint64_t ramulator_addr = ev->getAddr()%(1<<31);
    Request req(ramulator_addr, req_type, cb_func, 0);

    if(wrapper->send(req) == false)  
        waiting_req.push_back(req);
    
    inflightRequests.insert(std::pair<Address, RamulatorAccEvent*>(ramulator_addr, ev));
    ev->hold();
}

#else
#endif
