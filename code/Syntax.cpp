//=====================================================================//
// Contents:                                                           //
//                                                                     //
//...Some classes.................................................15...//
//...Parser......................................................314...//
//...Analize.....................................................479...//
//...Declarations................................................510...//
//...Operators...................................................672...//
//=====================================================================//

#include "Lex.cpp"
#include <vector>

//=====================================================================//
//================================================== SOME CLASSES =====//
//=====================================================================//

template <class T, int max_size>
class Stack {
    T s[max_size];
    int top;

public:
    Stack() {top = 0;}
    ~Stack() {}
    int size() {return max_size;}
    int get_top() {return top;}
    void reset() {top = 0;}
    void push(T i);
    T pop();
    bool is_empty() {return top == 0;}
    bool is_full() {return top == max_size;}
};

template <class T, int max_size>
void Stack <T, max_size >::push(T i) {
    if (!is_full()) {
        s[top] = i;
        ++top;
    } else throw "=!= Stack_is_full\n";
}

template <class T, int max_size>
T Stack <T, max_size >::pop() {
    if (!is_empty()) {
    --top;
    return s[top];
    } else throw "=!= Stack_is_empty\n";
}

class Poliz {
    Lex *p;
    int size;
    int free;

public:
    Poliz(int max_size){
        p = new Lex [size = max_size];
        free = 0;
    }
    ~Poliz() {delete []p;}
    void put_lex(Lex l) {p[free]=l; ++free;}
    void put_lex(Lex l, int place) {p[place]=l;}
    void blank() {++free;}
    int get_free() {return free;}
    Lex& operator[] (int index){
        if (index > size) throw "=!= POLIZ:out of array\n";
        else if (index > free) throw "=!= POLIZ:indefinite element of array\n";
        else return p[index];
    };
    void print(Scanner & scan) {
        //for (int i = 0; i < free; ++i) std::cout << "   " << p[i] << "  ";
        for (int i=0; i<free; ++i) {
            std::cout << std::setw(3) << std::right << i << " ";
            print_test_lex(p[i], scan);
        }
    }
};

class Lbl{
    int id;
    bool used;  //Lbl: ...
    int place;
    Stack <int, 10> all_goto;
    
public:
    friend class table_Lbl;
    
    Lbl(int mid=0):id(mid),used(false),place(0),all_goto(){}
    ~Lbl(){}
    
    void put_id(int i) {id = i;}
    int get_id() {return id;}
    void put_used() {if (used) throw "=!= Lbl used already\n"; used = true;}
    bool get_used() {return used;}
    void put_place(int n) {place = n;}
    int get_place() {return place;}
    void put_goto(int n) {all_goto.push(n);}
    void make_all_goto(Poliz & P){
        int i;
        if (used){
            while (!all_goto.is_empty()){
                i = all_goto.pop();
                P.put_lex(Lex(POLIZ_LABEL,place), i);
            }
        } else throw "=!= Label is unused\n";
    }
};

class table_Lbl{
    Lbl* all_Lbl;
    int size;
    int free;
    
public:
    table_Lbl(int msize){all_Lbl = new Lbl[size=msize]; free=0;}
    ~table_Lbl(){delete [] all_Lbl;}
    
    void put_Lbl(int id){
        if (free==size) throw "=!= table_Lbl is full\n";
        if (find(id)==-1){
            (all_Lbl+free)->put_id(id);
            free++;
        }
    }
    
    void put_used(int id){
        (all_Lbl+find(id))->put_used();
    }
    
    bool get_used(int id){
        return (all_Lbl+find(id))->get_used();
    }
    
    int find(int id){
        int i=0;
        while (i!=free){
            if ((all_Lbl+i)->id == id) return i;
            i++;
        }
        return -1;
    }
    
    void put_place(int id, int place){
        if (find(id)==-1) put_Lbl(id);                  // LBL!!!
        put_used(id);
        (all_Lbl+find(id))->put_place(place);
    }
    
    void put_goto(int id, int where){
        (all_Lbl+find(id))->put_goto(where);
    }
    
    void make_all_Lbl(Poliz & P){
        for (int i=0; i<free; i++)
            (all_Lbl+i)->make_all_goto(P);
    }
};

class struct_type{
    int id_t;
    tabl_ident TI;
    std::vector <int> VAL;
    
public:
    friend int & struct_id_value(int i);
    friend class Parser;
    friend class All_struct;
    friend class Executer;
    
    struct_type(int mid=0):id_t(mid), TI(10), VAL(10){}
    ~struct_type(){}
    
    struct_type & operator=(const struct_type & st){
        if (this == &st) return *this;
        TI = st.TI;
        return *this;
    }
    
    Ident & operator[](int i){
        return TI[i];
    }
    
    void put_id(int id){id_t = id;}
    int get_id(){return id_t;}
    
    // id - номер в TID
    void put_pole(int id){
        TI.put(TID[id].get_name());
        TI[TI.top-1].put_declare();
        TI[TI.top-1].put_value(id);
        TI[TI.top-1].put_unassign();
    }
    
    int get_pole(int id){
        int i=1;
        while (i!=TI.get_top()) {if (TI[i].get_value() == id) return i; i++;}
        std::cout << "[AAAAAAAAAAAAAAA _ get_pole]\n";
        return -1;
    }
    
    void put_assign(){
        for (int i=1; i<TI.get_top(); i++)
            TI[i].put_assign();
    }
    
    void put_assign(int id){
        TI[get_pole(id)].put_assign();
    }
    
    bool get_assign(){
        int i=1;
        while (i!=TI.get_top()) {if (!TI[i].get_assign()) return false; i++;}
        return true;
    }
    
    void print(){
        for (int i=1; i<TI.top; i++) std::cout << "=-=-=" << TI[i].get_name();
        std::cout << "\n";
    }
};

class All_struct{
    struct_type *A;
    int size;
    int free;
    
public:
    friend class Parser;
    
    All_struct(int msize=5){
        A = new struct_type[size=msize];
        free = 0;
    }
    ~All_struct(){}
    
    struct_type & operator[](int n){
        if (n<0 || n>=free) throw "=!= All_struct is out of range\n";
        return *(A+n);
    }
    void put_struct(struct_type & st, int id){
        if (free==size) throw "=!= All_struct is full\n";
        *(A+free) = st;
        (A+free)->id_t = id;
        free++;
    }
    
    int get_struct(int id){
        int i=0;
        while (i!=free){
            if ((A+i)->id_t == id) return i;
            i++;
        }
        return -1;
    }
    
    int find(int id){
        int i=0;
        while (i!=free){
            if ((A+i)->id_t == id) return i;
            i++;
        }
        std::cout << "BBBBBBBBBBB _ find\n";
        return -1;
    }
    
    void put_assign(int s_id){
        if (find(s_id) == -1) throw "=!= No struct to put assign\n";
        (A+find(s_id))->put_assign();
    }
    
    void put_assign(int s_id, int id){
        if (find(s_id) == -1) throw "=!= No struct to put assign\n";
        (A+find(s_id))->put_assign(id);
    }
    
    bool get_assign(int id){
        if (find(id) == -1) throw "=!= No struct to check assign\n";
        return (A+find(id))->get_assign();
    }
};

class stypes{
    int *t;
    int size;
    int free;
    int chet;
    
public:
    stypes(int msize=10){t = new int[size=msize]; free=0; chet=0;}
    ~stypes(){delete [] t;}
    
    int & operator[](int n){return t[n];}
    
    void put(){
        if (free != size) {*(t+free)=chet; free++;}
        else throw "=!= Stypes is full\n";
    }
    
    int get(int i){
        if (i<0 || i>=free) throw "=!= stypes out off range\n";
        return *(t+i);
    }
    
    void ch(){chet++;}
    
    void print(){
        for (int i=0; i<free; i++) std::cout << " /" << t[i] << "/ ";
        std::cout << "\n";
    }
};

//=====================================================================//
//======================================================== PARSER =====//
//=====================================================================//

class Parser {
    Lex curr_lex;
    type_of_lex c_type;
    int c_val;
    //  //  //  //
public:
    Scanner scan;
private:
    //  //  //  //
    Stack <int, 100> st_int;
    Stack <type_of_lex, 100> st_lex;
    Stack <int, lgt> st_struct;
    Stack <int, 100> st_break;
    
    std::vector <int> VVAL;
    std::vector <Lex> VLEX;
    
    void P();
    void D();
    void D0();
    void DS(struct_type &);
    void DS1(struct_type &, const type_of_lex &);
    void DS2(struct_type &, const type_of_lex &);
    void FS1();
    void FS2();
    void FS2M();
    void D1(const type_of_lex &);
    void D2(const type_of_lex &);
    void N1();
    void N2();
    void L1();
    void L2();
    void STR1();
    void STR2();
    void B(char a='n');
    void S(char a='n');
    void S0();
    void E();
    void E1();
    void E2();
    void E3();
    void E4();
    void E5();
    void F();
    
    void dec (type_of_lex type);
    void check_id();
    void F_check_id();
    void check_id_struct(struct_type & st);
    void F_check_id_struct(struct_type & st);
    void check_op();
    void check_not();
    void eq_type();
    void eq_type_struct();
    void eq_bool();
    void check_id_in_read();
    void check_id_in_read_struct(struct_type & st);
    void struct_poliz();
    void gl() {
        curr_lex = scan.get_lex();
        c_type = curr_lex.get_type();
        c_val = curr_lex.get_value();
}
    
public:
    Poliz prog;
    Parser (const char *program) : scan (program), prog (1000) {}
    void analyze();
};

table_Lbl TLBL(10);
All_struct TSTRUCT(10);
stypes TSTYPES(10);

Ident & struct_id(int i){
    return TSTRUCT[TSTRUCT.find(i/lgt)][TSTRUCT[TSTRUCT.find(i/lgt)].get_pole(i%lgt)];
}

int & struct_id_value(int i){
    int k,n;
    if ((k=TSTRUCT.find(i/lgt))==-1) throw "=!= No struct_id_value\n";
    n = TSTRUCT[k].get_pole(i%lgt);
    return TSTRUCT[k].VAL[n];
}

void Parser::F_check_id() {
    if (TID[c_val].get_declare() && TID[c_val].get_assign()) st_lex.push(TID[c_val].get_type());
    else if (!TID[c_val].get_declare()) throw "=!= Not declared\n";
    else throw "=!= Not assigned\n";
}

void Parser::check_id() {
    if (TID[c_val].get_declare()) st_lex.push(TID[c_val].get_type());
    else throw "=!= Not declared\n";
}

void Parser::F_check_id_struct(struct_type & st){
    if (st.TI[st.get_pole(c_val)].get_declare() && st.TI[st.get_pole(c_val)].get_assign()) st_lex.push(st.TI[st.get_pole(c_val)].get_type());
    else if (!st.TI[st.get_pole(c_val)].get_declare()) throw "=!= Not declared (in struct)\n";
    else throw "=!= Not assigned (in struct)\n";
}

void Parser::check_id_struct(struct_type & st){
    if (st.TI[st.get_pole(c_val)].get_declare()) st_lex.push(st.TI[st.get_pole(c_val)].get_type());
    else throw "=!= Not declared (in struct)\n";
}

// Для контроля типов аргументов (типы лексем)
void Parser::check_op() {
    type_of_lex t1, t2, op, t;
    t2 = st_lex.pop();
    op = st_lex.pop();
    t1 = st_lex.pop();
    if (op == LEX_MINUS || op == LEX_TIMES || op == LEX_SLASH || op == LEX_PCNT) t = LEX_INT;
    else if (op == LEX_OR || op == LEX_AND) t = LEX_BOOLEAN;
    else if (op == LEX_PLUS && (t1 == LEX_INT || t1 == LEX_STRING))     t = t1;
    else if (op == LEX_EQ || op == LEX_LSS || op == LEX_LEQ || op == LEX_GTR || op == LEX_GEQ || op == LEX_NEQ) t = LEX_BOOLEAN;
    else throw "=!= Strange operation\n";
    if (t1 == t2) st_lex.push(t);
    else throw "=!= Different types of operands\n";
    prog.put_lex(Lex (op));
}

void Parser::check_not(){
    if (st_lex.pop() != LEX_BOOLEAN) throw "=!= Wrong type is in not\n";
    else {
        st_lex.push(LEX_BOOLEAN);
        prog.put_lex(Lex(LEX_NOT));
    }
}

void Parser::eq_type() {
    if (st_lex.pop() != st_lex.pop()) throw "=!= Wrong types are in :=\n";
}

void Parser::eq_type_struct() {
    if (st_struct.pop() != st_struct.pop()) throw "=!= Wrong types are in := (struct)\n";
}

void Parser::eq_bool() {
    if (st_lex.pop() != LEX_BOOLEAN) throw "=!= Expression is not boolean\n";
}

void Parser::check_id_in_read() {
    if (!TID [c_val].get_declare()) throw "=!= Not declared\n";
    TID [c_val].put_assign();
}

void Parser::check_id_in_read_struct(struct_type & st){
    if (!st.TI[st.get_pole(c_val)].get_declare()) throw "=!= Not declared (in struct)\n";
    st.TI[st.get_pole(c_val)].put_assign();
}

void Parser::struct_poliz(){
    for (int i=0; i<VVAL.size(); i++){
        prog.put_lex(Lex(POLIZ_ADDRESS,lgt*c_val+VVAL[i]));
        prog.put_lex(VLEX[i]);
        prog.put_lex(Lex(LEX_ASSIGN));
    }
}

//=====================================================================//
//======================================================= ANALIZE =====//
//=====================================================================//

void Parser::analyze() {
    gl();
    P();
    std::cout << "\nPOLIZ:\n";
    prog.print(scan);
    std::cout << "\nYes!!!\n";
}

void Parser::P() {
    if (c_type == LEX_PROGRAM) gl();
    else throw curr_lex;
    if (c_type == LEX_LFIG) {
        std::cout << "=============== DECLARATIONS =================\n";
        gl();
    }
    else throw curr_lex;
    
    D();
    std::cout << "============== OPERATORS (1UP) ================\n";
    B();
    
    if (c_type != LEX_RFIG) throw curr_lex;
    gl();
    if (c_type != LEX_FIN) throw curr_lex;
    TLBL.make_all_Lbl(prog);
}

//=====================================================================//
//================================================== DECLARATIONS =====//
//=====================================================================//

void Parser::D() {
    type_of_lex t;
    while (c_type == LEX_INT || c_type == LEX_BOOLEAN || c_type == LEX_STRING || c_type == LEX_STRUCT){
        if (c_type == LEX_STRUCT) {
            gl();
            D0();
        } else {
            t = c_type;
            gl(); D1(t);
        }
        if (c_type == LEX_SEMICOLON) gl();
        else throw curr_lex;
    }
}

void Parser::D1(const type_of_lex & t) {
    D2(t);
    while (c_type == LEX_COMMA){
        gl();
        D2(t);
    }
}

void Parser::D2(const type_of_lex & t) {
    int id_num;
    if (c_type != LEX_ID) throw curr_lex;
    if (TID[c_val].get_declare()) throw "=!= Twice declared\n";
    TID[c_val].put_declare();
    TID[c_val].put_type(t);
    id_num = c_val;
    gl();
    if (c_type == LEX_ASSIGN) {
        c_val = id_num;
        check_id();
        TID[c_val].put_assign();
        prog.put_lex (Lex(POLIZ_ADDRESS,c_val));
        gl();
        E5();
        eq_type();
        prog.put_lex (Lex (LEX_ASSIGN));
    }
}

void Parser::D0(){
    if (c_type == LEX_LFIG){
        struct_type ST;
        VVAL.clear();
        VLEX.clear();
        gl();
        while (c_type != LEX_RFIG && c_type != LEX_FIN){
            if (!(c_type == LEX_INT || c_type == LEX_BOOLEAN || c_type == LEX_STRING)) throw curr_lex;
            DS(ST);
        }
        if (c_type == LEX_RFIG) {
            gl();
            if (c_type == LEX_ID){
                TSTRUCT.put_struct(ST,c_val);
                (TSTRUCT.A+TSTRUCT.free-1)->put_id(c_val);
                TID[c_val].put_type(LEX_STRUCT_ID);
                TID[c_val].put_declare();
                TSTYPES.put();
                struct_poliz();
                gl();
                while (c_type == LEX_COMMA) {
                    gl();
                    if (c_type == LEX_ID){
                        TSTRUCT.put_struct(ST,c_val);
                        (TSTRUCT.A+TSTRUCT.free-1)->put_id(c_val);
                        TID[c_val].put_type(LEX_STRUCT_ID);
                        TID[c_val].put_declare();
                        TSTYPES.put();
                        struct_poliz();
                        gl();
                    } else throw curr_lex;
                } TSTYPES.ch();
            } else throw curr_lex;
        } else throw curr_lex;
    } else throw curr_lex;
}

void Parser::DS(struct_type & st){
    type_of_lex t;
    while (c_type == LEX_INT || c_type == LEX_BOOLEAN || c_type == LEX_STRING){
        t = c_type;
        gl(); DS1(st,t);
        if (c_type == LEX_SEMICOLON) gl();
        else throw curr_lex;
    }
}

void Parser::DS1(struct_type & st, const type_of_lex & t) {
    DS2(st,t);
    while (c_type == LEX_COMMA){
        gl();
        DS2(st,t);
    }
}

void Parser::DS2(struct_type & st, const type_of_lex & t) {
    int id_num;
    if (c_type != LEX_ID) throw curr_lex;
    if (st.TI[st.get_pole(c_val)].get_declare()) throw "=!= Twice declared\n";
    st.put_pole(c_val);
    st.TI[st.get_pole(c_val)].put_declare();
    st.TI[st.get_pole(c_val)].put_type(t);
    
    st_lex.push(t);
    
    id_num = c_val;
    gl();
    if (c_type == LEX_ASSIGN) {
        c_val = id_num;
        check_id_struct(st);
        st.TI[st.get_pole(c_val)].put_assign();
        VVAL.push_back(c_val);
        gl();
        FS1();//
        eq_type();
    }
}

void Parser::FS1(){
    if (c_type == LEX_MINUS) {
        st_lex.push(LEX_INT);
        gl();
        FS2M();
    } else FS2();
}

void Parser::FS2(){
    if (c_type == LEX_NUM) {
        st_lex.push(LEX_INT);
        VLEX.push_back(curr_lex);
        gl();
    } else if (c_type == LEX_TRUE) {
        st_lex.push(LEX_BOOLEAN);
        VLEX.push_back(Lex (LEX_TRUE,1));
        gl();
    } else if (c_type == LEX_FALSE) {
        st_lex.push(LEX_BOOLEAN);
        VLEX.push_back(Lex (LEX_FALSE,0));
        gl();
    } else if (c_type == LEX_STR) {
        st_lex.push(LEX_STRING);
        VLEX.push_back(curr_lex);
        gl();
    } else throw curr_lex;
}

void Parser::FS2M(){
    if (c_type == LEX_NUM) {
        st_lex.push(LEX_INT);
        curr_lex = Lex(curr_lex.get_type(),-1*curr_lex.get_value());
        VLEX.push_back(curr_lex);
        gl();
    } else throw curr_lex;
}

//=====================================================================//
//===================================================== OPERATORS =====//
//=====================================================================//

void Parser::B(char a) {
    while (c_type != LEX_RFIG && c_type != LEX_FIN) S(a);
}

void Parser::S(char a) {
    int pl1,pl2,pl3,pl4;
    int sv,v=0,sf=0,br;
    
    switch (c_type){
    
    case LEX_IF:
            if (a == 'e') throw "=!= Error in operator-expression\n";
            gl();
            E();
            eq_bool();
            pl1 = prog.get_free();
            prog.blank();
            prog.put_lex (Lex(POLIZ_FGO));
            S(a);
            pl2 = prog.get_free();
            prog.blank();
            prog.put_lex (Lex(POLIZ_GO));
            prog.put_lex (Lex(POLIZ_LABEL, prog.get_free()),pl1);
            if (c_type == LEX_ELSE){
                gl();
                S(a);
                prog.put_lex(Lex(POLIZ_LABEL,prog.get_free()),pl2);
            } else throw curr_lex;
            break;
            
    case LEX_WHILE:
            if (a == 'e') throw "=!= Error in operator-expression\n";
            pl1 = prog.get_free();
            gl();
            E();
            eq_bool();
            pl2 = prog.get_free();
            prog.blank();
            prog.put_lex (Lex(POLIZ_FGO));
            
            br = st_break.get_top();
            S('b');

            prog.put_lex (Lex (POLIZ_LABEL, pl1));
            prog.put_lex (Lex (POLIZ_GO));
            prog.put_lex (Lex(POLIZ_LABEL, prog.get_free()),pl2);
            
            br = st_break.get_top() - br;
            if (br) {
                for (int i=0; i<br; i++)
                prog.put_lex(Lex(POLIZ_LABEL,prog.get_free()), st_break.pop());
            }
        break;
     
    case LEX_READ:
            if (a == 'e') throw "=!= Error in operator-expression\n";
            gl();
            if (c_type == LEX_LPAREN){
                gl();
                if (c_type == LEX_ID) {
                    
                    if (TID[c_val].get_type() == LEX_STRUCT_ID) {
                        sv = c_val;
                        gl();
                        if (c_type == LEX_DOT) {
                            gl();
                            if (c_type == LEX_ID) {
                                v = c_val;
                            check_id_in_read_struct(TSTRUCT[TSTRUCT.get_struct(sv)]);
                                prog.put_lex (Lex (POLIZ_ADDRESS, lgt*sv+v));
                                gl();
                            } else throw curr_lex;
                        } else throw "=!= Struct in read\n";
                    } else {
                        check_id_in_read();
                        prog.put_lex (Lex (POLIZ_ADDRESS, c_val));
                        gl();
                    }
                    
                } else throw curr_lex;
                if (c_type == LEX_RPAREN) {
                    gl();
                    prog.put_lex (Lex (LEX_READ));
                } else throw curr_lex;
            } else throw curr_lex;
            if (c_type == LEX_SEMICOLON) gl(); else throw curr_lex;
            break;
            
    case LEX_WRITE:
            if (a == 'e') throw "=!= Error in operator-expression\n";
            st_struct.reset();
            st_lex.reset();
            gl();
            if (c_type == LEX_LPAREN) {
                gl();
                E();
                if (!st_struct.is_empty()) throw "=!= Struct in write\n";
                prog.put_lex (Lex(LEX_WRITE));
                while (c_type == LEX_COMMA){
                    gl();
                    E();
                    if (!st_struct.is_empty()) throw "=!= Struct in write\n";
                    prog.put_lex (Lex(LEX_WRITE));
                }
                if (c_type == LEX_RPAREN) gl();
                else throw curr_lex;
            } else throw curr_lex;
            if (c_type == LEX_SEMICOLON) gl(); else throw curr_lex;
            break;
            
    case LEX_ID:
            st_struct.reset();
            st_lex.reset();
            sv = c_val;
            if (TID[sv].get_type() == LEX_STRUCT_ID) sf=1;
            gl();
            if (c_type == LEX_DOT) {
                gl();
                if (c_type == LEX_ID) {
                    v = c_val;
                    gl();
                } else throw curr_lex;
            }
            if (c_type == LEX_ASSIGN) {
                if (v == 0 || sf == 0) {
                    c_val = sv;
                    if (sf == 0) check_id();
                    else st_struct.push(TSTYPES[TSTRUCT.get_struct(sv)]);
                    prog.put_lex (Lex(POLIZ_ADDRESS,c_val));
                    gl();
                    E();
                    if (st_lex.get_top() == st_struct.get_top())
                        throw "=!= Different types (struct/not struct)\n";
                    else if (sf == 0) eq_type();
                    else eq_type_struct();
                    prog.put_lex (Lex (LEX_ASSIGN));
                    TID[sv].put_assign();
                    if (sf != 0) TSTRUCT.put_assign(sv);
                    if (a != 'e') {
                        if (c_type == LEX_SEMICOLON) gl();
                        else throw curr_lex;
                    }
                } else {
                    c_val = v;
                    check_id_struct(TSTRUCT[TSTRUCT.get_struct(sv)]);
                    prog.put_lex (Lex(POLIZ_ADDRESS,lgt*sv+v));
                    gl();
                    E();
                    eq_type();
                    prog.put_lex (Lex (LEX_ASSIGN));
                    TSTRUCT.put_assign(sv,v);
                    if (a != 'e') {
                        if (c_type == LEX_SEMICOLON) gl();
                        else throw curr_lex;
                    }
                }
            } else if (c_type == LEX_COLON) {
                if (a == 'e') throw "=!= Error in operator-expression\n";
                TLBL.put_place(sv, prog.get_free());
                gl();
            } else throw curr_lex;
            break;
            
    case LEX_FOR:
            if (a == 'e') throw "=!= Error in operator-expression\n";
            gl();
            if (c_type == LEX_LPAREN){
                gl();
                S('e');  
                pl1 = prog.get_free();
                if (c_type == LEX_SEMICOLON){
                    gl();
                    E();
                    eq_bool();
                    pl3 = prog.get_free();
                    prog.blank();
                    prog.put_lex(Lex(POLIZ_FGO));
                    pl2 = prog.get_free();
                    prog.blank();
                    prog.put_lex(Lex(POLIZ_GO));
                    if (c_type == LEX_SEMICOLON){
                        gl();
                        pl4 = prog.get_free();
                        S('e');
                        if (c_type == LEX_RPAREN){
                            gl();
                            br = st_break.get_top();
                            prog.put_lex(Lex(POLIZ_LABEL,pl1));
                            prog.put_lex(Lex(POLIZ_GO));
                            prog.put_lex(Lex(POLIZ_LABEL,prog.get_free()),pl2);
                            S('b');
                            prog.put_lex(Lex(POLIZ_LABEL,pl4));
                            prog.put_lex(Lex(POLIZ_GO));
                            prog.put_lex(Lex(POLIZ_LABEL, prog.get_free()),pl3);
                            
                            br = st_break.get_top() - br;
                            if (br) {
                                for (int i=0; i<br; i++)
                                prog.put_lex(Lex(POLIZ_LABEL,prog.get_free()), st_break.pop());
                            }
                        } else throw curr_lex;
                    } else throw curr_lex;
                } else throw curr_lex;
            } else throw curr_lex;
            break;
            
    case LEX_LFIG:
            if (a == 'e') throw "=!= Error in operator-expression\n";
            gl();
            B(a);
            if (c_type == LEX_RFIG) gl();
            else throw curr_lex;
            break;
            
    case LEX_GOTO:
            if (a == 'e') throw "=!= Error in operator-expression\n";
            gl();
            if (c_type == LEX_ID){
                TLBL.put_Lbl(c_val);
                pl1 = prog.get_free();
                TLBL.put_goto(c_val, pl1);
                prog.blank();
                prog.put_lex(Lex(POLIZ_GO));
                gl();
                if (c_type == LEX_SEMICOLON) gl(); else throw curr_lex;//
            }
            else throw curr_lex;
            break;
        
        case LEX_BREAK:
            if (a == 'e') throw "=!= Error in operator-expression\n";
            if (a != 'b') throw "=!= break isn't in cicle\n";
            st_break.push(prog.get_free());
            prog.blank();
            prog.put_lex(Lex(POLIZ_GO));
            gl();
            if (c_type == LEX_SEMICOLON) gl(); else throw curr_lex;
            break;
            
    default:
            throw curr_lex;
            break;
    }
}

void Parser::E() {
    E1();
    if (c_type == LEX_OR){
        st_lex.push(c_type);
        gl();
        E1();
        check_op();
    }
}

void Parser::E1() {
    E2();
    if (c_type == LEX_AND){
        st_lex.push(c_type);
        gl();
        E2();
        check_op();
    }
}

void Parser::E2() {
    E3();
    if (c_type == LEX_EQ || c_type == LEX_LSS || c_type == LEX_LEQ || c_type == LEX_GTR || c_type == LEX_GEQ || c_type == LEX_NEQ){
        st_lex.push(c_type);
        gl();
        E3();
        check_op();
    }
}

void Parser::E3() {
    E4();
    while (c_type==LEX_PLUS || c_type==LEX_MINUS){
        st_lex.push(c_type);
        gl();
        E4();
        check_op();
    }
}

void Parser::E4(){
    E5();
    while (c_type==LEX_TIMES || c_type==LEX_SLASH || c_type == LEX_PCNT){
        st_lex.push (c_type);
        gl();
        E5();
        check_op();
    }
}

void Parser::E5(){
    if (c_type == LEX_LPAREN) {
        gl();
        E();
        if (c_type == LEX_RPAREN) gl();
        else throw curr_lex;
    } else if (c_type == LEX_MINUS) {
        st_lex.push(LEX_INT);
        prog.put_lex(Lex(LEX_NUM,0));
        gl();
        F();
        prog.put_lex(Lex(LEX_MINUS));
    } else F();
}

void Parser::F() {
    int sv,v=0,sf=0;
    
    if (c_type == LEX_ID) {
        
        if (TID[c_val].get_type() == LEX_STRUCT_ID) sf = 1;
        sv = c_val;
        gl();
        if (c_type == LEX_DOT) {
            gl();
            if (c_type == LEX_ID) {
                v = c_val;
                F_check_id_struct(TSTRUCT[TSTRUCT.get_struct(sv)]);
                prog.put_lex (Lex (LEX_ID, lgt*sv+v));
                gl();
            } else throw curr_lex;
        }
        if (sf == 0 || v == 0) {
            c_val = sv;
            if (sf == 0) {
                F_check_id();
                prog.put_lex (Lex (LEX_ID, sv));
            } else {
                if (!TSTRUCT.get_assign(c_val)) throw "=!= F: struct is not assigned\n";
                st_struct.push(TSTYPES[TSTRUCT.get_struct(sv)]);
                prog.put_lex (Lex (LEX_STRUCT_ID, sv));
            }
        }
        
    } else if (c_type == LEX_NUM) {
        st_lex.push(LEX_INT);
        prog.put_lex(curr_lex);
        gl();
    } else if (c_type == LEX_TRUE) {
        st_lex.push(LEX_BOOLEAN);
        prog.put_lex(Lex (LEX_TRUE,1));
        gl();
    } else if (c_type == LEX_FALSE) {
        st_lex.push(LEX_BOOLEAN);
        prog.put_lex (Lex (LEX_FALSE,0));
        gl();
    } else if (c_type == LEX_STR) {
        st_lex.push(LEX_STRING);
        prog.put_lex(curr_lex);
        gl();
    } else if (c_type == LEX_NOT) {
        gl();
        F();
        check_not();
    } else throw curr_lex;
}

