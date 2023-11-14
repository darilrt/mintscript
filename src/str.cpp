#include "builtin.h"
#include "MintScript.h"

ir::Mainfold str_Concat(std::vector<ir::Mainfold> args) {
    return {
        ir::Mainfold::String,
        new std::string(*args[0].value.s + *args[1].value.s)
    };
}

void mint_Str() {
    t_str = mint::Type("str", { }, { });

    mint::Extend("str", { }, {
        { "Concat", { t_str, t_str } },
    });
}