#ifndef MQTT_BRIDGE_HH__
#define MQTT_BRIDGE_HH__

#include <libconfig.h>

int mqtt_sink_task_init(config_t* cfg);
int mqtt_sink_task_cleanup();
int mqtt_source_task_init(config_t* cfg);
int mqtt_source_task_cleanup();


int mqtt_bridge_task_init(config_t* cfg);
int mqtt_bridge_task_cleanup();

#endif