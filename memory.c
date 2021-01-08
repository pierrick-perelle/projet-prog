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
    mem = malloc(sizeof(memory));
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
    free(memory);
}

int memory_read_byte(memory mem, uint32_t address, uint8_t *value) {
    if(address<0 || address>=mem->size)
      return -1;

    *value = *(mem->data+address);
    return 0;
    //return -1;    //comment détecter les erreurs ?
}

int memory_read_half(memory mem, uint32_t address, uint16_t *value) {
    uint8_t temp;
    if(mem->is_big_endian){
      if(memory_read_byte(mem,address,&temp)==-1)
        return -1;
      *value=temp<<8;
      if(memory_read_byte(mem,address+1,&temp)==-1)
        return -1;
      *value=*value|temp;
    }else{
      if(memory_read_byte(mem,address+1,&temp)==-1)
        return -1;
      *value=temp<<8;
      if(memory_read_byte(mem,address,&temp)==-1)
        return -1;
      *value=*value|temp;
    }
    return 0;
  
    /*if(mem->is_big_endian == is_big_endian())
       memory_read_byte(mem, address, *value);
       *value=*value<<8;
       memory
     else
      *value = get_bits(*(mem->data + address) , 31,16 );
    return 0;*/
}

int memory_read_word(memory mem, uint32_t address, uint32_t *value) {
    uint8_t temp;
    if(mem->is_big_endian){
      if(memory_read_byte(mem,address,&temp)==-1)
        return -1;
      *value=temp<<24;
      if(memory_read_byte(mem,address+1,&temp)==-1)
        return -1;
      *value=*value|(temp<<16);
      if(memory_read_byte(mem,address+2,&temp)==-1)
        return -1;
      *value=*value|(temp<<8);
      if(memory_read_byte(mem,address+3,&temp)==-1)
        return -1;
      *value=*value|temp;
    }else{
      if(memory_read_byte(mem,address+3,&temp)==-1)
        return -1;
      *value=temp<<24;
      if(memory_read_byte(mem,address+2,&temp)==-1)
        return -1;
      *value=*value|(temp<<16);
      if(memory_read_byte(mem,address+1,&temp)==-1)
        return -1;
      *value=*value|(temp<<8);
      if(memory_read_byte(mem,address,&temp)==-1)
        return -1;
      *value=*value|temp;
    }
    return 0;
}

int memory_write_byte(memory mem, uint32_t address, uint8_t value) {
    if(address<0 || address>=mem->size)
      return -1;
    *(mem->data + address) = value;
    return 0;
    return -1;
}

int memory_write_half(memory mem, uint32_t address, uint16_t value) {
    if(mem->is_big_endian){
      if(memory_write_byte(mem,address,value>>8)==-1)
        return -1;
      if(memory_write_byte(mem,address+1,value&0x00FF)==-1)
        return -1;
    }else{
      if(memory_write_byte(mem,address+1,value>>8)==-1)
        return -1;
      if(memory_write_byte(mem,address,value&0x00FF)==-1)
        return -1;
    }
    return 0;
}

int memory_write_word(memory mem, uint32_t address, uint32_t value) {
    if(mem->is_big_endian){
      if(memory_write_half(mem,address,value>>16)==-1)
        return -1;
      if(memory_write_half(mem,address+2,value&0x0000FFFF)==-1)
        return -1;
    }else{
      if(memory_write_half(mem,address+2,value>>16)==-1)
        return -1;
      if(memory_write_half(mem,address,value&0x0000FFFF)==-1)
        return -1;
    }
    return 0;
}
