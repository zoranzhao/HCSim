
#include "Channels/ambaAhb.h"

using namespace HCSim;


/*============================ TLM LAYER ==============================*/
AmbaAhbMacTlm::AmbaAhbMacTlm(const sc_core::sc_module_name name, sc_dt::uint64 clock_period)
    :sc_core::sc_channel(name)
    ,write_flag(false)
{   
    this->bus_clock_period = clock_period;
    forward_flag =false;
}

AmbaAhbMacTlm::~AmbaAhbMacTlm()
{
}

sc_dt::uint64 AmbaAhbMacTlm::getShortestTransferTime(unsigned int len, unsigned long int addr, 
	bool burstEnable)
{   
    unsigned int cycles = 0u;
    unsigned int transfers;
    unsigned long long int cycleOffset;

	cycleOffset = ((sc_core::sc_time_stamp().value() + (bus_clock_period / 2)) % bus_clock_period);
	if ((cycleOffset == 0) && (sc_core::sc_delta_count() <= 1)) 
	{   
	    cycleOffset = bus_clock_period;
	}
	if (len && (addr & 1))
	{   
	    len -= 1;
	    addr++ ;
	    cycles += 4;
	}
	if ((len >= 2) && (addr & 2))
	{   
	    cycles += 4;
	    len -= 2;
	}
	if (burstEnable)
	{   
	    transfers = len / 64;
	    len -= transfers * 64;
	    cycles += transfers * 19;
	    transfers = len / 32;
	    len -= transfers * 32;
	    cycles += transfers * 11;
	    transfers = len / 16;
	    len -= transfers * 16;
	    cycles += transfers * 7;
	}
	transfers = len / 4;
	len -= transfers * 4;
	if (len >= 2)
	{   
	    len -= 2;
	    transfers++ ;
	}
	if (len & 1)
	{   
	    transfers++ ;
	}
	cycles += transfers * 4;
	return cycles * bus_clock_period - cycleOffset;
}

void AmbaAhbMacTlm::masterWrite(unsigned long int addr, const void *data, unsigned long int len)
{  

	mutex.lock();
	ADDR = addr;
	DATA = (void *)data;
	LEN = len;
	write_flag = false;
	dataReady.notify(sc_core::SC_ZERO_TIME);
	forward_flag = true;
	sc_core::wait(ack);
	mutex.unlock();
}

void AmbaAhbMacTlm::masterMemWrite(unsigned long int addr, const void *data, unsigned long int len)
{  
	masterWrite(addr, data, len);
}

void AmbaAhbMacTlm::masterRead(unsigned long int addr, void *data, unsigned long int len)
{   

	mutex.lock();
	ADDR = addr;
	DATA = (void *)data;
	LEN = len;
	write_flag = true;
	dataReady.notify(sc_core::SC_ZERO_TIME);
	sc_core::wait(ack);
	mutex.unlock();
}

void AmbaAhbMacTlm::masterMemRead(unsigned long int addr, void *data, unsigned long int len)
{   
	masterRead(addr, data, len);
}

void AmbaAhbMacTlm::slaveWrite(unsigned long int addr, const void *data, unsigned long int len)
{  
	do 
	{   
	    wait(dataReady);
	}
	while(( !write_flag) || (ADDR != addr));
	memcpy(DATA, data, len);
#ifdef BUS_TIMED_MODEL
	sc_core::wait(getShortestTransferTime(len, ADDR, false), SIM_RESOLUTION);
#endif	
	ack.notify();
}

void AmbaAhbMacTlm::slaveRead(unsigned long int addr, void *data, unsigned long int len)
{   

	do 
	{  
	    if (!forward_flag) { 
		  wait(dataReady);
	    } 
	    forward_flag = false;
	}
	while((write_flag) || (ADDR != addr));
	memcpy(data, DATA, len);
#ifdef BUS_TIMED_MODEL	
	sc_core::wait(getShortestTransferTime(len, ADDR, false), SIM_RESOLUTION);
#endif	
	ack.notify();
}

void AmbaAhbMacTlm::serve(unsigned long int addr, void *data, unsigned long int len)
{  

    do 
    {   
	    wait(dataReady);
	}
	while((ADDR < addr) || (ADDR >= addr + len));
	
	data = (void *)(((unsigned char *)data) + (ADDR - addr));
	if (write_flag)
	{   
	    memcpy(DATA, data, LEN);
	}
	else 
	{   
	    memcpy(data, DATA, LEN);
	}
#ifdef BUS_TIMED_MODEL	
	sc_core::wait(getShortestTransferTime(LEN, ADDR, true), SIM_RESOLUTION);
#endif	
	ack.notify();
}

void AmbaAhbMacTlm::serveRead(unsigned long int addr, void *data, unsigned long int len)
{ 
    serve(addr, data, len);
}

void AmbaAhbMacTlm::serveWrite(unsigned long int addr, void *data, unsigned long int len)
{   
	serve(addr, data, len);
}
/*===================================================================*/
void AmbaAhbBusSlaveMacLinkPass::slaveWrite(unsigned long busAddr, const void* data, unsigned long len)  
{
    protWMac->slaveWrite(busAddr, data, len);
}
  
void AmbaAhbBusSlaveMacLinkPass::slaveRead(unsigned long busAddr, void* data, unsigned long len)  
{
    protWMac->slaveRead(busAddr, data, len);
}
/*===================================================================*/
void AmbaAhbBusSlaveMacMemPass::serve(unsigned long busBaseAddr, void* pData, unsigned long len) 
{
    protWMac->serve(busBaseAddr, pData, len);
} /* serve */

void AmbaAhbBusSlaveMacMemPass::serveRead(unsigned long busBaseAddr, void* pData, unsigned long len) 
{
    protWMac->serveRead(busBaseAddr, pData, len);
}

void AmbaAhbBusSlaveMacMemPass::serveWrite(unsigned long busBaseAddr, void* pData, unsigned long len) 
{
    protWMac->serveWrite(busBaseAddr, pData, len);
}
/*===================================================================*/
void AmbaAhbBusMasterMacLinkPass::masterWrite(unsigned long addr, const void* data, unsigned long len)
{
    protWMac->masterWrite(addr, data, len);
}
  
void AmbaAhbBusMasterMacLinkPass::masterRead(unsigned long addr, void* data, unsigned long len) 
{
    protWMac->masterRead(addr, data, len);
}
/*===================================================================*/
void AmbaAhbBusMasterMacMemPass::masterMemWrite(unsigned long addr, const void* data, unsigned long len) 
{
    protWMac->masterMemWrite(addr, data, len);
}
  
void AmbaAhbBusMasterMacMemPass::masterMemRead(unsigned long addr, void* data, unsigned long len)
{
    protWMac->masterMemRead(addr, data, len);
} 
/*===================================================================*/  
void AmbaAhbMacTlm_MasterTLM_Wrap::masterMemRead(unsigned long int addr, void *data, unsigned long int len)
{   
    mutex.lock();
    tlm_port->masterMemRead(addr, data, len);
    mutex.unlock();
}

void AmbaAhbMacTlm_MasterTLM_Wrap::masterMemWrite(unsigned long int addr, const void *data, unsigned long int len)
{   
    mutex.lock();
    tlm_port->masterMemWrite(addr, data, len);
    mutex.unlock();
}

void AmbaAhbMacTlm_MasterTLM_Wrap::masterRead(unsigned long int addr, void *data, unsigned long int len)
{   
    mutex.lock();
    tlm_port->masterRead(addr, data, len);
    mutex.unlock();
}

void AmbaAhbMacTlm_MasterTLM_Wrap::masterWrite(unsigned long int addr, const void *data, unsigned long int len)
{
    mutex.lock();
    tlm_port->masterWrite(addr, data, len);
    mutex.unlock();
}

/*-------- EOF ---------*/
