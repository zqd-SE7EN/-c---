int courses[1000][4000];
int map[1000];
int is_passed[1000];
int course_name[50];

int GPA;
int GPAInt;
int GPADot;

int ch;
int num;
int index;
int total_credits;
int attempted_credits;
int passed_credits;
int remained_credits;
int total_score;
int field;
int credit;
int key;
int i;
int value;
int flag;
int j;
int k;
int trash;
int endf;
int endf2;

int printCources(int pi,int pj){
    for(pi=0;pi<num;pi=pi+1){
        for(pj=0;courses[pi][pj]!=0;pj=pj+1){
            write(courses[pi][pj],char);
        }
        write("\n",char);
    }
    return 0;
}

int putCourseName(int putI){
    for(putI=0;course_name[putI]!=0;putI=putI+1){
        write(course_name[putI],char);
    }
    return 0;
}

int main() {
    k=0;value = 0;GPA=0;GPAInt = 0;GPADot=0;i = 0;key = 0;credit = 0; field = 0; total_score = 0; remained_credits = 0; passed_credits = 0; attempted_credits = 0; total_credits = 0;
    j=0; index = 0; num = 0;flag = 0;
    endf=1;
    for(trash=0;endf==1;trash=0) {
        read(ch,char);
        if(ch == 10) { // '\n'
            endf = 0;
        }
        if(endf==1){
            index = 0;
            courses[num][index] = ch;
            index = index + 1;
            endf2=1;
            for(trash=0;endf2;trash=0) {
                read(ch,char);
                if(ch == 10) { // '\n'
                    endf2=0;
                }
                if(endf2){
                    courses[num][index] = ch;
                    index=index+1;
                }
            }
            courses[num][index] = 0;
            is_passed[num] = 0;
            num=num+1;
        }
    }
    //printCources(0,0);
    for(i = 0; i < num; i=i+1) {
        index = 0;
        field = 0;
        credit = 0;
        key = 0;
        for(trash=0;courses[i][index] != 0;trash=0) {
            if(courses[i][index] == 124) { // '|'
                field=field+1;
                index=index+1;
            }
            if(field==0){
                if(courses[i][index] <= 57 && courses[i][index] >= 48) { // '9'  '0'
                    key = key * 10 + courses[i][index] - 48; // '0'
                }
                index=index+1;
            }else if(field==1){
                credit = courses[i][index] - 48; // '0'
                total_credits = total_credits + credit;
                map[key] = i;
                index=index+1;
            }else if(field==2){
                if(courses[i][index] != 124) { //'|'
                    index=index+1;
                }
            }else if(field==3){
                if(courses[i][index] != 0) {
                    if(courses[i][index]==65){ //'A'
                        attempted_credits = attempted_credits + credit;
                        passed_credits = passed_credits + credit;
                        total_score = total_score + 4 * credit;
                        is_passed[i] = 1;
                    }else if(courses[i][index]==66){
                        attempted_credits = attempted_credits + credit;
                        passed_credits = passed_credits + credit;
                        total_score = total_score + 3 * credit;
                        is_passed[i] = 1;
                    }else if(courses[i][index]==67){
                        attempted_credits = attempted_credits + credit;
                        passed_credits = passed_credits + credit;
                        total_score = total_score + 2 * credit;
                        is_passed[i] = 1;
                    }else if(courses[i][index]==68){
                        attempted_credits = attempted_credits + credit;
                        passed_credits = passed_credits + credit;
                        total_score = total_score + credit;
                        is_passed[i] = 1;
                    }else if(courses[i][index]==70){
                        attempted_credits = attempted_credits + credit;
                    }
                }
                index=index+1;
            }
        }
    }

    if(attempted_credits != 0) {
        total_score = total_score * 1000000;
        GPAInt = total_score / attempted_credits;
        GPADot = GPAInt % 1000000;
        GPAInt = GPAInt / 1000000;
        total_score = total_score / 1000000;

        if(((GPADot/10000)%10) >= 5)GPADot = GPADot/100000 + 1;
        else GPADot = GPADot/100000;

        if(GPADot==10){
            GPADot=0;
            GPAInt=GPAInt+1;
        }
    }
    remained_credits = total_credits - passed_credits;
    write("GPA: ",char);write(GPAInt,int);write(".",char);write(GPADot,int);write("\n",char);
    write("Hours Attempted: ",char);write(attempted_credits,int);write("\n",char);
    write("Hours Completed: ",char);write(passed_credits,int);write("\n",char);
    write("Credits Remaining: ",char);write(remained_credits,int);write("\n",char);

    write("\nPossible Courses to Take Next\n",char);
    if(remained_credits == 0) {
        write("  None - Congratulations!\n",char);
    }else {
        value = 0;
        flag = 1;
        j = 0;
        for(i = 0; i < num; i=i+1) {
            endf=1;
            if(is_passed[i]) {
                endf=0;
            }
            if(endf){
                index = 0;
                j = 0;
                for(trash=0;courses[i][index] != 124;trash=0) { // '|'
                    course_name[j] = courses[i][index];
                    j=j+1;
                    index=index+1;
                }
                course_name[j] = 0;
                index=index+1;
                for(trash=0;courses[i][index] != 124;trash=0)index=index+1; // '|'
                index=index+1;
                if(courses[i][index] == 124) { // '|'
                    write("  ",char);putCourseName(0);write("\n",char);
                }else {
                    flag = 1;
                    key = 0;
                    endf2=1;
                    for(trash=0;courses[i][index] != 124 && endf2;trash=0) { // '|'
                        if(courses[i][index] <= 57 && courses[i][index] >= 48) { // '9'   '0'
                            key = key * 10 + courses[i][index] - 48; // '0'
                        }else if(courses[i][index] == 44) { // ','
                            value = map[key];
                            if(0==is_passed[value]) {
                                flag = 0;
                            }
                            key = 0;
                        }else if(courses[i][index] == 59) { // ';'
                            value = map[key];
                            if(0==is_passed[value]) {
                                flag = 0;
                            }
                            key = 0;
                            if(flag) {
                                endf2=0;
                            }
                            else {
                                flag = 1;
                            }
                        }
                        if(endf2){
                            index=index+1;
                        }
                    }
                    if(courses[i][index] == 124) { //'|'
                        value = map[key];
                        if(0==is_passed[value]) {
                            flag = 0;
                        }
                    }
                    if(flag) {
                        write("  ",char);putCourseName(0);write("\n",char);
                    }
                }
            }
        }
    }
    return 0;
}
