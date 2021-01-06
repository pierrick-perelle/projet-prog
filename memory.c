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
#include <stdlib.h>
#include <stdio.h>
#include "memory.h"
#include "util.h"

struct memory_data {
    size_t size;
    int is_big_endian;
    uint8_t *data;
};

memory memory_create(size_t size, int is_big_endian) {
    memory mem=NULL;
    mem = malloc(sizeof(memory)*size);
    mem->is_big_endian=is_big_endian;
    mem->data=malloc(sizeof(uint8_t)*size);
    mem->size=size;
   return mem;
}

size_t memory_get_size(memory mem) {
    return mem->size;
}

void memory_destroy(memory mem) {
    free(mem->data);
}

int memory_read_byte(memory mem, uint32_t address, uint8_t *value) {


    *value = get_bit(*(mem->data + address*8) , *(mem->data + address + 8)  );
    return 0;
    //return -1;    //comment d�tecter les erreurs ?
}

int memory_read_half(memory mem, uint32_t address, uint16_t *value) {
    /*uint8_t *temp;
    if(memory_read_byte(mem,address,temp)==-1)
      return -1;
    *value=temp<<8;
    if(memory_read_byte(mem,address+8,temp)==-1)
      return -1;
    *value=*value|*temp;
    return 0;*/
  
    if(mem->is_big_endian == is_big_endian())
       *value = get_bits(*(mem->data + address) , 15,0 );
     else
      *value = get_bits(*(mem->data + address) , 31,16 );
    return 0;
}

int memory_read_word(memory mem, uint32_t address, uint32_t *value) {
    if(address+32>mem->size)
      return -1;
    *value = *(mem->data + address) ^ 0xFFFFFFFF;
    return 0;
    return -1;
}

int memory_write_byte(memory mem, uint32_t address, uint8_t value) {
    *(mem->data + address) = value;
    return 0;
    return -1;
}

int memory_write_half(memory mem, uint32_t address, uint16_t value) {
    if(memory_write_byte(mem,address,value>>8)==-1)
      return -1;
    if(memory_write_byte(mem,address+8,value&0x0F)==-1)
      return -1;
    return 0;
}

int memory_write_word(memory mem, uint32_t address, uint32_t value) {
    if(memory_write_half(mem,address,value>>16)==-1)
      return -1;
    if(memory_write_half(mem,address+16,value&0x0F)==-1)
      return -1;
    return 0;
}
