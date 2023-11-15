#include <iostream>

#include "MintScript.h"

ir::Mainfold test(std::vector<ir::Mainfold> args) {
    std::cout << "Hello from lib!" << std::endl;
    return { ir::Mainfold::Null };
}

extern "C" void mint_Root() {
    mint::Function("test", { t_void } , test);
}