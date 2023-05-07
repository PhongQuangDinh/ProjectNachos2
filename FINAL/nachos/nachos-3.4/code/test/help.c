/* help.c
 *	A simple program to output name of each members
 */

#include "syscall.h"

int main()
{	PrintString("==================================================\n");
	PrintString("     Project 2 - NachOS    \n");
	PrintString("\n");
	PrintString("1/ Members\n");
	PrintString("	21127149 - Huynh Minh Quang	\n");
	PrintString("	21127456 - Vo Cao Tri		\n");
	PrintString("	21127660 - Doan Thi Yen Nhi	\n");
	PrintString("	21127668 - Dinh Quang Phong	\n");
	PrintString("	21127730 - Hoang Le Cat Thanh \n");
	PrintString("2/ Available programs\n");
	PrintString("	ascii: Print out the ascii table.\n");
	PrintString("	sort: The user input an array of integer\n");
	PrintString(" 	      Use bubble sort to sort the array.\n");
	PrintString("==================================================\n");
}
