# -c---
浙江大学《编译原理》课程实验：c--编译器

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
