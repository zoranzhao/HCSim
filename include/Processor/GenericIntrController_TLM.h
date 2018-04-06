/*********************************************                       
 * Generic Interrupt Controller
 * Parisa Razaghi, UT Austin <parisa.r@utexas.edu> 
 * Last update: Jun. 2013                                            
 ********************************************/
#include <stdint.h>
#include <vector>
#include <systemc>
#include <tlm.h>

#include "sim_config.h"
#include "Processor/global_gic.h"
#include "Processor/interrupt_if.h"
#include "Channels/receive_if.h"
#include "Channels/ambaAhb_if.h"

#ifndef SC_GIC_TLM__H
#define SC_GIC_TLM__H

namespace HCSim {

/******************************************************************************/
class ClearIntr_tlm_if
    :virtual public sc_core::sc_interface
{
    public:
        virtual void clearIntr() = 0;
};    
/******************************************************************************/
template< int INTR_NUM, int CPU_NUM >
class GIC_IntrDetect_TLM   
    :public sc_core::sc_module
    ,virtual public ClearIntr_tlm_if
{	

 public:
    sc_core::sc_port< receive_if > detect;
    sc_core::sc_in< sc_dt::sc_bv< GIC_MAX_CPU > > IPTR;
#ifdef SYSTEMC_2_3_1    
    sc_core::sc_vector< sc_core::sc_inout_rv< INTR_NUM > > pending;
#else
    sc_core::sc_out_rv< INTR_NUM > pending[CPU_NUM];
#endif    
    sc_core::sc_export< ClearIntr_tlm_if >  clearIPBR;
   
    SC_HAS_PROCESS(GIC_IntrDetect_TLM);
    GIC_IntrDetect_TLM(const sc_core::sc_module_name name, unsigned int intr_id);
    ~GIC_IntrDetect_TLM();
    /* ClearIntr_tlm_if method */
    void clearIntr();
    
 private:
    unsigned int intr_id;
    sc_dt::sc_lv< INTR_NUM > pending_flag_off;
    sc_dt::sc_lv< INTR_NUM > pending_flag_on;
    sc_core::sc_event clear_intr_event;
    
    void intr_detect_monitor(void);
};

template< int INTR_NUM, int CPU_NUM >
GIC_IntrDetect_TLM< INTR_NUM, CPU_NUM >::GIC_IntrDetect_TLM(const sc_core::sc_module_name name, 
                                                                                          unsigned int intr_id)
    :sc_core::sc_module(name)   
{
#ifdef SYSTEMC_2_3_1  
    pending.init(CPU_NUM);
#endif
    
    clearIPBR(*this); 
    this->intr_id = intr_id;
    
    for (int i = 0; i < INTR_NUM; i++)
        pending_flag_off[i] = sc_dt::SC_LOGIC_Z;
    pending_flag_off[intr_id] = sc_dt::SC_LOGIC_0;
   
    for (int i = 0; i < INTR_NUM; i++)
        pending_flag_on[i] = sc_dt::SC_LOGIC_Z;
    pending_flag_on[intr_id] = sc_dt::SC_LOGIC_1;
    
    //for (int cpu = 0; cpu < CPU_NUM; cpu++)
    //    pending[cpu].initialize(pending_flag_off);

    SC_THREAD(intr_detect_monitor);
}

template< int INTR_NUM, int CPU_NUM >
GIC_IntrDetect_TLM< INTR_NUM, CPU_NUM >::~GIC_IntrDetect_TLM()
{
}

template< int INTR_NUM, int CPU_NUM >
void GIC_IntrDetect_TLM< INTR_NUM, CPU_NUM >::clearIntr()
{
    clear_intr_event.notify();  
}

template< int INTR_NUM, int CPU_NUM >
void GIC_IntrDetect_TLM< INTR_NUM, CPU_NUM >::intr_detect_monitor(void)
{
    int cpu_loop = 0;
    
    for (int cpu = 0; cpu < CPU_NUM; cpu++)
        pending[cpu]->write(pending_flag_off);
        
    while (1) {	   
            cpu_loop = 0;
	        detect->receive();
	        while (cpu_loop < CPU_NUM) {
			    /* Set pending bit*/
	            if (IPTR->read()[cpu_loop] == '1') { /* Note: Only one cpu should be programmed as the target cpu. */           
	                pending[cpu_loop]->write(pending_flag_on);
#ifdef GIC_TRACE_ON
					printf("%llu: interrupt detected -------- %d, sends to core %d \n", sc_core::sc_time_stamp().value(), intr_id, cpu_loop);
#endif 
                    break;      
                }
                 cpu_loop++; 
	        }    
            /* Clear pending bit*/  
            sc_core::wait(clear_intr_event);
            for (int c = 0; c < CPU_NUM; c++)
                pending[c]->write(pending_flag_off);    
    }
}
/******************************************************************************/
 class intrState_tlm_if
    :virtual public sc_core::sc_interface
{
 public:
    virtual void readAck(unsigned int &data) = 0;
    virtual void writeEOI(unsigned int data) = 0; 
};   
/******************************************************************************/
template< int INTR_NUM, int CPU_NUM >
class GIC_CPUInterface_TLM
    :public sc_core::sc_module
    ,virtual public intrState_tlm_if
{
 
 public:
    sc_core::sc_in_rv< INTR_NUM > IPBR;
	sc_core::sc_in< sc_dt::sc_uint< INTR_NUM > > IER;
    sc_core::sc_out< bool > nIRQ;
#ifdef SYSTEMC_2_3_1      
    sc_core::sc_vector< sc_core::sc_port< ClearIntr_tlm_if > > clearIPBR;
#else
    sc_core::sc_port< ClearIntr_tlm_if > clearIPBR[INTR_NUM];
#endif
   
    SC_HAS_PROCESS(GIC_CPUInterface_TLM);
    GIC_CPUInterface_TLM(const sc_core::sc_module_name name, unsigned int cpu_id, sc_dt::uint64 clock_period = BUS_CLOCK_PERIOD);
     ~GIC_CPUInterface_TLM();

    /* intrState_tlm_if methods*/
    void readAck(unsigned int &data);
    void writeEOI(unsigned int data);          
     
 private:
    unsigned int cpu_id;
    sc_core::sc_event ack_event;
    sc_core::sc_event eoi_event;
    unsigned int ack, eoi;
    sc_dt::uint64 bus_clock_period;
    
    void cpu_monitor(void);
};    

template< int INTR_NUM, int CPU_NUM >
GIC_CPUInterface_TLM< INTR_NUM, CPU_NUM >::GIC_CPUInterface_TLM(const sc_core::sc_module_name name,
                                                                                                      unsigned int cpu_id, sc_dt::uint64 clock_period)
    :sc_core::sc_module(name)
{
#ifdef SYSTEMC_2_3_1   
    clearIPBR.init(INTR_NUM);
#endif
    this->bus_clock_period = clock_period;
    this->cpu_id = cpu_id;
    nIRQ.initialize(1);
    
    SC_THREAD(cpu_monitor);    
}

template< int INTR_NUM, int CPU_NUM >
GIC_CPUInterface_TLM< INTR_NUM, CPU_NUM >::~GIC_CPUInterface_TLM()
{
}
 
template< int INTR_NUM, int CPU_NUM >   
void GIC_CPUInterface_TLM< INTR_NUM, CPU_NUM >::readAck(unsigned int &data)
{
    data = ack;
    ack_event.notify();
}

template< int INTR_NUM, int CPU_NUM >   
void GIC_CPUInterface_TLM< INTR_NUM, CPU_NUM >::writeEOI(unsigned int data)
{
    eoi = data;
    eoi_event.notify();
} 

 template< int INTR_NUM, int CPU_NUM >   
void GIC_CPUInterface_TLM< INTR_NUM, CPU_NUM >::cpu_monitor(void)
{
    unsigned int intr_id;
    bool flag;        
	
    nIRQ->write(1);
    sc_core::wait(sc_core::SC_ZERO_TIME); // for start up

    while(1) {
        while ((IPBR.read().or_reduce()) == 0) 
            wait( IPBR->default_event() );
   
        intr_id = GIC_SPURIOUS_INTERRUPT;
        flag = false;
        intr_id = 0;
        while (!flag && (intr_id < INTR_NUM)) 
           if (IPBR->read()[intr_id] != '0'  ) 
                flag = true;
            else
                intr_id++;
        if (!flag)
            intr_id = GIC_SPURIOUS_INTERRUPT;
        if (flag)	 {
            clearIPBR[intr_id]->clearIntr();
       }    
        ack = intr_id;
        /* Activate nIRQ signal*/
        nIRQ->write(0);
        /* Ack. read */
        sc_core::wait(ack_event);
        /* Deactivate nIRQ signal*/
        nIRQ->write(1);
        /* EOI write*/
        sc_core::wait(eoi_event);
#ifdef BUS_TIMED_MODEL        
        sc_core::wait(bus_clock_period, SIM_RESOLUTION);
#endif
        if (eoi != ack)
            std::cerr << sc_core::sc_time_stamp() << ": ERROR GIC << ACK != EOI >> \n";
        intr_id = GIC_SPURIOUS_INTERRUPT;	
    }
}

/******************************************************************************/
template< int INTR_NUM = 32, int CPU_NUM = 2 >
class GenericIntrController_TLM 
    :public sc_core::sc_module
    ,tlm::tlm_fw_transport_if<>
{
 public:
    tlm::tlm_target_socket<> slave_target_socket;
#ifdef SYSTEMC_2_3_1       
    sc_core::sc_vector< sc_core::sc_port< receive_if > > HINT_tlm;
    sc_core::sc_vector< sc_core::sc_out< bool > > nIRQ;
#else
    sc_core::sc_port< receive_if > HINT_tlm[INTR_NUM];
    sc_core::sc_out< bool > nIRQ[CPU_NUM];
#endif

    GenericIntrController_TLM(const sc_core::sc_module_name name, sc_dt::uint64 clock_period = BUS_CLOCK_PERIOD);
    ~GenericIntrController_TLM();
	
	 void setIntrTargetCPU(int intrID, int targetCPU);
	 
 private:
     /* Interrupt Pending Registers */
#ifdef SYSTEMC_2_3_1      
    sc_core::sc_vector< sc_core::sc_signal_rv< INTR_NUM > >  IPBR;
#else
    sc_core::sc_signal_rv< INTR_NUM > IPBR[CPU_NUM];
#endif
    /* Interrupt Enable Bit Register */
    sc_core::sc_signal< sc_dt::sc_uint< INTR_NUM > >  IER;
    /* Interrupt Processor Target Registers */
#ifdef SYSTEMC_2_3_1 
    sc_core::sc_vector< sc_core::sc_signal< sc_dt::sc_bv< GIC_MAX_CPU > > > IPTR;
#else
    sc_core::sc_signal< sc_dt::sc_bv< GIC_MAX_CPU > > IPTR[INTR_NUM];
#endif
          
    std::vector< GIC_IntrDetect_TLM< INTR_NUM, CPU_NUM >* > intrDetect;
    std::vector< GIC_CPUInterface_TLM< INTR_NUM, CPU_NUM >* > cpuInterface; 
    
 public:
    void b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay);
    tlm::tlm_sync_enum nb_transport_fw( tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay);
    bool get_direct_mem_ptr( tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data);
    unsigned int transport_dbg(tlm::tlm_generic_payload& r);
      
};

template< int INTR_NUM, int CPU_NUM > 
GenericIntrController_TLM< INTR_NUM, CPU_NUM >::GenericIntrController_TLM(const sc_core::sc_module_name name, sc_dt::uint64 clock_period)
    :sc_core::sc_module(name)
{

    assert (INTR_NUM <= GIC_MAX_SPI);
    assert (CPU_NUM <= GIC_MAX_CPU);
    
    slave_target_socket.bind(*this);

#ifdef SYSTEMC_2_3_1     
    HINT_tlm.init(INTR_NUM);
    nIRQ.init(CPU_NUM);
    IPBR.init(CPU_NUM);
    IPTR.init(INTR_NUM);
#endif
    
   /* Map all interrupts to CPU 0 */
    sc_dt::sc_bv< GIC_MAX_CPU > tmp;
    for (int intr = 0; intr < INTR_NUM; intr++) {
        for (unsigned int i = 0; i < GIC_MAX_CPU; i++) // clear all bits
            tmp[i] = sc_dt::SC_LOGIC_0;
        tmp[0] = sc_dt::SC_LOGIC_1;
        IPTR[intr].write(tmp);
    }
                  
    /**********************************/  
    for (int intr = 0; intr < INTR_NUM; intr++ ) {
        GIC_IntrDetect_TLM< INTR_NUM, CPU_NUM >* det;
	    det = new GIC_IntrDetect_TLM< INTR_NUM, CPU_NUM >(sc_core::sc_gen_unique_name("intr_detect"), intr);
		 /* Port binding */  
		det->detect(HINT_tlm[intr]);
		det->IPTR(IPTR[intr]);
		for (int cpu = 0; cpu< CPU_NUM; cpu++) 
		    det->pending[cpu].bind(IPBR[cpu]); 
        intrDetect.push_back(det);				
    }
			    
    for (int cpu = 0; cpu < CPU_NUM; cpu++ ) {
	    GIC_CPUInterface_TLM< INTR_NUM, CPU_NUM >* cpu_if;
        cpu_if = new GIC_CPUInterface_TLM< INTR_NUM, CPU_NUM >(sc_core::sc_gen_unique_name("cpu_interface"), cpu, clock_period);
         /* Port binding */  
        cpu_if->IPBR(IPBR[cpu]);
        cpu_if->IER(IER);
        cpu_if->nIRQ(nIRQ[cpu]);
        for (int intr = 0; intr < INTR_NUM; intr++ ) 
            (cpu_if->clearIPBR[intr])(intrDetect[intr]->clearIPBR);
	    cpuInterface.push_back(cpu_if);				
    }
}

template< int INTR_NUM, int CPU_NUM > 		
GenericIntrController_TLM< INTR_NUM, CPU_NUM >::~GenericIntrController_TLM()
{
    /* cleanup */
    for( unsigned i = 0; i < intrDetect.size(); i++ )
        delete intrDetect[i];
    intrDetect.clear();

    for( unsigned i = 0; i < cpuInterface.size(); i++ )
        delete cpuInterface[i];
    cpuInterface.clear();
}

template< int INTR_NUM, int CPU_NUM > 		
void GenericIntrController_TLM< INTR_NUM, CPU_NUM >::setIntrTargetCPU(int intrID, int targetCPU)
{
    sc_dt::sc_bv< GIC_MAX_CPU > tmp;
   
    assert ( (unsigned int)targetCPU < GIC_MAX_CPU );
   
    for (unsigned int i = 0; i < GIC_MAX_CPU; i++) // clear all bits
        tmp[i] = sc_dt::SC_LOGIC_0;
    tmp[targetCPU] = sc_dt::SC_LOGIC_1;
    IPTR[intrID].write(tmp);
}

template< int INTR_NUM, int CPU_NUM >
void GenericIntrController_TLM< INTR_NUM, CPU_NUM >::b_transport( tlm::tlm_generic_payload& trans, sc_core::sc_time& delay )
{
    sc_dt::uint64 trans_addr, addr;
    unsigned int data;
    unsigned char* trans_data;
    
    trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);    
    trans_addr = trans.get_address();
    trans_data = trans.get_data_ptr();
    
    if (trans.is_read()) {
        for (int cpu = 0; cpu < CPU_NUM; cpu++) {
            addr = GIC_BASE_ADDRESS + (GIC_CPUADDR_STEP * cpu) + GIC_IAR_OFFSET;
            if (trans_addr == addr) {
                cpuInterface[cpu]->readAck(data);
                memcpy(trans_data, &data, 4);
                trans.set_response_status(tlm::TLM_OK_RESPONSE);
                break; 
             }
        }    
    } 
    else if (trans.is_write()) {
        for (int cpu = 0; cpu < CPU_NUM; cpu++) {
            addr = GIC_BASE_ADDRESS + (GIC_CPUADDR_STEP * cpu) + GIC_EOIR_OFFSET;
            if (trans_addr == addr) {
                memcpy(&data, trans_data, 4);
                cpuInterface[cpu]->writeEOI(data);
                trans.set_response_status(tlm::TLM_OK_RESPONSE);
                break; 
             }
        }    
    }
    else
        trans.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);        
}        

template< int INTR_NUM, int CPU_NUM >
tlm::tlm_sync_enum GenericIntrController_TLM< INTR_NUM, CPU_NUM >::nb_transport_fw( tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay ) 
{
        std::cout << sc_core::sc_time_stamp() <<":GenericIntrController_TLM <<nb_transport_fw>> not supported!\n";
        return tlm::TLM_ACCEPTED; 
}

template< int INTR_NUM, int CPU_NUM >    
bool GenericIntrController_TLM< INTR_NUM, CPU_NUM >::get_direct_mem_ptr( tlm::tlm_generic_payload& trans, tlm::tlm_dmi& dmi_data)
{ 
        std::cout << sc_core::sc_time_stamp() <<":GenericIntrController_TLM <<get_direct_mem_ptr>> not supported!\n";
        return false; 
} 
    
template< int INTR_NUM, int CPU_NUM >
unsigned int GenericIntrController_TLM< INTR_NUM, CPU_NUM >::transport_dbg(tlm::tlm_generic_payload& r)
{ 
        std::cout << sc_core::sc_time_stamp() <<":GenericIntrController_TLM <<transport_dbg>> not supported!\n";
        return 0; 
}


} // namespace HCSim

#endif /* SC_GIC__H*/

    
