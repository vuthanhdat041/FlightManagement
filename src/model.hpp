#ifndef MODEL_HPP
#define MODEL_HPP

#include <cstring>          // nếu cần dùng strlen, strcpy sau này
#include <cstdlib>          // nếu cần dùng malloc, free
#define MAX_MB 300          // số lượng tối đa máy bay
#define MAX_SO_HIEU_MB 16   // độ dài tối đa chuỗi số hiệu máy bay
#define MAX_LOAI_MB 41      // độ dài tối đa chuỗi loại máy bay
#define MAX_CMND_LENGTH 13  // độ dài chuỗi CMND
#define MAX_MA_CB_LENGTH 13 // độ dài chuỗi mã chuyến bay
#define MAX_SANBAY_DEN 20   // độ dài tên sân bay đến
#define MAX_HO_LENGTH 20    // độ dài chuỗi họ
#define MAX_TEN_LENGTH 30   // độ dài chuỗi tên
#define MAX_GIOI_TINH 4     // độ dài chuỗi giới tính

// ===== May bay =====
struct MayBay
{
    char soHieuMB[MAX_SO_HIEU_MB] = "";
    char loaiMB[MAX_LOAI_MB] = "";
    int soCho = 0;
};

struct DSMB
{
    int n = 0;
    MayBay *nodes[MAX_MB];
};

// ===== Chuyen bay =====
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
    char soCMND[MAX_CMND_LENGTH] = "";
};

struct DSVe
{
    int soVeDaDat;
    Ve *danhSach = NULL;
};

struct ChuyenBay
{
    char maCB[MAX_MA_CB_LENGTH];
    ThoiGian ngayGioKhoiHanh;
    char sanBayDen[MAX_SANBAY_DEN];
    char soHieuMB[MAX_SO_HIEU_MB];
    int soChoMax = 0;
    TrangThaiChuyenBay ttcb;
    DSVe dsVe;
};

struct nodeChuyenBay
{
    ChuyenBay cb;
    nodeChuyenBay *next;
};

typedef nodeChuyenBay *PTRCB;

// ===== Hanh khach =====
struct HanhKhach
{
    char soCM[MAX_CMND_LENGTH];
    char ho[MAX_HO_LENGTH];
    char ten[MAX_TEN_LENGTH];
    char phai[MAX_GIOI_TINH];
};

struct nodeHK
{
    HanhKhach hk;
    nodeHK *left, *right;
};

typedef nodeHK *treeHK;

#endif // MODEL_HPP
