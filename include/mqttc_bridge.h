#ifndef MQTTC_BRIDGE_HH__
#define MQTTC_BRIDGE_HH__

#include <libconfig.h>

int mqttc_bridge_task_init(config_t* cfg);
int mqttc_bridge_task_cleanup();

#endif