#pragma once

#include "symbol.h"
#include "object.h"
#include "symbol.h"

#include "builtin.h"

// #include "zdict.h"

void mInit();

void mShutdown();

void mRunFile(const std::string &path);

void mRunString(const std::string &source);

void mTest();

void mRunInteractive();