/*********************************************
 * OS-controlled sc_mutex channel interface: translated for systemc library
 * Sabine Francis, UT Austin <francisabine@utexas.edu>
 * Last update: Nov  2013 
 ********************************************/

#include <systemc>

#ifndef SC_MUTEX_OS_IF__H
#define SC_MUTEX_OS_IF__H

namespace HCSim { 

class sc_mutex_os_if
    : virtual public sc_core::sc_interface
{
 public:
    virtual int lock(int tID) = 0; 
	virtual int trylock(int tID) = 0; 
	virtual int unlock(int tID) = 0;

 protected:
	sc_mutex_os_if()
	{}

 private:
	// disabled
	sc_mutex_os_if( const sc_mutex_os_if& );
	sc_mutex_os_if& operator = ( const sc_mutex_os_if& );
};

} // namespace HCSim

#endif /*SC_MUTEX_OS_IF__H*/

