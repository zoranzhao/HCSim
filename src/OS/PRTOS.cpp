/*********************************************
 * Partitioned-RTOS with ATGA approach...
 * Parisa Razaghi, UT Austin <parisa.r@utexas.edu>
 * Last update: Jun. 2013
 ********************************************/
#include <iomanip>
#include <fstream>
#include <sstream>

#include "OS/PRTOS.h"

int total_input_sets;
int total_cli_num;
extern double CliEnergy[10];//definition from inet energyStorage lib 
extern double power_cli[10];//indexed with [NodeID-1]

using namespace HCSim; 

/*
 * Default constructor
 */
RTOS::RTOS()
    :sc_core::sc_channel(sc_core::sc_gen_unique_name("RTOS"))
#ifdef SYSTEMC_2_3_1    
    ,os_sched_event_list("sch_event_ch", OS_MAXPROC)
#else
    //,os_sched_event_list(OS_MAXPROC)    
#endif
{
}
/*
 *
 */
RTOS::RTOS(const sc_core::sc_module_name name):
    sc_core::sc_channel(name)
#ifdef SYSTEMC_2_3_1
    ,os_sched_event_list("sch_event_ch", OS_MAXPROC)
#endif    
{
}
/*
 *
 */
void RTOS::statRecordlwIP(sc_dt::uint64 delay){
    lwIPDuration=lwIPDuration+delay; 
}    


void RTOS::statRecordApp(sc_dt::uint64 delay){
    //std::cout<<"AppDuration is ... ... ... ... ...: "<<AppDuration<<"\n";
    AppDuration=AppDuration+delay; 
}  

void RTOS::adjustBusyTime(uint8_t coreID, sc_dt::uint64 delay){
	//std::cout<<"adjust_duration is: "<<delay<<"\n";
	adjust_duration[coreID] += delay;
}   
  
void RTOS::incBusyTime(uint8_t coreID, sc_dt::uint64 delay){
	//std::cout<<"adjust_duration is: "<<delay<<"\n";
	busy_duration[coreID] += delay;
	inc_busy_duration[coreID] += delay;
}    

void RTOS::statPrint(){
    std::ofstream jFile;
    std::string cli_num_s;
    std::stringstream cli_num_stm;
    cli_num_stm << NodeID;
    cli_num_s = cli_num_stm.str();

    double latency = sc_core::sc_time_stamp().value();
    double core_total = (double)(busy_duration[0] + busy_duration[1] - adjust_duration[0]  - adjust_duration[1])/(latency);
    double core_lwip = core_total*((double)(lwIPDuration))/((double)(lwIPDuration+AppDuration));
    double core_app = core_total*((double)(AppDuration))/((double)(lwIPDuration+AppDuration));  


    if(NodeID ==0){
       jFile.open ("./json/srv.json");
    }else{
       jFile.open ("./json/cli"+cli_num_s+".json");
    }
    jFile << "{\n";
    jFile <<"     \"adjust_duration\": "<<( adjust_duration[0]  + adjust_duration[1])<<",\n";
    jFile <<"     \"inc_busy_duration\": "<<(inc_busy_duration[0] + inc_busy_duration[1])<<",\n";
    jFile <<"     \"total_duration\": "<<(busy_duration[0] + busy_duration[1] - adjust_duration[0]  - adjust_duration[1])<<",\n";
    jFile <<"     \"app_duration\": "<<AppDuration<<",\n";
    jFile <<"     \"lwip_duration\": "<<lwIPDuration<<",\n";
    jFile <<"     \"lwip_core\": "<<core_lwip<<",\n";
    jFile <<"     \"app_core\": "<<core_app<<",\n";
    if(NodeID != 0){
    	jFile <<"     \"o2o_nic_energy\": "<<CliEnergy[NodeID-1]/total_input_sets <<",\n";
    	jFile <<"     \"core_power\": "<<power_cli[NodeID-1]<<",\n";
    }
    jFile <<"     \"o2o_delay\": "<<(latency/total_input_sets)/(1000000000000)<<",\n";
    jFile <<"     \"o2o_app_duration\": " <<   ( ((double)(AppDuration))/total_input_sets )/(1000000000000)<<",\n";
    jFile <<"     \"o2o_lwip_duration\": "<<   ( ((double)(lwIPDuration))/total_input_sets )/(1000000000000)<<"\n";
    jFile << "}\n";
    jFile.close();

}

RTOS::~RTOS()
{/*
#ifdef OS_STATISTICS_ON
    char time_uint[6][10] = {" fs", " ps", " ns", " us", " ms", " s"};
    std::cout << "\n***************************************************************************\n";
    std::cout << "\t wait(time) was called "<< wait_for_time_cnt << "  times.\n";
    std::cout << "---------------------------------------------------------------------------\n";
    std::cout << std::setw(30) << " busy duration " << std::setw(25) << "fallback duration" << std::setw(25) << " # of contex-switches \n";
    for (int c = 0; c < os_core_number; c++) {
        std::cout  << "CORE-" << c << std::setw(20) << busy_duration[c] << " " << time_uint[SIM_RESOLUTION] << std::setw(20) << fallback_duration[c] 
                        << time_uint[SIM_RESOLUTION] << std::setw(20) << task_switch_cnt[c] << "\n";
    }
    std::cout << "***************************************************************************\n";
#endif
*/

//    double latency = sc_core::sc_time_stamp().value();
    //for (int c = 0; c < os_core_number; c++) {
    //    std::cout  << busy_duration[c]<<  "  -	" ;
    //}
    //std::cout << "\n";

    //for (int c = 0; c < os_core_number; c++) {
    //    std::cout  << adjust_duration[c]<<  "  -	" ;
    //}

    //std::cout << "\n";
//    double core_lwip = (double)(busy_duration[0])/(latency);  
//    double core_comp = (double)(busy_duration[1] - adjust_duration[1])/(latency);  
//    latency = (latency/20)/(1000000000000);
 
/*
    for (int c = 0; c < os_core_number; c++) {
        std::cout  << busy_duration[c] - adjust_duration[c]<<  "	" ;
    }
*/
//    std::cout<< core_lwip <<"	"<< core_comp <<  "	"<< latency<<  "	" ;
//    std::cout << "\n";
    //std::cout << "Adjust time is: " <<adjust_duration[0] <<std::endl;
    //std::cout << "Adjust time is: " <<adjust_duration[1] <<std::endl;
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  +++++++++++++++++++++++++ OS Internal Methods ++++++++++++++++++++++++++++++
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*
 * Abort -- report serious error and  stop the system                  
 */
void RTOS::OSAbort(int err)
{
    exit(err);
}
/*
 * Insert 'proc' to the beginning of its priority list.
 */
void RTOS::insertBeginPriority(OSProc proc, OSQueue *que)
{
    unsigned int prt; /* priority of the task to be inserted */
    OSProc p;   /* pointer to the previous TCB*/
    OSProc q;   /* pointer to the next TCB */
  
    p = OS_NO_TASK;
    q = *que;
    prt = os_vdes[proc].priority;


    int temp=*que;
//  std::cout << "Inserting front task is :" << proc <<std::endl;
    while(temp!=OS_NO_TASK){
//	std::cout <<"Task ID in queue is mark 0: " <<temp <<std::endl;
	temp = os_vdes[temp].next;
    } 




    while ((q != OS_NO_TASK) && (prt > os_vdes[q].priority)) {
        p = q;
        q = os_vdes[q].next;
    }
  
    if (p != OS_NO_TASK)
        os_vdes[p].next = proc;
    else
        *que = proc;
  
    if (q != OS_NO_TASK)
        os_vdes[q].prev = proc;
  
    os_vdes[proc].next = q;
    os_vdes[proc].prev = p;

/*
    temp=*que;
    std::cout << "Inserting front task is :" << proc <<std::endl;
    while(temp!=OS_NO_TASK){
	std::cout <<"Task ID in queue is: " <<temp <<std::endl;
	temp = os_vdes[temp].next;
    } 
*/



}
/*
 * Insert 'proc' to the end of its priority list.
 */
void RTOS::insertEndPriority(OSProc proc, OSQueue *que)
{
    unsigned int prt; /* priority of the task to be inserted */
    OSProc p;   /* pointer to the previous TCB */
    OSProc q;   /* pointer to the next TCB */
 
    p = OS_NO_TASK;
    q = *que;
    prt = os_vdes[proc].priority;



    while ((q != OS_NO_TASK) && (prt >= os_vdes[q].priority)) {
        p = q;
        q = os_vdes[q].next;
    }

    if (p != OS_NO_TASK)
        os_vdes[p].next = proc;
    else
        *que = proc;

    if (q != OS_NO_TASK)
        os_vdes[q].prev = proc;

    os_vdes[proc].next = q;
    os_vdes[proc].prev = p;

/*
    int temp=*que;
    std::cout << "Inserting end task is :" << proc <<std::endl;
    while(temp!=OS_NO_TASK){
	std::cout <<"Task ID in queue is: " <<temp <<std::endl;
	temp = os_vdes[temp].next;
    } 
*/


}
/*
 * Insert periodic tasks based on their next release time.
 */
void RTOS::insertEndPeriod(OSProc proc, OSQueue *que)
{
    sc_dt::uint64 rt; /* release time of the task to be inserted */
    OSProc p; /* pointer to the previous TCB */
    OSProc q; /* pointer to the next TCB */
  
    p = OS_NO_TASK;
    q = *que;
    rt = os_vdes[proc].next_release_time;
    
    while ((q != OS_NO_TASK) && (rt >= os_vdes[q].next_release_time)) {
        p = q;
        q = os_vdes[q].next;
    }
    
    if (p != OS_NO_TASK)
        os_vdes[p].next = proc;
    else
        *que = proc;
    
    if (q != OS_NO_TASK)
        os_vdes[q].prev = proc;
    
    os_vdes[proc].next = q;
    os_vdes[proc].prev = p;
}
/*
 * Is 'que' empty?
 */
bool RTOS::empty(OSQueue *que)
{
    if (*que == OS_NO_TASK)
        return(true);
    else
        return(false);
}
/*
 * 
 */
void RTOS::freeTask(OSProc proc, OSQueue *que)
{
    OSProc p; /*pointer to the previous TCB*/
    OSProc q; /* pointer to the next TCB */

    p = OS_NO_TASK;
    q = *que;

    while (q != OS_NO_TASK) {
        p = q;
        q = os_vdes[q].next;
    }
    
    if (p != OS_NO_TASK)
        os_vdes[p].next = proc;
    else
        *que = proc;
    
    if (q != OS_NO_TASK)
        os_vdes[q].prev = proc;
    
    os_vdes[proc].next = q;
    os_vdes[proc].prev = p;
}
/*
 *
 */
void RTOS::extractTask(OSProc proc, OSQueue *que)
{
    OSProc p; /*pointer to the previous TCB*/
    OSProc q; /* pointer to the next TCB */

    p = os_vdes[proc].prev;
    q = os_vdes[proc].next;
    
    if (p == OS_NO_TASK)
        *que=q; /*first element*/
    else
        os_vdes[p].next = os_vdes[proc].next;
    
    if (q != OS_NO_TASK)
        os_vdes[q].prev = os_vdes[proc].prev;
}
/*
 *
 */
OSProc RTOS::getFirstTask(OSQueue *que)
{
    OSProc q; /*pointer to the first element*/

    q = *que;
    if (q == OS_NO_TASK)
        return(OS_NO_TASK);
    
    *que = os_vdes[q].next;
    if ( *que != OS_NO_TASK )
        os_vdes[*que].prev = OS_NO_TASK;



 



    return(q);
}
/*
 *
 */
OSProc RTOS::peekFirstTask(OSQueue *que)
{
    OSProc q; /*pointer to the first element*/

    q = *que;
    return(q);
}
/*
 * Predict next preemption point based on states of periodic tasks.
 */
sc_dt::uint64 RTOS::getPredictedDelay(OSProc proc) 
{
    sc_dt::uint64 predicted_delay;
    OSProc idle_task;
    uint8_t current_core;
    bool flag;

    if (!os_vdes[proc].predictive_mode) {

        current_core = os_vdes[proc].schedcore;
        predicted_delay = os_simulation_quantum;
        flag = false;
        idle_task = os_idle_queue[current_core];
        while ((idle_task != OS_NO_TASK) && (!flag)) {
            /* Lower value = higher priority*/
            /* Idle queue is sorted based on increasing next_releases_time. */
            if (os_vdes[idle_task].priority <= os_vdes[proc].priority) {
	            if (os_vdes[idle_task].next_release_time <= sc_core::sc_time_stamp().value()) 
	                predicted_delay = sc_dt::UINT64_ZERO;
	            else 
	                predicted_delay = os_vdes[idle_task].next_release_time - sc_core::sc_time_stamp().value();
	            flag = true;
            }
            idle_task = os_vdes[idle_task].next;
        }

        if (predicted_delay > os_vdes[proc].ts)
            predicted_delay = os_vdes[proc].ts; 
        if (predicted_delay > os_simulation_quantum)
            predicted_delay = os_simulation_quantum;
        os_vdes[proc].predicted_delay = predicted_delay;
        os_vdes[proc].predictive_mode = true;

    } else
        predicted_delay = os_vdes[proc].predicted_delay;
    return predicted_delay;
} 
/*
 * Check fall-back mode.
 */
bool RTOS::checkFallbackMode(OSProc proc)
{
    OSProc blocking_task;
    OSProc blocked_task;
    uint8_t current_core;
    bool flag;
    
    if (os_vdes[proc].fallback_check) {  
    
        current_core = os_vdes[proc].schedcore;
        flag = false;
        blocked_task = os_wait_queue[current_core];
        while ((blocked_task != OS_NO_TASK) && (!flag)) {
            blocking_task = os_vdes[blocked_task].blocking_task_id;
      
            if (os_vdes[blocked_task].priority <= os_vdes[proc].priority) 	 
	            if ((blocking_task == OS_NO_TASK) ||
	                ((os_vdes[blocking_task].state == OS_INTR_WAIT) && 
	                (os_vdes[blocking_task].priority <= os_vdes[proc].priority)))
	                flag = true;
      
            blocked_task = os_vdes[blocked_task].next;
        } 
        os_vdes[proc].fallback_mode = flag;
        os_vdes[proc].fallback_check = false;
    
    }
    return os_vdes[proc].fallback_mode; 
}
/*
 * Check inter-core interrupt dependency.
 */
bool RTOS::checkIntrDependency(OSProc proc)
{
    OSProc app_task;
    OSProc intr_task;
    uint8_t core_id, current_core, other_core;
    bool fb_flag, dp_flag;
    sc_dt::uint64 adjusted_delay;
 
    if (os_vdes[proc].id_check) {
    
        current_core = os_vdes[proc].schedcore;
        other_core = 0;
        dp_flag = false;
        fb_flag = false;
        adjusted_delay = OS_INFINIT_VAL;
           
        while (other_core < os_core_number) {
            if (other_core != current_core) {
          
                intr_task = os_intrwait_queue[other_core];
                while (intr_task != OS_NO_TASK) {
	                if ((os_vdes[intr_task].launched_core_id == current_core) || 
	                    (os_vdes[intr_task].launched_core_id == OS_NO_CPU)) {
	                    dp_flag = true;
	                    app_task = os_vdes[intr_task].blocked_task_id;
	                    if (app_task != OS_NO_TASK) {
	       
	                        if (os_vdes[app_task].state == OS_IDLE) {
	                        /* Interrupt handlers can be delayed until the next release time. */
	                        adjusted_delay = MIN_VAL(adjusted_delay, (os_vdes[app_task].next_release_time - 
						                                                	sc_core::sc_time_stamp().value()));
	                        } 
	                        else { 
	                            core_id = os_vdes[app_task].schedcore;
	                            if ((os_current[core_id] != OS_NO_TASK) &&
		                           ((os_vdes[os_current[core_id]].priority < os_vdes[app_task].priority) &&
		                            (os_vdes[os_current[core_id]].advance_time > sc_dt::UINT64_ZERO))) { 
		                            adjusted_delay = MIN_VAL(adjusted_delay, (os_vdes[os_current[core_id]].advance_time - 
							                                            sc_core::sc_time_stamp().value()));
	                            } else
		                            fb_flag = true;
	                        }
	      
	                    } else /* app_task is unknown */
	                        fb_flag = true;
	                }
	                intr_task = os_vdes[intr_task].next;
                }
            }
            other_core++;
        }
        if (fb_flag)
            adjusted_delay = sc_dt::UINT64_ZERO;
        os_vdes[proc].adjusted_delay = adjusted_delay;
        os_vdes[proc].id_check = false;
        
        return dp_flag;
        
    }
    else
        return os_vdes[proc].id_flag;
}
/*
 * Block 'proc' until to be scheduled.
 */
void RTOS::wait4Sched(OSProc proc)
{
    //std::cout << "Task" << proc << "wait4Sched"  <<std::endl;
    assert( proc < OS_MAXPROC );
    do {
        os_sched_event_list[proc].receive();
    } while((os_vdes[proc].state != OS_RUN) && (os_vdes[proc].state != OS_CPU));
    os_vdes[proc].state = OS_CPU;
}
/*
 * Send scheduled event.
 */
void RTOS::sendSched(OSProc proc)
{
    assert( proc < OS_MAXPROC );
    os_sched_event_list[proc].send();
}
/*
 * Dispatch.
 */
void RTOS::dispatch(uint8_t core_id)
{
    OSProc proc;
    //std::cout  << "dispatching on core :" << (int)core_id <<"\n";
    /* Update released tasks */
    if (!empty(&os_idle_queue[core_id])) {
        proc = peekFirstTask(&os_idle_queue[core_id]);
        while ((proc != OS_NO_TASK) &&
	                (os_vdes[proc].next_release_time <= sc_core::sc_time_stamp().value())) {
            proc = getFirstTask(&os_idle_queue[core_id]);
            os_vdes[proc].next_release_time += os_vdes[proc].period;
            os_vdes[proc].state = OS_READY;
            if (os_vdes[proc].ts == sc_dt::UINT64_ZERO) {
	            os_vdes[proc].ts = os_vdes[proc].dts;
		    //std::cout<<sc_core::sc_get_current_process_handle().name() <<":READYQUEUE, insertEnd"<<"\n";
	            insertEndPriority(proc, &os_ready_queue[core_id]);

            } 
            else{
		    //std::cout<<sc_core::sc_get_current_process_handle().name() <<":READYQUEUE, insertBegin dispatch"<<"\n";
	            insertBeginPriority(proc, &os_ready_queue[core_id]);
		}
        } //while
    }

    if (!empty(&os_intrhandler_ready_queue[core_id])) {
        /* Dispatch an interrupt handler*/

        proc = peekFirstTask(&os_intrhandler_ready_queue[core_id]);
        os_current[core_id] = proc;
        os_vdes[os_current[core_id]].state = OS_RUN;
        sendSched(os_current[core_id]);
    } 
    else 
        if (!empty(&os_ready_queue[core_id])) {
	        /* Dispatch a normal task*/

	    //std::cout<<sc_core::sc_get_current_process_handle().name() <<"READYQUEUE, getFirst"<<"\n";


//***********************************************************
//***********************************************************
//    int temp= os_ready_queue[core_id];
//    while(temp!=OS_NO_TASK){
//	std::cout <<"Task ID in queue before getting first is: " <<temp <<std::endl;
//	temp = os_vdes[temp].next;
//    }
//***********************************************************
//***********************************************************


            proc = getFirstTask(&os_ready_queue[core_id]);


//***********************************************************
//***********************************************************
//    while(temp!=OS_NO_TASK){
//	std::cout <<"Task ID in queue after getting first is: " <<temp <<std::endl;
//	temp = os_vdes[temp].next;
//    }
//***********************************************************
//***********************************************************


            os_current[core_id] = proc;
            os_vdes[proc].state = OS_RUN;
            os_vdes[proc].schedcore = core_id;
            os_vdes[proc].fallback_check = true;
            os_vdes[proc].predictive_mode= false;
            os_vdes[proc].id_check = false;
            sendSched(os_current[core_id]);
        } 
        else
            os_current[core_id] = OS_NILL;

#ifdef OS_STATISTICS_ON
            if (last_sched_task[core_id] != os_current[core_id]) {

		if(NodeID==0)//print out the client context switches
		{
		  //std::cout <<  "Context switches happen at time: " << sc_core::sc_time_stamp().value() <<std::endl;
	   	  //std::cout << "Server: " << last_sched_task[core_id] << "->" << os_current[core_id] << " : " << sc_core::sc_time_stamp().value() << std::endl;
	   	  //std::cout << "Core: "<< int(core_id) << ": "<< os_current[core_id] << ":" << sc_core::sc_time_stamp().value() << std::endl;
	   	  //std::cout << os_current[core_id] << std::endl;
		}

		if(NodeID==2 && core_id==0)//print out the client context switches
		{
		  //std::cout <<  "Context switches happen at time: " << sc_core::sc_time_stamp().value() <<std::endl;
	   	  //std::cout << "Client: " << last_sched_task[core_id] << "->" << os_current[core_id] << std::endl;
		}

                task_switch_cnt[core_id]++;

                if (last_sched_task[core_id] == OS_NO_TASK)  
                    os_busy_time[core_id] = sc_core::sc_time_stamp().value();
                if (os_current[core_id] == OS_NO_TASK)
                    busy_duration[core_id] += (sc_core::sc_time_stamp().value() - os_busy_time[core_id]);    
            }    
            last_sched_task[core_id] = os_current[core_id];
#endif
}
/*
 * OS scheduler.
 */
void RTOS::schedule(uint8_t core_id)
{    
    OSProc curr_task;
 
    curr_task = os_current[core_id];
    if ((curr_task != OS_NO_TASK) && (os_vdes[curr_task].type != OS_INTR_HANDLER))  {
        os_vdes[os_current[core_id]].state = OS_READY;
        if (os_vdes[curr_task].ts <= (sc_core::sc_time_stamp().value() - os_vdes[curr_task].start_time)) {
            os_vdes[curr_task].ts = os_vdes[curr_task].dts;

	    //std::cout<<sc_core::sc_get_current_process_handle().name() <<"READYQUEUE, insertEnd"<<"\n";
            insertEndPriority(curr_task, &os_ready_queue[core_id]);
        }
        else {
            os_vdes[curr_task].ts = os_vdes[curr_task].ts - 
	                                               (sc_core::sc_time_stamp().value() - os_vdes[curr_task].start_time);

	    //std::cout<<sc_core::sc_get_current_process_handle().name() <<"READYQUEUE, insertBegin schedule"<<"\n";
            insertBeginPriority(curr_task, &os_ready_queue[core_id]);
        }
    }
    dispatch(core_id);
}
/*
 *
 */
void RTOS::consumeAccumulatedDelay(OSProc proc)
{
    //std::cout<<"consumeAcc: "<<proc<<std::endl;
    uint8_t core_id;
    sc_dt::uint64 predicted_delay, consumed_delay;
    sc_dt::uint64 start_time, end_time;
    bool fallback, intr_dependency;

    core_id = os_vdes[proc].schedcore;
    while (os_vdes[proc].accumulated_delay > sc_dt::UINT64_ZERO) {
#ifdef OS_STATISTICS_ON
		wait_for_time_cnt++;
#endif	   
        predicted_delay = getPredictedDelay(proc);
        fallback = checkFallbackMode(proc);
        if (!fallback) {
            intr_dependency = checkIntrDependency(proc);
                if (intr_dependency && (os_vdes[proc].adjusted_delay > 0))
                    predicted_delay = MIN_VAL(predicted_delay, os_vdes[proc].adjusted_delay);
        }
 
        if (predicted_delay > os_vdes[proc].accumulated_delay)
            predicted_delay = os_vdes[proc].accumulated_delay;
 
        if (fallback || (intr_dependency && (os_vdes[proc].adjusted_delay == 0))) { /* fallback mode*/
            os_vdes[proc].advance_time = 0;
            start_time = sc_core::sc_time_stamp().value();
            sc_core::wait(predicted_delay, SIM_RESOLUTION, os_intrhandler_event_list[core_id]);
            end_time = sc_core::sc_time_stamp().value();
            consumed_delay = end_time - start_time;
#ifdef OS_STATISTICS_ON
           fallback_duration[core_id] += consumed_delay;
#endif            
        } 
        else { /* Predictive mode */
            os_vdes[proc].advance_time = sc_core::sc_time_stamp().value() + predicted_delay;
            sc_core::wait(predicted_delay, SIM_RESOLUTION);
            consumed_delay = predicted_delay;
        }
    
        os_vdes[proc].accumulated_delay -= consumed_delay;
        schedule(core_id);
        wait4Sched(proc);
    }
}
/*
 * Remove a process from the OS kernel.
 */
void RTOS::endProcess(OSProc proc)
{
    int core_id;
  
    consumeAccumulatedDelay(proc);
    core_id = os_vdes[proc].schedcore;
    assert (os_current[core_id] == proc);
    if (os_vdes[proc].type == OS_RT_PERIODIC) {
        os_vdes[proc].state = OS_ZOMBIE;
        insertEndPriority(proc, &os_zombie_queue);
    }
    else {
        os_vdes[proc].state = OS_FREE;
        freeTask(proc, &os_freetcb_queue);
    }
    os_current[core_id] = OS_NO_TASK;
    dispatch(core_id); 
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  ++++++++++++++++++++++++++++ OS API METHODS +++++++++++++++++++++++++++++
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/******************************************************************************
 * OS intialization and startup
 ******************************************************************************/
/*
 * Set the number of cores and simulation quantum.
 */
void RTOS::init(uint8_t core_number, sc_dt::uint64 simulation_quantum)
{
    /* Setup checking...*/
    if (core_number > OS_MAXCORE)
        std::cerr << "Warning in OS init(): MAX_CORE violation ("<< OS_MAXCORE << ")! \n";
    if (simulation_quantum == 0) {
        std::cerr << "Error in OS init(): Need a positive value for \"simulation quantum\".  \n";    
        exit(0);
    }

    /* initialize the list of free TCBs and semaphores */
    for (OSProc p = 0; p < OS_MAXPROC; p++) {
        os_vdes[p].type = OS_UNDEFINED;
        os_vdes[p].next = p+1;
    }
    os_vdes[OS_MAXPROC-1].type = OS_UNDEFINED;
    os_vdes[OS_MAXPROC-1].next = OS_NO_TASK;

    /* initialize the OS queues */
    for (int i = 0; i < OS_MAXCORE; i++) {
        os_ready_queue[i] = OS_NILL;
        os_idle_queue[i] = OS_NILL;
        os_sleep_queue[i] = OS_NILL;
        os_wait_queue[i] = OS_NILL;
        os_intrwait_queue[i] = OS_NILL;
        os_intrhandler_ready_queue[i] = OS_NILL;
    }
    os_zombie_queue = OS_NILL;
    os_freetcb_queue = 0;

    /* initialize current tasks */
    for (int i = 0; i < OS_MAXCORE; i++){
        os_current[i] = OS_NO_TASK;
    }
    os_core_number = core_number;
    os_simulation_quantum = simulation_quantum;
#ifdef OS_STATISTICS_ON 
    AppDuration=0;
    lwIPDuration=0;     
    wait_for_time_cnt = 0;
    for (int cpu = 0; cpu < OS_MAXCORE; cpu++) {
        context_cnt[cpu] = 0;
        task_switch_cnt[cpu] = 0;
        last_sched_task[cpu] = OS_NO_TASK;
        fallback_duration[cpu] = 0;
        busy_duration[cpu] = 0;
        inc_busy_duration[cpu] = 0;
        adjust_duration[cpu] = 0;
        os_busy_time[cpu] = 0;
    }
#endif    
}
/*
 * Start OS.
 */
void RTOS::start(void)
{
    //std::cout<<"OS is started ... ... ... ... ..."<<std::endl;
    for (int i = 0; i < os_core_number; i++)
        dispatch(i);
}

void RTOS::dynamicStart(uint8_t coreid)
{
      if (os_current[coreid] == OS_NO_TASK)
            dispatch(coreid);    
}

/******************************************************************************
 * Task management
 ******************************************************************************/
/*
 * Create a normal task.
 */
OSProc RTOS::taskCreate( const char *name, /* task name */
			 OSTaskType type, /* task type */
			 unsigned int priority, /* task static priority */
			 sc_dt::uint64 period, /* period or priority */
			 sc_dt::uint64 wcet, /* worst case exec. time */
			 sc_dt::uint64 dts, /* default time slice value */
			 sc_dt::uint64 affinity, /* target cores */
			 uint8_t coreid /* starting core */
			 )
{
    OSProc p;

    p = getFirstTask(&os_freetcb_queue);
    //std::cout << "Creating task ID: " << p <<std::endl;
    if (p == OS_NO_TASK)
        OSAbort(NO_TCB);
  
    os_vdes[p].type  = type;
    strcpy(os_vdes[p].name,name);
    os_vdes[p].priority = priority;
  
    if (type == OS_RT_PERIODIC) {
        assert( period != sc_dt::UINT64_ZERO );
        os_vdes[p].period = period ;
        os_vdes[p].wcet = wcet ;
        os_vdes[p].weight = (float)wcet/period;
        os_vdes[p].next_release_time = sc_core::sc_time_stamp().value() + period;
        os_vdes[p].start_cycle = sc_core::sc_time_stamp().value();
    }
    assert( dts != sc_dt::UINT64_ZERO );
    os_vdes[p].ts = dts;
    os_vdes[p].dts = dts;
    os_vdes[p].affinity = affinity;
    os_vdes[p].schedcore = coreid;
    os_vdes[p].blocked_task_id = OS_NO_TASK;
    os_vdes[p].blocking_task_id = OS_NO_TASK;
    os_vdes[p].predictive_mode = false;
    os_vdes[p].fallback_mode = false;
    os_vdes[p].fallback_check = true;
    os_vdes[p].id_flag = false;
    os_vdes[p].id_check = true;
  
    os_vdes[p].accumulated_delay = sc_dt::UINT64_ZERO;
    os_vdes[p].predicted_delay = sc_dt::UINT64_ZERO;
    os_vdes[p].adjusted_delay = sc_dt::UINT64_ZERO;
    os_vdes[p].advance_time = sc_dt::UINT64_ZERO;
    os_vdes[p].response_time = sc_dt::UINT64_ZERO;
  
    if (type == OS_INTR_TASK) {
        os_vdes[p].state = OS_INTR_WAIT;
        os_vdes[p].launched_core_id = OS_NO_CPU;
        insertEndPriority(p, &os_intrwait_queue[coreid]);
    } else {
        os_vdes[p].state = OS_READY;

	//std::cout<<sc_core::sc_get_current_process_handle().name() <<"READYQUEUE, insertEnd"<<"\n";
        insertEndPriority(p, &os_ready_queue[coreid]);//CAn we dynamically insert tasks into ready queue
    }
  
    return(p);
}
/*
 * Start task execution under OS control. 
 */
void RTOS::taskActivate(OSProc proc)
{
    //std::cout<<"taskAc: "<<proc<<std::endl;
    wait4Sched(proc);
}
/*
 * End task execution under OS control.
 */
void RTOS::taskTerminate(OSProc proc)
{
    endProcess(proc);
}
/*
 *
 */
void RTOS::taskKill(OSProc proc)
{
    if(os_vdes[proc].state == OS_READY){

	//std::cout<<sc_core::sc_get_current_process_handle().name() <<"READYQUEUE, extractTask"<<"\n";
        extractTask(proc,&os_ready_queue[os_vdes[proc].schedcore]);

    }
    if(os_vdes[proc].state == OS_IDLE)
        extractTask(proc,&os_idle_queue[os_vdes[proc].schedcore]);

    if(os_vdes[proc].type == OS_RT_PERIODIC)
        insertEndPriority(proc, &os_zombie_queue);
    else {
        os_vdes[proc].state = OS_FREE ;
        insertEndPriority(proc, &os_freetcb_queue);
    }
}
/*
 * immidiately move the periodic task 'proc' from RUN to IDLE state.
 */
void RTOS::taskEndCycle(OSProc proc)
{
    sc_dt::uint64 wait_delay, current_time;
    uint8_t core_id;
    
    /* Synchronize global time */
    consumeAccumulatedDelay(proc);
 
    core_id = os_vdes[proc].schedcore;
    current_time = sc_core::sc_time_stamp().value();
    os_vdes[proc].response_time = current_time - os_vdes[proc].start_cycle;

    if (os_vdes[proc].ts < (current_time - os_vdes[proc].start_time))
        os_vdes[proc].ts = sc_dt::UINT64_ZERO;
    else
        os_vdes[proc].ts -= (current_time - os_vdes[proc].start_time);
    /* Skip missed deadline */
    if (current_time >= os_vdes[proc].next_release_time) 
        while (current_time >= os_vdes[proc].next_release_time) 
            os_vdes[proc].next_release_time += os_vdes[proc].period; 
 
    os_vdes[proc].state = OS_IDLE;
    insertEndPeriod(proc, &os_idle_queue[core_id]);
    dispatch(core_id);
  
    /* calculate next wake up time */
    wait_delay = os_vdes[proc].next_release_time - current_time;
    sc_core::wait(wait_delay, SIM_RESOLUTION);
    os_vdes[proc].start_cycle = sc_core::sc_time_stamp().value();
    if( os_current[core_id] == OS_NO_TASK )
        dispatch(core_id);
    //std::cout<<"taskEnd: "<<proc<<std::endl;
    wait4Sched(proc);
}
/*
 *
 */
void RTOS::taskSleep(OSProc proc)
{
    uint8_t core_id;
  
    consumeAccumulatedDelay(proc);
 
    core_id = os_vdes[proc].schedcore;
    if (os_vdes[proc].ts <= (sc_core::sc_time_stamp().value() - os_vdes[proc].start_time))
        os_vdes[proc].ts = sc_dt::UINT64_ZERO;
    else
        os_vdes[proc].ts -= (sc_core::sc_time_stamp().value() - os_vdes[proc].start_time);

    os_vdes[proc].state = OS_SLEEP;
    insertEndPriority(proc, &os_sleep_queue[core_id]);
    dispatch(core_id);
    //std::cout<<"taskSleep: "<<proc<<std::endl;
    wait4Sched(proc);
}
/*
 *
 */
void RTOS::taskResume(OSProc proc)
{
    uint8_t core_id;

	consumeAccumulatedDelay(proc);
    core_id = os_vdes[proc].schedcore;
    if (os_vdes[proc].state == OS_SLEEP) {
        os_vdes[proc].state = OS_READY;
        extractTask(proc, &os_sleep_queue[core_id]);
          
	//std::cout<<sc_core::sc_get_current_process_handle().name() <<"READYQUEUE, insertEnd"<<"\n";
        insertEndPriority(proc, &os_ready_queue[core_id]);
    }
  
    if (os_current[core_id] == OS_NO_TASK)
        dispatch(core_id);
}

/******************************************************************************
 * Delay modeling & Event handling
 ******************************************************************************/  
/*
 * Sunchronize global time: consume local delay.
 */
void RTOS::syncGlobalTime(OSProc proc)
{
    consumeAccumulatedDelay(proc);
}
/*
 *  
 */
void RTOS::timeWait(sc_dt::uint64 sec, OSProc proc)
{
    //if(NodeID==0){
	//std::cout<<busy_duration[0]<<"\n";
    //}
    uint8_t core_id;
    sc_dt::uint64 predicted_delay, consumed_delay, start_time, end_time;
    bool fallback, intr_dependency;

    intr_dependency = 0;
    os_vdes[proc].accumulated_delay += sec;
    core_id = os_vdes[proc].schedcore;
    predicted_delay = getPredictedDelay(proc);
    fallback = checkFallbackMode(proc);
	
    if (!fallback) {
        intr_dependency = checkIntrDependency(proc); 
        if (intr_dependency && (os_vdes[proc].adjusted_delay > 0))
            predicted_delay = MIN_VAL(predicted_delay, os_vdes[proc].adjusted_delay);
    }
       
    while (((os_vdes[proc].accumulated_delay >= predicted_delay) || fallback || 
	        (intr_dependency && (os_vdes[proc].adjusted_delay == 0))) && 
	        (os_vdes[proc].accumulated_delay != 0)) {

#ifdef OS_STATISTICS_ON
		wait_for_time_cnt++;
#endif	  
   
        if (fallback || (intr_dependency && (os_vdes[proc].adjusted_delay == 0))) { // fallback mode

            os_vdes[proc].advance_time = 0;
            if (predicted_delay > os_vdes[proc].accumulated_delay)
	            predicted_delay = os_vdes[proc].accumulated_delay;
	        start_time = sc_core::sc_time_stamp().value();
            sc_core::wait(predicted_delay, SIM_RESOLUTION, os_intrhandler_event_list[core_id]);
            end_time = sc_core::sc_time_stamp().value();
            consumed_delay = end_time - start_time; 
    #ifdef OS_STATISTICS_ON
           fallback_duration[core_id] += consumed_delay;
#endif
        }
        else {
            os_vdes[proc].advance_time = sc_core::sc_time_stamp().value() + predicted_delay;
            sc_core::wait(predicted_delay, SIM_RESOLUTION);
            consumed_delay = predicted_delay;
        } 

        os_vdes[proc].accumulated_delay -= consumed_delay;
        schedule(core_id);
        //std::cout<<"timeWait: "<<proc<<std::endl;
        wait4Sched(proc);

        predicted_delay = getPredictedDelay(proc);
        fallback = checkFallbackMode(proc);
       if (!fallback) {
            intr_dependency = checkIntrDependency(proc);
            if (intr_dependency && (os_vdes[proc].adjusted_delay > 0))
	            predicted_delay = MIN_VAL(predicted_delay, os_vdes[proc].adjusted_delay);
        }
    }
}
/*
 *
 */
void RTOS::preWait(OSProc proc, OSProc blocking_tID)
{
    //std::cout << proc << "Blocking task in pre_wait ... ..." << blocking_tID <<std::endl;
    uint8_t core_id;
    
    assert( os_vdes[proc].type != OS_INTR_HANDLER );
  
    consumeAccumulatedDelay(proc);
    core_id = os_vdes[proc].schedcore;
    os_vdes[proc].state = OS_WAIT;
    os_vdes[proc].blocking_task_id = blocking_tID;
    insertEndPriority(proc, &os_wait_queue[core_id]);
    os_vdes[proc].advance_time = 0;

    if (os_vdes[proc].ts < (sc_core::sc_time_stamp().value() - os_vdes[proc].start_time))
        os_vdes[proc].ts = sc_dt::UINT64_ZERO;
    else
        os_vdes[proc].ts -= (sc_core::sc_time_stamp().value() - os_vdes[proc].start_time);
  
    os_current[core_id] = OS_NO_TASK;
    dispatch(core_id);
}
/*
 *
 */
void RTOS::postWait(OSProc proc)
{
    uint8_t core_id;
    
    assert( os_vdes[proc].type != OS_INTR_HANDLER );
    core_id = os_vdes[proc].schedcore;
    if (os_vdes[proc].state == OS_WAIT) {
   
        os_vdes[proc].state = OS_READY;
        extractTask(proc, &os_wait_queue[core_id]);
        if (os_vdes[proc].ts == sc_dt::UINT64_ZERO) {
	    //std::cout<<sc_core::sc_get_current_process_handle().name() <<"READYQUEUE, insertEnd"<<"\n";
            insertEndPriority(proc, &os_ready_queue[core_id]);
            os_vdes[proc].ts = os_vdes[proc].dts;
        }
        else{

	    //std::cout<<sc_core::sc_get_current_process_handle().name() <<"READYQUEUE, insertBegin, postWait"<<"\n";
            insertBeginPriority(proc, &os_ready_queue[core_id]);

   	}
        if (os_current[core_id] == OS_NO_TASK)
            dispatch(core_id);    
    } 
 
    //std::cout<<"postWait: "<<proc<<std::endl;
    wait4Sched(proc);
}

void RTOS::postWaitWithSWIntr(OSProc proc)
{
    uint8_t core_id;
    
    assert( os_vdes[proc].type != OS_INTR_HANDLER );
    core_id = os_vdes[proc].schedcore;
    if (os_vdes[proc].state == OS_WAIT) {
   
        os_vdes[proc].state = OS_READY;
        extractTask(proc, &os_wait_queue[core_id]);
        if (os_vdes[proc].ts == sc_dt::UINT64_ZERO) {
	    //std::cout<<sc_core::sc_get_current_process_handle().name() <<"READYQUEUE, insertEnd"<<"\n";
            insertEndPriority(proc, &os_ready_queue[core_id]);
            os_vdes[proc].ts = os_vdes[proc].dts;
        }
        else{

	    //std::cout<<sc_core::sc_get_current_process_handle().name() <<"READYQUEUE, insertBegin, postWait"<<"\n";
            insertBeginPriority(proc, &os_ready_queue[core_id]);

   	}
        if (os_current[core_id] == OS_NO_TASK)
            dispatch(core_id);    
        else{
            os_intrhandler_event_list[core_id].notify(); 
        }
    } 
    wait4Sched(proc);
}

/*
 *
 */
void RTOS::preNotify(OSProc proc, OSProc blocked_tID)
{
    //std::cout<<"In preNotify ... ... : "<<proc<<" blocked id: " << blocked_tID <<std::endl; 
    os_vdes[proc].blocked_task_id = blocked_tID;
    consumeAccumulatedDelay(proc);
}
/*
 *
 */
void RTOS::postNotify(OSProc proc, OSProc blocked_tID)
{
    uint8_t core_id, blocked_core_id;
    if (blocked_tID != OS_NO_TASK) {
        core_id = os_vdes[proc].schedcore;
        blocked_core_id = os_vdes[blocked_tID].schedcore;
        os_vdes[blocked_tID].state = OS_READY;
        extractTask(blocked_tID, &os_wait_queue[blocked_core_id]);
        if (os_vdes[blocked_tID].ts == sc_dt::UINT64_ZERO) {   
            os_vdes[blocked_tID].ts = os_vdes[blocked_tID].dts;

	    //std::cout<<sc_core::sc_get_current_process_handle().name() <<"READYQUEUE, insertEnd"<<"\n";
            insertEndPriority(blocked_tID, &os_ready_queue[blocked_core_id]);
        }
        else{

	    //std::cout<<sc_core::sc_get_current_process_handle().name() <<"READYQUEUE, insertBegin postNotify"<<"\n";
            insertBeginPriority(blocked_tID, &os_ready_queue[blocked_core_id]);
        }
        if ((core_id != blocked_core_id) && (os_current[blocked_core_id] == OS_NO_TASK))
       	    dispatch(blocked_core_id);
       	//std::cout << "In postNotify schedule .. " << std::endl;    
        schedule(core_id);
        //std::cout<<"postNotify: "<<proc<<std::endl;
        wait4Sched(proc);
    }
}

/******************************************************************************
 * Interrupt handling
 ******************************************************************************/  
/*
 *
 */
OSProc RTOS::createIntrTask(const char *name, unsigned int priority,
							sc_dt::uint64 affinity, uint8_t init_core, uint8_t init_launched_core)
{
    OSProc p;

    p = getFirstTask(&os_freetcb_queue);

    //std::cout << "createIntrTask task ID" << p <<std::endl;
    if (p == OS_NO_TASK)
	    OSAbort(NO_TCB);                                                                                            

    os_vdes[p].type  = OS_INTR_TASK;
    strcpy(os_vdes[p].name,name);
    os_vdes[p].priority = priority;
    os_vdes[p].ts = OS_INFINIT_VAL;
    os_vdes[p].dts = OS_INFINIT_VAL;
    os_vdes[p].affinity = affinity;
    os_vdes[p].schedcore = init_core;
    os_vdes[p].blocked_task_id = OS_NO_TASK;
    os_vdes[p].blocking_task_id = OS_NO_TASK;
    os_vdes[p].predictive_mode = false;
    os_vdes[p].fallback_mode = false;
    os_vdes[p].fallback_check = true;

    os_vdes[p].accumulated_delay = sc_dt::UINT64_ZERO;
    os_vdes[p].predicted_delay = sc_dt::UINT64_ZERO;
    os_vdes[p].adjusted_delay = sc_dt::UINT64_ZERO;
    os_vdes[p].advance_time = sc_dt::UINT64_ZERO;
    os_vdes[p].response_time = sc_dt::UINT64_ZERO;

    os_vdes[p].state = OS_INTR_WAIT;
    os_vdes[p].launched_core_id = init_launched_core;
                                                                  
    insertEndPriority(p, &os_intrwait_queue[init_core]);
 
    return (p);
}
/*
 *
 */
void RTOS::intrTrigger(OSProc intrID, uint8_t launchedCore)
{
    uint8_t core_id;

    os_vdes[intrID].launched_core_id = launchedCore;
    
    for (int i = 0; i < os_core_number; i++)
        if(os_current[i] != OS_NO_TASK)
            os_vdes[os_current[i]].fallback_check = true;

    core_id = os_vdes[intrID].schedcore;
    if (os_vdes[intrID].state == OS_INTR_WAIT) {
        os_vdes[intrID].state = OS_READY;
        extractTask(intrID, &os_intrwait_queue[core_id]);
          
	//std::cout<<sc_core::sc_get_current_process_handle().name() <<"READYQUEUE, insertBegin intrTrigger"<<"\n";
        insertBeginPriority(intrID, &os_ready_queue[core_id]);
    }
  
    if (os_current[core_id] == OS_NO_TASK)
        dispatch(core_id);
    
}
/*
 *
 */
void RTOS::intrSleep(OSProc intr)
{
    uint8_t core_id;
  
    //std::cout<<"intrSleep: "<<intr<<std::endl;
    consumeAccumulatedDelay(intr);
    
    core_id = os_vdes[intr].schedcore;
    if (os_vdes[intr].ts <= (sc_core::sc_time_stamp().value() - os_vdes[intr].start_time))
        os_vdes[intr].ts = sc_dt::UINT64_ZERO;
    else
        os_vdes[intr].ts -= (sc_core::sc_time_stamp().value() - os_vdes[intr].start_time);
    os_vdes[intr].state = OS_INTR_WAIT;
    insertEndPriority(intr, &os_intrwait_queue[core_id]);
    dispatch(core_id);
    wait4Sched(intr);
}
/*
 *
 */
OSProc RTOS::createIntrHandler(uint8_t coreID, unsigned int priority)
{
    OSProc p;

     p = getFirstTask(&os_freetcb_queue);
    //std::cout << "createIntrHandler task ID" << p <<std::endl;
    if (p == OS_NO_TASK)
        OSAbort(NO_TCB);
    strcat(os_vdes[p].name,"Intr_Handler");
    os_vdes[p].state = OS_READY;
    os_vdes[p].type = OS_INTR_HANDLER;
    os_vdes[p].priority = priority;
    os_vdes[p].schedcore = coreID;
    os_vdes[p].affinity = (1 << coreID);
    return p;
}
/*
 *
 */
void RTOS::iEnter(uint8_t coreID, OSProc handlerID)
{
    insertBeginPriority(handlerID, &os_intrhandler_ready_queue[coreID]); 
    os_intrhandler_event_list[coreID].notify(); 
    if( os_current[coreID] == OS_NO_TASK )
	    dispatch(coreID);    
}
/*
 *
 */
void RTOS::iReturn(uint8_t coreID)
{
    OSProc hID;
    
    hID = os_current[coreID];
    extractTask(hID, &os_intrhandler_ready_queue[coreID]);
    os_current[coreID] = OS_NO_TASK;
    dispatch(coreID);
}

/******************************************************************************
 * Debug & monitor functions
 ******************************************************************************/  
/*
 *
 */
sc_dt::uint64 RTOS::getResponseTime(OSProc proc)
{
    return os_vdes[proc].response_time;
}
/*
 *
 */  
sc_dt::uint64 RTOS::getStartCycle(OSProc proc)
{
    return os_vdes[proc].start_cycle;
}
/*
 *
 */  
void RTOS::resetStartCycle(OSProc proc)
{
    os_vdes[proc].start_cycle = sc_core::sc_time_stamp().value();
    os_vdes[proc].next_release_time = os_vdes[proc].start_cycle + os_vdes[proc].period;
}

#ifdef OS_STATISTICS_ON
/*
 *
 */  
unsigned long RTOS::getTaskContextSwitches(uint8_t coreID)
{
    assert (coreID < OS_MAXCORE);
    return  task_switch_cnt[coreID];
}
/*
 *
 */  
unsigned long RTOS::getOSContextCalled(uint8_t coreID)
{
    assert (coreID < OS_MAXCORE);
    return  context_cnt[coreID];
}    
/*
 *
 */  
sc_dt::uint64 RTOS::getBusyTime(uint8_t coreID)
{
     assert (coreID < OS_MAXCORE);
     return busy_duration[coreID];
}
/*
 *
 */  
sc_dt::uint64 RTOS::getFallbackTime(uint8_t coreID)
{
    assert (coreID < OS_MAXCORE);
    return fallback_duration[coreID];
}
/*
 * Code for debugging purpose
 */
void RTOS::debugPrint(){
  std::cout<< "Invoking RTOS ... " <<std::endl;
}


#endif
/*---------- EOF ----------*/
