/*********************************************
 * Mix Task Set Example
 * Parisa Razaghi, UT Austin, parisa.r@utexas.edu
 * Last update: July 2013
 ********************************************/

#include "MCProcessor.h"
#include "HInterruptGenerator.h"
 
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
        
        MainBus = new  HCSim::AmbaAhbMacTlm("MainBus");
        
        CPU = new MCProcessor<CON_INTR_NUM ,CON_CPU_NUM> ("DualCore_CPU", HCSim::OS_INFINIT_VAL);
        CPU->MainBus_tlm_master_port(*MainBus);
        CPU->MainBus_tlm_slave_port(*MainBus);

        for (int intr = 0; intr < CON_INTR_NUM ; intr++)
            CPU->HINTR_tlm[intr](MainBus_HINT[intr]);
        
	busSlaveMacLink_1.protWMac(*MainBus);
        busSlaveMacLink_2.protWMac(*MainBus);
        
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
        delete CPU;
        delete MainBus;
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
    HCSim::handshake_ch MainBus_HINT[CON_INTR_NUM];
    // TLM AMBA AHB bus model
    HCSim::AmbaAhbMacTlm* MainBus;
    // TLM MultiCore Processpr model
    MCProcessor<CON_INTR_NUM, CON_CPU_NUM>* CPU;
    //Interrupt resources
 
    HCSim::AmbaAhbBusSlaveMacLinkPass busSlaveMacLink_1;
    HCSim::AmbaAhbBusSlaveMacLinkPass busSlaveMacLink_2;
    HW_Standard_interruptGenerator* intr_gen_1;
	HW_Standard_interruptGenerator* intr_gen_2;
	HW_Standard_interruptHandlerMonitor* intr_hmonitor_intr1;
	HW_Standard_interruptHandlerMonitor* intr_hmonitor_intr2;
	HW_Standard_signalHandlerMonitor* intr_tmonitor_intr1;
	HW_Standard_signalHandlerMonitor* intr_tmonitor_intr2;
};    
