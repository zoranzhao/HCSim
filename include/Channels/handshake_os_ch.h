/*********************************************
 * OS-controlled handshake channel: translated for its specc version
 * Parisa Razaghi, UT Austin <parisa.r@utexas.edu>
 * Last update: July 2013 
 ********************************************/
#include <systemc>

#include "sim_config.h"
#include "OS/OSAPI.h"
#include "Channels/send_os_if.h"
#include "Channels/receive_os_if.h"

#ifndef SC_HANDSHAKE_OS_CH__H
#define SC_HANDSHAKE_OS_CH__H

namespace HCSim {

class handshake_os_ch
    :public sc_core::sc_channel
    ,virtual public send_os_if
    ,virtual public receive_os_if
{
 public:
    sc_core::sc_port< OSAPI > os_port;
	
	handshake_os_ch();
    handshake_os_ch(const sc_core::sc_module_name name, sc_dt::uint64 clock_period = CLOCK_PERIOD);
    ~handshake_os_ch();

    void send(int tID);
    void receive(int tID);

 private:
    sc_core::sc_event event;
    bool forward_flag, wait_flag;
    OSProc blocked_task_id;
    OSProc blocking_task_id;
    sc_dt::uint64 clock_period;
    
};

} //HCSim

#endif //SC_HANDSHAKE_OS_CH__H
