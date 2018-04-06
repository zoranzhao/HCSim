/*********************************************    
 * Parisa Razaghi, UT Austin <parisa.r@utexas.edu>
 * Last update: Jun. 2013 
 ********************************************/
#include <systemc>
#include <stdint.h>

#ifndef SC_GLOBAL_GIC__H
#define SC_GLOBAL_GIC__H

namespace HCSim {

/*--------------------------------------------------------------------- */
/* general global constants                                             */
/*--------------------------------------------------------------------- */

const unsigned int  GIC_MAX_CPU = 64;
const unsigned int  GIC_MAX_SPI = 32;
const unsigned int  GIC_SPURIOUS_INTERRUPT = 1023;
const unsigned int  GIC_PRIORITY_HIGH = 0;
const unsigned int  GIC_PRIORITY_LOW = 	255;
const unsigned int  GIC_INTERRUPT_COUNT = 16;
const unsigned int  GIC_SUPPORTED_CPU = GIC_MAX_CPU;

const unsigned int  GIC_BASE_ADDRESS  = 15728640;
const unsigned int  GIC_CPUADDR_STEP = 0x8;
const unsigned int  GIC_IAR_OFFSET = 0x0;
const unsigned int  GIC_EOIR_OFFSET = 0x4; 
//|--------|--------|--------|--------|
//| IAR0  | EOI0 | IAR1  | EOI1  |
//|--------|--------|--------|--------|

} // HCSim

#endif // SC_GLOBAL_GIC__H


