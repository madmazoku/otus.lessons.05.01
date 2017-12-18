#include "stdafx.h"

#include "global.h"

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> urd(0.0, 1.0);
