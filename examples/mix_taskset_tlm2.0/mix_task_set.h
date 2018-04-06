/********************************************* 
 * Mix Task Set
 * Parisa Razaghi, UT Austin, parisa.r@utexas.edu
 * Last update: July 2013
 ********************************************/
#include <systemc>
#include <string>
#include <stdint.h>


#include "HCSim.h"

#include "config.h"
#include "periodic_task_model.h"
#include "intrdriven_task_model.h"

#ifndef SC_MIX_TASK_SET__H
#define SC_MIX_TASK_SET__H


class Mix_Task_Set
    :public sc_core::sc_module
{
 public:
    //sc_core::sc_vector< sc_core::sc_port< HCSim::receive_os_if > > intrCH;
    //sc_core::sc_vector< sc_core::sc_port< HCSim::send_os_if > > dataCH;
    sc_core::sc_port< HCSim::receive_os_if >  intrCH[2];
    sc_core::sc_port< HCSim::send_os_if >  dataCH[2];
    sc_core::sc_port< HCSim::OSAPI > os_port;

    Mix_Task_Set(const sc_core::sc_module_name name): sc_core::sc_module(name)
    {
        //intrCH.init(2);
        //dataCH.init(2);
        
        for (int t = 0; t < TOTAL_TASK_NUMBER; t++) {
            Periodic_Task * PT;
            PT = new Periodic_Task(sc_core::sc_gen_unique_name("periodic_task"), const_task_delay[t], 
                                                    const_task_period[t], const_task_priority[t], const_id[t], const_init_core[t], const_end_time);
            PT->os_port(os_port);
            periodic_tasks.push_back(PT);
        }
#ifndef NO_INTR                   
        for (int t = 0; t < TOTAL_INTR_TASK_NUMBER; t++) {      
            IntrDriven_Task * IT;
#ifdef INTH
            IT = new IntrDriven_Task(sc_core::sc_gen_unique_name("intrdriven_task"), const_task_delay_INTH[t], 
                                                    const_task_period_INTH[t], const_task_priority_INTH[t], const_id_INT[t], const_init_core_INT[t], const_end_time);
#endif
#ifdef INTM
			IT = new IntrDriven_Task(sc_core::sc_gen_unique_name("intrdriven_task"), const_task_delay_INTM[t],
									 const_task_period_INTM[t], const_task_priority_INTM[t], const_id_INT[\
																										  t], const_init_core_INT[t], const_end_time);

#endif
#ifdef INTL
			IT = new IntrDriven_Task(sc_core::sc_gen_unique_name("intrdriven_task"), const_task_delay_INTL[t],
									 const_task_period_INTL[t], const_task_priority_INTL[t], const_id_INT[\
																										  t], const_init_core_INT[t], const_end_time);

#endif
            IT->intrCH(intrCH[t]);
            IT->dataCH(dataCH[t]);
            IT->os_port(os_port);
            intrdriven_tasks.push_back(IT);
        }
#endif        
		
    }  

    
    ~Mix_Task_Set() 
    {
        for(unsigned int i  = 0 ; i< periodic_tasks.size(); i++) 
           delete periodic_tasks[i];
        periodic_tasks.clear();
#ifndef NO_INTR
        for(unsigned int i = 0; i < intrdriven_tasks.size(); i++) 
           delete  intrdriven_tasks[i];
        intrdriven_tasks.clear();    
#endif        
    };

    void end_of_elaboration()
    {
        for (int t = 0; t < TOTAL_TASK_NUMBER; t++)
            periodic_tasks[t]->OSTaskCreate();
#ifndef NO_INTR            
        for (int t = 0; t < TOTAL_INTR_TASK_NUMBER; t++)
          intrdriven_tasks[t]->OSTaskCreate();     
#endif          
    }
 
 private:

    std::vector< Periodic_Task * > periodic_tasks;
    std::vector< IntrDriven_Task * > intrdriven_tasks;

};

#endif //SC_MIX_TASK_SET__H
