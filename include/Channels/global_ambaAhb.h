
#ifndef GLOBAL_AMBAAHB__H
#define GLOBAL_AMBAAHB__H

namespace HCSim {
/*****************************************************************************/
/*                   Common Definitions                                      */
/*****************************************************************************/
/* Preprocessor directives common for all AMBA AHB bus 
   implementations.
   NOTE: Interfaces are declared where implemented, 
   common interfaces are declared in i_ambaABHbus.sc */


/* assume little endian processor for byte swapping in link layer */
#define USE_LITTLE_ENDIAN

#define BUS_CLOCK_PERIOD_DFLT	20000ull	

#define HDATA_BUS_WIDTH    32
#define HDATA_BUS_HIGH_BIT 31

#define AHB_MAX_NUM_MASTER 4


// ----  transfer types (HTRANS)
// master got the bus but does not want to transfer something
#define AHB_TRANS_IDLE     00ub
// keep the bus but have an idle pattern within a burst
#define AHB_TRANS_BUSY     01ub
// non sequential transfer
#define AHB_TRANS_NONSEQ   10ub
// sequential (addr increase) trasfer
#define AHB_TRANS_SEQ      11ub

// ----  burst operation (HBURST)
// individual transfer
#define AHB_BURST_SINGLE   000ub
// incrementing burst of unspecified length
#define AHB_BURST_INCR     001ub
// 4 beat wrapping burst
#define AHB_BURST_WRAP4    010ub
// 4 beat incrementing burst
#define AHB_BURST_INCR4    011ub
// 8 beat wrapping burst
#define AHB_BURST_WRAP8    100ub
// 8 beat incrementing burst
#define AHB_BURST_INCR8    101ub
// 16 beat wrapping burst
#define AHB_BURST_WRAP16   110ub
// 16 beat incrementing burst
#define AHB_BURST_INCR16   111ub


// ----  slave response code (HRESP)
#define AHB_RESP_OKAY      00ub
// unspecified error (protection error ?)
#define AHB_RESP_ERROR     01ub
// retry later, but master keeps prio
#define AHB_RESP_RETRY     10ub
// retry later, master gets lowest prio
#define AHB_RESP_SPLIT     11ub


// ----  transfer size indication (HSIZE)
// size in bits of an individual beat (cycle)
// max size depends on bus width
#define AHB_SIZE_8         000ub
#define AHB_SIZE_16        001ub
#define AHB_SIZE_32        010ub
#define AHB_SIZE_64        011ub
#define AHB_SIZE_128       100ub
#define AHB_SIZE_256       101ub
#define AHB_SIZE_512       110ub
#define AHB_SIZE_1024      111ub

}
#endif
