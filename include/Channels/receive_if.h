/*********************************************
 * Parisa Razaghi, UT Austin
 * Last update: Jan. 2012 
 ********************************************/
#include <systemc>

#ifndef SC_RECEIVE_IF__H
#define SC_RECEIVE_IF__H

namespace HCSim {

class receive_if
    :virtual public sc_core::sc_interface
{
 public:
    virtual void receive(void) = 0;
};

} //HCSim

#endif //SC_RECEIVE_IF__H
