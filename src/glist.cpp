#include "builtin.h"

#include "MintScript.h"

class GList : public ir::Object {
public:
    std::vector<ir::Mainfold> list;

    GList() : ir::Object(0) { };
};

ir::Mainfold glist_New(std::vector<ir::Mainfold> args) {
    return { ir::Mainfold::Object, (ir::Object*) new GList };
}

ir::Mainfold glist_Push(std::vector<ir::Mainfold> args) {
    GList* list = (GList*) args[0].value.st;
    list->list.push_back(args[1]);
    
    return { ir::Mainfold::Object };
}

ir::Mainfold glist_At(std::vector<ir::Mainfold> args) {
    const GList* list = (GList*) args[0].value.st;
    const int index = args[1].value.i;

    return list->list[index];
}

void mint_GList() {
    sa::Type* t_glist = mint::Type("GList",  { }, { });    

    mint::Extend("GList", { }, {
        { "GList", { t_glist }, glist_New },
        { "Push", { t_void, t_int }, glist_Push },
        { "At", { t_int, t_int }, glist_At },
    });
}