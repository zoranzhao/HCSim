/*********************************************                                                       
 * Interrupt-Driven Task Model                                                                     
 * Parisa Razaghi, UT Austin, parisa.r@utexas.edu                                                    
 * Last update: July 2013                                                                            
 ********************************************/
#include <systemc>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "HCSim.h"

#ifndef SC_INTRDRIVEN_TASK__H
#define SC_INTRDRIVEN_TASK__H

class IntrDriven_Task
    :public sc_core::sc_module
  	,virtual public HCSim::OS_TASK_INIT 
{
 public:
    
    sc_core::sc_port< HCSim::receive_os_if > intrCH;
    sc_core::sc_port< HCSim::send_os_if > dataCH;
  	sc_core::sc_port< HCSim::OSAPI > os_port;

  	SC_HAS_PROCESS(IntrDriven_Task);
  	IntrDriven_Task(const sc_core::sc_module_name name, 
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
      
        SC_THREAD(run_jobs);	
    }
    
    ~IntrDriven_Task() {}

    void OSTaskCreate(void)
    {
        os_task_id = os_port->taskCreate(sc_core::sc_gen_unique_name("intrdriven_task"), 
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

        strcpy(file_name, "dump_task_");
        cvtInt(index, init_core);
        strcat(file_name, index);
        strcat(file_name, "_");
        cvtInt(index, id);
        strcat(file_name, index);
        df = (FILE *)fopen(file_name, "w");
       
		os_port->taskActivate(os_task_id);
		std::cout << sc_core::sc_time_stamp() << ":interrupt-driven task " << os_task_id <<" starts.\n";
		os_port->timeWait(1, os_task_id);
		os_port->syncGlobalTime(os_task_id);
		dataCH->send(os_task_id); // Reset the hardware
		count = 0;
        total_iterations = end_sim_time / period;
      
        while (count < total_iterations) {
		  intrCH->receive(os_task_id); // Receive an interrupt.
			os_port->timeWait(exe_cost, os_task_id);
			os_port->syncGlobalTime(os_task_id);
			dataCH->send(os_task_id); // Reset the hardware
			count = count + 1;
        }
   
        os_port->taskTerminate(os_task_id);
        fclose(df);
    }
};

#endif // SC_INTRDRIVEN_TASK__H 
