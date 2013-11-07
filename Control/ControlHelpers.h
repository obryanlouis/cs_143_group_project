


#ifndef CONTROL_HELPERS_H
#define CONTROL_HELPERS_H

#include "Control.h"
#include "Flow.h"
#include "Link.h"


void routerUpdate(void *args);
void printSystem(void *args);

void linkPacketIn(void *args);
void linkPopQueue(void *args);
void linkPacketOut(void *args);

void updateFlow(void *args);

#endif
