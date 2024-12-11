#include "../g_local.h"
#include "../../ext/jsmn/jsmn.h"
#include <curl/curl.h>

queueNode* outboundHead;
queueNode* outboundTail;
queueNode* inboundHead;
queueNode* inboundTail;

int killThread;
CURL* curl;

// preprocessor flags to handle both OS's handling the threaded functions in the game library.
// both in and out will have a mutex on both write and read as we expect to NULL the values after they're used.
// which can cause a crash if one call is about to write to the queue, which another call just NULL's.

#ifdef __linux__

#include <pthread.h>
#include <unistd.h>

static void* runThread(void* data);

pthread_mutex_t outboundMutex;
pthread_mutex_t inboundMutex;
pthread_t thread;

void initMutex() {
	pthread_mutex_init(&outboundMutex, NULL);
	pthread_mutex_init(&inboundMutex, NULL);
}

void acquireInboundMutex() {
	pthread_mutex_lock(&inboundMutex);
}

void acquireOutboundMutex() {
	pthread_mutex_lock(&outboundMutex);
}

void freeInboundMutex() {
	pthread_mutex_unlock(&inboundMutex);
}

void freeOutboundMutex() {
	pthread_mutex_unlock(&outboundMutex);
}

void startThread() {
	killThread = 0;
	initMutex();
	outboundHead = outboundTail = NULL;
	inboundHead = inboundTail = NULL;
	pthread_create(&thread, NULL, &runThread, NULL);
	pthread_detach(thread);
}

void closeThread() {
	killThread = 1;
	usleep(THREAD_SLEEP_DURATION * 1.5);
}


#elif defined _WIN32

#include <windows.h>
#include <process.h>


static unsigned int WINAPI runThread(void* data);

HANDLE outboundMutex;
HANDLE inboundMutex;
HANDLE thread;

void initMutex() {
	outboundMutex = CreateMutex(NULL, FALSE, NULL);
	inboundMutex = CreateMutex(NULL, FALSE, NULL);
}

void acquireInboundMutex() {
	WaitForSingleObject(inboundMutex, INFINITE);
}

void acquireOutboundMutex() {
	WaitForSingleObject(outboundMutex, INFINITE);
}

void freeInboundMutex() {
	ReleaseMutex(inboundMutex);
}

void freeOutboundMutex() {
	ReleaseMutex(outboundMutex);
}

void startThread() {
	killThread = 0;
	outboundHead = outboundTail = NULL;
	inboundHead = inboundTail = NULL;
	initMutex();
	thread = (HANDLE)_beginthreadex(0, 0, &runThread, 0, 0, 0);
}

void closeThread() {
	killThread = 1;

	WaitForSingleObject(thread, 1000);
	CloseHandle(inboundMutex);
	CloseHandle(outboundMutex);
	CloseHandle(thread);
}


#endif

int enqueueInbound(int action, int playerId, char* message, int sizeOfMessage) {

	queueNode* tmp = (queueNode*)malloc(sizeof(queueNode));

	if (!tmp) {
		// the call failed hard.
		return THREADRESPONSE_ENQUEUE_COULDNT_MALLOC;
	}

	tmp->message = (char*)malloc(sizeOfMessage);

	if (!tmp->message) {
		return THREADRESPONSE_ENQUEUE_COULDNT_MALLOC;
	}

	Q_strncpyz(tmp->message, message, sizeOfMessage);
	tmp->action = action;
	tmp->playerId = playerId;
	tmp->next = NULL;

	acquireInboundMutex();

	if (inboundHead == NULL) {
		inboundHead = inboundTail = tmp;
	}
	else {
		inboundTail->next = tmp;
		inboundTail = tmp;
	}

	freeInboundMutex();
	Com_Printf("Enqueued %d %d %s\n", action, playerId, message);
	return THREADRESPONSE_SUCCESS;
}

int enqueueOutbound(int action, int playerId, char* message, int sizeOfMessage) {

	queueNode* tmp = (queueNode*)malloc(sizeof(queueNode));

	if (!tmp) {
		// the call failed hard.
		return THREADRESPONSE_ENQUEUE_COULDNT_MALLOC;
	}

	tmp->message = (char*)malloc(sizeOfMessage);

	if (!tmp->message) {
		return THREADRESPONSE_ENQUEUE_COULDNT_MALLOC;
	}

	Q_strncpyz(tmp->message, message, sizeOfMessage);
	tmp->action = action;
	tmp->playerId = playerId;
	tmp->next = NULL;

	acquireOutboundMutex();

	if (outboundHead == NULL) {
		outboundHead = outboundTail = tmp;
	}
	else {
		outboundTail->next = tmp;
		outboundTail = tmp;
	}

	freeOutboundMutex();

	return THREADRESPONSE_SUCCESS;
}

int dequeueInbound(int* action, int* playerId, char* message, int sizeOfMessage) {

	queueNode* tmp;

	acquireInboundMutex();

	if (inboundHead == NULL) {
		freeInboundMutex();
		return THREADRESPONSE_NOTHING_ENQUEUED;
	}

	Q_strncpyz(message, inboundHead->message, sizeOfMessage);
	*action = inboundHead->action;
	*playerId = inboundHead->playerId;

	tmp = inboundHead;
	inboundHead = inboundHead->next;

	free(tmp->message);
	free(tmp);

	if (inboundHead == NULL) {
		inboundHead = inboundTail = NULL;
	}

	// free mutex.
	freeInboundMutex();

	return THREADRESPONSE_SUCCESS;

}

int dequeueOutbound(int* action, int* playerId, char* message, int sizeOfMessage) {

	queueNode* tmp;

	acquireOutboundMutex();

	if (outboundHead == NULL) {
		freeOutboundMutex();
		return THREADRESPONSE_NOTHING_ENQUEUED;
	}

	Q_strncpyz(message, outboundHead->message, sizeOfMessage);
	*action = outboundHead->action;
	*playerId = outboundHead->playerId;

	tmp = outboundHead;
	outboundHead = outboundHead->next;

	free(tmp->message);
	free(tmp);

	if (outboundHead == NULL) {
		outboundHead = outboundTail = NULL;
	}

	// free mutex.
	freeOutboundMutex();

	return THREADRESPONSE_SUCCESS;

}

void shutdownThread() {
	curl_global_cleanup();
}

// this looks ugly, but because the inside of this function is exactly the same no matter what platform we're on, this is fine.
// this function dequeues from outbound queue and enqueues into inbound queue
static 
#ifdef __linux__
void* 
#elif defined _WIN32
unsigned int WINAPI 
#endif
runThread(void* data) {

	// the body itself might change in the future, depending on what do we want to do with the thread.
	// as-is, we want to query IPHub for country and VPN check

	char message[THREAD_CURL_BIGBUF], curlOutput[THREAD_CURL_BIGBUF];
	int action = -1, playerId = -1;

	struct curl_slist* iphubCustomHeaders = NULL;

	while (!killThread) {
		int response = dequeueOutbound(&action, &playerId, message, sizeof(message));

		if (response == THREADRESPONSE_SUCCESS) {
			if (action == THREADACTION_IPHUB_DATA_REQUEST) {
				Com_Printf("GOT MSG %s", message);
				// message will be the ip address of the player.
				if (strlen(g_iphubAPIKey.string) > 0 && g_useCountryAPI.integer) {

					if (!iphubCustomHeaders) {
						iphubCustomHeaders = curl_slist_append(iphubCustomHeaders, va("X-Key: %s", g_iphubAPIKey.string));
					}

					qboolean curlResp = performCurlRequest(va("%s%s", IPHUB_API_ENDPOINT, message), iphubCustomHeaders, qfalse, curlOutput);

					if (curlResp) {
						jsmn_parser jsonParser;
						jsmntok_t jsonTokens[50];

						jsmn_init(&jsonParser);

						int response = jsmn_parse(&jsonParser, curlOutput, strlen(curlOutput), jsonTokens, 50);

						if (response) {
							char countryCode[10], countryName[100], blockLevel[10];
							for (int i = 1; i < response; i++) {

								if (jsoneq(curlOutput, &jsonTokens[i], "countryCode") == 0) {
									snprintf(countryCode, sizeof(countryCode), "%.*s", jsonTokens[i + 1].end - jsonTokens[i + 1].start, curlOutput + jsonTokens[i + 1].start);
								}
								else if (jsoneq(curlOutput, &jsonTokens[i], "countryName") == 0) {
									snprintf(countryName, sizeof(countryName), "%.*s", jsonTokens[i + 1].end - jsonTokens[i + 1].start, curlOutput + jsonTokens[i + 1].start);
								}
								else if (jsoneq(curlOutput, &jsonTokens[i], "block") == 0) {
									snprintf(blockLevel, sizeof(blockLevel), "%.*s", jsonTokens[i + 1].end - jsonTokens[i + 1].start, curlOutput + jsonTokens[i + 1].start);
								}
							}

							if (strlen(countryCode) > 0 && strlen(countryName) > 0 && strlen(blockLevel) > 0) {
								// got everything I need.
								char outputString[128];
								Q_strncpyz(outputString, va("countryCode\\%s\\countryName\\%s\\blockLevel\\%s", countryCode, countryName, blockLevel), sizeof(outputString));
								enqueueInbound(THREADACTION_IPHUB_DATA_RESPONSE, playerId, outputString, sizeof(outputString));
							}

						}
					}
				}
			}
		}
#ifdef __linux__
		usleep((unsigned int)THREAD_SLEEP_DURATION);
#elif defined _WIN32
		Sleep(THREAD_SLEEP_DURATION);
#endif
	}

	shutdownThread();
#ifdef __linux__
	return NULL;
#elif defined _WIN32
	return 0;
#endif
}

qboolean performCurlRequest(char* url, struct curl_slist* customHeaders, qboolean verifypeer, char* output) {

	struct curlProgressData curlChunk;
	CURLcode res;
	qboolean returnable = qfalse;

	curlChunk.prvt = malloc(1);
	curlChunk.size = 0;

	curl = curl_easy_init();

	curl_easy_setopt(curl, CURLOPT_URL, url);

	if (customHeaders) {
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, customHeaders);
	}

	if (!verifypeer) {
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
	}

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlCallbackWriteToChar);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&curlChunk);

	res = curl_easy_perform(curl);

	if (res == CURLE_OK) {

		Q_strncpyz(output, curlChunk.prvt, THREAD_CURL_BIGBUF);
		returnable = qtrue;
	}

	curl_easy_cleanup(curl);
	free(curlChunk.prvt);

	return returnable;

}

size_t curlCallbackWriteToChar(void* contents, size_t size, size_t nmemb, void* userp) {
	size_t realsize = size * nmemb;
	struct curlProgressData* mem = (struct curlProgressData*)userp;

	char* ptr = realloc(mem->prvt, mem->size + realsize + 1);
	if (!ptr) {
		return 0;
	}

	mem->prvt = ptr;
	memcpy(&(mem->prvt[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->prvt[mem->size] = 0;
	return realsize;
}