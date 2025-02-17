#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <cmath>

using namespace std;

vector<unsigned int> Registers(16);
vector<unsigned int> Memory(1048576);

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
void Switcher( int cmd, int reg_dst, int reg_src, long modifier )
// cmd - команды,
// reg_dst - номер регистра-приёмника
// reg_src - номер рестра-источника
// modifier - адрес или модификатор источника
{
    unsigned long mul, div;

    switch(cmd)
    {
        case 0:
            break;
        case 1:
            break;
        case 2: // add (Сложение регистров. К R1 (регистру-первому аргументу) прибавляется содержимое R2
                  // (регистра-второго аргумента), модифицированное непосредственным операндом)
            Registers[reg_dst] += Registers[reg_src] + static_cast<unsigned int>(modifier);
            break;
        case 3: // addi (Сложение регистра с непосредственным операндом.
                  // К содержимому R1 прибавляется значение Imm (непосредственного операнда))
            Registers[reg_dst] += static_cast<unsigned int>(modifier);
            break;
        case 4: // sub (Вычитание регистров. Из R1 вычитается содержимое R2)
            Registers[reg_dst] -= Registers[reg_src] + modifier;
            break;
        case 5: // subi (Вычитание из регистра непосредственного операнда)
            Registers[reg_dst] -= static_cast<unsigned int>(modifier);
            break;
        case 6: // mul (Умножение регистров.
                // Содержимое R1 умножается на R2. Результат помещается в пару регистров, начинающуюся с R1)
            mul = static_cast<unsigned long>(Registers[reg_dst]) * (Registers[reg_src] + modifier);
            Registers[reg_dst] = static_cast<unsigned int>(mul % static_cast<unsigned long>(pow(2.0, 32))); // Здесь можно сделать через побитовые операции
            Registers[reg_dst + 1] = static_cast<unsigned int>(mul / static_cast<unsigned long>(pow(2.0, 32)));
            break;
        case 7: // muli (Умножение регистра R1 на Imm.
                // Результат помещается в пару регистров, начинающуюся с R1.
            mul = static_cast<unsigned long>(Registers[reg_dst]) * modifier;
            Registers[reg_dst] = static_cast<unsigned int>(mul % static_cast<unsigned long>(pow(2.0, 32)));
            Registers[reg_dst + 1] = static_cast<unsigned int>(mul / static_cast<unsigned long>(pow(2.0, 32)));
            break;
        case 8: // div (Деление регистров
                // Младшие 32 бита первого операнда находятся в регистре R1, старшие — в регистре, номер которого на 1 больше R1.
                // Второй операнд находится в регистре. Частное помещается в регистр R1, остаток — в следующий регистр.
            div = static_cast<unsigned long>(Registers[reg_dst + 1]) * static_cast<unsigned long>(pow(2.0, 32)) + Registers[reg_dst];
            Registers[reg_dst] = static_cast<unsigned int>(div / Registers[reg_src]);
            Registers[reg_dst + 1] = static_cast<unsigned int>(div % Registers[reg_src]);
            break;
        case 9: // divi (Деление регистра на непосредственный операнд.
                // Аналогично div)
            div = static_cast<unsigned long>(Registers[reg_dst + 1]) * static_cast<unsigned long>(pow(2.0, 32)) + Registers[reg_dst];
            Registers[reg_dst] = static_cast<unsigned int>(div / modifier);
            Registers[reg_dst + 1] = static_cast<unsigned int>(div % modifier);
            break;
        case 12: // lc (Загрузка константы Imm в регистр R1.
                 // Для загрузки констант, б´ольших 2^20 требуются дополнительные команды сдвига и логического сложения)
            Registers[reg_dst] = static_cast<unsigned int>(modifier);
            break;
        case 13: // shl (Сдвинуть биты в регистре R1 влево на значение регистра R2)
            Registers[reg_dst] <<= Registers[reg_src];
            break;
        case 14: // shli (Сдвинуть биты в регистре R1 влево на Imm)
            Registers[reg_dst] <<= static_cast<unsigned int>(modifier);
            break;
        case 15: // shr (Сдвинуть биты в регистре R1 вправо на значение регистра R2.
                 // Сдвижка на 32 или более разрядов обнуляет регистр R1)
            if (Registers[reg_src] >= 32) {
                Registers[reg_dst] = 0;
                break;
            }
            Registers[reg_dst] >>= Registers[reg_src];
            break;
        case 16: // shri (Сдвинуть биты в регистре R1 вправо на Imm)
            if (static_cast<unsigned int>(modifier) >= 32) {
                Registers[reg_dst] = 0;
                break;
            }
            Registers[reg_dst] >>= static_cast<unsigned int>(modifier);
            break;
        case 17: // and (Логическое И регистров R1 и R2. Результат — в регистре R1)
            Registers[reg_dst] &= Registers[reg_src];
            break;
        case 18: // andi (Логическое И над регистром R1 и Imm)
            Registers[reg_dst] &= static_cast<unsigned int>(modifier);
            break;
        case 19: // or (Логическое ИЛИ регистров R1 и R2. Результат — в регистре R1)
            Registers[reg_dst] |= Registers[reg_src];
            break;
        case 20: // ori (Логическое ИЛИ над регистром R1 и Imm)
            Registers[reg_dst] |= static_cast<unsigned int>(modifier);
            break;
        case 21: // xor (Логическое исключающее ИЛИ регистров R1 и R2. Результат — в регистре R1)
            Registers[reg_dst] ^= Registers[reg_src];
            break;
        case 22: // xori (Логическое исключающее ИЛИ над регистром R1 и Imm)
            Registers[reg_dst] ^= static_cast<unsigned int>(modifier);
            break;
        case 23: // not (Поразрядное НЕ над всеми битами R1.
                 // Непосредственный операнд игнорируется, но он может присутствовать в команде)
            Registers[reg_dst] ^= static_cast<unsigned int>(pow(2.0, 32) - 1);
            break;
        default:
            break;
    }
}
void ParcerCmd( void )
{
    string cmd, reg_dst, reg_src;
    long modifier;

    while (1) {
        cin >> cmd;
        if (cmdinfo.find(cmd) == cmdinfo.end())
            break;
        switch (cmdinfo.find(cmd)->second.second) {
            case 0: // RM (8 старших бит код команды, 4 следующих бита — код регистра (приёмника или источника),
                // 20 младших бит — адрес в памяти в виде беззнакового числа от 0 до 2^20 − 1)
                cin >> reg_dst;
                cin >> modifier;
                Switcher(cmdinfo.find(cmd)->second.first, reg_info.find(reg_dst)->second, 0, modifier);
                break;
            case 1: // RR (8 бит код команды, 4 бит код регистра-приёмника, 4 бит код регистра-источника,
                // 16 бит модификатор источника, число со знаком от −2^15 до 2^15 − 1)
                cin >> reg_dst;
                cin >> reg_src;
                cin >> modifier;
                Switcher(cmdinfo.find(cmd)->second.first, reg_info.find(reg_dst)->second,
                         reg_info.find(reg_src)->second, modifier);
                break;
            case 2: // RI (8 бит код команды, 4 бит код регистра-приёмника,
                // 20 бит непосредственный операнд, число со знаком от −2^19 до 2^19)
                cin >> reg_dst;
                cin >> modifier;
                Switcher(cmdinfo.find(cmd)->second.first, reg_info.find(reg_dst)->second, 0, modifier);
                break;
            default:
                break;
        }
    }
    //cout << cmdinfo.find(cmd)->first << " " << cmdinfo.find(cmd)->second.first << " " << cmdinfo.find(cmd)->second.second;
}
int main() {
    cout << Registers[0] << " " << Registers[1] << endl;

    ParcerCmd();

    cout << Registers[0] << " " << Registers[1] << endl;
    //for (auto it = cmdinfo.begin(); it != cmdinfo.end(); it++)
    //    if (cmdinfo.find(it->first)->second.second == 0)
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
