#include "../g_local.h"
#include "httpFunctions.h"
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
#include <wininet.h>
#pragma comment(lib, "wininet.lib")
#else
#include <pthread.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#endif

#define MAX_HTTP_REQUESTS 64

typedef enum {
    REQ_IDLE = 0,
    REQ_PENDING,
    REQ_DONE,
    REQ_ERROR
} HTTPRequestStatus;

typedef struct {
    volatile HTTPRequestStatus status;
    int clientNum;
    char secretGuid[MAX_STRING_CHARS];
    char chguid[MAX_STRING_CHARS];
} HTTPRequest_t;

static HTTPRequest_t httpRequests[MAX_HTTP_REQUESTS];

void HTTP_Init(void) {
    memset(httpRequests, 0, sizeof(httpRequests));
#ifndef _WIN32
    curl_global_init(CURL_GLOBAL_ALL);
#endif
}

void HTTP_Shutdown(void) {
#ifndef _WIN32
    curl_global_cleanup();
#endif
}

#ifdef _WIN32
DWORD WINAPI CH_VerifyThread(LPVOID lpParam) {
    int index = (int)(intptr_t)lpParam;
    HTTPRequest_t* req = &httpRequests[index];

    HINTERNET hInternet = InternetOpenA("1fxplus", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet) {
        DWORD timeout = 15000;
        InternetSetOptionA(hInternet, INTERNET_OPTION_CONNECT_TIMEOUT, &timeout, sizeof(timeout));
        InternetSetOptionA(hInternet, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));
        InternetSetOptionA(hInternet, INTERNET_OPTION_SEND_TIMEOUT, &timeout, sizeof(timeout));
        char url[512];
        _snprintf(url, sizeof(url), "https://api.ch-sof2.online/auth/verify/%s", req->secretGuid);
        url[sizeof(url) - 1] = '\0';

        HINTERNET hUrl = InternetOpenUrlA(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_SECURE, 0);
        if (hUrl) {
            char buffer[512] = {0};
            DWORD bytesRead = 0;
            if (InternetReadFile(hUrl, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
                buffer[bytesRead] = '\0';

                while(bytesRead > 0 && (buffer[bytesRead-1] == '\n' || buffer[bytesRead-1] == '\r' || buffer[bytesRead-1] == ' ')) {
                    buffer[bytesRead-1] = '\0';
                    bytesRead--;
                }

                Q_strncpyz(req->chguid, buffer, sizeof(req->chguid));
                MemoryBarrier();
                req->status = REQ_DONE;
            } else {
                req->status = REQ_ERROR;
            }
            InternetCloseHandle(hUrl);
        } else {
            req->status = REQ_ERROR;
        }
        InternetCloseHandle(hInternet);
    } else {
        req->status = REQ_ERROR;
    }

    return 0;
}
#else

typedef struct {
    char *memory;
    size_t size;
} MemoryStruct;

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    MemoryStruct *mem = (MemoryStruct *)userp;

    if (mem->size + realsize > 1024) {
        return 0;
    }

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(ptr == NULL) {
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

void* CH_VerifyThread(void* arg) {
    int index = (int)(intptr_t)arg;
    HTTPRequest_t* req = &httpRequests[index];
    CURL *curl_handle;
    CURLcode res;
    MemoryStruct chunk;

    chunk.memory = malloc(1);
    chunk.size = 0;

    curl_handle = curl_easy_init();
    if(curl_handle) {
        char url[512];
        snprintf(url, sizeof(url), "https://api.ch-sof2.online/auth/verify/%s", req->secretGuid);

        curl_easy_setopt(curl_handle, CURLOPT_URL, url);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "CheatHaram");
        curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 15L);

        res = curl_easy_perform(curl_handle);

        if(res == CURLE_OK && chunk.size > 0) {
            size_t bytesRead = chunk.size;
            // Trim
            while(bytesRead > 0 && (chunk.memory[bytesRead-1] == '\n' || chunk.memory[bytesRead-1] == '\r' || chunk.memory[bytesRead-1] == ' ')) {
                chunk.memory[bytesRead-1] = '\0';
                bytesRead--;
            }
            if(bytesRead > 0) {
                Q_strncpyz(req->chguid, chunk.memory, sizeof(req->chguid));
                __sync_synchronize();
                req->status = REQ_DONE;
            } else {
                req->status = REQ_ERROR;
            }
        } else {
            req->status = REQ_ERROR;
        }

        curl_easy_cleanup(curl_handle);
    } else {
        req->status = REQ_ERROR;
    }

    free(chunk.memory);
    return NULL;
}
#endif

void HTTP_VerifyCheatHaram(int clientNum, const char* secretGuid) {
    int i;
    if (!secretGuid || !secretGuid[0]) {
        return;
    }

    for (i = 0; i < MAX_HTTP_REQUESTS; i++) {
        if (httpRequests[i].status == REQ_IDLE) {
            httpRequests[i].clientNum = clientNum;
            Q_strncpyz(httpRequests[i].secretGuid, secretGuid, sizeof(httpRequests[i].secretGuid));
            memset(httpRequests[i].chguid, 0, sizeof(httpRequests[i].chguid));

            httpRequests[i].status = REQ_PENDING;

#ifdef _WIN32
            {
                HANDLE hThread = CreateThread(NULL, 0, CH_VerifyThread, (LPVOID)(intptr_t)i, 0, NULL);
                if (hThread) CloseHandle(hThread);
            }
#else
            {
                pthread_t thread;
                pthread_create(&thread, NULL, CH_VerifyThread, (void*)(intptr_t)i);
                pthread_detach(thread);
            }
#endif
            return;
        }
    }

    Com_Printf("WARNING: HTTP_VerifyCheatHaram queue is full! Cannot verify client %i.\n", clientNum);
}

void HTTP_Update(void) {
    int i;
    for (i = 0; i < MAX_HTTP_REQUESTS; i++) {
        HTTPRequestStatus currentStatus = httpRequests[i].status;

        if (currentStatus == REQ_DONE) {
            gentity_t* ent = &g_entities[httpRequests[i].clientNum];

            if (ent && ent->client && ent->client->pers.connected == CON_CONNECTED) {
                if (Q_stricmp(ent->client->sess.secert_guid, httpRequests[i].secretGuid) == 0) {
                    if (!ent->client->sess.hasch) {
                        G_printCustomMessage(ent, "CheatHaram Verification", "CheatHaram verification succeeded.");
                    }
                    Q_strncpyz(ent->client->sess.chguid, httpRequests[i].chguid, sizeof(ent->client->sess.chguid));
                    ent->client->sess.hasch = qtrue;
                }
            }

            httpRequests[i].status = REQ_IDLE;

        } else if (currentStatus == REQ_ERROR) {
            gentity_t* ent = &g_entities[httpRequests[i].clientNum];

            if (ent && ent->client && ent->client->pers.connected == CON_CONNECTED) {
                if (Q_stricmp(ent->client->sess.secert_guid, httpRequests[i].secretGuid) == 0) {
                    ent->client->sess.hasch = qfalse;
                    G_printCustomMessage(ent, "CheatHaram Verification", "CheatHaram verification failed.");
                }
            }

            httpRequests[i].status = REQ_IDLE;
        }
    }
}
