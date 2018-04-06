/*********************************************                       
 * Partitioned-RTOS with ATGA approach... 
 * Parisa Razaghi, UT Austin <parisa.r@utexas.edu> 
 * Last update: Jun. 2013                                            
 ********************************************/
#include <stdint.h>
#include <systemc>
#ifndef SYSTEMC_2_3_1
#include <vector>
#endif

#include "sim_config.h"
#include "OS/OSAPI.h"
#include "Channels/handshake_ch.h"

#ifndef	SC_PRTOS_ATGA__H
#define	SC_PRTOS_ATGA__H

namespace HCSim {

/* Abstract multi-core OS; SystemC implementation */
class RTOS
    :public sc_core::sc_channel
    ,virtual public HCSim::OSAPI
{
 public:

    int NodeID;

    RTOS();
    RTOS(const sc_core::sc_module_name name);
    virtual ~RTOS();
  
    /***********************
     * OS API methods
     **********************/
    void init(uint8_t core_number, sc_dt::uint64 simulation_quantum);
    void start();
    void dynamicStart(uint8_t coreid);    

    OSProc taskCreate(const char *name, OSTaskType type, unsigned int priority, 
		    sc_dt::uint64 period, sc_dt::uint64 wcet, sc_dt::uint64 dts, 
		    sc_dt::uint64 affinity, uint8_t coreid);
    void taskActivate(OSProc tID);
    void taskSleep(OSProc tID);
    void taskResume(OSProc tID);
    void taskTerminate(OSProc tID);
    void taskEndCycle(OSProc tID);
    void taskKill(OSProc tID);
    void preWait(OSProc tID, OSProc btID);
    void postWait(OSProc tID);
    void preNotify(OSProc tID, OSProc btID);
    void postNotify(OSProc tID, OSProc btID);
    void timeWait(sc_dt::uint64 sec, OSProc p);
    void syncGlobalTime(OSProc tID);
	OSProc createIntrTask(const char *name, unsigned int priority,
						  sc_dt::uint64 affinity, uint8_t init_core, uint8_t init_launched_core);
    void intrTrigger(OSProc intrID, uint8_t launcedCore);
    void intrSleep(OSProc intrID);
    OSProc createIntrHandler(uint8_t coreID, unsigned int priority);
    void iEnter(uint8_t coreID, OSProc handlerID);
    void iReturn(uint8_t coreID);
    sc_dt::uint64 getResponseTime(OSProc tID);
    sc_dt::uint64 getStartCycle(OSProc tID);
    void resetStartCycle(OSProc tID);
#ifdef OS_STATISTICS_ON
    unsigned long getTaskContextSwitches(uint8_t coreID);
    unsigned long getOSContextCalled(uint8_t coreID);    
    sc_dt::uint64 getBusyTime(uint8_t coreID);
    sc_dt::uint64 getFallbackTime(uint8_t coreID);
    void debugPrint();
    void statPrint();
    void statRecordlwIP(sc_dt::uint64 delay);     
    void statRecordApp(sc_dt::uint64 delay);   
    void adjustBusyTime(uint8_t coreID, sc_dt::uint64 delay);  
    void incBusyTime(uint8_t coreID, sc_dt::uint64 delay);                  
#endif

 private:
    void OSAbort(int err);
    void wait4Sched(OSProc p);
    void sendSched(OSProc p);
    void dispatch(uint8_t coreid);
    void schedule(uint8_t coreid);
    void consumeAccumulatedDelay(OSProc p);
    void endProcess(OSProc p);
    sc_dt::uint64 getPredictedDelay(OSProc p);
    bool checkFallbackMode(OSProc p);
    bool checkIntrDependency(OSProc p);
    void load_balance();
    void insertBeginPriority(OSProc p, OSQueue *que);
    void insertEndPriority(OSProc p, OSQueue *que);
    void insertEndPeriod(OSProc p, OSQueue *que);
    void freeTask(OSProc p, OSQueue *que);
    void extractTask(OSProc p, OSQueue *que);
    bool empty( OSQueue *que);
    OSProc getFirstTask(OSQueue *que);
    OSProc peekFirstTask(OSQueue *que);
  
 private:
#ifdef SYSTEMC_2_3_1
    sc_core::sc_vector< handshake_ch > os_sched_event_list; /* scheduler event list */
#else
     //std::vector< handshake_ch > os_sched_event_list;//OS_MAXPROC); /* scheduler event list */
    handshake_ch os_sched_event_list[OS_MAXPROC];
#endif
    sc_core::sc_event os_intrhandler_event_list[OS_MAXCORE];
    struct TCB os_vdes[OS_MAXPROC]; /* task control block */
    OSProc     os_current[OS_MAXCORE]; /* tasks in execution */
    uint8_t    os_core_number; /* number of cores */
    sc_dt::uint64 os_simulation_quantum; /* simulation quantum */
    
    /*
     * OS internal queues
     */
    OSQueue os_intrhandler_ready_queue[OS_MAXCORE]; /* interrupt handler ready os queue*/
    OSQueue os_ready_queue[OS_MAXCORE]; /* ready os queue */
    OSQueue os_sleep_queue[OS_MAXCORE]; /* sleep os queue */
    OSQueue os_wait_queue[OS_MAXCORE]; /* wait os queue */
    OSQueue os_intrwait_queue[OS_MAXCORE]; /* intr wait os queue*/
    OSQueue os_idle_queue[OS_MAXCORE]; /* idle os queue */
    OSQueue os_zombie_queue; /* zombie os queue */
    OSQueue os_freetcb_queue; /* zombie os queue */

#ifdef OS_STATISTICS_ON
    unsigned long context_cnt[OS_MAXCORE]; // # of dispatch called
    unsigned long task_switch_cnt[OS_MAXCORE]; // # of context-switch 
    OSProc last_sched_task[OS_MAXCORE];
    sc_dt::uint64 AppDuration;
    sc_dt::uint64 lwIPDuration;   
    sc_dt::uint64 wait_for_time_cnt;
    sc_dt::uint64 fallback_duration[OS_MAXCORE];
    sc_dt::uint64 busy_duration[OS_MAXCORE];
    sc_dt::uint64 os_busy_time[OS_MAXCORE];
    sc_dt::uint64 adjust_duration[OS_MAXCORE];
    sc_dt::uint64 inc_busy_duration[OS_MAXCORE];;   
#endif    

};

} //namespace HCSim

#endif // SC_PRTOS_ATGA__H
