/*********************************************                       
 * Generic Interrupt Controller
 * Parisa Razaghi, UT Austin <parisa.r@utexas.edu> 
 * Last update: Jun. 2013                                            
 ********************************************/
#include <systemc>

#ifndef SC_INTERRUPT_INTERFACE__H
#define SC_INTERRUPT_INTERFACE__H

namespace HCSim {

class IntrTrigger_if
    :virtual public sc_core::sc_interface
{
 public:
    virtual void start(int coreID) = 0;
};

} // namespace HCSim

#endif // SC_INTERRUPT_INTERFACE__H
