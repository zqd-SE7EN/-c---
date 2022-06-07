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
    std::string mSymbolName; // 产生式的名字（符号的名字）
    std::string mTokenValue; // token的字符串
    bool mIsTerminal;       // 是否是终结符
    bool mIsNegligible;     // 是否可以删掉。比如对分号、逗号这样的终结符，这个是true。
    std::vector<Node*> mChildren; // 孩子们
public:
    // 检验名为name的变量在该单元内，是否被赋值。是true否false
    bool checkIfModified(std::string name);
    Node(){};
    Node(std::string _symbolName, int childrenNumber, ...);
    Node(std::string _tokenValue, bool negligible=false):mIsNegligible(negligible),mSymbolName("I am a terminal, valued "+_tokenValue),mIsTerminal(true),mTokenValue(_tokenValue){}
    void addChild(Node *newChild);
    Node* getChildrenById(int i);
    int getChildrenNumber();
    std::vector<Node*> getChildren();
    bool isTerminal()const;
    bool isNegligible();
    std::string getSymbolName()const;
    std::string getTokenValue();

    /* 返回名字：
       对于算符，返回这个算符；
       对于变量，返回这个变量的名字
       对于常量，返回这个常量（以字符串形式）
       对于树中的其它节点，返回这个节点对应的产生式名字
    */
    std::string getName()const;

    // 打印以这里为根节点的数，用来看的。
    // 用法：printTree(0);
    void printTree(int depth);

    // 简化树，删去所有的诸如分号、逗号、大括号等对AST无用的东西
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
    // 数据类型，如int、void等。
    AttributivedNode::Type mTokenType; 

    // 该节点的类别，如变量、函数名、结构体名等。
    AttributivedNode::Kind mTokenKind; 

    // 对于函数，这里存了参数的类型；
    // 对于其它，这个是空的。
    std::vector<AttributivedNode::Type> mTokenArgList; 

    // 配合mTokenArgList存储参数类型的结构体名
    std::vector<std::string> mTokenArgListStructTypeName;

    // 数组的维度。
    // 对于非数组，这个是空的。
    std::vector<int> mArraySizes;

    // 结构体的名字。
    // 若变量不是结构体，这个是空的。
    std::string mStructTypeName;

    // 变量名。
    std::string mVariableName;

    // 行数，用于报错。
    int mLineNumber;

    // 列数，用于报错。
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
    
    // 设定数据类型，如int、void等。
    void setType(AttributivedNode::Type _type);

    // 从节点c复制数据类型。
    void setType(Node *c);

    // 取得数据类型，如int、void等。
    AttributivedNode::Type getType();

    // 取得数据类型的字符串版本，用于方便输出。
    std::string getTypeString();

    // 设定类别，如变量、函数名、结构体名等。
    void setKind(AttributivedNode::Kind _kind);

    // 取得该节点的类别，如变量、函数名、结构体名等。
    AttributivedNode::Kind getKind();

    // 设定该函数的参数们的类型。
    void setArgList(std::vector<AttributivedNode::Type> _argList);
    
    // 对于函数，取得参数的类型；
    // 若不是函数，取得一个空vector。
    std::vector<AttributivedNode::Type> getArgList();

    // 配合setArgList()设定参数类型的结构体名
    void setArgListStructName(std::vector<std::string> _structName);
    
    // 配合getArgList()取得参数类型的结构体名
    std::vector<std::string> getArgListStructName();

    // 设定数组的维度。
    void setArraySizes(std::vector<int> _sizes);

    // 取得数组的维度。
    // 若不是数组，取得一个空vector。
    std::vector<int> getArraySizes();

    // 判定是不是数组。是true；不是false
    bool isArray();

    // 取得数组的维数。
    // 例：int a[5];    是1维数组
    //     int a[5][5]; 是2维数组
    int getArrayDimension();

    // 设定结构体的名字
    void setStructTypeName(std::string _name);

    // 取得结构体的名字
    // 对于类型是结构体的，取得结构体的名字
    // 若变量类型不是结构体，取得空字符串
    std::string getStructTypeName();

    // 设定变量名字，常用于算符的变量名继承
    // 例：a[10]的变量名还是a，但顶层只能看到算符"[]"
    void setVariableName(std::string _name);

    // 取得变量名字，常用于跨越取得下层的变量名
    // 例：a[10]的变量名还是a，但顶层只能看到算符"[]"
    std::string getVariableName();

    // 设置这个在文件中的位置
    void setPosition(int l,int c);
    int getLineNumber();
    int getColumnNumber();

    // 复制位置
    void setPosition(Node *c);
};

// 结构体定义，其孩子是结构体名和{若干结构体成员定义节点}（2个孩子）
// 创建方法：
// 1. 首先建立AttributivedNode以维护树结构
// 2. 用构造函数StructDeclarationNode(Node *) 将刚建立的AttributivedNode转成StructDeclarationNode
class StructDeclarationNode : public AttributivedNode{
public:
    // 将一个普通Node转成StructDeclarationNode，并复制所有的信息。
    StructDeclarationNode(Node *a){this->fullCopyFrom(a);}

    // 总是返回Node::TYPE_STRUCT
    Node::Type getType();

    // 计算这个结构体的数据宽度
    // 如，struct node{int a[10],b,c;} 的宽度就是 13
    int getSize();
    virtual std::string codeGen();
    int mSizeSum = -1;
};

// 结构体成员定义，一般没有逻辑上的孩子，codeGen()也是到此不递归。
// 创建方法：
// 1. 首先建立 AttributivedNode 以维护树结构
// 2. 用构造函数 StructMemberNode(Node *) 将刚建立的 AttributivedNode 转成 StructMemberNode
class StructMemberNode : public AttributivedNode{
public:
    // 将一个普通Node转成StructMemberNode，并复制所有的信息。
    StructMemberNode(Node *a){this->fullCopyFrom(a);}

    // 返回这个成员的宽度。
    // 通常，int的宽度是1、数组的宽度是单个宽度乘以数组大小。
    int getSize();
    virtual std::string codeGen(){
        return "`";
    }
};

// 变量定义语句节点，其孩子是若干变量定义节点
// 创建方法：
// 1. 首先建立 AttributivedNode 以维护树结构
// 2. 用构造函数 VariableDeclarationStatementNode(Node *) 将刚建立的 AttributivedNode 转成 VariableDeclarationStatementNode
class VariableDeclarationStatementNode : public AttributivedNode{
public:
    // 将一个普通Node转成VariableDeclarationStatementNode，并复制所有的信息。
    VariableDeclarationStatementNode(Node *a){this->fullCopyFrom(a);}

    // 返回这个语句中变量的类型。
    // 例：int a,b,c,d; 是一个 VariableDeclarationStatementNode ，此时这个 getType() 返回 Node::TYPE_INT
    Node::Type getType();

    // 用来方便输出的
    std::string getTypeString();
    virtual std::string codeGen();
};

// 变量定义节点，没有逻辑上的孩子。codeGen()也是到此不递归。
// 创建方法：
// 1. 首先建立 AttributivedNode 以维护树结构
// 2. 用构造函数 VariableDeclarationNode(Node *) 将刚建立的 AttributivedNode 转成 VariableDeclarationNode
class VariableDeclarationNode : public AttributivedNode{
public:
    // 将一个普通Node转成VariableDeclarationNode，并复制所有的信息。
    VariableDeclarationNode(Node *a){this->fullCopyFrom(a);}

    // 返回这个变量的类型。
    Node::Type getType();

    // 用来方便输出的。
    std::string getTypeString();

    // 计算这个变量的数据宽度。
    // 通常，int的宽度是1、数组的宽度是单个宽度乘以数组大小。
    int getSize();
    virtual std::string codeGen();
};

// 函数定义节点，没有逻辑上的孩子。codeGen()也是到此不递归。
// 创建方法：
// 1. 首先建立 AttributivedNode 以维护树结构
// 2. 用构造函数 FunctionDeclarationNode(Node *) 将刚建立的 AttributivedNode 转成 FunctionDeclarationNode
class FunctionDeclarationNode : public AttributivedNode{
public:
    // 将一个普通Node转成FunctionDeclarationNode，并复制所有的信息。
    FunctionDeclarationNode(Node *c):AttributivedNode(){this->fullCopyFrom(c);}
    virtual std::string codeGen();
    int getSize();
};

// 表达式节点。
// 创建方法：
// 1. 首先建立 AttributivedNode 以维护树结构
// 2. 用构造函数 ExpressionNode(Node *) 将刚建立的 AttributivedNode 转成 ExpressionNode
class ExpressionNode : public AttributivedNode{
public:
    ExpressionNode(std::string _symbolName, int childrenNumber, ...);
    ExpressionNode(Node *c):AttributivedNode(){this->fullCopyFrom(c);}

    // 用于直接取得这个表达式对应的变量名。
    // 如 a[i]，我们认为它的变量名仍然是 a，但是顶层只能看到一个算符 "[]" 以及它的两个孩子 "a" 和 "i" 
    std::string getArgumentVariableName();
    virtual std::string codeGen();
public:
    int intValue;
    double doubleValue;
};

// 空语句
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

// 分支语句节点
class IfNode : public AttributivedNode{
public:
    IfNode(Node *c):AttributivedNode(){this->fullCopyFrom(c);}
    virtual std::string codeGen();
};

// For循环语句节点
class ForNode : public AttributivedNode{
public:
    ForNode(Node *c):AttributivedNode(){this->fullCopyFrom(c);}
    virtual std::string codeGen();
    bool boostable();
};

// While循环语句节点
class WhileNode : public AttributivedNode{
public:
    WhileNode(Node *c):AttributivedNode(){this->fullCopyFrom(c);}
    virtual std::string codeGen();
};

// return语句节点
class RetNode : public AttributivedNode{
public:
    RetNode(Node* c):AttributivedNode(){this->fullCopyFrom(c);}
    virtual std::string codeGen();
};

// Read语句节点
class ReadNode : public ExpressionNode{
public:
    ReadNode(std::string _symbolName, int childrenNumber, ...);
    virtual std::string codeGen();
};

// Write语句节点
class WriteNode : public ExpressionNode{
public:
    WriteNode(std::string _symbolName, int childrenNumber, ...);
    virtual std::string codeGen();
};

// 属性，是存在符号表里的、变量的属性。
struct Attribute{
    // 标识符
    std::string name;
    // 标识符类型，如int、struct等
    Node::Type type;
    // 标识符类别，比如变量、结构体名、函数名等
    Node::Kind kind;
    // 若是函数，这里存了函数参数的类型
    std::vector<Node::Type> argList;
    // 配合argList进一步存储参数的类型的结构体名
    std::vector<std::string> argListStructName;
    // 若是数组，则这个vector不空，而且还会存储每个维度的大小。
    std::vector<int> arraySizes;
    // 若是结构体，这里存了结构体的名字。
    std::string structTypeName;
    // 该变量将被安排在哪个地址里。
    int addr;
    // 该变量的数据宽度，通常来说，int的size是1、数组的size是(单个数据的size*数组大小)
    int size=-1;
    // 对于参数，这里用于标记参数的顺序；
    // 对于结构体成员，这里用于标记成员的偏移量
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

// 符号表
class SymbolTable{
private:
    std::string mSymbolTableName;
    std::map<std::string, Attribute*> map;
    // 所有的符号表都存在这里
    // 全局符号表的名字叫 Global_SymbolTable
    // 函数 f 的符号表的名字就叫 f （与函数名相同）
    // 结构体 node 的符号表的名字就叫 node （与结构体名字相同）
    static std::map<std::string, SymbolTable*> set;
public:
    SymbolTable();
    SymbolTable(std::string name);
    std::map<std::string, Attribute*> getTable();
    std::string getName();

    // 入表
    bool insert(Attribute* t);

    // 查表
    Attribute *lookUp(std::string name);

    // 查看这个符号表
    void print();

    // 取得某个符号表
    // 全局符号表的名字叫 Global_SymbolTable
    // 函数 f 的符号表的名字就叫 f （与函数名相同）
    // 结构体 node 的符号表的名字就叫 node （与结构体名字相同）
    static SymbolTable *getSymbolTableByName(std::string symbolTableName);

    // 所有的符号表都存在这里
    // 全局符号表的名字叫 Global_SymbolTable
    // 函数 f 的符号表的名字就叫 f （与函数名相同）
    // 结构体 node 的符号表的名字就叫 node （与结构体名字相同）
    static void viewAllSymbolTable();
};

//符号表栈
class SymbolTableStack{
private:
    std::vector<SymbolTable*> stack;
public:
    SymbolTableStack(SymbolTable *globalSymbolTable);
    void push(SymbolTable* t);
    void pop();
    SymbolTable *top();

    // 查表，从栈顶一层层向下查
    Attribute *lookUp(std::string name);

    // 临时以符号表tName为栈顶，查表
    // 若tName为空字符串，则等价于 lookUp(name)
    Attribute *lookUp(std::string name, std::string tName);

    // 入表，入到栈顶的表里。
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
