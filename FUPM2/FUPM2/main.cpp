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
int Switcher( unsigned int cmd, unsigned int reg_dst, unsigned int reg_src, unsigned modifier ) // 1 - Останов, 0 - все хорошо
// cmd - команды,
// reg_dst - номер регистра-приёмника
// reg_src - номер рестра-источника
// modifier - адрес или модификатор источника
{
    unsigned long long mul, div, Dst, Src;
    double double_dst, double_src;

    switch(cmd)
    {
        case 0: // halt (Останов процессора)
            return 1;
        case 1: // syscall (Вызов операционной системы)
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
                    cout << Registers[reg_dst];
                    break;
                case 103: // PRINTDOUBLE
                    cout << Registers[reg_dst + 1];
                    cout << Registers[reg_dst];
                    break;
                case 104: // GETCHAR
                    cin >> Registers[reg_dst];
                    break;
                case 105: // PUTCHAR
                    cout << Registers[reg_dst];
                    break;
                default:
                    break;
            }
            break;
        case 2: // add (Сложение регистров. К R1 (регистру-первому аргументу) прибавляется содержимое R2
                  // (регистра-второго аргумента), модифицированное непосредственным операндом)
            Registers[reg_dst] += Registers[reg_src] + modifier;
            break;
        case 3: // addi (Сложение регистра с непосредственным операндом.
                  // К содержимому R1 прибавляется значение Imm (непосредственного операнда))
            Registers[reg_dst] += modifier;
            break;
        case 4: // sub (Вычитание регистров. Из R1 вычитается содержимое R2)
            Registers[reg_dst] -= Registers[reg_src] + modifier;
            break;
        case 5: // subi (Вычитание из регистра непосредственного операнда)
            Registers[reg_dst] -= modifier;
            break;
        case 6: // mul (Умножение регистров.
                // Содержимое R1 умножается на R2. Результат помещается в пару регистров, начинающуюся с R1)
                // reg_dst - младшие биты, reg_dst - старшие биты
            mul = Registers[reg_dst] * (static_cast<unsigned long long>(Registers[reg_src]) + modifier);
            Registers[reg_dst] = static_cast<unsigned int>(mul & 0xFFFFFFFF);
            Registers[reg_dst + 1] = static_cast<unsigned int>((mul >> 32) & 0xFFFFFFFF);
            break;
        case 7: // muli (Умножение регистра R1 на Imm.
                // Результат помещается в пару регистров, начинающуюся с R1.
            mul = static_cast<unsigned long long>(Registers[reg_dst]) * modifier;
            Registers[reg_dst] = static_cast<unsigned int>(mul & 0xFFFFFFFF);
            Registers[reg_dst + 1] = static_cast<unsigned int>((mul >> 32) & 0xFFFFFFFF);
            break;
        case 8: // div (Деление регистров
                // Младшие 32 бита первого операнда находятся в регистре R1, старшие — в регистре, номер которого на 1 больше R1.
                // Второй операнд находится в регистре. Частное помещается в регистр R1, остаток — в следующий регистр.
            div = (static_cast<unsigned long long>(Registers[reg_dst + 1]) << 32) + Registers[reg_dst];
            Registers[reg_dst] = static_cast<unsigned int>(div / Registers[reg_src]);
            Registers[reg_dst + 1] = static_cast<unsigned int>(div % Registers[reg_src]);
            break;
        case 9: // divi (Деление регистра на непосредственный операнд.
                // Аналогично div)
            div = (static_cast<unsigned long long>(Registers[reg_dst + 1]) << 32) + Registers[reg_dst];
            Registers[reg_dst] = static_cast<unsigned int>(div / modifier);
            Registers[reg_dst + 1] = static_cast<unsigned int>(div % modifier);
            break;
        case 12: // lc (Загрузка константы Imm в регистр R1.
                 // Для загрузки констант, больших 2^20 требуются дополнительные команды сдвига и логического сложения)
            Registers[reg_dst] = modifier;
            break;
        case 13: // shl (Сдвинуть биты в регистре R1 влево на значение регистра R2)
            Registers[reg_dst] <<= Registers[reg_src];
            break;
        case 14: // shli (Сдвинуть биты в регистре R1 влево на Imm)
            Registers[reg_dst] <<= modifier;
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
            if (modifier >= 32) {
                Registers[reg_dst] = 0;
                break;
            }
            Registers[reg_dst] >>= modifier;
            break;
        case 17: // and (Логическое И регистров R1 и R2. Результат — в регистре R1)
            Registers[reg_dst] &= Registers[reg_src];
            break;
        case 18: // andi (Логическое И над регистром R1 и Imm)
            Registers[reg_dst] &= modifier;
            break;
        case 19: // or (Логическое ИЛИ регистров R1 и R2. Результат — в регистре R1)
            Registers[reg_dst] |= Registers[reg_src];
            break;
        case 20: // ori (Логическое ИЛИ над регистром R1 и Imm)
            Registers[reg_dst] |= modifier;
            break;
        case 21: // xor (Логическое исключающее ИЛИ регистров R1 и R2. Результат — в регистре R1)
            Registers[reg_dst] ^= Registers[reg_src];
            break;
        case 22: // xori (Логическое исключающее ИЛИ над регистром R1 и Imm)
            Registers[reg_dst] ^= modifier;
            break;
        case 23: // not (Поразрядное НЕ над всеми битами R1.
                 // Непосредственный операнд игнорируется, но он может присутствовать в команде)
            Registers[reg_dst] ^= 0xFFFFFFFF;
            break;
        case 24: // mov (Пересылка из регистра R2 в регистр R1)
            Registers[reg_dst] = Registers[reg_src] + modifier;
            break;
        case 32: // addd (Вещественное сложение регистров R0 и R1. Вещественные числа занимают пару регистров, младшие
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
        case 33: // subd (Вещественное вычитание регистров R0 и R1)
            Dst = (static_cast<unsigned long long>(Registers[reg_dst + 1]) << 32) + Registers[reg_dst];
            Src = (static_cast<unsigned long long>(Registers[reg_src + 1]) << 32) + Registers[reg_src];
            double_dst = *(double *)&Dst;
            double_src = *(double *)&Src;
            double_dst -= double_src;
            Dst = *(unsigned long long *)&double_dst;
            Registers[reg_dst] = static_cast<unsigned int>(Dst & 0xFFFFFFFF);
            Registers[reg_dst + 1] = static_cast<unsigned int>((Dst >> 32) & 0xFFFFFFFF);
            break;
        case 34: // muld (Вещественное умножение регистров R0 и R1)
            Dst = (static_cast<unsigned long long>(Registers[reg_dst + 1]) << 32) + Registers[reg_dst];
            Src = (static_cast<unsigned long long>(Registers[reg_src + 1]) << 32) + Registers[reg_src];
            double_dst = *(double *)&Dst;
            double_src = *(double *)&Src;
            double_dst *= double_src;
            Dst = *(unsigned long long *)&double_dst;
            Registers[reg_dst] = static_cast<unsigned int>(Dst & 0xFFFFFFFF);
            Registers[reg_dst + 1] = static_cast<unsigned int>((Dst >> 32) & 0xFFFFFFFF);
            break;
        case 35: // divd (Вещественное деление регистров R0 и R1)
            Dst = (static_cast<unsigned long long>(Registers[reg_dst + 1]) << 32) + Registers[reg_dst];
            Src = (static_cast<unsigned long long>(Registers[reg_src + 1]) << 32) + Registers[reg_src];
            double_dst = *(double *)&Dst;
            double_src = *(double *)&Src;
            double_dst /= double_src;
            Dst = *(unsigned long long *)&double_dst;
            Registers[reg_dst] = static_cast<unsigned int>(Dst & 0xFFFFFFFF);
            Registers[reg_dst + 1] = static_cast<unsigned int>((Dst >> 32) & 0xFFFFFFFF);
            break;
        case 36: // itod (Преобразование целого числа R2 в вещественное R1)
            double_dst = static_cast<double>(Registers[reg_src]);
            Dst = *(unsigned long long *)&double_dst;
            Registers[reg_dst] = static_cast<unsigned int>(Dst & 0xFFFFFFFF);
            Registers[reg_dst + 1] = static_cast<unsigned int>((Dst >> 32) & 0xFFFFFFFF);
            break;
        case 37: // dtoi (Преобразование вещественного числа R2 в целое R1
                 // Вещественное число, занимающее пару регистров преобразуется в целое отбрасыванием дробной части.
                 // Если число не помещается в регистр, возникает ошибка)
            Dst = (static_cast<unsigned long long>(Registers[reg_src + 1]) << 32) + Registers[reg_src];
            double_dst = *(double *)&Dst;
            if (double_dst >= pow(2.0, 32))
                return 1;
            Registers[reg_dst] = static_cast<unsigned int>(double_dst);
            break;
        case 38: // push (Отправить значение, находящееся в регистре R1 с добавлением к нему Imm в стек
                 // Указатель стека (r14) уменьшится на 1)
            Memory[Registers[14]] = Registers[reg_dst] + modifier;
            Registers[14]--;
            break;
        case 39: // pop (Извлечь из стека находящееся там число, поместить его в регистр R1
                 // и затем прибавить к регистру R1 значение Imm)
            Registers[reg_dst] = Memory[Registers[14]];
            Registers[reg_dst] += modifier;
            Registers[14]++;
            break;
        // Допилить 40-42
        case 43: // cmp (Сравнение. Результат записывается в регистр
                 // флагов, который используется в командах перехода)
            if (Registers[reg_dst] > Registers[reg_src])
                Flags[0] = Flags[1] = Flags[2] = Flags[3] = false;
            else if (Registers[reg_dst] < Registers[reg_src])
                Flags[0] = Flags[1] = Flags[2] = false, Flags[3] = true;
            else
                Flags[0] = Flags[2] = Flags[3] = false, Flags[1] = true;
            break;
        case 44: // cmpi (Сравнение с константой)
            if (Registers[reg_dst] > modifier)
                Flags[0] = Flags[1] = Flags[2] = Flags[3] = false;
            else if (Registers[reg_dst] < modifier)
                Flags[0] = Flags[1] = Flags[2] = false, Flags[3] = true;
            else
                Flags[0] = Flags[2] = Flags[3] = false, Flags[1] = true;
            break;
        case 45: // cmpd (Сравнение вещественных чисел)
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
        // Допилить 46-52
        case 64: // load (Загрузка из памяти в регистр)
            Registers[reg_dst] = Memory[modifier];
            break;
        case 65: // store (Выгрузка из регистра в память)
            Memory[modifier] = Registers[reg_dst];
            break;
        case 66: // load2 (Загрузка из памяти в пару регистров)
            Registers[reg_dst] = Memory[modifier];
            Registers[reg_dst + 1] = Memory[modifier + 1];
            break;
        case 67: // store2 (Выгрузка из пары регистров в память)
            Memory[modifier] = Registers[reg_dst];
            Memory[modifier + 1] = Registers[reg_dst + 1];
            break;
        case 68: // loadr (Загрузка из памяти в регистр)
            Registers[reg_dst] = Memory[Registers[reg_src] + modifier];
            break;
        case 69: // storer (Выгрузка из регистра в память)
            Memory[Registers[reg_src] + modifier] = Registers[reg_dst];
            break;
        case 70: // loadr2 (Загрузка из памяти в пару регистров)
            Registers[reg_dst] = Memory[Registers[reg_src] + modifier];
            Registers[reg_dst + 1] = Memory[Registers[reg_src] + modifier + 1];
            break;
        case 71: // storer2 (Выгрузка из пары регистров в память)
            Memory[Registers[reg_src] + modifier] = Registers[reg_dst];
            Memory[Registers[reg_src] + modifier + 1] = Registers[reg_dst + 1];
            break;
        default:
            break;
    }
    return 0;
}
void ParcerCmd( int pointer ) { // pointer - указатель на ячейку памяти
    unsigned int cmd, reg_dst, reg_src, modifier;

    cmd =  Memory[pointer] >> 24;
    if (cmd_info_mode.find(cmd) == cmd_info_mode.end())
        return;
    switch (cmd_info_mode.find(cmd)->second) {
        case 0: // RM (8 старших бит код команды, 4 следующих бита — код регистра (приёмника или источника),
            // 20 младших бит — адрес в памяти в виде беззнакового числа от 0 до 2^20 − 1)
            reg_dst = (Memory[pointer] >> 20) & 15;
            modifier = Memory[pointer] & 0xFFFFF;
            Switcher(cmd, reg_dst, 0, modifier);
            break;
        case 1: // RR (8 бит код команды, 4 бит код регистра-приёмника, 4 бит код регистра-источника,
            // 16 бит модификатор источника, число со знаком от −2^15 до 2^15 − 1)
            reg_dst = (Memory[pointer] >> 20) & 15;
            reg_src = (Memory[pointer] >> 16) & 15;
            modifier = Memory[pointer] & 0xFFFF;
            Switcher(cmd, reg_dst, reg_src, modifier);
            break;
        case 2: // RI (8 бит код команды, 4 бит код регистра-приёмника,
            // 20 бит непосредственный операнд, число со знаком от −2^19 до 2^19)
            reg_dst = (Memory[pointer] >> 20) & 15;
            modifier = Memory[pointer] & 0xFFFFF;
            Switcher(cmd, reg_dst, 0, modifier);
            break;
        default:
            break;
    }
    return;
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
    int i = 0, row_counter = 0, comment_flag = 0, marker_flag = 0; // row_counter - счётчик строки
    string word; // Слово
    map <string, int> marker; // Метки (значение указывает на строку начала метки)
    map <string, int>::iterator itMarker;


    // Первый проход (Уборка комментариев и лишних строк)
    while (buffer[i]) {
        switch (buffer[i]) {
            case ';': // Комментарий
                while (buffer[i] != '\n')
                    i++;
                word.clear();
                if ((comment_flag == 1) || (marker_flag == 1))
                    word.push_back('o');
                comment_flag = 0;
                marker_flag = 0;
                //new_buffer.push_back(buffer[i]);
                //i++;
                break;
            case ':': // Метка
                new_buffer.push_back(buffer[i]);
                /*itMarker = marker.find(word);
                if (itMarker != marker.end()) // В мапе нашлось слово
                    cout << "Ошибка. Данное имя метки уже занято.";
                marker.insert(pair<string,int>(word, row_counter));*/
                marker_flag = 1;
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
            case '\n': // Перевод строки
                if (!word.empty()) {
                    word.clear();
                    new_buffer.push_back(buffer[i]);
                }
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
                    if (marker_flag == 1)
                        new_buffer.insert(new_buffer.length()-word.length(), "\n");
                    comment_flag = 1;
                }
                break;
        }
    }

    // Второй проход для определения строк меток
    i = 0;
    while (new_buffer[i]) {
        switch (new_buffer[i]) {
            case ':': // Метка
                itMarker = marker.find(word);
                if (itMarker != marker.end()) // В мапе нашлось слово
                    cout << "Ошибка. Данное имя метки уже занято.";
                marker.insert(pair<string,int>(word, row_counter));
                word.clear();
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
                row_counter++;
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

    for (i = 0; new_buffer[i] != 0; i++)
        cout << new_buffer[i];
    cout << endl;
    for (auto it = marker.begin(); it != marker.end(); it++)
        cout << it->first << ": " << it->second << endl;
    return 0;
}
/*
    int i = 0, counter = 0, marker_begin, marker_end; // counter - счётчик строки, указатель на начало и конец метки
    map <string, pair<int, int>> marker; // Метки (первое значение указывает на начало метки, второе - на конец)
    string word, prevmarker; // word - слово, prevmarker - название предыдущего маркера
    int flag = 0;
    map <string, pair<int, int>>::iterator itMarker;

    // Первый проход
    while (buffer[i])
    {
        switch (buffer[i]) {
            case ';': // Комментарий
                while (buffer[i] != '\n')
                    i++;
                break;
            case ':': // Метка
                marker_begin = i; // i  сейчас указывает на ":"
                //while ((buffer[marker_begin] > '9'))
                //    marker_begin++;
                marker_begin++;
                if (flag < 1) {// Первый заход
                    prevmarker = word;
                    flag++;
                    break;
                }
                // Поиск предыдущего конца метки
                while (buffer[i] != '\n')
                    i--;
                i--;
                marker_end = i;
                itMarker = marker.find(prevmarker);
                if (itMarker != marker.end()) // В мапе нашлось слово
                    itMarker->second.second = marker_end;
                marker.insert(pair<string,pair<int, int>>(word,make_pair(marker_begin, -1)));
                prevmarker = word;
                word.clear();
                i = marker_begin;
                break;
            case ' ': // Окончание слова
                word.clear();
                i++;
                break;
            case '\n': // Перевод строки
                word.clear();
                counter++;
                i++;
                break;
            default: // Считывание слова
                while ((buffer[i] != ' ') && (buffer[i] != ':') && (buffer[i] != '\n') && (buffer[i] != ';')) {
                    //while (((buffer[i] >= 'a') && (buffer[i] <= 'z')) || ((buffer[i] >= '0') && (buffer[i] <= '9'))
                    //                                                  || ((buffer[i] >= 'A') && (buffer[i] <= 'Z'))) {
                    word.push_back(buffer[i]);
                    i++;
                }
                break;
        }
    }
    itMarker = marker.find(prevmarker);
    if (itMarker != marker.end())
        itMarker->second.second = i - 2;
    i = 0;
    // Конец первого прохода. Теперь в мапе marker лежат значения начала и конца метки.

    // Поиск адреса, с которого должно начатсья исполнение
    decltype(buffer.cbegin()) iter;
    size_t position = 0, end_position; // position - указатель на начало директивы end, end_position - конечный символ до которого надо исполнять

    string end{"end"};
    iter = search(buffer.cbegin(), buffer.cend(), end.cbegin(), end.cend());
    position = distance(buffer.cbegin(), iter);
    assert(position == buffer.find("end"));
    position += 4;
    end_position = position;

    word.clear();
    while (((buffer[position] >= 'a') && (buffer[position] <= 'z')) || ((buffer[position] >= 'A') && (buffer[position] <= 'Z'))) {
        word.push_back(buffer[position]);
        position++;
    }

    itMarker = marker.find(word);
    if (itMarker != marker.end())
        i = itMarker->second.first;
    else
        cout << "Программа не может быть исполнена, так как в ней нет директивы end" << endl;

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
    word.clear();
    while (i <= end_position) {

    }
    // Передвижение в пределах одной метки
    while (i <= itMarker->second.second) {
        switch (buffer[i]) {
            case ';': // Комментарий
                while (buffer[i] != '\n')
                    i++;
                break;
            case ' ': // Окончание слова
                // Если слово не является командой
                if (cmdinfo.find(word) == cmdinfo.end()) {
                    word.clear();
                    i++;
                    break;
                }
                switch (cmdinfo.find(word)->second.second) {
                    case 0: // RM (8 старших бит код команды, 4 следующих бита — код регистра (приёмника или источника),
                        // 20 младших бит — адрес в памяти в виде беззнакового числа от 0 до 2^20 − 1)
                        i++;
                        while (buffer[i] != ' ') {
                            reg_dst.push_back(buffer[i]);
                            i++;
                        }
                        i++;
                        while ((buffer[i] != ' ') && (buffer[i] != '\n')) {
                            modifier.push_back(buffer[i]);
                            i++;
                        }
                        word_to_file = static_cast<unsigned int>(cmdinfo.find(word)->second.first);
                        word_to_file <<= 24;
                        word_to_file_1 = static_cast<unsigned int>(reg_info.find(reg_dst)->second);
                        word_to_file_1 <<= 20;
                        word_to_file += word_to_file_1;
                        word_to_file += static_cast<unsigned int>(stoul (modifier,nullptr,10));
                        fprintf(F, "%.8X", word_to_file);
                        reg_dst.clear();
                        modifier.clear();
                        word.clear();
                        break;
                    case 1: // RR (8 бит код команды, 4 бит код регистра-приёмника, 4 бит код регистра-источника,
                        // 16 бит модификатор источника, число со знаком от −2^15 до 2^15 − 1)
                        i++;
                        while (buffer[i] != ' ') {
                            reg_dst.push_back(buffer[i]);
                            i++;
                        }
                        i++;
                        while (buffer[i] != ' ') {
                            reg_src.push_back(buffer[i]);
                            i++;
                        }
                        i++;
                        while ((buffer[i] != ' ') && (buffer[i] != '\n')) {
                            modifier.push_back(buffer[i]);
                            i++;
                        }
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
                        reg_dst.clear();
                        reg_src.clear();
                        modifier.clear();
                        word.clear();
                        break;
                    case 2: // RI (8 бит код команды, 4 бит код регистра-приёмника,
                        // 20 бит непосредственный операнд, число со знаком от −2^19 до 2^19)
                        i++;
                        while (buffer[i] != ' ') {
                            reg_dst.push_back(buffer[i]);
                            i++;
                        }
                        i++;
                        while ((buffer[i] != ' ') && (buffer[i] != '\n')) {
                            modifier.push_back(buffer[i]);
                            i++;
                        }
                        word_to_file = static_cast<unsigned int>(cmdinfo.find(word)->second.first);
                        word_to_file <<= 24;
                        word_to_file_1 = static_cast<unsigned int>(reg_info.find(reg_dst)->second);
                        word_to_file_1 <<= 20;
                        word_to_file += word_to_file_1;
                        word_to_file += static_cast<unsigned int>(stoi (modifier,nullptr,10));
                        fprintf(F, "%.8X", word_to_file);
                        reg_dst.clear();
                        modifier.clear();
                        word.clear();
                        break;
                    default:
                        break;
                }
                i++;
                break;
            case '\n': // Перевод строки
                word.clear();
                i++;
                break;
            default: // Считывание слова
                //while ((buffer[i] != ' ') && (buffer[i] != ':') && (buffer[i] != '\n') && (buffer[i] != ';')) {
                    while (((buffer[i] >= 'a') && (buffer[i] <= 'z')) || ((buffer[i] >= '0') && (buffer[i] <= '9'))
                                                                      || ((buffer[i] >= 'A') && (buffer[i] <= 'Z'))) {
                    word.push_back(buffer[i]);
                    i++;
                }
                break;
        }
    }
    fclose(F);

// Считывание из файла
F = fopen("FUPM2OS", "r");
if (F == NULL) {
    printf("File is not found\n");
    return 0;
}

fclose(F);

void ParcerCmd( unsigned int *Registers, unsigned int *Memory, bool *Flags, map <string, pair<code , mode>>* cmdinfo, map <string, int>* reg_info )
{
    string cmd, reg_dst, reg_src;
    long modifier;

    while (true) {
        cin >> cmd;
        if ((*cmdinfo).find(cmd) == (*cmdinfo).end())
            break;
        switch ((*cmdinfo).find(cmd)->second.second) {
            case 0: // RM (8 старших бит код команды, 4 следующих бита — код регистра (приёмника или источника),
                // 20 младших бит — адрес в памяти в виде беззнакового числа от 0 до 2^20 − 1)
                cin >> reg_dst;
                cin >> modifier;
                Switcher((*cmdinfo).find(cmd)->second.first, (*reg_info).find(reg_dst)->second, 0, modifier, Registers, Memory, Flags);
                break;
            case 1: // RR (8 бит код команды, 4 бит код регистра-приёмника, 4 бит код регистра-источника,
                // 16 бит модификатор источника, число со знаком от −2^15 до 2^15 − 1)
                cin >> reg_dst;
                cin >> reg_src;
                cin >> modifier;
                Switcher((*cmdinfo).find(cmd)->second.first, (*reg_info).find(reg_dst)->second,
                         (*reg_info).find(reg_src)->second, modifier, Registers, Memory, Flags);
                break;
            case 2: // RI (8 бит код команды, 4 бит код регистра-приёмника,
                // 20 бит непосредственный операнд, число со знаком от −2^19 до 2^19)
                cin >> reg_dst;
                cin >> modifier;
                Switcher((*cmdinfo).find(cmd)->second.first, (*reg_info).find(reg_dst)->second, 0, modifier, Registers, Memory, Flags);
                break;
            default:
                break;
        }
    }
    //cout << cmdinfo.find(cmd)->first << " " << cmdinfo.find(cmd)->second.first << " " << cmdinfo.find(cmd)->second.second;
} */