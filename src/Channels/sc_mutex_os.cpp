/*********************************************
 * Sabine Francis, UT Austin <francisabine@utexas.edu>
 * Last update: Nov  2013 
 ********************************************/

#include "Channels/sc_mutex_os.h"
#include "OS/global_os.h"

using namespace HCSim;  

sc_mutex_os::sc_mutex_os()
    :sc_core::sc_channel(sc_core::sc_gen_unique_name( "mutex" ) )
    ,m_owner( 0 )
    ,blocked_task_id(OS_NO_TASK)
    ,blocking_task_id(OS_NO_TASK)
{
} 

sc_mutex_os::sc_mutex_os(const sc_core::sc_module_name name)
    :sc_core::sc_channel(name)
    ,m_owner( 0 )
    ,blocked_task_id(OS_NO_TASK)
    ,blocking_task_id(OS_NO_TASK)
{
}

sc_mutex_os::~sc_mutex_os()
{
}

// Methods
int sc_mutex_os::lock(int tID)
{ /* returns -1 if mutex could not be locked */

	os_port->syncGlobalTime(tID);
    if ( m_owner == sc_core::sc_get_current_process_b()) 
        return 0;
    
    while( in_use() ) {
        blocked_task_id=tID;	
        os_port->preWait(tID, blocking_task_id);
        sc_core::wait( m_free, sc_core::sc_get_curr_simcontext() ); 
        os_port->postWait(tID);    
    }

    m_owner = sc_core::sc_get_current_process_b();
    blocking_task_id = tID; /* tID has the lock*/ 
    blocked_task_id = OS_NO_TASK; /* default value */  /*put OSNoTask*/

    return 0;
}

int sc_mutex_os::trylock(int tID)
{ /* returns -1 if mutex was not locked by caller */

    os_port->syncGlobalTime(tID);

    if ( m_owner == sc_core::sc_get_current_process_b()) 
        return 0;

    if( in_use() ){
        return -1;
    }

    m_owner = sc_core::sc_get_current_process_b();

    return 0;
}

int sc_mutex_os::unlock(int tID)
{ 
    os_port->syncGlobalTime(tID);
    if( m_owner != sc_core::sc_get_current_process_b()) {
        return -1;
    }

    m_owner = 0;
    os_port->preNotify(tID, blocked_task_id);
    m_free.notify(); 
    os_port->postNotify(tID, blocked_task_id); 
    blocking_task_id = OS_NO_TASK;  

    return 0;
}
