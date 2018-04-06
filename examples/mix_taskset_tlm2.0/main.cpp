/*********************************************
 * Parisa Razaghi, UT Austin, parisa.r@utexas.edu
 * Last update: July 2013
 ********************************************/
#include <systemc>
#include <stdint.h>

#include "HCSim.h"

#include "top_module.h"

int sc_main(int argc, char* argv[])
{
    sc_core::sc_set_time_resolution(1, SIM_RESOLUTION);
    artificial_example *System = new artificial_example ("mix_taskset_example"); 
    sc_core::sc_start();
    delete System;
    return 0;
}
