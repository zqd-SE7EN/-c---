# -c---
浙江大学《编译原理》课程实验：c--编译器

生成三地址码，实现三地址码解释器，使用三地址码解释器运行三地址码。

已实现结构体。

已通过所有测试。

新加了对while和前后加加减减的支持。
```
yacc -d cParser.y -o cParser.cpp
flex -o cScanner.cpp cScanner.l
cc cScanner.cpp cParser.cpp cCompilerCommon.cpp main.cpp runner.cpp -o compiler -lm -lstdc++
cc runner.cpp runnerMain.cpp -o runner -lm -lstdc++

```
```
./compiler < ./tests/test1.c > parsetree.txt
./runner

```

### 三地址码格式：
```
Instruction:  Operation1 Source Source Destination | Operation2 Destination | RET;
Operation1: ADD|SUB|MUL|DIV|REM|SLT|BNE|BEQ ;
Operation2: CALL|READ|WRITE|PUSH|POP ;
Source: '[' NUMBER ']' | '[' TempVar ']' | NUMBER | TempVar ;
Destination: '[' NUMBER ']' | TempVar ;
TempVar: '`' 't' '[' NUMBER ']' ;

NUMBER: [0-9]+ ;
```
> `Source: NUMBER` 指的是立即数，而被 `[]` 包裹的 `NUMBER` 则指的是地址，即 `[]` 的意思是，从地址中取值。
> `TempVar` 是用来存放中间结果的临时变量们。
