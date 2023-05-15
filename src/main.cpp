#include "MintScript.h"

#include <iostream>

int main() {
    mInit();
    // mRunInteractive();
    mRunFile("examples/test.ms");
    mShutdown();
    return 0;
}
