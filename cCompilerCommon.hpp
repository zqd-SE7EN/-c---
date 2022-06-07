#pragma once
#ifndef __C_COMPILER_COMMON_HPP__  
#define __C_COMPILER_COMMON_HPP__  
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<string>
#include<sstream>
#include<iostream>
#include<vector>
#include<map>
#include<cstdarg>
#include<cassert>
class SymbolTableStack;
class SymbolTable;
class VariableDeclarationNode;
class VariableDeclarationStatementNode;
struct Attribute;
extern int csLineCnt;
extern int csColumnCnt;
extern SymbolTableStack *symbolTableStack;


class NameCounter{
private:
    std::map<std::string, int> map;
    std::map<std::pair<std::string, int>, bool> mark;
public:
    NameCounter(){}
    std::string getNumberedName(std::string name);
    std::string releaseName(std::string name);
};
extern NameCounter programNameCounter;

class AddressCounter{
private:
    int addr;
public:
    AddressCounter():addr(0){}
    int getNextAddr(){
        return addr++;
    }
    int getNextNAddr(int n){
        int t = addr;
        addr += n;
        return t;
    }
};
extern AddressCounter* addressCounter;

class Node{
public:
    enum Type{
        TYPE_INT,TYPE_FLOAT,TYPE_DOUBLE,TYPE_CHAR,TYPE_STRING,TYPE_VOID,TYPE_STRUCT
    };
    enum Kind{
        KIND_FUNCTION,KIND_VARIABLE,KIND_ARGUMENT,KIND_ATTRIBUTE,KIND_CONSTANT
    };
protected:
    std::string mSymbolName;
    std::string mTokenValue; //(token string)
    bool mIsTerminal;
    bool mIsNegligible;
    std::vector<Node*> mChildren;
public:
    bool checkIfModified(std::string name);
    Node(){};
    Node(std::string _symbolName, int childrenNumber, ...);
    Node(std::string _tokenValue, bool negligible=false):mIsNegligible(negligible),mSymbolName("I am a terminal, valued "+_tokenValue),mIsTerminal(true),mTokenValue(_tokenValue){
    }
    void addChild(Node *newChild);
    Node* getChildrenById(int i);
    int getChildrenNumber();
    std::vector<Node*> getChildren();
    bool isTerminal()const;
    bool isNegligible();
    std::string getSymbolName()const;
    std::string getTokenValue();
    std::string getName()const;
    void printTree(int depth);
    void simplify();
    

public:
    virtual std::string codeGen(){
        for(auto i : mChildren){
            i->codeGen();
        }
        return "`";
    }
    virtual void setType(Node::Type _type){/*std::cout<<"wrong\n";*/}
    virtual void setType(Node* c){/*std::cout<<"wrong\n";*/}
    virtual Node::Type getType(){/*std::cout<<"wrong\n";*/}
    virtual std::string getTypeString(){/*std::cout<<"wrong\n";*/}
    virtual void setKind(Node::Kind _kind){/*std::cout<<"wrong\n";*/}
    virtual Node::Kind getKind(){/*std::cout<<"wrong\n";*/}
    virtual void setArgList(std::vector<Node::Type> _argList){/*std::cout<<"wrong\n";*/}
    virtual std::vector<Node::Type> getArgList(){/*std::cout<<"wrong\n";*/}
    virtual void setArgListStructName(std::vector<std::string> _structName){/*std::cout<<"wrong\n";*/}
    virtual std::vector<std::string> getArgListStructName(){/*std::cout<<"wrong\n";*/}
    virtual void setArraySizes(std::vector<int> _sizes){/*std::cout<<"wrong\n";*/}
    virtual std::vector<int> getArraySizes(){/*std::cout<<"wrong\n";*/}
    virtual bool isArray(){/*std::cout<<"wrong\n";*/}
    virtual int getArrayDimension(){/*std::cout<<"wrong\n";*/}
    virtual void setStructTypeName(std::string _name){/*std::cout<<"wrong\n";*/}
    virtual std::string getStructTypeName(){/*std::cout<<"wrong\n";*/}
    virtual void setVariableName(std::string _name){/*std::cout<<"wrong\n";*/}
    virtual std::string getVariableName(){/*std::cout<<"wrong\n";*/}
    virtual void setPosition(int l,int c){/*std::cout<<"wrong\n";*/}
    virtual void setPosition(Node*){/*std::cout<<"wrong\n";*/}
    virtual int getLineNumber(){/*std::cout<<"wrong\n";*/}
    virtual int getColumnNumber(){/*std::cout<<"wrong\n";*/}
    virtual void setAttribute(void *p);
    virtual void copyFromChild();
    virtual void copyFrom(Node *c);
    virtual void copyFrom(Attribute *c);
    virtual void fullCopyFrom(Node *c);
};


class AttributivedNode : public Node{
protected:
    AttributivedNode::Type mTokenType;
    AttributivedNode::Kind mTokenKind;
    std::vector<AttributivedNode::Type> mTokenArgList;
    std::vector<std::string> mTokenArgListStructTypeName;
    std::vector<int> mArraySizes;
    std::string mStructTypeName;
    std::string mVariableName;
    int mLineNumber;
    int mColumnNumber;
public:
    virtual std::string getArgumentVariableName(){throw("FOOL.");}
    virtual std::string codeGen(){
        for(auto i : mChildren){
            programNameCounter.releaseName(i->codeGen());
        }
        return "`";
    }
    virtual int getSize(){return -1;}
    AttributivedNode(){};
    AttributivedNode(std::string _symbolName, int childrenNumber, ...);
    AttributivedNode(std::string _tokenValue, bool negligible=false):Node(_tokenValue,negligible){}
    void setType(AttributivedNode::Type _type);
    void setType(Node *c);
    AttributivedNode::Type getType();
    std::string getTypeString();
    void setKind(AttributivedNode::Kind _kind);
    AttributivedNode::Kind getKind();
    void setArgList(std::vector<AttributivedNode::Type> _argList);
    std::vector<AttributivedNode::Type> getArgList();
    void setArgListStructName(std::vector<std::string> _structName);
    std::vector<std::string> getArgListStructName();
    void setArraySizes(std::vector<int> _sizes);
    std::vector<int> getArraySizes();
    bool isArray();
    int getArrayDimension();
    void setStructTypeName(std::string _name);
    std::string getStructTypeName();
    void setVariableName(std::string _name);
    std::string getVariableName();
    void setPosition(int l,int c);
    int getLineNumber();
    int getColumnNumber();
    void setPosition(Node *c);
};

class StructDeclarationNode : public AttributivedNode{
public:
    StructDeclarationNode(Node *a){this->fullCopyFrom(a);}
    Node::Type getType();
    int getSize();
    virtual std::string codeGen();
    int mSizeSum = -1;
};

class StructMemberNode : public AttributivedNode{
public:
    StructMemberNode(Node *a){this->fullCopyFrom(a);}
    int getSize();
    virtual std::string codeGen(){
        return "`";
    }
};

class VariableDeclarationStatementNode : public AttributivedNode{
public:
    VariableDeclarationStatementNode(Node *a){this->fullCopyFrom(a);}
    Node::Type getType();
    std::string getTypeString();
    virtual std::string codeGen();
};

class VariableDeclarationNode : public AttributivedNode{
public:
    VariableDeclarationNode(Node *a){this->fullCopyFrom(a);}
    Node::Type getType();
    std::string getTypeString();
    int getSize();
    virtual std::string codeGen();
};

class FunctionDeclarationNode : public AttributivedNode{
public:
    FunctionDeclarationNode(Node *c):AttributivedNode(){this->fullCopyFrom(c);}
    virtual std::string codeGen();
    int getSize();
};

class ExpressionNode : public AttributivedNode{
public:
    ExpressionNode(std::string _symbolName, int childrenNumber, ...);
    ExpressionNode(Node *c):AttributivedNode(){this->fullCopyFrom(c);}
    std::string getArgumentVariableName();
    virtual std::string codeGen();
public:
    int intValue;
    double doubleValue;
};

class EmptyNode : public AttributivedNode{
public:
    EmptyNode(Node *a){this->fullCopyFrom(a);}
    virtual std::string codeGen(){return "`";}
};

class StringNode : public ExpressionNode{
public:
    StringNode(Node *c):ExpressionNode(c){}
    virtual std::string codeGen();
};

class IfNode : public AttributivedNode{
public:
    IfNode(Node *c):AttributivedNode(){this->fullCopyFrom(c);}
    virtual std::string codeGen();
};

class ForNode : public AttributivedNode{
public:
    ForNode(Node *c):AttributivedNode(){this->fullCopyFrom(c);}
    virtual std::string codeGen();
    bool boostable();
};

class WhileNode : public AttributivedNode{
public:
    WhileNode(Node *c):AttributivedNode(){this->fullCopyFrom(c);}
    virtual std::string codeGen();
};
class RetNode : public AttributivedNode{
public:
    RetNode(Node* c):AttributivedNode(){this->fullCopyFrom(c);}
    virtual std::string codeGen();
};

class ReadNode : public ExpressionNode{
public:
    ReadNode(std::string _symbolName, int childrenNumber, ...);
    virtual std::string codeGen();
};

class WriteNode : public ExpressionNode{
public:
    WriteNode(std::string _symbolName, int childrenNumber, ...);
    virtual std::string codeGen();
};

struct Attribute{
    std::string name;
    Node::Type type;
    Node::Kind kind;
    std::vector<Node::Type> argList;
    std::vector<std::string> argListStructName;
    std::vector<int> arraySizes;
    std::string structTypeName;
    int addr;
    int size=-1;
    int offset=-1;
    int lineNumber;
    int columnNumber;
    Attribute(std::string _name, Node::Type _type, Node::Kind _kind, std::vector<Node::Type> _argList, std::vector<std::string> _argListStructName, std::vector<int> _arraySizes, std::string _structTypeName, int l, int c)
        : name(_name),type(_type),kind(_kind),argList(_argList),arraySizes(_arraySizes),structTypeName(_structTypeName),lineNumber(l),columnNumber(c),argListStructName(_argListStructName){};
    Attribute(){}
    Attribute(Node *p)
        : name(p->getVariableName()),type(p->getType()),kind(p->getKind()),argList(p->getArgList()),arraySizes(p->getArraySizes()),
          structTypeName(p->getStructTypeName()),lineNumber(p->getLineNumber()),columnNumber(p->getColumnNumber()),argListStructName(p->getArgListStructName()){};
    void print();
};

class SymbolTable{
private:
    std::string mSymbolTableName;
    std::map<std::string, Attribute*> map;
    static std::map<std::string, SymbolTable*> set;
public:
    SymbolTable();
    SymbolTable(std::string name);
    std::map<std::string, Attribute*> getTable();
    std::string getName();
    bool insert(Attribute* t);
    Attribute *lookUp(std::string name);
    void print();
    static SymbolTable *getSymbolTableByName(std::string symbolTableName);
    static void viewAllSymbolTable();
};

class SymbolTableStack{
private:
    std::vector<SymbolTable*> stack;
public:
    SymbolTableStack(SymbolTable *globalSymbolTable);
    void push(SymbolTable* t);
    void pop();
    SymbolTable *top();
    Attribute *lookUp(std::string name);
    Attribute *lookUp(std::string name, std::string tName);
    bool insert(Attribute* t);
};

bool checkType(Node *p, Node::Type type);
bool checkKind(Node *p, Node::Kind kind);
bool typeMatch(Node *a, Node *b);
bool typeMatch(std::vector<Node::Type> a, std::vector<Node::Type> b);
bool typeMatch(std::vector<Node::Type> a,Node *c , std::vector<std::string> s);
bool typeMatch(Attribute *a, Node* b);
std::string type_to_string(Attribute *t);
#endif
