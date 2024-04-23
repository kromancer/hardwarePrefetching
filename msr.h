#ifndef __MSR_H
#define __MSR_H

#include <stdint.h>

#include "atom_msr.h"

#define MSR_COUNT (6)

union msr_u
{
    struct msr1320_reg msr1320;
	struct msr1321_reg msr1321;
	struct msr1322_reg msr1322;
	struct msr1323_reg msr1323;
	struct msr1324_reg msr1324;
    struct msr1A4_reg  msr1A4;
};

typedef union msr_u msr_t[MSR_COUNT];

int msr_corepmu_setup(int msr_file, int nr_events, uint64_t *event);
int msr_corepmu_read(int msr_file, int nr_events, uint64_t *result);

int msr_init(int core, msr_t msr);

int msr_open(int core);
int msr_read_all(int msr_file, msr_t msr);
int msr_write_all(int msr_file, msr_t msr);

int msr_disable_l1ipp(msr_t msr);
int msr_enable_l1ipp(msr_t msr);

int msr_disable_l1npp(msr_t msr);
int msr_enable_l1npp(msr_t msr);

int msr_disable_l2stream(msr_t msr);
int msr_enable_l2stream(msr_t msr);

int msr_disable_l2amp(msr_t msr);
int msr_enable_l2amp(msr_t msr);

int msr_disable_llcstream(msr_t msr);
int msr_enable_llcstream(msr_t msr);

void msr_set_l2xq(msr_t msr, int value);
int  msr_get_l2xq(msr_t msr);

void msr_set_l3xq(msr_t msr, int value);
int  msr_get_l3xq(msr_t msr);

void msr_set_l2maxdist(msr_t msr, int value);
int  msr_get_l2maxdist(msr_t msr);

void msr_set_l3maxdist(msr_t msr, int value);
int  msr_get_l3maxdist(msr_t msr);

#endif
