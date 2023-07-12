/*-----------------------------------------------------------------------
TITLE: Emulator (emu.cpp)
AUTHOR: NIKHIL PATHAK (2101CS50)
Declaration of Authorship
This file, emu.cpp, is part of the assignment of CS209/CS210 at the
department of Computer Science and Engineering, IIT Patna .
-------------------------------------------------------------------------*/

#include <bits/stdc++.h>
using namespace std;

int mem[20000005];		  // this array stores the array content
int pc, sp, a, b;		  // stores the value of 4 registers
vector<int> machinecodes; // stores the machinecodes

struct reading
{
	int add;
	int value;
};
vector<reading> read1; // stores memory read

struct writing
{
	int add;
	int previous;
	int present;
};
vector<writing> write1; // stores memory write

// This function converts the decimal to hex and returns tring
string dec_hex(int num)
{
	unsigned int num1 = num;
	string res = "";
	for (int i = 0; i < 8; i++)
	{
		int rem = num1 % 16;
		if (rem <= 9)
		{
			char ch = rem + '0';
			res += ch;
		}
		else
		{
			char ch = rem - 10 + 'a';
			res += ch;
		}
		num1 /= 16;
	}
	reverse(res.begin(), res.end());
	return res;
}

// Following function displays all possible commands
void inst_set()
{
	cout << "Opcode Mnemonic Operand" << endl;
	cout << "       data     value " << endl;
	cout << "0      ldc      value " << endl;
	cout << "1      adc      value " << endl;
	cout << "2      ldl      value " << endl;
	cout << "3      stl      value " << endl;
	cout << "4      ldnl     value " << endl;
	cout << "5      stnl     value " << endl;
	cout << "6      add            " << endl;
	cout << "7      sub            " << endl;
	cout << "8      shl            " << endl;
	cout << "9      shr            " << endl;
	cout << "10     adj      value " << endl;
	cout << "11     a2sp           " << endl;
	cout << "12     sp2a           " << endl;
	cout << "13     call     offset" << endl;
	cout << "14     return         " << endl;
	cout << "15     brz      offset" << endl;
	cout << "16     brlz     offset" << endl;
	cout << "17     br       offset" << endl;
	cout << "18     HALT           " << endl;
	cout << "       SET      value " << endl;
}

// Follwing function traces the .o file whole and last tells total instruction or segmentation fault
void trace_funct(int siz)
{
	pc = 0;
	int cnt = 0;
	bool chk = false, chk1 = false;
	while (pc < siz)
	{

		int instr = machinecodes[pc];
		// We break the machine code to offset and opcode offset is same as value
		int type = 0;
		for (int i = 0; i < 8; i++)
		{
			if (instr & (1 << i))
				type += (1 << i);
		}
		int offset = 0;
		for (int i = 8; i < 31; i++)
		{
			if (instr & (1 << i))
				offset += (1 << (i - 8));
		}
		if (instr & (1 << 31))
			offset -= (1 << (23));

		int prev = -1;
		// If else statements to check what needs to be done
		switch (type)
		{
		case 0:
			b = a;
			a = offset;
			break;
		case 1:
			a = a + offset;
			break;
		case 2:
			b = a;
			a = mem[sp + offset];
			read1.push_back({sp + offset, a});
			break;
		case 3:
			prev = mem[sp + offset];
			mem[sp + offset] = a;
			a = b;
			write1.push_back({sp + offset, prev, mem[sp + offset]});
			break;
		case 4:
			prev = a;
			a = mem[a + offset];
			read1.push_back({prev + offset, a});
			break;
		case 5:
			prev = mem[a + offset];
			mem[a + offset] = b;
			write1.push_back({a + offset, prev, b});
			break;
		case 6:
			a = b + a;
			break;
		case 7:
			a = b - a;
			break;

		case 8:
			a = (b << a);
			break;

		case 9:
			a = (b >> a);
			break;

		case 10:
			sp = sp + (offset);
			break;

		case 11:
			sp = a;
			a = b;
			break;

		case 12:
			b = a;
			a = sp;
			break;
		case 13:
			b = a;
			a = pc;
			pc = (pc + offset);
			break;

		case 14:
			pc = a;
			a = b;
			break;

		case 15:
			if (a == 0)
			{
				pc = (pc + offset);
			}
			break;

		case 16:
			if (a < 0)
			{
				pc = (pc + offset);
			}
			break;

		case 17:
			pc = (pc + offset);
			break;

		case 18:
			chk = true;
			break;
		}

		pc++;
		cout << "PC= " << dec_hex(pc) << " SP= " << dec_hex(sp) << " A= " << dec_hex(a) << " B= " << dec_hex(b) << endl;
		// If infinite loop or jump to wrong instruction exit
		if (pc < 0 || cnt > (1 << 24))
		{
			chk1 = true;
			break;
		}
		cnt++;
		if (chk)
			break;
	}
	if (chk1)
	{
		cout << "Segmentation fault or some other error" << endl;
		exit(0);
	}
	cout << "Total instructions ran are " << endl;
	cout << cnt << endl;
	chk = false;
}

int32_t main()
{
	// Taking input of object (.o) file
	string file_name;
	cout << "Enter the File name: ";
	cin >> file_name;
	ifstream file(file_name, ios::in | ios::binary);
	unsigned int cur;
	int counter = 0;
	// Reading file and storing machine codes
	while (file.read((char *)&cur, sizeof(int)))
	{
		machinecodes.push_back(cur);
		mem[counter++] = cur;
	}
	int siz = machinecodes.size();
	// POP-UP MENU program which gives you some functions to perform
	while (1)
	{
		cout << "please Select one of the operation to perform :" << endl;
		cout << endl;
		cout << "1: To get trace" << endl;
		cout << "2: To display inst_set" << endl;
		cout << "3: To wipe out all the flags" << endl;
		cout << "4: To show memory dump before execution" << endl;
		cout << "5: to show memory dump after execution" << endl;
		cout << "6: Show memory reads" << endl;
		cout << "7: Show memory writes" << endl;
		cout << "Any number >=8 to exit" << endl;
		cout << endl;
		cout << "Enter the instruction which you want to perform: ";

		int type;
		cin >> type;
		if (type == 1)
		{
			trace_funct(siz);
		}
		else if (type == 2)
		{
			inst_set();
		}
		else if (type == 3)
		{
			a = b = pc = sp = 0;
		}
		else if (type == 4)
		{
			cout << "Memory dump before execution" << endl;
			for (int i = 0; i < siz; i += 4)
			{
				cout << dec_hex(i) << " ";
				for (int j = i; j < min(siz, i + 4); ++j)
				{
					cout << dec_hex(machinecodes[j]) << " ";
				}
				cout << endl;
			}
		}
		else if (type == 5)
		{
			cout << "Memory dump after execution" << endl;
			for (int i = 0; i < siz; i += 4)
			{
				cout << dec_hex(i) << " ";
				for (int j = i; j < min(siz, i + 4); ++j)
				{
					cout << dec_hex(mem[j]) << " ";
				}
				cout << endl;
			}
		}
		else if (type == 6)
		{
			for (int i = 0; i < read1.size(); i++)
			{
				cout << "Read memory[" << dec_hex(read1[i].add) << "] found " << dec_hex(read1[i].value) << endl;
			}
		}
		else if (type == 7)
		{
			for (int i = 0; i < write1.size(); i++)
			{
				cout << "Wrote memory[" << dec_hex(write1[i].add) << "] was " << dec_hex(write1[i].previous) << " now " << dec_hex(write1[i].present) << endl;
			}
		}
		else
		{
			break;
		}
	}

	return 0;
}