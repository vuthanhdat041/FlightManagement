#include <iostream>
#include <fstream>
#include <limits>
#include <string.h>
#include "Library/string_utils.hpp"
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
void xoaMayBay(DSMB &dsmb);
void HieuChinhMayBay(DSMB &dsmb);

// b) Quản lý chuyến bay
void themChuyenBay(PTRCB &dscb, DSMB &dsmb);
void suaNgayGioChuyenBay(PTRCB &dscb);
void huyChuyenBay(PTRCB &dscb);

// c) Đặt vé
void datVe(PTRCB &dscb, treeHK &dshk, DSMB &dsmb);

// d) Hủy vé
void huyVe(PTRCB &dscb);

// e) In danh sách hành khách của một chuyến bay
void inDSHanhKhach(PTRCB dscb);

// f) In danh sách chuyến bay trong ngày dd/mm/yyyy đến nơi XXXX còn vé
void inDSChuyenBayTheoNgayNoiDen(PTRCB dscb);

// g) In danh sách vé còn trống của chuyến bay X
void inVesTrong(PTRCB dscb);

// h) Thống kê số lượt thực hiện chuyến bay theo máy bay
void thongKeLuotBay(DSMB dsmb, PTRCB dscb);

// Hành khách: BST
void themHanhKhach(treeHK &root, HanhKhach hk);
treeHK timHanhKhach(treeHK root, const char *soCMND);

// Menu
void menu();

int main()
{
    DSMB dsmb;
    PTRCB dscb = nullptr;
    treeHK dshk = nullptr;

    char luaChon;

    do
    {
        menu();
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
                break;
            }
            }
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

void menu()
{
    cout << "\n========== MENU CHUONG TRINH ==========";
    cout << "\na. Cap nhat danh sach may bay (Them/Xoa/Hieu chinh)";
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
            cout << "Loi: may bay co so hieu \"" << mb.soHieuMB
                 << "\" da ton tai. Khong the them.\n";
            return;
        }
    }
    strcpy(mb.soHieuMB, temp_soHieuMB);
    cout << "Nhap loai may bay: ";
    cin.getline(mb.loaiMB, 41);

    cout << "Nhap so cho may bay: ";
    cin >> mb.soCho;
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
    MayBay *mb = new MayBay;

    char hieuChinh_SoHieuTemp[16],
        hieuChinh_SoHieu[16],
        hieuChinh_LoaiMB[41];
    int hieuChinh_SoCho;

    cout << "Nhap 0 de giu nguyen thong tin" << endl;
    cout << "Nhap so hieu may bay muon chinh sua: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.getline(hieuChinh_SoHieuTemp, 16);
    for (int i = 0; i < dsmb.n; i++)
    {
        if (strcmp(dsmb.nodes[i]->soHieuMB, hieuChinh_SoHieuTemp) != 0)
        {
            cout << "Khong co so hieu may bay tuong ung, vui long nhap lai!" << endl;
            continue;
        }
    }
    cout << "Chinh sua so hieu may bay: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cin.getline(hieuChinh_SoHieu, 16);
    cout << "Chinh sua loai may bay: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin >> hieuChinh_LoaiMB;

    cout << "Chinh sua so cho: ";
    cin >> hieuChinh_SoCho;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}
