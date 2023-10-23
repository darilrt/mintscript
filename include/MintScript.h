#pragma once

#include <string>

void mInit();

void mShutdown();

void mRunFile(const std::string &path);

void mRunString(const std::string &source);

void mTest();

void mRunInteractive();