

#define _CRT_SECURE_NO_WARNINGS
#define MAX_LEN 501

#include <stdio.h>
#include <string.h>
#include <ctype.h>



static char in_line[MAX_LEN];
static char sep_line[5][MAX_LEN];

void lineSeparator();//separate the words of the input line
void opcodeReader();//separate the line when it is an opcode
//void labelReader();//separate the line when it is an label
int WordE(int index);//find the end index of each word


int main()
{
	lineSeparator();
	printf("%s\n", sep_line[0]);
	printf("%s\n", sep_line[1]);
	printf("%s\n", sep_line[2]);
	return 0;
}

void lineSeparator()
{
	
	strcpy(sep_line[1], "Empty"); //intilazation of the strings
	strcpy(sep_line[2], "Empty");
	strcpy(sep_line[3], "Empty");
	strcpy(sep_line[4], "Empty");
	if (isspace(in_line[0])) //check if the first char is whitespace or a letter (define if it is a label or an opcode
		opcodeReader();
	else
		strcpy(sep_line[0], in_line);
}

int WordE(int index)
{
	while (!isspace(in_line[index]) && in_line[index] != NULL && in_line[index]!='#')//checking when the current word end or when the string end or when a comment begins
		index++;
	if (in_line[index-1] == ',')
		index--;
	return index;
}

void opcodeReader()
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

