
void ROCmod_sendBestPlayerStats(void);
void ROCmod_sendExtraTeamInfo(gentity_t* ent);
void ROCmod_clientUpdate(gentity_t* ent, int clientNum);
void ROCmod_verifyClient(gentity_t* ent, int clientNum);

// Flags to determine which extra features the client is willing to accept in ROCmod.
#define ROC_TEAMINFO    0x00000001
#define ROC_SPECLIST    0x00000002