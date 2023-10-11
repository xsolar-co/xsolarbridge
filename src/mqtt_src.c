/**
 * @file mqtt_src.c
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

#include "MQTTClient.h"
#include "datalog.h"
#include "mqtt_src.h"
#include "squeue.h"
#include "error.h"
#include "logger.h"

//FIXME
extern char* strdup(const char*);

volatile MQTTClient_deliveryToken deliveredtoken;
volatile int connected = 0;


#define DEBUG

/**
 * @brief 
 * 
 * @param context 
 * @param dt 
 */
static void delivered(void* context, MQTTClient_deliveryToken dt) 
{
    #ifdef DEBUG
    log_message(LOG_INFO, "Message with token value %d delivery confirmed\n", dt);
    #endif // DEBUG   

    deliveredtoken = dt;
}

/**
 * @brief Handle msg arrived 
 * 
 * @param context 
 * @param topicName 
 * @param topicLen 
 * @param message 
 * @return int 
 */
static int msgarrvd(void* context, char* topicName, int topicLen, MQTTClient_message* message) 
{
    struct Message* msg = create_message(topicName, NULL, message->payload, message->payloadlen);

    if (msg)
    {
        int i = 0;
        Queue *q = (Queue*) context;
        
        #ifdef DEBUG
        log_message(LOG_INFO, "Received message at tpoic %s: %s\n", topicName, (char*) message->payload);
        #endif // DEBUG    

        // send to queue
        enqueue( q, (void*) msg);
        

        MQTTClient_freeMessage(&message);
        MQTTClient_free(topicName);
    }

    return 1;
}

/**
 * @brief disconnected callback
 * 
 * @param context 
 * @param cause 
 */
static void connectionLost(void* context, char* cause) {
    #ifdef DEBUG
    log_message(LOG_WARNING, "MQTTSource Connection lost, cause: %s\n", cause);
    #endif // DEBUG
    
    connected = 0;
}

/**
 * @brief Main thread
 * 
 * @param arg 
 * @return void* 
 */
static void* mqtt_source_reader_task(void* arg) 
{
    mqtt_source_config* cfg = (mqtt_source_config*) arg;

    MQTTClient client;    
    int rc;
    Queue *q = (Queue*) cfg->q;


    char mqtt_addr[256];
    sprintf(mqtt_addr, "tcp://%s:%d", cfg->host, cfg->port);
    #ifdef DEBUG
    log_message(LOG_INFO, "connect to %s, client_id = %s\n", mqtt_addr, cfg->client_id); 
    #endif // DEBUG

 
    while (1) 
    {
        MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer; 

        if ( (rc = MQTTClient_create(&client, mqtt_addr, (const char*) cfg->client_id, MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS)
        {
            log_message(LOG_PERROR, "Create Client-source Error, error code: %d\n", rc);
            exit(ESVRERR);
        }

        conn_opts.keepAliveInterval = 20;
        conn_opts.cleansession = 1;

        if (cfg->username != NULL)
        {
            conn_opts.username = cfg->username;
            conn_opts.password = cfg->password;
        }
        
        conn_opts.MQTTVersion = 0;

        MQTTClient_setCallbacks(client, (void*) q, connectionLost, msgarrvd, delivered);

        if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) 
        {

            #ifdef DEBUG
            log_message(LOG_PERROR, "Failed to connect to source, return code %d. Retrying...\n", rc);
            #endif // DEBUG

            // destroy client
            MQTTClient_destroy(&client);
            
            sleep(5); // Wait for a while before retrying

            continue;
        }

        MQTTClient_subscribe(client, cfg->topic, 0);
        connected = 1;

        while (connected) {            
            sleep(1); // Sleep for a short period to avoid busy-waiting
        }

        #ifdef DEBUG
        log_message(LOG_INFO, "Source module - Disconnect & distroy current client and reconnect!");
        #endif // DEBUG

        MQTTClient_disconnect(client, 10000);
        MQTTClient_destroy(&client);
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
int mqtt_source_init(mqtt_source_config* cfg, Queue *q, const char* host, int port, const char* username, const char* password, const char* client_id, const char* topic)
{
    memset(cfg, 0, sizeof (mqtt_source_config));

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
int mqtt_source_term(mqtt_source_config* cfg)
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
int mqtt_source_run(mqtt_source_config* cfg)
{   
    return 
        pthread_create(&cfg->task_thread, NULL, mqtt_source_reader_task, cfg);
    
}

/**
 * @brief Wait until end
 * 
 * @param cfg 
 * @return int 
 */
int mqtt_source_wait(mqtt_source_config* cfg)
{
    return 
        pthread_join(cfg->task_thread, NULL);    
}