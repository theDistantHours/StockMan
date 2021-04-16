#pragma once
#include "commons.h"
const char useful1[] = "Dc6UYSh2";
const char useless1[] = "once3kzg";
const char useful2[] = "6ISzQ91T";
const char useless2[] = "ZcRmQm01";
const char useful3[] = "B3BDHR8o";
const char useless3[] = "7HSnPwDT";
const char useful4[] = "B6VvieY9";
const char useless4[] = "JcLBPJGi";

const char getAESKey();
const char encrypt(const char* src, const char* buffer, unsigned long long int size_max);
const char decrypt(const char* src, const char* buffer, unsigned long long int size_max);
