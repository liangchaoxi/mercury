/**
 * Copyright (c) 2013-2021 UChicago Argonne, LLC and The HDF Group.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef NA_TYPES_H
#define NA_TYPES_H

#include "na_config.h"

#include <limits.h>

/*************************************/
/* Public Type and Struct Definition */
/*************************************/

typedef struct na_class na_class_t;     /* Opaque NA class */
typedef struct na_context na_context_t; /* Opaque NA execution context */
typedef struct na_addr *na_addr_t;      /* Abstract NA address */
typedef uint32_t na_tag_t;              /* Tag */
typedef struct na_op_id na_op_id_t;     /* Opaque operation id */

typedef struct na_mem_handle *na_mem_handle_t; /* Abstract memory handle */
typedef uint64_t na_offset_t;                  /* Offset */

/* Address format */
enum na_addr_format {
    NA_ADDR_UNSPEC, /* Leave it upon plugin to choose */
    NA_ADDR_IPV4,   /* Use IPv4 when available */
    NA_ADDR_IPV6,   /* Use IPv6 when available */
    NA_ADDR_NATIVE  /* Use native addressing when available */
};

/* Memory type */
enum na_mem_type {
    NA_MEM_TYPE_HOST, /*!< Default system memory */
    NA_MEM_TYPE_CUDA, /*!< NVIDIA CUDA memory */
    NA_MEM_TYPE_ROCM, /*!< AMD ROCM memory */
    NA_MEM_TYPE_ZE,   /*!< Intel Level Zero memory */
    NA_MEM_TYPE_MAX,
    NA_MEM_TYPE_UNKNOWN = NA_MEM_TYPE_MAX
};

/* Init info */
struct na_init_info {
    /* Preferred IP subnet to use. */
    const char *ip_subnet;

    /* Authorization key that can be used for communication. All processes
     * should use the same key in order to communicate.
     * NB. generation of keys is done through third-party libraries. */
    const char *auth_key;

    /* Max unexpected size hint that can be passed to control the size of
     * unexpected messages. Note that the underlying plugin library may switch
     * to different transfer protocols depending on the message size that is
     * used. */
    size_t max_unexpected_size;

    /* Max expected size hint that can be passed to control the size of
     * expected messages. Note that the underlying plugin library may switch
     * to different transfer protocols depending on the message size that is
     * used. */
    size_t max_expected_size;

    /* Progress mode flag. Setting NA_NO_BLOCK will force busy-spin on progress
     * and remove any wait/notification calls. */
    uint8_t progress_mode;

    /* Preferred address format. Default is NA_ADDR_UNSPEC. */
    enum na_addr_format addr_format;

    /* Maximum number of contexts that are expected to be created. */
    uint8_t max_contexts;

    /* Thread mode flags can be used to relax thread-safety when it is not
     * needed. When setting NA_THREAD_MODE_SINGLE, only a single thread should
     * access both NA classes and contexts at a time. */
    uint8_t thread_mode;

    /* Request support for tranfers to/from memory devices (e.g., GPU, etc).
     * Default is: false. */
    bool request_mem_device;
};

/* Segment */
struct na_segment {
    void *base; /* Address of the segment */
    size_t len; /* Size of the segment in bytes */
};

/* Return codes:
 * Functions return 0 for success or corresponding return code */
#define NA_RETURN_VALUES                                                       \
    X(NA_SUCCESS)        /*!< operation succeeded */                           \
    X(NA_PERMISSION)     /*!< operation not permitted */                       \
    X(NA_NOENTRY)        /*!< no such file or directory */                     \
    X(NA_INTERRUPT)      /*!< operation interrupted */                         \
    X(NA_AGAIN)          /*!< operation must be retried */                     \
    X(NA_NOMEM)          /*!< out of memory */                                 \
    X(NA_ACCESS)         /*!< permission denied */                             \
    X(NA_FAULT)          /*!< bad address */                                   \
    X(NA_BUSY)           /*!< device or resource busy */                       \
    X(NA_EXIST)          /*!< entry already exists */                          \
    X(NA_NODEV)          /*!< no such device */                                \
    X(NA_INVALID_ARG)    /*!< invalid argument */                              \
    X(NA_PROTOCOL_ERROR) /*!< protocol error */                                \
    X(NA_OVERFLOW)       /*!< value too large */                               \
    X(NA_MSGSIZE)        /*!< message size too long */                         \
    X(NA_PROTONOSUPPORT) /*!< protocol not supported */                        \
    X(NA_OPNOTSUPPORTED) /*!< operation not supported on endpoint */           \
    X(NA_ADDRINUSE)      /*!< address already in use */                        \
    X(NA_ADDRNOTAVAIL)   /*!< cannot assign requested address */               \
    X(NA_HOSTUNREACH)    /*!< cannot reach host during operation */            \
    X(NA_TIMEOUT)        /*!< operation reached timeout */                     \
    X(NA_CANCELED)       /*!< operation canceled */                            \
    X(NA_RETURN_MAX)

#define X(a) a,
typedef enum na_return { NA_RETURN_VALUES } na_return_t;
#undef X

/* Callback operation type */
#define NA_CB_TYPES                                                            \
    X(NA_CB_SEND_UNEXPECTED) /*!< unexpected send callback */                  \
    X(NA_CB_RECV_UNEXPECTED) /*!< unexpected recv callback */                  \
    X(NA_CB_SEND_EXPECTED)   /*!< expected send callback */                    \
    X(NA_CB_RECV_EXPECTED)   /*!< expected recv callback */                    \
    X(NA_CB_PUT)             /*!< put callback */                              \
    X(NA_CB_GET)             /*!< get callback */                              \
    X(NA_CB_MAX)

#define X(a) a,
typedef enum na_cb_type { NA_CB_TYPES } na_cb_type_t;
#undef X

/* Callback info structs */
struct na_cb_info_recv_unexpected {
    size_t actual_buf_size;
    na_addr_t source;
    na_tag_t tag;
};

struct na_cb_info_recv_expected {
    size_t actual_buf_size;
};

/* Callback info struct */
struct na_cb_info {
    union { /* Union of callback info structures */
        struct na_cb_info_recv_unexpected recv_unexpected;
        struct na_cb_info_recv_expected recv_expected;
    } info;
    void *arg;         /* User data */
    na_cb_type_t type; /* Callback type */
    na_return_t ret;   /* Return value */
};

/* Callback type */
typedef int (*na_cb_t)(const struct na_cb_info *callback_info);

/*****************/
/* Public Macros */
/*****************/

/* Constant values */
#define NA_ADDR_NULL       ((na_addr_t) 0)
#define NA_MEM_HANDLE_NULL ((na_mem_handle_t) 0)

/* Max timeout */
#define NA_MAX_IDLE_TIME (3600 * 1000)

/* Context ID max value
 * \remark This is not the user limit but only the limit imposed by the type */
#define NA_CONTEXT_ID_MAX UINT8_MAX

/* Tag max value
 * \remark This is not the user limit but only the limit imposed by the type */
#define NA_TAG_MAX UINT_MAX

/* The memory attributes associated with the memory handle
 * can be defined as read only, write only or read/write */
#define NA_MEM_READ_ONLY  0x01
#define NA_MEM_WRITE_ONLY 0x02
#define NA_MEM_READWRITE  0x03

/* Progress modes */
#define NA_NO_BLOCK 0x01 /*!< no blocking progress */
#define NA_NO_RETRY 0x02 /*!< no retry of operations in progress */

/* Thread modes (default is thread-safe) */
#define NA_THREAD_MODE_SINGLE_CLS                                              \
    (0x01) /*!< only one thread will access class */
#define NA_THREAD_MODE_SINGLE_CTX                                              \
    (0x02) /*!< only one thread will access context */
#define NA_THREAD_MODE_SINGLE                                                  \
    (NA_THREAD_MODE_SINGLE_CLS | NA_THREAD_MODE_SINGLE_CTX)

/* NA init info initializer */
#define NA_INIT_INFO_INITIALIZER                                               \
    (struct na_init_info)                                                      \
    {                                                                          \
        .ip_subnet = NULL, .auth_key = NULL, .max_unexpected_size = 0,         \
        .max_expected_size = 0, .progress_mode = 0, .max_contexts = 1,         \
        .thread_mode = 0, .request_mem_device = false                          \
    }

#endif /* NA_TYPES_H */
