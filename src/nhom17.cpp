#include <iostream>
#include <fstream>
#include <limits>
#include <iomanip>
#include <string.h>
#include "../Library/string_utils.hpp"
#include "../Library/char_utils.hpp"
#include "model.hpp"
#include "../Library/time_check.hpp"
#include "dscb_io.hpp"

using namespace std;
// Ghi toàn bộ DSMB ra file DSMB.txt
void GhiDSMB(const DSMB &dsmb,
             const char *filePath = "DSMB.txt")
{
    ofstream fout(filePath, ios::out | ios::trunc);
    if (!fout)
    {
        cerr << "Không mở được " << filePath << '\n';
        return;
    }

    // ghi số lượng máy bay
    fout << dsmb.n << '\n';

    // mỗi máy bay 1 dòng: soHieu(15) + loai(20) + soCho
    for (int i = 0; i < dsmb.n; ++i)
    {
        // đảm bảo căn trái và đệm khoảng trắng
        fout << left
             << setw(15) << dsmb.nodes[i]->soHieuMB
             << setw(20) << dsmb.nodes[i]->loaiMB
             << dsmb.nodes[i]->soCho
             << '\n';
    }
}

// Đọc DSMB từ file DSMB.txt vào struct dsmb
bool DocDSMB(DSMB &dsmb,
             const char *filePath = "DSMB.txt")
{
    ifstream fin(filePath);
    if (!fin)
        return false;

    // reset
    dsmb.n = 0;
    int soLuong = 0;
    if (!(fin >> soLuong))
        return false;
    // bỏ qua phần còn lại của dòng đếm
    fin.ignore(numeric_limits<streamsize>::max(), '\n');

    char line[256];
    // đọc từng dòng đến hết hoặc đủ số lượng
    while (fin.getline(line, sizeof(line)) && dsmb.n < soLuong && dsmb.n < MAX_MB)
    {
        // trim cả dòng, nếu rỗng thì skip
        Trim(line);
        if (line[0] == '\0')
            continue;

        // tách substring
        char soHieu[16], loai[21], soChoStr[16];
        // lấy 15 ký tự đầu
        strncpy(soHieu, line, 15);
        soHieu[15] = '\0';
        // lấy 20 ký tự kế
        strncpy(loai, line + 15, 20);
        loai[20] = '\0';
        // phần còn lại
        strncpy(soChoStr, line + 35, sizeof(soChoStr) - 1);
        soChoStr[sizeof(soChoStr) - 1] = '\0';

        // trim từng phần
        Trim(soHieu);
        Trim(loai);
        Trim(soChoStr);

        // chuyển số
        int soCho = atoi(soChoStr);

        // tạo node mới
        MayBay *mb = new MayBay;
        strcpy(mb->soHieuMB, soHieu);
        strcpy(mb->loaiMB, loai);
        mb->soCho = soCho;

        dsmb.nodes[dsmb.n++] = mb;
    }

    return dsmb.n > 0;
}

// a) Quản lý máy bay (thêm / xóa / hiệu chỉnh)
void NhapMayBay(MayBay &mb);
void XemDSMB(DSMB dsmb);
void ThemMayBay(DSMB &dsmb);
void XoaMayBay(DSMB &dsmb, PTRCB &dscb);
void HieuChinhMayBay(DSMB &dsmb);

// b) Quản lý chuyến bay
void InsertLast(PTRCB &head, const ChuyenBay &cb);
void XemDanhSachChuyenBay(PTRCB dscb);

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
    DocDSMB(dsmb);

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
                ThemMayBay(dsmb);
                XemDSMB(dsmb);
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
                XoaMayBay(dsmb, dscb);
                GhiDSMB(dsmb);
                break;
            }
            }
            break;
        }
        case 'b':
        {
            if (dsmb.n == 0)
            {
                cout << "Danh sach may bay rong. Khong the dung chuc nang nay!\n";
                break;
            }
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
            case 2:
            {
                HieuChinhChuyenBay(dscb, dsmb);
            }
            case 3:
            {
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
    char temp_soHieuMB[MAX_SO_HIEU_MB];
    bool valid = false;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    while (!valid)
    {
        cout << "Nhap so hieu may bay dạng <Kí tự 1><Kí tự 2>-<4 Chữ số>, ví dụ: VN-1234): ";
        cin.getline(temp_soHieuMB, sizeof(temp_soHieuMB));
        // Loại bỏ hết dấu cách
        RemoveAllSpaces(temp_soHieuMB);
        // Chuyển toàn bộ thành in hoa để chống nhập vn-1234, Vn-1234...
        UpperCase(temp_soHieuMB);

        if (KiemTraDoDai(temp_soHieuMB, 4, 7) == 0 ||
            !(isalpha(temp_soHieuMB[0]) && isalpha(temp_soHieuMB[1]) && temp_soHieuMB[2] == '-') ||
            KiemTraPhanSauLaChuSo(3, strlen(temp_soHieuMB), temp_soHieuMB) == false)
        {
            cout << "  >> Sai định dạng!\n";
            continue;
        }
        // Duyệt kiểm tra trùng soHieuMB
        valid = true;
        for (int i = 0; i < dsmb.n; i++)
        {
            if (strcmp(dsmb.nodes[i]->soHieuMB, temp_soHieuMB) == 0)
            {
                cout << "Loi: may bay co so hieu \"" << temp_soHieuMB
                     << "\" da ton tai. Khong the them.\n";
                valid = false;
                break;
            }
        }
    }

    strcpy(mb.soHieuMB, temp_soHieuMB);

    while (true)
    {
        cout << "Nhap loai may bay: ";
        cin.getline(mb.loaiMB, MAX_LOAI_MB);
        if (IsEmpty(mb.loaiMB) || strcmp(mb.loaiMB, "0") == 0 || !isalpha(mb.loaiMB[0]))
        {
            cout << "Vui lòng nhập lại!" << endl;
            continue;
        }
        break;
    }
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

void ThemMayBay(DSMB &dsmb)
{
    while (true)
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

        GhiDSMB(dsmb);

        // Lựa nhọn nhập tiếp hay không
        char tiepTuc;
        cout << "Ban co muon them may bay tiep khong? (y/n): ";
        cin >> tiepTuc;
        if (tiepTuc != 'y' && tiepTuc != 'Y')
            break;
    }
}
void XemDSMB(DSMB dsmb)
{

    cout << "So hieu   Loai May bay   So cho: " << endl;

    for (int i = 0; i < dsmb.n; i++)
    {
        cout << dsmb.nodes[i]->soHieuMB << "      " << dsmb.nodes[i]->loaiMB
             << "      " << dsmb.nodes[i]->soCho << endl;
    }
}
void HieuChinhMayBay(DSMB &dsmb)
{
    // ===== B1. Tìm máy bay cần sửa ==========================
    char soHieuCanSua[MAX_SO_HIEU_MB];
    int index = -1;
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    while (true)
    {
        cout << "Nhap so hieu may bay dang <KT1><KT2>-<4 so> (VD: VN-1234): ";
        cin.getline(soHieuCanSua, MAX_SO_HIEU_MB);
        RemoveAllSpaces(soHieuCanSua);
        UpperCase(soHieuCanSua);

        // ─ Định dạng
        if (KiemTraDoDai(soHieuCanSua, 4, 7) == 0 ||
            !(isalpha(soHieuCanSua[0]) && isalpha(soHieuCanSua[1]) && soHieuCanSua[2] == '-') ||
            !KiemTraPhanSauLaChuSo(3, strlen(soHieuCanSua), soHieuCanSua))
        {
            cout << "  >> Sai dinh dang!\n";
            continue;
        }

        // ─ Tìm trong danh sách

        for (int i = 0; i < dsmb.n; ++i)
            if (strcmp(dsmb.nodes[i]->soHieuMB, soHieuCanSua) == 0)
            {
                index = i;
                break;
            }

        if (index == -1)
        {
            cout << "  >> Khong tim thay \"" << soHieuCanSua << "\". Vui long nhap lai.\n";
            continue;
        }
        break;
    }

    cout << "Nhap thong tin moi (nhap 0 de giu nguyen).\n";

    // ===== B2. Sửa SỐ HIỆU =================================
    // ... đã tìm được `index` của máy bay cần sửa

    char soHieuMoi[MAX_SO_HIEU_MB];
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    while (true)
    {
        cout << "So hieu moi (0 = giu nguyen): ";
        cin.getline(soHieuMoi, MAX_SO_HIEU_MB);
        RemoveAllSpaces(soHieuMoi);
        UpperCase(soHieuMoi);

        // 1) Nếu người dùng nhập "0", thoát vòng, giữ nguyên mã cũ
        if (strcmp(soHieuMoi, "0") == 0)
        {
            break;
        }

        // 2) Kiểm tra định dạng
        if (KiemTraDoDai(soHieuMoi, 4, 7) == 0 ||
            !(isalpha(soHieuMoi[0]) && isalpha(soHieuMoi[1]) && soHieuMoi[2] == '-') ||
            !KiemTraPhanSauLaChuSo(3, strlen(soHieuMoi), soHieuMoi))
        {
            cout << "  >> Sai dinh dang! Nhap lai.\n";
            continue;
        }

        // 3) Kiểm tra trùng với các máy bay khác
        bool trung = false;
        for (int i = 0; i < dsmb.n; ++i)
        {
            if (i != index && strcmp(dsmb.nodes[i]->soHieuMB, soHieuMoi) == 0)
            {
                cout << "  >> So hieu da ton tai! Nhap lai.\n";
                trung = true;
                break;
            }
        }
        if (trung)
            continue;

        // 4) Đến đây là hợp lệ → cập nhật và thoát
        strcpy(dsmb.nodes[index]->soHieuMB, soHieuMoi);
        break;
    }

    // ===== B3. Sửa LOẠI MÁY BAY =============================
    char loaiMoi[MAX_LOAI_MB];
    cout << "Loai may bay moi (0 = giu nguyen): ";
    cin.getline(loaiMoi, MAX_LOAI_MB);

    if (strcmp(loaiMoi, "0") != 0)
    {
        NormalizeSpaces(loaiMoi);
        UpperCase(loaiMoi);
        if (strlen(loaiMoi) == 0)
            cout << "  >> Bo qua vi de trong.\n";
        else
            strcpy(dsmb.nodes[index]->loaiMB, loaiMoi);
    }

    // ===== B4. Sửa SỐ CHỖ ===================================
    int soChoMoi = 0;
    while (true)
    {
        cout << "So cho moi (>=20, 0 = giu nguyen): ";
        if (!(cin >> soChoMoi)) // nhập không phải số
        {
            cin.clear();
            cout << "  >> Phai nhap so!\n";
        }
        else if (soChoMoi == 0 || soChoMoi >= 20)
        {
            if (soChoMoi >= 20)
                dsmb.nodes[index]->soCho = soChoMoi;
            break;
        }
        else
        {
            cout << "  >> So cho phai >= 20.\n";
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // dọn bộ đệm trước khi lặp
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // dọn bộ đệm sau cùng
    GhiDSMB(dsmb);

    cout << ">> Da cap nhat thong tin may bay thanh cong!\n";
}

void XoaMayBay(DSMB &dsmb, PTRCB &dscb)
{
    // ===== B1. Tìm máy bay cần xoá ==========================
    char soHieuCanXoa[MAX_SO_HIEU_MB];
    int index = -1;
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    while (true)
    {
        cout << "Nhap so hieu may bay dang <KT1><KT2>-<4 so> (VD: VN-1234): ";
        cin.getline(soHieuCanXoa, MAX_SO_HIEU_MB);
        RemoveAllSpaces(soHieuCanXoa);
        UpperCase(soHieuCanXoa);

        // ─ Định dạng
        if (KiemTraDoDai(soHieuCanXoa, 4, 7) == 0 ||
            !(isalpha(soHieuCanXoa[0]) && isalpha(soHieuCanXoa[1]) && soHieuCanXoa[2] == '-') ||
            !KiemTraPhanSauLaChuSo(3, strlen(soHieuCanXoa), soHieuCanXoa))
        {
            cout << "  >> Sai dinh dang!\n";
            continue;
        }

        // ─ Tìm trong danh sách

        for (int i = 0; i < dsmb.n; ++i)
            if (strcmp(dsmb.nodes[i]->soHieuMB, soHieuCanXoa) == 0)
            {
                index = i;
                break;
            }

        if (index == -1)
        {
            cout << "  >> Khong tim thay \"" << soHieuCanXoa << "\". Vui long nhap lai.\n";
            continue;
        }
        break; // đã tìm thấy
    }
    char choice;
    cout << "Ban co that su muon xoa may bay \"" << soHieuCanXoa << "\"? (y/N): ";
    cin >> choice;
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (choice != 'y' && choice != 'Y')
    {
        cout << "Huy xoa may bay.\n";
        return;
    }

    for (PTRCB p = dscb; p != NULL; p = p->next)
    {
        if (strcmp(p->cb.soHieuMB, soHieuCanXoa) == 0)
        {
            cout << "Máy bay đang có trong chuyến bay, không xoá được!" << endl;
            return;
        }
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

void ThemChuyenBay(PTRCB &dscb, DSMB &dsmb)
{

    ChuyenBay cb;
    PTRCB First = dscb;
    char maCB[MAX_MA_CB_LENGTH];
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    bool valid = false;

    while (!valid)
    {
        cout << "Nhập mã chuyến bay dạng <Kí tự 1><Kí tự 2> <1-4 Chữ số>, ví dụ: VN 1234): ";
        cin.getline(maCB, MAX_MA_CB_LENGTH);
        // Loại bỏ hết dấu cách
        NormalizeSpaces(maCB);
        // Chuyển toàn bộ thành in hoa để chống nhập vn-1234, Vn-1234...
        UpperCase(maCB);

        if (KiemTraDoDai(maCB, 4, 7) == 0 ||
            !(isalpha(maCB[0]) && isalpha(maCB[1]) && maCB[2] == SPACE_IN_ASCII) ||
            KiemTraPhanSauLaChuSo(3, strlen(maCB), maCB) == false)
        {
            cout << "  >> Sai định dạng!\n";
            continue;
        }
        // Duyệt kiểm tra trùng mã chuyến bay
        valid = true;
        for (PTRCB p = First; p != NULL; p = p->next)
        {
            if (strcmp(p->cb.maCB, maCB) == 0)
            {
                cout << "Loi: may bay co ma chuyen bay\"" << maCB
                     << "\" da ton tai. Khong the them.\n";
                valid = false;
                break;
            }
        }
    }

    strcpy(cb.maCB, maCB);

    // Nhập thời gian
    do
    {
        cout << "Nhap thoi gian khoi hanh (gio phut ngay thang nam): ";
        cin >> cb.ngayGioKhoiHanh.gio >> cb.ngayGioKhoiHanh.phut >> cb.ngayGioKhoiHanh.ngay >> cb.ngayGioKhoiHanh.thang >> cb.ngayGioKhoiHanh.nam;

    } while (!isValidDateTime(cb.ngayGioKhoiHanh.gio, cb.ngayGioKhoiHanh.phut, cb.ngayGioKhoiHanh.ngay, cb.ngayGioKhoiHanh.thang, cb.ngayGioKhoiHanh.nam));
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    while (true)
    {
        cout << "Nhap san bay den: ";
        cin.getline(cb.sanBayDen, MAX_LOAI_MB);
        if (IsEmpty(cb.sanBayDen) || strcmp(cb.sanBayDen, "0") == 0 || !isalpha(cb.sanBayDen[0]))
        {
            cout << "Vui lòng nhập lại!" << endl;
            continue;
        }
        break;
    }
    NormalizeSpaces(cb.sanBayDen);
    UpperCase(cb.sanBayDen);
    // Nhập số hiệu máy bay
    char soHieuMB[MAX_SO_HIEU_MB];

    // Khởi tạo giá trị số chỗ max của chuyến bay
    cb.soChoMax = -1;

    bool found = false;
    while (!found)
    {
        cout << "Nhap so hieu may bay dạng <Kí tự 1><Kí tự 2>-<4 Chữ số>, ví dụ: VN-1234): ";
        cin.getline(soHieuMB, MAX_SO_HIEU_MB);
        // Loại bỏ hết dấu cách
        RemoveAllSpaces(soHieuMB);
        // Chuyển toàn bộ thành in hoa để chống nhập vn-1234, Vn-1234...
        UpperCase(soHieuMB);

        if (KiemTraDoDai(soHieuMB, 4, 7) == 0 ||
            !(isalpha(soHieuMB[0]) && isalpha(soHieuMB[1]) && soHieuMB[2] == '-') ||
            KiemTraPhanSauLaChuSo(3, strlen(soHieuMB), soHieuMB) == false)
        {
            cout << "  >> Sai định dạng!\n";
            continue;
        }
        // Duyệt kiểm tra trùng soHieuMB

        for (int i = 0; i < dsmb.n; i++)
        {
            if (strcmp(dsmb.nodes[i]->soHieuMB, soHieuMB) == 0)
            {
                cb.soChoMax = dsmb.nodes[i]->soCho; // nếu trùng số hiệu máy bay thì node[i]-soCho = soChoMax chuyến bay
                found = true;
                break;
            }
        }
        if (!found)
        {
            cout << "  >> Loi: khong co so hieu \"" << soHieuMB
                 << "\" trong danh sach. Vui long nhap lai.\n";
            continue;
        }
    }
    strcpy(cb.soHieuMB, soHieuMB);

    int trangThai;
    while (true)
    {
        cout << "Nhap trang thai chuyen bay (0=HUY_CHUYEN, 1=CON_VE, 2=HET_VE, 3=HOAN_TAT): ";
        if (!(cin >> trangThai) || trangThai < 0 || trangThai > 3)
        {
            cout << "  >> Gia tri khong hop le. Vui long nhap lai.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        else
        {
            break;
        }
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    switch (trangThai)
    {
    case 0:
        cb.ttcb = HUY_CHUYEN;
        break;
    case 1:
        cb.ttcb = CON_VE;
        break;
    case 2:
        cb.ttcb = HET_VE;
        break;
    case 3:
        cb.ttcb = HOAN_TAT;
        break;
    default:
        cout << "Lua chon khong hop le.";
    }

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
    if (!dscb)
    {
        cout << ">> Danh sach chuyen bay rong.\n";
        return;
    }

    PTRCB first = dscb, p = nullptr;
    char maCB[MAX_MA_CB_LENGTH];
    bool found = false;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    // 1) Chọn chuyến bay
    while (!found)
    {
        cout << "Nhap ma chuyen bay can hieu chinh (VD: VN-1234): ";
        cin.getline(maCB, MAX_MA_CB_LENGTH);
        NormalizeSpaces(maCB);
        UpperCase(maCB);

        int len = strlen(maCB);
        if (KiemTraDoDai(maCB, 4, 7) == 0 ||
            !(isalpha(maCB[0]) && isalpha(maCB[1]) && maCB[2] == SPACE_IN_ASCII) ||
            KiemTraPhanSauLaChuSo(3, strlen(maCB), maCB) == false)
        {
            cout << "  >> Sai dinh dang! Vui long nhap lai.\n";
            continue;
        }

        for (PTRCB q = first; q; q = q->next)
        {
            if (strcmp(q->cb.maCB, maCB) == 0)
            {
                p = q;
                found = true;
                break;
            }
        }
        if (!found)
        {
            cout << "  >> Khong tim thay ma " << maCB << ". Nhap lai.\n";
        }
    }

    ChuyenBay &cb = p->cb;

    // 2) Hieu chinh ma chuyen bay
    cout << "\nMa hien tai: " << cb.maCB << "\n"
         << "Ma moi (-1 de giu): ";
    cin.getline(maCB, MAX_MA_CB_LENGTH);
    NormalizeSpaces(maCB);
    UpperCase(maCB);
    if (strcmp(maCB, "-1") != 0)
    {
        int len = strlen(maCB);
        if (KiemTraDoDai(maCB, 4, 7) &&
            isalpha(maCB[0]) && isalpha(maCB[1]) && maCB[2] == SPACE_IN_ASCII &&
            KiemTraPhanSauLaChuSo(3, len, maCB))
        {
            bool check = false;
            for (PTRCB q = first; q; q = q->next)
            {
                if (q != p && strcmp(q->cb.maCB, maCB) == 0)
                {
                    check = true;
                    break;
                }
            }
            if (!check)
            {
                strcpy(cb.maCB, maCB);
                cout << "  >> Da cap nhat ma.\n";
            }
            else
            {
                cout << "  >> Ma da ton tai, bo qua.\n";
            }
        }
        else
        {
            cout << "  >> Dinh dang khong hop le, bo qua.\n";
        }
    }
    // 3) hieu chinh thoi gian
    cout << "\nThoi gian hien tai: "
         << cb.ngayGioKhoiHanh.gio << ":"
         << cb.ngayGioKhoiHanh.phut << "  "
         << cb.ngayGioKhoiHanh.ngay << "/"
         << cb.ngayGioKhoiHanh.thang << "/"
         << cb.ngayGioKhoiHanh.nam << "\n";

    int gio, phut, ngay, thang, nam;
    do
    {
        cout << "Nhap gio phut ngay thang nam moi (-1 giu nguyen): ";
        cin >> gio >> phut >> ngay >> thang >> nam;
        if (gio == -1)
        {
            // người dùng muốn giữ nguyên
            break;
        }
        if (!isValidDateTime(gio, phut, ngay, thang, nam))
        {
            cout << "  >> Thoi gian khong hop le, vui long nhap lai.\n";
        }
    } while (gio != -1 && !isValidDateTime(gio, phut, ngay, thang, nam));

    if (gio != -1)
    {
        cb.ngayGioKhoiHanh.gio = gio;
        cb.ngayGioKhoiHanh.phut = phut;
        cb.ngayGioKhoiHanh.ngay = ngay;
        cb.ngayGioKhoiHanh.thang = thang;
        cb.ngayGioKhoiHanh.nam = nam;

        cout << "  >> Da cap nhat thoi gian.\n";
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    // 4) Hieu chinh san bay den
    cout << "\nSan bay den hien tai: " << cb.sanBayDen << "\n";
    char sanBayDen[MAX_SANBAY_DEN];
    while (true)
    {
        cout << "Nhap san bay den, nhan 0 de giu nguyen: ";
        cin.getline(sanBayDen, MAX_LOAI_MB);
        if (strcmp(sanBayDen, "0") == 0)
            break;

        if (IsEmpty(cb.sanBayDen) || !isalpha(cb.sanBayDen[0]))
        {
            cout << "Vui lòng nhập lại!" << endl;
            continue;
        }
        strcpy(cb.sanBayDen, sanBayDen);
        NormalizeSpaces(cb.sanBayDen);
        UpperCase(cb.sanBayDen);
        break;
    }
    cout << "  >> Da cap nhat san bay.\n";

    // 5) Hieu chinh so hieu may bay
    cout << "\nSo hieu hien tai: " << cb.soHieuMB << "\n";
    char soHieuMB[MAX_SO_HIEU_MB];
    cout << "So hieu moi (nhap 0 de giu): ";
    cin.getline(soHieuMB, MAX_SO_HIEU_MB);
    if (strcmp(soHieuMB, "0") != 0)
    {
        int len2 = strlen(soHieuMB);
        RemoveAllSpaces(soHieuMB);
        UpperCase(soHieuMB);
        if (KiemTraDoDai(soHieuMB, 4, 7) &&
            isalpha(soHieuMB[0]) && isalpha(soHieuMB[1]) && soHieuMB[2] == '-' &&
            KiemTraPhanSauLaChuSo(3, len2, soHieuMB))
        {
            bool ok = false;
            for (int i = 0; i < dsmb.n; ++i)
            {
                if (strcmp(dsmb.nodes[i]->soHieuMB, soHieuMB) == 0)
                {
                    cb.soChoMax = dsmb.nodes[i]->soCho;
                    ok = true;
                    break;
                }
            }
            if (ok)
            {
                strcpy(cb.soHieuMB, soHieuMB);
                cout << "  >> Da cap nhat so hieu.\n";
            }
            else
            {
                cout << "  >> Khong tim thay, bo qua.\n";
            }
        }
        else
        {
            cout << "  >> Dinh dang khong hop le, bo qua.\n";
        }
    }

    // 6) Hieu chinh trang thai
    cout << "\nTrang thai hien tai: " << cb.ttcb << "\n"
         << "Nhap trang thai moi (-1 giu, 0=HUY,1=CON_VE,2=HET_VE,3=HOAN_TAT): ";

    int tt;
    cin >> tt;
    if (tt >= 0 && tt <= 3)
    {
        cb.ttcb = static_cast<TrangThaiChuyenBay>(tt);
        cout << "  >> Da cap nhat trang thai.\n";
    }
    // tt == -1 thi giu nguyen
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "\n>> Hieu chinh chuyen bay " << cb.maCB << " thanh cong!\n";
}