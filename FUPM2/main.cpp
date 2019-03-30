#include <iostream>
#include <vector>
#include <map>
#include <utility>

using namespace std;

vector<int> Registers(16);
vector<int> Memory(1048576);

enum code {
    HALT = 0,
    SYSCALL = 1,
    ADD = 2,
    ADDI = 3,
    SUB = 4,
    SUBI = 5,
    MUL = 6,
    MULI = 7,
    DIV = 8,
    DIVI = 9,
    LC = 12,
    SHL = 13,
    SHLI = 14,
    SHR = 15,
    SHRI = 16,
    AND = 17,
    ANDI = 18,
    OR = 19,
    ORI = 20,
    XOR = 21,
    XORI = 22,
    NOT = 23,
    MOV = 24,
    ADDD = 32,
    SUBD = 33,
    MULD = 34,
    DIVD = 35,
    ITOD = 36,
    DTOI = 37,
    PUSH = 38,
    POP = 39,
    CALL = 40,
    CALLI = 41,
    RET = 42,
    CMP = 43,
    CMPI = 44,
    CMPD = 45,
    JMP = 46,
    JNE = 47,
    JEQ = 48,
    JLE = 49,
    JL = 50,
    JGE = 51,
    JG = 52,
    LOAD = 64,
    STORE = 65,
    LOAD2 = 66,
    STORE2 = 67,
    LOADR = 68,
    LOADR2 = 69,
    STORER = 70,
    STORER2 = 71
};
enum mode {
  RM = 0,
  RR = 1,
  RI = 2
};
map <string, pair<code , mode>> cmdinfo = {
        {"halt",make_pair(HALT,RI)},
        {"syscall",make_pair(SYSCALL,RI)},
        {"add",make_pair(ADD,RR)},
        {"addi",make_pair(ADDI,RI)},
        {"sub",make_pair(SUB,RR)},
        {"subi",make_pair(SUBI,RI)},
        {"mul",make_pair(MUL,RR)},
        {"muli",make_pair(MULI,RI)},
        {"div",make_pair(DIV,RR)},
        {"divi",make_pair(DIVI,RI)},
        {"lc",make_pair(LC,RI)},
        {"shl",make_pair(SHL,RR)},
        {"shli",make_pair(SHLI,RI)},
        {"shr",make_pair(SHR,RR)},
        {"shri",make_pair(SHRI,RI)},
        {"and",make_pair(AND,RR)},
        {"andi",make_pair(ANDI,RI)},
        {"or",make_pair(OR,RR)},
        {"ori",make_pair(ORI,RI)},
        {"xor",make_pair(XOR,RR)},
        {"xori",make_pair(XORI,RI)},
        {"not",make_pair(NOT,RI)},
        {"mov",make_pair(MOV,RR)},
        {"addd",make_pair(ADDD,RR)},
        {"subd",make_pair(SUBD,RR)},
        {"muld",make_pair(MULD,RR)},
        {"divd",make_pair(DIVD,RR)},
        {"itod",make_pair(ITOD,RR)},
        {"dtoi",make_pair(DTOI,RR)},
        {"push",make_pair(PUSH,RI)},
        {"pop",make_pair(POP,RI)},
        {"call",make_pair(CALL,RR)},
        {"calli",make_pair(CALLI,RM)},
        {"ret",make_pair(RET,RI)},
        {"cmp",make_pair(CMP,RR)},
        {"cmpi",make_pair(CMPI,RI)},
        {"cmpd",make_pair(CMPD,RR)},
        {"jmp",make_pair(JMP,RM)},
        {"jne",make_pair(JNE,RM)},
        {"jeq",make_pair(JEQ,RM)},
        {"jle",make_pair(JLE,RM)},
        {"jl",make_pair(JL,RM)},
        {"jge",make_pair(JGE,RM)},
        {"jg",make_pair(JG,RM)},
        {"load",make_pair(LOAD,RM)},
        {"store",make_pair(STORE,RM)},
        {"load2",make_pair(LOAD2,RM)},
        {"store2",make_pair(STORE2,RM)},
        {"loadr",make_pair(LOADR,RR)},
        {"storer",make_pair(STORER,RR)},
        {"loadr2",make_pair(LOADR2,RR)},
        {"storer2",make_pair(STORER2,RR)}
};
map <string, int> reg_info = {
        {"r0",0}, // r0-r12 - свободно используются
        {"r1",1},
        {"r2",2},
        {"r3",3},
        {"r4",4},
        {"r5",5},
        {"r6",6},
        {"r7",7},
        {"r8",8},
        {"r9",9},
        {"r10",10},
        {"r11",11},
        {"r12",12},
        {"r13",13}, // Указатель фрейма вызова
        {"r14",14}, // Указатель стека
        {"r15",15} // Счётчик команд
};
void Switcher( int cmd, int reg_dst, int reg_src, int adress )
// cmd - команды,
// reg_dst - номер регистра-приёмника
// reg_src - номер рестра-источника
// adress - адрес или модификатор источника
{
    switch(cmd)
    {
        case '0':
            break;
        default:
            break;
    }
}
void Parcer( void )
{
    string cmd, reg_dst, reg_src;
    int adress;

    cin >> cmd;
    switch(cmdinfo.find(cmd)->second.second)
    {
        case '0': // RM (8 старших бит код команды, 4 следующих бита — код регистра (приёмника или источника),
                  // 20 младших бит — адрес в памяти в виде беззнакового числа от 0 до 2^20 − 1)
            cin >> reg_dst;
            cin >> adress;
            Switcher(cmdinfo.find(cmd)->second.first, reg_info.find(reg_dst)->second, -1, adress);
            break;
        case '1': // RR (8 бит код команды, 4 бит код регистра-приёмника, 4 бит код регистра-источника,
                  // 16 бит модификатор источника, число со знаком от −2^15 до 2^15 − 1)
            cin >> reg_dst;
            cin >> reg_src;
            cin >> adress;
            Switcher(cmdinfo.find(cmd)->second.first, reg_info.find(reg_dst)->second, reg_info.find(reg_src)->second, adress);
            break;
        case '2': // RI (8 бит код команды, 4 бит код регистра-приёмника,
                   // 20 бит непосредственный операнд, число со знаком от −2^19 до 2^19)
            cin >> reg_dst;
            cin >> adress;
            Switcher(cmdinfo.find(cmd)->second.first, reg_info.find(reg_dst)->second, reg_info.find(reg_src)->second, adress);
            break;
        default:
            break;
    }
    //cout << cmdinfo.find(cmd)->first << " " << cmdinfo.find(cmd)->second.first << " " << cmdinfo.find(cmd)->second.second;
}
int main() {
    Parcer();
    //for (auto it = cmdinfo.begin(); it != cmdinfo.end(); it++)
    //    cout << it->first << " " << it->second.first << " " << it->second.second <<  endl;
    return 0;
}

/*void Parcer1( void ) // Функция распознания
{
    string cmd, reg_dst, reg_src, adress; // Строки отвечающие за комманду, код регистра-приёмника,
                                          // код рестра-источника, адресс в памяти
    string mark; // метка

    cin >> mark;
    while (cmd != "end")
    {
        cout << word << endl;
        cin >> word;
    }
    cout << "Success";
}*/
