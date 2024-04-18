#ifndef __MSR_H
#define __MSR_H

#include <stdint.h>

#include "atom_msr.h"

#define HWPF_MSR_FIELDS (6)

int msr_corepmu_setup(int msr_file, int nr_events, uint64_t *event);
int msr_corepmu_read(int msr_file, int nr_events, uint64_t *result);

int msr_int(int core, union msr_u msr[]);

int msr_open(int core, union msr_u msr[]);

int msr_hwpf_read(int msr_file, union msr_u msr[]);
int msr_hwpf_write(int msr_file, union msr_u msr[]);

int msr_disable_l1ipp(union msr_u msr[]);
int msr_enable_l1ipp(union msr_u msr[]);

int msr_disable_l1npp(union msr_u msr[]);
int msr_enable_l1npp(union msr_u msr[]);

int msr_disable_l2stream(union msr_u msr[]);
int msr_enable_l2stream(union msr_u msr[]);

int msr_set_l2xq(union msr_u msr[], int value);
int msr_get_l2xq(union msr_u msr[]);
int msr_set_l3xq(union msr_u msr[], int value);
int msr_get_l3xq(union msr_u msr[]);

int msr_set_l2maxdist(union msr_u msr[], int value);
int msr_get_l2maxdist(union msr_u msr[]);
int msr_set_l3maxdist(union msr_u msr[], int value);
int msr_get_l3maxdist(union msr_u msr[]);

#endif
