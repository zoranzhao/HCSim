

#ifndef __LWIPCPPWRAPPER_H__
#define __LWIPCPPWRAPPER_H__


#define SYSTEMC_ARCH_THREAD

#ifdef SYSTEMC_ARCH_THREAD
#include <systemc>
#include "HCSim.h"
#endif


typedef void (*lwip_thread_fn)(void *arg);
typedef void (*OS_wrapper_fn)(void *ctxt, lwip_thread_fn function, void* arg, int taskID);
void sys_thread_new(void *ctxt, lwip_thread_fn function, void *arg);
void OS_child_task(void *arg);


struct sys_sem {
  bool free;
  int id;

  unsigned int c;
  sc_core::sc_event cond; // systemc channel
  sc_core::sc_mutex mutex; // systemc mutex

  void * ctxt;
  //sc_core::sc_port<sc_core::sc_fifo_in_if<char> > in;
  //sc_core::sc_port<sc_core::sc_fifo_out_if<char> > out;
  //sc_core::sc_fifo<char> *fifo_inst;

/* For OS task integration */
  int blocked_task_id;
  int blocking_task_id;

/* For OS task integration */

};
typedef struct sys_sem * sys_sem_t;

class LwipCntxt {
  public:



  sys_sem_t *sem;

  sc_core::sc_port< HCSim::OSAPI > os_port;
  std::vector< sc_core::sc_process_handle> taskHandlerList;  
  std::vector< int> taskIDList;  
  int getOSModelTaskID(sc_core::sc_process_handle taskHandler){
	std::vector< sc_core::sc_process_handle >::iterator handlerIt = taskHandlerList.begin();
	std::vector< int >::iterator idIt = taskIDList.begin();
	for(; (handlerIt!=taskHandlerList.end() && idIt!=taskIDList.end() ) ;handlerIt++, idIt++){
		if(*handlerIt == taskHandler)
			return *idIt;	
	}
	return -1;
  } 
  LwipCntxt(){
	std::cout << "Testing lwip integration" << std::endl;
  }
  ~LwipCntxt(){
  }

};


extern struct sys_sem sems[100];

extern int global;


struct sys_sem *
sys_sem_new_internal(void* ctxt, unsigned int count)
{ //printf("SEM INTERNAL\n"); /* problem */


//  printf("Testing os_port connections ... ... \n");
//  ((LwipCntxt *)ctxt) -> os_port->debugPrint();

  struct sys_sem *sem;

  //sem = (sys_sem *)new(sizeof(sys_sem));//
  //sem = new sys_sem; 
  sem = sems + global;
  sem->id = global;

  global++;
  //sem->fifo_inst = new sc_core::sc_fifo<char>("Fifo2",10);
  //sem->out(*(sem->fifo_inst));
  //sem->in(*(sem->fifo_inst));

  if (sem != NULL) {
    sem->c = count;
    sem->ctxt = ctxt;
    sem->blocked_task_id = HCSim::OS_NO_TASK;
    sem->blocking_task_id = HCSim::OS_NO_TASK;


    //pthread_cond_init(&(sem->cond), NULL);
    //pthread_mutex_init(&(sem->mutex), NULL);
  }
  return sem;
}

int 
sys_sem_new(void *ctxt, struct sys_sem **sem, unsigned int count)
{ 

  *sem = sys_sem_new_internal(ctxt, count);
  if (*sem == NULL) {
    return -1;
  }
  return -1;
}





int /* problem here */
sys_arch_sem_wait(struct sys_sem **s, int timeout)
{

  sc_dt::uint64 start_time;

  int time_needed = 0;
  struct sys_sem *sem;

  sem = *s;
  int taskID = ((LwipCntxt*)(sem->ctxt)) -> getOSModelTaskID( sc_core::sc_get_current_process_handle());
  sem->blocked_task_id=taskID;
  sem->mutex.lock();  

  while (sem->c <= 0) {
    if (timeout > 0) {
      sem->mutex.unlock();
      ((LwipCntxt*)((sem->ctxt)))->os_port->preWait(taskID, sem->blocking_task_id);
      sc_core::wait(sem->cond);
      ((LwipCntxt*)((sem->ctxt)))->os_port->postWait(taskID);
      sem->mutex.lock();
      time_needed = timeout;
      if (time_needed == -1) {
        sem->mutex.unlock();
        return -1;
      }
    } else { 

      {sem->mutex.unlock();   sc_core::wait(sem->cond), sem->mutex.lock();} //

    }
  }//while


  sem->c--;
  sem->mutex.unlock();

  return time_needed;
}

void
sys_sem_signal(struct sys_sem **s)
{

  struct sys_sem *sem;
  sem = *s;
  int taskID = ((LwipCntxt*)(sem->ctxt)) -> getOSModelTaskID( sc_core::sc_get_current_process_handle());
  sem->blocking_task_id=taskID;
  sem->mutex.lock();
  sem->c++;

  if (sem->c > 1) {
    sem->c = 1;
  }

  ((LwipCntxt*)(sem->ctxt)) -> os_port->preNotify(taskID, sem->blocked_task_id);
  sem->cond.notify(sc_core::SC_ZERO_TIME);
  ((LwipCntxt*)(sem->ctxt)) -> os_port->postNotify(taskID, sem->blocked_task_id);
  sem->mutex.unlock();

}












void 
OS_child_task(void *arg)
{
	printf("OS Child Task ... \n");
	int taskID = ((LwipCntxt*)(arg)) -> getOSModelTaskID( sc_core::sc_get_current_process_handle());

	std::cout<<"OS task ID is: "<< taskID << std::endl;

	//((LwipCntxt*)(arg))->os_port->timeWait(2000000, taskID);
	printf("OS Child Task ... \n");
}

void 
OS_test(void *arg)
{



  LwipCntxt* ctxt = ((LwipCntxt*)arg);
//  sys_sem_new(ctxt, &sem, 0);
//  ctxt->sem=&sem;

	printf("OS Total Level Main Thread ... \n");
	int taskID = ((LwipCntxt*)(arg)) -> getOSModelTaskID( sc_core::sc_get_current_process_handle());

	std::cout<<"OS task ID is: "<< taskID << std::endl;
        sys_thread_new(arg, OS_child_task, arg);
	//((LwipCntxt*)(arg))->os_port->timeWait(2000000, taskID);

//sys_arch_sem_wait();

	printf("OS Total Level Main Thread ... \n");
}







void wrapper( void *ctxt, lwip_thread_fn function, void *arg, int taskID){

   	sc_core::sc_process_handle taskKey = sc_core::sc_get_current_process_handle();
	((LwipCntxt*)(ctxt))->taskIDList.push_back(taskID);
	((LwipCntxt*)(ctxt))->taskHandlerList.push_back(taskKey);
        //((LwipCntxt*)(ctxt))->os_port->debugPrint();


	((LwipCntxt*)(ctxt))->os_port->taskActivate(taskID);
	function(arg);//All lwip thread args are converted to context object pointers.

	printf("Before terminations ... ...\n");
 	((LwipCntxt*)(ctxt))->os_port->taskTerminate(taskID);
	printf("After terminations ... ...\n");

}


void sys_thread_new(void *ctxt, lwip_thread_fn function, void *arg)
{

  int child_id; 
  const sc_dt::uint64 exe_cost = 8006000ull;
  const sc_dt::uint64 period = 49000000ull;
  const unsigned int priority = 49;
  const int init_core = 0;

  child_id =  ((LwipCntxt*)(ctxt))->os_port->taskCreate(
				sc_core::sc_gen_unique_name("child_task"), 
				HCSim::OS_RT_APERIODIC, priority, period, exe_cost, 
				HCSim::DEFAULT_TS, HCSim::ALL_CORES, init_core);




  OS_wrapper_fn OS_fn = NULL;
  OS_fn = &wrapper;


  sc_core::sc_process_handle th_handle;
  th_handle = sc_core::sc_spawn(     
                                 sc_bind(  
                                         OS_fn,
                                         ctxt, function, arg, child_id 
                                         )         
                                ); //function with input (to do) sc_bind( )



  printf("/***************New thread name***************/\n");
  std::cout << "Thread ID in OS model is: "<<child_id<<std::endl; 
  printf("/***************New thread name***************/\n");

}




#endif /* __LWIPCPPWRAPPER_H__ */
