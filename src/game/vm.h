#ifndef GAME_VM_H
#define GAME_VM_H
#include "common.h"
#include "game/hardware.h"

typedef struct {
    u8 R[4];       
    u16 PC;        
    
    u64 total_cycles;
    f32 current_load;
} VmCore;

#define TERM_COLS 50
#define TERM_ROWS 20

typedef struct {
    char chars[TERM_ROWS][TERM_COLS];
    u8   cursor_x;
    u8   cursor_y;
} VirtualMonitor;

typedef enum {
    VM_OFF,
    VM_VGA_POST,   /* Brief Video card init */
    VM_MAIN_POST,  /* Memory testing and IDE drive detection */
    VM_BIOS_MENU,  /* Intercepted BIOS Setup Menu */
    VM_OS          /* Interactive terminal */
} VmStateMode;

typedef struct {
    Computer* hardware;
    VmCore cpu;
    VirtualMonitor monitor;
    
    VmStateMode mode;
    double timer;
    double sub_timer;
    u32 mem_check_counter;
    int post_phase;
    
    char input_buffer[64];
    u8   input_len;
} VmState;

void vm_init(VmState* vm, Computer* hw);
void vm_tick(VmState* vm, double dt_sec);
void vm_print(VmState* vm, const char* text);
void vm_clear(VmState* vm);
void vm_handle_input(VmState* vm, const char* text, bool enter_pressed);
void vm_intercept_f3(VmState* vm);
void vm_exit_bios(VmState* vm);

#endif
