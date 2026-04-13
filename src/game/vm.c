#include "game/vm.h"
#include <stdio.h>

static void monitor_clear(VirtualMonitor* m) {
    memset(m->chars, ' ', sizeof(m->chars));
    for (int y = 0; y < TERM_ROWS; ++y) {
        m->chars[y][TERM_COLS-1] = '\0';
    }
    m->cursor_x = 0;
    m->cursor_y = 0;
}

static void monitor_scroll(VirtualMonitor* m) {
    for (int y = 1; y < TERM_ROWS; ++y) {
        memcpy(m->chars[y-1], m->chars[y], TERM_COLS);
    }
    memset(m->chars[TERM_ROWS-1], ' ', TERM_COLS - 1);
    m->chars[TERM_ROWS-1][TERM_COLS-1] = '\0';
    m->cursor_y = TERM_ROWS - 1;
}

static void monitor_putchar(VirtualMonitor* m, char c) {
    if (c == '\n') {
        m->cursor_x = 0;
        m->cursor_y++;
        if (m->cursor_y >= TERM_ROWS) monitor_scroll(m);
        return;
    }
    if (c == '\r') {
        m->cursor_x = 0;
        return;
    }
    
    if (m->cursor_x >= TERM_COLS - 1) {
        m->cursor_x = 0;
        m->cursor_y++;
        if (m->cursor_y >= TERM_ROWS) monitor_scroll(m);
    }
    
    m->chars[m->cursor_y][m->cursor_x] = c;
    m->cursor_x++;
}

void vm_print(VmState* vm, const char* text) {
    for (const char* p = text; *p; ++p) {
        monitor_putchar(&vm->monitor, *p);
    }
}

void vm_clear(VmState* vm) { monitor_clear(&vm->monitor); }

/* Hyper Realistic Boot Init */
void vm_init(VmState* vm, Computer* hw) {
    memset(vm, 0, sizeof(*vm));
    vm->hardware = hw;
    monitor_clear(&vm->monitor);
    
    if(!hw->is_powered_on || hw->is_crashed) {
        vm->mode = VM_OFF;
        return;
    }
    
    /* Start heavily realistic Phase 1: VGA BIOS */
    vm->mode = VM_VGA_POST;
    vm->timer = 0.0;
    vm->post_phase = 0;
    
    vm_print(vm, "VGA BIOS V1.03\n");
    vm_print(vm, "Copyright (C) ");
    vm_print(vm, hw->parts[COMP_MOTHERBOARD].name);
    vm_print(vm, "\n");
    vm_print(vm, "256KB Video RAM\n\n");
}

static void display_energy_star(VmState* vm) {
    vm_print(vm, "        EPA POLLUTION PREVENTER\n");
    vm_print(vm, "      +-------------------------+\n");
    vm_print(vm, "      | * * *   energy          |\n");
    vm_print(vm, "      | * * * *                 |\n");
    vm_print(vm, "      +-------------------------+\n\n");
}

static void boot_os(VmState* vm) {
    vm->mode = VM_OS;
    vm_clear(vm);
    vm_print(vm, "Starting MS-DOS...\n\n");
    vm_print(vm, "HIMEM is testing extended memory...done.\n");
    vm_print(vm, "C:\\> ");
}

void vm_intercept_f3(VmState* vm) {
    if (vm->mode == VM_MAIN_POST && vm->timer >= 0.5) {
        vm->mode = VM_BIOS_MENU;
        vm_clear(vm);
    }
}

void vm_exit_bios(VmState* vm) {
    if (vm->mode == VM_BIOS_MENU) {
        boot_os(vm);
    }
}

void vm_tick(VmState* vm, double dt_sec) {
    if (!vm->hardware->is_powered_on || vm->hardware->is_crashed) {
        vm->cpu.current_load = 0.0f;
        vm->mode = VM_OFF;
        return;
    }
    
    vm->timer += dt_sec;

    /* Phase 1: VGA Block */
    if (vm->mode == VM_VGA_POST) {
        if (vm->timer > 1.5) {
            vm_clear(vm);
            display_energy_star(vm);
            
            vm_print(vm, "Award Modular BIOS v4.51PG, An Energy Star Ally\n");
            vm_print(vm, "Copyright (C) 1984-95, Award Software, Inc.\n\n");
            
            char buf[128];
            snprintf(buf, sizeof(buf), "%s BIOS Release V1.4\n\n", vm->hardware->parts[COMP_MOTHERBOARD].name);
            vm_print(vm, buf);
            
            vm_print(vm, "Main Processor  : ");
            vm_print(vm, vm->hardware->parts[COMP_CPU].name);
            snprintf(buf, sizeof(buf), " at %u Hz\n", vm->hardware->sys_clock_hz);
            vm_print(vm, buf);
            
            vm_print(vm, "Memory Testing  : ");
            vm->mode = VM_MAIN_POST;
            vm->timer = 0;
            vm->mem_check_counter = 0;
            vm->post_phase = 0;
        }
        vm->cpu.current_load = 0.8f;
        vm->cpu.total_cycles += (u64)(vm->hardware->sys_clock_hz * dt_sec);
        return;
    }
    
    /* Phase 2: Core POST (RAM and IDE) */
    if (vm->mode == VM_MAIN_POST) {
        if (vm->post_phase == 0) {
            /* RAM Counting */
            if (vm->mem_check_counter < vm->hardware->sys_memory_bytes) {
                u32 step = (u32)(vm->hardware->sys_memory_bytes / 2.0 * dt_sec); /* Scale speed to 2 seconds total */
                if (step < 512) step = 512;
                vm->mem_check_counter += step;
                if (vm->mem_check_counter >= vm->hardware->sys_memory_bytes) {
                    vm->mem_check_counter = vm->hardware->sys_memory_bytes;
                }
                
                char buf[64];
                snprintf(buf, sizeof(buf), "\rMemory Testing  : %u B OK", vm->mem_check_counter);
                
                vm->monitor.cursor_x = 0;
                for(const char* c = buf; *c; c++) monitor_putchar(&vm->monitor, *c);
                
                if (vm->mem_check_counter == vm->hardware->sys_memory_bytes) {
                    vm_print(vm, "\n\n");
                    vm->post_phase = 1;
                    vm->sub_timer = 0;
                }
            }
        } 
        else if (vm->post_phase == 1) {
            /* IDE Detection Simulation */
            vm->sub_timer += dt_sec;
            if (vm->sub_timer > 0.1 && vm->sub_timer < 0.2) {
                vm_print(vm, "Detecting IDE Primary Master ... ");
                vm->sub_timer = 0.5; /* jump to next stage */
            }
            if (vm->sub_timer > 1.2 && vm->sub_timer < 1.4) {
                vm_print(vm, vm->hardware->parts[COMP_STORAGE].name);
                vm_print(vm, "\nDetecting IDE Primary Slave  ... None\n");
                vm->sub_timer = 2.0;
            }
            if (vm->sub_timer > 2.5) {
                vm_print(vm, "\nPress F3 to ENTER SETUP\n");
                vm->post_phase = 2;
                vm->timer = 0; /* Window opens */
            }
        }
        else if (vm->post_phase == 2) {
            if (vm->timer > 2.0) { /* 2 second interrupt window */
                boot_os(vm);
            }
        }
        
        vm->cpu.current_load = 1.0f;
        vm->cpu.total_cycles += (u64)(vm->hardware->sys_clock_hz * dt_sec);
        return;
    }
    
    if (vm->mode == VM_BIOS_MENU) {
        vm->cpu.current_load = 0.05f;
        vm->cpu.total_cycles += (u64)(vm->hardware->sys_clock_hz * 0.05f * dt_sec);
        return;
    }

    /* Idle cycles for OS / General */
    u64 cycle_tick = (u64)(vm->hardware->sys_clock_hz * dt_sec);
    vm->cpu.total_cycles += (u64)(cycle_tick * 0.05f);
    
    if(vm->cpu.current_load > 0.05f) {
        vm->cpu.current_load -= (f32)(dt_sec * 0.5f);
    } else {
        vm->cpu.current_load = 0.05f;
    }
}

static void process_os_command(VmState* vm) {
    vm_print(vm, "\n");
    if (strcmp(vm->input_buffer, "sysinfo") == 0) {
        vm_print(vm, "CPU: ");
        vm_print(vm, vm->hardware->parts[COMP_CPU].name);
        char buf[64];
        snprintf(buf, sizeof(buf), "\nCLK: %u Hz\n", vm->hardware->sys_clock_hz);
        vm_print(vm, buf);
    } 
    else if (strcmp(vm->input_buffer, "benchmark") == 0) {
        vm_print(vm, "RUNNING BENCHMARK...\nCYCLE STRESS TEST OK.\n");
        vm->cpu.current_load = 1.0f;
        vm->cpu.total_cycles += vm->hardware->sys_clock_hz * 4; 
    }
    else if (strcmp(vm->input_buffer, "clear") == 0) {
        vm_clear(vm);
    }
    else if (vm->input_len > 0) {
        vm_print(vm, "Bad command or file name\n");
    }
    
    vm->input_len = 0;
    vm->input_buffer[0] = '\0';
    vm_print(vm, "C:\\> ");
}

void vm_handle_input(VmState* vm, const char* text, bool enter_pressed) {
    if (!vm->hardware->is_powered_on || vm->hardware->is_crashed) return;

    if (vm->mode != VM_OS) return;

    if (enter_pressed) {
        process_os_command(vm);
        return;
    }

    if (text && text[0] != '\0') {
        if (text[0] == '\b' && vm->input_len > 0) {
            /* Basic backspace */
            vm->input_len--;
            vm->input_buffer[vm->input_len] = '\0';
            vm_print(vm, "\b \b");
        } else if (text[0] != '\b') {
            int add_len = strlen(text);
            if (vm->input_len + add_len < sizeof(vm->input_buffer) - 1) {
                strcat(vm->input_buffer, text);
                vm->input_len += add_len;
                vm_print(vm, text);
            }
        }
    }
}
