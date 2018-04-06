/*********************************************
 * Mix Task Set Example
 * Parisa Razaghi, UT Austin, parisa.r@utexas.edu
 * Last update: July 2013
 ********************************************/
#include <systemc>
#include <string.h>

#include "HCSim.h"

#include "config.h"

#ifndef SC_HINTERRUPTGENERATOR_TLM__H
#define SC_HINTERRUPTGENERATOR_TLM__H

extern sc_dt::uint64 start_time[MAX_INTR];

class HW_Standard_interruptGenerator
    :public sc_core::sc_module
{
 public:
    sc_core::sc_port< HCSim::IAmbaAhbBusSlaveMacLink > Port_tlm;
    sc_core::sc_port < HCSim::send_if > MainBus_HINT;
    
    SC_HAS_PROCESS(HW_Standard_interruptGenerator);
    HW_Standard_interruptGenerator(const sc_core::sc_module_name name, int id, unsigned int addr, 
            sc_dt::uint64 intr_period, sc_dt::uint64 end_time, int enabled)
        :sc_core::sc_module(name)
    {
        this->id = id;
        this->addr = addr;
        this->intr_period = intr_period;
        this->end_time = end_time;
        this->enabled = enabled;
        
        SC_THREAD(generator);  
    }          
    ~HW_Standard_interruptGenerator() {}
     
 private:
    int id;
    unsigned long addr;
    sc_dt::uint64 intr_period;
    sc_dt::uint64 end_time;
    int enabled;
    
    void cvtInt(char *str, int num)
    {
        sprintf( str, "%d", num );
    }
	
    void generator(void)
    {
        FILE * intr_fp;
        sc_dt::uint64 start_period, delay, intr_end, init_time;
        int data;
        int count, total_iteration;
        char filename[20];
        char index[5];
        unsigned int slot;
    	
    	strcpy(filename, "dump_intr_app_");
       	cvtInt(index, id);
       	strcat(filename, index);
    	intr_fp = (FILE *) fopen(filename, "w");
    	
        //wait(1, SIM_RESOLUTION);
    	count = 0;
    	slot = 0;
    	total_iteration = end_time / intr_period;
      	if (enabled) {
      	    Port_tlm->slaveRead(addr, (void *)&data, 4ull );
    		std::cout << sc_core::sc_time_stamp() << " - HW:enabled interrupt  \n";
    		init_time = sc_core::sc_time_stamp().value();
    		while(count < total_iteration) {
    			start_time[id] = sc_core::sc_time_stamp().value();
    			MainBus_HINT->send(); // send an interrupt //
    			Port_tlm->slaveRead(addr, (void *)&data, 4ull );
    			intr_end = sc_core::sc_time_stamp().value();
    			fprintf(intr_fp, "%llu \n", ((intr_end - start_time[id]) ));
    			slot = ((sc_core::sc_time_stamp().value()-init_time) / intr_period) + 1;
    			start_period = init_time + slot * intr_period; //start_period + intr_period;
    			delay = start_period - sc_core::sc_time_stamp().value();
    			sc_core::wait(delay, SIM_RESOLUTION);
				count++;
    		}
    	}   
    	fclose(intr_fp);
    }
};

class HW_Standard_interruptHandlerMonitor
    :sc_core::sc_module
{   
 public:
    sc_core::sc_port< HCSim::IAmbaAhbBusSlaveMacLink > Port_tlm;
    
    SC_HAS_PROCESS(HW_Standard_interruptHandlerMonitor);
    HW_Standard_interruptHandlerMonitor(const sc_core::sc_module_name name, int id, unsigned int addr, 
            sc_dt::uint64 intr_period, sc_dt::uint64 end_time, int enabled)
        :sc_core::sc_module(name)
    {
        this->id = id;
        this->addr = addr;
        this->intr_period = intr_period;
        this->end_time = end_time;
        this->enabled = enabled;
        
        SC_THREAD(monitor);
    }    
    ~HW_Standard_interruptHandlerMonitor() {}
    
 private:    
	int id;
    sc_dt::uint64 end_time;
    sc_dt::uint64 intr_period;
    unsigned long addr;
    int enabled;
    
    void cvtInt(char *str, int num)
    {
        sprintf( str, "%d", num );
    }
	
    void monitor(void)
    {
        FILE * intr_fp;
        sc_dt::uint64 intr_handler_rtime;
        int data;
        int count, total_iteration;
        char filename[20];
        char index[5];
    	
        strcpy(filename, "dump_intr_handler_");
        cvtInt(index, id);
        strcat(filename, index);
        intr_fp = (FILE *) fopen(filename, "w");
    	
        count = 0;
        total_iteration = end_time / intr_period;
        if (enabled) {
            while(count < total_iteration) {
                Port_tlm->slaveRead(addr, (void *)&data, 4ull );
                intr_handler_rtime = (sc_core::sc_time_stamp().value() - start_time[id]);
                //fprintf(intr_fp, "delay[%d] = \t %llu \n", count, intr_handler_rtime);
                fprintf(intr_fp, "%llu \n", intr_handler_rtime);
                count++;
            }
        }   
        fclose(intr_fp);
    }
    
};
 /*--------------------------------------------------------------------------------------------------------------------------*/
class HW_Standard_signalHandlerMonitor
    :sc_core::sc_module
{
 public:
    sc_core::sc_port< HCSim::IAmbaAhbBusSlaveMacLink > Port_tlm;

    SC_HAS_PROCESS(HW_Standard_signalHandlerMonitor);
    HW_Standard_signalHandlerMonitor(const sc_core::sc_module_name name,
            int id, unsigned int addr, sc_dt::uint64 intr_period, sc_dt::uint64 end_time, int enabled)
        :sc_core::sc_module(name)
    {
        this->id = id;
        this->addr = addr;
        this->intr_period = intr_period;
        this->end_time = end_time;
        this->enabled = enabled;

        SC_THREAD(monitor);
    }        
    ~HW_Standard_signalHandlerMonitor() {}
    
 private:    
    int id;
    sc_dt::uint64 end_time;
    sc_dt::uint64 intr_period;
    unsigned long addr;
    int enabled;
    
    void cvtInt(char *str, int num)
    {
        sprintf( str, "%d", num );
    }
	
    void monitor(void)
    {
        FILE * intr_fp;
        sc_dt::uint64 intr_task_rtime;
        int data;
        int count, total_iteration;
        char filename[20];
        char index[5];

    	
        strcpy(filename, "dump_intr_task_");
        cvtInt(index, id);
        strcat(filename, index);
        intr_fp = (FILE *) fopen(filename, "w");
    	   	
        count = 0;
        total_iteration = end_time / intr_period;
    	    	
        if (enabled) {
            while (count < total_iteration) {
                Port_tlm->slaveRead(addr, (void *)&data, 4ull );
                intr_task_rtime = (sc_core::sc_time_stamp().value() - start_time[id]) ;
    			//fprintf(intr_fp, "%llu: delay[%d] = \t %llu \n", start_time, count, intr_task_rtime);
                fprintf(intr_fp, "%llu \n", intr_task_rtime);
                count++;
            }
        }   
        fclose(intr_fp);
    }
    
};
/*--------------------------------------------------------------------------------------------------------------------------*/

#endif // SC_HINTERRUPTGENERATOR_TLM__H


