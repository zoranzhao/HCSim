/******************************************************************************
 * Multi-Core Processor TLM 2.0 model
 * Parisa Razaghi, UT Austin, parisa.r@utexas.edu
 * Last update: August 2013
 ******************************************************************************/

#include <systemc>
#include <tlm.h>

#include "HCSim.h"

#include "config.h"
#include "mix_task_set.h"

#ifndef SC_MCPROCESSOR_TLM__H
#define SC_MCPROCESSOR_TLM__H

/******************************************************************************
 * Application-specific interrupt handling interface
 *
 ******************************************************************************/
class IntHandler_if
    :virtual public sc_core::sc_interface
{
 public:
    virtual void intHandler(void) = 0;
    virtual void intHandler__HINT1(int coreID) = 0;
    virtual void intHandler__HINT2(int coreID) = 0;
};

/******************************************************************************
 * Application-specific interrupt task model
 *
 ******************************************************************************/
class IntrTask_HINT
    :public sc_core::sc_module
    ,virtual public HCSim::OS_TASK_INIT
    ,virtual public HCSim::IntrTrigger_if
{
 public:
    /*---------------------------------------------------------
       OS & Intr. interface
     ----------------------------------------------------------*/ 
    sc_core::sc_port< HCSim::OSAPI > os_port;
    sc_core::sc_port< HCSim::send_os_if > intr_ch;
    /*---------------------------------------------------------
       User-defined communication interface
       >> Put application-specific interface here... 
     ----------------------------------------------------------*/    
    sc_core::sc_port< HCSim::send_os_if > data_ch;
    /*--------------------------------------------------------*/

    SC_HAS_PROCESS(IntrTask_HINT);
    IntrTask_HINT(const sc_core::sc_module_name name, 
                            int intr_id,
                            int target_cpu, 
                            unsigned int priority, 
                            sc_dt::uint64 exe_cost)
        :sc_core::sc_module(name)
    {
        this->target_cpu = target_cpu;
        this->priority = priority;
        this->intr_id = intr_id;
        this->exe_cost = exe_cost;
        
        SC_THREAD(monitor);
    }    
    ~IntrTask_HINT() {}
    /*---------------------------------------------------------
       OS_TASK_INIT interface method
       >> Creates interrupt-task (i.e. Signal Handler) 
     ----------------------------------------------------------*/ 
    void OSTaskCreate(void) {   
        /*----------------------------------------------------------------------------------------------------------------------
	    os_task_id = os_port->taskCreate("intTask", HCSim::OS_INTR_TASK, priority, HCSim::OS_INFINIT_VAL, 
                                                         exe_cost, HCSim::DEFAULT_TS, HCSim::ALL_CORES, target_cpu);
            -------------------------------------------------- OR ------------------------------------------------------------*/                                             
	  os_task_id = os_port->createIntrTask("intTask", priority, HCSim::ALL_CORES, target_cpu, target_cpu);

    }
    /*---------------------------------------------------------
       IInitTask interface method
       >> Triggers the interrupt-task 
     ----------------------------------------------------------*/
    void start(int coreID) {  
		os_port->intrTrigger(os_task_id, coreID);
    }
                        
 private:
	int os_task_id;
	int target_cpu;
	int intr_id;
	unsigned int priority;
	sc_dt::uint64 exe_cost;
	
	void intr_body(void) {   
		os_port->timeWait(exe_cost, os_task_id);
		os_port->syncGlobalTime(os_task_id);
		data_ch->send(os_task_id);
		intr_ch->send(os_task_id); /* Notifys the interrupt event */
	}

    void monitor(void) {  
	    os_port->taskActivate(os_task_id);
        while(1) {   
	        intr_body();
	        os_port->intrSleep(os_task_id);  
	    }
    }
};

/******************************************************************************
 * User-defined adapter for Mac Link Master Port
 *
 ******************************************************************************/
class MacLink_Data_Out_Adapter
    :sc_core::sc_module
    ,virtual public HCSim::send_os_if 
{
 public:   
    /*---------------------------------------------------------
       OS/HAL interface
     ----------------------------------------------------------*/ 
    sc_core::sc_port< HCSim::IAmbaAhbBusMasterMacLink > mac_link_port;
    
    SC_HAS_PROCESS(MacLink_Data_Out_Adapter);
    MacLink_Data_Out_Adapter(const sc_core::sc_module_name name, unsigned long long addr)
        :sc_core::sc_module(name)
    {
        this->addr = addr;
    }
    ~MacLink_Data_Out_Adapter() {}
    /*---------------------------------------------------------
       send_os_if  interface method
        >> translates the high-level send method to 
             the MacLink interface methods
     ----------------------------------------------------------*/     
    void send(int task_id) {   
        unsigned char tmp[1];
        mac_link_port->masterWrite(addr, tmp, 4ull);
    }
    
 private:
    unsigned long long addr;    
};

/******************************************************************************
 * User-defined adapter for Mac Mem Master Port
 *
 ******************************************************************************/
class MacMem_Data_Out_Adapter
    :sc_core::sc_module
    ,virtual public HCSim::send_os_if 
{
 public:   
    /*---------------------------------------------------------
       OS/HAL interface
     ----------------------------------------------------------*/ 
    sc_core::sc_port< HCSim::IAmbaAhbBusMasterMacMem > mac_mem_port;
    
    SC_HAS_PROCESS(MacMem_Data_Out_Adapter);
    MacMem_Data_Out_Adapter(const sc_core::sc_module_name name, unsigned long long addr)
        :sc_core::sc_module(name)
    {
        this->addr = addr;
    }
    ~MacMem_Data_Out_Adapter() {}
    /*---------------------------------------------------------
       send_os_if  interface method
        >> translates the high-level send method to 
             the MacLink interface methods
     ----------------------------------------------------------*/     
    void send(int task_id) {   
    
        unsigned char tmp[1];
        mac_mem_port->masterMemWrite(addr, tmp, 4ull);
	}
    
 private:
    unsigned long long addr;    
};
/******************************************************************************
 * Processor OS Layer
 *
 ******************************************************************************/
template< int INTR_NUM, int CPU_NUM >
class MCProcessor_OS
    :public sc_core::sc_module
    ,virtual public IntHandler_if
{
 public:
    /*---------------------------------------------------------
       OS/HAL interface
       >> MAC LINK/MEM Master Interface
     ----------------------------------------------------------*/  
    //sc_core::sc_vector< sc_core::sc_port< HCSim::IAmbaAhbBusMasterMacLink > > mac_link_port;
    //sc_core::sc_vector< sc_core::sc_port< HCSim::IAmbaAhbBusMasterMacMem > > mac_mem_port;
    sc_core::sc_port< HCSim::IAmbaAhbBusMasterMacLink >  mac_link_port[CPU_NUM];
    sc_core::sc_port< HCSim::IAmbaAhbBusMasterMacMem >  mac_mem_port[CPU_NUM];
    /*---------------------------------------------------------
       Export OS interface to the HAL
     ----------------------------------------------------------*/
    sc_core::sc_export< HCSim::OSAPI > os_export;
    /*---------------------------------------------------------
       Export user-defined Intr. interface to the HAL
     ----------------------------------------------------------*/
    sc_core::sc_export< IntHandler_if > intr_export; 
   
    SC_HAS_PROCESS(MCProcessor_OS);
    MCProcessor_OS(const sc_core::sc_module_name name,
                                sc_dt::uint64 simulation_quantum);  
    ~MCProcessor_OS(); 
    /*---------------------------------------------------------
       IIntrHandler interface methods
     ----------------------------------------------------------*/    
    void intHandler(void);
    void intHandler__HINT1(int coreID);
    void intHandler__HINT2(int coreID);
  
 private:
    /*---------------------------------------------------------
       OS Channel
     ----------------------------------------------------------*/  
    HCSim::RTOS* OS;
    /*---------------------------------------------------------
       User-defined application and interrupt tasks
     ----------------------------------------------------------*/     
    Mix_Task_Set* CPU_APP;
    IntrTask_HINT* intr_task_1;
    IntrTask_HINT* intr_task_2;
    /*---------------------------------------------------------
       Application channels and adapters
     ----------------------------------------------------------*/ 
    HCSim::handshake_os_ch flag_intr_1;
    HCSim::handshake_os_ch flag_intr_2;
    MacLink_Data_Out_Adapter intr_data_out_1;
    MacLink_Data_Out_Adapter intr_data_out_2;
    MacLink_Data_Out_Adapter app_data_out_1;
    MacLink_Data_Out_Adapter app_data_out_2;
    /*--------------------------------------------------------*/      
    
    void end_of_elaboration();    
    void start_of_simulation();
};

template< int INTR_NUM, int CPU_NUM >
MCProcessor_OS< INTR_NUM, CPU_NUM >::MCProcessor_OS(const sc_core::sc_module_name name, 
					       sc_dt::uint64 simulation_quantum)
    :sc_core::sc_module(name)
    ,flag_intr_1("intr_1")
    ,flag_intr_2("intr_2")
    ,intr_data_out_1("intr_data_out_1", const_intr1_task_address)
    ,intr_data_out_2("intr_data_out_2", const_intr2_task_address)
    ,app_data_out_1("app_data_out_1", const_intr1_address)
    ,app_data_out_2("app_data_out_2", const_intr2_address)
{
    //mac_link_port.init(CPU_NUM);
    //mac_mem_port.init(CPU_NUM);

    intr_export(*this);
      
    OS = new HCSim::RTOS(sc_core::sc_gen_unique_name("RTOS"));
    OS->init(CPU_NUM, simulation_quantum);
    os_export(*OS);
    
    flag_intr_1.os_port(*OS);   
    flag_intr_2.os_port(*OS);   
    
    intr_task_1 = new IntrTask_HINT("intr_task_1", 0, 0, const_intr1_priority, SHANDLER_DELAY);
    intr_task_1->os_port(*OS);
    intr_task_1->intr_ch(flag_intr_1);
    intr_task_1->data_ch(intr_data_out_1);
    
    intr_task_2 = new IntrTask_HINT("intr_task_2", 0, 1, const_intr2_priority, SHANDLER_DELAY);
    intr_task_2->os_port(*OS);
    intr_task_2->intr_ch(flag_intr_2);
    intr_task_2->data_ch(intr_data_out_2);
    
    CPU_APP = new Mix_Task_Set("Application");
    CPU_APP->os_port(*OS);
    CPU_APP->intrCH[0](flag_intr_1);
    CPU_APP->dataCH[0](app_data_out_1);
    CPU_APP->intrCH[1](flag_intr_2);
    CPU_APP->dataCH[1](app_data_out_2);    
    
    intr_data_out_1.mac_link_port(mac_link_port[0]);
    intr_data_out_2.mac_link_port(mac_link_port[1]);
    app_data_out_1.mac_link_port(mac_link_port[0]);
    app_data_out_2.mac_link_port(mac_link_port[1]);  
}

template< int INTR_NUM, int CPU_NUM >
MCProcessor_OS< INTR_NUM, CPU_NUM >::~MCProcessor_OS()
{
    delete intr_task_1;
    delete intr_task_2; 
    delete CPU_APP;
    delete OS;
}

template< int INTR_NUM, int CPU_NUM >
void MCProcessor_OS< INTR_NUM, CPU_NUM >::end_of_elaboration()
{
    /*---------------------------------------------------------
       Create user-define interrupt tasks 
     ----------------------------------------------------------*/ 
    intr_task_1->OSTaskCreate();
    intr_task_2->OSTaskCreate();
}

template< int INTR_NUM, int CPU_NUM >
void MCProcessor_OS< INTR_NUM, CPU_NUM >::start_of_simulation()
{
    OS->start();
}

template< int INTR_NUM, int CPU_NUM >
void MCProcessor_OS< INTR_NUM, CPU_NUM >::intHandler(void)
{   
}

template< int INTR_NUM, int CPU_NUM >
void MCProcessor_OS< INTR_NUM, CPU_NUM >::intHandler__HINT1(int coreID)
{  
    intr_task_1->start(coreID);
}

template< int INTR_NUM, int CPU_NUM >
void MCProcessor_OS< INTR_NUM, CPU_NUM >::intHandler__HINT2(int coreID)
{ 
    intr_task_2->start(coreID);
}
   
/******************************************************************************
 * Interrupt Handler Model
 *
 ******************************************************************************/
class Interrupt_Handler
    :public sc_core::sc_module
    ,virtual public HCSim::OS_TASK_INIT
    ,virtual public HCSim::HAL_if
{
 public:
     /*---------------------------------------------------------
        HW interface
       >> MAC LINK Master Interface
     ----------------------------------------------------------*/ 
    sc_core::sc_port< HCSim::IAmbaAhbBusMasterMacLink > mac_link_port;
    /*---------------------------------------------------------
       OS interface 
     ----------------------------------------------------------*/    
    sc_core::sc_port< HCSim::OSAPI> os_port;
    /*---------------------------------------------------------
       User-defined interrupt task interface 
     ----------------------------------------------------------*/  
    sc_core::sc_port< IntHandler_if > intr_port;
    sc_core::sc_export< HCSim::HAL_if > hal_export;

    SC_HAS_PROCESS(Interrupt_Handler);
    Interrupt_Handler(const sc_core::sc_module_name name, int coreID);
    ~Interrupt_Handler();
    
    /*---------------------------------------------------------
       OS_TASK_INIT interface method
       >> Creates interrupt-handler (called by HAL) 
     ----------------------------------------------------------*/ 
    void OSTaskCreate(void) ; 
    void IRQEnter(int coreID); 
      
 private:
    int intr_handler_id;
    int core_id;
    
    void intr_handler(void);
};

Interrupt_Handler::Interrupt_Handler(const sc_core::sc_module_name name, int coreID)
    :sc_core::sc_module(name)
    ,core_id(coreID)
{  
    hal_export(*this);
    
    SC_THREAD(intr_handler);
}

Interrupt_Handler::~Interrupt_Handler()
{
}

void Interrupt_Handler::OSTaskCreate(void) 
{   
    intr_handler_id = os_port->createIntrHandler(core_id, 0);
}

void Interrupt_Handler::IRQEnter(int coreID) 
{   
    assert (coreID == core_id);
    os_port->iEnter(core_id, intr_handler_id);
}

void Interrupt_Handler::intr_handler(void)
{
    unsigned int INTSRC;
    unsigned long int addrPic;
    sc_dt::uint64 delay;
    
   while(1) {
           
        os_port->taskActivate(intr_handler_id);
#ifdef INTR_TRACE_ON             
            printf("%llu: C%d Interrupt handler HAL layer start. \n", sc_core::sc_time_stamp().value(), core_id);
#endif
        delay =  (sc_dt::uint64) IHANDLER_INSTR_1 * CLOCK_PERIOD;
        sc_core::wait(delay, SIM_RESOLUTION); 
         /*---------------------------------------------------------
            Reads interrupt source ID 
         ----------------------------------------------------------*/ 	    	
        addrPic = (unsigned long int)(HCSim::GIC_BASE_ADDRESS + (HCSim::GIC_CPUADDR_STEP * core_id) + HCSim::GIC_IAR_OFFSET);
        mac_link_port->masterRead(addrPic,  &INTSRC, sizeof(INTSRC));
            
#ifdef INTR_TRACE_ON 
			printf("%llu: C%d- Interrupt handler HAL layer -- source %d. \n", sc_core::sc_time_stamp().value(), core_id, INTSRC);
#endif	
		/*---------------------------------------------------------
            Triggers interrupt tasks 
         ----------------------------------------------------------*/ 	
        if (INTSRC != HCSim::GIC_SPURIOUS_INTERRUPT) {
            switch(INTSRC) {
	    	    case 0:
	    	       intr_port->intHandler();
             break;
                case 1:
				  	     addrPic = const_intr1_handler_address;
                    intr_port->intHandler__HINT1(core_id);
                    break;
                case 2:
				        addrPic = const_intr2_handler_address;

                    intr_port->intHandler__HINT2(core_id);
                    break;
               default:
                   intr_port->intHandler();
                    break;
            }
            /* To measure interrupt handler response time (in mix_task_set example) */
            /* Note: needs to be removed in other examples... */
            mac_link_port->masterWrite(addrPic,  &INTSRC, sizeof(INTSRC));
            delay = (sc_dt::uint64)IHANDLER_INSTR_2 * CLOCK_PERIOD;
            sc_core::wait(delay , SIM_RESOLUTION); 
            /*---------------------------------------------------------
                Writes End-Of-Interrupt register
            ----------------------------------------------------------*/   	
            addrPic = (unsigned long int)(HCSim::GIC_BASE_ADDRESS + (HCSim::GIC_CPUADDR_STEP * core_id) + HCSim::GIC_EOIR_OFFSET);
            mac_link_port->masterWrite(addrPic,  &INTSRC, sizeof(INTSRC));
	         
        }
        os_port->iReturn(core_id);
#ifdef INTR_TRACE_ON            
           printf("%llu: C%d- Interrupt handler HAL layer end on source %d. \n", sc_core::sc_time_stamp().value(), core_id, INTSRC);
#endif           
    }
}  
  
/******************************************************************************
 * Processor HAL Layer
 *
 ******************************************************************************/
template< int INTR_NUM, int CPU_NUM >
class MCProcessor_HAL_TLM
    :public sc_core::sc_module
    ,virtual public HCSim::HAL_if
{
 public:
      /*---------------------------------------------------------
       HAL/HW interface
       >> TLM2.0 Master Interface
     ----------------------------------------------------------*/ 
    //sc_core::sc_vector< tlm::tlm_initiator_socket<> > master_initiator_socket;
    tlm::tlm_initiator_socket<> master_initiator_socket[CPU_NUM];
    /*---------------------------------------------------------
       Export OS interface to the HW layer
     ----------------------------------------------------------*/
    sc_core::sc_export< HCSim::HAL_if>  hal_export;
    MCProcessor_HAL_TLM(const sc_core::sc_module_name name, 
                                  sc_dt::uint64 simulation_quantum);
    ~MCProcessor_HAL_TLM();
    
    void end_of_elaboration();
    void IRQEnter(int coreID);

 private:
    /*---------------------------------------------------------
       CPU OS LAYER
     ----------------------------------------------------------*/  
    MCProcessor_OS< INTR_NUM, CPU_NUM >* CPU_OS;
    /*---------------------------------------------------------
       CPU Interrupt Handlers
     ----------------------------------------------------------*/     
    std::vector< Interrupt_Handler* > Intr_Handler;
    /*---------------------------------------------------------
       MAC-(Link,Mem) / MAC-TLM Transactors
     ----------------------------------------------------------*/     
    //sc_core::sc_vector< HCSim::MasterMacLink2TLMInitiator > mac_link2tlm;
    //sc_core::sc_vector< HCSim::MasterMacMem2TLMInitiator > mac_mem2tlm;
    //sc_core::sc_vector< HCSim::TLMTarget2Initiator_Transducer > socket_transducer;
    HCSim::MasterMacLink2TLMInitiator mac_link2tlm[CPU_NUM];
    HCSim::MasterMacMem2TLMInitiator mac_mem2tlm[CPU_NUM];
    HCSim::TLMTarget2Initiator_Transducer socket_transducer[CPU_NUM];
};

template< int INTR_NUM, int CPU_NUM >
MCProcessor_HAL_TLM< INTR_NUM, CPU_NUM >::MCProcessor_HAL_TLM(const sc_core::sc_module_name name,
						sc_dt::uint64 simulation_quantum)
    :sc_core::sc_module(name)
   // ,master_initiator_socket("master_init_socket", CPU_NUM)    
    //,mac_link2tlm("mac_link2tlm", CPU_NUM)
    //,mac_mem2tlm("mac_mem2tlm", CPU_NUM)
    //,socket_transducer("socket_transducer", CPU_NUM)
{
  	hal_export(*this);
  	
  	std::stringstream module_name;
    module_name << name << "_OS";
	CPU_OS = new MCProcessor_OS< INTR_NUM, CPU_NUM >(module_name.str().c_str(), simulation_quantum);
    for (int cpu = 0; cpu < CPU_NUM; cpu++) {
    	CPU_OS->mac_link_port[cpu](mac_link2tlm[cpu]);
    	CPU_OS->mac_mem_port[cpu](mac_mem2tlm[cpu]);
    }
		
	for (int cpu = 0; cpu < CPU_NUM; cpu++) {
	    Interrupt_Handler* intr_h;
	    intr_h = new Interrupt_Handler(sc_core::sc_gen_unique_name("intr_handler"), cpu);
	    intr_h->mac_link_port(mac_link2tlm[cpu]);
	    intr_h->os_port(CPU_OS->os_export);
	    intr_h->intr_port(CPU_OS->intr_export);
	    Intr_Handler.push_back(intr_h);
	}
	
    for (int cpu = 0; cpu < CPU_NUM; cpu++) {
	    mac_link2tlm[cpu].initiator_socket.bind(socket_transducer[cpu].target_socket);
       mac_mem2tlm[cpu].initiator_socket.bind(socket_transducer[cpu].target_socket);
    }
    
    for (int cpu = 0; cpu < CPU_NUM; cpu++) {
    	socket_transducer[cpu].initiator_socket.bind(master_initiator_socket[cpu]);
    }	
}

template< int INTR_NUM, int CPU_NUM >
MCProcessor_HAL_TLM< INTR_NUM, CPU_NUM >::~MCProcessor_HAL_TLM()
{
    delete CPU_OS;
    for(std::vector< Interrupt_Handler* >::iterator iter =  Intr_Handler.begin();
          iter !=  Intr_Handler.end(); ++iter) {
        delete *iter;
    }
    Intr_Handler.clear(); 
}

template< int INTR_NUM, int CPU_NUM >
void MCProcessor_HAL_TLM< INTR_NUM, CPU_NUM >::end_of_elaboration()
{
    for (int cpu = 0; cpu < CPU_NUM; cpu++) 
	    Intr_Handler[cpu]->OSTaskCreate();
}

template< int INTR_NUM, int CPU_NUM >
void MCProcessor_HAL_TLM< INTR_NUM, CPU_NUM >::IRQEnter(int coreID) 
{   
    Intr_Handler[coreID]->IRQEnter(coreID);
}

/******************************************************************************
 * CPU Interrupt Interface
 *
 ******************************************************************************/
class Interrupt_Interface
    :public sc_core::sc_module
{
 public:
    sc_core::sc_in< bool > nIRQ;
    sc_core::sc_port< HCSim::HAL_if > hal_port;
     
    SC_HAS_PROCESS(Interrupt_Interface); 
    Interrupt_Interface(const sc_core::sc_module_name name, int core_id)
        :sc_core::sc_module(name)
    {
        this->core_id = core_id;
        
        SC_THREAD(intr_monitor);
    }      
    ~Interrupt_Interface() {}
 
 private:
    int core_id;
    
    void intr_monitor(void)
    {
        while (1) {       
            while (nIRQ->read() == 1)
                sc_core::wait(nIRQ->value_changed_event());
            hal_port->IRQEnter(core_id);
            sc_core::wait(nIRQ->posedge_event());
        }
    }
};

/******************************************************************************
 * Processor HW Layer
 * 
 ******************************************************************************/
template< int INTR_NUM, int CPU_NUM >
class MCProcessor_HW_TLM
    :public sc_core::sc_module
{
 public:
    /*---------------------------------------------------------
       HW/TLM interface
       >> TLM2.0 Master Interface
     ----------------------------------------------------------*/
    //sc_core::sc_vector< tlm::tlm_initiator_socket<> > master_initiator_socket;
    tlm::tlm_initiator_socket<>  master_initiator_socket[CPU_NUM];
    /*---------------------------------------------------------
       nIRQ signals
     ----------------------------------------------------------*/    
    //sc_core::sc_vector< sc_core::sc_in< bool > > nIRQ;
    sc_core::sc_in< bool > nIRQ[CPU_NUM];    
    
    MCProcessor_HW_TLM(const sc_core::sc_module_name name,
		    sc_dt::uint64 simulation_quantum);
    ~MCProcessor_HW_TLM();

private:
    /*---------------------------------------------------------
       CPU HAL LAYER
     ----------------------------------------------------------*/ 
    MCProcessor_HAL_TLM< INTR_NUM, CPU_NUM >* CPU_HAL;
    /*---------------------------------------------------------
       CPU Interrupt Interfaces
     ----------------------------------------------------------*/     
    std::vector< Interrupt_Interface* > CPU_Interrupt_Interface;

};

template< int INTR_NUM, int CPU_NUM >
MCProcessor_HW_TLM< INTR_NUM, CPU_NUM >::MCProcessor_HW_TLM(const sc_core::sc_module_name name,
					      sc_dt::uint64 simulation_quantum)
   :sc_core::sc_module(name)
{
    //master_initiator_socket.init(CPU_NUM);
   // nIRQ.init(CPU_NUM);
  
  	std::stringstream module_name;
    module_name << name << "_HAL";    
    CPU_HAL = new MCProcessor_HAL_TLM< INTR_NUM, CPU_NUM >(module_name.str().c_str(), simulation_quantum);
    for (int cpu = 0; cpu < CPU_NUM; cpu++)   
       CPU_HAL->master_initiator_socket[cpu](master_initiator_socket[cpu]);
     
    for (int cpu = 0; cpu < CPU_NUM; cpu++)  {
        Interrupt_Interface* cpu_if;
        cpu_if = new Interrupt_Interface(sc_core::sc_gen_unique_name("Interrupt_Interface"), cpu);
        cpu_if->nIRQ(nIRQ[cpu]); 
        cpu_if->hal_port(CPU_HAL->hal_export);
        CPU_Interrupt_Interface.push_back(cpu_if);
    }     
}

template< int INTR_NUM, int CPU_NUM >
MCProcessor_HW_TLM< INTR_NUM, CPU_NUM >::~MCProcessor_HW_TLM()
{
    delete CPU_HAL;
    for(std::vector< Interrupt_Interface* >::iterator iter =  CPU_Interrupt_Interface.begin();
          iter !=  CPU_Interrupt_Interface.end(); ++iter) {
        delete *iter;
    }
    CPU_Interrupt_Interface.clear(); 
}

/******************************************************************************
 *    Processor TLM Layer << Processor + Interrupt Controller >>
 *   >> interfaces:: Bus master/slave ports + hardware interrupt signals <<
 ******************************************************************************/
template< int INTR_NUM, int CPU_NUM >
class MCProcessor_TLM
	:public sc_core::sc_module
{
 public:
    /*---------------------------------------------------------
       Hardware interface
       >>  TLM2.0  Master/Slave Interface
     ----------------------------------------------------------*/
    tlm::tlm_initiator_socket<> master_initiator_socket; 
    tlm::tlm_target_socket<>  slave_target_socket;
    /*---------------------------------------------------------
       Interrupt interface
       >> Hardware interrupts 
     ----------------------------------------------------------*/    
	//sc_core::sc_vector< sc_core::sc_port< HCSim::receive_if > > HINTR_tlm;
	sc_core::sc_port< HCSim::receive_if >  HINTR_tlm[INTR_NUM];
    MCProcessor_TLM(const sc_core::sc_module_name name, 
                                  sc_dt::uint64 simulation_quantum);
    ~MCProcessor_TLM();

 private:
    /*---------------------------------------------------------
       Interrupt Controller
     ----------------------------------------------------------*/ 
     HCSim::GenericIntrController_TLM< INTR_NUM, CPU_NUM >* GIC;
    /*---------------------------------------------------------
       CPU HW LAYER
     ----------------------------------------------------------*/ 
    MCProcessor_HW_TLM< INTR_NUM, CPU_NUM >* CPU_HW;
    /*---------------------------------------------------------
       Internal communication channels
       >> Master port wrapper / nIRQ signals
     ----------------------------------------------------------*/    
    HCSim::TLMInitiatorSocket_Master_Wrap<CPU_NUM> master_socket_wrapper;
    sc_core::sc_signal< bool > nIRQ[CPU_NUM];
};

template< int INTR_NUM, int CPU_NUM >
MCProcessor_TLM< INTR_NUM, CPU_NUM >::MCProcessor_TLM(const sc_core::sc_module_name name,
				 sc_dt::uint64 simulation_quantum)
   :sc_core::sc_module(name)
   ,master_socket_wrapper("tlm_master_socket_wrapper")
{
    //HINTR_tlm.init(INTR_NUM);
    
   	GIC = new HCSim::GenericIntrController_TLM<INTR_NUM, CPU_NUM> (sc_core::sc_gen_unique_name("GIC")) ;
    for (int intr = 0; intr < INTR_NUM; intr++) 
        GIC->HINT_tlm[intr](HINTR_tlm[intr]);   
    for (int cpu = 0; cpu < CPU_NUM; cpu++)      
        (GIC->nIRQ[cpu])(nIRQ[cpu]);
    /* Set interrupt target CPU IDs */
    GIC->setIntrTargetCPU(1, 0);
    GIC->setIntrTargetCPU(2, 1);    

    slave_target_socket(GIC->slave_target_socket);
             	
  	std::stringstream module_name;
    module_name << name << "_HW";
  	CPU_HW = new MCProcessor_HW_TLM< INTR_NUM, CPU_NUM >(module_name.str().c_str(), simulation_quantum);
  	for (int cpu = 0; cpu < CPU_NUM; cpu++) {
        CPU_HW->master_initiator_socket[cpu].bind(master_socket_wrapper.target_socket[cpu]);
        CPU_HW->nIRQ[cpu](nIRQ[cpu]);
    }  
    
    master_socket_wrapper.initiator_socket(master_initiator_socket);
}

template< int INTR_NUM, int CPU_NUM >
MCProcessor_TLM< INTR_NUM, CPU_NUM >::~MCProcessor_TLM()
{
    delete CPU_HW;
    delete GIC;
}

#endif //SC_MCPROCESSOR__H
