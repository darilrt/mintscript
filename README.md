### MintScript

MintScript is a multipropose programming language.

file extension: .ms

#### MintScript internal structure

program
|
|--- module
|    |--- submodules -> module..
|    |
|    |--- interface
|    |    
|    |--- class
|    |    |--- fields
|    |    |--- methods
|    |
|    |--- fields
|    |--- methods

#### MintScript c++ api

```c++
#include <MintScript.h>

int main(int argc, char** argv) {
    // Create a new zen script interpreter
    zs::Interpreter* interpreter = new zs::Interpreter();

    // Get zs module
    zs::Object* zs = interpreter->getModule("zs");

    // Get print function
    zs::Object* print = zs->get("print");

    // Call print function
    print->call("Hello World!");

    // Delete print reference
    delete print;

    // Delete zs reference
    delete zs;

    // Delete interpreter
    delete interpreter;
    
    return 0;
}
```

