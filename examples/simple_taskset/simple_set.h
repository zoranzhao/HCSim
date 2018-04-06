/********************************************* 
 * Simple Task Set
 * Parisa Razaghi, UT Austin, parisa.r@utexas.edu
 * Last update: June 2013
 ********************************************/
#include <systemc>
#include <string>
#include <stdint.h>

#include "HCSim.h"

#include "periodic_task_model.h"

#ifndef SC_SIMPLE_TASK_SET__H
#define SC_SIMPLE_TASK_SET__H

const unsigned long long const_end_time =  5000000000ull;


#define TOTAL_TASK_NUMBER 1

const sc_dt::uint64 const_task_delay[] = { 8006000ull,  6008000ull, 20011000ull,  13011000ull,  6008000ull,  5008000ull,  6008000ull};
const sc_dt::uint64 const_task_period[] ={49000000ull, 30000000ull, 70000000ull,  65000000ull, 31000000ull, 39000000ull, 40000000ull};
const unsigned int const_task_priority[] ={49, 30, 70, 65, 31, 39, 40};
const int const_init_core[] = {0, 0, 0, 1, 1, 1, 1};
const int const_id[] = {0, 1, 2, 0, 1, 2, 3};


class Simple_Set
:public sc_core::sc_module
{
 public:
  sc_core::sc_port< HCSim::OSAPI > os;

  Simple_Set(const sc_core::sc_module_name name): sc_core::sc_module(name)
  {
        for (int t = 0; t < TOTAL_TASK_NUMBER; t++) {
            Periodic_Task * PT;
            PT = new Periodic_Task(sc_core::sc_gen_unique_name("periodic_task"), const_task_delay[t], 
                                                    const_task_period[t], const_task_priority[t], const_id[t], const_init_core[t], const_end_time);
            PT->os(os);
            periodic_tasks.push_back(PT);
        }
  } 
    
  ~Simple_Set() 
  {
        for(unsigned int i  = 0 ; i< periodic_tasks.size(); i++) 
           delete periodic_tasks[i];
        periodic_tasks.clear();
  };

  void end_of_elaboration()
  {
        for (int t = 0; t < TOTAL_TASK_NUMBER; t++)
            periodic_tasks[t]->OSTaskCreate();
  }
 private:

    std::vector< Periodic_Task * > periodic_tasks;

};
#endif //SC_SIMPLE_TASK_SET__H
