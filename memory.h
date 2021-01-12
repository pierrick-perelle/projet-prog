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
#ifndef __MEMORY_H__
#define __MEMORY_H__
#include <stdint.h>
#include <sys/types.h>

typedef struct memory_data *memory;

 /*
memory_create
Donnees : size : taille de notre tableau,
		  is_big_endian : entier indiquant endianess de nos données (1 pour big-endian, 0 pour little-endian)
Resultat : retourne un pointeur vers la structure mem, qui vient d'être initialisé
Effet de bord : De la mémoire a été alloué
*/
memory memory_create(size_t size, int is_big_endian);


/*
memory_get_size
Donnees : mem : notre structure mémoire
Resultat : retourne la taille du tableau data (size)
Effet de bord : Pas d'effet de bord
*/
size_t memory_get_size(memory mem);


/*
memory_destroy
Donnees : mem : notre structure mémoire
Resultat : Aucun
Effet de bord : Libère de la mémoire
*/
void memory_destroy(memory mem);

/* All these functions perform a read/write access to a byte/half/word data at
 * address a in mem. The result is respectively taken from or stored to the
 * parameter value. The access is made using the given endianess (be == 1 for a
 * big endian access and be == 0 for a little endian access).
 * The return value indicates a succes (0) or a failure (-1).
 */


 /*
memory_read_byte
Donnees : mem : notre structure mémoire,
		  address : addresse de la valeur dans la structure,
		  value : la valeur lu (1 octet)
Resultat : En cas d'erreur retourne -1 sinon retourne 0
Effet de bord : On récupère dans notre sturture une valeur uint8_t que l'on place dans value
*/
int memory_read_byte(memory mem, uint32_t address, uint8_t *value);

 /*
memory_read_half
Donnees : mem : notre structure mémoire,
		  address : addresse de la valeur dans la structure,
		  value : la valeur lu (un demi-mot, 2 octets)
Resultat : En cas d'erreur retourne -1 sinon retourne 0
Effet de bord : On récupère dans notre sturture une valeur uint16_t que l'on place dans value
*/
int memory_read_half(memory mem, uint32_t address, uint16_t *value);

/*
memory_read_word
Donnees : mem : notre structure mémoire,
		  address : addresse de la valeur dans la structure,
		  value : la valeur lu (un mot, 4 octets)
Resultat : En cas d'erreur retourne -1 sinon retourne 0
Effet de bord : On récupère dans notre sturture une valeur uint32_t que l'on place dans value
*/
int memory_read_word(memory mem, uint32_t address, uint32_t *value);

/*
memory_write_byte
Donnees : mem : notre structure mémoire,
		  address : addresse à laquelle on veut insérer une valeur,
		  value : la valeur à écrire (1 octets)
Resultat : En cas d'erreur retourne -1 sinon retourne 0
Effet de bord : On insère dans notre sturture une valeur uint8_t que l'on place à l'adresse uint32_t address dans le tableau data
*/
int memory_write_byte(memory mem, uint32_t address, uint8_t value);

/*
memory_write_byte
Donnees : mem : notre structure mémoire,
		  address : addresse à laquelle on veut insérer une valeur,
		  value : la valeur à écrire (un demi-mot, 2 octets)
Resultat : En cas d'erreur retourne -1 sinon retourne 0
Effet de bord : On insère dans notre sturture une valeur uint16_t que l'on place à l'adresse uint32_t address dans le tableau data
*/
int memory_write_half(memory mem, uint32_t address, uint16_t value);

/*
memory_write_byte
Donnees : mem : notre structure mémoire,
		  address : addresse à laquelle on veut insérer une valeur,
		  value : la valeur à écrire (un mot, 4 octets)
Resultat : En cas d'erreur retourne -1 sinon retourne 0
Effet de bord : On insère dans notre sturture une valeur uint32_t que l'on place à l'adresse uint32_t address dans le tableau data
*/
int memory_write_word(memory mem, uint32_t address, uint32_t value);

#endif
