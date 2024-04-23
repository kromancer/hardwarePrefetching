#define _GNU_SOURCE

#include "msr.h"

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <fcntl.h>
#include <getopt.h>
#include <pthread.h>
#include <sys/mman.h>

#include "pmu.h"
#include "log.h"

int msr_addr[MSR_COUNT] = {
    0x1320,
    0x1321,
    0x1322,
    0x1323,
    0x1324,
    0x1A4
};

enum INDEX {
    INDEX_1320 = 0,
    INDEX_1321 = 1,
    INDEX_1322 = 2,
    INDEX_1323 = 3,
    INDEX_1324 = 4,
    INDEX_1A4 = 5,
    COUNT
};
static_assert(COUNT == MSR_COUNT, "Missing msr index for msr_t");

#define TAG "MSR"

#define DISABLE (1)
#define ENABLE  (0)

int msr_open(int core)
{
    int msr_file;
	char filename[128];

	sprintf(filename, "/dev/cpu/%d/msr", core);
	msr_file = open(filename, O_RDWR);

	if (msr_file < 0)
    {
        const char *err = strerror(errno);
        loge(TAG, "Could not open MSR file %s: %s\n", filename, err);
        return -errno;
	}

    return msr_file;
}

static int msr_read(int msr_file, union msr_u *reg, uint64_t addr)
{
    if(pread(msr_file, reg, 8, addr) != 8)
    {
        const char *err = strerror(errno);
        loge(TAG, "Could not read MSR 0x%X: %s\n", addr, err);
        return -errno;
    }

    return 0;
}

static int msr_write(int msr_file, union msr_u *reg, uint64_t addr)
{
    if(pwrite(msr_file, reg, 8, addr) != 8)
    {
        const char *err = strerror(errno);
        loge(TAG, "Could not write MSR 0x%X: %s\n", addr, err);
        return -errno;
    }

    return 0;
}

int msr_read_all(int msr_file, msr_t msr)
{
    for (int i = 0; i < MSR_COUNT; i++) {
        int ret = msr_read(msr_file, &msr[i], msr_addr[i]);
        if ( ret < 0)
            return ret;
    }

    return 0;
}

int msr_write_all(int msr_file, msr_t msr)
{
    for (int i = 0; i < MSR_COUNT; i++) {
        int ret = msr_write(msr_file, &msr[i], msr_addr[i]);
        if ( ret < 0)
            return ret;
    }

    return 0;
}

int msr_init(int core, msr_t msr)
{
    int msr_file = msr_open(core);
    if (msr_file < 0) {
        return msr_file;
    }

    int ret = msr_read_all(msr_file, msr);
    if (ret < 0) {
        loge(TAG, "Is %d an atom core?\n", core);
        return ret;
    }

	return msr_file;
}

//
// Write new HWPF MSR values
int msr_corepmu_setup(int msr_file, int nr_events, uint64_t *event)
{
	if(nr_events > PMU_COUNTERS){
		loge(TAG, "Too many PMU events, max is %d\n", PMU_COUNTERS);
		exit(-1);
	}

	for(int i = 0; i < nr_events; i++){
		if(pwrite(msr_file, &event[i], 8, PMU_PERFEVTSEL0 + i) != 8){
			loge(TAG, "Could not write MSR %d\n", PMU_PERFEVTSEL0 + i);
			return -1;
		}
	}

	return 0;
}

int msr_corepmu_read(int msr_file, int nr_events, uint64_t *result)
{
	if(nr_events > PMU_COUNTERS){
		loge(TAG, "Too many PMU events, max is %d\n", PMU_COUNTERS);
		exit(-1);
	}

	for(int i = 0; i < nr_events; i++){
		if(pread(msr_file, &result[i], 8, PMU_PMC0 + i) != 8){
			loge(TAG, "Could not read MSR 0x%x\n", PMU_PMC0 + i);
			exit(-1);
		}
	}

	return 0;
}

void msr_set_l2xq(msr_t msr, int value)
{
	msr[INDEX_1320].msr1320.L2_STREAM_AMP_XQ_THRESHOLD = value;
}

int msr_get_l2xq(msr_t msr)
{
	return msr[INDEX_1320].msr1320.L2_STREAM_AMP_XQ_THRESHOLD;
}

void msr_set_l3xq(msr_t msr, int value)
{
	msr[INDEX_1320].msr1320.LLC_STREAM_XQ_THRESHOLD = value;
}

int msr_get_l3xq(msr_t msr)
{
	return msr[INDEX_1320].msr1320.LLC_STREAM_XQ_THRESHOLD;
}

void msr_set_l2maxdist(msr_t msr, int value)
{
	msr[INDEX_1320].msr1320.L2_STREAM_MAX_DISTANCE = value;
}

int msr_get_l2maxdist(union msr_u msr[])
{
	return msr[INDEX_1320].msr1320.L2_STREAM_MAX_DISTANCE;
}

void msr_set_l3maxdist(union msr_u msr[], int value)
{
	msr[INDEX_1320].msr1320.LLC_STREAM_MAX_DISTANCE = value;
}

int msr_get_l3maxdist(union msr_u msr[])
{
	return msr[INDEX_1320].msr1320.LLC_STREAM_MAX_DISTANCE;
}

int msr_disable_l1ipp(msr_t msr)
{
    int val_before = msr[INDEX_1A4].msr1A4.L1_IPP_DISABLE;
    msr[INDEX_1A4].msr1A4.L1_IPP_DISABLE = DISABLE;
    return val_before;
}

int msr_enable_l1ipp(msr_t msr)
{
    int val_before = msr[INDEX_1A4].msr1A4.L1_IPP_DISABLE;
    msr[INDEX_1A4].msr1A4.L1_IPP_DISABLE = ENABLE;
    return val_before;
}

int msr_disable_l1npp(msr_t msr)
{
    int val_before = msr[INDEX_1A4].msr1A4.L1_NPP_DISABLE;
    msr[INDEX_1A4].msr1A4.L1_NPP_DISABLE = DISABLE;
    return val_before;
}

int msr_enable_l1npp(msr_t msr)
{
    int val_before = msr[INDEX_1A4].msr1A4.L1_NPP_DISABLE;
    msr[INDEX_1A4].msr1A4.L1_NPP_DISABLE = ENABLE;
    return val_before;
}

int msr_disable_l2stream(msr_t msr)
{
    int val_before = msr[INDEX_1A4].msr1A4.L2_STREAM_DISABLE;
    msr[INDEX_1A4].msr1A4.L2_STREAM_DISABLE = DISABLE;
    return val_before;
}

int msr_enable_l2stream(msr_t msr)
{
    int val_before = msr[INDEX_1A4].msr1A4.L2_STREAM_DISABLE;
    msr[INDEX_1A4].msr1A4.L2_STREAM_DISABLE = ENABLE;
    return val_before;
}

int msr_disable_l2amp(msr_t msr)
{
    int val_before = msr[INDEX_1A4].msr1A4.L2_AMP_DISABLE;
    msr[INDEX_1A4].msr1A4.L2_AMP_DISABLE = DISABLE;
    return val_before;
}

int msr_enable_l2amp(msr_t msr)
{
    int val_before = msr[INDEX_1A4].msr1A4.L2_AMP_DISABLE;
    msr[INDEX_1A4].msr1A4.L2_AMP_DISABLE = ENABLE;
    return val_before;
}

int msr_disable_llcstream(msr_t msr)
{
    int val_before = msr[0].msr1320.LLC_STREAM_DISABLE;
    msr[0].msr1320.LLC_STREAM_DISABLE = DISABLE;
    return val_before;
}

int msr_enable_llcstream(msr_t msr)
{
    int val_before = msr[0].msr1320.LLC_STREAM_DISABLE;
    msr[0].msr1320.LLC_STREAM_DISABLE = ENABLE;
    return val_before;
}
