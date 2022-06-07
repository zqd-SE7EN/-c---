#include "./cCompilerCommon.hpp"
#include "./runner.hpp"
int csLineCnt = 0;
int csColumnCnt = 0;

AddressCounter* addressCounter = new AddressCounter();

std::map<std::string, SymbolTable*> SymbolTable::set;

SymbolTableStack *symbolTableStack = new SymbolTableStack(new SymbolTable("Global_SymbolTable"));

bool checkType(Node *p, Node::Type type){
    return p->getType()==type;
}
bool checkKind(Node *p, Node::Kind kind){
    return p->getKind()==kind;
}
bool typeMatch(Node *a, Node *b){
    if(a->isArray()||b->isArray()){
        return false;
    }
    if(a->getType()==b->getType()){
        if(a->getType()!=Node::TYPE_STRUCT)return true;
        else return a->getStructTypeName().compare(b->getStructTypeName())==0;
    }
    if(a->getType()==Node::TYPE_INT){
        return b->getType()==Node::TYPE_DOUBLE;
    }
    if(a->getType()==Node::TYPE_DOUBLE){
        return b->getType()==Node::TYPE_INT;
    }
    return false;
}
bool typeMatch(std::vector<Node::Type> a, std::vector<Node::Type> b){
    if(a.size()!=b.size())return false;
    for(int i=0;i<a.size();i++){
        if(a[i]==b[i])continue;
        if(a[i]==Node::TYPE_INT && b[i]==Node::TYPE_DOUBLE)continue;
        if(b[i]==Node::TYPE_INT && a[i]==Node::TYPE_DOUBLE)continue;
        return false;
    }

    return true;
}
bool typeMatch(std::vector<Node::Type> a,Node *c , std::vector<std::string> s){
    auto b=c->getArgList();
    if(a.size()!=b.size())return false;
    for(int i=0;i<a.size();i++){
        if(a[i]==b[i]){
            if(a[i]!=Node::TYPE_STRUCT)continue;
            else if(c->getArgListStructName()[i].compare(s[i])!=0)return false;
            else continue;
        }
        if(a[i]==Node::TYPE_INT && b[i]==Node::TYPE_DOUBLE)continue;
        if(b[i]==Node::TYPE_INT && a[i]==Node::TYPE_DOUBLE)continue;
        return false;
    }

    return true;
}
bool typeMatch(Attribute *a, Node* b){
    if(b->isArray())return false;
    if(a->type==b->getType()){
        if(a->type==Node::TYPE_STRUCT){
            return a->structTypeName.compare(b->getStructTypeName())==0;
        }else return true;
    }
    if((a->type==Node::TYPE_INT && b->getType()==Node::TYPE_DOUBLE)||(a->type==Node::TYPE_DOUBLE && b->getType()==Node::TYPE_INT))return true;
    return false;
}
std::string type_to_string(Attribute *t){
    switch(t->type){
        case(Node::TYPE_VOID):
            return "void";
        case(Node::TYPE_INT):
            return "int";
        case(Node::TYPE_DOUBLE):
            return "double";
        case(Node::TYPE_STRUCT):
            return std::string("struct ")+t->structTypeName;
    }
}

void Node::setAttribute(void *p){
    auto c = (Attribute*)p;
    this->setType(c->type);
    this->setKind(c->kind);
    this->setArgList(c->argList);
    this->setArgListStructName(c->argListStructName);
    this->setArraySizes(c->arraySizes);
    this->setStructTypeName(c->structTypeName);
    this->setVariableName(c->name);
    this->setPosition(c->lineNumber, c->columnNumber);
}

void Node::copyFrom(Attribute *c){
    if(!c)return;
    this->setType(c->type);
    this->setKind(c->kind);
    this->setArgList(c->argList);
    this->setArraySizes(c->arraySizes);
    this->setStructTypeName(c->structTypeName);
    this->setVariableName(c->name);
    this->setPosition(c->lineNumber, c->columnNumber);
}

void Node::fullCopyFrom(Node *c){
    mSymbolName = c->mSymbolName;
    mTokenValue = c->mTokenValue; //(token string)
    mIsTerminal = c->mIsTerminal;
    mIsNegligible = c->mIsNegligible;
    mChildren = c->mChildren;
    if(dynamic_cast<AttributivedNode*>(c)!=NULL){
        this->setType(c);
        this->setKind(c->getKind());
        this->setArgList(c->getArgList());
        this->setArgListStructName(c->getArgListStructName());
        this->setArraySizes(c->getArraySizes());
        this->setStructTypeName(c->getStructTypeName());
        this->setVariableName(c->getVariableName());
        this->setPosition(c->getLineNumber(), c->getColumnNumber());
    }
}

Node::Node(std::string _symbolName, int childrenNumber, ...):mIsNegligible(false),mSymbolName(_symbolName),mIsTerminal(false),mTokenValue("I am not a terminal."){
    va_list vl;
    va_start(vl, childrenNumber);
    for(int i=0;i<childrenNumber;i++){
        mChildren.push_back(va_arg(vl,Node*));
    }
}
void Node::addChild(Node *newChild){
    mChildren.push_back(newChild);
}
Node* Node::getChildrenById(int i){
    if(i>=mChildren.size())return NULL;
    return mChildren[i];
}
int Node::getChildrenNumber(){
    return mChildren.size();
}
std::vector<Node*> Node::getChildren(){
    return mChildren;
}
bool Node::isTerminal()const{
    return mIsTerminal;
}
bool Node::isNegligible(){
    return mIsTerminal && mIsNegligible;
}
std::string Node::getSymbolName()const{
    return this->mSymbolName;
}
std::string Node::getTokenValue(){
    if(!(this->mIsTerminal)){
        return getSymbolName();
    }
    return this->mTokenValue;
}
std::string Node::getName()const{
    return mIsTerminal?mTokenValue:mSymbolName;
}
void Node::printTree(int depth){
    for(int i=0;i<depth;i++){
        std::cout<<"    ";
    }
    std::cout<<this->getName()<<std::endl;
    for(int i=0;i<mChildren.size();i++){
        mChildren[i]->printTree(depth+1);
    }
}
void Node::simplify(){
    if(mIsTerminal)return;
    for(int i=0;i<mChildren.size();i++){
        if(mChildren[i]->isNegligible()){
            delete mChildren[i];
            mChildren.erase(mChildren.begin()+i, mChildren.begin()+i+1);
            i--;
        }
    }
    for(auto child : mChildren){
        child->simplify();
    }
}
void Node::copyFromChild(){
    this->setType(mChildren[0]->getType());
    this->setKind(mChildren[0]->getKind());
    this->setArgList(mChildren[0]->getArgList());
    this->setArgListStructName(mChildren[0]->getArgListStructName());
    this->setArraySizes(mChildren[0]->getArraySizes());
    this->setStructTypeName(mChildren[0]->getStructTypeName());
    this->setVariableName(mChildren[0]->getVariableName());
    this->setPosition(mChildren[0]->getLineNumber(), mChildren[0]->getColumnNumber());
}
void Node::copyFrom(Node *c){
    this->setType(c);
    this->setKind(c->getKind());
    this->setArgList(c->getArgList());
    this->setArgListStructName(c->getArgListStructName());
    this->setArraySizes(c->getArraySizes());
    this->setStructTypeName(c->getStructTypeName());
    this->setVariableName(c->getVariableName());
    this->setPosition(c->getLineNumber(), c->getColumnNumber());
}

AttributivedNode::AttributivedNode(std::string _symbolName, int childrenNumber, ...):Node(_symbolName,0){
    va_list vl;
    va_start(vl, childrenNumber);
    for(int i=0;i<childrenNumber;i++){
        mChildren.push_back(va_arg(vl,Node*));
    }
    mIsNegligible=(false),mSymbolName=(_symbolName),mIsTerminal=(false),mTokenValue=("I am not a terminal.");
}
void AttributivedNode::setType(AttributivedNode::Type _type){
    this->mTokenType = _type;
}
void AttributivedNode::setType(Node *c){
    this->setType(c->getType());
    if(c->getType()==Node::TYPE_STRUCT){
        this->setStructTypeName(c->getStructTypeName());
    }
}
AttributivedNode::Type AttributivedNode::getType(){
    return this->mTokenType;
}
std::string AttributivedNode::getTypeString(){
    std::string string;
    switch(this->mTokenType){
        case(Node::TYPE_DOUBLE):
            string+={"double"};
            break;
        case(Node::TYPE_INT):
            string+={"int"};
            break;
        case(Node::TYPE_STRUCT):
            string+=(std::string("struct ")+this->mStructTypeName);
            break;
        default :
            string+=std::to_string(this->mTokenType);
    }
    for(int i=0;i<this->mArraySizes.size();i++){
        string+="[]";
    }
    return string;
}
void AttributivedNode::setKind(AttributivedNode::Kind _kind){
    this->mTokenKind = _kind;
}
AttributivedNode::Kind AttributivedNode::getKind(){
    return this->mTokenKind;
}
void AttributivedNode::setArgList(std::vector<AttributivedNode::Type> _argList){
    mTokenArgList.assign(_argList.begin(),_argList.end());
}
std::vector<AttributivedNode::Type> AttributivedNode::getArgList(){
    return this->mTokenArgList;
}
void AttributivedNode::setArgListStructName(std::vector<std::string> _structName){
    mTokenArgListStructTypeName.assign(_structName.begin(), _structName.end());
}
std::vector<std::string> AttributivedNode::getArgListStructName(){
    return mTokenArgListStructTypeName;
}
void AttributivedNode::setArraySizes(std::vector<int> _sizes){
    mArraySizes.assign(_sizes.begin(),_sizes.end());
}
std::vector<int> AttributivedNode::getArraySizes(){
    return mArraySizes;
}
bool AttributivedNode::isArray(){
    return mArraySizes.size()>0;
}
int AttributivedNode::getArrayDimension(){
    return mArraySizes.size();
}
void AttributivedNode::setStructTypeName(std::string _name){
    mStructTypeName = _name;
}
std::string AttributivedNode::getStructTypeName(){
    return mStructTypeName;
}
void AttributivedNode::setVariableName(std::string _name){
    this->mVariableName = _name;
}
std::string AttributivedNode::getVariableName(){
    return mVariableName;
}
void AttributivedNode::setPosition(int l,int c){
    mLineNumber = l;
    mColumnNumber = c;
}
int AttributivedNode::getLineNumber(){
    return mLineNumber;
}
int AttributivedNode::getColumnNumber(){
    return mColumnNumber;
}
void AttributivedNode::setPosition(Node *c){
    mLineNumber = c->getLineNumber();
    mColumnNumber = c->getColumnNumber();
}

Node::Type StructDeclarationNode::getType(){
    return Node::TYPE_STRUCT;
}
int StructMemberNode::getSize(){
    int arraySize = 1;
    if(isArray()){
        for(int i : this->getArraySizes()){
            arraySize *= i;
        }
    }
    int singleSize = 1;
    return arraySize * singleSize;
}
Node::Type VariableDeclarationStatementNode::getType(){
    return mChildren[0]->getType();
}
std::string VariableDeclarationStatementNode::getTypeString(){
    return mChildren[0]->getTypeString();
}
Node::Type VariableDeclarationNode::getType(){
    return AttributivedNode::getType();
}
std::string VariableDeclarationNode::getTypeString(){
    return AttributivedNode::getTypeString();
}
ExpressionNode::ExpressionNode(std::string _symbolName, int childrenNumber, ...):AttributivedNode(_symbolName,0){
    va_list vl;
    va_start(vl, childrenNumber);
    for(int i=0;i<childrenNumber;i++){
        mChildren.push_back(va_arg(vl,Node*));
    }
    mIsNegligible=(false),mSymbolName=(_symbolName),mIsTerminal=(false),mTokenValue=("I am not a terminal.");
}
std::string ExpressionNode::getArgumentVariableName(){
    if(this->getVariableName().compare(".")==0) {
        return this->mChildren[1]->getVariableName();
    }
    else return this->getVariableName();
}
ReadNode::ReadNode(std::string _symbolName, int childrenNumber, ...):ExpressionNode(_symbolName,0){
    va_list vl;
    va_start(vl, childrenNumber);
    for(int i=0;i<childrenNumber;i++){
        mChildren.push_back(va_arg(vl,Node*));
    }
    mIsNegligible=(false),mSymbolName=(_symbolName),mIsTerminal=(false),mTokenValue=("I am not a terminal.");
}
WriteNode::WriteNode(std::string _symbolName, int childrenNumber, ...):ExpressionNode(_symbolName,0){
    va_list vl;
    va_start(vl, childrenNumber);
    for(int i=0;i<childrenNumber;i++){
        mChildren.push_back(va_arg(vl,Node*));
    }
    mIsNegligible=(false),mSymbolName=(_symbolName),mIsTerminal=(false),mTokenValue=("I am not a terminal.");
}