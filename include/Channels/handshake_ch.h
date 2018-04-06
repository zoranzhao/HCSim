/*********************************************
 * Handshake channel: translated for its specc version
 * Parisa Razaghi, UT Austin <parisa.r@utexas.edu>
 * Last update: Jan. 2012 
 ********************************************/
#include <systemc>

#include "Channels/send_if.h"
#include "Channels/receive_if.h"

#ifndef SC_HANDSHAKE_CH__H
#define SC_HANDSHAKE_CH__H

namespace HCSim {

class handshake_ch
    :public sc_core::sc_channel
    ,virtual public send_if
    ,virtual public receive_if
{
 public:
	
    handshake_ch(const sc_core::sc_module_name name);
	handshake_ch();
    ~handshake_ch();

    void send(void);
    void receive(void);

 private:
    sc_core::sc_event event;
    bool forward_flag, wait_flag;
};

} //HCSim

#endif //SC_C_HANDSHAKE__H
