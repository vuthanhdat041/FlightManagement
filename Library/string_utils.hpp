#ifndef STRING_UTILS_HPP
#define STRING_UTILS_HPP

#include <string>
#include <algorithm>

bool notSpace(char c)
{
    return !std::isspace(c);
}

std::string removeLeadingSpaces(std::string &s)
{
    auto it = std::find_if(s.begin(), s.end(), notSpace);
    s.erase(s.begin(), it);
    return s;
}

std::string removeTailingSpaces(std::string &s)
{
    auto it = std::find_if(s.rbegin(), s.rend(), notSpace);
    s.erase(it.base(), s.end());
    return s;
}

std::string removesExtraSpaces(const std::string &s)
{
    std::string result;
    bool isSpace = false;

    for (char c : s)
    {
        if (std::isspace(c))
        {
            if (!isSpace)
            {
                result += ' '; // chuyển các thể loại ký tự trắng về space
                isSpace = true;
            }
        }
        else
        {
            result += c;
            isSpace = false;
        }
    }

    return result;
}

std::string removeSpaces(std::string &s)
{
    // đầu tiên xoá các khoảng trắng ở đâu và cuối chuỗi
    s = removeLeadingSpaces(s);
    s = removeTailingSpaces(s);

    // sau đó xoá các khoảng trắng thừa ở giữa chuỗi
    s = removesExtraSpaces(s);

    return s;
}

#endif // STRING_UTILS_HPP
