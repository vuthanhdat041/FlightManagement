
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <cstring>
#include "model.hpp" // Chứa định nghĩa ChuyenBay, PTRCB, DSMB, Ve,...
                     // Nếu bạn dùng các hàm xử lý chuỗi (tùy chọn)
// ==== Doc/Ghi danh sách chuyến bay (DSCB) =====================
// LƯU Ý: đặt phần khai báo này vào sau phần khai báo struct, enum…
//        nhưng TRƯỚC hàm main.
//        Bạn có thể chỉnh sửa path file mặc định ("DSCB.txt") tuỳ ý.

#include <sstream>

// -----------------------------------------------------------------
// Hàm tiện ích: chuyển struct DateTime -> chuỗi "dd/MM/yyyy HH:mm"
// -----------------------------------------------------------------
static std::string FormatDateTime(const ThoiGian &dt)
{
    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(2) << dt.ngay << '/'
        << std::setw(2) << dt.thang << '/'
        << std::setw(4) << dt.nam << ' '
        << std::setw(2) << dt.gio << ':'
        << std::setw(2) << dt.phut;
    return oss.str();
}

// -----------------------------------------------------------------
// Ghi toàn bộ danh sách chuyến bay ra file.
// Mỗi chuyến bay một dòng, định dạng:
// <maCB>, dd/MM/yyyy HH:mm, <sanBayDen>, <trangThai>, <soHieuMB>, <soVeDaDat>/<soChoMax>:<danh sách CMND>
// -----------------------------------------------------------------
void GhiDSCB(PTRCB dscb, const char *filePath = "DSCB.txt")
{
    std::ofstream fout(filePath, std::ios::out | std::ios::trunc);
    if (!fout)
    {
        std::cerr << "Không mở được " << filePath << '\n';
        return;
    }

    // 1. Đếm số chuyến bay
    int dem = 0;
    for (PTRCB p = dscb; p; p = p->next)
        ++dem;

    fout << dem << '\n';

    // 2. Ghi từng chuyến
    for (PTRCB p = dscb; p; p = p->next)
    {
        const ChuyenBay &cb = p->cb;

        // 2.1 định dạng ngày giờ
        std::string tg = FormatDateTime(cb.ngayGioKhoiHanh);

        // 2.2 vé đã đặt "x/y:cmnd1;cmnd2;..."
        std::ostringstream ossVe;
        ossVe << cb.dsVe.soVeDaDat << '/' << cb.soChoMax << ':';
        bool first = true;
        for (int i = 0; i < cb.soChoMax; ++i)
        {
            if (cb.dsVe.danhSach[i].soCMND[0] != '\0')
            {
                if (!first)
                    ossVe << ';';
                first = false;
                ossVe << cb.dsVe.danhSach[i].soCMND;
            }
        }

        fout << cb.maCB << ", " << tg << ", "
             << cb.sanBayDen << ", " << static_cast<int>(cb.ttcb) << ", "
             << cb.soHieuMB << ", " << ossVe.str() << '\n';
    }
}

// -----------------------------------------------------------------
// Đọc file DSCB.txt và tái tạo Linked-list dscb
// Trả về true nếu đọc OK, ngược lại false
// -----------------------------------------------------------------
bool DocDSCB(PTRCB &dscb, DSMB &dsmb, const char *filePath = "DSCB.txt")
{
    std::ifstream fin(filePath);
    if (!fin)
        return false;

    // Giải phóng danh sách cũ (nếu có)
    while (dscb)
    {
        PTRCB tmp = dscb;
        dscb = dscb->next;
        delete[] tmp->cb.dsVe.danhSach;
        delete tmp;
    }

    int soLuong = 0;
    if (!(fin >> soLuong))
        return false;
    fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::string line;
    for (int k = 0; k < soLuong && std::getline(fin, line); ++k)
    {
        if (line.empty())
        {
            --k;
            continue;
        }
        std::stringstream ss(line);

        ChuyenBay cb{};
        std::string tgStr, ttStr, veStr;

        // 1) tách 6 trường chính bằng dấu ','
        std::getline(ss, cb.maCB, ',');
        ss.ignore(1); // bỏ khoảng trắng
        std::getline(ss, tgStr, ',');
        ss.ignore(1);
        std::getline(ss, cb.sanBayDen, ',');
        ss.ignore(1);
        std::getline(ss, ttStr, ',');
        ss.ignore(1);
        std::getline(ss, cb.soHieuMB, ',');
        ss.ignore(1);
        std::getline(ss, veStr);

        // 2) parse thời gian "dd/MM/yyyy HH:mm"
        {
            std::istringstream is(tgStr);
            char delim1, delim2, space, delim3;
            is >> cb.ngayGioKhoiHanh.ngay >> delim1 >> cb.ngayGioKhoiHanh.thang >> delim2 >> cb.ngayGioKhoiHanh.nam >> space >> cb.ngayGioKhoiHanh.gio >> delim3 >> cb.ngayGioKhoiHanh.phut;
        }

        // 3) trạng thái
        cb.ttcb = static_cast<TrangThaiChuyenBay>(std::stoi(ttStr));

        // 4) Vé:  soVeDaDat/soChoMax:cmnd1;cmnd2;...
        int posSlash = veStr.find('/');
        int posColon = veStr.find(':');
        if (posSlash != std::string::npos && posColon != std::string::npos)
        {
            cb.dsVe.soVeDaDat = std::stoi(veStr.substr(0, posSlash));
            cb.soChoMax = std::stoi(veStr.substr(posSlash + 1, posColon - posSlash - 1));
            cb.dsVe.danhSach = new Ve[cb.soChoMax]{};

            std::string listCMND = veStr.substr(posColon + 1);
            std::stringstream ssCMND(listCMND);
            std::string cmnd;
            int idx = 0;
            while (idx < cb.soChoMax && std::getline(ssCMND, cmnd, ';'))
            {
                std::strncpy(cb.dsVe.danhSach[idx].soCMND, cmnd.c_str(), sizeof(cb.dsVe.danhSach[idx].soCMND) - 1);
                ++idx;
            }
        }
        else // không có info vé
        {
            cb.soChoMax = 0;
            cb.dsVe.soVeDaDat = 0;
            cb.dsVe.danhSach = nullptr;
        }

        // 5) Chèn vào linked-list
        InsertLast(dscb, cb);
    }

    return true;
}

void InsertLast(PTRCB &head, const ChuyenBay &cb)
{
    PTRCB newNode = new nodeChuyenBay;
    newNode->cb = cb;
    newNode->next = nullptr;

    // Chèn vào đầu nếu danh sách rỗng hoặc cb.maCB nhỏ hơn node đầu
    if (head == nullptr || strcmp(cb.maCB, head->cb.maCB) < 0)
    {
        newNode->next = head;
        head = newNode;
    }
    else
    {
        PTRCB prev = head;
        while (prev->next != nullptr && strcmp(cb.maCB, prev->next->cb.maCB) > 0)
        {
            prev = prev->next;
        }
        newNode->next = prev->next;
        prev->next = newNode;
    }
}