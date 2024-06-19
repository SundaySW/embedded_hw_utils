#pragma once

#define mStep                       float(32)
#define mSTEPS(v)                   int((v) * mStep)
#define uSec(htim_psc)              (SystemCoreClock / (htim_psc))
#define rampT(v, htim_psc)          uint32_t((v)/mStep * uSec(htim_psc))
