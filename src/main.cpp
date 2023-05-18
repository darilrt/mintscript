#include "MintScript.h"

#include <iostream>

int main() {
    mInit();
    // mRunInteractive();
    mRunFile("examples/test.ms");
    // mTest();
    mShutdown();
    return 0;
}
