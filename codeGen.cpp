#pragma once
#include "./cCompilerCommon.hpp"
#include "./runner.hpp"

static int forexpandexp=1;
ProgramRam program;
NameCounter programNameCounter;
std::string computeArrayOffset(ExpressionNode *c);

int VariableDeclarationNode::getSize(){
    
    if(this->mTokenType==Node::TYPE_VOID){
        return 1;
    }
    int arraySize = 1;
    if(isArray()){
        for(int i : this->getArraySizes()){
            arraySize *= i;
        }
    }
    int singleSize = 1;
    if(getType()==Node::TYPE_STRUCT){
        singleSize = symbolTableStack->lookUp(this->getStructTypeName())->size;
    }
    return arraySize * singleSize;
}

int StructDeclarationNode::getSize(){
    if(mSizeSum!=-1)return mSizeSum;
    int sizeSum = 0;
    for(auto record : (SymbolTable::getSymbolTableByName(mChildren[1]->getStructTypeName()))->getTable()){
        auto attribute = record.second;
        attribute->offset = sizeSum;
        if(attribute->type == Node::TYPE_STRUCT){
            attribute->size = symbolTableStack->lookUp(attribute->structTypeName)->size;
        }else{
            attribute->size = 1;
        }
        if(attribute->arraySizes.size()!=0){
            for(int i:attribute->arraySizes){
                attribute->size *= i;
            }
        }
        sizeSum += attribute->size;
    }
    return mSizeSum=sizeSum;
}

std::string StructDeclarationNode::codeGen(){
    std::cout<<this->getName()<<std::endl;
    auto attribute = symbolTableStack->lookUp(mChildren[1]->getStructTypeName());
    if(attribute==NULL){
        exit(1);
    }
    attribute->size = getSize();
    std::cout<<this->getName()<<" end"<<std::endl;
    return "`";
}

std::string VariableDeclarationStatementNode::codeGen(){
    std::cout<<this->getName()<<std::endl;
    auto variables = mChildren[1]->getChildren();
    std::stringstream ss;
    for(auto variable : variables){
        dynamic_cast<VariableDeclarationNode*>(variable)->codeGen();
    }
    return "`";
}

std::string VariableDeclarationNode::codeGen(){
    std::cout<<this->getName()<<std::endl;
    auto attribute = symbolTableStack->lookUp(this->getVariableName());
    assert(attribute!=NULL);
    attribute->size = getSize();
    attribute->addr = addressCounter->getNextNAddr(getSize());
    return "`";
}

int FunctionDeclarationNode::getSize(){
    std::cout<<this->getName()<<std::endl;
    VariableDeclarationNode t(this);
    return t.getSize();
}

std::string FunctionDeclarationNode::codeGen(){
    std::cout<<this->getName()<<std::endl;
    auto attribute = symbolTableStack->lookUp(this->getVariableName());
    assert(attribute!=NULL);
    attribute->size = getSize();
    attribute->addr = addressCounter->getNextNAddr(getSize());
    auto table = SymbolTable::getSymbolTableByName(this->getVariableName());
    for(auto record : table->getTable()){
        AttributivedNode s("t",0);
        s.copyFrom(record.second);
        VariableDeclarationNode t(&s);
        record.second->size = t.getSize();
        record.second->addr = addressCounter->getNextNAddr(t.getSize());
    }
    program.labelNextLine(this->getVariableName());
    symbolTableStack->push(SymbolTable::getSymbolTableByName(this->getVariableName()));
    return "`";
}

std::string getAddr(Node *c){
    std::stringstream ss;
    while(c->getName().compare("[]")==0)c=c->getChildrenById(0);
    if(c->getChildrenNumber()==0){
        auto attribute = symbolTableStack->lookUp(c->getVariableName());
        assert(attribute!=NULL);
        ss<<attribute->addr;
        return ss.str();
    }else if(c->getName().compare(".")){
        std::string res = c->codeGen();
        return res.substr(1,res.length()-2);// TODO
    }else if(c->getName().compare("[]")){
        std::string res = c->codeGen();
        return res.substr(1,res.length()-2);// TODO
    }
}


std::string getOffsetAddr(std::string symbolTableName, Node *c){
    std::string res;
    Node *p=c;
    while(p->getChildrenNumber()!=0)p=p->getChildrenById(0);
    auto attribute = SymbolTable::getSymbolTableByName(symbolTableName)->lookUp(p->getTokenValue());
    assert(attribute!=NULL);
    std::stringstream ss;
    ss<<attribute->offset;
    if(c->getName().compare("[]")==0){
        std::string offset = computeArrayOffset(dynamic_cast<ExpressionNode*>(c));
        program.push({"ADD", ss.str(), offset, res=programNameCounter.getNumberedName("`t")});
        return res;
    }else if(c->getName().compare(".")==0){
        res = c->codeGen();
        return res.substr(1,res.length()-2);
    }
    return ss.str();
}

std::string computeArrayOffset(ExpressionNode *c){
    ExpressionNode *p = c;
    while(p->getVariableName().compare("[]")==0){
        p = dynamic_cast<ExpressionNode*>(p->getChildrenById(0));
        assert(p!=NULL);
    }
    auto attribute = symbolTableStack->lookUp(p->getArgumentVariableName(), p->getStructTypeName());
    assert(attribute!=NULL);
    auto &arraySizes = attribute->arraySizes;

    std::string res, t, tober;
    res = c->getChildrenById(1)->codeGen();
    c=dynamic_cast<ExpressionNode*>(c->getChildrenById(0));
    int sizeMul = arraySizes[arraySizes.size()-1];
    for(int i=arraySizes.size()-2; i>=0; i--){
        std::string midRes;
        std::stringstream ss;
        ss<<sizeMul;
        program.push({"MUL", ss.str(), tober=c->getChildrenById(1)->codeGen(), midRes=programNameCounter.getNumberedName("`t")});
        programNameCounter.releaseName(tober);
        program.push({"ADD", midRes, res, t=programNameCounter.getNumberedName("`t")});
        programNameCounter.releaseName(midRes);
        programNameCounter.releaseName(res);
        res = t;
        sizeMul *= arraySizes[i];
        c=dynamic_cast<ExpressionNode*>(c->getChildrenById(0));
    }
    std::stringstream ss;
    ss<<(attribute->size/sizeMul);
    program.push({"MUL", ss.str(), res, t=programNameCounter.getNumberedName("`t")}); /* for struct. */
    programNameCounter.releaseName(ss.str());
    programNameCounter.releaseName(res);
    return t;
}

std::string ExpressionNode::codeGen(){
    std::cout<<this->getName()<<std::endl;
    std::string res, t1, t2;
    if(mChildren.size()==0){ // is terminal
        if(this->getKind()==Node::KIND_VARIABLE){
            auto attribute = symbolTableStack->lookUp(this->getVariableName());
            assert(attribute!=NULL);
            std::stringstream ss;
            ss<<"["<<attribute->addr<<"]";
            return ss.str();
        }else if(this->getType()==Node::TYPE_INT){
            sscanf(mTokenValue.c_str(),"%d",&this->intValue);
            return {mTokenValue};
        }else if(this->getType()==Node::TYPE_DOUBLE){
            sscanf(mTokenValue.c_str(),"%lf",&this->doubleValue);
            return {mTokenValue};
        }else if(this->getType()==Node::TYPE_CHAR){
            this->intValue = (int)mTokenValue.c_str()[1];
            if(mTokenValue.c_str()[1]=='\\'){
                this->intValue = '\n';
            }
            std::stringstream ss;
            ss<<this->intValue;
            return ss.str();
        }
    }else if(this->getName().length()==1){ /*            + - * / % = > < .         */
        switch(this->getName()[0]){
            case '+':
                program.push({"ADD", t1=this->mChildren[0]->codeGen(), t2=this->mChildren[1]->codeGen(), res=programNameCounter.getNumberedName("`t")});
                programNameCounter.releaseName(t1);
                programNameCounter.releaseName(t2);
                return res;
            case '-':
                program.push({"SUB", t1=this->mChildren[0]->codeGen(), t2=this->mChildren[1]->codeGen(), res=programNameCounter.getNumberedName("`t")});
                programNameCounter.releaseName(t1);
                programNameCounter.releaseName(t2);
                return res;
            case '*':
                program.push({"MUL", t1=this->mChildren[0]->codeGen(), t2=this->mChildren[1]->codeGen(), res=programNameCounter.getNumberedName("`t")});
                programNameCounter.releaseName(t1);
                programNameCounter.releaseName(t2);
                return res;
            case '/':
                program.push({"DIV", t1=this->mChildren[0]->codeGen(), t2=this->mChildren[1]->codeGen(), res=programNameCounter.getNumberedName("`t")});
                programNameCounter.releaseName(t1);
                programNameCounter.releaseName(t2);
                return res;
            case '%':
                program.push({"REM", t1=this->mChildren[0]->codeGen(), t2=this->mChildren[1]->codeGen(), res=programNameCounter.getNumberedName("`t")});
                programNameCounter.releaseName(t1);
                programNameCounter.releaseName(t2);
                return res;
            case '>':
                program.push({"SLT", t1=this->mChildren[1]->codeGen(), t2=this->mChildren[0]->codeGen(), res=programNameCounter.getNumberedName("`t")});
                programNameCounter.releaseName(t1);
                programNameCounter.releaseName(t2);
                return res;
            case '<':
                program.push({"SLT", t1=this->mChildren[0]->codeGen(), t2=this->mChildren[1]->codeGen(), res=programNameCounter.getNumberedName("`t")});
                programNameCounter.releaseName(t1);
                programNameCounter.releaseName(t2);
                return res;
            case '=':
                program.push({"ADD", "0", t1=this->mChildren[1]->codeGen(), t2=this->mChildren[0]->codeGen()});
                programNameCounter.releaseName(t1);
                programNameCounter.releaseName(t2);
                return "`";
            case '.':
                if(mChildren[0]->getName().compare("[]")){
                    std::string baseAddr = getAddr(mChildren[0]);
                    std::string offsetAddr = getOffsetAddr(mChildren[0]->getStructTypeName(), mChildren[1]);
                    
                    program.push({"ADD", baseAddr, offsetAddr, res=programNameCounter.getNumberedName("`t")});
                    return std::string("[")+res+std::string("]");
                }else{
                    std::string baseAddr = mChildren[0]->codeGen();
                    baseAddr=baseAddr.substr(1,baseAddr.length()-2);
                    std::string offsetAddr = getOffsetAddr(mChildren[0]->getStructTypeName(), mChildren[1]);
                    program.push({"ADD", baseAddr, offsetAddr, res=programNameCounter.getNumberedName("`t")});
                    return std::string("[")+res+std::string("]");
                }
        }
    }else if(this->getName().length()==2){/*      != == >= <= ++ --  []   || &&  ()                          */
        if(this->getName().compare({"=="}) == 0){
            std::string subRes;
            res=programNameCounter.getNumberedName("`t");
            program.push({"SUB", t1=this->mChildren[0]->codeGen(), t2=this->mChildren[1]->codeGen(), subRes=programNameCounter.getNumberedName("`t")});
            program.push({"ADD", "0", "0", res});
            program.push({"BNE", subRes, "0", "1"});
            programNameCounter.releaseName(subRes);
            program.push({"ADD", "1", "0", res});
            programNameCounter.releaseName(t1);
            programNameCounter.releaseName(t2);
            return res;
        }else if(this->getName().compare({">="})==0){
            std::string subRes;
            res=programNameCounter.getNumberedName("`t");
            program.push({"SUB", t1=this->mChildren[0]->codeGen(), t2=this->mChildren[1]->codeGen(), subRes=programNameCounter.getNumberedName("`t")});
            program.push({"SLT", "-1", subRes, res});
            programNameCounter.releaseName(t1);
            programNameCounter.releaseName(t2);
            programNameCounter.releaseName(subRes);
            return res;
        }else if(this->getName().compare({"<="})==0){
            std::string subRes;
            res=programNameCounter.getNumberedName("`t");
            program.push({"SUB", t1=this->mChildren[1]->codeGen(), t2=this->mChildren[0]->codeGen(), subRes=programNameCounter.getNumberedName("`t")});
            program.push({"SLT", "-1", subRes, res});
            programNameCounter.releaseName(t1);
            programNameCounter.releaseName(t2);
            programNameCounter.releaseName(subRes);
            return res;
        }else if(this->getName().compare({"||"})==0){
            std::string subRes;
            res=programNameCounter.getNumberedName("`t");
            program.push({"ADD", t1=this->mChildren[1]->codeGen(), t2=this->mChildren[0]->codeGen(), subRes=programNameCounter.getNumberedName("`t")});
            program.push({"ADD", "0", "0", res});
            program.push({"BEQ", subRes, "0", "1"});
            programNameCounter.releaseName(subRes);
            program.push({"ADD", "1", "0", res});
            programNameCounter.releaseName(t1);
            programNameCounter.releaseName(t2);
            return res;
        }else if(this->getName().compare({"&&"})==0){
            std::string subRes;
            res=programNameCounter.getNumberedName("`t");
            program.push({"ADD", t1=this->mChildren[1]->codeGen(), t2=this->mChildren[0]->codeGen(), subRes=programNameCounter.getNumberedName("`t")});
            program.push({"ADD", "0", "0", res});
            program.push({"BNE", subRes, "2", "1"});
            programNameCounter.releaseName(subRes);
            program.push({"ADD", "1", "0", res});
            programNameCounter.releaseName(t1);
            programNameCounter.releaseName(t2);
            return res;
        }else if(this->getName().compare({"!="})==0){
            std::string subRes;
            res=programNameCounter.getNumberedName("`t");
            program.push({"SUB", t1=this->mChildren[0]->codeGen(), t2=this->mChildren[1]->codeGen(), subRes=programNameCounter.getNumberedName("`t")});
            program.push({"ADD", "0", "0", res});
            program.push({"BEQ", subRes, "0", "1"});
            programNameCounter.releaseName(subRes);
            program.push({"ADD", "1", "0", res});
            programNameCounter.releaseName(t1);
            programNameCounter.releaseName(t2);
            return res;
        }else if(this->getName().compare({"[]"})==0){
            std::string subRes;
            std::string baseAddr = getAddr(mChildren[0]);
            program.push({"ADD", t1=baseAddr, t2=computeArrayOffset(this), res=programNameCounter.getNumberedName("`t")});
            programNameCounter.releaseName(t1);
            programNameCounter.releaseName(t2);
            return std::string("[") + res + std::string("]");
        }else if(this->getName().compare({"()"})==0){
            auto table = SymbolTable::getSymbolTableByName(this->mChildren[0]->getVariableName())->getTable();
            int retAddr = symbolTableStack->lookUp(this->mChildren[0]->getVariableName())->addr;
            if(this->mChildren.size()==1){
                program.push({"CALL", this->mChildren[0]->getVariableName(), "NULL", "NULL"});
                std::stringstream ss;
                ss<<"["<<retAddr<<"]";
                return ss.str();
            }else{
                auto argValueExpressions = this->mChildren[1]->getChildren();
                int argCnt = 0;
                std::stack<std::string> s;
                while(true){
                    int f=true;
                    for(auto i : table){
                        if(i.second->offset == argCnt){
                            f=false;
                            std::stringstream ss;
                            ss<<"["<<i.second->addr<<"]";
                            program.push({"PUSH", ss.str(), "", ""});
                            s.push(ss.str());
                            argCnt++;
                            break;
                        }
                    }
                    if(f)break;
                }
                for(auto i : table){
                    if(i.second->offset == -1){
                        std::stringstream ss;
                        ss<<"["<<i.second->addr<<"]";
                        program.push({"PUSH", ss.str(), "", ""});
                        s.push(ss.str());
                    }
                }
                argCnt=0;
                /* chuan can shu */
                for(auto expression : argValueExpressions){
                    std::string argValue;
                    argValue = expression->codeGen();
                    for(auto i : table){
                        if(i.second->offset == argCnt){
                            std::stringstream ss;
                            ss<<"["<<i.second->addr<<"]";
                            program.push({"ADD", "0", argValue, ss.str()});
                            programNameCounter.releaseName(argValue);
                            argCnt++;
                            break;
                        }
                    }
                }
                program.push({"CALL", this->mChildren[0]->getVariableName(), "NULL", "NULL"});
                while(!s.empty()){
                    program.push({"POP", s.top(), "", ""});
                    s.pop();
                }
                std::stringstream ss;
                ss<<"["<<retAddr<<"]";
                return ss.str();
            }
        }
    }else {
        if(this->getName().compare("pre++")==0){ // ++a
            program.push({"ADD", this->mChildren[0]->codeGen(), "1", this->mChildren[0]->codeGen()});
            return this->mChildren[0]->codeGen();
        }else if(this->getName().compare("pre--")==0){ // --a
            program.push({"SUB", this->mChildren[0]->codeGen(), "1", this->mChildren[0]->codeGen()});
            return this->mChildren[0]->codeGen();
        }else if(this->getName().compare("post++")==0){ // a++
            program.push({"ADD", this->mChildren[0]->codeGen(), "0", res=programNameCounter.getNumberedName("`t")});
            program.push({"ADD", this->mChildren[0]->codeGen(), "1", this->mChildren[0]->codeGen()});
            return res;
        }else if(this->getName().compare("post--")==0){ // a--
            program.push({"ADD", this->mChildren[0]->codeGen(), "0", res=programNameCounter.getNumberedName("`t")});
            program.push({"SUB", this->mChildren[0]->codeGen(), "1", this->mChildren[0]->codeGen()});
            return res;
        }
    }
}

std::string IfNode::codeGen(){
    std::cout<<this->getName()<<std::endl;
    Node *condition = this->mChildren[1];
    Node *thenBlock = this->mChildren[2];
    Node *elseBlock = this->getChildrenById(4);
    std::string conditionResult;
    std::string thenBlockLabel = programNameCounter.getNumberedName("IfConditionIsTrue");
    std::string elseBlockLabel = programNameCounter.getNumberedName("IfConditionIsFalse");
    std::string arterBlockLabel = programNameCounter.getNumberedName("IfStatementEnd");
    /*condition*/
    conditionResult=this->mChildren[1]->codeGen();
    program.push({"BEQ", "0", conditionResult, elseBlockLabel});
    programNameCounter.releaseName(conditionResult);
    /*then*/
    thenBlock->codeGen();
    program.push({"BEQ", "0", "0", arterBlockLabel});
    /*else*/
    program.labelNextLine(elseBlockLabel);
    if(elseBlock){
        elseBlock->codeGen();
    }
    program.labelNextLine(arterBlockLabel);
    return "`";
}

std::string ForNode::codeGen(){
    std::cout<<this->getName()<<std::endl;
    int K=4;
    Node *kNode = new ExpressionNode(new AttributivedNode("4"));
    kNode->setVariableName("4");
    kNode->setType(Node::TYPE_INT);
    kNode->setKind(Node::KIND_CONSTANT);
    Node *initialExpressions = this->mChildren[1];
    Node *conditionExpresstion = this->mChildren[2]->getChildrenById(0);
    Node *tailExpression = this->mChildren[3];
    Node *loopBody = this->mChildren[4];
    std::string tober;
    std::string conditionResult;
    if(!this->boostable()){
        std::string loopStartLabel = programNameCounter.getNumberedName("LoopStart");
        std::string afterLoopLabel = programNameCounter.getNumberedName("LoopEnd");
        tober=initialExpressions->codeGen();
        programNameCounter.releaseName(tober);

        program.labelNextLine(loopStartLabel);
        if(dynamic_cast<EmptyNode*>(conditionExpresstion)==NULL){
            conditionResult = conditionExpresstion->codeGen();
            program.push({"BEQ", "0", conditionResult, afterLoopLabel});
            programNameCounter.releaseName(conditionResult);
        }

        loopBody->codeGen();
        tober=tailExpression->codeGen();
        programNameCounter.releaseName(tober);
        program.push({"BEQ", "0", "0", loopStartLabel});
        program.labelNextLine(afterLoopLabel);
        return "`";
    }else{
        forexpandexp*=K;
        std::string loopStartLabelK = programNameCounter.getNumberedName("LoopStartK");
        std::string loopStartLabel1 = programNameCounter.getNumberedName("LoopStart1");
        std::string afterLoopLabel = programNameCounter.getNumberedName("LoopEnd");
        tober=initialExpressions->codeGen();
        programNameCounter.releaseName(tober);

        program.labelNextLine(loopStartLabelK);
        Node *leftHand = conditionExpresstion->getChildrenById(0);
        Node *rightHand = conditionExpresstion->getChildrenById(1);
        
        Node *addNode = new ExpressionNode(new Node("+",2,leftHand,kNode));
        Node *BENode = new ExpressionNode (new Node("<",2,addNode,rightHand));
        conditionResult = BENode->codeGen();
        program.push({"BEQ", "0", conditionResult, loopStartLabel1});
        programNameCounter.releaseName(conditionResult);

        for(int i=0;i<K;i++){
            loopBody->codeGen();
            tober=tailExpression->codeGen();
            programNameCounter.releaseName(tober);
        }
        program.push({"BEQ", "0", "0", loopStartLabelK});
        program.labelNextLine(loopStartLabel1);

        conditionResult = conditionExpresstion->codeGen();
        program.push({"BEQ", "0", conditionResult, afterLoopLabel});
        programNameCounter.releaseName(conditionResult);
        loopBody->codeGen();
        tober=tailExpression->codeGen();
        programNameCounter.releaseName(tober);
        program.push({"BEQ", "0", "0", loopStartLabel1});
        program.labelNextLine(afterLoopLabel);
        forexpandexp/=K;
        return "`";
    }
}

std::string WhileNode::codeGen(){
    std::cout<<this->getName()<<std::endl;
    ExpressionNode *conditionExpression = dynamic_cast<ExpressionNode*>(this->mChildren[1]);
    Node *whileBody = this->mChildren[2];

    std::string loopStartLabel = programNameCounter.getNumberedName("LoopStart");
    std::string afterLoopLabel = programNameCounter.getNumberedName("LoopEnd");
    std::string tober;

    program.labelNextLine(loopStartLabel);
    program.push({"BEQ", "0", tober=conditionExpression->codeGen(), afterLoopLabel});
    programNameCounter.releaseName(tober);

    whileBody->codeGen();
    program.push({"BEQ","0","0",loopStartLabel});
    program.labelNextLine(afterLoopLabel);

    return "`";
}

std::string RetNode::codeGen(){
    std::cout<<this->getName()<<std::endl;
    auto table = SymbolTable::getSymbolTableByName(symbolTableStack->top()->getName())->getTable();
    std::string tober;
    if(this->mChildren.size()==1){
        program.push({"RET","","",""});
    }else{
        std::stringstream ss;
        ss<<"["<<symbolTableStack->lookUp(symbolTableStack->top()->getName())->addr<<"]";
        program.push({"ADD", "0", tober=mChildren[1]->codeGen(), ss.str()});
        programNameCounter.releaseName(tober);
        program.push({"RET","","",""});
    }
    symbolTableStack->pop();
    return "0";
}

std::string ReadNode::codeGen(){
    std::cout<<this->getName()<<std::endl;
    std::string dst;
    dst = this->mChildren[0]->codeGen();
    std::string type = this->mChildren[1]->getChildrenById(0)->getTokenValue();
    if(type.compare("int")==0){
        program.push({"READ", dst, "INT", ""});
    }else if(type.compare("char")==0){
        program.push({"READ", dst, "CHAR", ""});
    }
    programNameCounter.releaseName(dst.substr(1,dst.length()-2));
    return "`";

}

std::string WriteNode::codeGen(){
    std::cout<<this->getName()<<std::endl;
    std::string dst;
    dst = this->mChildren[0]->codeGen();
    std::string type = this->mChildren[1]->getChildrenById(0)->getTokenValue();
    if(type.compare("int")==0){
        program.push({"WRITE", dst, "INT", ""});
    }else if(type.compare("char")==0){
        program.push({"WRITE", dst, "CHAR", ""});
    }
    programNameCounter.releaseName(dst.substr(1,dst.length()-2));
    return "`";
}


std::string StringNode::codeGen(){
    return this->mTokenValue;
}

bool ForNode::boostable(){
    if(forexpandexp>256)return false;
    Node *initialExpressions = this->mChildren[1];
    Node *conditionExpresstion = this->mChildren[2]->getChildrenById(0);
    Node *tailExpression = this->mChildren[3];
    Node *loopBody = this->mChildren[4];
    if(tailExpression->getName().compare("post++")!=0)return false;
    if(conditionExpresstion->getName().compare("<")!=0)return false;
    if(tailExpression->getChildrenById(0)->getName().compare(
            conditionExpresstion->getChildrenById(0)->getName()
            )!=0)return false;
    if(loopBody->checkIfModified(conditionExpresstion->getChildrenById(0)->getName()) || 
        loopBody->checkIfModified(conditionExpresstion->getChildrenById(1)->getName()))return false;
    return true;
}

bool Node::checkIfModified(std::string name){
    char *u=new char;// allign address.
    if(this->mChildren.size()==0)return false;
    if(this->getName().compare("=")==0 || this->getName().compare("post++")==0 || this->getName().compare("post--")==0){
        if(this->mChildren[0]->getName().compare(name)==0)return true;;
    }
    for(auto child : mChildren){
        if(child->checkIfModified(name))return true;
    }
    return false;
}
