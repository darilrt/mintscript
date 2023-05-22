# [WIP] MintScript

MintScript is a multipropose interpreted programming language.

# WIP Warning

This project is still in early development and is not ready for production use. The language is still being designed and the interpreter is not stable. The syntax and semantics are subject to change at any time.

# Build instructions

## Dependencies

- Git
- Make
- C++17 compiler

Clone the repository and run `make` in the root directory.

```
git clone https://github.com/darilrt/mintscript.git
cd mintscript
make
```

Now you can run the interpreter with `./build/mint` or `./build/mint.exe` depending on your platform.

## VS 2022

If you are using Visual Studio 2022, you can open the project with the `MintScript.sln` file.

# Editor/IDE plugin

- [VSCode plugin](https://github.com/darilrt/mintscript-vscode)

# Running Examples

```
$ mint
MintScript v0.0.1
Type "exit()" to exit
>> print("Hello, World!")
Hello, World!
```