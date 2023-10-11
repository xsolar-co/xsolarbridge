/**
 * @file mqttc_src.c
 * @author longdh (longdh@xsolar.vn)
 * @brief 
 * @version 0.1
 * @date 2023-09-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "datalog.h"
#include "mqttc_src.h"
#include "squeue.h"
#include "error.h"
#include "logger.h"

//FIXME
extern char* strdup(const char*);

static volatile int connected = 0;


#define DEBUG


/**
 * @brief Main thread
 * 
 * @param arg 
 * @return void* 
 */
static void* mqttc_source_reader_task(void* arg) 
{
    mqttc_source_config* cfg = (mqttc_source_config*) arg;

    int rc;
    Queue *q = (Queue*) cfg->q;


    char mqttc_addr[256];
    sprintf(mqttc_addr, "tcp://%s:%d", cfg->host, cfg->port);
    #ifdef DEBUG
    log_message(LOG_INFO, "connect to %s, client_id = %s\n", mqttc_addr, cfg->client_id); 
    #endif // DEBUG

 
    while (1) 
    {
        connected = 1;

        while (connected) {            
            sleep(1); // Sleep for a short period to avoid busy-waiting
        }

    }

    return NULL;
}

/**
 * @brief Init Source task
 * 
 * @param cfg 
 * @param host 
 * @param port 
 * @param username 
 * @param password 
 * @return int 
 */
int mqttc_source_init(mqttc_source_config* cfg, Queue *q, const char* host, int port, const char* username, const char* password, const char* client_id, const char* topic)
{
    memset(cfg, 0, sizeof (mqttc_source_config));

    cfg->q = q;

    if (host != NULL)
        cfg->host = strdup(host);

    cfg->port = port;
    if (username != NULL)
        cfg->username = strdup(username);

    if (password != NULL)
        cfg->password = strdup(password);

    if (topic != NULL)
        cfg->topic = strdup(topic);

    if (client_id != NULL)
        cfg->client_id = strdup(client_id);
    else {
        char id[128];
        memset(id, 0, sizeof(id));
        sprintf(id, "src_%lu\n", (unsigned long)time(NULL));

        cfg->client_id = strdup(id);
    } 

    return 0;
}

/**
 * @brief Free task's data
 * 
 * @param cfg 
 * @return int 
 */
int mqttc_source_term(mqttc_source_config* cfg)
{
    if (cfg->host != NULL)
        free(cfg->host);

    if (cfg->username != NULL)
        free(cfg->username);

    if (cfg->password != NULL)
        free(cfg->password);
    
    if (cfg->client_id != NULL)
        free(cfg->client_id);

    if (cfg->topic != NULL)
        free(cfg->topic);

    return 0;
}

/**
 * @brief Do the task
 * 
 * @param cfg 
 * @return int 
 */
int mqttc_source_run(mqttc_source_config* cfg)
{   
    return 
        pthread_create(&cfg->task_thread, NULL, mqttc_source_reader_task, cfg);
    
}

/**
 * @brief Wait until end
 * 
 * @param cfg 
 * @return int 
 */
int mqttc_source_wait(mqttc_source_config* cfg)
{
    return 
        pthread_join(cfg->task_thread, NULL);    
}