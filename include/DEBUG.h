#ifndef DEBUG_H
#define DEBUG_H
#if DEBUG
#define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG_BEGIN(...) Serial.begin(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...) ((void)0)
#define DEBUG_PRINTLN(...) ((void)0)
#define DEBUG_PRINT(...) ((void)0)
#define DEBUG_BEGIN(...) ((void)0)
#endif
#endif