#include "MintScript.h"
#include "ir.h"

#include <iostream>

int main(int argc, char** argv) {
    
    mInit();

    if (argc == 1) {
        mRunInteractive();
    } else {
        mRunFile(argv[1]);
    }

    mShutdown();

    return 0;
}

