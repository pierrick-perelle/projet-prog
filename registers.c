/*
Armator - simulateur de jeu d'instruction ARMv5T � but p�dagogique
Copyright (C) 2011 Guillaume Huard
Ce programme est libre, vous pouvez le redistribuer et/ou le modifier selon les
termes de la Licence Publique G�n�rale GNU publi�e par la Free Software
Foundation (version 2 ou bien toute autre version ult�rieure choisie par vous).

Ce programme est distribu� car potentiellement utile, mais SANS AUCUNE
GARANTIE, ni explicite ni implicite, y compris les garanties de
commercialisation ou d'adaptation dans un but sp�cifique. Reportez-vous � la
Licence Publique G�n�rale GNU pour plus de d�tails.

Vous devez avoir re�u une copie de la Licence Publique G�n�rale GNU en m�me
temps que ce programme ; si ce n'est pas le cas, �crivez � la Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
�tats-Unis.

Contact: Guillaume.Huard@imag.fr
	 B�timent IMAG
	 700 avenue centrale, domaine universitaire
	 38401 Saint Martin d'H�res
*/
#include "registers.h"
#include "arm_constants.h"
#include <stdlib.h>

//define utilisateur 0 -> spsr définit dans arm_constant.h
 
//define for 37 reg
enum registre_interne {
INTERNE_R0,
INTERNE_R1,
INTERNE_R2,
INTERNE_R3,
INTERNE_R4,
INTERNE_R5,
INTERNE_R6,
INTERNE_R7,
INTERNE_R8,INTERNE_R8_FIQ,
INTERNE_R9,INTERNE_R9_FIQ,
INTERNE_R10,INTERNE_R10_FIQ,
INTERNE_R11,INTERNE_R11_FIQ,
INTERNE_R12,INTERNE_R12_FIQ,
INTERNE_R13,INTERNE_R13_SVC,INTERNE_R13_ABT,INTERNE_R13_UND,INTERNE_R13_IRQ,INTERNE_R13_FIQ,
INTERNE_R14,INTERNE_R14_SVC,INTERNE_R14_ABT,INTERNE_R14_UND,INTERNE_R14_IRQ,INTERNE_R14_FIQ,
INTERNE_PC,
INTERNE_CPSR,
INTERNE_SPSR,INTERNE_SPSR_SVC,INTERNE_SPSR_ABT,INTERNE_SPSR_UND,INTERNE_SPSR_IRQ,INTERNE_SPSR_FIQ};

struct registers_data {
    uint32_t reg[37];
    uint8_t mode;
};

registers registers_create() {
    registers r = malloc(sizeof(struct registers_data));
    return r;
}

void registers_destroy(registers r) {
    free(r);
}

uint8_t get_mode(registers r) {
    return r->mode;
} 

//Voir page 41 et 43 du manuel.

int current_mode_has_spsr(registers r) {
    if(get_mode(r) != 0b10000 && get_mode(r) != 0b11111){ 
        return 0;
    }
    return -1;
}

//Voir page 41 du manuel.

int in_a_privileged_mode(registers r) {
    if(get_mode(r) != 0b10000){
        return 0;
    }
    return -1;
}

uint32_t read_register(registers r, uint8_t reg) {

    if(reg == PC || reg == CPSR){
        return read_usr_register(r,reg);
    }
    else if(reg <= R7){
        return read_usr_register(r,reg);
    }
    else if (reg <= R12 && get_mode(r) != IRQ){
        return read_usr_register(r,reg);
    }
    else{
        switch (get_mode(r)){
        case SVC:
            return read_svc_register(r,reg);
        case ABT:
            return read_abt_register(r,reg);
        case UND:
            return read_und_register(r,reg);
        case IRQ:
            return read_irq_register(r,reg);
        case FIQ:
            return read_fiq_register(r,reg);
        default:
            return read_usr_register(r,reg);
        }
    }

}

//valable pour le mode USR et SYS (User et Système).
uint32_t read_usr_register(registers r, uint8_t reg) {
    switch (reg){
        case R0:
            return r->reg[INTERNE_R0];
        case R1:
            return r->reg[INTERNE_R1];
        case R2:
            return r->reg[INTERNE_R2];
        case R3:
            return r->reg[INTERNE_R3];
        case R4:
            return r->reg[INTERNE_R4];
        case R5:
            return r->reg[INTERNE_R5];
        case R6:
            return r->reg[INTERNE_R6];
        case R7:
            return r->reg[INTERNE_R7];
        case R8:
            return r->reg[INTERNE_R8];
        case R9:
            return r->reg[INTERNE_R9];
        case R10:
            return r->reg[INTERNE_R10];
        case R11:
            return r->reg[INTERNE_R11];
        case R12:
            return r->reg[INTERNE_R12];
        case R13:
            return r->reg[INTERNE_R13];
        case R14:
            return r->reg[INTERNE_R14];
        case PC:
            return r->reg[INTERNE_PC];
        case CPSR:
            return r->reg[INTERNE_CPSR];
        case SPSR:
            return EXIT_FAILURE;
        default:
            return EXIT_FAILURE;
    }
}

uint32_t read_svc_register(registers r, uint8_t reg) {
    switch (reg){
        case R13:
            return r->reg[INTERNE_R13_SVC];
        case R14:
            return r->reg[INTERNE_R14_SVC];
        case SPSR:
            return r->reg[INTERNE_SPSR_SVC];
        default:
            return EXIT_FAILURE;
    }
}

uint32_t read_abt_register(registers r, uint8_t reg) {
    switch (reg){
        case R13:
            return r->reg[INTERNE_R13_ABT];
        case R14:
            return r->reg[INTERNE_R14_ABT];
        case SPSR:
            return r->reg[INTERNE_SPSR_ABT];
        default:
            return EXIT_FAILURE;
    }
}

uint32_t read_und_register(registers r, uint8_t reg) {
    switch (reg){
        case R13:
            return r->reg[INTERNE_R13_UND];
        case R14:
            return r->reg[INTERNE_R14_UND];
        case SPSR:
            return r->reg[INTERNE_SPSR_UND];
        default:
            return EXIT_FAILURE;
    }
}


uint32_t read_irq_register(registers r, uint8_t reg) {
    switch (reg){
        case R13:
            return r->reg[INTERNE_R13_IRQ];
        case R14:
            return r->reg[INTERNE_R14_IRQ];
        case SPSR:
            return r->reg[INTERNE_SPSR_IRQ];
        default:
            return EXIT_FAILURE;
    }
}

uint32_t read_fiq_register(registers r, uint8_t reg) {
    switch (reg){
        case R8:
            return r->reg[INTERNE_R8_FIQ];
        case R9:
            return r->reg[INTERNE_R9_FIQ];
        case R10:
            return r->reg[INTERNE_R10_FIQ];
        case R11:
            return r->reg[INTERNE_R11_FIQ];
        case R12:
            return r->reg[INTERNE_R12_FIQ];
        case R13:
            return r->reg[INTERNE_R13_FIQ];
        case R14:
            return r->reg[INTERNE_R14_FIQ];
        case SPSR:
            return r->reg[INTERNE_SPSR_FIQ];
        default:
            return EXIT_FAILURE;
    }
}

uint32_t read_cpsr(registers r) {
    return read_register(r,CPSR);
}

uint32_t read_spsr(registers r) {
    return read_register(r,SPSR);
}

void write_register(registers r, uint8_t reg, uint32_t value) {

    if(reg == PC || reg == CPSR){
        write_usr_register(r,reg,value);
    }
    else if(reg <= R7){
        write_usr_register(r,reg,value);
    }
    else if (reg <= R12 && get_mode(r) != IRQ){
        write_usr_register(r,reg,value);
    }
    else{
        switch (get_mode(r)){
        case SVC:
            write_svc_register(r,reg,value);
        case ABT:
            write_abt_register(r,reg,value);
        case UND:
            write_und_register(r,reg,value);
        case IRQ:
            write_irq_register(r,reg,value);
        case FIQ:
            write_fiq_register(r,reg,value);
        default:
            write_usr_register(r,reg,value);
        }
    }
}

void write_usr_register(registers r, uint8_t reg, uint32_t value) {
    switch (reg){
        case R0:
            r->reg[INTERNE_R0] = value;
        case R1:
            r->reg[INTERNE_R1] = value;
        case R2:
            r->reg[INTERNE_R2] = value;
        case R3:
            r->reg[INTERNE_R3] = value;
        case R4:
            r->reg[INTERNE_R4] = value;
        case R5:
            r->reg[INTERNE_R5] = value;
        case R6:
            r->reg[INTERNE_R6] = value;
        case R7:
            r->reg[INTERNE_R7] = value;
        case R8:
            r->reg[INTERNE_R8] = value;
        case R9:
            r->reg[INTERNE_R9] = value;
        case R10:
            r->reg[INTERNE_R10] = value;
        case R11:
            r->reg[INTERNE_R11] = value;
        case R12:
            r->reg[INTERNE_R12] = value;
        case R13:
            r->reg[INTERNE_R13] = value;
        case R14:
            r->reg[INTERNE_R14] = value;
        case PC:
            r->reg[INTERNE_PC] = value;
        case CPSR:
            r->reg[INTERNE_CPSR] = value;
        case SPSR:
            EXIT_FAILURE;
        default:
            EXIT_FAILURE;
    }
}

void read_svc_register(registers r, uint8_t reg,uint32_t value) {
    switch (reg){
        case R13:
            r->reg[INTERNE_R13_SVC] = value;
        case R14:
            r->reg[INTERNE_R14_SVC] = value;
        case SPSR:
            r->reg[INTERNE_SPSR_SVC] = value;
        default:
            return EXIT_FAILURE;
    }
}

void read_abt_register(registers r, uint8_t reg,uint32_t value) {
    switch (reg){
        case R13:
            r->reg[INTERNE_R13_ABT] = value;
        case R14:
            r->reg[INTERNE_R14_ABT] = value;
        case SPSR:
            r->reg[INTERNE_SPSR_ABT] = value;
        default:
            return EXIT_FAILURE;
    }
}

void read_und_register(registers r, uint8_t reg,uint32_t value) {
    switch (reg){
        case R13:
            r->reg[INTERNE_R13_UND] = value;
        case R14:
            r->reg[INTERNE_R14_UND] = value;
        case SPSR:
            r->reg[INTERNE_SPSR_UND] = value;
        default:
            return EXIT_FAILURE;
    }
}


void read_irq_register(registers r, uint8_t reg,uint32_t value) {
    switch (reg){
        case R13:
            r->reg[INTERNE_R13_IRQ] = value;
        case R14:
            r->reg[INTERNE_R14_IRQ] = value;
        case SPSR:
            r->reg[INTERNE_SPSR_IRQ] = value;
        default:
            return EXIT_FAILURE;
    }
}

void read_fiq_register(registers r, uint8_t reg,uint32_t value) {
    switch (reg){
        case R8:
            r->reg[INTERNE_R8_FIQ] = value;
        case R9:
            r->reg[INTERNE_R9_FIQ] = value;
        case R10:
            r->reg[INTERNE_R10_FIQ] = value;
        case R11:
            r->reg[INTERNE_R11_FIQ] = value;
        case R12:
            r->reg[INTERNE_R12_FIQ] = value;
        case R13:
            r->reg[INTERNE_R13_FIQ] = value;
        case R14:
            r->reg[INTERNE_R14_FIQ] = value;
        case SPSR:
            r->reg[INTERNE_SPSR_FIQ] = value;
        default:
            return EXIT_FAILURE;
    }
}

void write_cpsr(registers r, uint32_t value) {
    write_register(r,CPSR,value);
}

void write_spsr(registers r, uint32_t value) {
    write_register(r,SPSR,value);
}


