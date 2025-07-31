#ifndef CHAR_UTILS_HPP
#define CHAR_UTILS_HPP

#include <cstring>
#include <cctype>

// Xóa khoảng trắng đầu và cuối chuỗi
void Trim(char *str)
{
    int start = 0;
    while (isspace(str[start]))
        start++;

    int end = strlen(str) - 1;
    while (end >= start && isspace(str[end]))
        end--;

    int j = 0;
    for (int i = start; i <= end; ++i)
        str[j++] = str[i];
    str[j] = '\0';
}

// Xóa tất cả khoảng trắng trong chuỗi
void RemoveAllSpaces(char *str)
{
    int j = 0;
    for (int i = 0; str[i] != '\0'; ++i)
    {
        if (!isspace(str[i]))
            str[j++] = str[i];
    }
    str[j] = '\0';
}

// Chuẩn hóa chuỗi: cắt trắng đầu/cuối và rút gọn khoảng trắng giữa còn 1
void NormalizeSpaces(char *str)
{
    int len = strlen(str);
    int j = 0;
    bool inSpace = false;

    for (int i = 0; i < len; ++i)
    {
        if (isspace(str[i]))
        {
            if (!inSpace)
            {
                str[j++] = ' ';
                inSpace = true;
            }
        }
        else
        {
            str[j++] = str[i];
            inSpace = false;
        }
    }

    // Xoá trắng cuối nếu có
    if (j > 0 && str[j - 1] == ' ')
        j--;

    str[j] = '\0';

    // Xoá trắng đầu nếu có (dời chuỗi về đầu)
    if (str[0] == ' ')
    {
        int k = 0;
        while (str[k] == ' ')
            k++;

        int i = 0;
        while (str[k])
            str[i++] = str[k++];
        str[i] = '\0';
    }
}
void NormalizeToUpperCase(char *str)
{
    int len = strlen(str);
    int j = 0;

    for (int i = 0; i < len; ++i)
    {
        char c = toupper(str[i]);
        if ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '-' || c == '_')
        {
            str[j++] = c;
        }
    }

    str[j] = '\0';
}

// Cho tất cả các kí tự in hoa
void UpperCase(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        str[i] = toupper(str[i]);
    }
}
// So sánh chuỗi bỏ qua viết hoa & viết thường
bool equalsIgnoreCase(const char *s1, const char *s2)
{
    while (*s1 && *s2)
    {
        if (tolower(*s1) != tolower(*s2))
            return false;
        ++s1;
        ++s2;
    }
    return *s1 == *s2;
}

// Ép độ dài tối thiểu và tối đa
int KiemTraDoDai(char *str, int doDaiToiThieu, int doDaiToiDa)
{
    if (strlen(str) < doDaiToiThieu || strlen(str) > doDaiToiDa)
        return 0;
    return 1;
}

// kiểm tra trùng chuỗi
bool KiemTraTrung(int n, char *str_root, char *str_input)
{
    for (int i = 0; i < n; i++)
    {
        if (strcmp(str_root, str_input) == 0)
        {
            return true;
        }
    }
    return false;
}

// Kiểm tra phần tử là chữ số ở vị trí nào
bool KiemTraPhanSauLaChuSo(int startNumber, int endNumber, char *str)
{
    for (int i = startNumber; i < endNumber; i++)
        if (!isdigit(static_cast<unsigned char>(str[i])))
            return false;
    return true;
}
// Hàm chống nhập rỗng
bool IsEmpty(const char s[])
{
    // nếu ký tự đầu là '\0' thì rỗng ngay
    if (s[0] == '\0')
        return true;
    // duyệt từng ký tự
    for (int i = 0; s[i] != '\0'; ++i)
    {
        char c = s[i];
        // nếu gặp ký tự không phải space, tab, CR thì không rỗng
        if (c != ' ' && c != '\t' && c != '\r' && c != '\n')
        {
            return false;
        }
    }
    return true; // toàn space/tab/CR/NL
}
#endif // CHAR_UTILS_HPP
