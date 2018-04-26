/*********************************************
 * Partitioned-RTOS with ATGA approach...  
 * Parisa Razaghi, UT Austin < parisa.r@utexas.edu>
 * Last update: Jun. 2013 
 ********************************************/
#include <stdint.h>
#include <systemc>

#include "OS/global_os.h"

#ifndef SC_OSAPI__H
#define SC_OSAPI__H

namespace HCSim {

/* OS API for multi-core SMP OS*/
class OSAPI
    :virtual public sc_core::sc_interface
{
 public:
    /***********************
     * OS intialization and startup
     **********************/
    virtual void init(uint8_t corenumber, sc_dt::uint64 sim_quantum) = 0 ;
    virtual void start() = 0;
    virtual void dynamicStart(uint8_t coreid)=0;    


    /*********************
     * Task management
     ********************/
    virtual OSProc taskCreate(const char *name, OSTaskType type, unsigned int priority, 
			  sc_dt::uint64 period, sc_dt::uint64 wcet, sc_dt::uint64 dts, 
			  sc_dt::uint64 affinity, uint8_t init_core) = 0;
    virtual void taskActivate(OSProc tID) = 0;
    virtual void taskSleep(OSProc tID) = 0;
    virtual void taskResume(OSProc tID) = 0;
    virtual void taskEndCycle(OSProc tID) = 0;
    virtual void taskTerminate(OSProc tID) = 0;
    virtual void taskKill(OSProc tID) = 0;      
    
    /************************
     * Delay modeling & Event handling
     ************************/ 
    virtual void preWait(OSProc tID, OSProc blocking_tID) = 0;
    virtual void postWait(OSProc tID) = 0;
    virtual void preNotify(OSProc tID, OSProc blocked_tID) = 0;
    virtual void postNotify(OSProc tID, OSProc blocked_tID) = 0;
    virtual void timeWait(sc_dt::uint64 sec, OSProc tID) = 0;
    virtual void syncGlobalTime(OSProc tID) = 0;
    
     /************************
     * Interrupt handling
     ************************/   
    virtual OSProc createIntrTask(const char *name, unsigned int priority,
								  sc_dt::uint64 affinity, uint8_t init_core, uint8_t init_launched_core) = 0;
    virtual void intrTrigger(OSProc intrID, uint8_t launcedCore) = 0;
    virtual void intrSleep(OSProc intrID) = 0;
    virtual OSProc createIntrHandler(uint8_t coreID, unsigned int priority) = 0;
    virtual void iEnter(uint8_t coreID, OSProc handlerID) = 0;
    virtual void iReturn(uint8_t coreID) = 0;
    
    /************************
     * Debug & monitor functions 
     ************************/ 
    virtual sc_dt::uint64 getResponseTime(OSProc tID) = 0;
    virtual sc_dt::uint64 getStartCycle(OSProc tID) = 0;
    virtual void resetStartCycle(OSProc tID) = 0;
#ifdef OS_STATISTICS_ON
    virtual unsigned long getTaskContextSwitches(uint8_t coreID) = 0;
    virtual unsigned long getOSContextCalled(uint8_t coreID) = 0;    
    virtual sc_dt::uint64 getBusyTime(uint8_t coreID) = 0;
    virtual sc_dt::uint64 getFallbackTime(uint8_t coreID) = 0;
    virtual void debugPrint()=0;
    virtual void statPrint()=0;     
    virtual void statRecordlwIP(sc_dt::uint64 delay)=0;     
    virtual void statRecordApp(sc_dt::uint64 delay)=0;     
    virtual void adjustBusyTime(uint8_t coreID, sc_dt::uint64 delay)=0;     
    virtual void incBusyTime(uint8_t coreID, sc_dt::uint64 delay)=0;     
#endif

};

class OS_TASK_INIT
    :virtual public sc_core::sc_interface
{
 public:
    virtual void OSTaskCreate() = 0;
};

class HAL_if
    :virtual public sc_core::sc_interface
{
 public:
    virtual void IRQEnter(int coreID) = 0;
};

} //HCSim

#endif //SC_OSAPI__H
