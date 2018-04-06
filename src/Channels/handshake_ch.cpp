/*********************************************
 * Parisa Razaghi, UT Austin
 * Last update: Jan. 2012 
 ********************************************/

#include "Channels/handshake_ch.h"
#include <string>     // std::string, std::to_string

int ID = 0;

using namespace HCSim;

handshake_ch::handshake_ch()
    :sc_core::sc_channel(sc_core::sc_module_name(sc_core::sc_gen_unique_name("handshake_ch")))
    ,forward_flag(false)
    ,wait_flag(false)
{
}

handshake_ch::handshake_ch(const sc_core::sc_module_name name)
    :sc_core::sc_channel(name)
    ,forward_flag(false)
    ,wait_flag(false)
{
}

handshake_ch::~handshake_ch()
{
}

void handshake_ch::send(void)
{
     if (wait_flag) {
        event.notify();
    }
    forward_flag = true;
}

void handshake_ch::receive(void)
{
    if (!forward_flag) { 
        wait_flag = true; 
        sc_core::wait(event);
        wait_flag = false; 
    } 
    forward_flag = false;
}

/*------- EOF -------*/
