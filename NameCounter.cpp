#pragma once
#include "./cCompilerCommon.hpp"

std::string NameCounter::getNumberedName(std::string name){
    if(map.find(name)==map.end()){
        map.insert({name,0});
    }
    int i;
    for(i=0;i<map[name];i++){
        if(mark.find({name,i})!=mark.end()){
            if(mark[{name,i}]==false)break;
        }
    }
    if(i<map[name]){
        mark[{name,i}] = true;
        return name+"["+std::to_string(i)+"]";
    }else{
        std::string ret = name+"["+std::to_string(map[name]++)+"]";
        mark.insert({{name,map[name]-1}, true});
        return ret;
    }
}

std::string NameCounter::releaseName(std::string name){
    if(name[0]=='[')name=name.substr(1,name.length()-2);
    if(name[0]!='`'||name.length()<3)return name;
    
    int id;
    int i=name.length()-1;
    while(name[i]!='[')i--;
    std::string ids = name.substr(i+1, name.length()-i-2);
    std::string nameo = name.substr(0, i);
    sscanf(ids.c_str(),"%d",&id);
    if(mark.find({nameo,id})==mark.end())return name;
    else mark[std::pair<std::string, int>(nameo,id)]=false;
    return name;
}

