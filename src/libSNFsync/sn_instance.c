#include "sn_instance.h"

struct MemoryStruct {
  char *memory;
  size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  char *ptr = NULL, *orig = NULL;
  ptr = malloc(8);
  orig = ptr;

  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(ptr == NULL) {
    /* out of memory! */ 
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  free(orig);

  return realsize;
}

void load_sn_source_records(SN_INSTANCE *instance) {
	CURL *curl;
	CURLcode res;
	char prefix[9] = "https://\0";
	char suffix[77] = ".service-now.com/api/x_236565_file_sync/file_sync_inbound/get_configurations\0";
	char *url = NULL, *orig = NULL;
	url = malloc(sizeof(char)*400);

	strncpy(url, prefix, 9);
	strcat(url, instance->host_name);
	strcat(url, suffix);

	printf("\n\nCalling URL: %s\n\n", url);

	struct MemoryStruct chunk;

	chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */ 
	chunk.size = 0;    /* no data at this point */ 

	curl_global_init(CURL_GLOBAL_ALL);

	/* get a curl handle */
	curl = curl_easy_init();

	if(curl) {
		/* First set the URL that is about to receive our POST. This URL can
		   just as well be a https:// URL if that is what should receive the
		   data. */
		curl_easy_setopt(curl, CURLOPT_URL, url);

		/* Now specify the POST data */
		curl_easy_setopt(curl, CURLOPT_USERNAME, instance->username);
		curl_easy_setopt(curl, CURLOPT_PASSWORD, instance->password);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		/* we pass our 'chunk' struct to the callback function */ 
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if(res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));
		}
		else 
			parse_returned_sn_source_records((char *)chunk.memory);
		

		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
	free(chunk.memory);
	free(orig);
	free(url);
}

void post_attachment_to_servicenow(char *file_path, SN_SOURCE_RECORD *source_record, char *file_name) {
	CURL *curl;
	CURLcode res;
	char prefix[9] = "https://\0";
	char suffix[77] = ".service-now.com/api/now/v1/attachment/file?table_name=";
	char *url = NULL, *body = NULL, *content_type = NULL;
	struct curl_slist *list = NULL;
	FILE *fd;

	url = malloc(sizeof(char)*400);

	strncpy(url, prefix, 9);
	strcat(url, source_record->instance->host_name);
	strcat(url, suffix);
	strcat(url, source_record->table);
	strcat(url, "&table_sys_id=");
	strcat(url, source_record->record);
	strcat(url, "&file_name=");
	strcat(url, file_name);

	content_type = malloc(sizeof(char)*(14+strlen(source_record->file_type)));
	strcpy(content_type, "Content-Type:");
	strcat(content_type, source_record->file_type);

	printf("\n\nCalling URL: %s\n\n", url);
	
	fd = fopen(file_path, "rb");
	if (!fd) {
		return;
	}

	curl_global_init(CURL_GLOBAL_ALL);

	curl = curl_easy_init();

	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);

		curl_easy_setopt(curl, CURLOPT_USERNAME, source_record->instance->username);
		curl_easy_setopt(curl, CURLOPT_PASSWORD, source_record->instance->password);
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		curl_slist_append(list, "Accept:application/json");
		list = curl_slist_append(list, content_type);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

		body = read_file_to_buffer(file_path);
		printf("\nRead from File: %s\n", body);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);

		res = curl_easy_perform(curl);
		if(res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));
		}
		
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
	free(url);
}

void trigger_attachment_deletion(SN_SOURCE_RECORD *source_record) {
	CURL *curl;
	CURLcode res;
	char prefix[9] = "https://\0";
	char suffix[77] = ".service-now.com/api/x_236565_file_sync/file_sync_inbound/delete_attachments\0";
	char *url = NULL, *body = NULL;
	struct curl_slist *list = NULL;

	url = malloc(sizeof(char)*120);

	strncpy(url, prefix, 9);
	strcat(url, source_record->instance->host_name);
	strcat(url, suffix);

	printf("\n\nCalling URL: %s\n\n", url);

	curl_global_init(CURL_GLOBAL_ALL);

	curl = curl_easy_init();

	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);

		curl_easy_setopt(curl, CURLOPT_USERNAME, source_record->instance->username);
		curl_easy_setopt(curl, CURLOPT_PASSWORD, source_record->instance->password);
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		curl_slist_append(list, "Accept:application/json");
		list = curl_slist_append(list, "Content-Type:application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
		body = malloc(sizeof(char)*(100+strlen(source_record->table)));
		if (!body) {
			//implement error library
			return;
		}
		strcpy(body, "{\"target_table\":\"");
		strcat(body, source_record->table);
		strcat(body, "\",\"target_sys_id\":\"");
		strcat(body, source_record->record);
		strcat(body, "\"}\0");
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);

		res = curl_easy_perform(curl);
		if(res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));
		}
		
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
	free(url);
	free(body);
}

char* get_body_for_file_post_attachment(char *file_name, SN_SOURCE_RECORD *source_record) {
	char *body = 0;

	body = malloc(sizeof(char)+200);
	if (!body) {
		//implement error library
		return NULL;
	}

	strcpy(body, "{\'table_name\':\'");
	strcat(body, source_record->table);
	strcat(body, "\', \'table_sys_id\':\'");
	strcat(body, source_record->record);
	strcat(body, "\'}");

	printf("\n--------\nGot body:\n\n%s\n----------\n", body);
	return body;
}
