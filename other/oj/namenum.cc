/*
 * ID: hankjin1
 * LANG: C++
 * TASK: namenum
 * */
 
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
using namespace std;

const char code[26] = {'2','2','2','3','3','3','4','4','4','5','5','5','6','6','6','7','0','7','7','8','8','8','9','9','9','0'};
char num[13], name[13], c[13];

int main()
{
    ifstream fin("namenum.in");
    ofstream fout("namenum.out");
    ifstream dict("dict.txt");
    bool flag = false;
    int i;
    fin>>num;
    while (dict>>name)
    {
	/*
	if (code[b[0]-'A'] != num[0]) 
	    continue;
	    */
	for (i = 0; name[i]; ++i)
	    c[i] = code[name[i]-'A'];
	c[i] = '\0';
	if (!strcmp(num, c))
	{
	    flag = true;
	    fout<<name;
	}
    }
    if (!flag) 
	fout<<"NONE";
}
