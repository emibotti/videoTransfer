#include <string>
using namespace std;

#if !defined(PROTOCOL_CONSTANTS_H)
#define PROTOCOL_CONSTANTS_H 1

// Bindings
static const int PORT = 8888;

// Definidos como ASCII
static const string INIT = "init";
static const string PLAY = "play";
static const string PAUSE = "pause";
static const string STOP = "stop";
static const string CLOSE = "close";

#endif