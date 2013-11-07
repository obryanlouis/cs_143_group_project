


#include "ControlHelpers.h"
#include "Control.h"
#include <iostream>


void routerUpdate(void* args){
    SYSTEM_CONTROLLER->routerUpdate();
    void (*fp)(void*) = &routerUpdate;
    SYSTEM_CONTROLLER->add  \
        (new Event(SYSTEM_TIME + ROUTING_UPDATE_PERIOD, fp, 0));
}

void printSystem(void* args){
    SYSTEM_CONTROLLER->printSystem();
    void (*fp)(void*) = &printSystem;
    SYSTEM_CONTROLLER->add \
        (new Event(SYSTEM_TIME + SNAPSHOT_PERIOD, fp, 0));
}
