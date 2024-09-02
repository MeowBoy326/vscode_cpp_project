#pragma once

#include "std.h"

#include "vector.h"
#include "architecture.h"

typedef enum configMode {
    modeDefault,
    modeNoAssemble,
    modeNoLink,
    modeVersion,
    modeHelp
} configMode;

typedef struct config {
    bool fail;
    configMode mode;
    bool deleteAsm;

    architecture arch;

    vector/*<char*>*/ inputs, intermediates;
    char* output;
    vector/*<char*>*/ includeSearchPaths;
} config;

config configCreate (void);
void configDestroy (config conf);

void optionsParse (config* conf, int argc, char** argv);
