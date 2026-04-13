#include "game/market.h"

void market_sell_computer(Computer* comp, double* company_cash) {
    if(comp->assembled) {
        f32 sale_price = comp->total_cost * 2.5f; /* simple profit margin */
        *company_cash += sale_price;
        comp->assembled = false; /* "sold" */
    }
}
