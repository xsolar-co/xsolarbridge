/**
 * @file datalog.c
 * @author longdh
 * @brief 
 * @version 0.1
 * @date 2023-10-05
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "datalog.h"

//FIXME
extern char* strdup(const char*);

/**
 * @brief Create a message object
 * 
 * @param source_topic 
 * @param target_topic 
 * @param data 
 * @param datalen 
 * @return struct Message* 
 */
struct Message* create_message(const char* source_topic, const char* target_topic, void* data, int64_t datalen)
{
    struct Message* msg = malloc(sizeof(struct Message));

    if (msg == NULL)
    {
        exit(ESYSERR);
    }

    memset(msg, 0, sizeof(struct Message));

    if (source_topic)
        msg->source_topic = strdup(source_topic);

    if (target_topic)
        msg->target_topic = strdup(target_topic);

    if (data && (datalen > 0))
    {
        msg->data = malloc(datalen);
        memcpy(msg->data, data, datalen);
    }

    return msg;
}

/**
 * @brief 
 * 
 * @param msg 
 */
void free_message(struct Message* msg)
{
    if (msg)
    {   
        // free item
        if (msg->source_topic)
            free(msg->source_topic);

        if (msg->target_topic)
            free(msg->target_topic);

        if (msg->data)
            free(msg->data);

        // free pointer
        free(msg);
    }
}
