#pragma once
#include "./cCompilerCommon.hpp"

void Attribute::print(){
    std::cout<<name<<' ';
    switch(type){
        case Node::TYPE_INT:
            std::cout<<"int ";
            break;
        case Node::TYPE_DOUBLE:
            std::cout<<"double ";
            break;
        case Node::TYPE_STRUCT:
            std::cout<<"struct "<<structTypeName<<' ';
            break;
        default:
            std::cout<<type<<' ';
    }
    switch(kind){
        case Node::KIND_ARGUMENT:
            std::cout<<"augument ";
            break;
        case Node::KIND_ATTRIBUTE:
            std::cout<<"attribute ";
            break;
        case Node::KIND_CONSTANT:
            std::cout<<"constant ";
            break;
        case Node::KIND_FUNCTION:
            std::cout<<"function ";
            break;
        case Node::KIND_VARIABLE:
            std::cout<<"variable ";
            break;
        default:
            std::cout<<kind<<' ';
    }
    if(kind==Node::KIND_FUNCTION){
        std::cout<<'(';
        for(int i=0;i<argList.size();i++){
            auto string = argList[i];
            switch(string){
                case Node::TYPE_INT:
                    std::cout<<"int";
                    break;
                case Node::TYPE_DOUBLE:
                    std::cout<<"double";
                    break;
                case Node::TYPE_STRUCT:
                    std::cout<<"struct "<<argListStructName[i];
                    break;
                default:
                    std::cout<<type;
            }
            if(i!=argList.size()-1)std::cout<<",";
        }
        std::cout<<") ";
    }
    if(arraySizes.size()>0){
        for(auto size : arraySizes){
            std::cout<<"["<<size<<"]";
        }
        std::cout<<' ';
    }
    printf("addr:%d size:%d offset:%d ",addr,size,offset);
    printf(" --pos:l%dc%d\n", lineNumber, columnNumber);
}

SymbolTable::SymbolTable():mSymbolTableName({"Unamed Symbol Table"}){
    set.insert({mSymbolTableName, this});
}
SymbolTable::SymbolTable(std::string name):mSymbolTableName(name){
    set.insert({mSymbolTableName, this});
}
std::map<std::string, Attribute*> SymbolTable::getTable(){
    return map;
}
std::string SymbolTable::getName(){
    return mSymbolTableName;
}
bool SymbolTable::insert(Attribute* t){
    if(map.find(t->name)!=map.end()){
        return false;
    }else{
        map.insert({t->name,t});
        return true;
    }
}
Attribute *SymbolTable::lookUp(std::string name){
    if(map.find(name)==map.end()){
        return NULL;
    }else{
        return map[name];
    }
}
void SymbolTable::print(){
    std::cout<<"Symbol Table Name: "<<mSymbolTableName<<std::endl;
    int i=1;
    for(auto pair : map){
        printf("No.%03d ",i++);
        pair.second->print();
    }
}
SymbolTable *SymbolTable::getSymbolTableByName(std::string symbolTableName){
    return set[symbolTableName];
}
void SymbolTable::viewAllSymbolTable(){
    std::cout<<"Printing All Symbol Tables attended...\n";
    for(auto pair : set){
        std::cout<<"----------------------------------------------\n";
        pair.second->print();
        std::cout<<"----------------------------------------------\n";
    }
}

SymbolTableStack::SymbolTableStack(SymbolTable *globalSymbolTable){
    stack.push_back(globalSymbolTable);
}
void SymbolTableStack::push(SymbolTable* t){
    stack.push_back(t);
}
void SymbolTableStack::pop(){
    if(stack.size()==1){
        throw("You cannot pop the global symbol table.");
    }
    stack.pop_back();
}
SymbolTable *SymbolTableStack::top(){
    return stack[stack.size()-1];
}
Attribute *SymbolTableStack::lookUp(std::string name){
    for(int i=stack.size()-1;i>=0;i--){
        if(stack[i]->lookUp(name)){
            return stack[i]->lookUp(name);
        }
    }
    return NULL;
}
Attribute *SymbolTableStack::lookUp(std::string name, std::string tName){
    if(tName.length()==0)return this->lookUp(name);
    else {
        this->push(SymbolTable::getSymbolTableByName(tName));
        auto ret = this->lookUp(name);
        this->pop();
        return ret;
    }
}
bool SymbolTableStack::insert(Attribute* t){
    return stack[stack.size()-1]->insert(t);
}