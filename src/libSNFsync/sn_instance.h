#ifndef INSTANCE_H
#define INSTANCE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "curl/curl.h"
#include "config.h"

typedef struct SN_INSTANCE SN_INSTANCE;

void load_sn_source_records(SN_INSTANCE *instance);

#ifdef __cplusplus
}
#endif

#endif
