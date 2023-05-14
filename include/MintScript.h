#pragma once

#include "types.h"
#include "exception.h"
#include "mfn.h"
#include "mint.h"
#include "mfloat.h"
#include "mstr.h"
#include "mbool.h"
#include "mnull.h"
#include "mlist.h"

#include "symbol.h"
#include "object.h"
#include "symbol.h"

#include "builtin.h"

// #include "zdict.h"

void mInit();

void mShutdown();

void mRunFile(const std::string &path);

void mRunString(const std::string &source);

void mRunInteractive();