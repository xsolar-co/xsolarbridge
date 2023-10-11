#ifndef MOSQ_BRIDGE_HH__
#define MOSQ_BRIDGE_HH__

#include <libconfig.h>

int mosq_bridge_task_init(config_t* cfg);
int mosq_bridge_task_cleanup();

#endif