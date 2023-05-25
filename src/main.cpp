#include "MintScript.h"

#include <iostream>

int main(int argc, char** argv) {
    // if file is passed as argument, run it
    // else run interactive mode

    mInit();
    if (argc > 1) {
        mRunFile(argv[1]);
    } else {
        mRunFile("examples/test.mint");
        mRunInteractive();
    }
    mShutdown();

    return 0;
}
