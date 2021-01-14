/*
Armator - simulateur de jeu d'instruction ARMv5T à but pédagogique
Copyright (C) 2011 Guillaume Huard
Ce programme est libre, vous pouvez le redistribuer et/ou le modifier selon les
termes de la Licence Publique Générale GNU publiée par la Free Software
Foundation (version 2 ou bien toute autre version ultérieure choisie par vous).

Ce programme est distribué car potentiellement utile, mais SANS AUCUNE
GARANTIE, ni explicite ni implicite, y compris les garanties de
commercialisation ou d'adaptation dans un but spécifique. Reportez-vous à la
Licence Publique Générale GNU pour plus de détails.

Vous devez avoir reçu une copie de la Licence Publique Générale GNU en même
temps que ce programme ; si ce n'est pas le cas, écrivez à la Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
États-Unis.

Contact: Guillaume.Huard@imag.fr
	 Bâtiment IMAG
	 700 avenue centrale, domaine universitaire
	 38401 Saint Martin d'Hères
*/
#include "arm_load_store.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "util.h"
#include "debug.h"

uint32_t arithmetic_Shift_Right(uint32_t valeur, uint8_t shift_imm){
    uint32_t index = valeur;
    if (shift_imm == 0){ // ASR #32
        if (get_bit(valeur,31) == 1){
            index = 0xFFFFFFFF;
        }
        else{
            index = 0;
        }
    }
    else{
        index = asr(valeur, shift_imm);
    }
    return index;
}

int arm_load_store(arm_core p, uint32_t ins) {
    uint8_t rd = get_bits(ins,15,12);
    uint8_t rn = get_bits(ins,19,16);
    uint8_t w_bit = get_bit(ins,21);
    uint8_t u_bit = get_bit(ins,23);
    uint8_t p_bit = get_bit(ins,24);
    uint8_t l_bit = get_bit(ins,20); // This bit distinguishes between a Load (L==1) and a Store (L==0) instruction.
    uint32_t address;
    uint32_t value;
    uint16_t valueH;
    uint8_t valueB;
    
    if (get_bit(ins,26) == 1){ // LDR, LDRB, STR	& STRB
        uint16_t offset_12 = get_bits(ins,11,0);
        uint8_t rm = get_bits(ins,3,0);
        uint8_t i_bit = get_bit(ins,25);
        uint8_t b_bit = get_bit(ins,22); // This bit distinguishes between an unsigned byte (B==1) and a word (B==0) access.
        uint8_t shift = get_bits(ins,6,5);
        uint8_t shift_imm = get_bits(ins,11,7);
        
        // Calcul de l'adresse - Addressing Mode 2 - A5.2 (p 458)
        uint32_t index;
        
        if (((p_bit == 1 && w_bit == 1) || (p_bit == 0 && w_bit == 0)) && rn == 15){
            return DATA_ABORT; // UNPREDICTABLE
        }
        
        if (i_bit == 0){  // Immediate offset - A5.2.2 (p 460)
            if (u_bit == 1){
                address = arm_read_register(p, rn) + offset_12;
            }
            else{ // u_bit == 0
              address = arm_read_register(p, rn) - offset_12;
            }
        }
        else{ // i_bit == 1  Register offset or Scaled register offset
            if (get_bit(ins,4) == 0){ 
                if (((p_bit == 1 && w_bit == 1) || (p_bit == 0 && w_bit == 0)) && rn == rm){
                    return DATA_ABORT; // UNPREDICTABLE
                }
                if (rm == 15){ 
                    return DATA_ABORT; // UNPREDICTABLE
                }
                if (shift_imm == 0 && shift == 0){ // Register offset - A5.2.3 (p 461)
                    if (u_bit == 1){
                        address = arm_read_register(p, rn) + arm_read_register(p, rm);
                    }
                    else{ // u_bit == 0
                        address = arm_read_register(p, rn) - arm_read_register(p, rm);
                    }
                }
                else{ //  Scaled register offset - A5.2.4 (p 462)
                    switch (shift){
                        case 0: // LSL
                            index = arm_read_register(p, rm) << shift_imm;
                            break;
                        case 1: // LSR
                            if (shift_imm == 0){ // LSR #32
                                index = 0;
                            }
                            else{
                                index = arm_read_register(p, rm) >> shift_imm;
                            }
                            break;
                        case 2: // ASR
                            index = arithmetic_Shift_Right(arm_read_register(p, rm), shift_imm);
                            break;
                        case 3: // ROR or RRX
                            if (shift_imm == 0){ // RRX
                                index = (arm_read_cpsr(p) >> 28 & 1) << 31 | arm_read_register(p, rm) >> 1;
                            }
                            else{ // ROR
                                index = ror(arm_read_register(p, rm), shift_imm);
                            }
                            break;
                        default: // On est pas censé arriver là...
                            return UNDEFINED_INSTRUCTION;
                    }
                    if (u_bit == 1){
                        address = arm_read_register(p, rn) + index;
                    }
                    else{ // u_bit == 0
                        address = arm_read_register(p, rn) - index;
                    }
                }
            }
            else{ // get_bit(ins,4) == 1 - On est pas censé arriver là...
                return UNDEFINED_INSTRUCTION;
            }
        }
        if (p_bit == 1 && w_bit == 1){ // Pre-indexed - A5.2.5 (p 464), A5.2.6 (p 465), A5.2.7 (p 466)
            arm_write_register(p, rn, address);
        }
        if (p_bit == 0 && w_bit == 0){ // Post-indexed - A5.2.5 (p 464), A5.2.6 (p 465), A5.2.7 (p 466)
            uint32_t tempor;
            tempor = arm_read_register(p, rn);
            arm_write_register(p, rn, address);
            address = tempor;
        }
        if (p_bit == 0 && w_bit == 1){
            return DATA_ABORT; // UNPREDICTABLE
        }
        
        // Execution de l'instruction
        
        if (l_bit == 1 && b_bit == 0){ // LDR - A4.1.23 (p 193)
        
            if (address%4 != 0){ // CP15_reg1_Ubit == 0
                arm_read_word(p, address, &value);
                value = ror(value, (8 * get_bits(address,1,0)));
            }
            else{ // CP15_reg_Ubit == 1
                arm_read_word(p, address, &value);
            }
            if (rd == 15){
                arm_write_register(p, rd, (value & 0xFFFFFFFE));
                //T Bit = data[0]
            }
            else{
                arm_write_register(p, rd, value);
            }
        
        }
        else if (l_bit == 1 && b_bit == 1){ // LDRB - A4.1.24 (p 196)
            arm_read_byte(p, address, &valueB);
            arm_write_register(p, rd, valueB);
        }
        else if (l_bit == 0 && b_bit == 0){ // STR - A4.1.99 (p 343)

            value = arm_read_register(p, rd);
            arm_write_word(p, address, value);
        }
        else{ // l_bit == 0 && b_bit == 1 - STRB - A4.1.100 (p 345)
            arm_write_byte(p, address, get_bits(arm_read_register(p, rd),7,0));
        }
        return NO_ERROR;
    }
    
    else{ // get_bits(ins,26) == 0 - LDRH & SDRH
        uint8_t i_bit = get_bit(ins,22);
        uint8_t immedH = get_bits(ins,11,8);
        uint8_t immedL = get_bits(ins,3,0); // Correspond aussi à rm dans le register offset
        
        // Calcul de l'adresse - Addressing Mode 3 - A5.3 (p 473)
        if (((p_bit == 1 && w_bit == 1) || (p_bit == 0 && w_bit == 0)) && rn == 15){
            return DATA_ABORT; // UNPREDICTABLE
        }
        if (i_bit ==1){ // Immediate offset - A5.3.2 (p 475)
            uint8_t offset_8 = (immedH << 4) | immedL;
            if (u_bit == 1){
                address = arm_read_register(p, rn) + offset_8;
            }
            else{ // U == 0
                address = arm_read_register(p, rn) - offset_8;
            }
        }
        else{ // Register offset - A5.3.3 (p 476)
            if (((p_bit == 1 && w_bit == 1) || (p_bit == 0 && w_bit == 0)) && rn == immedL){
                return DATA_ABORT; // UNPREDICTABLE
            }
            if (immedL == 15){ 
                return DATA_ABORT; // UNPREDICTABLE
            }
            if (u_bit == 1){
                address = arm_read_register(p, rn) + arm_read_register(p, immedL);
            }
            else{ // U == 0
                address = arm_read_register(p, rn) - arm_read_register(p, immedL);
            }
        }
        if (p_bit == 1 && w_bit == 1){ // Pre-indexed - A5.3.4 (p 477), A5.3.5 (p 478)
            arm_write_register(p, rn, address);
        }
        if (p_bit == 0 && w_bit == 0){ // Post-indexed - A5.3.6 (p 479), A5.3.7 (p 480)
            uint32_t temporH;
            temporH = arm_read_register(p, rn);
            arm_write_register(p, rn, address);
            address = temporH;
        }
        if (p_bit == 0 && w_bit == 1){
            return DATA_ABORT; // UNPREDICTABLE
        }
        
        // Execution de l'instruction
         if (l_bit == 1){ // LDRH - A4.1.28 (p 204)
             if (address%2 != 0){ // CP15_reg1_Ubit == 0
                 if (get_bit(address,0) == 0){
                     arm_read_half(p, address, &valueH);
                 }
                 else{
                     return DATA_ABORT; // UNPREDICTABLE
                 }
             }
             else{ // CP15_reg1_Ubit == 1
                 arm_read_half(p, address, &valueH);
             }
             arm_write_register(p, rd, valueH);
             //Rd = ZeroExtend(data[15:0])
         }
         else{ // l_bit == 0 - STRH - A4.1.104 (p 354)
            if (address%2 != 0){ // CP15_reg1_Ubit == 0
                if (get_bit(address,0) == 0){
                    arm_write_byte(p, address, get_bits(arm_read_register(p, rd),15,0));
                }
                else{
                    return DATA_ABORT; // UNPREDICTABLE
                }
            
            }
            else{ // CP15_reg1_Ubit ==1 
                arm_write_byte(p, address, get_bits(arm_read_register(p, rd),15,0));
            }
         }
       return NO_ERROR; 
    }
}

int arm_load_store_multiple(arm_core p, uint32_t ins) {
    uint16_t register_list = get_bits(ins,15,0);
    uint8_t rn = get_bits(ins,19,16);
    uint8_t l_bit = get_bit(ins,20); // This bit distinguishes between a Load (L==1) and a Store (L==0) instruction.
    uint8_t w_bit = get_bit(ins,21);
    // uint8_t s_bit = get_bit(ins,22);
    uint8_t u_bit = get_bit(ins,23);
    uint8_t p_bit = get_bit(ins,24);
    int i;
    uint32_t value;
    uint32_t address, start_address, end_address;
    
    uint8_t nbSetBitsIn = 0;
    for (i = 0; i < 16; i++){
        if (get_bit(register_list,i)){
            nbSetBitsIn++;
        }
    }
    
    if (nbSetBitsIn == 0){
        return DATA_ABORT; // UNPREDICTABLE
    }
    
    //  Addressing Mode 4 - Load and Store Multiple - A5.4 (p 481)
    
    if (p_bit == 0 && u_bit == 1){ //  Increment after - A5.4.2 (p 483)
        start_address = arm_read_register(p, rn);
        end_address = arm_read_register(p, rn);
        end_address = arm_read_register(p, rn) + (nbSetBitsIn * 4) - 4;
        if (w_bit == 1){
            arm_write_register(p, rn, arm_read_register(p, rn) + (nbSetBitsIn * 4));
        }
    }
    else if (p_bit == 1 && u_bit == 1){ //  Increment before - A5.4.3 (p 484)
        start_address = arm_read_register(p, rn) + 4;
        end_address = arm_read_register(p, rn) + (nbSetBitsIn * 4);
        if (w_bit == 1){
            arm_write_register(p, rn, arm_read_register(p, rn) + (nbSetBitsIn * 4));
        }
    }
    else if (p_bit == 0 && u_bit == 0){ //  Decrement after - A5.4.4 (p 485)
        start_address = arm_read_register(p, rn) - (nbSetBitsIn * 4) + 4;
        end_address = arm_read_register(p, rn);
        if (w_bit == 1){
            arm_write_register(p, rn, arm_read_register(p, rn) - (nbSetBitsIn * 4));
        }
    }
    else{ // p_bit == 1 && u_bit == 1){ //  Decrement before - A5.4.5 (p 486)
        start_address = arm_read_register(p, rn) - (nbSetBitsIn * 4);
        end_address = arm_read_register(p, rn) - 4;
        if (w_bit == 1){
            arm_write_register(p, rn, arm_read_register(p, rn) - (nbSetBitsIn * 4));
        }
    }

    // Execution instruction
    
    if (l_bit == 1){ // LDM (1) A4.1.20 (p 186)
        address = start_address;
        for (i = 0; i <= 14; i++){
            if (get_bit(register_list,i) == 1){
                arm_read_word(p, address, &value);
                arm_write_register(p, i, value);
                address = address + 4;
            }
        }
        if (get_bit(register_list,15) == 1){
            arm_read_word(p, address, &value);
            arm_write_register(p, 15, value & 0xFFFFFFFE);
            //T Bit = value[0]
            address = address + 4;
        }
        if (end_address == address - 4){
            return NO_ERROR;
        }
        else{
            return DATA_ABORT; // Il y a eu une erreur
        }
    }
    else{ // l_bit == 0 - STM (1) A4.1.97 (p 339)
        address = start_address;
        for (i = 0; i <= 15; i++){
            if (get_bit(register_list,i) == 1){
                arm_write_word(p, address, arm_read_register(p, rn));
                address = address + 4;
            }
        }
        if (end_address == address - 4){
            return NO_ERROR;
        }
        else{
            return DATA_ABORT; // Il y a eu une erreur
        }
    }
}

int arm_coprocessor_load_store(arm_core p, uint32_t ins) {
    /* Not implemented */
    return UNDEFINED_INSTRUCTION;
}
