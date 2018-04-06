/*********************************************
 * Parisa Razaghi, UT Austin
 * Last update: Jan. 2012 
 ********************************************/
#include <systemc>

#ifndef SC_SEND_IF__H
#define SC_SEND_IF__H

namespace HCSim {

class send_if
    :virtual public sc_core::sc_interface
{
 public:
    virtual void send(void) = 0;
};

} //HCSim

#endif //SC_SEND_IF__H
