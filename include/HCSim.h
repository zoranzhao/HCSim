/*********************************************                                                       
 * Host-Compiled Simulator Library                                                                     
 * Parisa Razaghi, UT Austin, parisa.r@utexas.edu                                                    
 * Last update: Aug. 2013                                                                            
 ********************************************/
 
#ifndef SC_HCSIM__H
#define SC_HCSIM__H

#include "sim_config.h"

#include "OS/PRTOS.h"
#include "Processor/MCProcessor_FM.h"
#include "Processor/GenericIntrController.h"
#include "Channels/handshake_ch.h"
#include "Channels/handshake_os_ch.h"
#include "Channels/ambaAhb.h"
/* TLM2.0 models*/
#include "Processor/GenericIntrController_TLM.h"
#include "Channels/simpleBus_TLM.h"
#include "Channels/adapters_TLM.h"
/*Sabine*/
#include "Channels/sc_mutex_os_if.h"
#include "Channels/sc_mutex_os.h"

extern int total_input_sets;
extern int total_cli_num;
#endif /* SC_HCSIM__H */

