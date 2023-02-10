#include "../g_local.h"
#include "1fxFunctions.h"

// point of this file is to coordinate all logging activities in one place.
// this file will call out whatever logging function it needs to perform the action

void logGame() {

}

void logSystem(int faultLevel, char* message) {

}

void logAdmin() {

}

void logRcon(char* ip, char* action) {
    if (g_logToDatabase.integer) {
        dbLogRcon(ip, action);
    }
}

void logDamage() {

}

void logObjective() {

}

void logLogin(char* player, char* ip, int level, int method, char* reference) {

}
