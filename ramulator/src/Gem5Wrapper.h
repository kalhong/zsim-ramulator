#ifndef __GEM5_WRAPPER_H
#define __GEM5_WRAPPER_H

#include <string>

#include "Config.h"

using namespace std;

namespace ramulator
{

class Request;
class MemoryBase;

class Gem5Wrapper 
{
private:
    MemoryBase *mem;
    string stats_name;
public:
    double tCK;
    Gem5Wrapper(const Config& configs, string& statsPrefix, int cacheline);
    ~Gem5Wrapper();
    void tick();
    void printall();
    bool send(Request req);
    void finish(void);
};
} /*namespace ramulator*/

#endif /*__GEM5_WRAPPER_H*/
