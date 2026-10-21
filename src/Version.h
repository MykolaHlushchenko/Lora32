#ifndef VERSION_H
#define VERSION_H

#ifndef BUILD_HASH
  #define BUILD_HASH "unknown"
#endif

extern const char* const APP_VERSION;
extern const char* const GIT_HASH;

void printVersion();

#endif
