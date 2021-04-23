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

bool compare_to_lowers(char* a,char* b)
{
    if(strlen(a) != strlen(b)){return false;}
    std::string nameA;
    std::string nameB;
    size_t len = strlen(a);

    for(size_t i = 0; i < len; i++)
    {
        nameA += std::tolower(a[i]);
        nameB += std::tolower(b[i]);
    }
    if(nameA.compare(nameB) == 0)
    {
        return true;
    }
    return false;
}

std::string format_vec(const char* fmt, std::vector<unsigned char> args)
{
    return std::accumulate(
        std::begin(args),
        std::end(args),
        std::string(fmt),
        [](std::string toFmt, unsigned char arg){
            return fmt::format(toFmt, arg);
        }
    );
};
