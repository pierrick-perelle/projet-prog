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
#include "arm_instruction.h"
#include "arm_exception.h"
#include "arm_data_processing.h"
#include "arm_load_store.h"
#include "arm_branch_other.h"
#include "arm_constants.h"
#include "util.h"

//pointeur de function
//typedef int (*ptr_func)(arm_core p, uint32_t ins);


//enum de chacun des opcode (cond) voir fig page 112
enum cond_t {EQ,NE,CS_HS,CC_LO,MI,PL,VS,VC,HI,LS,GE,LT,GT,LE,AL,UNCOND};

static int arm_execute_instruction(arm_core p) {
    uint32_t ins;
    enum cond_t cond;

    if(arm_fetch(p,&ins) == -1){
        fprintf(stderr,"Erreur de fetch\n");
        return PREFETCH_ABORT;
        //voir page 58 pour les exceptions.
    }

    //ins contient maintenant l'instruction courante. voir arm_fetch()
    //on check si la condition est bonne.
    cond = get_cond(ins);
    if(!check_cond(p,cond)){
        return UNDEFINED_INSTRUCTION;
    }
 
    //on définit l'instruction en question
     return decode_ins(p,ins);

}

int check_cond(arm_core p,uint8_t condition){

    //on récup les flags (ZNVC + d'autre inutile ici) voir page 49.
    uint32_t flags = arm_read_cpsr(p) >> 28;

    //cond contient le type d'instruction EQ,NE etc.. voir page 112.
    int r = 0;
    switch (condition){
        
    case EQ : 
        r = (flags & 4) != 0;
        break;
    case NE : 
        r = (flags & 4) == 0;
        break;
    case CS_HS :
        r = (flags & 2) != 0;
        break;
    case CC_LO :
        r = (flags & 2) == 0;
        break;
    case MI :
        r = (flags & 8) != 0;
        break;
    case PL :
        r = (flags & 8) == 0;
        break;
    case VS :
        r= (flags & 1) != 0;
        break;
    case VC :
        r = (flags & 1) == 0;
        break;
    case HI :
        r = (flags & 2) && !(flags & 4);
        break;
    case LS :
        r = !(flags & 2) || (flags & 4);
        break;
    case GE :
        r = ((flags & 8) >> 3) == (flags & 1);
        break;
    case LT :
        r = ((flags & 8) >> 3) != (flags & 1);
        break;
    case GT :
        r = !(flags & 4) && (((flags & 8) >> 3) == (flags & 1));
        break;
    case LE :
        r = (flags & 4) || (((flags & 8) >> 3) != (flags & 1));
        break;
    case AL :
        r = 1;
        break;
    case UNCOND : 
        r = 1;
        fprintf(stderr,"ARMv4 condition unpredictable \n"); //en ARMv5 UNCOND (0b1111) peut donner lieu à d'autre instruction voir page 149.
        break;
    default: r = 0;
    }

    return r;
}

int decode_ins(arm_core p,uint32_t ins){
    uint8_t pre_opcode = get_bits(ins,27,25); // 27-25
    uint8_t first_half_opcode = get_bits(ins,24,23); //24-23
    uint8_t bit_4 = get_bit(ins,4); //4
    uint8_t bit_20 = get_bit(ins,20); //20
    uint8_t bit_7 = get_bit(ins,7);

    switch(pre_opcode){
        case 0:
            if(first_half_opcode == 2 && bit_20 == 0 && bit_4 == 0) {
                return arm_miscellaneous(p,ins);
            }
            else if(bit_7 == 1 && bit_4 == 1){
                return arm_load_store(p,ins);
            }
            else if(bit_7 == 0 && bit_4 == 1){
                return arm_data_processing_shift(p,ins);
            }
            else if(bit_4 == 0){
                return arm_data_processing_shift(p,ins);
            }
            else{return UNDEFINED_INSTRUCTION;}
        case 1:
            if(first_half_opcode == 2){
                return UNDEFINED_INSTRUCTION;
            }
            else{
                return arm_data_processing_immediate_msr(p,ins);
            }
        case 2:
            return arm_load_store(p,ins);
        case 3:
            if(bit_4 == 0){
                return arm_load_store(p,ins);
            }
            else{
                return  UNDEFINED_INSTRUCTION;
            }
        case 4:
            return arm_load_store_multiple(p,ins);
        case 5:
            return arm_branch(p,ins);
        case 6:
            return arm_coprocessor_load_store(p,ins);
        case 7:
            return arm_coprocessor_others_swi(p,ins);
        default:
            return UNDEFINED_INSTRUCTION;
    }

}

enum cond_t get_cond(uint32_t ins){
    return ins >> 28;
}

int arm_step(arm_core p) {
    int result;

    result = arm_execute_instruction(p);
    if (result)
        arm_exception(p, result);
    return result;
}


