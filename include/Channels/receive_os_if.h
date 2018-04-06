/*********************************************
 * Parisa Razaghi, UT Austin
 * Last update: July 2013 
 ********************************************/
#include <systemc>

#ifndef SC_RECEIVE_OS_IF__H
#define SC_RECEIVE_OS_IF__H

namespace HCSim {

class receive_os_if
    :virtual public sc_core::sc_interface
{
 public:
    virtual void receive(int tID) = 0;
};

} //HCSim

#endif //SC_RECEIVE_OS_IF__H
