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
#include "arm_data_processing.h"
#include "arm_exception.h"
#include "arm_constants.h"
#include "arm_branch_other.h"
#include "util.h"
#include "debug.h"

enum opcodes {AND,EOR,SUB,RSB,ADD,ADC,SBC,RSC,TST,TEQ,CMP,CMN,ORR,MOV,BIC,MVN};

int borrowFrom(uint32_t op1, uint16_t op2){
    if(op1<op2)
      return 1;
    return 0;          
}

int borrowFromRev(uint16_t op1, uint32_t op2){
    if(op1<op2)
      return 1;
    return 0;          
}

int carryFrom(uint32_t op1, uint16_t op2){
    if((int)op1+(int)op2>0xFFFFFFFF)
      return 1;
    return 0;          
}

int overflowFrom(uint32_t op1, uint16_t op2, uint32_t res, int isAddition){
    if(isAddition){
      if(get_bit(op1,31)==get_bit(op2,11) && get_bit(op1,31)!=get_bit(res,31))        //bit de signe sur le bit 11 ?????????????????
        return 1;
      return 0;
    }else{
      if(get_bit(op1,31)!=get_bit(op2,11) && get_bit(op1,31)!=get_bit(res,31))
        return 1;
      return 0;
    }        
}

int overflowFromRev(uint16_t op1, uint32_t op2, uint32_t res, int isAddition){
    if(isAddition){
      if(get_bit(op1,11)==get_bit(op2,31) && get_bit(op1,11)!=get_bit(res,31))      
        return 1;
      return 0;
    }else{
      if(get_bit(op1,11)!=get_bit(op2,31) && get_bit(op1,11)!=get_bit(res,31))
        return 1;
      return 0;
    }        
}

/* Decoding functions for different classes of instructions */
int arm_data_processing_shift(arm_core p, uint32_t ins) {

    uint8_t opcode = get_bits(ins,24,21);
    uint16_t shifter_operand=get_bits(ins,11,0);
    uint8_t rd = get_bits(ins,19,16);
    uint8_t rn = get_bits(ins,15,11);
    uint8_t S = get_bit(ins,20);
    uint32_t rnVal = arm_read_register(p,rn);
    uint16_t notcflag = !get_bit(arm_read_cpsr(p),29); 
    uint16_t cflag = get_bit(arm_read_cpsr(p),29);     
    uint32_t alu_out;
    uint32_t clearflagscpsr;
    uint32_t flags;

    switch (opcode){

        case AND:                  
          arm_write_register(p,rd,arm_read_register(p,rn)&shifter_operand); //AND
          if (S==1 && rd==PC){
            if (arm_current_mode_has_spsr(p)){
              arm_write_cpsr(p,arm_read_spsr(p));
            }else{
              //UNPREDICTABLE
            }
          }else if (S==1){
            uint32_t clearflagscpsr = arm_read_cpsr(p) & 0x3FFFFFFF;        //on passse N Z et C à 0, on garde V et C
            uint32_t flags = get_bit(arm_read_register(p,rd),31) << 1;      //N flag = rd[31]
            if(arm_read_register(p,rd)==0)                                  //Z flag = 1 si rd=0 et 0 sinon
              flags += 1;
            
            flags = flags<<30;                                              //C flag = on garde    +   on decale de 30 pour le | avec le cpsr
            
            arm_write_cpsr(p,flags|clearflagscpsr);
          }
          return NO_ERROR;
        break;

        case EOR:
          arm_write_register(p,rd,arm_read_register(p,rn)^shifter_operand);   //EOR
          if (S==1 && rd==PC){
            if (arm_current_mode_has_spsr(p)){
              arm_write_cpsr(p,arm_read_spsr(p));
            }else{
              //UNPREDICTABLE
            }
          }else if (S==1){
            uint32_t clearflagscpsr = arm_read_cpsr(p) & 0x3FFFFFFF;        
            uint32_t flags = get_bit(arm_read_register(p,rd),31) << 1;  
            if(arm_read_register(p,rd)==0)                              
              flags += 1;
            flags = flags<<30;
                                 
            
            arm_write_cpsr(p,flags|clearflagscpsr);
          }
          return NO_ERROR;
        break;

        case SUB:;     
          arm_write_register(p,rd,rnVal-shifter_operand);                     //SUB
          if (S==1 && rd==PC){
            if (arm_current_mode_has_spsr(p)){
              arm_write_cpsr(p,arm_read_spsr(p));
            }else{
              //UNPREDICTABLE
            }
          }else if (S==1){
            uint32_t clearflagscpsr = arm_read_cpsr(p) & 0x0FFFFFFF;              //cette fois on clear V aussi
            uint32_t flags = get_bit(arm_read_register(p,rd),31) << 1;                          //N
            if(arm_read_register(p,rd)==0)                              
              flags += 1;                                                                       //Z
            flags = flags<<1;
            
            flags = (flags + !get_bit(borrowFrom(rnVal,shifter_operand),0)) << 1;             //C
            
            flags += get_bit(overflowFrom(rnVal,shifter_operand,arm_read_register(p,rd),0),0); //V
            
            flags = flags<<28;                                                                  //shift de 28 cette fois pour V
            
            arm_write_cpsr(p,flags|clearflagscpsr);
          }
          return NO_ERROR;
        break;

        case RSB:;      
          arm_write_register(p,rd,shifter_operand-rnVal);                     //RSB
          if (S==1 && rd==PC){
            if (arm_current_mode_has_spsr(p)){
              arm_write_cpsr(p,arm_read_spsr(p));
            }else{
              //UNPREDICTABLE
            }
          }else if (S==1){
            uint32_t clearflagscpsr = arm_read_cpsr(p) & 0x0FFFFFFF;              
            uint32_t flags = get_bit(arm_read_register(p,rd),31) << 1;                          //N
            if(arm_read_register(p,rd)==0)                              
              flags += 1;                                                                       //Z
            flags = flags<<1;
            
            flags = (flags + !(get_bit(borrowFromRev(shifter_operand,rnVal),0))) << 1;             //C
            
            flags += get_bit(overflowFromRev(shifter_operand,rnVal,arm_read_register(p,rd),0),0); //V
            
            flags = flags<<28;                                                                  
            
            arm_write_cpsr(p,flags|clearflagscpsr);
          }
          return NO_ERROR;
        break;

        case ADD:;       
          arm_write_register(p,rd,rnVal+shifter_operand);                     //ADD
          if (S==1 && rd==PC){
            if (arm_current_mode_has_spsr(p)){
              arm_write_cpsr(p,arm_read_spsr(p));
            }else{
              //UNPREDICTABLE
            }
          }else if (S==1){
            uint32_t clearflagscpsr = arm_read_cpsr(p) & 0x0FFFFFFF;              
            uint32_t flags = get_bit(arm_read_register(p,rd),31) << 1;                          //N
            if(arm_read_register(p,rd)==0)                              
              flags += 1;                                                                       //Z
            flags = flags<<1;
            
            flags = (flags + get_bit(carryFrom(rnVal,shifter_operand),0)) << 1;              //C
            
            flags += get_bit(overflowFrom(rnVal,shifter_operand,arm_read_register(p,rd),1),0); //V
            
            flags = flags<<28;                                                                  
            
            arm_write_cpsr(p,flags|clearflagscpsr);
          }
          return NO_ERROR;
        break;

        case ADC:;    
          arm_write_register(p,rd, rnVal + shifter_operand + cflag );                     //ADC
          if (S==1 && rd==PC){
            if (arm_current_mode_has_spsr(p)){
              arm_write_cpsr(p,arm_read_spsr(p));
            }else{
              //UNPREDICTABLE
            }
          }else if (S==1){
            uint32_t clearflagscpsr = arm_read_cpsr(p) & 0x0FFFFFFF;              
            uint32_t flags = get_bit(arm_read_register(p,rd),31) << 1;                          //N
            if(arm_read_register(p,rd)==0)                              
              flags += 1;                                                                       //Z
            flags = flags<<1;
            
            flags = (flags + get_bit(carryFrom(rnVal,shifter_operand+cflag),0)) << 1;              //C
            
            flags += get_bit(overflowFrom(rnVal,shifter_operand+cflag,arm_read_register(p,rd),1),0); //V
            
            flags = flags<<28;                                                                  
            
            arm_write_cpsr(p,flags|clearflagscpsr);
          }
          return NO_ERROR;
        break;

        case SBC:;       
          arm_write_register(p,rd,rnVal-shifter_operand-notcflag);                     //SBC
          if (S==1 && rd==PC){
            if (arm_current_mode_has_spsr(p)){
              arm_write_cpsr(p,arm_read_spsr(p));
            }else{
              //UNPREDICTABLE
            }
          }else if (S==1){
            uint32_t clearflagscpsr = arm_read_cpsr(p) & 0x0FFFFFFF;              
            uint32_t flags = get_bit(arm_read_register(p,rd),31) << 1;                          //N
            if(arm_read_register(p,rd)==0)                              
              flags += 1;                                                                       //Z
            flags = flags<<1;
            
            flags = (flags + !(get_bit(borrowFrom(rnVal,shifter_operand - notcflag),0))) << 1;             //C
            
            flags += get_bit(overflowFrom(rnVal,shifter_operand - notcflag,arm_read_register(p,rd),0),0); //V
            
            flags = flags<<28;                                                                  //shift de 28 
            
            arm_write_cpsr(p,flags|clearflagscpsr);
          }
          return NO_ERROR;
        break;
        
        case RSC:;       
          arm_write_register(p,rd,shifter_operand-rnVal-notcflag);                     //SBC
          if (S==1 && rd==PC){
            if (arm_current_mode_has_spsr(p)){
              arm_write_cpsr(p,arm_read_spsr(p));
            }else{
              //UNPREDICTABLE;
            }
          }else if (S==1){
            uint32_t clearflagscpsr = arm_read_cpsr(p) & 0x0FFFFFFF;              
            uint32_t flags = get_bit(arm_read_register(p,rd),31) << 1;                          //N
            if(arm_read_register(p,rd)==0)                              
              flags += 1;                                                                       //Z
            flags = flags<<1;
            
            flags = (flags + !(get_bit(borrowFromRev(shifter_operand - notcflag,rnVal),0))) << 1;             //C
            
            flags += get_bit(overflowFromRev(shifter_operand - notcflag,rnVal,arm_read_register(p,rd),0),0); //V
            
            flags = flags<<28;                                                                  //shift de 28 
            
            arm_write_cpsr(p,flags|clearflagscpsr);
          }
          return NO_ERROR;
        break;

        case TST:
    		/* 
    			if ConditionPassed(cond) then
    			alu_out = Rn AND shifter_operand
    			N Flag = alu_out[31]
    			Z Flag = if alu_out == 0 then 1 else 0
    			C Flag = shifter_carry_out
    			V Flag = unaffected 
    			
    		*/
    
          alu_out = arm_read_register(p,rn) & shifter_operand;  // alu_out = Rn AND shifter_operand
    
    			clearflagscpsr = arm_read_cpsr(p) & 0x3FFFFFFF;
    
    			flags = get_bit(alu_out,31) << 1;      // N Flag = alu_out[31]
    
    			if (alu_out == 0){       //  Z Flag = if alu_out == 0 then 1 else 0
    				flags += 1; 		
    			}
			    flags = flags << 1 ;
			

    			flags = flags << 1 ;		// C Flag = shifter_carry_out  Inchangé
    
    			flags = flags << 1 ;		 //  V Flag = unaffected 
    
    			flags = flags<<28;

			    arm_write_cpsr(p,flags|clearflagscpsr);
			
		    break;

    		case TEQ:
    		/* 
    		if ConditionPassed(cond) then
    		alu_out = Rn EOR shifter_operand
    		N Flag = alu_out[31]
    		Z Flag = if alu_out == 0 then 1 else 0
    		C Flag = shifter_carry_out
    		V Flag = unaffected
    		*/
    
          alu_out = arm_read_register(p,rn) ^ shifter_operand;  // alu_out = Rn EOR shifter_operand
    
    			clearflagscpsr = arm_read_cpsr(p) & 0x3FFFFFFF;
    
    			flags = get_bit(alu_out,31) << 1;      // N Flag = alu_out[31]
    
    			if (alu_out == 0){       //  Z Flag = if alu_out == 0 then 1 else 0
    				flags += 1; 		
    			}
    			flags = flags << 1 ;
    			
    
    			flags = flags << 1 ;		// C Flag = shifter_carry_out Inchangé
    
    			flags = flags << 1 ;		 //  V Flag = unaffected 
    
    			flags = flags<<28;
    			arm_write_cpsr(p,flags|clearflagscpsr);
    
    		break;
    
    		case CMP:
    		/*
    		if ConditionPassed(cond) then
    		alu_out = Rn - shifter_operand
    		N Flag = alu_out[31]
    		Z Flag = if alu_out == 0 then 1 else 0
    		C Flag = NOT BorrowFrom(Rn - shifter_operand)
    		V Flag = OverflowFrom(Rn - shifter_operand)
    		*/
    		  alu_out = arm_read_register(p,rn) - shifter_operand;  // alu_out = Rn - shifter_operand
    
    			clearflagscpsr = arm_read_cpsr(p) & 0x0FFFFFFF;
               
    			flags = get_bit(alu_out,31);      // N Flag = alu_out[31]
    			flags = flags << 1 ;
    
    			if (alu_out == 0){       //  Z Flag = if alu_out == 0 then 1 else 0
    				flags += 1; 		
    			}
    			flags = flags << 1 ;
    			
    
    			flags = !borrowFrom(rn,shifter_operand);		// C Flag = NOT BorrowFrom(Rn - shifter_operand)
    			flags = flags << 1 ;
    
    			flags = overflowFrom(rn,shifter_operand,alu_out,0);		 //  V Flag = OverflowFrom(Rn - shifter_operand)
    			flags = flags << 1 ;
    
    			flags = flags<<28;
    			arm_write_cpsr(p,flags|clearflagscpsr);
    			
    
    		break;
    
    		case CMN:
    		/*
    		if ConditionPassed(cond) then
    		alu_out = Rn + shifter_operand
    		N Flag = alu_out[31]
    		Z Flag = if alu_out == 0 then 1 else 0
    		C Flag = CarryFrom(Rn + shifter_operand)
    		V Flag = OverflowFrom(Rn + shifter_operand)
    		*/
    		  alu_out = arm_read_register(p,rn) + shifter_operand;  // alu_out = Rn + shifter_operand
    
    			clearflagscpsr = arm_read_cpsr(p) & 0x0FFFFFFF;
    
    			flags = get_bit(alu_out,31) << 1;      // N Flag = alu_out[31]
    
    			if (alu_out == 0){       //  Z Flag = if alu_out == 0 then 1 else 0
    				flags += 1; 		
    			}
    			flags = flags << 1 ;
    			
    
    			flags = carryFrom(rn,shifter_operand);		// C Flag = CarryFrom(Rn + shifter_operand)
    			flags = flags << 1 ;
    
    			flags = overflowFrom(rn,shifter_operand,alu_out,0);		 //  V Flag = OverflowFrom(Rn + shifter_operand)
    			flags = flags << 1 ;
    
    			flags = flags<<28;
    			arm_write_cpsr(p,flags|clearflagscpsr);
    		break;
    
    		case ORR:
    		/*
    		if ConditionPassed(cond) then
    		Rd = Rn OR shifter_operand
    		if S == 1 and Rd == R15 then
    			if CurrentModeHasSPSR() then
    			CPSR = SPSR
    			else UNPREDICTABLE
    		else if S == 1 then
    		N Flag = Rd[31]
    		Z Flag = if Rd == 0 then 1 else 0
    		C Flag = shifter_carry_out
    		V Flag = unaffected
    		*/
    
    		  arm_write_register(p,rd,arm_read_register(p,rn)|shifter_operand); 			//  PAS COMPRIS AU SECOURS !!!
    
          if (S==1 && rd==PC){
                if (arm_current_mode_has_spsr(p)){
                  arm_write_cpsr(p,arm_read_spsr(p));
                }else{
                  //UNPREDICTABLE
                }
          }else if (S==1){
                uint32_t clearflagscpsr = arm_read_cpsr(p) & 0x3FFFFFFF;        //on passse N Z et C à 0, on garde V et C
                uint32_t flags = get_bit(arm_read_register(p,rd),31) << 1;      //N flag = rd[31]
                if(arm_read_register(p,rd)==0)                                  //Z flag = 1 si rd=0 et 0 sinon
                  flags += 1;
                
                flags = flags<<30;                                              //C flag = on garde    +   on decale de 30 pour le | avec le cpsr
                
                arm_write_cpsr(p,flags|clearflagscpsr);
              }
              return NO_ERROR;
    		break;
    
    		case MOV:
    		/*
    		if ConditionPassed(cond) then
    		Rd = shifter_operand
    		if S == 1 and Rd == R15 then
    		if CurrentModeHasSPSR() then
    		CPSR = SPSR
    		else UNPREDICTABLE
    		else if S == 1 then
    		N Flag = Rd[31]
    		Z Flag = if Rd == 0 then 1 else 0
    		C Flag = shifter_carry_out
    		V Flag = unaffected
    		*/
    		      arm_write_register(p,rd,arm_read_register(p,rn)|shifter_operand); //AND
              if (S==1 && rd==PC){
                if (arm_current_mode_has_spsr(p)){
                  arm_write_cpsr(p,arm_read_spsr(p));
                }else{
                  //UNPREDICTABLE
                }
              }else if (S==1){
                uint32_t clearflagscpsr = arm_read_cpsr(p) & 0x3FFFFFFF;        //on passse N Z et C à 0, on garde V et C
                uint32_t flags = get_bit(arm_read_register(p,rd),31) << 1;      //N flag = rd[31]
                if(arm_read_register(p,rd)==0)                                  //Z flag = 1 si rd=0 et 0 sinon
                  flags += 1;
                
                flags = flags<<30;                                              //C flag = on garde    +   on decale de 30 pour le | avec le cpsr
                
                arm_write_cpsr(p,flags|clearflagscpsr);
              }
              return NO_ERROR;
    		break;
    
    		case BIC:
    		/*
    		if ConditionPassed(cond) then
    		Rd = Rn AND NOT shifter_operand
    		if S == 1 and Rd == R15 then
    		if CurrentModeHasSPSR() then
    		CPSR = SPSR
    		else UNPREDICTABLE
    		else if S == 1 then
    		N Flag = Rd[31]
    		Z Flag = if Rd == 0 then 1 else 0
    		C Flag = shifter_carry_out
    		V Flag = unaffected
    		*/
    		      arm_write_register(p,rd,arm_read_register(p,rn)&!shifter_operand); //  AND NOT
              if (S==1 && rd==PC){
                if (arm_current_mode_has_spsr(p)){
                  arm_write_cpsr(p,arm_read_spsr(p));
                }else{
                  //UNPREDICTABLE
                }
              }else if (S==1){
                uint32_t clearflagscpsr = arm_read_cpsr(p) & 0x3FFFFFFF;        //on passse N Z et C à 0, on garde V et C
                uint32_t flags = get_bit(arm_read_register(p,rd),31) << 1;      //N flag = rd[31]
                if(arm_read_register(p,rd)==0)                                  //Z flag = 1 si rd=0 et 0 sinon
                  flags += 1;
                
                flags = flags<<30;                                              //C flag = on garde    +   on decale de 30 pour le | avec le cpsr
                
                arm_write_cpsr(p,flags|clearflagscpsr);
              }
              return NO_ERROR;
    		break;
    
    		case MVN:
    		/*
    		if ConditionPassed(cond) then
    		Rd = NOT shifter_operand
    		if S == 1 and Rd == R15 then
    		if CurrentModeHasSPSR() then
    		CPSR = SPSR
    		else UNPREDICTABLE
    		else if S == 1 then
    		N Flag = Rd[31]
    		Z Flag = if Rd == 0 then 1 else 0
    		C Flag = shifter_carry_out
    		V Flag = unaffected
    		*/
    		      arm_write_register(p,rd,!shifter_operand); //   NOT
              if (S==1 && rd==PC){
                if (arm_current_mode_has_spsr(p)){
                  arm_write_cpsr(p,arm_read_spsr(p));
                }else{
                  //UNPREDICTABLE
                }
              }else if (S==1){
                uint32_t clearflagscpsr = arm_read_cpsr(p) & 0x3FFFFFFF;        //on passse N Z et C à 0, on garde V et C
                uint32_t flags = get_bit(arm_read_register(p,rd),31) << 1;      //N flag = rd[31]
                if(arm_read_register(p,rd)==0)                                  //Z flag = 1 si rd=0 et 0 sinon
                  flags += 1;
                
                flags = flags<<30;                                              //C flag = on garde    +   on decale de 30 pour le | avec le cpsr
                
                arm_write_cpsr(p,flags|clearflagscpsr);
              }
              return NO_ERROR;
    		break;
       
    		default:
    		return UNDEFINED_INSTRUCTION;
   	}

}

int arm_data_processing_immediate_msr(arm_core p, uint32_t ins) {
    return UNDEFINED_INSTRUCTION;
}