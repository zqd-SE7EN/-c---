#include "./cCompilerCommon.hpp"
#include "./runner.hpp"
extern Node *makeParseTree();
extern ProgramRam program;
int main(){
    csLineCnt++;
    try{
        Node *treeRoot = makeParseTree();
        if(treeRoot){
            treeRoot->printTree(0);
            treeRoot->simplify();
            std::cout<<"\n\nThe simplified tree is:\n\n";
            treeRoot->printTree(0);
        }else{
            return 0;
        }
        std::cout<<"Original SymbolTable:\n\n";
        SymbolTable::viewAllSymbolTable();
        treeRoot->codeGen();
        std::cout<<"\n\nGenerated:\n\n";
        SymbolTable::viewAllSymbolTable();
        program.push({"NULL","","",""});
        program.print(ofstream("midcode.m"));

        Runner runner(program);
    }catch(char const *s){
        std::cout<<s;
    }
    return 0;
}
