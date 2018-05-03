#include "simulator.h"

static char memory[65536][6];
int PC = 0;   
int orders_counter = 0;
long memout_size = 0;
static signed long regArr[16] = { 0 };

void initialize()
{
	int i = 0;

	for (i = 0; i < 65536; i++)
	{
		memset(memory[i], '0', sizeof(char*));
	}
}

int Fill_Memory_Array(FILE *input_file)
{
	char line[6];  //change to 5
	int line_number = 0;

	//fill array memory and count the amount of lines in memin.txt
	while (fgets(line, 6, input_file) != NULL)
	{
			memcpy(memory[line_number], line, sizeof(char*));
			line_number++;
	}

	//return the Array zise =  memory size
	return line_number;
}

//ctol -char to long
/*
ctol - convert char to long int (32 bits)

inputs - char word 
output - long int
algorithem - 1.get char
             2.convert char to string 
			 3.convert string to long with strtol
*/
long ctol(char word)
{
	char line[2];
	/*2*/
	line[1] = '\0';
	line[0] = word;
	/*3*/
	return ((signed long)strtol(line, NULL, 32));
}

//Do_Order
/*
Do_Order - do assembly order according to opcode 

inputs - int line_num - num of line wich simbolised a word in memory cell
         char opcode  - order type
		 char rd      - rd register type
		 char rs      - rs register type
		 char rt      - rt register type
		 signed long  - immidiate value

output - void

algorithem - 1.convert registers from char to long
			 2.check which register is imm
			 3.chek the opcode and oparate it according to if there is imm or not.
*/
int Do_order(int line_num, char opcode , char rd, char rs, char rt, signed long imm_flag)
{
	unsigned long intrd = 0, intrs = 0, intrt = 0;
	int  Newfirst16;
	int  imm_rd_flag = 0; 
	int  imm_rs_flag = 0;
	int  imm_rt_flag = 0;
	int  value = 0;
	char str[6];
	int imm = 0;

	orders_counter++;
	/*1*/
	intrd = ctol(rd);
	intrs = ctol(rs);
	intrt = ctol(rt);
	/*2*/
	if (imm_flag != 0)
	{
		if (rd == '1')
			imm_rd_flag = 1;
	    if (rs == '1')
			imm_rs_flag = 1;
		if (rt == '1')
			imm_rt_flag = 1;
	}
	/*3*/
	if (opcode == '0')			//add
	{
		if (imm_flag == 0)
		{
			regArr[intrd] = regArr[intrs] + regArr[intrt];
			PC += 1;
		}
		else 
		{
			imm = sigen_exctation(memory[PC + 1]);
			if (imm_rs_flag == 1)
				regArr[intrd] = imm + regArr[intrt];

			if (imm_rt_flag == 1)
				regArr[intrd] = regArr[intrs] + imm;
			PC += 2;
		}
	}

	else if (opcode == '1')     //sub
	{
		if (imm_flag == 0)
		{
			regArr[intrd] = regArr[intrs] - regArr[intrt];
			PC += 1;
		}
		else
		{
			imm = sigen_exctation(memory[PC + 1]);
			if (imm_rs_flag == 1)
				regArr[intrd] = imm - regArr[intrt];
			if (imm_rt_flag == 1)
				regArr[intrd] = regArr[intrs] - imm;
			PC += 2;
		}
	}

	else if (opcode == '2')    //bitwise and
	{ 
		if (imm_flag == 0)
		{
			regArr[intrd] = regArr[intrs] & regArr[intrt];
			PC += 1;
		}
		else
		{
			imm = sigen_exctation(memory[PC + 1]);
			if(imm_rs_flag == 1)
				regArr[intrd] = imm & regArr[intrt];
			if (imm_rt_flag == 1)
				regArr[intrd] = regArr[intrs] & imm;
			PC += 2;
		}
	}
	
	else if (opcode =='3')    //bitwise or
	{
		if (imm_flag == 0)
		{
			regArr[intrd] = regArr[intrs] | regArr[intrt];
			PC += 1;
		}
		else
		{
			imm = sigen_exctation(memory[PC + 1]);
			if(imm_rs_flag == 1)
				regArr[intrd] = imm | regArr[intrt];
			if(imm_rt_flag == 1)
				regArr[intrd] = regArr[intrs] | imm;
			PC += 2;
		}
	}
	
	else if (opcode == '4')    //sll
	{
		if (imm_flag == 0)
		{
			regArr[intrd] = regArr[intrs] << regArr[intrt];
			PC += 1;
		}
		else
		{
			imm = sigen_exctation(memory[PC + 1]);
			if(imm_rs_flag == 1)
				regArr[intrd] = imm << regArr[intrt];
			if(imm_rt_flag == 1)
				regArr[intrd] = regArr[intrs] << imm;
			PC += 2;
		}
	}

	else if (opcode == '5')    //sra
	{
		if (imm_flag == 0)
		{
			regArr[intrd] = regArr[intrs] >> regArr[intrt];
			PC += 1;
		}
		else
		{
			imm = sigen_exctation(memory[PC + 1]);
			if (imm_rs_flag == 1)
				regArr[intrd] = imm >> regArr[intrt];
			if (imm_rt_flag == 1)
				regArr[intrd] = regArr[intrs] >> imm;
			PC += 2;
		}
	}
	
	else if (opcode == '6')   //reserved
	{ /*unused*/  }	

	else if (opcode  == '7')   //beq
	{
		if (imm_flag == 0)
		{
			if (regArr[intrs] == regArr[intrt])
				PC = (int)(regArr[intrd] & 0xFFFF);
			else PC++;
		}
		if (imm_flag != 0)
		{
			imm = sigen_exctation(memory[PC + 1]);
			if(imm_rd_flag)
			{
				if (regArr[intrs] == regArr[intrt])
					PC = (int)(imm & 0xFFFF);
			}
			else if (imm_rs_flag)
			{
				if (imm == regArr[intrt])
					PC = (int)(regArr[intrd] & 0xFFFF);
			}
			else if (imm_rt_flag)
			{
				if (regArr[intrs] == imm)
					PC = (int)(regArr[intrd] & 0xFFFF);
			}
			else PC += 2;
		}
	}

	else if (opcode == '8')   //bgt
	{
		if (imm_flag == 0)
		{
			if (regArr[intrs] > regArr[intrt])
				PC = regArr[intrd] & 0xFFFF;
			else PC++;
		}
		if (imm_flag != 0)
		{
			imm = sigen_exctation(memory[PC + 1]);
			if (imm_rd_flag)
			{
				if (regArr[intrs] > regArr[intrt])
					PC = (int)imm & 0xFFFF;
				else PC += 2;
			}
			else if (imm_rs_flag)
			{
				if (imm > regArr[intrt])
					PC = (int)regArr[intrd] & 0xFFFF;
				else PC += 2;
			}
			else if (imm_rt_flag)
			{
				if (regArr[intrs] > imm)
					PC = (int)regArr[intrd] & 0xFFFF;
				else PC += 2;
			}
			
		}
	}
	
	else if (opcode == '9')   //ble
	{
		if (imm_flag = 0)
		{
			if (regArr[intrs] <= regArr[intrt])
				PC = regArr[intrd] & 0xFFFF;
			else PC++;
		}
		if (imm_flag != 0)
		{
			imm = sigen_exctation(memory[PC + 1]);
			if (imm_rd_flag)
			{
				if (regArr[intrs] <= regArr[intrt])
					PC = imm & 0xFFFF;
			}
			else if (imm_rs_flag)
			{
				if (imm <= regArr[intrt])
					PC = regArr[intrd] & 0xFFFF;
			}
			else if (imm_rt_flag)
			{
				if (regArr[intrs] <= imm)
					PC = regArr[intrd] & 0xFFFF;
			}
			else PC += 2;
		}
	}
	
	else if (opcode == 'A')   //bne
	{
		if (imm_flag = 0)
		{
			if (regArr[intrs] != regArr[intrt])
				PC = regArr[intrd] & 0xFFFF;
			else PC++;
		}
		if (imm_flag != 0)
		{
			imm = sigen_exctation(memory[PC + 1]);
			if (imm_rd_flag)
			{
				if (regArr[intrs] != regArr[intrt])
					PC = imm & 0xFFFF;
			}
			else if (imm_rs_flag)
			{
				if (imm != regArr[intrt])
					PC = regArr[intrd] & 0xFFFF;
			}
			else if (imm_rt_flag)
			{
				if (regArr[intrs] != imm)
					PC = regArr[intrd] & 0xFFFF;
			}
			else PC += 2;
		}


	}
	
	else if (opcode == 'B')   //jal
	{
		if (imm_flag == 0 )
		{ 
			regArr[15] = PC + 1;
			PC = regArr[intrd] & 0xFFFF;
		}
		if (imm_flag != 0)
		{
			imm = sigen_exctation(memory[PC + 1]);
			if (imm_rd_flag == 1)
			{ 
				regArr[15] = PC + 2;
				PC = imm & 0xFFFF;
			}
			if (imm_rs_flag == 1 || imm_rt_flag == 1)
			{
				regArr[15] = PC + 2;
				PC = regArr[intrd] & 0xFFFF;
			}
		}
	}

	else if (opcode == 'C')   //lw
	{
		if (imm_flag == 0)
		{
			regArr[intrd] = sigen_exctation(memory[(regArr[intrs]) + (regArr[intrt])-1]);
			PC += 1;
		}
		if (imm_flag != 0)
		{
			imm = sigen_exctation(memory[PC + 1]);
			if (imm_rs_flag == 1)
				regArr[intrd] = sigen_exctation(memory[imm + (regArr[intrt])]);
			if (imm_rt_flag == 1)
				regArr[intrd] = sigen_exctation(memory[(regArr[intrs]) + imm]);
			PC += 2;		
		}		
	}
	
	else if (opcode == 'D')   //sw
	{
		if (imm_flag == 0)
		{
			value = (int)(regArr[intrd] & 0xFFFF);
			itoa(value, str, 16);
			memcpy(memory[(regArr[intrs] + regArr[intrt] - 1)], str, sizeof(char*));
			if (memout_size < regArr[intrs] + regArr[intrt] - 1)
				memout_size = regArr[intrs] + regArr[intrt];
			
			PC += 1;
		}
		if (imm_flag != 0)
		{
			imm = sigen_exctation(memory[PC + 1]);
			if (imm_rd_flag == 1)
			{
				value = (int)(imm & 0xFFFF);
				itoa(value, str, 16);
				memcpy(memory[(regArr[intrs] + regArr[intrt] - 1)], str, sizeof(char*));
				if (memout_size < regArr[intrs] + regArr[intrt] - 1)
					memout_size = regArr[intrs] + regArr[intrt];
				PC += 2;
			}
			if (imm_rs_flag == 1)
			{
				value = (int)(regArr[intrd] & 0xFFFF);
				itoa(value, str, 16);
				memcpy(memory[(imm + regArr[intrt] - 1)], str, sizeof(char*));
				if (memout_size < imm + regArr[intrt] - 1)
					memout_size = imm + regArr[intrt];
				PC += 2;
			}
			if (imm_rt_flag == 1)
			{
				value = (int)(regArr[intrd] & 0xFFFF);
				itoa(value, str, 16);
				memcpy(memory[(regArr[intrs] + imm - 1)], str, sizeof(char*));
				if (memout_size < regArr[intrs] + imm - 1)
					memout_size = regArr[intrs] + imm;
				PC += 2;
			}
		}
	}
	
	else if (opcode == 'E')   //lhi
	{
		if (imm_flag == 0)
		{
			Newfirst16 = regArr[intrs] & 0xFFFF;
			regArr[intrd] = (regArr[intrd] & 0xFFFF) | (Newfirst16 << 16);
			PC += 1;
		}
		if (imm_flag != 0)
		{ 
			imm = sigen_exctation(memory[PC + 1]);
			if (imm_rs_flag == 1)  //can rd be imm, if so what will happend?
			{
				Newfirst16 = imm & 0xFFFF;
				regArr[intrd] = (regArr[intrd] & 0xFFFF) | (Newfirst16 << 16);
			}
			PC += 2;
		}

	}
	
	else if (opcode == 'F')   //halt
	{
		return - 1;
	}
	
	return 0;
}

void write_regArr_to_regout(FILE *output_file)
{
	int i = 2;
	for (i = 2; i < 16; i++)
		fprintf(output_file, "%.8x\n", regArr[i]);
}

void write_memout(FILE *output_file)
{ 
	int i = 0;
	for (i = 0; i < memout_size; i++)
		fprintf(output_file , "%s\n", memory[i]);
}

void write_trace(FILE *fp_trace, char instruc[] , char imm[])
{
	//need to fix R[0], R[1]

	fprintf(fp_trace, "%.8x ", PC);
	//print instruction
	if (imm == 0)
		fprintf(fp_trace, "%s ", instruc);
	if (imm != 0)
		fprintf(fp_trace, "%s%s ", imm, instruc);
	//print registers
	fprintf(fp_trace, "%.8x %.8x %.8x %.8x %.8x %.8x %.8x %.8x %.8x %.8x %.8x %.8x %.8x %.8x %.8x %.8x\n",
		regArr[0], regArr[1], regArr[2], regArr[3], regArr[4], regArr[5],
		regArr[6], regArr[7], regArr[8], regArr[9], regArr[10], regArr[11],
		regArr[12], regArr[13], regArr[14], regArr[15], regArr[16]);
}

void write_count(FILE *output_file)
{ 
	fprintf(output_file, "%d\n", orders_counter);
}

int if_imm(int mem_cell)
{
	if (memory[mem_cell][1] == '1' || memory[mem_cell][2] == '1' || memory[mem_cell][3] == '1')
		return 1;
	return 0;
}

//sigen_exctation
 /*
 The function convert hex string to long

 input:	-sting of a hex number

 output:	-the int with the value of the string

 algorithm:	-inilize dictonary to ipper and lower letters
 -intilize the base to 16^0;
 for loop
 -each iteration pass through a letter in string
 for loop
 -if the i'th letter in string match to the char in dic or DIC
 -add the value
 -update the base
 -break and return to the main loop
 -check if the number is negative
 -preform 2'compliment
 -retrun value
 */

 long sigen_exctation(char string[])
 {
	 char DIC[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };//upper letter dictonary
	 char dic[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };//lower letter dictonary
	 int i, j;
	 int value = 0;
	 long res = 0;
	 int base = 1;
	 for (i = 3; i >= 0; i--)
	 {
		 for (j = 0; j < 16; j++)
		 {
			 if ((string[i] == dic[j]) || (string[i] == DIC[j]))
			 {
				 value += j*base;
				 base = base * 16;
				 break;
			 }
		 }
	 }
	 if (j > 7)
	 {
		 value = value | 0XFFFF0000;
	 }
	 res = (long) value;
	 return res;
 }
 
 void fetch_decode_execute()
{
	FILE *input_file;
	FILE *fp_memout;
	FILE *fp_regout;
	FILE *fp_trace;
	FILE *fp_count;
	int memin_size = 0;
	int i = 0, imm_flag = 0, orders = 0, exit_flag = 0;
	long imm = 0;

	//initailise all static array used
	initialize();
	// open memin.txt file and convert to matrix
	input_file = fopen("memin.txt", "r");
	
	//check if file opened correctly
	if (input_file == NULL)
	{
		printf("Fill_Memory_Array: Error in openning memin.txt");
	}
	
	memout_size = (long)Fill_Memory_Array(input_file);
	fclose(input_file);
	fp_trace = fopen("trace.txt", "w"); //open trace file

	while ( PC < memout_size) //reade line each time;
	{
		imm_flag = if_imm(PC);
		if (imm_flag == 0)
		{
			write_trace(fp_trace, memory[PC], "0");
			exit_flag = Do_order(i, memory[PC][0], memory[PC][1], memory[PC][2], memory[PC][3], imm_flag);
		}
		else
		{
			write_trace(fp_trace, memory[PC], memory[PC + 1]);
			exit_flag = Do_order(i, memory[PC][0], memory[PC][1], memory[PC][2], memory[PC][3], imm_flag);
			
		}

		
		
		if (exit_flag == -1)  //reach halt
			break;
	}
	fclose(fp_trace);
	
	fp_regout = fopen("regout.txt", "w");
	if (fp_regout == NULL)
		printf("cannot open regout.txt\n");
	else 
	{
		write_regArr_to_regout(fp_regout);
		fclose(fp_regout);
	}

	fp_memout = fopen("memout.txt", "w");
	if (fp_memout == NULL)
		printf("cannot open memout.txt\n");
	else
	{
		write_memout(fp_memout);
		fclose(fp_memout);
	}

	fp_count = fopen("count.txt", "w");
	if (fp_memout == NULL)
		printf("cannot open count.txt\n");
	else
	{
		write_count(fp_count);
		fclose(fp_count);
	}
}

int main()
{
	FILE *fp;
	int num = 0, i = 0, C1 , C2 = 4, g = 1024;
	int j, k, t;
	long C3;
	char word = 'D';
	char line[4]="0400", str[33];
	char sec[4];
	
	//fp = fopen("regout.txt", "w");
	//write_regArr_to_regout(fp);
	//num = Fill_Memory_Array(fp);

	
	fetch_decode_execute();
	//printf("%s\n", memory[0]);

	//Do_order(0, '0', 'D', '0', '1', 1024);
	//printf("long= %ld\n", C3);
	//C3 = strtol("-9", NULL, 10);
	
	//fp = fopen("check.txt", "w");
	//fprintf(fp,"%s ", "DD01\n");
	//fprintf(fp, "%s", "0000");
	//fprintf(fp, "%s", "0000\n");
	//fclose(fp);
	

	
	printf("%d\n", 15);
	//printf("%s\n", memory[1900]);
	
	//fputs("0D01", fp);
	

	
	//fscanf(fp ,"%s", line);
	//fscanf(fp, "%x", &num);
	//fgets(line, 5, fp);
	//printf("%s\n", line);
	
	//printf("%d\n", num);
	//fclose(fp);
	return 0;
}
