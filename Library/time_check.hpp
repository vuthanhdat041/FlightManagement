#ifndef TIME_CHECK_HPP
#define TIME_CHECK_HPP

#include <iostream>
using namespace std;
// Kiểm tra năm nhuận
bool isLeap(int y)
{
    return (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0));
}

// Trả về số ngày của tháng m trong năm y
int daysInMonth(int m, int y)
{
    static const int md[] = {0,
                             31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (m == 2 && isLeap(y))
        return 29;
    if (m >= 1 && m <= 12)
        return md[m];
    return 0;
}

// Kiểm tra toàn bộ
bool isValidDateTime(int gio, int phut, int ngay, int thang, int nam)
{
    if (gio < 0 || gio > 23)
    {
        cout << "  >> Gio phai tu 0 den 23\n";
        return false;
    }
    if (phut < 0 || phut > 59)
    {
        cout << "  >> Phut phai tu 0 den 59\n";
        return false;
    }
    if (thang < 1 || thang > 12)
    {
        cout << "  >> Thang phai tu 1 den 12\n";
        return false;
    }
    int md = daysInMonth(thang, nam);
    if (md == 0 || ngay < 1 || ngay > md)
    {
        cout << "  >> Ngay phai tu 1 den " << md
             << " (theo thang " << thang << " nam " << nam << ")\n";
        return false;
    }
    if (nam < 1900)
    {
        cout << "  >> Nam phai >= 1900\n";
        return false;
    }
    return true;
}
#endif