#include"../headers/splitter.h"
// #include<vector>
// #include<string>

std::vector<std::string> splitter(std::string s, std::string delimiter)
{
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while((pos_end = s.find(delimiter,pos_start)) != std::string::npos)
    {
        token = s.substr(pos_start,pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.emplace_back(token);
    }
    res.emplace_back(s.substr(pos_start));
    return res;
}

