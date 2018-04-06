/*********************************************                       
 * Generic Interrupt Controller
 * Parisa Razaghi, UT Austin <parisa.r@utexas.edu> 
 * Last update: Jun. 2013                                            
 ********************************************/
#include <stdint.h>
#include <systemc>
#include <vector>

#include "sim_config.h"
#include "Processor/global_gic.h"
#include "Processor/interrupt_if.h"
#include "Channels/receive_if.h"
#include "Channels/ambaAhb_if.h"

#ifndef SC_GIC__H
#define SC_GIC__H

namespace HCSim {

/******************************************************************************/
class ClearIntr_if
    :virtual public sc_core::sc_interface
{
    public:
        virtual void clearIntr() = 0;
};    
/******************************************************************************/
template< int INTR_NUM, int CPU_NUM >
class GIC_IntrDetect   
    :public sc_core::sc_module
    ,virtual public ClearIntr_if
{	

 public:
    sc_core::sc_port< receive_if > detect;
    sc_core::sc_in< sc_dt::sc_bv< GIC_MAX_CPU > > IPTR;
#ifdef SYSTEMC_2_3_1    
    sc_core::sc_vector< sc_core::sc_out_rv< INTR_NUM > > pending;
#else
    sc_core::sc_out_rv< INTR_NUM > pending[CPU_NUM];
#endif    
    sc_core::sc_export< ClearIntr_if >  clearIPBR;

    SC_HAS_PROCESS(GIC_IntrDetect);
    GIC_IntrDetect(const sc_core::sc_module_name name, unsigned int intr_id);
    ~GIC_IntrDetect();
    /* ClearIntr_if method */
    void clearIntr();
    
 private:
    unsigned int intr_id;
    sc_dt::sc_lv< INTR_NUM > pending_flag_off;
    sc_dt::sc_lv< INTR_NUM > pending_flag_on;
    sc_core::sc_event clear_intr_event;
    
    void intr_detect_monitor(void);
};

template< int INTR_NUM, int CPU_NUM >
GIC_IntrDetect< INTR_NUM, CPU_NUM >::GIC_IntrDetect(const sc_core::sc_module_name name, 
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
GIC_IntrDetect< INTR_NUM, CPU_NUM >::~GIC_IntrDetect()
{
}

template< int INTR_NUM, int CPU_NUM >
void GIC_IntrDetect< INTR_NUM, CPU_NUM >::clearIntr()
{
    clear_intr_event.notify();  
}

template< int INTR_NUM, int CPU_NUM >
void GIC_IntrDetect< INTR_NUM, CPU_NUM >::intr_detect_monitor(void)
{
    int cpu_loop = 0;
    
    for (int cpu = 0; cpu < CPU_NUM; cpu++)
        pending[cpu]->write(pending_flag_off);
        
    while (1) {	   
	        detect->receive();
	        while (cpu_loop < CPU_NUM) {
			    /* Set pending bit*/
	            if (IPTR->read()[cpu_loop] == sc_dt::SC_LOGIC_1) { /* Note: Only one cpu should be programmed as the target cpu. */
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
template< int INTR_NUM, int CPU_NUM >
class GIC_CPUInterface
    :public sc_core::sc_module
{
 
 public:
    
    sc_core::sc_port< IAmbaAhbBusSlaveMacTlmProt > bus_slave_port;
    sc_core::sc_in_rv< INTR_NUM > IPBR;
	sc_core::sc_in< sc_dt::sc_uint< INTR_NUM > > IER;
    sc_core::sc_out< bool > nIRQ;
#ifdef SYSTEMC_2_3_1    
    sc_core::sc_vector< sc_core::sc_port< ClearIntr_if > > clearIPBR;
#else
    sc_core::sc_port< ClearIntr_if > clearIPBR[INTR_NUM];
#endif
    SC_HAS_PROCESS(GIC_CPUInterface);
    GIC_CPUInterface(const sc_core::sc_module_name name, unsigned int cpu_id, sc_dt::uint64 clock_period = BUS_CLOCK_PERIOD);
     ~GIC_CPUInterface();
     
 private:
    unsigned int cpu_id;
    sc_dt::uint64 bus_clock_period;
    
    void cpu_monitor(void);
};    

template< int INTR_NUM, int CPU_NUM >
GIC_CPUInterface< INTR_NUM, CPU_NUM >::GIC_CPUInterface(const sc_core::sc_module_name name,
                                                                                                      unsigned int cpu_id, sc_dt::uint64 clock_period)
    :sc_core::sc_module(name)    
{
#ifdef SYSTEMC_2_3_1
    clearIPBR.init(INTR_NUM);
#endif    
    this->cpu_id = cpu_id;
    this->bus_clock_period = clock_period;
    nIRQ.initialize(1);
    
    SC_THREAD(cpu_monitor);    
}

template< int INTR_NUM, int CPU_NUM >
GIC_CPUInterface< INTR_NUM, CPU_NUM >::~GIC_CPUInterface()
{
}
  
 template< int INTR_NUM, int CPU_NUM >   
void GIC_CPUInterface< INTR_NUM, CPU_NUM >::cpu_monitor(void)
{
    unsigned int intr_id, ack, eoi;
    unsigned int address;
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
           if (IPBR->read()[intr_id] == '1' ) 
                flag = true;
            else
                intr_id++;
            
        if (!flag)
            intr_id = GIC_SPURIOUS_INTERRUPT;
        if (flag)	 {
            clearIPBR[intr_id]->clearIntr();
       }    
        /* Activate nIRQ signal*/
        nIRQ->write(0);
        address = GIC_BASE_ADDRESS;
        address += ((GIC_CPUADDR_STEP * cpu_id) + GIC_IAR_OFFSET);
        /* Ack. read */
        ack = intr_id;
        bus_slave_port->serveRead(address,  &ack, sizeof(ack));
        /* Deactivate nIRQ signal*/
        nIRQ->write(1);
        /* EOI write*/
        address = GIC_BASE_ADDRESS;
        address += ((GIC_CPUADDR_STEP * cpu_id) + GIC_EOIR_OFFSET); 
        bus_slave_port->serveWrite(address,  &eoi, sizeof(unsigned int));
#ifdef BUS_TIMED_MODEL        
        sc_core::wait(bus_clock_period, SIM_RESOLUTION);
#endif
        if (eoi != ack)
            std::cout << sc_core::sc_time_stamp() << ": ERROR GIC << ACK != EOI >> \n";
        intr_id = GIC_SPURIOUS_INTERRUPT;	
                   
    }
}

/******************************************************************************/
template< int INTR_NUM = 32, int CPU_NUM = 2 >
class GenericIntrController 
    :public sc_core::sc_module
{
 public:
    sc_core::sc_port< IAmbaAhbBusSlaveMacTlmProt > bus_slave_port;
#ifdef SYSTEMC_2_3_1    
    sc_core::sc_vector< sc_core::sc_port< receive_if > > HINT_tlm;
    sc_core::sc_vector< sc_core::sc_out< bool > > nIRQ;
#else
    sc_core::sc_port< receive_if > HINT_tlm[INTR_NUM];
    sc_core::sc_out< bool > nIRQ[CPU_NUM];
#endif
    GenericIntrController(const sc_core::sc_module_name name, sc_dt::uint64 clock_period = BUS_CLOCK_PERIOD);
    ~GenericIntrController();
	
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
    
    std::vector< GIC_IntrDetect< INTR_NUM, CPU_NUM >* > intrDetect;
    std::vector< GIC_CPUInterface< INTR_NUM, CPU_NUM >* > cpuInterface; 
    
};

template< int INTR_NUM, int CPU_NUM > 
GenericIntrController< INTR_NUM, CPU_NUM >::GenericIntrController(const sc_core::sc_module_name name, sc_dt::uint64 clock_period)
    :sc_core::sc_module(name)
{

    assert (INTR_NUM <= GIC_MAX_SPI);
    assert (CPU_NUM <= GIC_MAX_CPU);
    
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
        GIC_IntrDetect< INTR_NUM, CPU_NUM >* det;
	    det = new GIC_IntrDetect< INTR_NUM, CPU_NUM >(sc_core::sc_gen_unique_name("intr_detect"), intr);
		 /* Port binding */  
		det->detect(HINT_tlm[intr]);
		det->IPTR(IPTR[intr]);
		for (int cpu = 0; cpu< CPU_NUM; cpu++) 
		    det->pending[cpu](IPBR[cpu]); 
        intrDetect.push_back(det);				
    }
			    
    for (int cpu = 0; cpu < CPU_NUM; cpu++ ) {
	    GIC_CPUInterface< INTR_NUM, CPU_NUM >* cpu_if;
        cpu_if = new GIC_CPUInterface< INTR_NUM, CPU_NUM >(sc_core::sc_gen_unique_name("cpu_interface"), cpu, clock_period);
         /* Port binding */  
        cpu_if->bus_slave_port(bus_slave_port);
        cpu_if->IPBR(IPBR[cpu]);
        cpu_if->IER(IER);
        cpu_if->nIRQ(nIRQ[cpu]);
        for (int intr = 0; intr < INTR_NUM; intr++ ) 
            cpu_if->clearIPBR[intr](intrDetect[intr]->clearIPBR);
        
	    cpuInterface.push_back(cpu_if);				
    }
}

template< int INTR_NUM, int CPU_NUM > 		
GenericIntrController< INTR_NUM, CPU_NUM >::~GenericIntrController()
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
void GenericIntrController< INTR_NUM, CPU_NUM >::setIntrTargetCPU(int intrID, int targetCPU)
{
    sc_dt::sc_bv< GIC_MAX_CPU > tmp;
   
   assert ((unsigned int) targetCPU < GIC_MAX_CPU );
   
    for (unsigned int i = 0; i < GIC_MAX_CPU; i++) // clear all bits
        tmp[i] = sc_dt::SC_LOGIC_0;
    tmp[targetCPU] = sc_dt::SC_LOGIC_1;
    IPTR[intrID].write(tmp);
}

} // namespace HCSim

#endif /* SC_GIC__H*/

    
