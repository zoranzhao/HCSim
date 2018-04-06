/*********************************************
 * Parisa Razaghi, UT Austin
 * Last update: July 2013 
 ********************************************/
#include <systemc>

#ifndef SC_SEND_OS_IF__H
#define SC_SEND_OS_IF__H

namespace HCSim {

class send_os_if
    :virtual public sc_core::sc_interface
{
 public:
    virtual void send(int tID) = 0;
};

} //HCSim

#endif //SC_SEND_OS_IF__H
