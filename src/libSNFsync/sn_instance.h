#ifndef INSTANCE_H
#define INSTANCE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "curl/curl.h"
#include "config.h"
#include "file.h"


typedef struct SN_INSTANCE SN_INSTANCE;
typedef struct SN_SOURCE_RECORD SN_SOURCE_RECORD;

void load_sn_source_records(SN_INSTANCE *instance);
void post_attachment_to_servicenow(char *file_path, SN_SOURCE_RECORD *source_record, char *file_name);
char* get_body_for_file_post_attachment(char *file_path, SN_SOURCE_RECORD *source_record);
void trigger_attachment_deletion(SN_SOURCE_RECORD *source_record);

#ifdef __cplusplus
}
#endif

#endif
