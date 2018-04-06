/*********************************************
 * Parisa Razaghi, UT Austin
 * Last update: July 2013 
 ********************************************/

#include "Channels/handshake_os_ch.h"
#include "OS/global_os.h"

using namespace HCSim;

handshake_os_ch::handshake_os_ch()
    :sc_core::sc_channel(sc_core::sc_gen_unique_name("handshake_os_ch"))
    ,forward_flag(false)
    ,wait_flag(false)
    ,blocked_task_id(OS_NO_TASK)
    ,blocking_task_id(OS_NO_TASK)
    ,clock_period(CLOCK_PERIOD)
{
}

handshake_os_ch::handshake_os_ch(const sc_core::sc_module_name name, sc_dt::uint64 clock_period)
    :sc_core::sc_channel(name)
    ,forward_flag(false)
    ,wait_flag(false)
    ,blocked_task_id(OS_NO_TASK)
    ,blocking_task_id(OS_NO_TASK)
{
    this->clock_period = clock_period;
}

handshake_os_ch::~handshake_os_ch()
{
}

void handshake_os_ch::send(int tID)
{
    
    blocking_task_id = tID;
#ifdef TIMED_HANDSHAKE_CHANNEL
    sc_dt::uint64 delay;
    delay = (sc_dt::uint64) HSHK_CH_SEND_INSTR * clock_period;
	os_port->timeWait(delay, tID);
#endif
     os_port->syncGlobalTime(tID);
    if (wait_flag) {
        os_port->preNotify(tID, blocked_task_id);
        event.notify();
        os_port->postNotify(tID, blocked_task_id);
    }
    forward_flag = true;
}

void handshake_os_ch::receive(int tID)
{
   
    blocked_task_id = tID;
#ifdef TIMED_HANDSHAKE_CHANNEL
    sc_dt::uint64 delay;
    delay = (sc_dt::uint64) HSHK_CH_RECEIVE_INSTR * clock_period;
	os_port->timeWait(delay, tID);
#endif
    os_port->syncGlobalTime(tID);
    if (!forward_flag) { 
        wait_flag = true; 
        os_port->preWait(tID, blocking_task_id);
        sc_core::wait(event);
        os_port->postWait(tID);
        wait_flag = false; 
    } 
    forward_flag = false;
#ifdef TIMED_HANDSHAKE_CHANNEL    
    delay = (sc_dt::uint64) HSHK_CH_RECEIVE_INSTR_2 * clock_period;
    os_port->timeWait(delay, tID);
    os_port->syncGlobalTime(tID);
#endif    
}

/*------- EOF -------*/
