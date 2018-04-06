/*********************************************
 * TLM 2.0 master/slave adapters
 * Parisa Razaghi, UT Austin <parisa.r@utexas.edu>
 * Last update: August 2013 
 ********************************************/
 
#include <tlm.h>

#include "sim_config.h"
#include "Channels/ambaAhb_if.h"
#ifndef SYSTEMC_2_3_1
#include <vector>
#endif

#ifndef SC_ADAPTERS_TLM__H
#define SC_ADAPTERS_TLM__H

namespace HCSim{

/*===================================================================*/
class MasterMacLink2TLMInitiator
    :public sc_core::sc_channel
    ,virtual public IAmbaAhbBusMasterMacLink
    ,virtual public tlm::tlm_bw_transport_if<>
{
 public:
    tlm::tlm_initiator_socket<> initiator_socket;
    
    MasterMacLink2TLMInitiator()
         :sc_core::sc_module(sc_core::sc_gen_unique_name("MasterMacLink2TLMInitiator")) 
         ,initiator_socket(sc_core::sc_gen_unique_name("init_socket"))
         {
            initiator_socket.bind(*this);
         }
    MasterMacLink2TLMInitiator(sc_core::sc_module_name _name)
         :sc_core::sc_module(_name)
         ,initiator_socket(sc_core::sc_gen_unique_name("init_socket"))
          {
            initiator_socket.bind(*this);
          }
    virtual ~MasterMacLink2TLMInitiator(void) {}

    void masterWrite(unsigned long addr, const void* data, unsigned long len)
    {
        tlm::tlm_generic_payload trans;
        sc_core::sc_time delay;
         
        trans.set_command(tlm::TLM_WRITE_COMMAND);
        trans.set_address(addr);
        trans.set_data_length(len);
        trans.set_data_ptr((unsigned char *) data);
        initiator_socket->b_transport(trans, delay);
    }
  
    void masterRead(unsigned long addr, void* data, unsigned long len) 
    {
        tlm::tlm_generic_payload trans;
        sc_core::sc_time delay;
         
        trans.set_command(tlm::TLM_READ_COMMAND);
        trans.set_address(addr);
        trans.set_data_length(len);
        trans.set_data_ptr((unsigned char *) data);
          
        initiator_socket->b_transport(trans, delay);
    }     
    
   virtual tlm::tlm_sync_enum nb_transport_bw( tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay ) 
    {
        std::cout << sc_core::sc_time_stamp() <<":MasterMacLink2TLMInitiator <<nb_transport_bw>> not supported!\n";
        return tlm::TLM_COMPLETED; 
    }

    virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range) 
    { 
        std::cout << sc_core::sc_time_stamp() <<":MasterMacLink2TLMInitiator <<invalidate_direct_mem_ptr>> not supported!\n";
    } 
          
};
/*===================================================================*/
class MasterMacMem2TLMInitiator
    :public sc_core::sc_channel
    ,virtual public IAmbaAhbBusMasterMacMem
    ,virtual public tlm::tlm_bw_transport_if<>
{
 public:
    tlm::tlm_initiator_socket<> initiator_socket;  
    
    MasterMacMem2TLMInitiator()
        :sc_core::sc_module(sc_core::sc_gen_unique_name("MasterMacMem2TLMInitiator"))
        ,initiator_socket(sc_core::sc_gen_unique_name("init_socket"))
        {
            initiator_socket.bind(*this);
        }
    MasterMacMem2TLMInitiator(sc_core::sc_module_name _name)
        :sc_core::sc_module(_name)
        ,initiator_socket(sc_core::sc_gen_unique_name("init_socket"))
        {
            initiator_socket.bind(*this);
        }

    virtual ~MasterMacMem2TLMInitiator(void) {}
    
    void masterMemWrite(unsigned long addr, const void* data, unsigned long len) 
    {
        tlm::tlm_generic_payload trans;
        sc_core::sc_time delay;
         
        trans.set_command(tlm::TLM_WRITE_COMMAND);
        trans.set_address(addr);
        trans.set_data_length(len);
        trans.set_data_ptr((unsigned char *) data);
          
        initiator_socket->b_transport(trans, delay);
    }
  
    void masterMemRead(unsigned long addr, void* data, unsigned long len)
    {
        tlm::tlm_generic_payload trans;
        sc_core::sc_time delay;
         
        trans.set_command(tlm::TLM_READ_COMMAND);
        trans.set_address(addr);
        trans.set_data_length(len);
        trans.set_data_ptr((unsigned char *) data);
          
        initiator_socket->b_transport(trans, delay);    
    } 
    
    virtual tlm::tlm_sync_enum nb_transport_bw( tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay ) 
    {
        std::cout << sc_core::sc_time_stamp() <<":MasterMacMem2TLMInitiator <<nb_transport_bw>> not supported!\n";
        return tlm::TLM_COMPLETED; 
    }

    virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range) 
    { 
        std::cout << sc_core::sc_time_stamp() <<":MasterMacMem2TLMInitiator <<invalidate_direct_mem_ptr>> not supported!\n";
    } 
   
};
/*===================================================================*/
class SlaveMacLink2TLMTarget
    :public sc_core::sc_channel
    ,virtual public IAmbaAhbBusSlaveMacLink
    ,virtual public tlm::tlm_fw_transport_if<>
{
public:
    tlm::tlm_target_socket<> target_socket;
 
    SlaveMacLink2TLMTarget()
         :sc_core::sc_module(sc_core::sc_gen_unique_name("SlaveMacLink2TLMTarget"))
         ,target_socket(sc_core::sc_gen_unique_name("target_socket"))
    {
        target_socket.bind( *this );
        bus_clock_period = BUS_CLOCK_PERIOD;
    }   
    SlaveMacLink2TLMTarget(sc_core::sc_module_name _name, sc_dt::uint64 clock_period = BUS_CLOCK_PERIOD)
        :sc_core::sc_module(_name)
        ,target_socket(sc_core::sc_gen_unique_name("target_socket"))
    {
        target_socket.bind( *this );
        bus_clock_period = clock_period;
    }
    virtual ~SlaveMacLink2TLMTarget() {}
    
    void slaveWrite(unsigned long addr, const void* data, unsigned long len)  
    {
	            
        do {   
	        wait(master_read_event);
	    } while (ADDR != addr);
	    memcpy(DATA, data, len);
 	    
#ifdef BUS_TIMED_MODEL	
       sc_core::wait(getShortestTransferTime(len, ADDR, false), SIM_RESOLUTION);
#endif    
	    ack_event.notify();    
    }
  
    void slaveRead(unsigned long addr, void* data, unsigned long len)  
    {
        do {   
	        wait(master_write_event);
	    } while (ADDR != addr);
	    memcpy(data, DATA, len);
#ifdef BUS_TIMED_MODEL	
	sc_core::wait(getShortestTransferTime(len, ADDR, false), SIM_RESOLUTION);
#endif	    
	    ack_event.notify();     
    }
    
    void setClockPeriod(sc_dt::uint64 clock_period) 
    {
        bus_clock_period = clock_period;
    }
    
    virtual void b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
    {
	    mutex.lock();
	    ADDR = trans.get_address();
	    DATA = (void *)trans.get_data_ptr();
	    LEN = trans.get_data_length();
	    if (trans.is_write())  
	        master_write_event.notify();
	    else
	        master_read_event.notify();    
	    sc_core::wait(ack_event);
	    trans.set_response_status(tlm::TLM_OK_RESPONSE);
	    mutex.unlock();        
    } 

    virtual tlm::tlm_sync_enum nb_transport_fw( tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay ) 
    {
        std::cout << sc_core::sc_time_stamp() <<":SlaveMacLink2TLMTarget <<nb_transport_fw>> not supported!\n";
        return tlm::TLM_ACCEPTED; 
    }
    
    virtual bool get_direct_mem_ptr( tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data)
    { 
        std::cout << sc_core::sc_time_stamp() <<":SlaveMacLink2TLMTarget <<get_direct_mem_ptr>> not supported!\n";
        return false; 
    } 
    
    virtual unsigned int transport_dbg(tlm::tlm_generic_payload& r)
    { 
        std::cout << sc_core::sc_time_stamp() <<":SlaveMacLink2TLMTarget <<transport_dbg>> not supported!\n";
        return 0; 
    }

    virtual tlm::tlm_sync_enum nb_transport_bw( tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay ) 
    {
        std::cout << sc_core::sc_time_stamp() <<":SlaveMacLink2TLMTarget <<nb_transport_bw>> not supported!\n";
        return tlm::TLM_COMPLETED; 
    }

    virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range) 
    { 
        std::cout << sc_core::sc_time_stamp() <<":SlaveMacLink2TLMTarget <<invalidate_direct_mem_ptr>> not supported!\n";
    } 

 private:
    sc_core::sc_mutex mutex;
    sc_core::sc_event master_write_event;
    sc_core::sc_event master_read_event;
    sc_core::sc_event ack_event;
       
    sc_dt::uint64 ADDR;
    void * DATA;
    unsigned int LEN;    
    sc_dt::uint64 bus_clock_period;
    
 private:
   sc_dt::uint64 getShortestTransferTime(unsigned int len, 
                                         unsigned long int addr,
                                         bool burstEnable)
    {   
        unsigned int cycles = 0u;
        unsigned int transfers;
        unsigned long long int cycleOffset;

	     cycleOffset = ((sc_core::sc_time_stamp().value() + (bus_clock_period / 2)) % bus_clock_period);
        if ((cycleOffset == 0) && (sc_core::sc_delta_count() <= 1)) 
        {   
            cycleOffset = bus_clock_period;
        }
        if (len && (addr & 1))
        {   
            len -= 1;
            addr++ ;
            cycles += 4;
        }
        if ((len >= 2) && (addr & 2))
        {   
            cycles += 4;
            len -= 2;
        }
        if (burstEnable)
        {   
            transfers = len / 64;
            len -= transfers * 64;
            cycles += transfers * 19;
            transfers = len / 32;
            len -= transfers * 32;
            cycles += transfers * 11;
            transfers = len / 16;
            len -= transfers * 16;
            cycles += transfers * 7;
        }
        transfers = len / 4;
        len -= transfers * 4;
        if (len >= 2)
        {   
            len -= 2;
            transfers++ ;
        }
        if (len & 1)
        {   
            transfers++ ;
        }
        cycles += transfers * 4;
        return cycles * bus_clock_period - cycleOffset;
    }       
           
};
/*===================================================================*/
class SlaveMacMem2TLMTarget
    :public sc_core::sc_channel
    ,virtual public IAmbaAhbBusSlaveMacMem
    ,virtual public tlm::tlm_fw_transport_if<>
{
public:
    tlm::tlm_target_socket<> target_socket;
 
    SlaveMacMem2TLMTarget()
         :sc_core::sc_module(sc_core::sc_gen_unique_name("SlaveMacMem2TLMTarget"))
         ,target_socket(sc_core::sc_gen_unique_name("target_socket"))
    {
        target_socket.bind( *this );
        bus_clock_period = BUS_CLOCK_PERIOD;
    }   
    SlaveMacMem2TLMTarget(sc_core::sc_module_name _name, sc_dt::uint64 clock_period = BUS_CLOCK_PERIOD)
        :sc_core::sc_module(_name)
        ,target_socket(sc_core::sc_gen_unique_name("target_socket"))
    {
        target_socket.bind( *this );
        bus_clock_period = clock_period;
    }
    virtual ~SlaveMacMem2TLMTarget() {}
 
    void setClockPeriod(sc_dt::uint64 clock_period) 
    {
        bus_clock_period = clock_period;
    }   
    
    void serve(unsigned long int addr, void *data, unsigned long int len)
    {  
        do {   
	        sc_core::wait(data_ready_event);
        } while((ADDR < addr) || (ADDR >= addr + len));
	
        data = (void *)(((unsigned char *)data) + (ADDR - addr));
        if (write_flag)
        {   
            memcpy(DATA, data, LEN);
        }
        else 
        {   
            memcpy(data, DATA, LEN);
        }
	
#ifdef BUS_TIMED_MODEL	
        sc_core::wait(getShortestTransferTime(LEN, ADDR, true), SIM_RESOLUTION);
#endif	
        ack_event.notify();
    }

    void serveRead(unsigned long int addr, void *data, unsigned long int len)
    { 
        serve(addr, data, len);
    }

    void serveWrite(unsigned long int addr, void *data, unsigned long int len)
    {   
	     serve(addr, data, len);
    }
    
    virtual void b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
    {
	    mutex.lock();
	    ADDR = trans.get_address();
	    DATA = (void *)trans.get_data_ptr();
	    LEN = trans.get_data_length();
	    if (trans.is_write())  
	        write_flag = false;
	    else
	        write_flag = true; 
	    data_ready_event.notify();       
	    sc_core::wait(ack_event);
	    trans.set_response_status(tlm::TLM_OK_RESPONSE);
	    mutex.unlock();        
    } 

    virtual tlm::tlm_sync_enum nb_transport_fw( tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay ) 
    {
        std::cout << sc_core::sc_time_stamp() <<":SlaveMacMem2TLMTarget <<nb_transport_fw>> not supported!\n";
        return tlm::TLM_ACCEPTED; 
    }
    
    virtual bool get_direct_mem_ptr( tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data)
    { 
        std::cout << sc_core::sc_time_stamp() <<":SlaveMacMem2TLMTarget <<get_direct_mem_ptr>> not supported!\n";
        return false; 
    } 
    
    virtual unsigned int transport_dbg(tlm::tlm_generic_payload& r)
    { 
        std::cout << sc_core::sc_time_stamp() <<":SlaveMacMem2TLMTarget <<transport_dbg>> not supported!\n";
        return 0; 
    }

    virtual tlm::tlm_sync_enum nb_transport_bw( tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay ) 
    {
        std::cout << sc_core::sc_time_stamp() <<":SlaveMacMem2TLMTarget <<nb_transport_bw>> not supported!\n";
        return tlm::TLM_COMPLETED; 
    }

    virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range) 
    { 
        std::cout << sc_core::sc_time_stamp() <<":SlaveMacMem2TLMTarget <<invalidate_direct_mem_ptr>> not supported!\n";
    } 

 private:
    sc_core::sc_mutex mutex;
    sc_core::sc_event data_ready_event;
    sc_core::sc_event ack_event;
    bool write_flag; /*active low*/
       
    sc_dt::uint64 ADDR;
    void * DATA;
    unsigned int LEN;    
    sc_dt::uint64 bus_clock_period;
     
private:

   sc_dt::uint64 getShortestTransferTime(unsigned int len, 
                                         unsigned long int addr,
                                         bool burstEnable)
    {   
        unsigned int cycles = 0u;
        unsigned int transfers;
        unsigned long long int cycleOffset;

	     cycleOffset = ((sc_core::sc_time_stamp().value() + (bus_clock_period / 2)) % bus_clock_period);
        if ((cycleOffset == 0) && (sc_core::sc_delta_count() <= 1)) 
        {   
            cycleOffset = bus_clock_period;
        }
        if (len && (addr & 1))
        {   
            len -= 1;
            addr++ ;
            cycles += 4;
        }
        if ((len >= 2) && (addr & 2))
        {   
            cycles += 4;
            len -= 2;
        }
        if (burstEnable)
        {   
            transfers = len / 64;
            len -= transfers * 64;
            cycles += transfers * 19;
            transfers = len / 32;
            len -= transfers * 32;
            cycles += transfers * 11;
            transfers = len / 16;
            len -= transfers * 16;
            cycles += transfers * 7;
        }
        transfers = len / 4;
        len -= transfers * 4;
        if (len >= 2)
        {   
            len -= 2;
            transfers++ ;
        }
        if (len & 1)
        {   
            transfers++ ;
        }
        cycles += transfers * 4;
        return cycles * bus_clock_period - cycleOffset;
    }       
};
/*===================================================================*/
template < int MASTER_NUM >
class TLMInitiatorSocket_Master_Wrap
    :public sc_core::sc_channel
    ,virtual public tlm::tlm_fw_transport_if<>
    ,virtual public tlm::tlm_bw_transport_if<>
{
 public:
    tlm:: tlm_initiator_socket<> initiator_socket;
#ifdef SYSTEMC_2_3_1    
    sc_core::sc_vector< tlm::tlm_target_socket<> > target_socket;
#else
    tlm::tlm_target_socket<> target_socket[MASTER_NUM];
#endif
    TLMInitiatorSocket_Master_Wrap(const sc_core::sc_module_name name)
        :sc_core::sc_module(name)
        ,initiator_socket(sc_core::sc_gen_unique_name("init_socket"))
#ifdef SYSTEMC_2_3_1   
        ,target_socket(sc_core::sc_gen_unique_name("target_socket"), MASTER_NUM)
#endif
    {
        initiator_socket.bind(*this);
        for (int cpu = 0; cpu < MASTER_NUM; cpu++)
            target_socket[cpu].bind(*this);
    }
    ~TLMInitiatorSocket_Master_Wrap() 
    {   
    }
 
    virtual void b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
    {
        mutex.lock();
        initiator_socket->b_transport(trans, delay);        
        mutex.unlock();   
    } 

    virtual tlm::tlm_sync_enum nb_transport_fw( tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay ) 
    {
        std::cout << sc_core::sc_time_stamp() <<":TLMInitiatorSocket_Master_Wrap<<nb_transport_fw>> not supported!\n";
        return tlm::TLM_ACCEPTED; 
    }
    
    virtual bool get_direct_mem_ptr( tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data)
    { 
        std::cout << sc_core::sc_time_stamp() <<":TLMInitiatorSocket_Master_Wrap <<get_direct_mem_ptr>> not supported!\n";
        return false; 
    } 
    
    virtual unsigned int transport_dbg(tlm::tlm_generic_payload& r)
    { 
        std::cout << sc_core::sc_time_stamp() <<":TLMInitiatorSocket_Master_Wrap <<transport_dbg>> not supported!\n";
        return 0; 
    }

    virtual tlm::tlm_sync_enum nb_transport_bw( tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay ) 
    {
        std::cout << sc_core::sc_time_stamp() <<":TLMInitiatorSocket_Master_Wrap <<nb_transport_bw>> not supported!\n";
        return tlm::TLM_COMPLETED; 
    }

    virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range) 
    { 
        std::cout << sc_core::sc_time_stamp() <<":TLMInitiatorSocket_Master_Wrap <<invalidate_direct_mem_ptr>> not supported!\n";
    } 
     
 private:    
    sc_core::sc_mutex mutex;

};

/*===================================================================*/
class TLMTarget2Initiator_Transducer
    :public sc_core::sc_channel
    ,virtual public tlm::tlm_fw_transport_if<>
    ,virtual public tlm::tlm_bw_transport_if<>
{
typedef tlm::tlm_target_socket<32,
                               tlm::tlm_base_protocol_types,
                               0> target_socket_type;
 public:
    tlm:: tlm_initiator_socket<> initiator_socket;
    target_socket_type target_socket;

    TLMTarget2Initiator_Transducer()
        :sc_core::sc_module(sc_core::sc_gen_unique_name("tlm_target2initiator_transducer"))
        ,initiator_socket(sc_core::sc_gen_unique_name("init_socket"))
        ,target_socket(sc_core::sc_gen_unique_name("target_socket"))
    {
        initiator_socket.bind(*this);
        target_socket.bind(*this);
    }
    TLMTarget2Initiator_Transducer(const sc_core::sc_module_name name)
        :sc_core::sc_module(name)
        ,initiator_socket(sc_core::sc_gen_unique_name("init_socket"))
        ,target_socket(sc_core::sc_gen_unique_name("target_socket"))
    {
        initiator_socket.bind(*this);
        target_socket.bind(*this);
    }
    ~TLMTarget2Initiator_Transducer() {}
 
    virtual void b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
    {
        initiator_socket->b_transport(trans, delay);        
    } 

    virtual tlm::tlm_sync_enum nb_transport_fw( tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay ) 
    {
        std::cout << sc_core::sc_time_stamp() <<":TLMTarget2Initiator_Transducer <<nb_transport_fw>> not supported!\n";
        return tlm::TLM_ACCEPTED; 
    }
    
    virtual bool get_direct_mem_ptr( tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data)
    { 
        std::cout << sc_core::sc_time_stamp() <<":TLMTarget2Initiator_Transducer <<get_direct_mem_ptr>> not supported!\n";
        return false; 
    } 
    
    virtual unsigned int transport_dbg(tlm::tlm_generic_payload& r)
    { 
        std::cout << sc_core::sc_time_stamp() <<":TLMTarget2Initiator_Transducer <<transport_dbg>> not supported!\n";
        return 0; 
    }

    virtual tlm::tlm_sync_enum nb_transport_bw( tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay ) 
    {
        std::cout << sc_core::sc_time_stamp() <<":TLMTarget2Initiator_Transducer <<nb_transport_bw>> not supported!\n";
        return tlm::TLM_COMPLETED; 
    }

    virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range) 
    { 
        std::cout << sc_core::sc_time_stamp() <<":TLMTarget2Initiator_Transducer <<invalidate_direct_mem_ptr>> not supported!\n";
    } 
};    
    
} /* namespace HCSim*/

#endif /* SC_ADAPTERS_TLM__H*/
