#ifndef GAME_HARDWARE_H
#define GAME_HARDWARE_H
#include "common.h"

typedef struct {
    u32 id;
    ComponentType type;
    char name[64];
    
    /* Engineering variables */
    f32 power_draw;     /* Watts */
    f32 power_supply;   /* Watts */
    
    u32 clock_hz;       
    u32 memory_bytes;   
    u32 release_year;   /* The year this item enters the catalog */
} ComponentDef;

typedef struct {
    char name[64];
    bool is_assembled;
    bool is_powered_on;  /* False = shutdown completely */
    bool is_crashed;     /* E.g., Power exceeded */
    
    ComponentDef parts[COMP_TYPE_COUNT];
    
    f32 total_power_draw;
    f32 max_power_supply;
    
    u32 sys_clock_hz;
    u32 sys_memory_bytes;
} Computer;

void hardware_init(void);

/* Changed to strictly filter by the simulated global year */
int hardware_get_components(int current_year, ComponentType type, ComponentDef* out_list, int max_out);
const char* component_type_name(ComponentType t);

/* System logic */
void computer_assemble(Computer* c, const char* name, ComponentDef parts[COMP_TYPE_COUNT]);
void computer_power_toggle(Computer* c);

#endif
