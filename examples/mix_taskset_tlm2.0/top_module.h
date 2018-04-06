/*********************************************
 * Mix Task Set Example: TLM 2.0
 * Parisa Razaghi, UT Austin, parisa.r@utexas.edu
 * Last update: August 2013
 ********************************************/

#include "MCProcessor_TLM.h"
#include "HInterruptGenerator_TLM.h"
 
 class artificial_example
    :sc_core::sc_module
{
 public:
    artificial_example(const sc_core::sc_module_name name)
        :sc_core::sc_module(name)
        ,busSlaveMacLink_1("busSlaveMacLink_1")
        ,busSlaveMacLink_2("busSlaveMacLink_2")
    {

        //MainBus_HINT.init(CON_INTR_NUM);
        
        MainBus = new  HCSim::simpleBus_TLM<1, 3> ("MainBus");
        
        CPU = new MCProcessor_TLM<CON_INTR_NUM ,CON_CPU_NUM> ("DualCore_CPU", HCSim::OS_INFINIT_VAL);
        CPU->master_initiator_socket(MainBus->master_target_socket[0]);
        for (int intr = 0; intr < CON_INTR_NUM ; intr++)
            CPU->HINTR_tlm[intr](MainBus_HINT[intr]);
       
        MainBus->slave_initiator_socket[0].bind(CPU->slave_target_socket); 
        MainBus->slave_initiator_socket[1](busSlaveMacLink_1.target_socket);
        MainBus->slave_initiator_socket[2](busSlaveMacLink_2.target_socket);
        
        MainBus->setAddressSpace(HCSim::GIC_BASE_ADDRESS, (HCSim::GIC_BASE_ADDRESS + 0x68) , 0);
        MainBus->setAddressSpace(const_intr1_address, const_intr1_task_address, 1);
        MainBus->setAddressSpace(const_intr2_address, const_intr2_task_address, 2);
        
        intr_gen_1 = new HW_Standard_interruptGenerator("intr_gen_1", 1, const_intr1_address, const_intr1_load, const_end_time, 1);  
        intr_gen_1->Port_tlm(busSlaveMacLink_1);
        intr_gen_1->MainBus_HINT(MainBus_HINT[1]);
	     intr_gen_2 = new HW_Standard_interruptGenerator("intr_gen_2", 2, const_intr2_address, const_intr2_load, const_end_time, 1);
	     intr_gen_2->Port_tlm(busSlaveMacLink_2);
	     intr_gen_2->MainBus_HINT(MainBus_HINT[2]);
	    
	     intr_hmonitor_intr1 = new HW_Standard_interruptHandlerMonitor("handler_mon_1", 1, const_intr1_handler_address, const_intr1_load, const_end_time, 1);
	     intr_hmonitor_intr1->Port_tlm(busSlaveMacLink_1);
	     intr_hmonitor_intr2 = new HW_Standard_interruptHandlerMonitor("handler_mon_2", 2, const_intr2_handler_address, const_intr2_load, const_end_time,  1);
        intr_hmonitor_intr2->Port_tlm(busSlaveMacLink_2);
        
        intr_tmonitor_intr1 = new HW_Standard_signalHandlerMonitor("signal_mon_1" ,1,  const_intr1_task_address, const_intr1_load, const_end_time, 1);
        intr_tmonitor_intr1->Port_tlm(busSlaveMacLink_1);
        intr_tmonitor_intr2 = new HW_Standard_signalHandlerMonitor("signal_mon_2", 2,  const_intr2_task_address, const_intr2_load, const_end_time, 1);
        intr_tmonitor_intr2->Port_tlm(busSlaveMacLink_2);
           
    }    
    ~artificial_example()
    {
        delete MainBus;
        delete CPU;
        delete intr_gen_1;
        delete intr_gen_2;
        delete intr_hmonitor_intr1;
        delete intr_hmonitor_intr2;
        delete intr_tmonitor_intr1;
        delete intr_tmonitor_intr2; 
    }
    
 private:
    // Channels 
    //sc_core::sc_vector< HCSim::handshake_ch  > MainBus_HINT;
    HCSim::handshake_ch  MainBus_HINT[CON_INTR_NUM];
    // TLM AMBA AHB bus model
    HCSim::simpleBus_TLM<1, 3>* MainBus;
    // TLM MultiCore Processpr model
    MCProcessor_TLM<CON_INTR_NUM, CON_CPU_NUM>* CPU;
    //Interrupt resources
 
    HCSim::SlaveMacLink2TLMTarget busSlaveMacLink_1;
    HCSim::SlaveMacLink2TLMTarget busSlaveMacLink_2;
    
    HW_Standard_interruptGenerator* intr_gen_1;
	HW_Standard_interruptGenerator* intr_gen_2;
	HW_Standard_interruptHandlerMonitor* intr_hmonitor_intr1;
	HW_Standard_interruptHandlerMonitor* intr_hmonitor_intr2;
	HW_Standard_signalHandlerMonitor* intr_tmonitor_intr1;
	HW_Standard_signalHandlerMonitor* intr_tmonitor_intr2;
	
};    
