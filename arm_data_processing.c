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
#include "arm_data_processing.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "arm_branch_other.h"
#include "util.h"
#include "debug.h"

enum opcodes {AND,EOR,SUB,RSB,ADD,ADC,SBC,RSC,TST,TEQ,CMP,CMN,ORR,MOV,BIC,MVN};

/* Decoding functions for different classes of instructions */
int arm_data_processing_shift(arm_core p, uint32_t ins) {

	uint8_t opcode = get_bits(ins,24,21);
    uint16_t shifter_operand=get_bits(ins,11,0);
    uint8_t rd = get_bits(ins,19,16);
    uint8_t rn = get_bits(ins,15,11);

	switch (opcode)​{

		case AND:
		// Equal
		break;

		case EOR:
		// Not Equal
		break;

		case SUB:
		// Not Equal
		break;

		case RSB:
		// Not Equal
		break;

		case ADD:
		// Not Equal
		break;

		case ADC:
		// Not Equal
		break;

		case SBC:
		// Not Equal
		break;
		case RSC:
		// Not Equal
		break;

		case TST:
		// Not Equal
		break;

		case TEQ:
		// Not Equal
		break;

		case CMP:
		// Not Equal
		break;

		case CMN:
		// Not Equal
		break;

		case ORR:
		// Not Equal
		break;

		case MOV:
		// Not Equal
		break;

		case BIC:
		// Not Equal
		break;

		case MVN:
		// Not Equal
		break;
		default:
		return UNDEFINED_INSTRUCTION;
	}

}

int arm_data_processing_immediate_msr(arm_core p, uint32_t ins) {
    return UNDEFINED_INSTRUCTION;
}
