// dscb_io.hpp
#pragma once
#include "model.hpp"

void GhiDSCB(PTRCB dscb, const char *filePath = "DSCB.txt");
bool DocDSCB(PTRCB &dscb, DSMB &dsmb, const char *filePath = "DSCB.txt");
