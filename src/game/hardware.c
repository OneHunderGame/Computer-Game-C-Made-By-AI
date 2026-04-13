#include "game/hardware.h"

static ComponentDef s_components[] = {
    /* -- CPUs -- */
    {1, COMP_CPU, "Vacuum Tube ALU", 120.0f, 0.0f, 1000, 0, 1950},
    {2, COMP_CPU, "Early Transistor CPU", 45.0f, 0.0f, 50000, 0, 1960},
    {3, COMP_CPU, "Discrete Logic Chip", 60.0f, 0.0f, 200000, 0, 1965},
    {4, COMP_CPU, "16-bit Microprocessor", 85.0f, 0.0f, 1000000, 0, 1970},
    {5, COMP_CPU, "Intel 8086", 110.0f, 0.0f, 4770000, 0, 1978},
    {6, COMP_CPU, "AMD Am286", 120.0f, 0.0f, 8000000, 0, 1982},
    {7, COMP_CPU, "Intel 80386DX", 150.0f, 0.0f, 33000000, 0, 1985},
    {8, COMP_CPU, "AMD Am486DX", 185.0f, 0.0f, 100000000, 0, 1993},

    /* -- RAM -- */
    {10, COMP_RAM, "Delay Line Memory", 50.0f, 0.0f, 0, 512, 1950},
    {11, COMP_RAM, "Magnetic Core RAM", 25.0f, 0.0f, 0, 4096, 1960},
    {12, COMP_RAM, "Static RAM Board", 15.0f, 0.0f, 0, 16384, 1970},
    {13, COMP_RAM, "Dynamic RAM Array", 20.0f, 0.0f, 0, 65536, 1975},
    {14, COMP_RAM, "256KB SIMM Mod", 30.0f, 0.0f, 0, 262144, 1982},
    {15, COMP_RAM, "1MB SDRAM", 45.0f, 0.0f, 0, 1048576, 1988},

    /* -- STORAGE -- */
    {20, COMP_STORAGE, "Punch Tape Reader", 30.0f, 0.0f, 0, 1024, 1950},
    {21, COMP_STORAGE, "Magnetic Tape Drive", 45.0f, 0.0f, 0, 1048576, 1960},
    {22, COMP_STORAGE, "Early HDD (5MB)", 120.0f, 0.0f, 0, 5242880, 1975},
    {23, COMP_STORAGE, "Seagate IDE (40MB)", 80.0f, 0.0f, 0, 41943040, 1985},
    {24, COMP_STORAGE, "Maxtor HDD (250MB)", 95.0f, 0.0f, 0, 262144000, 1992},

    /* -- DISPLAYS -- */
    {30, COMP_DISPLAY, "Teletype Printer", 80.0f, 0.0f, 0, 0, 1950},
    {31, COMP_DISPLAY, "Oscilloscope CRT", 60.0f, 0.0f, 0, 0, 1960},
    {32, COMP_DISPLAY, "Monochrome VD Unit", 40.0f, 0.0f, 0, 0, 1970},
    {33, COMP_DISPLAY, "IBM CGA Color Display", 95.0f, 0.0f, 0, 0, 1981},
    {34, COMP_DISPLAY, "Sony Trinitron VGA", 150.0f, 0.0f, 0, 0, 1987},

    /* -- MOTHERBOARDS -- */
    {40, COMP_MOTHERBOARD, "Wire-wrapped Backplane", 15.0f, 0.0f, 0, 0, 1950},
    {41, COMP_MOTHERBOARD, "Standard PCB", 10.0f, 0.0f, 0, 0, 1965},
    {42, COMP_MOTHERBOARD, "High-Density Logic Board", 25.0f, 0.0f, 0, 0, 1975},
    {43, COMP_MOTHERBOARD, "Gigabyte AX-Motherboard", 35.0f, 0.0f, 0, 0, 1980},
    {44, COMP_MOTHERBOARD, "MSI Pro Chipset", 45.0f, 0.0f, 0, 0, 1986},
    {45, COMP_MOTHERBOARD, "Asus Socket 3 Board", 55.0f, 0.0f, 0, 0, 1992},

    /* -- POWER SUPPLIES -- */
    {50, COMP_PSU, "Bench Transformer", 0.0f, 200.0f, 0, 0, 1950},
    {51, COMP_PSU, "Standard Power Box", 0.0f, 400.0f, 0, 0, 1965},
    {52, COMP_PSU, "Heavy Duty PSU", 0.0f, 750.0f, 0, 0, 1975},
    {53, COMP_PSU, "Industrial Generator", 0.0f, 1500.0f, 0, 0, 1980},

    /* -- CASES -- */
    {60, COMP_CASE, "Metal Rack", 0.0f, 0.0f, 0, 0, 1950},
    {61, COMP_CASE, "Wooden Cabinet", 0.0f, 0.0f, 0, 0, 1960},
    {62, COMP_CASE, "Desktop Chassis", 0.0f, 0.0f, 0, 0, 1970},
    {63, COMP_CASE, "Beige ATX Tower", 0.0f, 0.0f, 0, 0, 1985},
};

void hardware_init(void) {}

int hardware_get_components(int current_year, ComponentType type, ComponentDef* out_list, int max_out) {
    int count = 0;
    for(int i=0; i<(int)ARRAY_LEN(s_components) && count<max_out; ++i) {
        if(s_components[i].type == type && s_components[i].release_year <= (u32)current_year) {
            out_list[count++] = s_components[i];
        }
    }
    return count;
}

void computer_assemble(Computer* c, const char* name, ComponentDef parts[COMP_TYPE_COUNT]) {
    memset(c, 0, sizeof(*c));
    strncpy(c->name, name, sizeof(c->name)-1);
    
    for(int i=0; i<COMP_TYPE_COUNT; ++i) {
        c->parts[i] = parts[i];
        c->total_power_draw += parts[i].power_draw;
        c->max_power_supply += parts[i].power_supply;
        
        if (parts[i].type == COMP_CPU) c->sys_clock_hz = parts[i].clock_hz;
        if (parts[i].type == COMP_RAM) c->sys_memory_bytes += parts[i].memory_bytes;
    }
    c->is_assembled = true;
}

void computer_power_toggle(Computer* c) {
    if (!c->is_assembled) return;
    
    c->is_powered_on = !c->is_powered_on;
    c->is_crashed = false;
    
    if(c->is_powered_on && c->total_power_draw > c->max_power_supply) {
        c->is_crashed = true;
    }
}
