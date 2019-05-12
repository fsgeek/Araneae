#pragma once

#include <wdm.h>

const ULONG TarantulaControlGetRequest CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 0xF01, METHOD_NEITHER, FILE_READ_ACCESS | FILE_WRITE_ACCESS);