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

enum ins_t decode_ins(uint32_t ins){
    //reconnaitre entre data_process branch/misc/copro et load/store
    if(ins )
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


