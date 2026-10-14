#ifndef __HTTP_FUNCTIONS_H__
#define __HTTP_FUNCTIONS_H__

void HTTP_Init(void);
void HTTP_Shutdown(void);
void HTTP_VerifyCheatHaram(int clientNum, const char* secretGuid);
void HTTP_Update(void);
void CheckCheatHaram(int clientNum);

#endif // __HTTP_FUNCTIONS_H__
