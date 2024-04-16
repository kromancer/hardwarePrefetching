#define _GNU_SOURCE
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
#include <stdatomic.h>
#include <sys/mman.h>

#include "msr.h"
#include "pmu.h"
#include "log.h"

#define TAG "MSR"

#define HWPF_MSR_BASE_1 (0x1320)
#define HWPF_MSR_FIELDS_IN_BASE_1 (5)

#define HWPF_MSR_BASE_2 (0x1A4)
#define HWPF_MSR_FIELDS_IN_BASE_2 (1)

#define DISABLE (1)
#define ENABLE  (0)

static_assert(HWPF_MSR_FIELDS_IN_BASE_1 + HWPF_MSR_FIELDS_IN_BASE_2 == HWPF_MSR_FIELDS,
              "Mismatch between HWPF_MSR_FIELDS and HWPF_MSR_FIELDS_IN_BASE_1 + HWPF_MSR_FIELDS_IN_BASE_2");

int msr_open(int core, union msr_u msr[]) {
    int msr_file;
	char filename[128];

	sprintf(filename, "/dev/cpu/%d/msr", core);
	msr_file = open(filename, O_RDWR);

	if (msr_file < 0){
        loge(TAG, "Could not open MSR file %s, running as root/sudo?\n", filename);
		exit(-1);
	}

    return msr_file;

}

//
// Read MSR values
//
int msr_hwpf_read(int msr_file, union msr_u msr[]) {
    int field_num = 0;
    for(int i = 0; i < HWPF_MSR_FIELDS_IN_BASE_1; i++, field_num++){
		if(pread(msr_file, &msr[field_num], 8, HWPF_MSR_BASE_1 + i) != 8){
            loge(TAG, "Could not read MSR 0x%X\n", HWPF_MSR_BASE_1 + i);
            return -1;
		}
	}

	for(int i = 0; i < HWPF_MSR_FIELDS_IN_BASE_2; i++, field_num++){
		if(pread(msr_file, &msr[i], 8, HWPF_MSR_BASE_2 + i) != 8){
            loge(TAG, "Could not read MSR 0x%X\n", HWPF_MSR_BASE_2 + i);
            return -1;
		}
	}

    return 0;
}

//
// Write new HWPF MSR values
//
int msr_hwpf_write(int msr_file, union msr_u msr[])
{
    for(int i = 0; i < HWPF_MSR_FIELDS_IN_BASE_1; i++){
		if(pwrite(msr_file, &msr[i], 8, HWPF_MSR_BASE_1 + i) != 8){
			loge(TAG, "Could not write MSR %d\n", HWPF_MSR_BASE_1 + i);
			return -1;
		}
	}

	for(int i = 0; i < HWPF_MSR_FIELDS_IN_BASE_2; i++){
		if(pwrite(msr_file, &msr[i], 8, HWPF_MSR_BASE_2 + i) != 8){
			loge(TAG, "Could not write MSR %d\n", HWPF_MSR_BASE_2 + i);
			return -1;
		}
	}

	return 0;
}

//
// Open and read MSR values
//
int msr_int(int core, union msr_u msr[])
{
    int msr_file = msr_open(core, msr);

    if (msr_hwpf_read(msr_file, msr) < 0) {
        loge(TAG, "Is %d an atom core?\n", core);
        exit(-1);
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

// Set value in MSR table
int msr_set_l2xq(union msr_u msr[], int value)
{
	msr[0].msr1320.L2_STREAM_AMP_XQ_THRESHOLD = value;

	return 0;
}

int msr_get_l2xq(union msr_u msr[])
{
	return msr[0].msr1320.L2_STREAM_AMP_XQ_THRESHOLD;
}

// Set value in MSR table
int msr_set_l3xq(union msr_u msr[], int value)
{
	msr[0].msr1320.LLC_STREAM_XQ_THRESHOLD = value;

	return 0;
}

int msr_get_l3xq(union msr_u msr[])
{
	return msr[0].msr1320.LLC_STREAM_XQ_THRESHOLD;
}

// Set value in MSR table
int msr_set_l2maxdist(union msr_u msr[], int value)
{
	msr[0].msr1320.L2_STREAM_MAX_DISTANCE = value;

	return 0;
}

int msr_get_l2maxdist(union msr_u msr[])
{
	return msr[0].msr1320.L2_STREAM_MAX_DISTANCE;
}

// Set value in MSR table
int msr_set_l3maxdist(union msr_u msr[], int value)
{
	msr[0].msr1320.LLC_STREAM_MAX_DISTANCE = value;

	return 0;
}

int msr_get_l3maxdist(union msr_u msr[])
{
	return msr[0].msr1320.LLC_STREAM_MAX_DISTANCE;
}

int msr_disable_l1ipp(union msr_u msr[])
{
    int val_before = msr[5].msr1A4.L1_IPP_DISABLE;
    msr[5].msr1A4.L1_IPP_DISABLE = DISABLE;

    atomic_thread_fence(memory_order_seq_cst);

    return val_before;
}

int msr_enable_l1ipp(union msr_u msr[])
{
    int val_before = msr[5].msr1A4.L1_IPP_DISABLE;
    msr[5].msr1A4.L1_IPP_DISABLE = ENABLE;

    atomic_thread_fence(memory_order_seq_cst);

    return val_before;
}

int msr_disable_l1npp(union msr_u msr[])
{
    int val_before = msr[5].msr1A4.L1_NPP_DISABLE;
    msr[5].msr1A4.L1_NPP_DISABLE = DISABLE;

    atomic_thread_fence(memory_order_seq_cst);

    return val_before;
}

int msr_enable_l1npp(union msr_u msr[])
{
    int val_before = msr[5].msr1A4.L1_NPP_DISABLE;
    msr[5].msr1A4.L1_NPP_DISABLE = ENABLE;

    atomic_thread_fence(memory_order_seq_cst);

    return val_before;
}

int msr_disable_l2stream(union msr_u msr[])
{
    int val_before = msr[5].msr1A4.L2_STREAM_DISABLE;
    msr[5].msr1A4.L2_STREAM_DISABLE = DISABLE;

    atomic_thread_fence(memory_order_seq_cst);

    return val_before;
}

int msr_enable_l2stream(union msr_u msr[])
{
    int val_before = msr[5].msr1A4.L2_STREAM_DISABLE;
    msr[5].msr1A4.L2_STREAM_DISABLE = ENABLE;

    atomic_thread_fence(memory_order_seq_cst);

    return val_before;
}
