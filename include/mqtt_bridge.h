#ifndef MQTT_BRIDGE_HH__
#define MQTT_BRIDGE_HH__

#include <libconfig.h>

int mqtt_bridge_task_init(config_t* cfg);
int mqtt_bridge_task_cleanup();

#endif