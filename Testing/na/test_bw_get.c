/**
 * Copyright (c) 2013-2021 UChicago Argonne, LLC and The HDF Group.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "na_test_perf.h"

/****************/
/* Local Macros */
/****************/
#define BENCHMARK_NAME "NA_Get() Bandwidth"

/************************************/
/* Local Type and Struct Definition */
/************************************/

/********************/
/* Local Prototypes */
/********************/

static na_return_t
na_test_perf_run(struct na_test_perf_info *info, size_t buf_size, size_t skip);

/*******************/
/* Local Variables */
/*******************/

/*---------------------------------------------------------------------------*/
static na_return_t
na_test_perf_run(struct na_test_perf_info *info, size_t buf_size, size_t skip)
{
    hg_time_t t1, t2;
    na_return_t ret;
    size_t i, j;

    if (info->na_test_info.mpi_comm_size > 1)
        NA_Test_barrier(&info->na_test_info);

    /* Actual benchmark */
    for (i = 0; i < skip + (size_t) info->na_test_info.loop; i++) {
        struct na_test_perf_rma_info rma_info = {.request = info->request,
            .complete_count = 0,
            .expected_count = (int32_t) info->rma_count};

        if (i == skip)
            hg_time_get_current(&t1);

        hg_request_reset(info->request);

        if (info->na_test_info.verify)
            memset(info->rma_buf, 0, buf_size);

        if (info->na_test_info.force_register) {
            ret = NA_Mem_handle_create(info->na_class, info->rma_buf,
                info->rma_size_max * info->rma_count, NA_MEM_READWRITE,
                &info->local_handle);
            NA_TEST_CHECK_NA_ERROR(error, ret,
                "NA_Mem_handle_create() failed (%s)", NA_Error_to_string(ret));

            ret = NA_Mem_register(
                info->na_class, info->local_handle, NA_MEM_TYPE_HOST, 0);
            NA_TEST_CHECK_NA_ERROR(error, ret, "NA_Mem_register() failed (%s)",
                NA_Error_to_string(ret));
        }

        /* Post gets */
        for (j = 0; j < info->rma_count; j++) {
            ret = NA_Get(info->na_class, info->context,
                na_test_perf_rma_request_complete, &rma_info,
                info->local_handle, j * info->rma_size_max, info->remote_handle,
                j * info->rma_size_max, buf_size, info->target_addr, 0,
                info->rma_op_ids[j]);
            NA_TEST_CHECK_NA_ERROR(
                error, ret, "NA_Get() failed (%s)", NA_Error_to_string(ret));
        }

        hg_request_wait(info->request, NA_MAX_IDLE_TIME, NULL);

        if (info->na_test_info.verify) {
            for (j = 0; j < info->rma_count; j++) {
                ret = na_test_perf_verify_data(
                    (const char *) info->rma_buf + j * info->rma_size_max,
                    buf_size, 0);
                NA_TEST_CHECK_NA_ERROR(error, ret,
                    "na_test_perf_verify_data() failed (%s)",
                    NA_Error_to_string(ret));
            }
        }

        if (info->na_test_info.force_register) {
            NA_Mem_deregister(info->na_class, info->local_handle);
            NA_Mem_handle_free(info->na_class, info->local_handle);
            info->local_handle = NA_MEM_HANDLE_NULL;
        }
    }

    if (info->na_test_info.mpi_comm_size > 1)
        NA_Test_barrier(&info->na_test_info);

    hg_time_get_current(&t2);

    if (info->na_test_info.mpi_comm_rank == 0)
        na_test_perf_print_bw(info, buf_size, hg_time_subtract(t2, t1));

    return NA_SUCCESS;

error:
    return ret;
}

/*---------------------------------------------------------------------------*/
int
main(int argc, char *argv[])
{
    struct na_test_perf_info info;
    size_t size;
    na_return_t na_ret;

    /* Initialize the interface */
    na_ret = na_test_perf_init(argc, argv, false, &info);
    NA_TEST_CHECK_NA_ERROR(error, na_ret, "na_test_perf_init() failed (%s)",
        NA_Error_to_string(na_ret));

    /* Retrieve server memory handle */
    na_ret = na_test_perf_mem_handle_recv(&info, NA_TEST_PERF_TAG_GET);
    NA_TEST_CHECK_NA_ERROR(error, na_ret,
        "na_test_perf_mem_handle_recv() failed (%s)",
        NA_Error_to_string(na_ret));

    /* Header info */
    if (info.na_test_info.mpi_comm_rank == 0)
        na_test_perf_print_header_bw(&info, BENCHMARK_NAME);

    /* Msg with different sizes */
    for (size = info.rma_size_min; size <= info.rma_size_max; size *= 2) {
        na_ret = na_test_perf_run(&info, size,
            (size > NA_TEST_PERF_LARGE_SIZE) ? NA_TEST_PERF_BW_SKIP_LARGE
                                             : NA_TEST_PERF_BW_SKIP_SMALL);
        NA_TEST_CHECK_NA_ERROR(error, na_ret,
            "na_test_perf_run(%zu) failed (%s)", size,
            NA_Error_to_string(na_ret));
    }

    /* Finalize interface */
    if (info.na_test_info.mpi_comm_rank == 0)
        na_test_perf_send_finalize(&info);

    na_test_perf_cleanup(&info);

    return EXIT_SUCCESS;

error:
    na_test_perf_cleanup(&info);

    return EXIT_FAILURE;
}
