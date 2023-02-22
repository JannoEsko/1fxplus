
#include <curl/curl.h>
// queue struct.
// this is to hand over commands to the thread
// and to respond to the main thread.

typedef struct queueNode_s queueNode;

struct queueNode_s {
	int action;
	int playerId;
	char* message; // in outgoing calls, this can be the value we want to request from whatever (e.g. OTP) 
				   // in incoming calls, this will be the response structured with \.
	struct queueNode_s* next;
};

typedef enum {
	THREADRESPONSE_SUCCESS,
	THREADRESPONSE_NOTHING_ENQUEUED,
	THREADRESPONSE_ENQUEUE_COULDNT_MALLOC
} threadResponse;

typedef enum {
	IPHUB_DATA_REQUEST,
	IPHUB_DATA_RESPONSE,
	RUN_PRINTF
} threadAction;

struct curlProgressData {
	char* prvt;
	size_t size;
};

#define THREAD_CURL_BIGBUF 8192

#ifdef __linux__
#define THREAD_SLEEP_DURATION 50000
#elif defined _WIN32
#define THREAD_SLEEP_DURATION 100
#endif

#define IPHUB_API_ENDPOINT "http://v2.api.iphub.info/ip/"

#ifdef __linux__
void*
#elif defined _WIN32
unsigned int WINAPI
#endif
runThread(void* data);

qboolean performCurlRequest(char* url, struct curl_slist* customHeaders, qboolean verifypeer, char* output);
size_t curlCallbackWriteToChar(void* contents, size_t size, size_t nmemb, void* userp);
void shutdownThread(void);
int dequeueOutbound(int* action, int* playerId, char* message);
int dequeueInbound(int* action, int* playerId, char* message);
int enqueueOutbound(int action, int playerId, char* message);
int enqueueInbound(int action, int playerId, char* message);

void initMutex(void);
void acquireInboundMutex(void);
void acquireOutboundMutex(void);
void freeInboundMutex(void);
void freeOutboundMutex(void);
void startThread(void);
void closeThread(void);