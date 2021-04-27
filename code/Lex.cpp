//=====================================================================//
// Contents:                                                           //
//                                                                     //
//...General Classes & Some Methods...............................16...//
//...TABLES......................................................216...//
//...get_lex Method..............................................271...//
//...Methods For The Scanner Testing.............................430...//
//=====================================================================//

#include <iostream>
#include <iomanip>

#define lgt 100

//=====================================================================//
//================================ GENERAL CLASSES & SOME METHODS =====//
//=====================================================================//

enum type_of_lex {
    LEX_NULL,      LEX_AND,      LEX_BEGIN,   LEX_BOOLEAN,    // 0
    LEX_DO,        LEX_ELSE,     LEX_END,     LEX_IF,         // 4
    LEX_FALSE,     LEX_INT,      LEX_NOT,     LEX_OR,         // 8
    LEX_PROGRAM,   LEX_READ,     LEX_TRUE,    LEX_CASE,       // 12
    LEX_WHILE,     LEX_WRITE,    LEX_OF,      LEX_FOR,        // 16
    LEX_STRING,    LEX_STEP,     LEX_UNTIL,   LEX_GOTO,       // 20
    LEX_CONTINUE,  LEX_BREAK,    LEX_STRUCT,                  // 24
    
    LEX_SEMICOLON, LEX_COMMA,    LEX_COLON,   LEX_ASSIGN,     // 27
    LEX_LPAREN,    LEX_RPAREN,   LEX_EQ,      LEX_LSS,        // 31
    LEX_GTR,       LEX_PLUS,     LEX_MINUS,   LEX_TIMES,      // 35
    LEX_SLASH,     LEX_LEQ,      LEX_NEQ,     LEX_GEQ,        // 39
    LEX_PCNT,      LEX_LFIG,     LEX_RFIG,                    // 43
    
    LEX_ID,        LEX_NUM,      LEX_STR,     POLIZ_LABEL,    // 46
    POLIZ_ADDRESS, POLIZ_GO,     POLIZ_FGO,   LEX_FIN,        // 50
    LEX_STRUCT_ID, LEX_DOT                                    // 51
};

class Lex {
    type_of_lex t_lex;
    int v_lex;
    public:
    Lex (type_of_lex t = LEX_NULL, int v = 0) {t_lex = t; v_lex = v;}
    type_of_lex get_type() {return t_lex;}
    int get_value() {return v_lex;}
    friend std::ostream& operator << (std::ostream &s, Lex l) {
        int fir;
        if (l.get_type()/10==0) fir=1; else fir=2;
        s << '(' << l.t_lex << ',' << std::setw(7-fir) << std::right << l.v_lex << ")";
        return s;
    }
};

class tabl_ident;

class Ident {
    char* name;
    bool declare;
    type_of_lex type;
    bool assign;
    int value;

public:
    Ident() {
        declare = false;
        assign = false;
    }
    ~Ident(){delete [] name;}
    Ident (Ident & I){
        put_name(I.name);
        put_value(I.value);
        put_type(I.type);
        assign = I.get_assign();
        declare = true;
    }
    friend std::ostream& operator << (std::ostream &s, tabl_ident TI);
    
    char *get_name() {return name;}
    void put_name(const char *n) {
        name = new char [strlen(n) + 1];
        strcpy(name, n);
    }
    bool get_declare() {return declare;}
    void put_declare() {declare = true;}
    type_of_lex get_type() {return type;}
    void put_type(type_of_lex t) {type = t;}
    bool get_assign() {return assign;}
    void put_assign() {assign = true;}
    void put_unassign() {assign = false;}
    int get_value() {return value;}
    void put_value(int v) {value = v;}
};

class tabl_ident {
    Ident* p;
    int size;
    int top;
    
public:
    friend class struct_type;
    
    tabl_ident(int max_size) {
        p = new Ident[size=max_size];
        top = 1;
    }
    ~tabl_ident() {}
    tabl_ident & operator=(const tabl_ident & ti){
        if (this == &ti) return *this;
        p = new Ident[size=ti.size];
        top = ti.top;
        for (int i=1; i<top; i++) *(p+i)=*(ti.p+i);
        return *this;
    }
    
    int get_top() {return top;}
    Ident& operator[](int k) {return *(p+k);}
    int put(const char *buf);
    
    friend std::ostream& operator << (std::ostream &s, tabl_ident TI) {
        for (int i=1; i<TI.top; i++)
            s << TI[i].name << std::endl;
        return s;
    }
};

int tabl_ident::put(const char *buf) {
    for (int j=1; j<top; ++j)
        if (!strcmp(buf, p[j].get_name()))
            return j;
    p[top].put_name(buf);
    ++top;
    return top-1;
}

class tabl_string {
    std::string* s;
    int size;
    int top;
    
    public:
    tabl_string(int max_size) {
        s = new std::string[size=max_size];
        top = 1;
    }
    ~tabl_string() {}
    int get_top() {return top;}
    std::string& operator[](int k) {return s[k];}
    int put(const char *buf);
    int put(std::string buf);
    
    friend std::ostream& operator << (std::ostream &s, tabl_string TS) {
        for (int i=1; i<TS.top; i++)
            s << TS[i] << std::endl;
        return s;
    }
};

int tabl_string::put(const char *buf) {
    s[top]=buf;
    ++top;
    return top-1;
}

int tabl_string::put(std::string buf) {
    s[top]=buf;
    ++top;
    return top-1;
}

class Scanner {
    enum state {H, IDENT, NUMB, STR, COM, ALE, DELIM, NEQ};
    static char* TW[];
    static type_of_lex words[];
    static char* TD[];
    static type_of_lex dlms[];
    state CS;
    FILE* fp;
    char c;
    char buf[80];
    int buf_top;
    
    void clear(){
        buf_top = 0;
        for (int j = 0; j < 80; ++j)
        buf[j] = '\0';
    }
    void add(){
        buf[buf_top++] = c;
    }
    int look(const char *buf, char **list) {
        int i = 0;
        while (list[i]) {
            if (!strcmp(buf, list[i])) return i;
            ++i;
        }
        return 0;
    }
    void gc() {c = fgetc(fp);}

public:
    friend void print_test_lex(Lex& L, Scanner& Cs);
    Lex get_lex();
    
    Scanner(const char* program){
        fp = fopen(program, "r");
        if (fp == NULL) throw (char*)"\n>O no! File didn't open!\n";
        CS = H;
        clear();
        gc();
    }
    
    void test_Lex_Scanner();
};
 
//=====================================================================//
//======================================================== TABLES =====//
//=====================================================================//

char* Scanner::TW[]={
    (char*)"",        (char*)"and",   (char*)"begin", (char*)"boolean",
    (char*)"do",      (char*)"else",  (char*)"end",   (char*)"if",
    (char*)"false",   (char*)"int",   (char*)"not",   (char*)"or",
    (char*)"program", (char*)"read",  (char*)"true",  (char*)"case",
    (char*)"while",   (char*)"write", (char*)"string",(char*)"struct",
    (char*)"!",       (char*)"!F",    (char*)"of",    (char*)"for",
    (char*)"step",    (char*)"until", (char*)"goto",  (char*)"continue",
    (char*)"break",
    
    NULL
};

char* Scanner::TD[]={
    (char*)"",   (char*)"@",  (char*)";", (char*)",",
    (char*)":",  (char*)"=",  (char*)"(", (char*)")",
    (char*)"==", (char*)"<",  (char*)">", (char*)"+",
    (char*)"-",  (char*)"*",  (char*)"/", (char*)"<=",
    (char*)"!=", (char*)">=", (char*)"{", (char*)"}",
    (char*)"%",  (char*)".",
    
    NULL
};

tabl_ident TID(100);
tabl_string TSTR(20);

type_of_lex Scanner::words[]={
    LEX_NULL,    LEX_AND,   LEX_BEGIN, LEX_BOOLEAN,
    LEX_DO,      LEX_ELSE,  LEX_END,   LEX_IF,
    LEX_FALSE,   LEX_INT,   LEX_NOT,   LEX_OR,
    LEX_PROGRAM, LEX_READ,  LEX_TRUE,  LEX_CASE,
    LEX_WHILE,   LEX_WRITE, LEX_STRING,LEX_STRUCT,
    POLIZ_GO,    POLIZ_FGO, LEX_OF,    LEX_FOR,
    LEX_STEP,    LEX_UNTIL, LEX_GOTO,  LEX_CONTINUE,
    LEX_BREAK,
    
    LEX_NULL
};

type_of_lex Scanner::dlms[]={
    LEX_NULL,  LEX_FIN,    LEX_SEMICOLON, LEX_COMMA,
    LEX_COLON, LEX_ASSIGN, LEX_LPAREN,    LEX_RPAREN,
    LEX_EQ,    LEX_LSS,    LEX_GTR,       LEX_PLUS,
    LEX_MINUS, LEX_TIMES,  LEX_SLASH,     LEX_LEQ,
    LEX_NEQ,   LEX_GEQ,    LEX_LFIG,      LEX_RFIG,
    LEX_PCNT,  LEX_DOT,
    
    LEX_NULL
};

//=====================================================================//
//================================================ get_lex METHOD =====//
//=====================================================================//

void print_test_lex(Lex & L, Scanner & Sc);

Lex Scanner::get_lex(){
    int d=0, j;
    Lex L;
    CS = H;
    
    do {
    switch (CS){

    case H:
        if (c == ' ' || c == '\n' || c== '\r' || c == '\t') gc();
        else if (isalpha(c)){
            clear();
            add();
            gc();
            CS = IDENT;
        } else if (isdigit(c)){
            d = c - '0';
            gc();
            CS = NUMB;
        } else if (c == '"'){
            clear();
            gc();
            CS = STR;
        } else if (c== '=' || c== '<' || c== '>'){
            clear();
            add();
            gc();
            CS = ALE;
        } else if (c=='/'){
            clear();
            add();
            gc();
            if (c!='*'){
                j = look(buf, TD);
                L = Lex(dlms[j],j);
                print_test_lex(L, *this);
                return L;
            }
            else {gc(); CS = COM;}
            //
        } else if (c == EOF) {
            L = Lex(LEX_FIN);
            print_test_lex(L, *this);
            return L;
        } else if (c == '!'){      // Знак в одиночку не употребляется
            clear();
            add();
            gc();
            CS = NEQ;
        } else {
            CS = DELIM;    // Знаки употребляются только по одному
        }
        break;
        
        case IDENT:
            if (isalpha(c) || isdigit(c)){
                add();
                gc();
            } else {
                if ((j = look(buf,TW))) {
                    L = Lex(words[j], j);
                    print_test_lex(L, *this);
                    return L;
                } else {
                    j = TID.put(buf);
                    L = Lex(LEX_ID, j);
                    print_test_lex(L, *this);
                    return L;
                }
            }
            break;
        
        case NUMB:
            if (isdigit(c)){
                d = d*10 + (c-'0');
                gc();
            } else {
                L = Lex(LEX_NUM,d);
                print_test_lex(L, *this);
                return L;
            }
            break;
            
        case STR:
            if (isalpha(c)){
                add();
                gc();
            } else if (c == '"'){
                gc();
                j = TSTR.put(buf);
                L = Lex(LEX_STR,j);
                print_test_lex(L, *this);
                return L;
            } else if (c==EOF) throw c;
            else gc();
            break;
        
        case COM:
            if (c=='*'){
                gc();
                if (c=='/') {gc(); CS=H; break;}
                if (c!=EOF) gc();
                else throw c;
            }
            else if (c!=EOF) gc();
            else throw c;
            break;
        
         
        case ALE:
            if (c == '=') {
                add();
                gc();
                j = look(buf, TD);
                L = Lex(dlms[j],j);
                print_test_lex(L, *this);
                return L;
            } else {
                j = look(buf, TD);
                L = Lex(dlms[j],j);
                print_test_lex(L, *this);
                return L;
            }
            break;
        
        case NEQ:
            if (c == '='){
                add();
                gc();
                j = look(buf,TD);
                L = Lex(LEX_NEQ,j);
                print_test_lex(L, *this);
                return L;
            }
            else throw '!';
            break;
        
        case DELIM:
            clear();
            add();
            if ((j = look(buf, TD))){
                gc();
                L = Lex(dlms[j],j);
                print_test_lex(L, *this);
                return L;
            }
            else
                throw c;
            break;
        }
    } while(1);
}

//=====================================================================//
//=============================== METHODS FOR THE SCANNER TESTING =====//
//=====================================================================//

void print_test_lex(Lex & L, Scanner & Sc){
    std::cout << L << "  ";
    
    switch (L.get_type()){
        case 0: std::cout << "NULL"; break;
        case 1: std::cout << "AND"; break;
        case 2: std::cout << "BEGIN"; break;
        case 3: std::cout << "BOOLEAN"; break;
        case 4: std::cout << "DO"; break;
        case 5: std::cout << "ELSE"; break;
        case 6: std::cout << "END"; break;
        case 7: std::cout << "IF"; break;
        case 8: std::cout << "FALSE"; break;
        case 9: std::cout << "INT"; break;
        case 10: std::cout << "NOT"; break;
        case 11: std::cout << "OR"; break;
        case 12: std::cout << "PROGRAM"; break;
        case 13: std::cout << "READ"; break;
        case 14: std::cout << "TRUE"; break;
        case 15: std::cout << "CASE"; break;
        case 16: std::cout << "WHILE"; break;
        case 17: std::cout << "WRITE"; break;
        case 18: std::cout << "OF"; break;
        case 19: std::cout << "FOR"; break;
        case 20: std::cout << "STRING"; break;
        case 21: std::cout << "STEP"; break;
        case 22: std::cout << "UNTIL"; break;
        case 23: std::cout << "GOTO"; break;
        case 24: std::cout << "CONTINUE"; break;
        case 25: std::cout << "BREAK"; break;
        case 26: std::cout << "STRUCT"; break;
        case 27: std::cout << ";"; break;
        case 28: std::cout << ","; break;
        case 29: std::cout << ":"; break;
        case 30: std::cout << "="; break;
        case 31: std::cout << "("; break;
        case 32: std::cout << ")"; break;
        case 33: std::cout << "=="; break;
        case 34: std::cout << "<"; break;
        case 35: std::cout << ">"; break;
        case 36: std::cout << "+"; break;
        case 37: std::cout << "-"; break;
        case 38: std::cout << "*"; break;
        case 39: std::cout << "/"; break;
        case 40: std::cout << "<="; break;
        case 41: std::cout << "!="; break;
        case 42: std::cout << ">="; break;
        case 43: std::cout << "%"; break;
        case 44: std::cout << "{"; break;
        case 45: std::cout << "}"; break;
        case 46:
            if (L.get_value() < lgt)
                std::cout << TID[L.get_value()].get_name() << " (ID)";
            else {
                std::cout << TID[L.get_value() / lgt].get_name();
                std::cout << ".";
                std::cout << TID[L.get_value() % lgt].get_name();
                std::cout << " (ID)";
            }
            break;
        case 47:
            std::cout << L.get_value();
            break;
        case 48:
            std::cout << TSTR[L.get_value()] << " (STR)";
            break;
        case 49:
            std::cout << L.get_value() << " (LBL)"; break;
        case 50:
            std::cout << "&";
            if (L.get_value() < lgt)
                std::cout << TID[L.get_value()].get_name();
            else {
                std::cout << TID[L.get_value() / lgt].get_name();
                std::cout << ".";
                std::cout << TID[L.get_value() % lgt].get_name();
            }
            break;
        case 51: std::cout << "!"; break;
        case 52: std::cout << "!F"; break;
        case 53: std::cout << "FIN"; break;
        case 54:
            std::cout << TID[L.get_value()].get_name() << " (STRUCT_ID)";
            break;
        case 55: std::cout << "."; break;
    }
    
    std::cout << "\n";
}
    
void Scanner::test_Lex_Scanner(){
    int err=0;
    std::cout << "\n>Program starts the work!\n\n";
        Lex L;
        do {
            try {L = get_lex();}
            catch(char cerr) {gc(); std::cout << "===================== ! ==================== " << "[" << cerr <<"]\n"; err=1;
            }
            catch(...){if (c==EOF) err=2; else err=1;}
            if (err==0) print_test_lex(L,*this);
            else err=0;
        } while (L.get_type()!=LEX_FIN && err!=2);
                 
        std::cout << std::endl;
        if (TID.get_top() != 1) {
            std::cout << "All IDs:\n";
            std::cout << TID;
        }
    
        std::cout << std::endl;
        if (TSTR.get_top() != 1) {
            std::cout << "All STRs:\n";
            std::cout << TSTR;
        }
        std::cout << std::endl;
    std::cout << ">Program finishes the work!\n\n";
}
