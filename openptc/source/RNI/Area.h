/*  DO NOT EDIT - automatically generated by msjavah  */
#include <native.h>
#pragma warning(disable:4510)
#pragma warning(disable:4512)
#pragma warning(disable:4610)


/*  Header for class ptc/Area  */

#ifndef _Included_ptc_Area
#define _Included_ptc_Area

#define Hptc_Area Classptc_Area
typedef struct Classptc_Area {
#include <pshpack4.h>
    long MSReserved;
    long left;
    long top;
    long right;
    long bottom;
#include <poppack.h>
} Classptc_Area;

typedef struct ClassArrayOfptc_Area {
    int32_t MSReserved;
    const unsigned long length;
    Hptc_Area * const body[1];
} ClassArrayOfptc_Area;
#define HArrayOfptc_Area ClassArrayOfptc_Area
#define ArrayOfptc_Area ClassArrayOfptc_Area

#endif  /* _Included_ptc_Area */

#pragma warning(default:4510)
#pragma warning(default:4512)
#pragma warning(default:4610)

