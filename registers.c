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
// interne_R0 = 0, interne_R1 = 1,...
enum registre_interne {INTERNE_R0, interne_R1, interne_R2, interne_R3,...}

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
    return 0;
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
    //penser au remplacement de registre page 43 fonction du mode.
    //with reg as n register
    //switch get_mode(r)
    //  super read_super_register(r,reg)
    // default read_usr_register() valable pour system et usr
    uint32_t value = r->reg[reg];
    return value;
}

uint32_t read_usr_register(registers r, uint8_t reg) {
    switch (reg){
        case R0:
            return r->reg[interne_R0];
    }
}
uint32_t read_fast_interrupt_register(registers r, uint8_t reg) {
    switch (reg){
        case R8:
            return r->reg[interne_R8_fiq];
    }
}
/* read_supervisor_register */
// 

uint32_t read_cpsr(registers r) {
    uint32_t value = r->reg[16];
    return value;
}

uint32_t read_spsr(registers r) {
    //read_register(r,SPSR)  CPSR defini dans constant
    uint32_t value = r->reg[17];
    return value;
}

void write_register(registers r, uint8_t reg, uint32_t value) {
    //switch get_mode(r)
    //  super write_super_register(r,reg)
    // default write_usr_register() valable pour system et usr
    r->reg[reg] = value;
}

void write_usr_register(registers r, uint8_t reg, uint32_t value) {
    switch (reg){
        case R0:
            r->reg[interne_R0] = value;
    }
}

void write_cpsr(registers r, uint32_t value) {
    r->reg[16] = value;
}

void write_spsr(registers r, uint32_t value) {
    r->reg[17] = value;
}
