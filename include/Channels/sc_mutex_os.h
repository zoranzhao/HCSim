/*********************************************
 * OS-controlled sc_mutex channel: translated for systemc library
 * Sabine Francis, UT Austin <francisabine@utexas.edu>
 * Last update: Nov  2013 
 ********************************************/
#include <systemc>

#include "sim_config.h"
#include "OS/OSAPI.h"
#include "Channels/sc_mutex_os_if.h"

#ifndef SC_OS_MUTEX__H
#define SC_OS_MUTEX__H


namespace  HCSim {

class sc_mutex_os
	:public sc_core::sc_channel,
	virtual public sc_mutex_os_if
{
 public:

    sc_core::sc_port< OSAPI > os_port;
	
    sc_mutex_os(); 
    explicit sc_mutex_os(const sc_core::sc_module_name name);
    virtual ~sc_mutex_os();

    virtual int lock(int tID); 
    virtual int trylock(int tID); 
    virtual int unlock(int tID); 
    virtual const char* kind() const { return "sc_mutex"; }

 protected:
    sc_core::sc_process_b * m_owner;
    sc_core::sc_event m_free;
    OSProc blocked_task_id;
    OSProc blocking_task_id;
	
    bool in_use() const { return ( m_owner != 0 ); }
	
 private:
    // disabled 
    sc_mutex_os( const sc_mutex_os& );
    sc_mutex_os& operator = ( const sc_mutex_os& );
};

} // namespace HCSim
#endif /*SC_MUTEX_OS__H*/
