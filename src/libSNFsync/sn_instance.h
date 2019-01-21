#ifndef INSTANCE_H
#define INSTANCE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "curl/curl.h"
#include "config.h"

void load_data_sources(char* instance);

#ifdef __cplusplus
}
#endif

#endif