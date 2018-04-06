/*********************************************
 * Simple Bus: Loosely timed TLM 2.0 model
 * Parisa Razaghi, UT Austin <parisa.r@utexas.edu>
 * Last update: August 2013 
 ********************************************/
//#define SC_INCLUDE_DYNAMIC_PROCESSES
 
#include <tlm.h>
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/simple_initiator_socket.h"
#ifndef SYSTEMC_2_3_1  
#include <vector>
#endif

#include "sim_config.h"



#ifndef SC_SIMPLE_BUS_TLM__H
#define SC_SIMPLE_BUS_TLM__H

namespace HCSim{

template< int MASTER_NUM, int SLAVE_NUM>
class simpleBus_TLM
    :public sc_core::sc_module
{
  public:
#ifdef SYSTEMC_2_3_1  
    sc_core::sc_vector< tlm_utils::simple_target_socket< simpleBus_TLM > > master_target_socket;
    sc_core::sc_vector< tlm_utils::simple_initiator_socket< simpleBus_TLM > > slave_initiator_socket;
#else
    tlm_utils::simple_target_socket< simpleBus_TLM > master_target_socket[MASTER_NUM];
    tlm_utils::simple_initiator_socket< simpleBus_TLM > slave_initiator_socket[SLAVE_NUM];
#endif  

    SC_HAS_PROCESS(simpleBus_TLM);
    simpleBus_TLM(const sc_core::sc_module_name name)
        :sc_core::sc_module(name)
#ifdef SYSTEMC_2_3_1          
        ,master_target_socket("master_target_socket", MASTER_NUM)
        ,slave_initiator_socket("slave_initiator_socket", SLAVE_NUM)
#endif        
    {
        for (int m = 0; m < MASTER_NUM; m++) {
            master_target_socket[m].register_nb_transport_fw(this, &simpleBus_TLM::nb_transport_fw);
            master_target_socket[m].register_b_transport(this, &simpleBus_TLM::b_transport);
            master_target_socket[m].register_get_direct_mem_ptr(this, &simpleBus_TLM::get_direct_mem_ptr);
            master_target_socket[m].register_transport_dbg(this, &simpleBus_TLM::transport_dbg);            
        }
        
        for (int s = 0; s < SLAVE_NUM; s++) {
            slave_initiator_socket[s].register_nb_transport_bw( this, &simpleBus_TLM::nb_transport_bw);
            slave_initiator_socket[s].register_invalidate_direct_mem_ptr( this, &simpleBus_TLM::invalidate_direct_mem_ptr);
        }
        
        for (int s = 0; s < SLAVE_NUM; s++) {
            address_valid[s] = false;
            start_address[s] = 0;
            end_address[s] = 0;
        }      
    }     
    ~simpleBus_TLM() {}
    
    void setAddressSpace(sc_dt::uint64 start_addr, sc_dt::uint64 end_addr, int slave_id)
    {
        assert (slave_id < SLAVE_NUM );
        
        address_valid[slave_id] = true;
        start_address[slave_id] = start_addr;
        end_address[slave_id] = end_addr;
    }
    
    virtual void b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
    {
        int socket;
        mutex.lock();
        socket = decode(trans.get_address());
        if ( socket>= 0) {
            slave_initiator_socket[socket]->b_transport(trans, delay);
#ifdef BUS_TIMED_MODEL 
            if (trans.get_response_status() == tlm::TLM_OK_RESPONSE) 
               sc_core::wait(delay);
#endif                 
        }
        else 
            trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);            
               
        mutex.unlock();   
    } 

    virtual tlm::tlm_sync_enum nb_transport_fw( tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay ) 
    {
        std::cout << sc_core::sc_time_stamp() <<":simpleBus_TLM <<nb_transport_fw>> not supported!\n";
        return tlm::TLM_ACCEPTED; 
    }
    
    virtual bool get_direct_mem_ptr( tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data)
    { 
        std::cout << sc_core::sc_time_stamp() <<":simpleBus_TLM <<get_direct_mem_ptr>> not supported!\n";
        return false; 
    } 
    
    virtual unsigned int transport_dbg(tlm::tlm_generic_payload& r)
    { 
        std::cout << sc_core::sc_time_stamp() <<":simpleBus_TLM <<transport_dbg>> not supported!\n";
        return 0; 
    }

    virtual tlm::tlm_sync_enum nb_transport_bw( tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay ) 
    {
        std::cout << sc_core::sc_time_stamp() <<":simpleBus_TLM <<nb_transport_bw>> not supported!\n";
        return tlm::TLM_COMPLETED; 
    }

    virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range) 
    { 
        std::cout << sc_core::sc_time_stamp() <<":simpleBus_TLM <<invalidate_direct_mem_ptr>> not supported!\n";
    } 

 private:
    sc_core::sc_mutex mutex;
    sc_dt::uint64 start_address[SLAVE_NUM];
    sc_dt::uint64 end_address[SLAVE_NUM];
    bool address_valid[SLAVE_NUM];
    
    int decode(sc_dt::uint64 addr)
    {
        for (int slave = 0; slave < SLAVE_NUM; slave++)
            if ((address_valid[slave]) && (addr >= start_address[slave]) && (addr <= end_address[slave])) 
                return slave;
        return (-1);           
    }    
};

} /* namespace HCSim*/

#endif /* SC_BUS_TLM__H*/
