#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* define */
#define MEM_SIZE 65536;
#define LEN_SIZE 4;

/*functions*/
void fetch_decode_execute();   //main function
int Fill_Memory_Array(FILE *input_file); 
long ctol(char word);   //convert char to long int
int Do_order(int line_num, char opcode, char rd, char rs, char rt, signed long imm);
long sigen_exctation(char word[]);
void initialize();
void write_regArr_to_regout(FILE *output_file);
void write_memout(FILE *output_file);
void write_trace(FILE *fp_trace, char instruc[], char imm[]);
void write_count(FILE *output_file);
int if_imm(int mem_cell);





