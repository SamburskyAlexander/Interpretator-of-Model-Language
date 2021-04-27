//=====================================================================//
// Contents:                                                           //
//                                                                     //
//...Executer.....................................................12...//
//...Interpretator...............................................300...//
//...Main........................................................319...//
//=====================================================================//

#include "Syntax.cpp"

//=====================================================================//
//====================================================== EXECUTER =====//
//=====================================================================//

class Executer {
    Lex pc_el;
public:
    void execute(Poliz& prog);
};

void Executer::execute(Poliz& prog){
    Stack <int, 100> args;
    int i, j, k, index = 0, size = prog.get_free();
    
    while (index < size) {
        pc_el = prog[index];
        switch (pc_el.get_type()) {
        
            case LEX_STRUCT_ID:
            case LEX_NUM:
            case LEX_STR:
            case POLIZ_ADDRESS:
            case POLIZ_LABEL:
                args.push (pc_el.get_value());
                break;
                
            case LEX_TRUE:
                args.push(1);
                break;
                
            case LEX_FALSE:
                args.push(0);
                break;
                
            case LEX_ID:
                i = pc_el.get_value();
                if (i < lgt) {
                    args.push (TID[i].get_value());
                }
                else {
                    args.push (struct_id_value(i));
                }
                break;
                
            case LEX_NOT:
                args.push(!args.pop());
                break;
        
            case LEX_OR:
                i = args.pop();
                args.push (args.pop() || i);
                break;
            
            case LEX_AND:
                i = args.pop();
                args.push (args.pop() && i);
                break;
        
            case POLIZ_GO:
                index = args.pop() - 1;
                break;
        
            case POLIZ_FGO:
                i = args.pop();
                if (!args.pop()) index = i - 1;
                break;
            
            case LEX_WRITE:
            {
                int help_ind = index-1;
                while (prog[help_ind].get_type() != LEX_STR && prog[help_ind].get_type() != LEX_NUM && prog[help_ind].get_type() != LEX_BOOLEAN && prog[help_ind].get_type() != LEX_ID)
                    help_ind--;
                if (prog[help_ind].get_type() == LEX_ID) {
                    if (prog[help_ind].get_value() < lgt) {
                        if (TID[prog[help_ind].get_value()].get_type() == LEX_STRING) std::cout << TSTR[args.pop()];
                        else if (TID[prog[help_ind].get_value()].get_type() == LEX_BOOLEAN) {
                            i = args.pop();
                            if (i == 1) std::cout << "true";
                            else std::cout << "false";
                        } else std::cout << args.pop();
                    } else {
                        if (struct_id(prog[help_ind].get_value()).get_type() == LEX_STRING) std::cout << TSTR[args.pop()];
                        else if (struct_id(prog[help_ind].get_value()).get_type() == LEX_BOOLEAN) {
                            i = args.pop();
                            if (i == 1) std::cout << "true";
                            else std::cout << "false";
                        } else std::cout << args.pop();
                    }
                    
                } else if (prog[help_ind].get_type() == LEX_STR)
                    std::cout << TSTR[args.pop()];
                else std::cout << args.pop();
            }
                break;
            
            case LEX_READ:
            {
                int k;
                int sf=0;
                i = args.pop();
                if (i >= lgt) sf=1;
    
                if (sf == 0){
    
                    if (TID[i].get_type () == LEX_INT) {
                        std::cout << "Input int value for ";
                        std::cout << TID[i].get_name () << std::endl;
                        std::cin >> k;
                    } else if (TID[i].get_type() == LEX_STRING) {
                        char j[20];
                    rep1:
                        std::cout << "Input boolean value";
                        std::cout << " (true or false) for ";
                        std::cout << TID[i].get_name() << std::endl;
                        std::cin >> j;
                        if (!strcmp(j, "true"))
                            k = 1;
                        else if (!strcmp(j, "false"))
                            k = 0;
                        else {
                            std::cout << "Error in input:true/false\n";
                            goto rep1;
                        }
                    } else {
                        char* j = NULL;
                        std::cout << "Input string value";
                        std::cout << " for ";
                        std::cout << TID[i].get_name() << std::endl;
                        std::cin >> j;
                        k = TSTR.put(j);
                    }
                    TID[i].put_value(k);
                    
                } else {
                    
                    if (struct_id(i).get_type () == LEX_INT) {
                        std::cout << "Input int value for " << TID[i/lgt].get_name() << ".";
                        std::cout << struct_id(i).get_name () << std::endl;
                        std::cin >> k;
                    } else if (struct_id(i).get_type() == LEX_BOOLEAN) {
                        char j[20];
                    rep2:
                        std::cout << "Input boolean value";
                        std::cout << " (true or false) for " << TID[i/lgt].get_name() << ".";
                        std::cout << struct_id(i).get_name() << std::endl;
                        std::cin >> j;
                        if (!strcmp(j, "true"))
                            k = 1;
                        else if (!strcmp(j, "false"))
                            k = 0;
                        else {
                            std::cout << "Error in input:true/false\n";
                            goto rep2;
                        }
                        
                    } else {
                        char j[20];
                        std::cout << "Input string value";
                        std::cout << " for " << TID[i/lgt].get_name() << ".";
                        std::cout << struct_id(i).get_name() << std::endl;
                        std::cin >> j;
                        k = TSTR.put(j);
                    }
                    struct_id_value(i) = k;
                }
            }
                break;
                
            case LEX_PLUS:
                if (prog[index-1].get_type() == LEX_STR) {
                    i = args.pop();
                    j = args.pop();
                    std::string s = TSTR[j] + TSTR[i];
                    k = TSTR.put(s);
                    args.push(k);
                } else if (prog[index-1].get_type() == LEX_ID) {
                    if (prog[index-1].get_value() < lgt) {
                        if (TID[prog[index-1].get_value()].get_type() == LEX_STRING) {
                            i = args.pop();
                            j = args.pop();
                            std::string s = TSTR[j] + TSTR[i];
                            k = TSTR.put(s);
                            args.push(k);
                        }
                        else args.push (args.pop() + args.pop());
                    } else {
                        if (struct_id(prog[index-1].get_value()).get_type() == LEX_STRING) {
                            i = args.pop();
                            j = args.pop();
                            std::string s = TSTR[j] + TSTR[i];
                            k = TSTR.put(s);
                            args.push(k);
                        }
                        else std::cout << args.pop();
                    }
                } else args.push (args.pop() + args.pop());
                break;
                
            case LEX_MINUS:
                args.push (- args.pop() + args.pop());
                break;
            
            case LEX_TIMES:
                args.push (args.pop() * args.pop());
                break;
            
            case LEX_SLASH:
                i = args.pop();
                if (i){
                    args.push (args.pop()/i);
                    break;
                } else throw "POLIZ: divide by zero\n";
                
            case LEX_PCNT:
                i = args.pop();
                if (i){
                    args.push (args.pop()%i);
                    break;
                } else throw "POLIZ: divide by zero\n";
                
            case LEX_EQ:
                args.push (args.pop() == args.pop());
                break;
            
            case LEX_LSS:
                i = args.pop();
                args.push (args.pop() < i);
                break;
                
            case LEX_GTR:
                i = args.pop();
                args.push (args.pop() > i);
                break;
    
            case LEX_LEQ:
                i = args.pop();
                args.push (args.pop() <= i);
                break;
                
            case LEX_GEQ:
                i = args.pop();
                args.push (args.pop() >= i);
                break;
            
            case LEX_NEQ:
                i = args.pop();
                args.push (args.pop() != i);
                break;
                
            case LEX_ASSIGN:
                i = args.pop();
                j = args.pop();
                
                if (j < lgt){
                
                if (TID[prog[index-2].get_value()].get_type() == LEX_INT || TID[prog[index-2].get_value()].get_type() == LEX_BOOLEAN) {
                        TID[j].put_value(i);
                } else if (TID[prog[index-2].get_value()].get_type() == LEX_STRING) {
                    k = TSTR.put(TSTR[i]);
                    TID[j].put_value(k);
                } else if (TID[prog[index-2].get_value()].get_type() == LEX_STRUCT_ID) {
                    TSTRUCT[TSTRUCT.find(j)].VAL = TSTRUCT[TSTRUCT.find(i)].VAL;
                }

                } else {
                    
                    if (struct_id(j).get_type() == LEX_INT || struct_id(j).get_type() == LEX_BOOLEAN) {
                        struct_id_value(j)=i;
                    } else if (struct_id(j).get_type() == LEX_STRING) {
                        k = TSTR.put(TSTR[i]);
                        struct_id_value(j)=k;
                    }
                    
                }
                break;
                
            case LEX_SEMICOLON:
                args.pop();
                break;
            
            default:
                throw "POLIZ: unexpected elem\n";
        }
        ++index;
    }
    std::cout << "\n\nFinish of executing!!!\n";
}

//=====================================================================//
//================================================= INTERPRETATOR =====//
//=====================================================================//

class Interpretator {
    Parser pars;
    Executer E;
    
public:
    Interpretator(const char * program): pars(program){};
    void interpretation();
};

void Interpretator::interpretation() {
    pars.analyze();
    std::cout << "\n===== EXECUTION =====\n\n";
    E.execute (pars.prog);
}

//=====================================================================//
//========================================================== MAIN =====//
//=====================================================================//

int main(){
    try {
        Interpretator I("./Test");
        I.interpretation();
        return 0;
    }
    catch (char c) {
        std::cout << "unexpected symbol " << c << std::endl;
        return 1;
    }
    catch (Lex l) {
        std::cout << "unexpected lexeme "; std::cout << l;
        return 1;
    }
    catch (const char *source) {
        std::cout << source << std::endl;
        return 1;
    }
}
