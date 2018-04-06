/*********************************************
 * Parisa Razaghi, UT Austin, parisa.r@utexas.edu
 * Last update: June 2013
 ********************************************/
#include <systemc>
#include <stdint.h>

#include "HCSim.h"
#include "simple_set.h"

struct sys_sem sems[100];

int global=0;


int sc_main(int argc, char* argv[])
{

    sc_core::sc_set_time_resolution(1, SIM_RESOLUTION);
    HCSim::MCProcessor_FM< Simple_Set >* System = new HCSim::MCProcessor_FM< Simple_Set > ("simple_taskset", 2 /* cores*/, 1000 /*simulation quantum*/); 
    
    sc_core::sc_start();
    
    delete System;
    return 0;
}
