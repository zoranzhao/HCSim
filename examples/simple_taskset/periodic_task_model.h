/*********************************************                                                       
 * Periodic Task Model                                                                     
 * Parisa Razaghi, UT Austin, parisa.r@utexas.edu                                                    
 * Last update: June 2013                                                                            
 ********************************************/
#include <systemc>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "HCSim.h"
#include "lwip_task.h"

#ifndef SC_PERIODIC_TASK__H
#define SC_PERIODIC_TASK__H

class Periodic_Task
  :public sc_core::sc_module
  ,virtual public HCSim::OS_TASK_INIT 
{
 public:
  
  sc_core::sc_port< HCSim::OSAPI > os;

  SC_HAS_PROCESS(Periodic_Task);
  Periodic_Task(const sc_core::sc_module_name name, 
	       sc_dt::uint64 exe_cost, sc_dt::uint64 period, 
	       unsigned int priority, int id, uint8_t init_core,
	       sc_dt::uint64 end_sim_time)
    :sc_core::sc_module(name)
    {
      this->exe_cost = exe_cost;
      this->period = period;
      this->priority = priority;
      this->id = id;
      this->end_sim_time = end_sim_time;
      this->init_core = init_core;
 

//Init context for lwip and OMNeT++ in OS model
      this->lwip_task = new LwipCntxt();
      lwip_task->os_port(this->os);
//Init context for lwip and OMNeT++ in OS model

 
      SC_THREAD(run_jobs);
    }
  ~Periodic_Task() {}

  void OSTaskCreate(void)
  {
    os_task_id = os->taskCreate(sc_core::sc_gen_unique_name("periodic_task"), 
				HCSim::OS_RT_APERIODIC, priority, period, exe_cost, 
				HCSim::DEFAULT_TS, HCSim::ALL_CORES, init_core);
  }

 private:
  
  int id;
  uint8_t init_core;
  sc_dt::uint64 exe_cost;
  sc_dt::uint64 period;
  unsigned int priority;
  sc_dt::uint64 end_sim_time;
  int os_task_id;

//Init context for lwip and OMNeT++ in OS model
  LwipCntxt *lwip_task;
//Init context for lwip and OMNeT++ in OS model


  void cvtInt(char* str, int num)
  {
    sprintf( str, "%d", num );
  }
 
  void run_jobs(void)
  {
    FILE * df;
    char index[5];
    char file_name[20];
    int count, total_iterations;
    sc_dt::uint64 response_time;

    strcpy(file_name, "dump_task_");
    cvtInt(index, init_core);
    strcat(file_name, index);
    strcat(file_name, "_");
    cvtInt(index, id);
    strcat(file_name, index);
    df = (FILE *)fopen(file_name, "w");
     
    os->taskActivate(os_task_id);
    count = 0;
    total_iterations = end_sim_time / period;
 //   while (count < total_iterations) {


//Spawn tasks:
   
    sys_thread_new(lwip_task, OS_test, lwip_task);
    

//
 //     os->timeWait(exe_cost, os_task_id);
 //     os->timeWait(exe_cost, os_task_id);
      
     // os->taskEndCycle(os_task_id);
    response_time = os->getResponseTime(os_task_id);
//    fprintf(df,"%llu \n", response_time);
    count = count + 1;
   // }
    std::cout << "Before terminations ... ... " << std::endl;
    os->taskTerminate(os_task_id); 
    std::cout << "After terminations ... ... " << std::endl;
    fclose(df);
  }

};

#endif // SC_PERIODIC_TASK__H 
