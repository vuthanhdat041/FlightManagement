#include <iostream>
#include <fstream>
#include <limits>
#include <string.h>
#include "../Library/string_utils.hpp"
#include "../Library/char_utils.hpp"
#include "model.hpp"

using namespace std;

// a) Quản lý máy bay (thêm / xóa / hiệu chỉnh)
void NhapMayBay(MayBay &mb);
void XemDSMB(DSMB dsmb);
void themMayBay(DSMB &dsmb);
void XoaMayBay(DSMB &dsmb);
void HieuChinhMayBay(DSMB &dsmb);

// b) Quản lý chuyến bay
void InsertLast(PTRCB &head, const ChuyenBay &cb);
void XemDanhSachChuyenBay(PTRCB dscb);
bool isLeap(int y);
int daysInMonth(int m, int y);
bool isValidDateTime(const ThoiGian &t);

void ThemChuyenBay(PTRCB &dscb, DSMB &dsmb);
void HieuChinhChuyenBay(PTRCB &dscb, DSMB &dsmb);
void HuyChuyenBay(PTRCB &dscb);

// c) Đặt vé
void DatVe(PTRCB &dscb, treeHK &dshk, DSMB &dsmb);

// d) Hủy vé
void HuyVe(PTRCB &dscb);

// e) In danh sách hành khách của một chuyến bay
void InDSHanhKhach(PTRCB dscb);

// f) In danh sách chuyến bay trong ngày dd/mm/yyyy đến nơi XXXX còn vé
void InDSChuyenBayTheoNgayNoiDen(PTRCB dscb);

// g) In danh sách vé còn trống của chuyến bay X
void InVesTrong(PTRCB dscb);

// h) Thống kê số lượt thực hiện chuyến bay theo máy bay
void ThongKeLuotBay(DSMB dsmb, PTRCB dscb);

// Hành khách: BST
void ThemHanhKhach(treeHK &root, HanhKhach hk);
treeHK TimHanhKhach(treeHK root, const char *soCMND);

// Menu
void Menu();

int main()
{
    DSMB dsmb;
    PTRCB dscb = nullptr;
    treeHK dshk = nullptr;

    char luaChon;

    do
    {
        Menu();
        cin >> luaChon;
        cin.ignore(1000, '\n');

        switch (luaChon)
        {
        case 'a':
        {
            cout << "============" << endl;
            cout << "1. Them may bay" << endl;
            cout << "2. Xem danh sach may bay" << endl;
            cout << "3. Hieu chinh" << endl;
            cout << "4. Xoa may bay" << endl;

            int choiceA;
            cin >> choiceA;
            switch (choiceA)
            {
            case 1:
            {
                themMayBay(dsmb);
                break;
            }
            case 2:
            {
                XemDSMB(dsmb);
                break;
            }
            case 3:
            {
                HieuChinhMayBay(dsmb);
                break;
            }
            case 4:
            {
                XoaMayBay(dsmb);
                break;
            }
            }
            break;
        }
        case 'b':
        {
            cout << "=============" << endl;
            cout << "1. Them chuyen bay" << endl;
            cout << "2. Hieu chinh chuyen bay" << endl;
            cout << "3. Huy chuyen bay" << endl;
            int choiceB;
            cin >> choiceB;
            switch (choiceB)
            {
            case 1:
            {
                ThemChuyenBay(dscb, dsmb);
                XemDanhSachChuyenBay(dscb);
                break;
            }
            }

            break;
        }
        case 0:
        {
            cout << "Dang thoat chuong trinh...\n";
            break;
        }
        default:
            cout << "Lua chon khong hop le. Vui long chon lai.\n";
        }

    } while (luaChon != '0');

    return 0;
}

void Menu()
{
    cout << "\n========== MENU CHUONG TRINH ==========";
    cout << "\na. Cap nhat danh sach may bay (Them/Hieu chinh/Xoa)";
    cout << "\nb. Cap nhat chuyen bay (Them/Hieu chinh ngay/Huy)";
    cout << "\nc. Dat ve";
    cout << "\nd. Huy ve";
    cout << "\ne. In danh sach hanh khach tren chuyen bay";
    cout << "\nf. In cac chuyen bay theo ngay va noi den con ve";
    cout << "\ng. In danh sach ve con trong cua 1 chuyen bay";
    cout << "\nh. Thong ke so luot chuyen bay theo may bay";
    cout << "\n0. Thoat chuong trinh";
    cout << "\n========================================";
    cout << "\nNhap lua chon: ";
}

void NhapMayBay(MayBay &mb, DSMB dsmb)
{
    char temp_soHieuMB[16];
    bool valid = false;
    while (!valid)
    {
        cout << "Nhap so hieu may bay (dang VN-<chu so>, ví du: VN-1234): ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.getline(temp_soHieuMB, sizeof(temp_soHieuMB));
        RemoveAllSpaces(temp_soHieuMB);
        // Chuyển toàn bộ thành in hoa để chống nhập vn-1234, Vn-1234...
        UpperCase(temp_soHieuMB);

        // Kiểm tra độ dài tối thiểu (3 ký tự cho "VN-" + ít nhất 1 chữ số)
        int len = strlen(temp_soHieuMB);
        if (len < 4)
        {
            cout << "  >> Nhập quá ngắn!\n";
            continue;
        }
        // Kiểm tra tiền tố "VN-"
        if (strncmp(temp_soHieuMB, "VN-", 3) != 0)
        {
            cout << "  >> Phải bắt đầu bằng \"VN-\"!\n";
            continue;
        }
        // Kiểm tra phần sau đều là chữ số
        valid = true;
        for (int i = 3; i < len; i++)
        {
            if (!isdigit(static_cast<unsigned char>(temp_soHieuMB[i])))
            {
                cout << "  >> Phải là chữ số ở sau \"VN-\"!\n";
                valid = false;
                break;
            }
        }
    }

    // Duyệt kiểm tra trùng soHieuMB
    for (int i = 0; i < dsmb.n; i++)
    {
        if (strcmp(dsmb.nodes[i]->soHieuMB, temp_soHieuMB) == 0)
        {
            cout << "Loi: may bay co so hieu \"" << temp_soHieuMB
                 << "\" da ton tai. Khong the them.\n";
            return;
        }
    }

    strcpy(mb.soHieuMB, temp_soHieuMB);

    cout << "Nhap loai may bay: ";
    cin.getline(mb.loaiMB, 41);
    NormalizeSpaces(mb.loaiMB);
    UpperCase(mb.loaiMB);
    // Nhập số chỗ với kiểm tra >= 20
    do
    {
        cout << "Nhap so cho may bay (>= 20): ";
        cin >> mb.soCho;
        if (mb.soCho < 20)
        {
            cout << "So cho phai lon hon hoac bang 20. Vui long nhap lai!\n";
        }
    } while (mb.soCho < 20);

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void themMayBay(DSMB &dsmb)
{
    if (dsmb.n >= MAX_MB)
    {
        cout << "Danh sach may bay da day, khong the them nua!\n";
        return;
    }

    // Cấp phát và nhập thông tin
    MayBay *mb = new MayBay;
    NhapMayBay(*mb, dsmb);

    // Nếu không trùng thì thêm vào
    dsmb.nodes[dsmb.n++] = mb;
    cout << "Da them may bay thanh cong\n";
}
void XemDSMB(DSMB dsmb)
{
    MayBay *mb = new MayBay;
    cout << "So hieu   Loai May bay   So cho: " << endl;

    for (int i = 0; i < dsmb.n; i++)
    {
        cout << dsmb.nodes[i]->soHieuMB << "      " << dsmb.nodes[i]->loaiMB
             << "      " << dsmb.nodes[i]->soCho << endl;
    }
}
void HieuChinhMayBay(DSMB &dsmb)
{
    char soHieuCanSua[16];
    bool valid = false;

    while (!valid)
    {
        cout << "Nhap so hieu may bay muon chinh sua (dang VN-<chu so>, ví du: VN-1234): ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.getline(soHieuCanSua, sizeof(soHieuCanSua));
        RemoveAllSpaces(soHieuCanSua);
        // Chuyển toàn bộ thành in hoa để chống nhập vn-1234, Vn-1234...
        UpperCase(soHieuCanSua);

        // Kiểm tra độ dài tối thiểu (3 ký tự cho "VN-" + ít nhất 1 chữ số)
        int len = strlen(soHieuCanSua);
        if (len < 4)
        {
            cout << "  >> Nhập quá ngắn!\n";
            continue;
        }
        // Kiểm tra tiền tố "VN-"
        if (strncmp(soHieuCanSua, "VN-", 3) != 0)
        {
            cout << "  >> Phải bắt đầu bằng \"VN-\"!\n";
            continue;
        }
        // Kiểm tra phần sau đều là chữ số
        valid = true;
        for (int i = 3; i < len; i++)
        {
            if (!isdigit(static_cast<unsigned char>(soHieuCanSua[i])))
            {
                cout << "  >> Phải là chữ số ở sau \"VN-\"!\n";
                valid = false;
                break;
            }
        }
    }
    cin.getline(soHieuCanSua, 16);

    int index = -1;

    for (int i = 0; i < dsmb.n; i++)
    {
        if (strcmp(dsmb.nodes[i]->soHieuMB, soHieuCanSua) == 0)
        {
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        cout << "Khong tim thay may bay voi so hieu \"" << soHieuCanSua << "\".\n";
        return;
    }
    MayBay *mb = dsmb.nodes[index];
    cout << "Nhap thong tin moi (nhap 0 de giu nguyen):\n";

    // So hieu moi
    char soHieuMoi[16];

    cout << "So hieu moi (nhan 0 de khong chinh sua): ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.getline(soHieuMoi, 16);

    if (strcmp(soHieuMoi, "0") != 0)
    {
        bool valid = false;

        while (!valid)
        {

            RemoveAllSpaces(soHieuMoi);
            // Chuyển toàn bộ thành in hoa để chống nhập vn-1234, Vn-1234...
            UpperCase(soHieuMoi);

            // Kiểm tra độ dài tối thiểu (3 ký tự cho "VN-" + ít nhất 1 chữ số)
            int len = strlen(soHieuMoi);
            if (len < 4)
            {
                cout << "  >> Nhập quá ngắn!\n";
                continue;
            }
            // Kiểm tra tiền tố "VN-"
            if (strncmp(soHieuMoi, "VN-", 3) != 0)
            {
                cout << "  >> Phải bắt đầu bằng \"VN-\"!\n";
                continue;
            }
            // Kiểm tra phần sau đều là chữ số
            valid = true;
            for (int i = 3; i < len; i++)
            {
                if (!isdigit(static_cast<unsigned char>(soHieuMoi[i])))
                {
                    cout << "  >> Phải là chữ số ở sau \"VN-\"!\n";
                    valid = false;
                    break;
                }
            }
        }
        // Kiểm tra trùng số hiệu
        for (int i = 0; i < dsmb.n; i++)
        {
            if (i != index && strcmp(dsmb.nodes[i]->soHieuMB, soHieuMoi) == 0)
            {
                cout << "So hieu bi trung voi may bay khac. Khong cap nhat.\n";
                return;
            }
        }
        strcpy(mb->soHieuMB, soHieuMoi);
    }

    // Loai may bay
    char loaiMoi[41];
    cout << "Loai may bay moi: ";
    cin.getline(loaiMoi, 41);
    NormalizeSpaces(loaiMoi);
    UpperCase(loaiMoi);
    if (strcmp(loaiMoi, "0") != 0)
    {
        strcpy(mb->loaiMB, loaiMoi);
    }

    // So cho
    int soChoMoi;
    do
    {
        cout << "So cho moi (>= 20, nhap 0 de giu nguyen): ";
        cin >> soChoMoi;
        if (soChoMoi == 0)
        {
            // Giữ nguyên
            break;
        }
        else if (soChoMoi >= 20)
        {
            mb->soCho = soChoMoi;
            break;
        }
        else
        {
            cout << "Loi: so cho phai >= 20 hoac nhap 0 de giu nguyen. Vui long nhap lai.\n";
        }
    } while (true);

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Da cap nhat thong tin may bay thanh cong!\n";
}

void XoaMayBay(DSMB &dsmb)
{
    char soHieuCanXoa[16];

    bool valid = false;
    while (!valid)
    {
        cout << "Nhap so hieu may bay muon xoa (dang VN-<chu so>, ví du: VN-1234): ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.getline(soHieuCanXoa, sizeof(soHieuCanXoa));
        RemoveAllSpaces(soHieuCanXoa);
        // Chuyển toàn bộ thành in hoa để chống nhập vn-1234, Vn-1234...
        UpperCase(soHieuCanXoa);

        // Kiểm tra độ dài tối thiểu (3 ký tự cho "VN-" + ít nhất 1 chữ số)
        int len = strlen(soHieuCanXoa);
        if (len < 4)
        {
            cout << "  >> Nhập quá ngắn!\n";
            continue;
        }
        // Kiểm tra tiền tố "VN-"
        if (strncmp(soHieuCanXoa, "VN-", 3) != 0)
        {
            cout << "  >> Phải bắt đầu bằng \"VN-\"!\n";
            continue;
        }
        // Kiểm tra phần sau đều là chữ số
        valid = true;
        for (int i = 3; i < len; i++)
        {
            if (!isdigit(static_cast<unsigned char>(soHieuCanXoa[i])))
            {
                cout << "  >> Phải là chữ số ở sau \"VN-\"!\n";
                valid = false;
                break;
            }
        }
    }

    int index = -1;
    for (int i = 0; i < dsmb.n; i++)
    {
        if (strcmp(dsmb.nodes[i]->soHieuMB, soHieuCanXoa) == 0)
        {
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        cout << "Khong tim thay may bay co so hieu \"" << soHieuCanXoa << "\".\n";
        return;
    }

    // Xóa máy bay khỏi mảng
    delete dsmb.nodes[index];

    for (int i = index; i < dsmb.n - 1; i++)
    {
        dsmb.nodes[i] = dsmb.nodes[i + 1];
    }

    dsmb.n--;

    cout << "Da xoa may bay co so hieu \"" << soHieuCanXoa << "\" thanh cong.\n";
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
bool isValidDateTime(const ThoiGian &t)
{
    if (t.gio < 0 || t.gio > 23)
    {
        cout << "  >> Gio phai tu 0 den 23\n";
        return false;
    }
    if (t.phut < 0 || t.phut > 59)
    {
        cout << "  >> Phut phai tu 0 den 59\n";
        return false;
    }
    if (t.thang < 1 || t.thang > 12)
    {
        cout << "  >> Thang phai tu 1 den 12\n";
        return false;
    }
    int md = daysInMonth(t.thang, t.nam);
    if (md == 0 || t.ngay < 1 || t.ngay > md)
    {
        cout << "  >> Ngay phai tu 1 den " << md
             << " (theo thang " << t.thang << " nam " << t.nam << ")\n";
        return false;
    }
    if (t.nam < 1900 /*hoac gioi han mong muon*/)
    {
        cout << "  >> Nam phai >= 1900\n";
        return false;
    }
    return true;
}
void ThemChuyenBay(PTRCB &dscb, DSMB &dsmb)
{
    ChuyenBay cb;
    char maCB[16];
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    bool valid = false;
    while (!valid)
    {
        cout << "Nhap ma chuyen bay dinh dang 'VN <Chữ số>': (ma duy nhat): ";
        cin.getline(maCB, sizeof(maCB));
        NormalizeSpaces(maCB);
        // Chuyển toàn bộ thành in hoa để chống nhập vn 1234, Vn 1234...
        UpperCase(maCB);

        // Kiểm tra độ dài tối thiểu (3 ký tự cho "VN " + ít nhất 1 chữ số)
        int len = strlen(maCB);
        if (len < 4)
        {
            cout << "  >> Nhập lại, tối thiểu 4 kí tự\n";
            continue;
        }
        // Kiểm tra tiền tố "VN "
        if (strncmp(maCB, "VN ", 3) != 0)
        {
            cout << "  >> Phải bắt đầu bằng \"VN \"!\n";
            continue;
        }
        // Kiểm tra phần sau đều là chữ số
        valid = true;
        for (int i = 3; i < len; i++)
        {
            if (!isdigit(static_cast<unsigned char>(maCB[i])))
            {
                cout << "  >> Phải là chữ số ở sau \"VN \"!\n";
                valid = false;
                break;
            }
        }
    }

    // Kiểm tra trùng mã chuyến bay
    for (PTRCB p = dscb; p != nullptr; p = p->next)
    {
        if (strcmp(p->cb.maCB, maCB) == 0)
        {
            cout << "Ma chuyen bay da ton tai!\n";
            return;
        }
    }
    strcpy(cb.maCB, maCB);
    // Nhập thời gian

    do
    {
        cout << "Nhap thoi gian khoi hanh (gio phut ngay thang nam): ";
        cin >> cb.ngayGioKhoiHanh.gio >> cb.ngayGioKhoiHanh.phut >> cb.ngayGioKhoiHanh.ngay >> cb.ngayGioKhoiHanh.thang >> cb.ngayGioKhoiHanh.nam;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    } while (!isValidDateTime(cb.ngayGioKhoiHanh));

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Nhap san bay den: ";
    cin.getline(cb.sanBayDen, 20);
    NormalizeSpaces(cb.sanBayDen);
    UpperCase(cb.sanBayDen);

    // Nhập số hiệu máy bay
    char soHieuMB[16];
    cout << "Nhap so hieu may bay: ";

    cin.getline(soHieuMB, 16);
    bool valid1 = false;
    while (!valid1)
    {
        cout << "Nhap so hieu may bay (dang VN-<chu so>, ví du: VN-1234): ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.getline(soHieuMB, sizeof(soHieuMB));
        RemoveAllSpaces(soHieuMB);
        // Chuyển toàn bộ thành in hoa để chống nhập vn-1234, Vn-1234...
        UpperCase(soHieuMB);

        // Kiểm tra độ dài tối thiểu (3 ký tự cho "VN-" + ít nhất 1 chữ số)

        if (strlen(soHieuMB) < 4)
        {
            cout << "  >> Nhập quá ngắn!\n";
            continue;
        }
        // Kiểm tra tiền tố "VN-"
        if (strncmp(soHieuMB, "VN-", 3) != 0)
        {
            cout << "  >> Phải bắt đầu bằng \"VN-\"!\n";
            continue;
        }
        // Kiểm tra phần sau đều là chữ số
        valid1 = true;
        for (int i = 3; i < strlen(soHieuMB); i++)
        {
            if (!isdigit(static_cast<unsigned char>(soHieuMB[i])))
            {
                cout << "  >> Phải là chữ số ở sau \"VN-\"!\n";
                valid1 = false;
                break;
            }
        }
    }
    cb.soChoMax = -1;
    for (int i = 0; i < dsmb.n; i++)
    {
        if (equalsIgnoreCase(dsmb.nodes[i]->soHieuMB, soHieuMB))
        {
            cb.soChoMax = dsmb.nodes[i]->soCho;
            break;
        }
    }

    if (cb.soChoMax == -1)
    {
        cout << "Khong tim thay may bay co so hieu nay!\n";
        return;
    }

    cb.ttcb = CON_VE;
    strcpy(cb.soHieuMB, soHieuMB);

    // Cấp phát danh sách vé
    /*cb.dsVe.soVeDaDat = 0;
    cb.dsVe.danhSach = new Ve[cb.soChoMax];
    for (int i = 0; i < cb.soChoMax; i++)
    {
        strcpy(cb.dsVe.danhSach[i].soCMND, "");
    } */
    // Them chuyen bay
    InsertLast(dscb, cb);
    cout << "Da them chuyen bay thanh cong.\n";
}

void XemDanhSachChuyenBay(PTRCB dscb)
{
    if (dscb == nullptr)
    {
        cout << "Danh sach chuyen bay rong.\n";
        return;
    }

    cout << "====== DANH SACH CHUYEN BAY ======\n";
    cout << left << setw(16) << "Ma CB"
         << setw(20) << "San bay den"
         << setw(20) << "Thoi gian"
         << setw(12) << "So hieu MB"
         << setw(10) << "So cho"
         << setw(12) << "Trang thai"
         << endl;

    for (PTRCB p = dscb; p != nullptr; p = p->next)
    {
        const ChuyenBay &cb = p->cb;
        string tt;

        switch (cb.ttcb)
        {
        case HUY_CHUYEN:
            tt = "Huy";
            break;
        case CON_VE:
            tt = "Con ve";
            break;
        case HET_VE:
            tt = "Het ve";
            break;
        case HOAN_TAT:
            tt = "Hoan tat";
            break;
        default:
            tt = "Khac";
            break;
        }

        cout << left << setw(16) << cb.maCB
             << setw(20) << cb.sanBayDen
             << setw(2) << cb.ngayGioKhoiHanh.gio << ":"
             << setw(2) << cb.ngayGioKhoiHanh.phut << " "
             << setw(2) << cb.ngayGioKhoiHanh.ngay << "/"
             << setw(2) << cb.ngayGioKhoiHanh.thang << "/"
             << cb.ngayGioKhoiHanh.nam
             << setw(6) << "   "
             << setw(12) << cb.soHieuMB
             << setw(10) << cb.soChoMax
             << setw(12) << tt
             << endl;
    }

    cout << "==================================\n";
}

void HieuChinhChuyenBay(PTRCB &dscb, DSMB &dsmb)
{

    char hieuChinh_maCB[16];
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    bool valid = false;
    while (!valid)
    {
        cout << "Nhap ma chuyen bay can hieu chinh dinh dang 'VN <Chữ số>': (ma duy nhat): ";
        cin.getline(hieuChinh_maCB, sizeof(hieuChinh_maCB));
        NormalizeSpaces(hieuChinh_maCB);
        // Chuyển toàn bộ thành in hoa để chống nhập vn 1234, Vn 1234...
        UpperCase(hieuChinh_maCB);

        // Kiểm tra độ dài tối thiểu (3 ký tự cho "VN " + ít nhất 1 chữ số)
        int len = strlen(hieuChinh_maCB);
        if (len < 4)
        {
            cout << "  >> Nhập lại, tối thiểu 4 kí tự\n";
            continue;
        }
        // Kiểm tra tiền tố "VN "
        if (strncmp(hieuChinh_maCB, "VN ", 3) != 0)
        {
            cout << "  >> Phải bắt đầu bằng \"VN \"!\n";
            continue;
        }
        // Kiểm tra phần sau đều là chữ số
        valid = true;
        for (int i = 3; i < len; i++)
        {
            if (!isdigit(static_cast<unsigned char>(hieuChinh_maCB[i])))
            {
                cout << "  >> Phải là chữ số ở sau \"VN \"!\n";
                valid = false;
                break;
            }
        }
    }

    PTRCB p = dscb;
    while (p && strcmp(p->cb.maCB, hieuChinh_maCB) != 0)
        p = p->next;
    if (!p)
    {
        cout << "Khong tim thay ma chuyen bay " << hieuChinh_maCB << "\n";
        return;
    }
    ChuyenBay &cb = p->cb;
    cout << "Nhap thong tin moi (nhap 0 de giu nguyen):\n";

    // Nhập thời gian
    char newMa[16];
    cout << "Ma CB moi: ";
    cin.getline(newMa, sizeof(newMa));
    if (strcmp(newMa, "0") != 0)
    {
        // xác thực y hệt phần nhập ở trên
        bool ok = false;
        while (!ok)
        {
            NormalizeSpaces(newMa);
            UpperCase(newMa);
            int L = strlen(newMa);
            if (L < 4 || strncmp(newMa, "VN ", 3) != 0)
            {
                cout << "  >> Sai dinh dang, phai VN <chu so>.\n";
                cout << "Ma CB moi: ";
                cin.getline(newMa, sizeof(newMa));
                continue;
            }
            ok = true;
            for (int i = 3; i < L; i++)
            {
                if (!isdigit(static_cast<unsigned char>(newMa[i])))
                {
                    ok = false;
                    break;
                }
            }
            if (!ok)
            {
                cout << "  >> Phai la chu so sau VN .\n";
                cout << "Ma CB moi: ";
                cin.getline(newMa, sizeof(newMa));
            }
        }
        // kiểm tra trùng
        for (PTRCB q = dscb; q; q = q->next)
        {
            if (q != p && strcmp(q->cb.maCB, newMa) == 0)
            {
                cout << "Ma CB bi trung, khong cap nhat.\n";
                goto SKIP_MACB;
            }
        }
        strcpy(cb.maCB, newMa);
    }
SKIP_MACB:

    // --- b) Thoi gian ---
    cout << "Thoi gian khoi hanh moi (nhap 0 0 0 0 0 de giu nguyen):\n";
    ThoiGian tmp;
    do
    {
        cout << "  Gio Phut Ngay Thang Nam: ";
        cin >> tmp.gio >> tmp.phut >> tmp.ngay >> tmp.thang >> tmp.nam;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (tmp.gio == 0 && tmp.phut == 0 && tmp.ngay == 0 && tmp.thang == 0 && tmp.nam == 0)
            break;
    } while (!isValidDateTime(tmp));
    if (!(tmp.gio == 0 && tmp.phut == 0 && tmp.ngay == 0 && tmp.thang == 0 && tmp.nam == 0))
        cb.ngayGioKhoiHanh = tmp;

    // --- c) San bay den ---
    char sbDen[20];
    cout << "San bay den moi: ";
    cin.getline(sbDen, sizeof(sbDen));
    if (strcmp(sbDen, "0") != 0)
    {
        NormalizeSpaces(sbDen);
        UpperCase(sbDen);
        strcpy(cb.sanBayDen, sbDen);
    }

    // --- d) So hieu MB ---
    char newSo[16];
    cout << "So hieu MB moi (dang VN-<chu so>): ";
    cin.getline(newSo, sizeof(newSo));
    if (strcmp(newSo, "0") != 0)
    {
        bool ok2 = false;
        while (!ok2)
        {
            RemoveAllSpaces(newSo);
            UpperCase(newSo);
            int L = strlen(newSo);
            if (L < 4 || strncmp(newSo, "VN-", 3) != 0)
            {
                cout << "  >> Phai VN-<chu so>.\n";
                cout << "So hieu MB moi: ";
                cin.getline(newSo, sizeof(newSo));
                continue;
            }
            ok2 = true;
            for (int i = 3; i < L; i++)
            {
                if (!isdigit(static_cast<unsigned char>(newSo[i])))
                {
                    ok2 = false;
                    break;
                }
            }
            if (!ok2)
            {
                cout << "  >> Phai la chu so.\n";
                cout << "So hieu MB moi: ";
                cin.getline(newSo, sizeof(newSo));
            }
        }
        // tìm chỗ ngồi mới
        int sc = -1;
        for (int i = 0; i < dsmb.n; i++)
        {
            if (equalsIgnoreCase(dsmb.nodes[i]->soHieuMB, newSo))
            {
                sc = dsmb.nodes[i]->soCho;
                break;
            }
        }
        if (sc == -1)
        {
            cout << "Khong tim thay may bay so hieu " << newSo << "\n";
        }
        else
        {
            strcpy(cb.soHieuMB, newSo);
            cb.soChoMax = sc;
        }
    }

    cout << "Da cap nhat thong tin chuyen bay.\n";
}
