
#include "Header.h"

static char in_line[MAX_LEN];
static char sep_line[5][MAX_LEN];

int main()
{
	reader("fib.asm", "check.txt");
	return 0;
}


/*
Read and write the input and output files
*/

void reader(char ReadFile[], char WriteFile[])
{
	FILE *fpR, *fpW;
	label *head = NULL;
	Word *WHead = NULL;
	int count = 0, line =0;
	char **stringArr = NULL;
	fpR = fopen(ReadFile, "r");
	if (fpR == NULL)
		exit(-1);
	head = ListCreator(fpR, &WHead, &count);
	fclose(fpR);
	fpR = fopen(ReadFile, "r");
	fpW = fopen(WriteFile, "w");
	if (fpR == NULL || fpW == NULL)
		exit(-1);
	count = max(count, 1+FindMaxWord(WHead));
	stringArr = D_Arr_Creator(count);
	while (fgets(in_line, MAX_LEN, fpR) != NULL)
	{
		if (strcmp(in_line, "\n") == 0)
			continue;
		lineSeparator();
		Writer(head, WHead, &line, stringArr);
	}
	fclose(fpR);
	for (line = 0; line < count; line++)
		fprintf(fpW, "%s\n", stringArr[line]);
	fclose(fpW);

}


/*
	The function write each line to the memmory array


	input:	-label linked list
			-Word linked list
			-int pointer of the number of current line
			-2D string array of the main memmory

	algorithm:	-if the word is not a opcode( a label, a whitespace, a comment and so)
					-end the function
				if the input is .word
					-find the data and place it in place
					-end the function
				-fill the line with the opcode parameters
				-if there is use of imm or label
					-advance the line by 1
					-check if the information is a number or a label
					translate the data to the correct foramt and place it in the memmory
				advance the line by one
						

*/

void Writer(label *Lhead, Word *Whead, int *line, char **memArr)
{
	label *currentL;
	Word *currentW;
	int check = 0;
	char str[MAX_LEN];
	if (strcmp(sep_line[1], "Empty") == 0)//it is not a opcode
		return;
	if (strcmp(sep_line[0], ".word") == 0)
	{
		currentW = FindWord(Whead, sep_line[1]);
		sprintf(str, "%.4d", currentW->value);
		strcpy(memArr[currentW->line], decToHex(str));
		return;
	}
	FillOpcode(*line, memArr);
	if (strcmp(sep_line[4], "Empty") != 0)//there is use in imm or a label
	{
		(*line)++;
		check = IsNum(sep_line[4]);
		if (check == 1)
		{
			strcpy(memArr[*line],decToHex(sep_line[4]));
		}
		else
		{
			currentL = findLabel(Lhead, sep_line[4]);
			sprintf(str, "%.4d", currentL->place);
			strcpy(memArr[*(line)], decToHex(str));
		}		
	}
	(*line)++;
}


/*
the function use the static string in_line and separate it into words in the format of an opcode

algorithm:	-intilize all the arrays in sep_line to "Empty"
-if the first letter is whitespace
separate the line to opcode format
else
copy all the line as is (label format) without ':' if occure in the end of the line
*/
void lineSeparator()
{

	strcpy(sep_line[1], "Empty"); //intilazation of the strings
	strcpy(sep_line[2], "Empty");
	strcpy(sep_line[3], "Empty");
	strcpy(sep_line[4], "Empty");
	if (isspace(in_line[0])) //check if the first char is whitespace or a letter (define if it is a label or an opcode)
		opcodeReader();
	else
	{
		strcpy(sep_line[0], in_line);
		if (sep_line[0][strlen(sep_line) - 2] == ':')
			sep_line[0][strlen(sep_line) - 2] = '\0';
	}
}


/*
separate in_line to opcode format

algorithm:	-while loop
-if the char is a whitespace (meaning a word hasn't start)
continue the while until a word start has found
- if the letter '#' occure stop reading
-find the end index of the word
-copy the word to her place in sep_line
-continue the while

*/

void opcodeReader()//separate the line when it is an opcode format
{
	int start = 1, end, i = 0;
	while (start<MAX_LEN && in_line[start] != '\0')
	{
		if (isspace(in_line[start]))// find when the word start
		{
			start++;
			continue;
		}
		if (in_line[start] == '#')//check if it is a comment
			break;
		end = WordE(start);
		strncpy(sep_line[i], &in_line[start], end - start);//copy the word to it place
		sep_line[i][end - start] = NULL;
		start = end + 1;
		i++;
	}
}

/*
find the end index of each word

input:	start index of a word

output:	end index of a word

algorithm:	-while loop
each iteration check if the current char is not whitespace, NULL or '#'
if it isn't index++
-if the last index is ','
index--
-return index
*/

int WordE(int index)
{
	while (!isspace(in_line[index]) && in_line[index] != NULL && in_line[index] != '#')//checking when the current word end or when the string end or when a comment begins
		index++;
	if (in_line[index - 1] == ',')
		index--;
	return index;
}


/*function: FindOPcode()
The function gets a char which contain the order and return the opcode of it by the table
in the project

inputs: char real_oreder[] - order from memin

outputs: int opcode

algorithem:	-set two arrays
one contains all the possible orders.
second contains the opcodes.
-for loop
in each iteration we caompwr the input to one of the possible order
if the words are wqual
get the opcode from the opcode array
break the loop
else
continue
if match not fond return -1
*/

int FindOPcode(char real_order[])
{

	int possible = 0;
	char orders[][16] = { "add", "sub", "and", "or", "sll", "sra", "reserved", "beq",
		"bgt", "ble", "bne", "jal", "lw", "sw", "lhi", "halt", ".word" };
	int optional_codes[17] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
	for (possible = 0; possible < 17; possible++)
	{


		if (strcmp(orders[possible], real_order) == 0)
		{
			return possible;
		}
	}
	return -1;
}

/*
create a new label struct

-input: label name in string, the place of the label in int

-output: return the new label struct
*/
label* LabelCreator(int line)
{
	label *newL;
	newL = (label*)malloc(sizeof(label));
	strcpy(newL->name, sep_line[0]);
	newL->place = line;
	newL->next = NULL;
	return newL;
}

/*
find the label by it name

input: labels list, sring with the name of the laabel

output: the requested label

algorithm:	-check if the list is not empty
-while loop
-if the current label name is the requested one
returnt the current label
-continue to search
-if not found return NULL
*/

label* findLabel(label* head, char name[])
{
	label *temp = head;
	if (temp == NULL)
		return NULL;
	while (temp != NULL)
	{
		if (strcmp(temp->name, name) == 0)
			return temp;
		temp = temp->next;
	}
	return NULL;
}

/*
The function creat linked list of all the labels in the input file

input:	pointer to the read file

output:	return liked list of label struct

algorithem:	-while loop
-the iterations continue while there are still lines to read from the file
-if the line is not "\n"
-if the first char is not white space (it is a label)
-if the list is empty (there aren't labels yet)
-create a new label and start is pointing on it
-else (there are already labels in the list)
-create a label
-update temp pointer to point the new label
-else if this is '.word' format
-update the Word linked list
-else
-check if there is use of imm and count if there is a use of it
-count the line in order to know what is the line of each label
*/

label* ListCreator(FILE* fp, Word** list, int *count)
{
	label *start = NULL, *temp = NULL;
	Word *Wtemp = NULL;
	int check = 0;
	while (fgets(in_line, MAX_LEN, fp) != NULL)
	{
		if (strcmp(in_line, "\n") != 0)
		{
			lineSeparator();
			if (!isspace(in_line[0]))
			{
				if (temp == NULL)
				{
					start = LabelCreator(*count);
					temp = start;
				}
				else
				{
					temp->next = LabelCreator(*count);
					temp = temp->next;
				}
			}
			else if (strcmp(sep_line[0], ".word") == 0)
			{

				if ((*list) == NULL)
				{
					*list = CreateWord();
					Wtemp = *list;
				}
				else
				{
					Wtemp->next = CreateWord();
					Wtemp = Wtemp->next;
				}
			}
			else
			{
				*count += checkImm();
				(*count)++;
			}
		}
	}

	return start;
}

/*
The function return the number of the register

input: A string which contain a register name

output: The register's number

algorithm: -for loop
-if the input string match the current register
return the register number
*/

char FindReg(char reg[])
{
	char list[][10] = { "$zero", "$imm", "$v0", "$a0", "$a1", "$t0",
		"$t1", "$t2", "$t3", "$s0", "$s1", "$s2", "$gp", "$sp", "$fp", "$ra" };
	char code[17] = { "0123456789ABCDEF" };
	int i;
	for (i = 0; i < 16; i++)
	{
		if (strcmp(reg, list[i]) == 0)
			return code[i];
	}
	return NULL;
}

/*
The function check if $imm occure in the line

algorithm: -for loop
-if the register is "$imm"
return 1
there are no use of $imm
return 0
*/

int checkImm()
{
	int i;
	for (i = 1; i < 4; i++)//$imm cant be at i==0 or i==4
	{
		if (strcmp(sep_line[i], "$imm") == 0)
			return 1;
	}
	return 0;
}

/*
The function create a word sturct from the input line
*/

Word* CreateWord()
{
	Word *newW;
	newW = (Word*)malloc(sizeof(Word));
	if (sep_line[1][0] == '0' || sep_line[1][0] == 'x')
		newW->line = hexToDec(sep_line[1]);
	else
		newW->line = atoi(sep_line[1]);
	if (sep_line[2][0] == '0' || sep_line[2][0] == 'x')
		newW->value = hexToDec(sep_line[2]);
	else
		newW->value = atoi(sep_line[2]);
	newW->next = NULL;
	return newW;
}

/*
The function return a Word by line search if exist

input:	-int *count - show the current line number which is read
-Word* list - the head of the Word linked list

output:	-return pointer to the match Word if exist, else return NULL

algorithm:	-check if list dosn't exit
return NULL
-while loop
each iteration check if temp is NULL
-if temp->line is even to count's value
return temp (a matched word)
-continue to the next Word in the list
if a Word havn't found return NULL
*/

Word* CheckIfWord(int *count, Word* list)
{
	Word *temp = list;
	if (list == NULL)
		return NULL;
	while (temp != NULL)
	{
		if (temp->line == *count)
			return temp;
		temp = temp->next;
	}
	return NULL;
}

/*
The function find the max line for .word

input:	limked list of Word

output:	max line of .word


*/

int FindMaxWord(Word *Whead)
{
	int maxVal = 0;
	Word *temp;
	temp = Whead;

	while (temp != NULL)
	{
		if (temp->line > maxVal)
			maxVal = temp->line;
		temp = temp->next;
	}
	return maxVal;
}


/*
	The function create 2D array of strings (the proccessor main memmory)

	intput:	int count-number of lines which need to print in the program

	output: 2D array of strings fill with the string "0000" in each line

	algorithm: -create pointer to the array
				-check if succeed
				-for loop
					create a line
					check if succeed
					fill each line with "0000"
				return the array
*/

char **D_Arr_Creator(int count)
{
	int i;
	char** arr;
	char** temp;
	arr = (char **) malloc(count * sizeof(char *));
	if (arr == NULL)
		exit(1);
	temp = arr;
	for (i = 0; i < count; i++)
	{
		arr[i] = (char*) malloc( 5 * sizeof(char));
		if (arr[i] == NULL)
			exit(1);
		strcpy(arr[i], "0000");
	}
	return temp;

}

/*
	The function match the registers and the opcode with their corecct values and fill the corecct line in the memmory

	input:	int pointers to opcode, r1, r2, r3
*/

void FillOpcode(int line, char **memArr)
{
	char str[5];
	char code[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	str[0] = code[FindOPcode(sep_line[0])];
	str[1] = FindReg(sep_line[1]);
	str[2] = FindReg(sep_line[2]);
	str[3] = FindReg(sep_line[3]);
	str[4] = '\0';
	strcpy(memArr[line], str);
}

/*
	The function read a string and check if it represent a number

	input:	char string[]

	output:	int 1 if the string represent a number
			int	-1 if the string dosn't represent a number

	algorithm:	-while loop
					-each iteration check if the string end
					-if the i'th letter is not a number
						return -1
				return 1 (it is a number)
*/

int IsNum(char string[])
{
	int i = 0;
	if (string[0] == '-')
		i++;
	while (string[i] != '\0')
	{
		if (isdigit(string[i]) == 0)
			return -1;
		i++;
	}
	return 1;
}

/*
	The function write 
*/

void ImmFiller(char memArr[][5], int line, label *head)
{
	int check = IsNum(sep_line[4]);
	label *temp;
	char dec[20];
	if (check == 1)
		strcpy(memArr[line], decToHex(sep_line[4]));
	else
	{
		temp = findLabel(head, sep_line[4]);
		sprintf(dec, "%d", temp->place);
		strcpy(memArr[line], decToHex(dec));
	}
}

/*
	The function transform a string of decmial number into a string of hexadecimal number

	input:	-char string (string of a Decimal number)

	output:	-char arr (string of a hexaDecimal number)

	algorithm:	-covert the string to int
				-if the number is not negative
					-convert as it is
				else
					-preform 2's complimet translate
					-covert the int to 8 digit hexadecimal number
					-for loop
						cut the 4 right digit of the hexa string
				return the 4 digit hexa number as string
*/

char* decToHex(char string[])
{
	char arr[9];
	int i;
	int num = atoi(string);
	if (num >= 0)
		sprintf(arr, "%.4X", num);
	else
	{
		num = abs(num);
		num = ~num;
		num++;
		sprintf(arr, "%.8X", num);
		for (i = 0; i < 4; i++)
			arr[i] = arr[i + 4];
		arr[4] = '\0';
	}
	return arr;
}


/*
	The function convert hex string to int
	
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

int hexToDec(char string[])
{
	char DIC[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };//upper letter dictonary
	char dic[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };//lower letter dictonary
	int i, j;
	int value = 0;
	int base = 1;
	for (i = strlen(string)-1; i >=0; i--)
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
	return value;
}


/*
	The function find the Word struck which match the input

	input:	-linked list of Word struct
			-string of the line of the .word input

	output:	-Word struck pointer

	algorithm:	-translate the string to a int (deal both case of input in hex and in decimal
				-while loop
					-while the linked list hasn't ended
						if the current struct match the input
							return it
						continue to the next struct
*/

Word* FindWord(Word *head, char string[])
{
	Word* temp;
	int num;
	temp = head;
	if ((string[0] == '0') && string[1] == 'x')
		num = hexToDec(string + 2);
	else
		num = atoi(string);
	while (temp != NULL)
	{
		if (temp->line == num)
			return temp;
		temp = temp->next;
	}
	return NULL;
}


/*
	The function free the linked list of Word struct

	input: Word linked list
*/

void FreeW(Word* head)
{
	Word* temp;
	while (head != NULL)
	{
		temp = head->next;
		free(head);
		head = temp;
	}
}

/*
The function free the linked list of label struct

input: label linked list
*/

void FreeL(label *head)
{
	label* temp;
	while (head != NULL)
	{
		temp = head->next;
		free(head);
		head = temp;
	}
}

/*
	The function free the 2D array of the memmory

	input:	-2D array of string
*/

void FreeStr(char **arr, int size)
{
	int i;
	for (i = 0; i < size; i++)
		free(arr[i]);
	free (arr);
}

