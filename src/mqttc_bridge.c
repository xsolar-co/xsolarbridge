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

#include "mqttc_sink.h"
#include "mqttc_src.h"

int mqttc_bridge_task_init(config_t* cfg)
{
    return ENOERR;
}


int mqttc_bridge_task_cleanup()
{
    return ENOERR;
}