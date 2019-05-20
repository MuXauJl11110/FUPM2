#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <cmath>
#include <fstream>
#include <string>
#include <algorithm>
#include <cassert>
#include <cstdio>

using namespace std;

vector<unsigned int> Registers(16);
vector<unsigned int> Memory(1048576);
vector<bool> Flags(4);
/* Флаги:
   C - переноса (Carry). Устанавливается при сложении больших чисел,
   Z - нуля (Zero). Устанавливается, если результат равен нулю.
   O - переполнения (Overflow).
   S - результат отрицательный (Sign). */
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
map <string, unsigned int> reg_info = {
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
map <int, mode> cmd_info_mode = {
        {0,RI},
        {1,RI},
        {2,RR},
        {3,RI},
        {4,RR},
        {5,RI},
        {6,RR},
        {7,RI},
        {8,RR},
        {9,RI},
        {12,RI},
        {13,RR},
        {14,RI},
        {15,RR},
        {16,RI},
        {17,RR},
        {18,RI},
        {19,RR},
        {20,RI},
        {21,RR},
        {22,RI},
        {23,RI},
        {24,RR},
        {32,RR},
        {33,RR},
        {34,RR},
        {35,RR},
        {36,RR},
        {37,RR},
        {38,RI},
        {39,RI},
        {40,RR},
        {41,RM},
        {42,RI},
        {43,RR},
        {44,RI},
        {45,RR},
        {46,RM},
        {47,RM},
        {48,RM},
        {49,RM},
        {50,RM},
        {51,RM},
        {52,RM},
        {64,RM},
        {65,RM},
        {66,RM},
        {67,RM},
        {68,RR},
        {69,RR},
        {70,RR},
        {71,RR}
};
unsigned int Switcher( unsigned int cmd, unsigned int reg_dst, unsigned int reg_src, unsigned int modifier, unsigned int pointer )
// Возврат: -1 - Останов
// cmd - команды,
// reg_dst - номер регистра-приёмника
// reg_src - номер рестра-источника
// modifier - адрес или модификатор источника
{
    unsigned long long mul, div, Dst, Src;
    double double_dst, double_src;
    int num;
    char c;

    switch(cmd)
    {
        case HALT: // halt (Останов процессора)
            return -1;
        case SYSCALL: // syscall (Вызов операционной системы)
            switch(modifier)
            {
                case 0: // EXIT
                    exit(0);
                case 100: // SCANINT
                    cin >> Registers[reg_dst];
                    break;
                case 101: // SCANDOUBLE
                    cin >> double_dst;
                    Dst = *(unsigned long long *)&double_dst;
                    Registers[reg_dst] = static_cast<unsigned int>(Dst & 0xFFFFFFFF);
                    Registers[reg_dst + 1] = static_cast<unsigned int>(Dst >> 32);
                    break;
                case 102: // PRINTINT
                    num = static_cast<int>(Registers[reg_dst]);
                    printf("%d", num);
                    break;
                case 103: // PRINTDOUBLE
                    Dst = static_cast<unsigned long long>(Registers[reg_dst + 1]) << 32;
                    Src = Registers[reg_dst];
                    Dst += Src;
                    double_dst = *(double *)&Dst;
                    printf("%lg", double_dst);
                    break;
                case 104: // GETCHAR
                    scanf("%c", &c);
                    Registers[reg_dst] = static_cast<unsigned int>(c);
                    break;
                case 105: // PUTCHAR
                    c = static_cast<char>(Registers[reg_dst]);
                    printf("%c", c);
                    break;
                default:
                    break;
            }
            break;
        case ADD: // add (Сложение регистров. К R1 (регистру-первому аргументу) прибавляется содержимое R2
                  // (регистра-второго аргумента), модифицированное непосредственным операндом)
            Registers[reg_dst] += Registers[reg_src] + modifier;
            break;
        case ADDI: // addi (Сложение регистра с непосредственным операндом.
                  // К содержимому R1 прибавляется значение Imm (непосредственного операнда))
            Registers[reg_dst] += modifier;
            break;
        case SUB: // sub (Вычитание регистров. Из R1 вычитается содержимое R2)
            Registers[reg_dst] -= Registers[reg_src] + modifier;
            break;
        case SUBI: // subi (Вычитание из регистра непосредственного операнда)
            Registers[reg_dst] -= modifier;
            break;
        case MUL: // mul (Умножение регистров.
                // Содержимое R1 умножается на R2. Результат помещается в пару регистров, начинающуюся с R1)
                // reg_dst - младшие биты, reg_dst - старшие биты
            mul = Registers[reg_dst] * (static_cast<unsigned long long>(Registers[reg_src]) + modifier);
            Registers[reg_dst] = static_cast<unsigned int>(mul & 0xFFFFFFFF);
            Registers[reg_dst + 1] = static_cast<unsigned int>((mul >> 32) & 0xFFFFFFFF);
            break;
        case MULI: // muli (Умножение регистра R1 на Imm.
                // Результат помещается в пару регистров, начинающуюся с R1.
            mul = static_cast<unsigned long long>(Registers[reg_dst]) * modifier;
            Registers[reg_dst] = static_cast<unsigned int>(mul & 0xFFFFFFFF);
            Registers[reg_dst + 1] = static_cast<unsigned int>((mul >> 32) & 0xFFFFFFFF);
            break;
        case DIV: // div (Деление регистров
                // Младшие 32 бита первого операнда находятся в регистре R1, старшие — в регистре, номер которого на 1 больше R1.
                // Второй операнд находится в регистре. Частное помещается в регистр R1, остаток — в следующий регистр.
            div = (static_cast<unsigned long long>(Registers[reg_dst + 1]) << 32) + Registers[reg_dst];
            Registers[reg_dst] = static_cast<unsigned int>(div / Registers[reg_src]);
            Registers[reg_dst + 1] = static_cast<unsigned int>(div % Registers[reg_src]);
            break;
        case DIVI: // divi (Деление регистра на непосредственный операнд.
                // Аналогично div)
            div = (static_cast<unsigned long long>(Registers[reg_dst + 1]) << 32) + Registers[reg_dst];
            Registers[reg_dst] = static_cast<unsigned int>(div / modifier);
            Registers[reg_dst + 1] = static_cast<unsigned int>(div % modifier);
            break;
        case LC: // lc (Загрузка константы Imm в регистр R1.
                 // Для загрузки констант, больших 2^20 требуются дополнительные команды сдвига и логического сложения)
            Registers[reg_dst] = modifier;
            break;
        case SHL: // shl (Сдвинуть биты в регистре R1 влево на значение регистра R2)
            Registers[reg_dst] <<= Registers[reg_src];
            break;
        case SHLI: // shli (Сдвинуть биты в регистре R1 влево на Imm)
            Registers[reg_dst] <<= modifier;
            break;
        case SHR: // shr (Сдвинуть биты в регистре R1 вправо на значение регистра R2.
                 // Сдвижка на 32 или более разрядов обнуляет регистр R1)
            if (Registers[reg_src] >= 32) {
                Registers[reg_dst] = 0;
                break;
            }
            Registers[reg_dst] >>= Registers[reg_src];
            break;
        case SHRI: // shri (Сдвинуть биты в регистре R1 вправо на Imm)
            if (modifier >= 32) {
                Registers[reg_dst] = 0;
                break;
            }
            Registers[reg_dst] >>= modifier;
            break;
        case AND: // and (Логическое И регистров R1 и R2. Результат — в регистре R1)
            Registers[reg_dst] &= Registers[reg_src];
            break;
        case ANDI: // andi (Логическое И над регистром R1 и Imm)
            Registers[reg_dst] &= modifier;
            break;
        case OR: // or (Логическое ИЛИ регистров R1 и R2. Результат — в регистре R1)
            Registers[reg_dst] |= Registers[reg_src];
            break;
        case ORI: // ori (Логическое ИЛИ над регистром R1 и Imm)
            Registers[reg_dst] |= modifier;
            break;
        case XOR: // xor (Логическое исключающее ИЛИ регистров R1 и R2. Результат — в регистре R1)
            Registers[reg_dst] ^= Registers[reg_src];
            break;
        case XORI: // xori (Логическое исключающее ИЛИ над регистром R1 и Imm)
            Registers[reg_dst] ^= modifier;
            break;
        case NOT: // not (Поразрядное НЕ над всеми битами R1.
                 // Непосредственный операнд игнорируется, но он может присутствовать в команде)
            Registers[reg_dst] ^= 0xFFFFFFFF;
            break;
        case MOV: // mov (Пересылка из регистра R2 в регистр R1)
            Registers[reg_dst] = Registers[reg_src] + modifier;
            break;
        case ADDD: // addd (Вещественное сложение регистров R0 и R1. Вещественные числа занимают пару регистров, младшие
                 //из которых фигурируют в коде операции)
            Dst = (static_cast<unsigned long long>(Registers[reg_dst + 1]) << 32) + Registers[reg_dst];
            Src = (static_cast<unsigned long long>(Registers[reg_src + 1]) << 32) + Registers[reg_src];
            double_dst = *(double *)&Dst;
            double_src = *(double *)&Src;
            double_dst += double_src;
            Dst = *(unsigned long long *)&double_dst;
            Registers[reg_dst] = static_cast<unsigned int>(Dst & 0xFFFFFFFF);
            Registers[reg_dst + 1] = static_cast<unsigned int>((Dst >> 32) & 0xFFFFFFFF);
            break;
        case SUBD: // subd (Вещественное вычитание регистров R0 и R1)
            Dst = (static_cast<unsigned long long>(Registers[reg_dst + 1]) << 32) + Registers[reg_dst];
            Src = (static_cast<unsigned long long>(Registers[reg_src + 1]) << 32) + Registers[reg_src];
            double_dst = *(double *)&Dst;
            double_src = *(double *)&Src;
            double_dst -= double_src;
            Dst = *(unsigned long long *)&double_dst;
            Registers[reg_dst] = static_cast<unsigned int>(Dst & 0xFFFFFFFF);
            Registers[reg_dst + 1] = static_cast<unsigned int>((Dst >> 32) & 0xFFFFFFFF);
            break;
        case MULD: // muld (Вещественное умножение регистров R0 и R1)
            Dst = (static_cast<unsigned long long>(Registers[reg_dst + 1]) << 32) + Registers[reg_dst];
            Src = (static_cast<unsigned long long>(Registers[reg_src + 1]) << 32) + Registers[reg_src];
            double_dst = *(double *)&Dst;
            double_src = *(double *)&Src;
            double_dst *= double_src;
            Dst = *(unsigned long long *)&double_dst;
            Registers[reg_dst] = static_cast<unsigned int>(Dst & 0xFFFFFFFF);
            Registers[reg_dst + 1] = static_cast<unsigned int>((Dst >> 32) & 0xFFFFFFFF);
            break;
        case DIVD: // divd (Вещественное деление регистров R0 и R1)
            Dst = (static_cast<unsigned long long>(Registers[reg_dst + 1]) << 32) + Registers[reg_dst];
            Src = (static_cast<unsigned long long>(Registers[reg_src + 1]) << 32) + Registers[reg_src];
            double_dst = *(double *)&Dst;
            double_src = *(double *)&Src;
            double_dst /= double_src;
            Dst = *(unsigned long long *)&double_dst;
            Registers[reg_dst] = static_cast<unsigned int>(Dst & 0xFFFFFFFF);
            Registers[reg_dst + 1] = static_cast<unsigned int>((Dst >> 32) & 0xFFFFFFFF);
            break;
        case ITOD: // itod (Преобразование целого числа R2 в вещественное R1)
            double_dst = static_cast<double>(Registers[reg_src]);
            Dst = *(unsigned long long *)&double_dst;
            Registers[reg_dst] = static_cast<unsigned int>(Dst & 0xFFFFFFFF);
            Registers[reg_dst + 1] = static_cast<unsigned int>((Dst >> 32) & 0xFFFFFFFF);
            break;
        case DTOI: // dtoi (Преобразование вещественного числа R2 в целое R1
                 // Вещественное число, занимающее пару регистров преобразуется в целое отбрасыванием дробной части.
                 // Если число не помещается в регистр, возникает ошибка)
            Dst = (static_cast<unsigned long long>(Registers[reg_src + 1]) << 32) + Registers[reg_src];
            double_dst = *(double *)&Dst;
            if (double_dst >= pow(2.0, 32))
                return 1;
            Registers[reg_dst] = static_cast<unsigned int>(double_dst);
            break;
        case PUSH: // push (Отправить значение, находящееся в регистре R1 с добавлением к нему Imm в стек
                 // Указатель стека (r14) уменьшится на 1)
            Registers[14]--;
            Memory[Registers[14]] = Registers[reg_dst] + modifier;
            break;
        case POP: // pop (Извлечь из стека находящееся там число, поместить его в регистр R1
                 // и затем прибавить к регистру R1 значение Imm)
            Registers[reg_dst] = Memory[Registers[14]];
            Registers[reg_dst] += modifier;
            Memory[Registers[14]] = 0;
            Registers[14]++;
            break;
        case CALL: // call (Вызвать функцию, адрес которой расположен в R2+Imm.
                 // call r0 r5 0
                 // Вызвать функцию, адрес которой извлекается из регистра r5.
                 // При вызове функции в стек помещается адрес команды, следующей за текущей и управление передаётся по указанному адресу.
                 // Этот же адрес помещается и в регистр r0.)
            Registers[14]--;
            Memory[Registers[14]] = pointer + 1;
            pointer = Registers[reg_dst];
            Registers[reg_src] = Registers[reg_dst];
            return pointer;
        case CALLI: // calli (Вызвать функцию, адрес которой расположен в imm20.
                 // call r0 13323
                 // Вызвать функцию, начинающуюся с оператора по адресу 13323.
                 // При вызове функции в стек помещается адрес команды, следующей за текущей и управление передаётся по указанному адресу)
            Registers[14]--;
            Memory[Registers[14]] = pointer + 1;
            pointer = modifier;
            return pointer;
        case RET:
            // Возврат из функции.
            // ret r0 0
            // Вернуться из вызванной функции в вызвавшую. Из стека извлекается адрес, по которому передаётся
            // управление и который будет адресом следующего исполняемого слова.
            // Непосредственный аргумент показывает количество дополнительных слов, на которое требуется продвинуть указатель стека (оно
            // должно соответствовать количеству аргументов при вызове функции)
            pointer = Memory[Registers[14]];
            Registers[14] += 1 + modifier;
            return pointer;
        case CMP: // cmp (Сравнение. Результат записывается в регистр
                 // флагов, который используется в командах перехода)
            if (Registers[reg_dst] > Registers[reg_src])
                Flags[0] = Flags[1] = Flags[2] = Flags[3] = false;
            else if (Registers[reg_dst] < Registers[reg_src])
                Flags[0] = Flags[1] = Flags[2] = false, Flags[3] = true;
            else
                Flags[0] = Flags[2] = Flags[3] = false, Flags[1] = true;
            break;
        case CMPI: // cmpi (Сравнение с константой)
            if (Registers[reg_dst] > modifier)
                Flags[0] = Flags[1] = Flags[2] = Flags[3] = false;
            else if (Registers[reg_dst] < modifier)
                Flags[0] = Flags[1] = Flags[2] = false, Flags[3] = true;
            else
                Flags[0] = Flags[2] = Flags[3] = false, Flags[1] = true;
            break;
        case CMPD: // cmpd (Сравнение вещественных чисел)
            Dst = (static_cast<unsigned long>(Registers[reg_dst + 1]) << 32) + Registers[reg_dst];
            Src = (static_cast<unsigned long>(Registers[reg_src + 1]) << 32) + Registers[reg_src];
            double_dst = *(double *)&Dst;
            double_src = *(double *)&Src;
            if (double_dst > double_src)
                Flags[0] = Flags[1] = Flags[2] = Flags[3] = false;
            else if (double_dst < double_src)
                Flags[0] = Flags[1] = Flags[2] = false, Flags[3] = true;
            else
                Flags[0] = Flags[2] = Flags[3] = false, Flags[1] = true;
            break;
        case JMP: // jmp Безусловный переход.
                 // jmp r0 2212
                 // Следующая исполняемая команда будет находиться по адресу 2212. Регистр в команде игнорируется
            return modifier;
        case JNE: // Переход при наличии условия !=.
                 // jne 2212
                 // Если регистр флагов содержит условие !=, то следующая исполняемая команда будет находиться по
                 // адресу 2212 иначе ход исполнения программы не нарушается.
            if (Flags[1] == false)
                return modifier;
            break;
        case JEQ: // Переход при наличии условия ==.
                 // jeq 2212
                 // Если регистр флагов содержит условие ==, то следующая исполняемая команда будет находиться по
                 // адресу 2212 иначе ход исполнения программы не нарушается.
            if (Flags[1] == true)
                return modifier;
            break;
        case JLE: // Переход при наличии условия <=.
                 // jle 2212
                 // Если регистр флагов содержит условие <=, то следующая исполняемая команда будет находиться по
                 // адресу 2212 иначе ход исполнения программы не нарушается.
            if ((Flags[1] == true) || (Flags[3] == true))
                return modifier;
            break;
        case JL: // Переход при наличии условия <.
                 // jl 2212
                 // Если регистр флагов содержит условие <, то следующая исполняемая команда будет находиться по
                 // адресу 2212 иначе ход исполнения программы не нарушается.
            if ((Flags[1] == false) && (Flags[3] == true))
                return modifier;
            break;
        case JGE: // Переход при наличии условия >=.
                 // jge 2212
                 // Если регистр флагов содержит условие >=, то следующая исполняемая команда будет находиться по
                 // адресу 2212 иначе ход исполнения программы не нарушается.
            if ((Flags[1] == true) || (Flags[3] == false))
                return modifier;
            break;
        case JG: // Переход при наличии условия >.
                 // jg 2212
                 // Если регистр флагов содержит условие >, то следующая исполняемая команда будет находиться по
                 // адресу 2212 иначе ход исполнения программы не нарушается.
            if ((Flags[1] == false) && (Flags[3] == false))
                return modifier;
            break;
        case LOAD: // load (Загрузка из памяти в регистр)
            Registers[reg_dst] = Memory[modifier];
            break;
        case STORE: // store (Выгрузка из регистра в память)
            Memory[modifier] = Registers[reg_dst];
            break;
        case LOAD2: // load2 (Загрузка из памяти в пару регистров)
            Registers[reg_dst] = Memory[modifier];
            Registers[reg_dst + 1] = Memory[modifier + 1];
            break;
        case STORE2: // store2 (Выгрузка из пары регистров в память)
            Memory[modifier] = Registers[reg_dst];
            Memory[modifier + 1] = Registers[reg_dst + 1];
            break;
        case LOADR: // loadr (Загрузка из памяти в регистр)
            Registers[reg_dst] = Memory[Registers[reg_src] + modifier];
            break;
        case STORER: // storer (Выгрузка из регистра в память)
            Memory[Registers[reg_src] + modifier] = Registers[reg_dst];
            break;
        case LOADR2: // loadr2 (Загрузка из памяти в пару регистров)
            Registers[reg_dst] = Memory[Registers[reg_src] + modifier];
            Registers[reg_dst + 1] = Memory[Registers[reg_src] + modifier + 1];
            break;
        case STORER2: // storer2 (Выгрузка из пары регистров в память)
            Memory[Registers[reg_src] + modifier] = Registers[reg_dst];
            Memory[Registers[reg_src] + modifier + 1] = Registers[reg_dst + 1];
            break;
        default:
            break;
    }
    pointer++;
    return pointer;
}

unsigned int ParcerCmd( unsigned int pointer ) { // pointer - указатель на ячейку памяти, возвращает на следующую исполнимую строчку
    unsigned int cmd, reg_dst, reg_src, modifier;

    cmd =  Memory[pointer] >> 24;
    switch (cmd_info_mode.find(cmd)->second) {
        case 0: // RM (8 старших бит код команды, 4 следующих бита — код регистра (приёмника или источника),
            // 20 младших бит — адрес в памяти в виде беззнакового числа от 0 до 2^20 − 1)
            reg_dst = (Memory[pointer] >> 20) & 15;
            modifier = Memory[pointer] & 0xFFFFF;
            pointer = Switcher(cmd, reg_dst, 0, modifier, pointer);
            break;
        case 1: // RR (8 бит код команды, 4 бит код регистра-приёмника, 4 бит код регистра-источника,
            // 16 бит модификатор источника, число со знаком от −2^15 до 2^15 − 1)
            reg_dst = (Memory[pointer] >> 20) & 15;
            reg_src = (Memory[pointer] >> 16) & 15;
            modifier = Memory[pointer] & 0xFFFF;
            pointer = Switcher(cmd, reg_dst, reg_src, modifier, pointer);
            break;
        case 2: // RI (8 бит код команды, 4 бит код регистра-приёмника,
            // 20 бит непосредственный операнд, число со знаком от −2^19 до 2^19)
            reg_dst = (Memory[pointer] >> 20) & 15;
            modifier = Memory[pointer] & 0xFFFFF;
            pointer = Switcher(cmd, reg_dst, 0, modifier, pointer);
            break;
        default:
            break;
    }
    return pointer;
}
int main() {
    // Работа с файлом
    int length; // Длина считываемого файла

    setlocale(LC_ALL, "rus"); // Корректное отображение Кириллицы
    char *buff; // Буфер промежуточного хранения считываемого из файла текста
    ifstream fin("input.fasm");

    // Если файл не открылся
    if (!fin.is_open()) {
        cout << "Файл не может быть открыт!" << endl;
        return 0;
    }

    // Нахождение длины файла
    fin.seekg (0, ios::end);
    length = fin.tellg();
    fin.seekg (0, ios::beg);

    buff = new char [length];

    fin.read (buff,length);
    fin.close();

    string buffer = string(buff, sizeof(char) * length);
    delete[] buff;

    string new_buffer; // Новая строка без комментариев и лишних строк

    // Обработка, полученной строки
    unsigned int row_counter = 0, marker_flag = 0, word_flag = 0; // row_counter - счётчик строки
    int i = 0;
    string word; // Слово

    // Первый проход (Уборка комментариев и лишних строк)
    map <string, pair<unsigned int, unsigned int>>marker;
    map <string, pair<unsigned int, unsigned int>>::iterator itMarker;

    unsigned int markers_before = 0, markers_counter = 0;

    word.clear();
    while (buffer[i]) {
        switch (buffer[i]) {
            case ';': // Комментарий
                while (buffer[i] != '\n')
                    i++;
                word.clear();
                if ((word_flag == 1) || (marker_flag == 1))
                    word.push_back('o');
                word_flag = 0;
                marker_flag = 0;
                break;
            case ':': // Метка
                new_buffer.push_back(buffer[i]);
                marker_flag = 1;
                itMarker = marker.find(word);
                if (itMarker != marker.end()) // В мапе нашлось слово
                    cout << "Ошибка. Данное имя метки уже занято.";
                marker.insert(pair<string, pair<unsigned int, unsigned int>>(word, make_pair(row_counter, markers_before)));
                markers_before++;
                markers_counter++;
                word.clear();
                i++;
                break;
            case ' ': // Окончание слова
                if (!word.empty()) {
                    word.clear();
                    new_buffer.push_back(buffer[i]);
                }
                i++;
                break;
            case 9:
                i++;
                break;
            case '\n': // Перевод строки
                if (!word.empty() || marker_flag || word_flag) {
                    word.clear();
                    new_buffer.push_back(buffer[i]);
                    row_counter++;
                }
                word_flag = 0;
                marker_flag = 0;
                i++;
                break;
            default: // Считывание слова
                while (((buffer[i] >= 'a') && (buffer[i] <= 'z')) || ((buffer[i] >= '0') && (buffer[i] <= '9'))
                                                                  || ((buffer[i] >= 'A') && (buffer[i] <= 'Z'))) {
                    word.push_back(buffer[i]);
                    new_buffer.push_back(buffer[i]);
                    i++;
                }
                if (cmdinfo.find(word) != cmdinfo.end()) {// Комментарий находится на полезной строке
                    if (marker_flag == 1) {
                        new_buffer.insert(new_buffer.length() - word.length(), "\n");
                        row_counter++;
                    }
                    word_flag = 1;
                }
                break;
        }
    }
    // Второй проход (Замена меток на адреса строк)
    i = 0;
    while (new_buffer[i]) {
        switch (new_buffer[i]) {
            case ':': // Метка
                i++;
                break;
            case ' ': // Окончание слова
                if (!word.empty())
                    word.clear();
                i++;
                break;
            case '\n': // Перевод строки
                if (!word.empty())
                    word.clear();
                i++;
                break;
            default: // Считывание слова
                while ((new_buffer[i] != ' ') && (new_buffer[i] != ':') && (new_buffer[i] != '\n')) {
                    word.push_back(new_buffer[i]);
                    i++;
                }
                if (word == "end") {
                    word.clear();
                    i++;
                    while ((new_buffer[i] != ' ') && (new_buffer[i] != ':') && (new_buffer[i] != '\n')) {
                        word.push_back(new_buffer[i]);
                        i++;
                    }
                    new_buffer.replace(i - word.length(), word.length(), to_string(marker.find(word)->second.first - marker.find(word)->second.second));
                    break;
                }
                if ((marker.find(word) != marker.end()) && (new_buffer[i] != ':')) {
                    if (((new_buffer[i - word.length() - 3] == 'r') && ((new_buffer[i - word.length() - 2] >= '0'))) ||
                        ((new_buffer[i - word.length() - 4] == 'r') && ((new_buffer[i - word.length() - 3] >= '0'))))
                        new_buffer.replace(i - word.length(), word.length(), to_string(marker.find(word)->second.first - marker.find(word)->second.second));
                    else
                        new_buffer.replace(i - word.length(), word.length(), "r0 " + to_string(marker.find(word)->second.first - marker.find(word)->second.second));
                    word.clear();
                }
                break;
        }
    }
    /*i = 0;
    int j = 0;
    cout << j << ": ";
    j = 1;
    while (new_buffer[i]) {
        if (new_buffer[i] == '\n') {
            cout << new_buffer[i];
            cout << j << ": ";
            j++;
        } else
            cout << new_buffer[i];
        i++;
    }*/

    // Третий проход для занесения команд в память и в исполнимый файл
    // Поиск адреса, с которого должно начатсья исполнение
    decltype(new_buffer.cbegin()) iter;
    size_t position = 0; // position - указатель на начало директивы end

    string end{"end "};
    iter = search(new_buffer.cbegin(), new_buffer.cend(), end.cbegin(), end.cend());
    position = distance(new_buffer.cbegin(), iter);
    assert(position == new_buffer.find("end "));
    position += 3;
    while (new_buffer[position] == ' ')
        position++;

    word.clear();
    while (((new_buffer[position] >= 'a') && (new_buffer[position] <= 'z')) || ((new_buffer[position] >= '0') && (new_buffer[position] <= '9'))
           || ((new_buffer[position] >= 'A') && (new_buffer[position] <= 'Z'))) {
        word.push_back(new_buffer[position]);
        position++;
    }
    Registers[15] = position = stoi(word, nullptr, 10);

    // Запись в файл
    FILE *F;
    string reg_dst, reg_src, modifier;
    unsigned int word_to_file, word_to_file_1, word_to_file_2;
    int number;

    F = fopen("FUPM2OS", "w");
    if (F == NULL) {
        printf("File is not found\n");
        return 0;
    }

    // Заполнение первых 36 байтов исполнимого файла
    fprintf(F, "%s", "ThisIsFUPM2Exec"); // строка ASCII "ThisIsFUPM2Exec"
    fprintf(F, "%.8X", 0); // Размер кода программы
    fprintf(F, "%.8X", 0); // Размер констант программы
    fprintf(F, "%.8X", 0); // Размер данных программы
    fprintf(F, "%.8X", static_cast<unsigned int>(position)); // Начальный адресс исполнения программы
    fprintf(F, "%.8X", 0xFFFFF); // Начальный адресс стека
    i = 0;
    word.clear();
    unsigned  int memory_counter = 0;
    while (new_buffer[i]) {
        switch (new_buffer[i]) {
            case ' ': // Окончание слова
                // Пустое слово
                if (word.empty()) {
                    i++;
                    break;
                }
                // Если слово не является командой
                if (cmdinfo.find(word) == cmdinfo.end()) {
                    word.clear();
                    i++;
                    break;
                }
                if (cmdinfo.find(word)->second.first == 40) { // call
                    i++;
                    while (new_buffer[i] != ' ') {
                        reg_dst.push_back(new_buffer[i]);
                        i++;
                    }
                    i++;
                    while ((new_buffer[i] != ' ') && (new_buffer[i] != '\n')) {
                        reg_src.push_back(new_buffer[i]);
                        i++;
                    }
                    i++;
                    modifier = string("0");
                    //cout << "Word: " << word << " " << "Reg_dst: " << reg_dst << " " << "Reg_src: " << reg_src << " " << "Modifier: " << modifier << endl;
                    word_to_file = static_cast<unsigned int>(cmdinfo.find(word)->second.first);
                    word_to_file <<= 24;
                    word_to_file_1 = static_cast<unsigned int>(reg_info.find(reg_dst)->second);
                    word_to_file_1 <<= 20;
                    word_to_file_2 = static_cast<unsigned int>(reg_info.find(reg_src)->second);
                    word_to_file_2 <<= 16;
                    word_to_file += word_to_file_1;
                    word_to_file += word_to_file_2;
                    number = stoi(modifier, nullptr, 10);
                    if (number < 0)
                        number ^= ((1 << 31) + (1 << 15));
                    word_to_file += static_cast<unsigned int>(number);
                    fprintf(F, "%.8X", word_to_file);
                    Memory[memory_counter] = word_to_file;
                    reg_dst.clear();
                    reg_src.clear();
                    modifier.clear();
                    word.clear();
                    memory_counter++;
                    break;
                }
                if (cmdinfo.find(word)->second.first == 42) { // ret
                    i++;
                    while ((new_buffer[i] != '\n') && (new_buffer[i] != ' ')) {
                        modifier.push_back(new_buffer[i]);
                        i++;
                    }
                    //cout << "Word: " << word << " " << "Modifier: " << modifier << endl;
                    word_to_file = static_cast<unsigned int>(cmdinfo.find(word)->second.first);
                    word_to_file <<= 24;
                    word_to_file += stoi(modifier, nullptr, 10);
                    fprintf(F, "%.8X", word_to_file);
                    Memory[memory_counter] = word_to_file;
                    modifier.clear();
                    word.clear();
                    memory_counter++;
                    i++;
                    break;
                }
                switch (cmdinfo.find(word)->second.second) {
                    case 0: // RM (8 старших бит код команды, 4 следующих бита — код регистра (приёмника или источника),
                        // 20 младших бит — адрес в памяти в виде беззнакового числа от 0 до 2^20 − 1)
                        i++;
                        while (new_buffer[i] != ' ') {
                            reg_dst.push_back(new_buffer[i]);
                            i++;
                        }
                        i++;
                        while (new_buffer[i] != '\n') {
                            modifier.push_back(new_buffer[i]);
                            i++;
                        }
                        //cout << "Word: " << word << " " << "Reg_dst: " << reg_dst << " " << "Modifier: " << modifier << endl;
                        word_to_file = static_cast<unsigned int>(cmdinfo.find(word)->second.first);
                        word_to_file <<= 24;
                        word_to_file_1 = static_cast<unsigned int>(reg_info.find(reg_dst)->second);
                        word_to_file_1 <<= 20;
                        word_to_file += word_to_file_1;
                        word_to_file += static_cast<unsigned int>(stoi (modifier,nullptr,10));
                        fprintf(F, "%.8X", word_to_file);
                        Memory[memory_counter] = word_to_file;
                        reg_dst.clear();
                        modifier.clear();
                        word.clear();
                        break;
                    case 1: // RR (8 бит код команды, 4 бит код регистра-приёмника, 4 бит код регистра-источника,
                        // 16 бит модификатор источника, число со знаком от −2^15 до 2^15 − 1)
                        i++;
                        while (new_buffer[i] != ' ') {
                            reg_dst.push_back(new_buffer[i]);
                            i++;
                        }
                        i++;
                        while (new_buffer[i] != ' ') {
                            reg_src.push_back(new_buffer[i]);
                            i++;
                        }
                        i++;
                        while (new_buffer[i] != '\n') {
                            modifier.push_back(new_buffer[i]);
                            i++;
                        }
                        //cout << "Word: " << word << " " << "Reg_dst: " << reg_dst << " " << "Reg_src: " << reg_src << " " << "Modifier: " << modifier << endl;
                        word_to_file = static_cast<unsigned int>(cmdinfo.find(word)->second.first);
                        word_to_file <<= 24;
                        word_to_file_1 = static_cast<unsigned int>(reg_info.find(reg_dst)->second);
                        word_to_file_1 <<= 20;
                        word_to_file_2 = static_cast<unsigned int>(reg_info.find(reg_src)->second);
                        word_to_file_2 <<= 16;
                        word_to_file += word_to_file_1;
                        word_to_file += word_to_file_2;
                        number = stoi (modifier,nullptr,10);
                        if (number < 0)
                            number ^= ((1 << 31) + (1 << 15));
                        word_to_file += static_cast<unsigned int>(number);
                        fprintf(F, "%.8X", word_to_file);
                        Memory[memory_counter] = word_to_file;
                        reg_dst.clear();
                        reg_src.clear();
                        modifier.clear();
                        word.clear();
                        break;
                    case 2: // RI (8 бит код команды, 4 бит код регистра-приёмника,
                        // 20 бит непосредственный операнд, число со знаком от −2^19 до 2^19)
                        i++;
                        while (new_buffer[i] != ' ') {
                            reg_dst.push_back(new_buffer[i]);
                            i++;
                        }
                        i++;
                        while ((new_buffer[i] != ' ') && (new_buffer[i] != '\n')) {
                            modifier.push_back(new_buffer[i]);
                            i++;
                        }
                        //cout << "Word: " << word << " " << "Reg_dst: " << reg_dst << " " << "Modifier: " << modifier << endl;
                        word_to_file = static_cast<unsigned int>(cmdinfo.find(word)->second.first);
                        word_to_file <<= 24;
                        word_to_file_1 = static_cast<unsigned int>(reg_info.find(reg_dst)->second);
                        word_to_file_1 <<= 20;
                        word_to_file += word_to_file_1;
                        word_to_file += static_cast<unsigned int>(stoi (modifier,nullptr,10));
                        fprintf(F, "%.8X", word_to_file);
                        Memory[memory_counter] = word_to_file;
                        reg_dst.clear();
                        modifier.clear();
                        word.clear();
                        break;
                    default:
                        break;
                }
                memory_counter++;
                i++;
                break;
            case '\n': // Перевод строки
                if (!word.empty()) {
                    word.clear();
                    memory_counter++;
                }
                i++;
                break;
            case ':':
                word.clear();
                i++;
                break;
            default: // Считывание слова
                while ((new_buffer[i] != ' ') && (new_buffer[i] != ':') && (new_buffer[i] != '\n')) {
                    word.push_back(new_buffer[i]);
                    i++;
                }
                break;
        }
    }
    fclose(F);

    // Иcполнение в эмуляторе
    i = Registers[15];

    Registers[14] = 0xFFFFF;
    while (i < row_counter - markers_counter) { // != 11) {//
        i = ParcerCmd(i);
        if (i == -1)
            return 1;
    }
    return 0;
}