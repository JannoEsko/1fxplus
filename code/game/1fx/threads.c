#include "../g_local.h"
#include "../../ext/yyjson/yyjson.h"

#ifdef _WIN32
#include <winsock2.h>
#elif defined __linux__ || defined __APPLE__
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif


queueNode* outboundHead;
queueNode* outboundTail;
queueNode* inboundHead;
queueNode* inboundTail;

int killThread;
CURL* curl;
int sockhandle;
struct sockaddr_in svaddr;

typedef enum sockStatus_e {
    SOCKSTATUS_CLOSED,
    SOCKSTATUS_CONNECTING,
    SOCKSTATUS_CONNECTED
} sockStatus_t;

sockStatus_t socketStatus = SOCKSTATUS_CLOSED;
int socketRetries = 0;
int nextRetry = 0;

static void cleanupQueue(queueNode** head) {
    queueNode* current = *head;
    while (current) {
        queueNode* temp = current;
        current = current->next;
        free(temp->message);
        free(temp);
    }
    *head = NULL;
}

// preprocessor flags to handle both OS's handling the threaded functions in the game library.
// both in and out will have a mutex on both write and read as we expect to NULL the values after they're used.
// which can cause a crash if one call is about to write to the queue, which another call just NULL's.

#if defined __linux__ || defined __APPLE__

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
    if (killThread != 1) {
        killThread = 1;
        usleep(THREAD_SLEEP_DURATION * 1.5);

        acquireInboundMutex();
        cleanupQueue(&inboundHead);
        freeInboundMutex();

        acquireOutboundMutex();
        cleanupQueue(&outboundHead);
        freeOutboundMutex();

        pthread_mutex_destroy(&inboundMutex);
        pthread_mutex_destroy(&outboundMutex);

        if (socketStatus) {
            shutdown(sockhandle, SHUT_WR);
            close(sockhandle);
        }

    }
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
    WSADATA wsaData;
    if (!WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        thread = (HANDLE)_beginthreadex(0, 0, &runThread, 0, 0, 0);
    }
}

void closeThread() {

    if (killThread != 1) {
        killThread = 1;

        WaitForSingleObject(thread, 1000);

        acquireInboundMutex();
        cleanupQueue(&inboundHead);
        freeInboundMutex();

        acquireOutboundMutex();
        cleanupQueue(&outboundHead);
        freeOutboundMutex();

        CloseHandle(inboundMutex);
        CloseHandle(outboundMutex);
        CloseHandle(thread);

        if (socketStatus) {
            shutdown(sockhandle, SD_SEND);
            closesocket(sockhandle);
        }

        WSACleanup();
    }
}


#endif

int enqueueInbound(int action, char* message, int sizeOfMessage) {

    if (killThread) {
        return THREADRESPONSE_THREAD_STOPPED;
    }

    queueNode* tmp = (queueNode*)malloc(sizeof(queueNode));

    if (!tmp) {
        // the call failed hard.
        return THREADRESPONSE_ENQUEUE_COULDNT_MALLOC;
    }
    sizeOfMessage++;
    tmp->message = (char*)malloc(sizeOfMessage);

    if (!tmp->message) {
        return THREADRESPONSE_ENQUEUE_COULDNT_MALLOC;
    }

    Q_strncpyz(tmp->message, message, sizeOfMessage);
    tmp->action = action;
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

    return THREADRESPONSE_SUCCESS;
}

int enqueueOutbound(int action, char* message, int sizeOfMessage) {

    if (killThread) {
        return THREADRESPONSE_THREAD_STOPPED;
    }

    queueNode* tmp = (queueNode*)malloc(sizeof(queueNode));

    if (!tmp) {
        // the call failed hard.
        return THREADRESPONSE_ENQUEUE_COULDNT_MALLOC;
    }
    sizeOfMessage++;
    tmp->message = (char*)malloc(sizeOfMessage);

    if (!tmp->message) {
        return THREADRESPONSE_ENQUEUE_COULDNT_MALLOC;
    }

    Q_strncpyz(tmp->message, message, sizeOfMessage);
    tmp->action = action;
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

int dequeueInbound(int* action, char* message, int sizeOfMessage) {

    if (killThread) {
        return THREADRESPONSE_THREAD_STOPPED;
    }

    queueNode* tmp;

    acquireInboundMutex();

    if (inboundHead == NULL) {
        freeInboundMutex();
        return THREADRESPONSE_NOTHING_ENQUEUED;
    }

    Q_strncpyz(message, inboundHead->message, sizeOfMessage);
    *action = inboundHead->action;

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

int dequeueOutbound(int* action, char* message, int sizeOfMessage) {

    if (killThread) {
        return THREADRESPONSE_THREAD_STOPPED;
    }

    queueNode* tmp;

    acquireOutboundMutex();

    if (outboundHead == NULL) {
        freeOutboundMutex();
        return THREADRESPONSE_NOTHING_ENQUEUED;
    }

    Q_strncpyz(message, outboundHead->message, sizeOfMessage);
    *action = outboundHead->action;

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
#ifdef _WIN32
    shutdown(sockhandle, SD_BOTH);
    closesocket(sockhandle);
#elif defined __linux__ || defined __APPLE__
    shutdown(sockhandle, SHUT_RDWR);
    close(sockhandle);
#endif
    socketStatus = qfalse;
}

// this looks ugly, but because the inside of this function is exactly the same no matter what platform we're on, this is fine.
// this function dequeues from outbound queue and enqueues into inbound queue
static 
#if defined __linux__ || defined __APPLE__
void* 
#elif defined _WIN32
unsigned int WINAPI 
#endif
runThread(void* data) {

    // the body itself might change in the future, depending on what do we want to do with the thread.
    // as-is, we want to query IPHub for country and VPN check

    char message[THREAD_CURL_BIGBUF], curlOutput[THREAD_CURL_BIGBUF];
    int action = -1;

    struct curl_slist* iphubCustomHeaders = NULL;


    while (!killThread) {

        // full new approach:
        // we will ALWAYS recv, remove blocking states. If nothing to recv, just continue.
        // That means expectResponse is now useless and should be removed. 
        // Still need an idea on how to handle responses which previously were "expected".

        // First setup the socket if we need to.

        if (g_useSockets.integer) {
            if (socketStatus == SOCKSTATUS_CLOSED && (level.time >= nextRetry || !nextRetry)) {
                nextRetry = level.time + 3000;
                if (socketRetries <= 20) {
                    socketRetries++;

#ifdef _WIN32
                    sockhandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#elif defined __linux__ || defined __APPLE__
                    sockhandle = socket(AF_INET, SOCK_STREAM, 0);
#endif
                    if (sockhandle >= 0) {
                        Com_Memset(&svaddr, 0, sizeof(svaddr));
                        svaddr.sin_family = AF_INET;
                        svaddr.sin_port = htons(g_sockPort.integer);
                        inet_pton(AF_INET, g_sockIp.string, &svaddr.sin_addr);
#ifdef _WIN32
                        u_long mode = 1;
                        ioctlsocket(sockhandle, FIONBIO, &mode); // Set the socket to be non-blocking
#elif defined(__linux__) || defined(__APPLE__)
                        int flags = fcntl(sockhandle, F_GETFL, 0);
                        fcntl(sockhandle, F_SETFL, flags | O_NONBLOCK);
#endif
                        int r = connect(sockhandle, (struct sockaddr*)&svaddr, sizeof(svaddr));
#ifdef _WIN32
                        if (r < 0) {
                            int err = WSAGetLastError();
                            if (err != WSAEWOULDBLOCK && err != WSAEINPROGRESS) {
                                closesocket(sockhandle);
                                sockhandle = INVALID_SOCKET;
                                socketStatus = SOCKSTATUS_CLOSED;
                            }
                        }
#elif defined(__linux__) || defined(__APPLE__)
                        if (r < 0 && errno != EINPROGRESS) {
                            close(sockhandle);
                            sockhandle = INVALID_SOCKET;
                            socketStatus = SOCKSTATUS_CLOSED;
                        }
#endif
                        // NB - the connection might not yet be ready.
                        socketStatus = SOCKSTATUS_CONNECTING;
                        socketRetries = 0; // reset retries on success
                    }
                }
            }

            if (socketStatus == SOCKSTATUS_CONNECTING) {
                // Check if the socket is now connected.

                fd_set writefds, exceptfds;
                struct timeval tv = { 0, 0 };
                FD_ZERO(&writefds);
                FD_ZERO(&exceptfds);
                FD_SET(sockhandle, &writefds);
                FD_SET(sockhandle, &exceptfds);

                int sel = select(sockhandle + 1, NULL, &writefds, &exceptfds, &tv);

                if (sel > 0) {
                    if (FD_ISSET(sockhandle, &exceptfds)) {
#ifdef _WIN32
                        shutdown(sockhandle, SD_BOTH);
                        closesocket(sockhandle);
#elif defined __linux__ || defined __APPLE__
                        shutdown(sockhandle, SHUT_RDWR);
                        close(sockhandle);
#endif
                        socketStatus = SOCKSTATUS_CLOSED;
                        sockhandle = INVALID_SOCKET;
                    } else if (FD_ISSET(sockhandle, &writefds)) {

                        int err = 0;
                        socklen_t len = sizeof(err);
                        if (getsockopt(sockhandle, SOL_SOCKET, SO_ERROR, &err, &len) < 0 || err != 0) {
#ifdef _WIN32
                            shutdown(sockhandle, SD_BOTH);
                            closesocket(sockhandle);
#elif defined __linux__ || defined __APPLE__
                            shutdown(sockhandle, SHUT_RDWR);
                            close(sockhandle);
#endif
                            socketStatus = SOCKSTATUS_CLOSED;
                            sockhandle = INVALID_SOCKET;
                        }
                        else {
                            socketStatus = SOCKSTATUS_CONNECTED;
                        }
                    }
                }
                else if (sel < 0) {
#ifdef _WIN32
                    shutdown(sockhandle, SD_BOTH);
                    closesocket(sockhandle);
#elif defined __linux__ || defined __APPLE__
                    shutdown(sockhandle, SHUT_RDWR);
                    close(sockhandle);
#endif
                    socketStatus = SOCKSTATUS_CLOSED;
                    sockhandle = INVALID_SOCKET;
                }
            }

        }

        // Outbound part remains largely the same. We just don't expect a synchronous response any more.

        int response = dequeueOutbound(&action, message, sizeof(message));
        qboolean shutdownSocket = qfalse;
        // If we have something in the queue, process, if not, recv.

        if (response == THREADRESPONSE_SUCCESS) {
            if (action == THREADACTION_IPHUB_DATA_REQUEST) {
                // message will be the ip address of the player.
                if (strlen(g_iphubAPIKey.string) > 0 && g_useCountryAPI.integer) {

                    if (!iphubCustomHeaders) {
                        iphubCustomHeaders = curl_slist_append(iphubCustomHeaders, va("X-Key: %s", g_iphubAPIKey.string));
                    }

                    qboolean curlResp = performCurlRequest(va("%s%s", IPHUB_API_ENDPOINT, message), iphubCustomHeaders, qfalse, curlOutput);

                    if (curlResp) {

                        yyjson_doc* doc = yyjson_read(curlOutput, strlen(curlOutput), 0);
                        if (doc) {

                            yyjson_val* root = yyjson_doc_get_root(doc);

                            if (yyjson_is_obj(root)) {

                                char countryCode[MAX_COUNTRYCODE], countryName[MAX_COUNTRYNAME], blockLevel[10], cleanCountry[MAX_COUNTRYNAME], ipAddr[MAX_IP];
                                Com_Memset(countryCode, 0, sizeof(countryCode));
                                Com_Memset(countryName, 0, sizeof(countryName));
                                Com_Memset(blockLevel, 0, sizeof(blockLevel));
                                Com_Memset(cleanCountry, 0, sizeof(cleanCountry));
                                Com_Memset(ipAddr, 0, sizeof(ipAddr));

                                yyjson_val* ctryVal = yyjson_obj_get(root, "countryCode");
                                yyjson_val* ctryNameVal = yyjson_obj_get(root, "countryName");
                                yyjson_val* blockVal = yyjson_obj_get(root, "block");
                                yyjson_val* ipVal = yyjson_obj_get(root, "ip");

                                if (yyjson_is_str(ctryVal)) {
                                    Q_strncpyz(countryCode, yyjson_get_str(ctryVal), sizeof(countryCode));
                                }

                                if (yyjson_is_str(ctryNameVal)) {
                                    Q_strncpyz(countryName, yyjson_get_str(ctryNameVal), sizeof(countryName));
                                }

                                if (yyjson_is_str(blockVal)) {
                                    Q_strncpyz(blockLevel, yyjson_get_str(blockVal), sizeof(blockLevel));
                                }
                                else if (yyjson_is_int(blockVal)) {
                                    int blockInt = (int)yyjson_get_int(blockVal);
                                    Com_sprintf(blockLevel, sizeof(blockLevel), "%d", blockInt);
                                }

                                if (yyjson_is_str(ipVal)) {
                                    Q_strncpyz(ipAddr, yyjson_get_str(ipVal), sizeof(ipAddr));
                                }

                                if (strlen(countryCode) > 0 && strlen(countryName) > 0 && strlen(blockLevel) > 0 && strlen(ipVal) > 0) {
                                    // got everything I need.
                                    char outputString[MAX_THREAD_OUTPUT];
                                    char* ctryPtr = countryName;
                                    char* cleanPtr = cleanCountry;
                                    char* cleanEnd = cleanCountry + sizeof(cleanCountry) - 1;

                                    while (*ctryPtr && cleanPtr < cleanEnd) {
                                        if (*ctryPtr != '\\') {
                                            *cleanPtr++ = *ctryPtr;
                                        }
                                        ctryPtr++;
                                    }
                                    *cleanPtr = '\0';

                                    Q_strncpyz(outputString, va("countryCode\\%s\\countryName\\%s\\blockLevel\\%s\\ipaddr\\%s", countryCode, cleanCountry, blockLevel, ipAddr), sizeof(outputString));
                                    enqueueInbound(THREADACTION_IPHUB_DATA_RESPONSE, outputString, sizeof(outputString));
                                }

                            }

                            yyjson_doc_free(doc);

                        }
                    }
                }
            }
            else if (action == THREADACTION_LOG_VIA_SOCKET) {

                if (g_useSockets.integer && g_logThroughSocket.integer) {

                    if (socketStatus != SOCKSTATUS_CONNECTED || sockhandle <= 0) {
                        // Can't log via socket if not connected. Because dequeue is effectively a pop, push it back.
                        enqueueOutbound(action, message, strlen(message));
                    }
                    else {
                        size_t msgLen = strlen(message) + 1; // include terminating \0 which caused issues in the previous version
                        size_t sent = 0;
                        while (sent < msgLen) {
                            int n = send(sockhandle, message + sent, (int)(msgLen - sent), 0);
                            if (n <= 0) {
                                shutdownSocket = qtrue;
                                break;
                            }
                            sent += n;
                        }
                    }

                }
            }
        }

        // recv part

        if (g_useSockets.integer && socketStatus == SOCKSTATUS_CONNECTED && sockhandle >= 0 && !shutdownSocket) {
            static char recvBuffer[THREAD_CURL_BIGBUF];
            static size_t recvLen = 0;

            if (recvLen >= sizeof(recvBuffer) - 1) {
                // Should not happen, but just in case, reset buffer, this will in effect drop the message.
                recvLen = 0;
            }

            int n = recv(sockhandle, recvBuffer + recvLen, (int)(sizeof(recvBuffer) - 1 - recvLen), 0);
            if (n == 0) {
                shutdownSocket = qtrue;
            }
            else if (n < 0) {
#ifdef _WIN32
                int err = WSAGetLastError();
                if (err != WSAEWOULDBLOCK) {
                    
#elif defined __linux__ || defined __APPLE__
                if (errno != EWOULDBLOCK && errno != EAGAIN) {
#endif
                    // Assume an actual issue.
                    shutdownSocket = qtrue;
                }
            }
            else {

                recvLen += n;

                size_t i = 0;
                while (i < recvLen) {
                    size_t msgEnd = i;
                    while (msgEnd < recvLen && recvBuffer[msgEnd] != '\0') msgEnd++; // find null terminator, fixes partial reads. NB - Server ALWAYS HAS TO SEND \0 in the end

                    if (msgEnd == recvLen) break;

                    enqueueInbound(THREADACTION_SOCKET_RESPONSE, recvBuffer + i, (int)(msgEnd - i));

                    if (msgEnd + 1 < recvLen) {
                        memmove(recvBuffer, recvBuffer + msgEnd + 1, recvLen - (msgEnd + 1));
                    }
                    recvLen -= (msgEnd + 1);
                    i = 0;
                }
            }
        }

        if (shutdownSocket) {
#ifdef _WIN32
            shutdown(sockhandle, SD_BOTH);
            closesocket(sockhandle);
#elif defined __linux__ || defined __APPLE__
            shutdown(sockhandle, SHUT_RDWR);
            close(sockhandle);
#endif
            socketStatus = SOCKSTATUS_CLOSED;
            sockhandle = INVALID_SOCKET;
        }

#if defined __linux__ || defined __APPLE__
        usleep((unsigned int)THREAD_SLEEP_DURATION);
#elif defined _WIN32
        Sleep(THREAD_SLEEP_DURATION);
#endif
    }

    if (iphubCustomHeaders) {
        curl_slist_free_all(iphubCustomHeaders);
    }

    shutdownThread();
#if defined __linux__ || defined __APPLE__
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