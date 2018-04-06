/*********************************************    
 * Parisa Razaghi, UT Austin <parisa.r@utexas.edu>
 * Last update: Jun. 2013 
 ********************************************/
#include <systemc>
#include <stdint.h>

#ifndef SC_GLOBAL_OS__H
#define SC_GLOBAL_OS__H

namespace HCSim {

/*--------------------------------------------------------------------- */
/* Macros                                                                            */
/*--------------------------------------------------------------------- */
#define MIN_VAL(a,b) ((a) < (b) ? a : b)

/*--------------------------------------------------------------------- */
/* general global constants                                                  */
/*--------------------------------------------------------------------- */
const int OS_MAXLEN = 20;                        /* max string length */
const int OS_MAXPROC = 100;                   /* max number of tasks */
const sc_dt::uint64 OS_MAXDLINE = (0xFFFFFFFFFFFFFFFF);  /* max deadline */
const unsigned int OS_PRT_LEV = 255;                     /* number of priority levels */
const int OS_NILL = -1;                                              /* null pointer */
const int OS_MAXCORE = 64;                     /* max number of cores */
const sc_dt::uint64 OS_INFINIT_VAL = 0X7FFFFFFFFFFFFFFF;   /* infinit number*/
const unsigned int OS_DEFAULT_GRANULARITY = 1000;   /* default timing granularity (PSec) */
const  int OS_NO_TASK  = OS_NILL;
const  int OS_NO_CPU = OS_NILL;
const sc_dt::uint64 DEFAULT_TS = (0xFFFFFFFFFFFFFFFF);
const sc_dt::uint64 ALL_CORES = (0xFFFFFFFFFFFFFFFF);

/*---------------------------------------------------------------------  */
/* task states                                                                      */
/*---------------------------------------------------------------------  */
  enum OSTaskState
  {
    OS_FREE = 0,    /* TCB not allocated */
    OS_READY,        /* ready state */
    OS_RUN,           /* running state */
    OS_IDLE,          /* idle state */
    OS_WAIT,         /* wait state */
    OS_INTR_WAIT,/* intr wait state */
    OS_ZOMBIE,     /* zombie state */
    OS_SLEEP,       /* sleep state */
    OS_CPU           /* on CPU state */
  };

/*--------------------------------------------------------------------- */
/* error messages                                                               */
/*--------------------------------------------------------------------- */
const unsigned int OS_OK = 0;                  /* no error */
const unsigned int TIME_OVERFLOW = 1;  /* missed deadline */
const unsigned int TIME_EXPIRED = 2;      /* lifetime reached */
const unsigned int NO_GUARANTEE = 3;    /* task not schedulable */
const unsigned int NO_TCB = 4;                /* too many tasks */
const unsigned int NO_EVENT = 5;            /* too many events */

/*--------------------------------------------------------------------- */
/* task types                                                                      */
/*--------------------------------------------------------------------- */
  enum OSTaskType
  {
    OS_RT_PERIODIC = 1,     /* periodic hard real time task */
    OS_RT_APERIODIC,         /* non periodic hard  real time task */	
    OS_INTR_HANDLER,         /* interrupt handler */
    OS_INTR_TASK,               /* interrupt task */
    OS_UNDEFINED = OS_NILL /* undefined type */
  };

/*--------------------------------------------------------------------- */
/* OS internal types                                                            */
/*--------------------------------------------------------------------- */
  typedef int OSQueue;  /* head index */
  typedef int OSProc;   /* process index */

/*--------------------------------------------------------------------- */
/* OS task control block                                                       */ 
/*--------------------------------------------------------------------- */
struct TCB {
    char                name[100];           /* task name */
    OSTaskType    type;                     /* task type */
    OSTaskState   state; 	                /* task state */
    sc_dt::uint64  next_release_time; /* next period start time */
    sc_dt::uint64  period;            /* task period */
    unsigned int    priority;          /* task priority */
    sc_dt::uint64  wcet;              /* worst case execution time */
    float                weight;           /* task utilization factor */
    uint8_t           schedcore;      /* scheduled core */
    sc_dt::uint64 affinity;           /* target cores */
    sc_dt::uint64 ts;                   /* current time slice */
    sc_dt::uint64 dts;                 /* default time slice */
    sc_dt::uint64 start_time;      /* start time of execution */
    sc_dt::uint64 advance_time;          /* advance sim. time */
    sc_dt::uint64 accumulated_delay; /* accumulate delay */
    sc_dt::uint64 predicted_delay;      /* next predicted delay */
    sc_dt::uint64 adjusted_delay;      /* adjustd delay for inter-core intr. dep. cases */
    bool               predictive_mode;     /* os is in predictive mode */
    bool               fallback_mode;        /* os is in fallback mode */
    bool               fallback_check;        /* fallback check is required */
    bool               id_flag;                   /* inter-core interrupt dependency (id) flag */
    bool               id_check;                /* id check is required */
    OSProc          blocked_task_id;     /* blocked task */
    OSProc          blocking_task_id;    /* blocking task */
    uint8_t          launched_core_id;  /* launched core id of interrupt handler */ 
    sc_dt::uint64 start_cycle;            /* start time for periodic tasks */
    sc_dt::uint64 response_time;     /* response time for periodic tasks */
    OSProc          next;              /* pointer to the next TCB */
    OSProc          prev;              /* pointer to previous TCB */   
};


} //HCSim 

#endif //SC_GLOBAL_OS__H
