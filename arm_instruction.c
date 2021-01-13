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
typedef int (*ptr_func)(arm_core p, uint32_t ins);


//enum de chacun des opcode (cond) voir fig page 112
enum cond_t {EQ,NE,CS_HS,CC_LO,MI,PL,VS,VC,HI,LS,GE,LT,GT,LE,AL,UNCOND};
enum ins_t {BRANCH, LOAD_STORE, DATA_PROCESS};

static int arm_execute_instruction(arm_core p) {
    uint32_t ins;
    enum cond_t cond;
    enum ins_t instruction;

    if(arm_fetch(p,&ins) == -1){
        fprintf(stderr,"Erreur de fetch\n");
        return PREFETCH_ABORT;
        //voir page 58 pour les excpetions.
    }

    //ins contient maintenant l'instruction courante. voir arm_fetch()
    //on check si la condition est bonne.
    cond = get_cond(ins);
    if(!check_cond(cond)){
        return UNDEFINED_INSTRUCTION;
    }
 
    //on définit l'instruction en question
    instruction = decode_ins(ins);



    switch (instruction)
    {
        case BRANCH : 
            arm_global_branch(p, ins);
            
             break;

        case LOAD_STORE :
            arm_global_load_store(p,ins);
            break;

        case DATA_PROCESS :
            arm_global_data_process(p,ins);
             break;

        default :
            return UNDEFINED_INSTRUCTION;
    }


    return 0;
}

int check_cond(cond condition){

    //on récup les flags (ZNVC + d'autre inutile ici) voir page 49.
    uint32_t flags = arm_read_cpsr(p) >> 28;

    //cond contient le type d'instruction EQ,NE etc.. voir page 112.

    switch (condition){
    case EQ : 
        result = flags[3] == 1;
        break;
    case NE : 
    
        break;
    case CS_HS :
    
        break;
    case CC_LO :
    
        break;
    case MI :
    
        break;
    case PL :
    
        break;
    case VS :
    
        break;
    case VC :
    
        break;
    case HI :
    
        break;
    case LS :
    
        break;
    case GE :
    
        break;
    case LT :
    
        break;
    case GT :
    
        break;
    case LE :
    
        break;
    case AL :
    
        break;
    case UNCOND : 
        fprintf(stderr,"ARMv4 condition unpredictable \n"); //en ARMv5 UNCOND (0b1111) peut donner lieu à d'autre instruction voir page 149.
        break;
    default: return UNDEFINED_INSTRUCTION;
    }
}

static int decode_ins(arm_core p,uint32_t ins){
    uint8_t pre_opcode = get_bits(ins,27,25); // 27-25
    uint8_t first_half_opcode = get_bits(ins,24,23); //24-23
    uint8_t opcode = get_bits(ins,24,21); //24-21
    uint8_t bit_4 = get_bit(ins,4); //4
    uint8_t bit_20 = get_bit(ins,20); //20
    uint8_t bit_7 = get_bit(ins,7);

    switch(pre_opcode){
        case 0:
            if(first_half_opcode == 2 && bit_20 == 0 && bit_4 == 0) {
                return arm_miscellaneous(p,ins);
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
            return arm_coprocessor_others_swi(p,ubs);
        default:
            return UNDEFINED_INSTRUCTION;
    }

}

enum cond get_cond(uint32_t ins){
    return ins >> 28;
}

void initialiser_func_handler(){
    /*


    int arm_load_store(arm_core p, uint32_t ins);
    int arm_load_store_multiple(arm_core p, uint32_t ins);
    int arm_coprocessor_load_store(arm_core p, uint32_t ins);


    
    int arm_branch(arm_core p, uint32_t ins);
    int arm_coprocessor_others_swi(arm_core p, uint32_t ins);
    int arm_miscellaneous(arm_core p, uint32_t ins);

    int arm_data_processing(arm_core p,uint32)
    int arm_data_processing_shift(arm_core p, uint32_t ins);
    int arm_data_processing_immediate_msr(arm_core p, uint32_t ins);
    */
    tab[B_BL] = &arm_branch;
    tab[AND] = &arm;
}

int arm_step(arm_core p) {
    int result;

    result = arm_execute_instruction(p);
    if (result)
        arm_exception(p, result);
    return result;
}


