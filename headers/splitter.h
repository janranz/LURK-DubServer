#ifndef SPLITTER_H
#define SPLITTER_H
#include"../headers/fmt/format.h"
#include<vector>
#include<string>
#include<string.h>
#include<numeric>
#include<mutex>

std::vector<std::string> splitter(std::string, std::string);
bool compare_to_lowers(char*,char*);
std::string format_vec(const char*, std::vector<unsigned char>);

extern unsigned int g_seed;
extern std::mutex randLock;
int fast_rand();
#endif //SPLITTER_H
