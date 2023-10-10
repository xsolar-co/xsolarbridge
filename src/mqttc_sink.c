/**
 * @file mqttc_sink.c
 * @author longdh (longdh@xsolar.vn)
 * @brief 
 * @version 0.1
 * @date 2023-09-20
 * 
 * @copyright Copyright (c) 2023
 * 
 * Note: chu y cho ID-client
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#include "mqttc_sink.h"
#include "error.h"
#include "datalog.h"
#include "logger.h"

//FIXME
extern char* strdup(const char*);

#define DEBUG

volatile int _connected = 0;

/**
 * @brief sink thread (forward thread)
 * 
 * @param arg 
 * @return void* 
 */
void* mqttc_sink_task(void* arg) {

    mqttc_sync_config* cfg= (mqttc_sync_config*) arg;
    if (cfg == NULL)
    {
        #ifdef DEBUG
        log_message(LOG_PERROR, "Error config...\n");
        #endif // DEBUG
        
        exit(ESYSERR);
    }

    Queue* q = (Queue*) cfg->q;

    if (q == NULL)
    {
        #ifdef DEBUG
        log_message(LOG_PERROR, "Error queue...\n");
        #endif // DEBUG
        
        exit(EQUERR);
    }

    char mqttc_addr[256];
    sprintf(mqttc_addr, "tcp://%s:%d", cfg->host, cfg->port);
    
    #ifdef DEBUG
    // log_message(LOG_INFO, "sink connect to %s\n", mqttc_addr);
    #endif // DEBUG

    while(1)
    {

        _connected = 1;

    
        while (_connected) 
        {
            struct Message* data;

            if (wait_dequeue(q, (void**) &data))
            {

           }
        }

        // connect fail, reconnect
    }


    return NULL;
}

/**
 * @brief Init sync task
 * 
 * @param cfg 
 * @param host 
 * @param port 
 * @param username 
 * @param password 
 * @return int 
 */
int mqttc_sink_init(mqttc_sync_config* cfg, Queue* q, const char* host, int port, const char* username, const char* password, const char* client_id, const char* topic)
{
    memset(cfg, 0, sizeof (mqttc_sync_config));

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
        sprintf(id, "cli_%lu\n", (unsigned long)time(NULL));

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
int mqttc_sink_term(mqttc_sync_config* cfg)
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
int mqttc_sink_run(mqttc_sync_config* cfg)
{   
    return 
        pthread_create(&cfg->task_thread, NULL, mqttc_sink_task, cfg);
    
}

/**
 * @brief Wait until end
 * 
 * @param cfg 
 * @return int 
 */
int mqttc_sink_wait(mqttc_sync_config* cfg)
{
    return 
        pthread_join(cfg->task_thread, NULL);    
}