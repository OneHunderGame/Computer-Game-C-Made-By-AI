#include "game/company.h"

void company_init(Company* c, const char* name, double starting_cash) {
    strncpy(c->name, name, sizeof(c->name) - 1);
    c->cash = starting_cash;
}
