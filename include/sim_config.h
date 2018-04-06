
#define SIM_RESOLUTION sc_core::SC_PS

#define CLOCK_PERIOD 1000/*SC_PS*/
#define BUS_CLOCK_PERIOD 1000 /*SC_PS*/
/********************************************
    COMMENT OUT FOR UNTIMED BUS MODEL                    */    
//#define BUS_TIMED_MODEL
/********************************************/
/********************************************
    Interrupt Handler macros
    >> Delays could be application-specific
    >> The following numbers are obtained from Linux kernel traces.
*/
//#define IHANDLER_DELAY_1 ((sc_dt::uint64)2650*CLOCK_PERIOD)
//#define IHANDLER_DELAY_2 ((sc_dt::uint64)450*CLOCK_PERIOD)
//#define SHANDLER_DELAY ((sc_dt::uint64)2800*CLOCK_PERIOD)

//#define IHANDLER_INSTR_1 2650
//#define IHANDLER_INSTR_2 450
//#define SHANDLER_INSTR 2800


#define IHANDLER_DELAY_1 ((sc_dt::uint64)0*CLOCK_PERIOD)
#define IHANDLER_DELAY_2 ((sc_dt::uint64)0*CLOCK_PERIOD)
#define SHANDLER_DELAY ((sc_dt::uint64)0*CLOCK_PERIOD)

#define IHANDLER_INSTR_1 0
#define IHANDLER_INSTR_2 0
#define SHANDLER_INSTR 0


/********************************************
    COMMENT OUT FOR UNTIMED OS HANDSHAKE CHANNEL 
    >> Delays are application specific.
    >> The following numbers are for artificial task sets example.
*/    


//#define TIMED_HANDSHAKE_CHANNEL
#define HSHK_CH_RECEIVE_DELAY ((sc_dt::uint64)800*CLOCK_PERIOD)
#define HSHK_CH_RECEIVE_DELAY_2 ((sc_dt::uint64)1290*CLOCK_PERIOD)
#define HSHK_CH_SEND_DELAY ((sc_dt::uint64)1500*CLOCK_PERIOD)

#define HSHK_CH_RECEIVE_INSTR 800
#define HSHK_CH_RECEIVE_INSTR_2 1290
#define HSHK_CH_SEND_INSTR 1500


/*
*********************************************/

/********************************************
    Generic interrupt controller trace enabled.    
#define GIC_TRACE_ON 
*********************************************/

/********************************************
    Interrupt handler trace enabled. 
#define INTR_TRACE_ON 
*********************************************/

/********************************************
    OS statistics report enabled. 
*/
#define OS_STATISTICS_ON 
/*********************************************/
