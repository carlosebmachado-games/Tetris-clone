#ifndef UTIL_H
#define UTIL_H


#include <iostream>
#include <string>
#include <vector>


std::vector<std::string> split(std::string str, char sc)
{
    auto strings = std::vector<std::string>();
    std::string sstr = "";
    for (char c : str)
    {
        if (c == sc)
        {
            strings.push_back(sstr);
            sstr = "";
            continue;
        }
        sstr += c;
    }
    strings.push_back(sstr);
    return strings;
}

std::string trim(std::string str)
{
    auto s = 0, c = 0;
    for (size_t i = 0; i < str.size(); i++)
    {
        if (str[i] != ' ')
        {
            s = i;
            break;
        }
    }
    for (size_t i = str.size(); i > 0; i--)
    {
        if (str[i - 1] != ' ')
        {
            c = i - s;
            break;
        }
    }
    return str.substr(s, c);
}

double clamp(double val, double min, double max)
{
    if (val > max) return max;
    if (val < min) return min;
    return val;
}

#endif // UTIL_H
