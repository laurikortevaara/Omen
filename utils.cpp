//
// Created by Lauri Kortevaara(Intopalo) on 29/12/15.
//
#include <string>
#include "utils.h"
#include <stdio.h>  /* defines FILENAME_MAX */
#ifdef WINDOWS
#include <direct.h>
    #define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd

#endif

namespace Omen {
    std::string getWorkingDir() {
        char cCurrentPath[FILENAME_MAX];

        getcwd(cCurrentPath, FILENAME_MAX);
        if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath))) {
            return "";
        }

        cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
        return std::string(cCurrentPath);
    }
}