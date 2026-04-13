#ifndef GAME_COMPANY_H
#define GAME_COMPANY_H
#include "common.h"

typedef struct {
    char name[64];
    double cash;
} Company;

void company_init(Company* c, const char* name, double starting_cash);

#endif
