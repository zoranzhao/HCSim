/*********************************************
 * Multi-Core Processor TLM - designed for Fast Modeling (FM)
 * Parisa Razaghi, UT Austin, parisa.r@utexas.edu
 * Last update: June 2013
 ********************************************/

#include <systemc>
#include <string.h>

#include "sim_config.h" 
#include "OS/PRTOS.h"

#ifndef SC_MCPROCESSOR_FM__H
#define SC_MCPROCESSOR_FM__H

namespace HCSim {

/*........................*/
/* MCProcessor - OS Level */
/*........................*/
template< class Application > 
class MCProcessor_OS_FM
  :public sc_core::sc_module
{
 public:
      
    MCProcessor_OS_FM(const sc_core::sc_module_name name,
		 uint8_t core_number,
		 sc_dt::uint64 simulation_quantum);
    ~MCProcessor_OS_FM();
  
     void start_of_simulation();
     
 private:
    RTOS* OS;
    Application* CPU_APP;
      
   };
/*........................*/
template< class Application >
MCProcessor_OS_FM< Application >::MCProcessor_OS_FM(const sc_core::sc_module_name name, 
					       uint8_t core_number, 
					       sc_dt::uint64 simulation_quantum)
  :sc_core::sc_module(name)
{
    OS = new RTOS(sc_core::sc_gen_unique_name("RTOS"));
    OS->init(core_number, simulation_quantum);
  
    std::stringstream module_name_1;
    module_name_1 << name << "_APP";
    CPU_APP = new Application (module_name_1.str().c_str());
    CPU_APP->os(*OS);
}

template< class Application >
MCProcessor_OS_FM< Application >::~MCProcessor_OS_FM()
{
    delete CPU_APP;
    delete OS;
}

template< class Application >
void MCProcessor_OS_FM< Application >::start_of_simulation()
{
    OS->start();
}

/*.........................*/
/* MCProcessor - HAL Level */
/*.........................*/
template< class Application >
class MCProcessor_HAL_FM
  :public sc_core::sc_module
{
 public:

    MCProcessor_HAL_FM(const sc_core::sc_module_name name, 
		  uint8_t core_number,
          sc_dt::uint64 simulation_quantum);
    ~MCProcessor_HAL_FM();

 private:
    MCProcessor_OS_FM< Application >* CPU_OS;
};
/*.........................*/
template< class Application >
MCProcessor_HAL_FM< Application >::MCProcessor_HAL_FM(const sc_core::sc_module_name name,
						uint8_t core_number,
						sc_dt::uint64 simulation_quantum)
  :sc_core::sc_module(name)
{
  	std::stringstream module_name;
    module_name << name << "_OS";
    CPU_OS = new MCProcessor_OS_FM< Application >(module_name.str().c_str(), core_number, simulation_quantum);
}

template< class Application >
MCProcessor_HAL_FM< Application >::~MCProcessor_HAL_FM()
{
    delete CPU_OS;
}

/*........................*/
/* MCProcessor - HW Level */
/*........................*/
template< class Application >
class MCProcessor_HW_FM
    :public sc_core::sc_module
{
 public:
    MCProcessor_HW_FM(const sc_core::sc_module_name name,
		 uint8_t core_numeber,
		 sc_dt::uint64 simulation_quantum);
    ~MCProcessor_HW_FM();

private:
    MCProcessor_HAL_FM< Application >* CPU_HAL; 
};
/*........................*/
template< class Application >
MCProcessor_HW_FM< Application >::MCProcessor_HW_FM(const sc_core::sc_module_name name,
					      uint8_t core_number,
					      sc_dt::uint64 simulation_quantum)
   :sc_core::sc_module(name)
{
 	std::stringstream module_name;
    module_name << name << "_HAL";
    CPU_HAL = new MCProcessor_HAL_FM< Application >(module_name.str().c_str(), core_number, simulation_quantum);
}

template< class Application >
MCProcessor_HW_FM< Application >::~MCProcessor_HW_FM()
{
    delete CPU_HAL;
}

/*................................*/
/* Multi-Core Processor TLM model */
/*................................*/
template< class Application >
class MCProcessor_FM
  :public sc_core::sc_module
{
 public:
    MCProcessor_FM(const sc_core::sc_module_name name, 
		  uint8_t core_number, 
		  sc_dt::uint64 simulation_quantum);
    ~MCProcessor_FM();

 private:
    // CPU HW level
    MCProcessor_HW_FM< Application > * CPU_HW;
    uint8_t core_number;
};
/*................................*/
template< class Application >
MCProcessor_FM< Application >::MCProcessor_FM(const sc_core::sc_module_name name,
				 uint8_t core_number, 
				 sc_dt::uint64 simulation_quantum)
   :sc_core::sc_module(name)
{
    this->core_number = core_number;
    
    std::stringstream module_name;
    module_name << name << "_HW";
    CPU_HW = new MCProcessor_HW_FM< Application >(module_name.str().c_str(), core_number, simulation_quantum);
}

template< class Application >
MCProcessor_FM< Application >::~MCProcessor_FM()
{
    delete CPU_HW;
}

} //HCSim

#endif //SC_MCPROCESSOR_TLM_FM__H
