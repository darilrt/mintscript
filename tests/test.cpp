#include "test.h"
#include "lexer.h"

#include <iostream>
#include <Windows.h>

#define TEST(test_function) \
    std::cout << "Running test: " << #test_function << " "; test_function(); std::cout << "\t✓" << std::endl;

void Tests() {
    SetConsoleOutputCP(CP_UTF8);

    TEST(TestLexerNumberLiterals);
    TEST(TestLexerKeywords);
    TEST(TestLexerSymbols);
    TEST(TestLexerStrings);
    TEST(TestLexerIdentifiers);

    TEST(TestScanner);
}

int main() {
    Tests();
    return 0;
}