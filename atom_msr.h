#ifndef __ATOM_MSR_H
#define __ATOM_MSR_H

#include <assert.h>
#include <stdint.h>

#define L2MAXDIST_MAX (31)
#define L3MAXDIST_MAX (63)
#define L2XQ_MAX (31)
#define L3XQ_MAX (31)
#define LOW_DMDDENS_L2L3XQ_MAX (63)

struct msr1A4_reg
{
    uint64_t L2_STREAM_DISABLE : 1;
    uint64_t pad0 : 1;
    uint64_t L1_NLP_DISABLE : 1;
    uint64_t L1_IPP_DISABLE : 1;
    uint64_t L1_NPP_DISABLE : 1;
    uint64_t L2_AMP_DISABLE : 1;
    uint64_t pad1: 58;
};
static_assert(sizeof(struct msr1A4_reg) == sizeof(uint64_t),
              "msr1A4_s is missing padding bits");

struct msr1320_reg
{
	uint64_t L2_STREAM_AMP_XQ_THRESHOLD : 5;
	uint64_t pad0 : 15;
	uint64_t L2_STREAM_MAX_DISTANCE : 5;
	uint64_t pad1 : 5;
	uint64_t L2_AMP_DISABLE_RECURSION : 1;
	uint64_t pad2 : 6;
	uint64_t LLC_STREAM_MAX_DISTANCE : 6;
	uint64_t LLC_STREAM_DISABLE : 1;
	uint64_t pad3 : 14;
	uint64_t LLC_STREAM_XQ_THRESHOLD : 5;
    uint64_t pad4 : 1;
};
static_assert(sizeof(struct msr1320_reg) == sizeof(uint64_t),
              "msr1320_s is missing padding bits");

struct msr1321_reg
{
	uint64_t L2_STREAM_AMP_CREATE_IL1 : 1;
	uint64_t pad0 : 20;
	uint64_t L2_STREAM_DEMAND_DENSITY : 8;
	uint64_t L2_STREAM_DEMAND_DENSITY_OVR : 4;
	uint64_t pad1 : 7;
	uint64_t L2_DISABLE_NEXT_LINE_PREFETCH : 1;
	uint64_t L2_LLC_STREAM_AMP_XQ_THRESHOLD : 6;
    uint64_t pad2 : 17;
};
static_assert(sizeof(struct msr1321_reg) == sizeof(uint64_t),
              "msr1321_s is missing padding bits");

struct msr1322_reg
{
	uint64_t pad0 : 14;
	uint64_t LLC_STREAM_DEMAND_DENSITY : 9;
	uint64_t LLC_STREAM_DEMAND_DENSITY_OVR : 4;
	uint64_t L2_AMP_CONFIDENCE_DPT0 : 6;
	uint64_t L2_AMP_CONFIDENCE_DPT1 : 6;
	uint64_t L2_AMP_CONFIDENCE_DPT2 : 6;
	uint64_t L2_AMP_CONFIDENCE_DPT3 : 6;
	uint64_t pad1 : 8;
	uint64_t L2_LLC_STREAM_DEMAND_DENSITY_XQ : 3;
    uint64_t pad2 : 1;
};
static_assert(sizeof(struct msr1322_reg) == sizeof(uint64_t),
              "msr1321_s is missing padding bits");

struct msr1323_reg
{
	uint64_t pad0 : 34;
	uint64_t L2_STREAM_AMP_CREATE_SWPFRFO : 1;
	uint64_t L2_STREAM_AMP_CREATE_SWPFRD : 1;
	uint64_t pad1 : 1;
	uint64_t L2_STREAM_AMP_CREATE_HWPFD : 1;
	uint64_t L2_STREAM_AMP_CREATE_DRFO : 1;
	uint64_t STABILIZE_PREF_ON_SWPFRFO : 1;
	uint64_t STABILIZE_PREF_ON_SWPFRD : 1;
	uint64_t STABILIZE_PREF_ON_IL1 : 1;
	uint64_t pad2 : 1;
	uint64_t STABILIZE_PREF_ON_HWPFD : 1;
	uint64_t STABILIZE_PREF_ON_DRFO : 1;
	uint64_t L2_STREAM_AMP_CREATE_PFNPP : 1;
	uint64_t L2_STREAM_AMP_CREATE_PFIPP : 1;
	uint64_t STABILIZE_PREF_ON_PFNPP : 1;
	uint64_t STABILIZE_PREF_ON_PFIPP : 1;
    uint64_t pad3 : 15;
};
static_assert(sizeof(struct msr1323_reg) == sizeof(uint64_t),
              "msr1323_s is missing padding bits");

struct msr1324_reg
{
	uint64_t pad0 : 54;
	uint64_t L1_HOMELESS_THRESHOLD : 8;
    uint64_t pad1 : 2;
};
static_assert(sizeof(struct msr1324_reg) == sizeof(uint64_t),
              "msr1324_s is missing padding bits");


#endif
