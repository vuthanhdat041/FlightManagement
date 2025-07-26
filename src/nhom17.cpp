#include <iostream>
#include <fstream>
#include <limits>
#include <string.h>
#include "../Library/string_utils.hpp"
using namespace std;

const int MAX_MB = 300;

// May bay:
struct MayBay
{
    char soHieuMB[16] = "";
    char loaiMB[41] = "";
    int soCho = 0;
};

struct DSMB
{
    int n = 0;
    MayBay *nodes[MAX_MB];
};

// Chuyen bay:
struct ThoiGian
{
    int phut;
    int gio;
    int ngay;
    int thang;
    int nam;
};

enum TrangThaiChuyenBay
{
    HUY_CHUYEN = 0,
    CON_VE = 1,
    HET_VE = 2,
    HOAN_TAT = 3
};

struct Ve
{

    char soCMND[13] = "";
};

struct DSVe
{
    int soVeDaDat = 0;
    Ve *danhSach = nullptr;
};

struct ChuyenBay
{
    char maCB[16];
    ThoiGian ngayGioKhoiHanh;
    char sanBayDen[20];
    int soHieuMB = 0;
    int soVeMax = 0;
    TrangThaiChuyenBay ttcb;
    DSVe dsVe;
};

struct nodeChuyenBay
{
    ChuyenBay cb;
    nodeChuyenBay *next;
};

typedef nodeChuyenBay *PTRCB;

// Hanh khach:
struct HanhKhach
{
    char soCM[13];
    char ho[20];
    char ten[30];
    char phai[4];
};

struct nodeHK
{
    HanhKhach hk;
    nodeHK *left, *right;
};

typedef nodeHK *treeHK;

// a) Quản lý máy bay (thêm / xóa / hiệu chỉnh)
void NhapMayBay(MayBay &mb);
void XemDSMB(DSMB dsmb);
void themMayBay(DSMB &dsmb);
void XoaMayBay(DSMB &dsmb);
void HieuChinhMayBay(DSMB &dsmb);

// b) Quản lý chuyến bay
void ThemChuyenBay(PTRCB &dscb, DSMB &dsmb);
void HieuChinhNgayGioChuyenBay(PTRCB &dscb);
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
        }
        case 'b':
        {
            cout << "=============" << endl;
            cout << "1. Them chuyen bay" << endl;
            cout << "2. Hieu chinh chuyen bay" << endl;
            cout << "3. Huy chuyen bay" << endl;
        case 1:
        {

            break;
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
    cout << "Nhap so hieu may bay: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.getline(temp_soHieuMB, 16);

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
    cout << "Nhap so hieu may bay muon chinh sua: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
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
    cout << "So hieu moi: ";
    cin.getline(soHieuMoi, 16);
    if (strcmp(soHieuMoi, "0") != 0)
    {
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
    cout << "Nhap so hieu may bay muon xoa: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.getline(soHieuCanXoa, 16);

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

void ThemChuyenBay(PTRCB &dscb, DSMB &dsmb)
{
}
