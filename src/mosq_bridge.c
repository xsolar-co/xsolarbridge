/**
 * @file mosq_bridge.c
 * @author longdh
 * @brief 
 * @version 0.1
 * @date 2023-10-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "mosq_sink.h"
#include "mosq_src.h"
#include "mosq_bridge.h"
#include "error.h"
#include "configuration.h"
#include "logger.h"

// queue 
Queue   mosq_sink_queue;

mosq_sync_config mosq_sink_conf;
int mosq_sink_task_init(config_t* cfg)
{
    // read config
    // Access the 'mosq-sink' subsetting
    config_setting_t* mosq_src = config_lookup(cfg, "mosq-sink");

    if (mosq_src != NULL) 
    {
        char* host = (char*)read_string_setting(mosq_src, "host", "103.161.39.186");
        int port = read_int_setting(mosq_src, "port", 1883);
        char* username = (char*)read_string_setting(mosq_src, "username", "lxdvinhnguyen01");
        char* password = (char*)read_string_setting(mosq_src, "password", "lxd@123");
        char* clientid = (char*)read_string_setting(mosq_src, "clientid", "sinktaskcli-01");
        char* topic = (char*)read_string_setting(mosq_src, "topic", "lxd/BA31605780");

        printf("Host: %s\n", host);
        printf("Port: %d\n", port);
        printf("Username: %s\n", username);
        printf("Password: %s\n", password);
        printf("Client ID: %s\n", clientid);
        printf("Topic: %s\n", topic);

        mosq_sink_init(&mosq_sink_conf, &mosq_sink_queue, host, port, username, password, clientid, topic);
        mosq_sink_run(&mosq_sink_conf);

    } 
    else 
    {
        fprintf(stderr, "The 'mosq-sink' subsetting is missing.\n");
    }   

    return 0;
}

int mosq_sink_task_cleanup()
{
    mosq_sink_wait(&mosq_sink_conf);
    mosq_sink_term(&mosq_sink_conf);

    return 0;
}

mosq_source_config mosq_source_conf;
int mosq_source_task_init(config_t* cfg)
{
    config_setting_t* mosq_src = config_lookup(cfg, "mosq-src");

    if (mosq_src != NULL) 
    {
        char* host = (char*)read_string_setting(mosq_src, "host", "192.168.31.166");
        int port = read_int_setting(mosq_src, "port", 1883);
        char* username = (char*)read_string_setting(mosq_src, "username", "lxdvinhnguyen01");
        char* password = (char*)read_string_setting(mosq_src, "password", "lxd@123");
        char* clientid = (char*)read_string_setting(mosq_src, "clientid", "sourcetaskcli-01");
        char* topic = (char*)read_string_setting(mosq_src, "topic", "lxp/BA31605780");

        printf("Host: %s\n", host);
        printf("Port: %d\n", port);
        printf("Username: %s\n", username);
        printf("Password: %s\n", password);
        printf("Client ID: %s\n", clientid);
        printf("Topic: %s\n", topic);

        // init queue
        init_queue(&mosq_sink_queue);

        mosq_source_init(&mosq_source_conf, &mosq_sink_queue, host, port, username, password, clientid, topic);
        mosq_source_run(&mosq_source_conf);


        // free(host);
        // free(username);
        // free(password);
        // free(clientid);
        // free(topic);

    } else {
        fprintf(stderr, "The 'mosq-src' subsetting is missing.\n");
    }   

    return 0;

}

/**
 * @brief Cleanup
 * 
 * @return int 
 */
int mosq_source_task_cleanup()
{
    mosq_source_wait(&mosq_source_conf);
    mosq_source_term(&mosq_source_conf);

    return 0;
}


/**
 * @brief Init Bridge
 * 
 * @param cfg 
 * @return int 
 */
int mosq_bridge_task_init(config_t* cfg)
{
    mosquitto_lib_init();
    
    // mqtt source task
    log_message(LOG_INFO, "Init MQTT source reader task\n");
    mosq_source_task_init(cfg);

     // mqtt target task
    log_message(LOG_INFO, "Init MQTT send task\n");
    mosq_sink_task_init(cfg);

    return ENOERR;
}

/**
 * @brief Cleanup
 * 
 * @return int: error code
 */
int mosq_bridge_task_cleanup()
{
    mosq_source_task_cleanup();
    mosq_sink_task_cleanup();

    return ENOERR;
}