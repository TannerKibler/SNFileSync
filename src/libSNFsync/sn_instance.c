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

void load_data_sources(char* instance) {
	CURL *curl;
	CURLcode res;
	char prefix[9] = "https://\0";
	char suffix[67] = ".service-now.com/api/236565/mid_server_file_share/get_data_sources\0";
	char *url = NULL, *orig = NULL;
	url = malloc(sizeof(char)*400);
	//orig = url; //storing pointer to beginning of url before concatting. This is so I can free it later.

	strncpy(url, prefix, 9);
	strcat(url, instance);
	strcat(url, suffix);

	printf("Calling url: %s\n", url);

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
		curl_easy_setopt(curl, CURLOPT_USERNAME, "admin");
		curl_easy_setopt(curl, CURLOPT_PASSWORD, "Kibbles42");
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
		else {
			/*
			 * Now, our chunk.memory points to a memory block that is chunk.size
			 * bytes big and contains the remote file.
			 *
			 * Do something nice with it!
			 */ 



			printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
			//printf("Received %s\n", chunk.memory);
			parse_returned_data_sources((char *)chunk.memory);
		}

		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
	free(chunk.memory);
	free(orig);
	//return 0;
}