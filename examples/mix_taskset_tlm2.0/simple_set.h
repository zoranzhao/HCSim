/********************************************* 
 * Simple Task Set
 * Parisa Razaghi, UT Austin, parisa.r@utexas.edu
 * Last update: June 2013
 ********************************************/
#include <systemc>
#include <string>
#include <stdint.h>

#include "OS/PRTOS.h"
#include "periodic_task_model.h"

#ifndef SC_SIMPLE_TASK_SET__H
#define SC_SIMPLE_TASK_SET__H

class Simple_Set
:public sc_core::sc_module
{
 public:
  sc_core::sc_port< HCSim::OSAPI > os;

  Simple_Set(const sc_core::sc_module_name name): sc_core::sc_module(name)
  {
    T1 = new Periodic_Task("T1", 3, 10, 1,  0,  0, 100);
    T1->os(os);

    T2 = new Periodic_Task("T2", 3, 10, 1, 1,  1, 100);
    T2->os(os);
  } 

    
  ~Simple_Set() 
  {
    delete T1;
    delete T2;
  };

  void end_of_elaboration()
  {
    T1->os_task_create();
    T2->os_task_create();
  }
 private:

  Periodic_Task * T1;
  Periodic_Task * T2;

};
#endif //SC_SIMPLE_TASK_SET__H
