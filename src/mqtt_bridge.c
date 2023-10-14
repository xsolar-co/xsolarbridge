#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <libconfig.h>

#include "error.h"
#include "configuration.h"
#include "logger.h"
#include "squeue.h"
#include "datalog.h"

#ifdef PAHO
#include "MQTTClient.h"
#include "mqtt_sink.h"
#include "mqtt_src.h"
#endif

// queue 
Queue   mqtt_sink_queue;


#ifdef PAHO
mqtt_sync_config mqtt_sink_conf;
int mqtt_sink_task_init(config_t* cfg)
{
    // read config
    // Access the 'mqtt-sink' subsetting
    config_setting_t* mqtt_src = config_lookup(cfg, "mqtt-sink");

    if (mqtt_src != NULL) 
    {
        char* host = (char*)read_string_setting(mqtt_src, "host", "103.161.39.186");
        int port = read_int_setting(mqtt_src, "port", 1883);
        char* username = (char*)read_string_setting(mqtt_src, "username", "lxdvinhnguyen01");
        char* password = (char*)read_string_setting(mqtt_src, "password", "lxd@123");
        char* clientid = (char*)read_string_setting(mqtt_src, "clientid", "sinktaskcli-01");
        char* topic = (char*)read_string_setting(mqtt_src, "topic", "lxd/BA31605780");

        // printf("Host: %s\n", host);
        // printf("Port: %d\n", port);
        // printf("Username: %s\n", username);
        // printf("Password: %s\n", password);
        // printf("Client ID: %s\n", clientid);
        // printf("Topic: %s\n", topic);

        mqtt_sink_init(&mqtt_sink_conf, &mqtt_sink_queue, host, port, username, password, clientid, topic);
        mqtt_sink_run(&mqtt_sink_conf);

    } 
    else 
    {
        fprintf(stderr, "The 'mqtt-sink' subsetting is missing.\n");
    }   

    return 0;
}

int mqtt_sink_task_cleanup()
{
    mqtt_sink_wait(&mqtt_sink_conf);
    mqtt_sink_term(&mqtt_sink_conf);

    return 0;
}
#else
int mqtt_sink_task_init(config_t* cfg)
{
    return 0;
}

int mqtt_sink_task_cleanup()
{
    return 0;
}
#endif


#ifdef PAHO
mqtt_source_config mqtt_source_conf;
int mqtt_source_task_init(config_t* cfg)
{
    config_setting_t* mqtt_src = config_lookup(cfg, "mqtt-src");

    if (mqtt_src != NULL) 
    {
        char* host = (char*)read_string_setting(mqtt_src, "host", "192.168.31.166");
        int port = read_int_setting(mqtt_src, "port", 1883);
        char* username = (char*)read_string_setting(mqtt_src, "username", "lxdvinhnguyen01");
        char* password = (char*)read_string_setting(mqtt_src, "password", "lxd@123");
        char* clientid = (char*)read_string_setting(mqtt_src, "clientid", "sourcetaskcli-01");
        char* topic = (char*)read_string_setting(mqtt_src, "topic", "lxp/BA31605780");

        // printf("Host: %s\n", host);
        // printf("Port: %d\n", port);
        // printf("Username: %s\n", username);
        // printf("Password: %s\n", password);
        // printf("Client ID: %s\n", clientid);
        // printf("Topic: %s\n", topic);

        // init queue
        init_queue(&mqtt_sink_queue);

        mqtt_source_init(&mqtt_source_conf, &mqtt_sink_queue, host, port, username, password, clientid, topic);
        mqtt_source_run(&mqtt_source_conf);


        // free(host);
        // free(username);
        // free(password);
        // free(clientid);
        // free(topic);

    } else {
        fprintf(stderr, "The 'mqtt-src' subsetting is missing.\n");
    }   

    return 0;

}

int mqtt_source_task_cleanup()
{
    mqtt_source_wait(&mqtt_source_conf);
    mqtt_source_term(&mqtt_source_conf);

    return 0;
}
#else
int mqtt_source_task_init(config_t* cfg)
{
    return 0;
}
int mqtt_source_task_cleanup()
{
    return 0;
}
#endif


/**
 * @brief init bridge task
 * 
 * @param cfg 
 * @return int 
 */
int mqtt_bridge_task_init(config_t* cfg)
{
    // mqtt source task
    log_message(LOG_INFO, "Init MQTT source reader task\n");
    mqtt_source_task_init(cfg);

     // mqtt target task
    log_message(LOG_INFO, "Init MQTT send task\n");
    mqtt_sink_task_init(cfg);
  
     return ENOERR;
}

/**
 * @brief Cleanup memory
 * 
 * @return int 
 */
int mqtt_bridge_task_cleanup()
{
    mqtt_source_task_cleanup();
    mqtt_sink_task_cleanup();

    return ENOERR;
}