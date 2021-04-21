#ifndef SPLITTER_H
#define SPLITTER_H
#include<vector>
#include<string>
#include<string.h>
#include<numeric>
#include"../headers/fmt/format.h"
std::vector<std::string> splitter(std::string, std::string);
bool compare_to_lowers(const std::vector<unsigned char> &,const std::vector<unsigned char> &);
std::string format_vec(const char*, std::vector<unsigned char>);
#endif //SPLITTER_H
