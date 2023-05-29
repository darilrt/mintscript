#include "MintScript.h"

#include <iostream>

int main(int argc, char** argv) {

    mInit();
    if (argc > 1) {
        mRunFile(argv[1]);
    } else {
        mRunInteractive();
    }
    mShutdown();

    return 0;
}

