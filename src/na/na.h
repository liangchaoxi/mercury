/**
 * Copyright (c) 2013-2021 UChicago Argonne, LLC and The HDF Group.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef NA_H
#define NA_H

#include "na_types.h"

/*************************************/
/* Public Type and Struct Definition */
/*************************************/

/* See na_types.h */

/*****************/
/* Public Macros */
/*****************/

/* See na_types.h */

/*********************/
/* Public Prototypes */
/*********************/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get NA version number.
 *
 * \param major [OUT]           pointer to unsigned integer
 * \param minor [OUT]           pointer to unsigned integer
 * \param patch [OUT]           pointer to unsigned integer
 */
NA_PUBLIC void
NA_Version_get(unsigned int *major, unsigned int *minor, unsigned int *patch);

/**
 * Initialize the network abstraction layer.
 * Must be finalized with NA_Finalize().
 *
 * \param info_string [IN]      host address with port number (e.g.,
 *                              "tcp://localhost:3344" or
 *                              "bmi+tcp://localhost:3344")
 * \param listen [IN]           listen for incoming connections
 *
 * \return Pointer to NA class or NULL in case of failure
 */
NA_PUBLIC na_class_t *
NA_Initialize(const char *info_string, bool listen) NA_WARN_UNUSED_RESULT;

/**
 * Initialize the network abstraction layer with options provided by init_info.
 * Must be finalized with NA_Finalize().
 *
 * \param info_string [IN]      host address with port number (e.g.,
 *                              "tcp://localhost:3344" or
 *                              "bmi+tcp://localhost:3344")
 * \param listen [IN]           listen for incoming connections
 * \param na_init_info [IN]     (Optional) NA init info, NULL if no info
 *
 * \return Pointer to NA class or NULL in case of failure
 */
NA_PUBLIC na_class_t *
NA_Initialize_opt(const char *info_string, bool listen,
    const struct na_init_info *na_init_info) NA_WARN_UNUSED_RESULT;

/**
 * Finalize the network abstraction layer.
 *
 * \param na_class [IN/OUT]     pointer to NA class
 *
 * \return NA_SUCCESS or corresponding NA error code
 */
NA_PUBLIC na_return_t
NA_Finalize(na_class_t *na_class);

/**
 * Clean up all temporary files that were created in previous NA instances.
 * While temporary resources (e.g., tmp files) are cleaned up on a call
 * to NA_Finalize(), this routine gives a chance to programs that terminate
 * abnormally to easily clean up those resources. This includes instances
 * from all plugins.
 */
NA_PUBLIC void
NA_Cleanup(void);

/**
 * Set the log level for NA. That setting is valid for all NA classes.
 *
 * \param level [IN]            level string, valid values are:
 *                                "none", "error", "warning", "debug"
 */
NA_PUBLIC void
NA_Set_log_level(const char *level);

/**
 * Return the name of the NA class.
 *
 * \param na_class [IN]         pointer to NA class
 *
 * \return Pointer to NA class name or NULL in case of failure
 */
static NA_INLINE const char *
NA_Get_class_name(const na_class_t *na_class) NA_WARN_UNUSED_RESULT;

/**
 * Return the protocol of the NA class.
 *
 * \param na_class [IN]         pointer to NA class
 *
 * \return Pointer to NA class protocol or NULL in case of failure
 */
static NA_INLINE const char *
NA_Get_class_protocol(const na_class_t *na_class) NA_WARN_UNUSED_RESULT;

/**
 * Test whether class is listening or not.
 *
 * \param na_class [IN]         pointer to NA class
 *
 * \return true if listening or false if not
 */
static NA_INLINE bool
NA_Is_listening(const na_class_t *na_class) NA_WARN_UNUSED_RESULT;

/**
 * Create a new context.
 *
 * \param na_class [IN/OUT]     pointer to NA class
 *
 * \return Pointer to NA context or NULL in case of failure
 */
NA_PUBLIC na_context_t *
NA_Context_create(na_class_t *na_class) NA_WARN_UNUSED_RESULT;

/**
 * Create a new context with a specific ID.
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param id [IN]               context ID
 *
 * \return Pointer to NA context or NULL in case of failure
 */
NA_PUBLIC na_context_t *
NA_Context_create_id(na_class_t *na_class, uint8_t id) NA_WARN_UNUSED_RESULT;

/**
 * Destroy a context created by using NA_Context_create().
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param context [IN/OUT]      pointer to context of execution
 *
 * \return NA_SUCCESS or corresponding NA error code
 */
NA_PUBLIC na_return_t
NA_Context_destroy(na_class_t *na_class, na_context_t *context);

/**
 * Allocate an operation ID for the higher level layer to save and
 * pass back to the NA layer rather than have the NA layer allocate operation
 * IDs all the time.
 * Allocating an operation ID gives ownership of that ID to the higher level
 * layer, hence it must be explicitly released with NA_Op_destroy() when it
 * is no longer needed.
 *
 * \param na_class [IN/OUT]     pointer to NA class
 *
 * \return valid pointer to operation ID or NULL
 */
NA_PUBLIC na_op_id_t *
NA_Op_create(na_class_t *na_class);

/**
 * Destroy operation ID created with NA_Op_create().
 * Reference counting prevents involuntary free.
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param op_id [IN]            pointer to operation ID
 *
 * \return NA_SUCCESS or corresponding NA error code
 */
NA_PUBLIC na_return_t
NA_Op_destroy(na_class_t *na_class, na_op_id_t *op_id);

/**
 * Lookup an addr from a peer address/name. Addresses need to be
 * freed by calling NA_Addr_free().
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param name [IN]             lookup name
 * \param addr [OUT]            pointer to abstract address
 *
 * \return NA_SUCCESS or corresponding NA error code
 */
NA_PUBLIC na_return_t
NA_Addr_lookup(na_class_t *na_class, const char *name, na_addr_t *addr);

/**
 * Free the addr from the list of peers.
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param addr [IN]             abstract address
 *
 * \return NA_SUCCESS or corresponding NA error code
 */
NA_PUBLIC na_return_t
NA_Addr_free(na_class_t *na_class, na_addr_t addr);

/**
 * Hint that the address is no longer valid. This may happen if the peer is
 * no longer responding. This can be used to force removal of the
 * peer address from the list of the peers, before freeing it and reclaim
 * resources.
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param addr [IN]             abstract address
 *
 * \return NA_SUCCESS or corresponding NA error code
 */
NA_PUBLIC na_return_t
NA_Addr_set_remove(na_class_t *na_class, na_addr_t addr);

/**
 * Access self address.
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param addr [OUT]            pointer to abstract address
 *
 * \return NA_SUCCESS or corresponding NA error code
 */
NA_PUBLIC na_return_t
NA_Addr_self(na_class_t *na_class, na_addr_t *addr);

/**
 * Duplicate an existing NA abstract address. The duplicated address can be
 * stored for later use and the origin address be freed safely. The duplicated
 * address must be freed with NA_Addr_free().
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param addr [IN]             abstract address
 * \param new_addr [OUT]        pointer to abstract address
 *
 * \return NA_SUCCESS or corresponding NA error code
 */
NA_PUBLIC na_return_t
NA_Addr_dup(na_class_t *na_class, na_addr_t addr, na_addr_t *new_addr);

/**
 * Compare two addresses.
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param addr1 [IN]            abstract address
 * \param addr2 [IN]            abstract address
 *
 * \return true if addresses are determined to be equal, false otherwise
 */
NA_PUBLIC bool
NA_Addr_cmp(na_class_t *na_class, na_addr_t addr1, na_addr_t addr2);

/**
 * Test whether address is self or not.
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param addr [IN]             abstract address
 *
 * \return true if self or false if not
 */
static NA_INLINE bool
NA_Addr_is_self(na_class_t *na_class, na_addr_t addr);

/**
 * Convert an addr to a string (returned string includes the terminating
 * null byte '\0'). If buf is NULL, the address is not converted and only
 * the required size of the buffer is returned. If the input value passed
 * through buf_size is too small, NA_OVERFLOW is returned and the buf_size
 * output is set to the minimum size required.
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param buf [IN/OUT]          pointer to destination buffer
 * \param buf_size [IN/OUT]     pointer to buffer size
 * \param addr [IN]             abstract address
 *
 * \return NA_SUCCESS or corresponding NA error code
 */
NA_PUBLIC na_return_t
NA_Addr_to_string(
    na_class_t *na_class, char *buf, size_t *buf_size, na_addr_t addr);

/**
 * Get size required to serialize address.
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param addr [IN]             abstract address
 *
 * \return Non-negative value
 */
static NA_INLINE size_t
NA_Addr_get_serialize_size(
    na_class_t *na_class, na_addr_t addr) NA_WARN_UNUSED_RESULT;

/**
 * Serialize address into a buffer.
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param buf [IN/OUT]          pointer to buffer used for serialization
 * \param buf_size [IN]         buffer size
 * \param addr [IN]             abstract address
 *
 * \return NA_SUCCESS or corresponding NA error code
 */
NA_PUBLIC na_return_t
NA_Addr_serialize(
    na_class_t *na_class, void *buf, size_t buf_size, na_addr_t addr);

/**
 * Deserialize address from a buffer. The returned address must be freed with
 * NA_Addr_free().
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param addr [OUT]            pointer to abstract address
 * \param buf [IN]              pointer to buffer used for deserialization
 * \param buf_size [IN]         buffer size
 *
 * \return NA_SUCCESS or corresponding NA error code
 */
NA_PUBLIC na_return_t
NA_Addr_deserialize(
    na_class_t *na_class, na_addr_t *addr, const void *buf, size_t buf_size);

/**
 * Get the maximum size of messages supported by unexpected send/recv.
 * Small message size.
 *
 * \param na_class [IN]         pointer to NA class
 *
 * \return Non-negative value
 */
static NA_INLINE size_t
NA_Msg_get_max_unexpected_size(
    const na_class_t *na_class) NA_WARN_UNUSED_RESULT;

/**
 * Get the maximum size of messages supported by expected send/recv.
 * Small message size that may differ from the unexpected message size.
 *
 * \param na_class [IN]         pointer to NA class
 *
 * \return Non-negative value
 */
static NA_INLINE size_t
NA_Msg_get_max_expected_size(const na_class_t *na_class) NA_WARN_UNUSED_RESULT;

/**
 * Get the header size for unexpected messages. Plugins may use that header
 * to encode specific information (such as source addr, etc).
 *
 * \param na_class [IN]         pointer to NA class
 *
 * \return Non-negative value
 */
static NA_INLINE size_t
NA_Msg_get_unexpected_header_size(
    const na_class_t *na_class) NA_WARN_UNUSED_RESULT;

/**
 * Get the header size for expected messages. Plugins may use that header
 * to encode specific information.
 *
 * \param na_class [IN]         pointer to NA class
 *
 * \return Non-negative value
 */
static NA_INLINE size_t
NA_Msg_get_expected_header_size(
    const na_class_t *na_class) NA_WARN_UNUSED_RESULT;

/**
 * Get the maximum tag value that can be used by send/recv (both expected and
 * unexpected).
 *
 * \param na_class [IN]         pointer to NA class
 *
 * \return Non-negative value
 */
static NA_INLINE na_tag_t
NA_Msg_get_max_tag(const na_class_t *na_class) NA_WARN_UNUSED_RESULT;

/**
 * Allocate buf_size bytes and return a pointer to the allocated memory.
 * If size is 0, NA_Msg_buf_alloc() returns NULL. The plugin_data output
 * parameter can be used by the underlying plugin implementation to store
 * internal memory information.
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param buf_size [IN]         buffer size
 * \param plugin_data [OUT]     pointer to internal plugin data
 *
 * \return Pointer to allocated memory or NULL in case of failure
 */
NA_PUBLIC void *
NA_Msg_buf_alloc(na_class_t *na_class, size_t buf_size,
    void **plugin_data) NA_WARN_UNUSED_RESULT;

/**
 * The NA_Msg_buf_free() function releases the memory space pointed to by buf,
 * which must have been returned by a previous call to NA_Msg_buf_alloc().
 * If buf is NULL, no operation is performed.
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param buf [IN]              pointer to buffer
 * \param plugin_data [IN]      pointer to internal plugin data
 *
 * \return NA_SUCCESS or corresponding NA error code
 */
NA_PUBLIC na_return_t
NA_Msg_buf_free(na_class_t *na_class, void *buf, void *plugin_data);

/**
 * Initialize a buffer so that it can be safely passed to the
 * NA_Msg_send_unexpected() call. In the case the underlying plugin adds its
 * own header to that buffer, the header will be written at this time and the
 * usable buffer payload will be buf + NA_Msg_get_unexpected_header_size().
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param buf [IN]              pointer to buffer
 * \param buf_size [IN]         buffer size
 *
 * \return NA_SUCCESS or corresponding NA error code
 */
NA_PUBLIC na_return_t
NA_Msg_init_unexpected(na_class_t *na_class, void *buf, size_t buf_size);

/**
 * Send an unexpected message to dest_addr. Unexpected sends do not require a
 * matching receive to complete. After completion, the user callback is
 * placed into the context completion queue and can be triggered using
 * NA_Trigger().
 * The plugin_data parameter returned from the NA_Msg_buf_alloc() call must
 * be passed along with the buffer, it allows plugins to store and retrieve
 * additional buffer information such as memory descriptors.
 * \remark Note also that unexpected messages do not require an unexpected
 * receive to be posted at the destination before sending the message and the
 * destination is allowed to drop the message without notification. However,
 * in general, NA plugins are encouraged to remain reliable to avoid unnecessary
 * timeouts and cancelations.
 *
 * Users must manually create an operation ID through NA_Op_create() and pass
 * it through op_id for future use and prevent multiple ID creation.
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param context [IN/OUT]      pointer to context of execution
 * \param callback [IN]         pointer to function callback
 * \param arg [IN]              pointer to data passed to callback
 * \param buf [IN]              pointer to send buffer
 * \param buf_size [IN]         buffer size
 * \param plugin_data [IN]      pointer to internal plugin data
 * \param dest_addr [IN]        abstract address of destination
 * \param dest_id [IN]          destination context ID
 * \param tag [IN]              tag attached to message
 * \param op_id [IN/OUT]        pointer to operation ID
 *
 * \return NA_SUCCESS or corresponding NA error code
 */
static NA_INLINE na_return_t
NA_Msg_send_unexpected(na_class_t *na_class, na_context_t *context,
    na_cb_t callback, void *arg, const void *buf, size_t buf_size,
    void *plugin_data, na_addr_t dest_addr, uint8_t dest_id, na_tag_t tag,
    na_op_id_t *op_id);

/**
 * Receive an unexpected message. Unexpected receives may wait on any tag and
 * any source depending on the implementation. After completion, the user
 * callback parameter is placed into the context completion queue and can be
 * triggered using NA_Trigger().
 * The plugin_data parameter returned from the NA_Msg_buf_alloc() call must
 * be passed along with the buffer, it allows plugins to store and retrieve
 * additional buffer information such as memory descriptors.
 *
 * Users must manually create an operation ID through NA_Op_create() and pass
 * it through op_id for future use and prevent multiple ID creation.
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param context [IN/OUT]      pointer to context of execution
 * \param callback [IN]         pointer to function callback
 * \param arg [IN]              pointer to data passed to callback
 * \param buf [IN]              pointer to send buffer
 * \param buf_size [IN]         buffer size
 * \param plugin_data [IN]      pointer to internal plugin data
 * \param op_id [IN/OUT]        pointer to operation ID
 *
 * \return NA_SUCCESS or corresponding NA error code
 */
static NA_INLINE na_return_t
NA_Msg_recv_unexpected(na_class_t *na_class, na_context_t *context,
    na_cb_t callback, void *arg, void *buf, size_t buf_size, void *plugin_data,
    na_op_id_t *op_id);

/**
 * Initialize a buffer so that it can be safely passed to the
 * NA_Msg_send_expected() call. In the case the underlying plugin adds its
 * own header to that buffer, the header will be written at this time and the
 * usable buffer payload will be buf + NA_Msg_get_expected_header_size().
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param buf [IN]              pointer to buffer
 * \param buf_size [IN]         buffer size
 *
 * \return NA_SUCCESS or corresponding NA error code
 */
NA_PUBLIC na_return_t
NA_Msg_init_expected(na_class_t *na_class, void *buf, size_t buf_size);

/**
 * Send an expected message to dest_addr. After completion, the user callback is
 * placed into the context completion queue and can be triggered using
 * NA_Trigger().
 * The plugin_data parameter returned from the NA_Msg_buf_alloc() call must
 * be passed along with the buffer, it allows plugins to store and retrieve
 * additional buffer information such as memory descriptors.
 * \remark Note that expected messages require an expected receive to be posted
 * at the destination before sending the message, otherwise the destination is
 * allowed to drop the message without notification.
 *
 * Users must manually create an operation ID through NA_Op_create() and pass
 * it through op_id for future use and prevent multiple ID creation.
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param context [IN/OUT]      pointer to context of execution
 * \param callback [IN]         pointer to function callback
 * \param arg [IN]              pointer to data passed to callback
 * \param buf [IN]              pointer to send buffer
 * \param buf_size [IN]         buffer size
 * \param plugin_data [IN]      pointer to internal plugin data
 * \param dest_addr [IN]        abstract address of destination
 * \param dest_id [IN]          destination context ID
 * \param tag [IN]              tag attached to message
 * \param op_id [IN/OUT]        pointer to operation ID
 *
 * \return NA_SUCCESS or corresponding NA error code
 */
static NA_INLINE na_return_t
NA_Msg_send_expected(na_class_t *na_class, na_context_t *context,
    na_cb_t callback, void *arg, const void *buf, size_t buf_size,
    void *plugin_data, na_addr_t dest_addr, uint8_t dest_id, na_tag_t tag,
    na_op_id_t *op_id);

/**
 * Receive an expected message from source_addr. After completion, the user
 * callback is placed into the context completion queue and can be triggered
 * using NA_Trigger().
 * The plugin_data parameter returned from the NA_Msg_buf_alloc() call must
 * be passed along with the buffer, it allows plugins to store and retrieve
 * additional buffer information such as memory descriptors.
 *
 * Users must manually create an operation ID through NA_Op_create() and pass
 * it through op_id for future use and prevent multiple ID creation.
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param context [IN/OUT]      pointer to context of execution
 * \param callback [IN]         pointer to function callback
 * \param arg [IN]              pointer to data passed to callback
 * \param buf [IN]              pointer to receive buffer
 * \param buf_size [IN]         buffer size
 * \param plugin_data [IN]      pointer to internal plugin data
 * \param source_addr [IN]      abstract address of source
 * \param source_id [IN]        source context ID
 * \param tag [IN]              matching tag used to receive message
 * \param op_id [IN/OUT]        pointer to operation ID
 *
 * \return NA_SUCCESS or corresponding NA error code
 */
static NA_INLINE na_return_t
NA_Msg_recv_expected(na_class_t *na_class, na_context_t *context,
    na_cb_t callback, void *arg, void *buf, size_t buf_size, void *plugin_data,
    na_addr_t source_addr, uint8_t source_id, na_tag_t tag, na_op_id_t *op_id);

/**
 * Create memory handle for RMA operations.
 * For non-contiguous memory, use NA_Mem_handle_create_segments() instead.
 *
 * \remark Note to plugin developers: NA_Mem_handle_create() may be called
 * multiple times on the same memory region.
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param buf [IN]              pointer to buffer that needs to be registered
 * \param buf_size [IN]         buffer size
 * \param flags [IN]            permission flag:
 *                                - NA_MEM_READWRITE
 *                                - NA_MEM_READ_ONLY
 * \param mem_handle [OUT]      pointer to returned abstract memory handle
 *
 * \return NA_SUCCESS or corresponding NA error code
 */
NA_PUBLIC na_return_t
NA_Mem_handle_create(na_class_t *na_class, void *buf, size_t buf_size,
    unsigned long flags, na_mem_handle_t *mem_handle);

/**
 * Create memory handle for RMA operations.
 * Create_segments can be used to register scatter-gather lists and get a single
 * memory handle.
 * \remark Implemented only if the network transport or hardware supports it.
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param segments [IN]         pointer to array of segments composed of:
 *                                - address of the segment that needs to be
 *                                  registered
 *                                - size of the segment in bytes
 * \param segment_count [IN]    segment count
 * \param flags [IN]            permission flag:
 *                                - NA_MEM_READWRITE
 *                                - NA_MEM_READ_ONLY
 * \param mem_handle [OUT]      pointer to returned abstract memory handle
 *
 * \return NA_SUCCESS or corresponding NA error code
 */
NA_PUBLIC na_return_t
NA_Mem_handle_create_segments(na_class_t *na_class, struct na_segment *segments,
    size_t segment_count, unsigned long flags, na_mem_handle_t *mem_handle);

/**
 * Free memory handle.
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param mem_handle [IN]       abstract memory handle
 *
 * \return NA_SUCCESS or corresponding NA error code
 */
NA_PUBLIC na_return_t
NA_Mem_handle_free(na_class_t *na_class, na_mem_handle_t mem_handle);

/**
 * Get the maximum segment count that can be passed to
 * NA_Mem_handle_create_segments().
 *
 * \param na_class [IN]         pointer to NA class
 *
 * \return Non-negative value
 */
static NA_INLINE size_t
NA_Mem_handle_get_max_segments(
    const na_class_t *na_class) NA_WARN_UNUSED_RESULT;

/**
 * Register memory for RMA operations.
 * Memory pieces must be registered before one-sided transfers can be
 * initiated.
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param mem_handle [IN]       pointer to abstract memory handle
 * \param mem_type [IN]         memory type (default: NA_MEM_TYPE_HOST)
 * \param device [IN]           (optional) device ID if memory type is used
 *
 * \return NA_SUCCESS or corresponding NA error code
 */
NA_PUBLIC na_return_t
NA_Mem_register(na_class_t *na_class, na_mem_handle_t mem_handle,
    enum na_mem_type mem_type, uint64_t device);

/**
 * Unregister memory.
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param mem_handle [IN]       abstract memory handle
 *
 * \return NA_SUCCESS or corresponding NA error code
 */
NA_PUBLIC na_return_t
NA_Mem_deregister(na_class_t *na_class, na_mem_handle_t mem_handle);

/**
 * Get size required to serialize handle.
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param mem_handle [IN]       abstract memory handle
 *
 * \return Non-negative value
 */
static NA_INLINE size_t
NA_Mem_handle_get_serialize_size(
    na_class_t *na_class, na_mem_handle_t mem_handle) NA_WARN_UNUSED_RESULT;

/**
 * Serialize memory handle into a buffer.
 * One-sided transfers require prior exchange of memory handles between
 * peers, serialization callbacks can be used to "pack" a memory handle and
 * send it across the network.
 * \remark Memory handles can be variable size, therefore the space required
 * to serialize a handle into a buffer can be obtained using
 * NA_Mem_handle_get_serialize_size().
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param buf [IN/OUT]          pointer to buffer used for serialization
 * \param buf_size [IN]         buffer size
 * \param mem_handle [IN]       abstract memory handle
 *
 * \return NA_SUCCESS or corresponding NA error code
 */
NA_PUBLIC na_return_t
NA_Mem_handle_serialize(na_class_t *na_class, void *buf, size_t buf_size,
    na_mem_handle_t mem_handle);

/**
 * Deserialize memory handle from buffer.
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param mem_handle [OUT]      pointer to abstract memory handle
 * \param buf [IN]              pointer to buffer used for deserialization
 * \param buf_size [IN]         buffer size
 *
 * \return NA_SUCCESS or corresponding NA error code
 */
NA_PUBLIC na_return_t
NA_Mem_handle_deserialize(na_class_t *na_class, na_mem_handle_t *mem_handle,
    const void *buf, size_t buf_size);

/**
 * Put data to remote address.
 * Initiate a put to the registered memory regions with the given offset/size.
 * After completion, the user callback is placed into a completion queue and
 * can be triggered using NA_Trigger().
 * \remark Memory must be registered and handles exchanged between peers.
 *
 * Users must manually create an operation ID through NA_Op_create() and pass
 * it through op_id for future use and prevent multiple ID creation.
 *
 * \param na_class [IN/OUT]      pointer to NA class
 * \param context [IN/OUT]       pointer to context of execution
 * \param callback [IN]          pointer to function callback
 * \param arg [IN]               pointer to data passed to callback
 * \param local_mem_handle [IN]  abstract local memory handle
 * \param local_offset [IN]      local offset
 * \param remote_mem_handle [IN] abstract remote memory handle
 * \param remote_offset [IN]     remote offset
 * \param data_size [IN]         size of data that needs to be transferred
 * \param remote_addr [IN]       abstract address of remote destination
 * \param remote_id [IN]         target ID of remote destination
 * \param op_id [IN/OUT]         pointer to operation ID
 *
 * \return NA_SUCCESS or corresponding NA error code
 */
static NA_INLINE na_return_t
NA_Put(na_class_t *na_class, na_context_t *context, na_cb_t callback, void *arg,
    na_mem_handle_t local_mem_handle, na_offset_t local_offset,
    na_mem_handle_t remote_mem_handle, na_offset_t remote_offset,
    size_t data_size, na_addr_t remote_addr, uint8_t remote_id,
    na_op_id_t *op_id);

/**
 * Get data from remote address.
 * Initiate a get to the registered memory regions with the given offset/size.
 * After completion, the user callback is placed into a completion queue and
 * can be triggered using NA_Trigger().
 *
 * Users must manually create an operation ID through NA_Op_create() and pass
 * it through op_id for future use and prevent multiple ID creation.
 *
 * \param na_class [IN/OUT]      pointer to NA class
 * \param context [IN/OUT]       pointer to context of execution
 * \param callback [IN]          pointer to function callback
 * \param arg [IN]               pointer to data passed to callback
 * \param local_mem_handle [IN]  abstract local memory handle
 * \param local_offset [IN]      local offset
 * \param remote_mem_handle [IN] abstract remote memory handle
 * \param remote_offset [IN]     remote offset
 * \param data_size [IN]         size of data that needs to be transferred
 * \param remote_addr [IN]       abstract address of remote source
 * \param remote_id [IN]         target ID of remote source
 * \param op_id [IN/OUT]         pointer to operation ID
 *
 * \return NA_SUCCESS or corresponding NA error code
 */
static NA_INLINE na_return_t
NA_Get(na_class_t *na_class, na_context_t *context, na_cb_t callback, void *arg,
    na_mem_handle_t local_mem_handle, na_offset_t local_offset,
    na_mem_handle_t remote_mem_handle, na_offset_t remote_offset,
    size_t data_size, na_addr_t remote_addr, uint8_t remote_id,
    na_op_id_t *op_id);

/**
 * Retrieve file descriptor from NA plugin when supported. The descriptor
 * can be used by upper layers for manual polling through the usual
 * OS select/poll/epoll calls.
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param context [IN/OUT]      pointer to context of execution
 *
 * \return Non-negative integer if supported, 0 if not implemented and negative
 * in case of error.
 */
static NA_INLINE int
NA_Poll_get_fd(
    na_class_t *na_class, na_context_t *context) NA_WARN_UNUSED_RESULT;

/**
 * Used to signal when it is safe to block on the class/context poll descriptor
 * or if there is already work that can be progressed.
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param context [IN/OUT]      pointer to context of execution
 *
 * \return true if it is safe to block or false otherwise
 */
NA_PUBLIC bool
NA_Poll_try_wait(na_class_t *na_class, na_context_t *context);

/*NA_Trigger和NA_Progress是两个用来处理网络通信活动的重要函数。它们是Network Abstraction（NA）层的一部分，
这一层为Mercury提供了一个统一的网络接口，用来屏蔽不同网络协议的详细差异，以便于Mercury可以在各种不同的网络
环境中操作。na_return_t是一个枚举，定义了NA层函数可能返回的值，表明函数的执行结果，例如成功、失败或超时等。*/

/**
 调用这个函数是为了推动通信的进展，确保异步操作（如非阻塞发送和接收）能够继续处理。
 例如，该函数可能涉及处理网络队列中的事件、处理传入的消息以及处理任何其他需要被驱动的网络通信操作。
 * Try to progress communication for at most timeout until timeout is reached or
 * any completion has occurred.
 * Progress should not be considered as wait, in the sense that it cannot be
 * assumed that completion of a specific operation will occur only when
 * progress is called.
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param context [IN/OUT]      pointer to context of execution
 * \param timeout [IN]          timeout (in milliseconds)
 *
 * \return NA_SUCCESS if any completion has occurred / NA error code otherwise
 */
NA_PUBLIC na_return_t
NA_Progress(na_class_t *na_class, na_context_t *context, unsigned int timeout);


/**
 这个函数用来触发已经准备好的（或已经完成的）网络通信事件的回调函数。
 *context  是网络通信的上下文。
 *timeout  是等待事件的最大超时时间（以毫秒为单位）。
 *max_count      是这次调用要触发的最大事件数量。
 *callback_ret[] 是一个输出数组，用于存储回调函数的返回值。
 *actual_count   是一个输出指针，用来返回实际触发的事件数量。
 *函数返回NA_SUCCESS如果至少一个回调被成功触发，如果未触发任何回调或者发生错误，则返回相应的NA错误代码。
 
 函数用于处理异步通信模式下的事件，当事件完成时（例如，消息被发送或收到），
 与事件相关联的回调函数将被调用。这样，NA层可以通知上层应用哪些事件已经发生，并执行相应的操作。

 * Execute at most max_count callbacks. If timeout is non-zero, wait up to
 * timeout before returning. Function can return when at least one or more
 * callbacks are triggered (at most max_count).
 *
 * \param context [IN/OUT]      pointer to context of execution
 * \param timeout [IN]          timeout (in milliseconds)
 * \param max_count [IN]        maximum number of callbacks triggered
 * \param callback_ret [IN/OUT] array of callback return values
 * \param actual_count [OUT]    actual number of callbacks triggered
 *
 * \return NA_SUCCESS or corresponding NA error code
 */
NA_PUBLIC na_return_t
NA_Trigger(na_context_t *context, unsigned int timeout, unsigned int max_count,
    int callback_ret[], unsigned int *actual_count);

/**
 * Cancel an ongoing operation.
 *
 * \param na_class [IN/OUT]     pointer to NA class
 * \param context [IN/OUT]      pointer to context of execution
 * \param op_id [IN]            pointer to operation ID
 *
 * \return NA_SUCCESS or corresponding NA error code
 */
NA_PUBLIC na_return_t
NA_Cancel(na_class_t *na_class, na_context_t *context, na_op_id_t *op_id);

/**
 * Convert error return code to string (null terminated).
 *
 * \param errnum [IN]           error return code
 *
 * \return String
 */
NA_PUBLIC const char *
NA_Error_to_string(na_return_t errnum) NA_WARN_UNUSED_RESULT;

/************************************/
/* Local Type and Struct Definition */
/************************************/

/* NA info definition */
struct na_info {
    char *protocol_name; /* Protocol (e.g., tcp, ib) */
    char *host_name;     /* Host (may be NULL in anonymous mode) */
    /* Additional init info (NULL if no info) */
    const struct na_init_info *na_init_info;
};

/* NA class definition */
struct na_class {
    const struct na_class_ops *ops; /* Class operations */
    void *plugin_class;             /* Plugin private class */
    char *protocol_name;            /* Name of protocol */
    uint8_t progress_mode;          /* NA progress mode */
    bool listen;                    /* Listen for connections */
};

/* NA context definition */
struct na_context {
    void *plugin_context; /* Plugin private context */
};

/* NA plugin callbacks */
struct na_class_ops {
    const char *class_name;
    bool (*check_protocol)(const char *protocol_name);
    na_return_t (*initialize)(
        na_class_t *na_class, const struct na_info *na_info, bool listen);
    na_return_t (*finalize)(na_class_t *na_class);
    void (*cleanup)(void);
    na_return_t (*context_create)(
        na_class_t *na_class, void **plugin_context, uint8_t id);
    na_return_t (*context_destroy)(na_class_t *na_class, void *plugin_context);
    na_op_id_t *(*op_create)(na_class_t *na_class);
    na_return_t (*op_destroy)(na_class_t *na_class, na_op_id_t *op_id);
    na_return_t (*addr_lookup)(
        na_class_t *na_class, const char *name, na_addr_t *addr);
    na_return_t (*addr_free)(na_class_t *na_class, na_addr_t addr);
    na_return_t (*addr_set_remove)(na_class_t *na_class, na_addr_t addr);
    na_return_t (*addr_self)(na_class_t *na_class, na_addr_t *addr);
    na_return_t (*addr_dup)(
        na_class_t *na_class, na_addr_t addr, na_addr_t *new_addr);
    bool (*addr_cmp)(na_class_t *na_class, na_addr_t addr1, na_addr_t addr2);
    bool (*addr_is_self)(na_class_t *na_class, na_addr_t addr);
    na_return_t (*addr_to_string)(
        na_class_t *na_class, char *buf, size_t *buf_size, na_addr_t addr);
    size_t (*addr_get_serialize_size)(na_class_t *na_class, na_addr_t addr);
    na_return_t (*addr_serialize)(
        na_class_t *na_class, void *buf, size_t buf_size, na_addr_t addr);
    na_return_t (*addr_deserialize)(na_class_t *na_class, na_addr_t *addr,
        const void *buf, size_t buf_size);
    size_t (*msg_get_max_unexpected_size)(const na_class_t *na_class);
    size_t (*msg_get_max_expected_size)(const na_class_t *na_class);
    size_t (*msg_get_unexpected_header_size)(const na_class_t *na_class);
    size_t (*msg_get_expected_header_size)(const na_class_t *na_class);
    na_tag_t (*msg_get_max_tag)(const na_class_t *na_class);
    void *(*msg_buf_alloc)(
        na_class_t *na_class, size_t buf_size, void **plugin_data);
    na_return_t (*msg_buf_free)(
        na_class_t *na_class, void *buf, void *plugin_data);
    na_return_t (*msg_init_unexpected)(
        na_class_t *na_class, void *buf, size_t buf_size);
    na_return_t (*msg_send_unexpected)(na_class_t *na_class,
        na_context_t *context, na_cb_t callback, void *arg, const void *buf,
        size_t buf_size, void *plugin_data, na_addr_t dest_addr,
        uint8_t dest_id, na_tag_t tag, na_op_id_t *op_id);
    na_return_t (*msg_recv_unexpected)(na_class_t *na_class,
        na_context_t *context, na_cb_t callback, void *arg, void *buf,
        size_t buf_size, void *plugin_data, na_op_id_t *op_id);
    na_return_t (*msg_init_expected)(
        na_class_t *na_class, void *buf, size_t buf_size);
    na_return_t (*msg_send_expected)(na_class_t *na_class,
        na_context_t *context, na_cb_t callback, void *arg, const void *buf,
        size_t buf_size, void *plugin_data, na_addr_t dest_addr,
        uint8_t dest_id, na_tag_t tag, na_op_id_t *op_id);
    na_return_t (*msg_recv_expected)(na_class_t *na_class,
        na_context_t *context, na_cb_t callback, void *arg, void *buf,
        size_t buf_size, void *plugin_data, na_addr_t source_addr,
        uint8_t source_id, na_tag_t tag, na_op_id_t *op_id);
    na_return_t (*mem_handle_create)(na_class_t *na_class, void *buf,
        size_t buf_size, unsigned long flags, na_mem_handle_t *mem_handle);
    na_return_t (*mem_handle_create_segments)(na_class_t *na_class,
        struct na_segment *segments, size_t segment_count, unsigned long flags,
        na_mem_handle_t *mem_handle);
    na_return_t (*mem_handle_free)(
        na_class_t *na_class, na_mem_handle_t mem_handle);
    size_t (*mem_handle_get_max_segments)(const na_class_t *na_class);
    na_return_t (*mem_register)(na_class_t *na_class,
        na_mem_handle_t mem_handle, enum na_mem_type mem_type, uint64_t device);
    na_return_t (*mem_deregister)(
        na_class_t *na_class, na_mem_handle_t mem_handle);
    size_t (*mem_handle_get_serialize_size)(
        na_class_t *na_class, na_mem_handle_t mem_handle);
    na_return_t (*mem_handle_serialize)(na_class_t *na_class, void *buf,
        size_t buf_size, na_mem_handle_t mem_handle);
    na_return_t (*mem_handle_deserialize)(na_class_t *na_class,
        na_mem_handle_t *mem_handle, const void *buf, size_t buf_size);
    na_return_t (*put)(na_class_t *na_class, na_context_t *context,
        na_cb_t callback, void *arg, na_mem_handle_t local_mem_handle,
        na_offset_t local_offset, na_mem_handle_t remote_mem_handle,
        na_offset_t remote_offset, size_t length, na_addr_t remote_addr,
        uint8_t remote_id, na_op_id_t *op_id);
    na_return_t (*get)(na_class_t *na_class, na_context_t *context,
        na_cb_t callback, void *arg, na_mem_handle_t local_mem_handle,
        na_offset_t local_offset, na_mem_handle_t remote_mem_handle,
        na_offset_t remote_offset, size_t length, na_addr_t remote_addr,
        uint8_t remote_id, na_op_id_t *op_id);
    int (*na_poll_get_fd)(na_class_t *na_class, na_context_t *context);
    bool (*na_poll_try_wait)(na_class_t *na_class, na_context_t *context);
    na_return_t (*progress)(
        na_class_t *na_class, na_context_t *context, unsigned int timeout);
    na_return_t (*cancel)(
        na_class_t *na_class, na_context_t *context, na_op_id_t *op_id);
};

/*---------------------------------------------------------------------------*/
static NA_INLINE const char *
NA_Get_class_name(const na_class_t *na_class)
{
    return na_class->ops->class_name;
}

/*---------------------------------------------------------------------------*/
static NA_INLINE const char *
NA_Get_class_protocol(const na_class_t *na_class)
{
    return na_class->protocol_name;
}

/*---------------------------------------------------------------------------*/
static NA_INLINE bool
NA_Is_listening(const na_class_t *na_class)
{
    return na_class->listen;
}

/*---------------------------------------------------------------------------*/
static NA_INLINE bool
NA_Addr_is_self(na_class_t *na_class, na_addr_t addr)
{
    return na_class->ops->addr_is_self(na_class, addr);
}

/*---------------------------------------------------------------------------*/
static NA_INLINE size_t
NA_Addr_get_serialize_size(na_class_t *na_class, na_addr_t addr)
{
    return (na_class->ops->addr_get_serialize_size)
               ? na_class->ops->addr_get_serialize_size(na_class, addr)
               : 0;
}

/*---------------------------------------------------------------------------*/
static NA_INLINE size_t
NA_Msg_get_max_unexpected_size(const na_class_t *na_class)
{
    return na_class->ops->msg_get_max_unexpected_size(na_class);
}

/*---------------------------------------------------------------------------*/
static NA_INLINE size_t
NA_Msg_get_max_expected_size(const na_class_t *na_class)
{
    return na_class->ops->msg_get_max_expected_size(na_class);
}

/*---------------------------------------------------------------------------*/
static NA_INLINE size_t
NA_Msg_get_unexpected_header_size(const na_class_t *na_class)
{
    return (na_class->ops->msg_get_unexpected_header_size)
               ? na_class->ops->msg_get_unexpected_header_size(na_class)
               : 0;
}

/*---------------------------------------------------------------------------*/
static NA_INLINE size_t
NA_Msg_get_expected_header_size(const na_class_t *na_class)
{
    return (na_class->ops->msg_get_expected_header_size)
               ? na_class->ops->msg_get_expected_header_size(na_class)
               : 0;
}

/*---------------------------------------------------------------------------*/
static NA_INLINE na_tag_t
NA_Msg_get_max_tag(const na_class_t *na_class)
{
    return na_class->ops->msg_get_max_tag(na_class);
}

/*---------------------------------------------------------------------------*/
static NA_INLINE na_return_t
NA_Msg_send_unexpected(na_class_t *na_class, na_context_t *context,
    na_cb_t callback, void *arg, const void *buf, size_t buf_size,
    void *plugin_data, na_addr_t dest_addr, uint8_t dest_id, na_tag_t tag,
    na_op_id_t *op_id)
{
    return na_class->ops->msg_send_unexpected(na_class, context, callback, arg,
        buf, buf_size, plugin_data, dest_addr, dest_id, tag, op_id);
}

/*---------------------------------------------------------------------------*/
static NA_INLINE na_return_t
NA_Msg_recv_unexpected(na_class_t *na_class, na_context_t *context,
    na_cb_t callback, void *arg, void *buf, size_t buf_size, void *plugin_data,
    na_op_id_t *op_id)
{
    return na_class->ops->msg_recv_unexpected(
        na_class, context, callback, arg, buf, buf_size, plugin_data, op_id);
}

/*---------------------------------------------------------------------------*/
static NA_INLINE na_return_t
NA_Msg_send_expected(na_class_t *na_class, na_context_t *context,
    na_cb_t callback, void *arg, const void *buf, size_t buf_size,
    void *plugin_data, na_addr_t dest_addr, uint8_t dest_id, na_tag_t tag,
    na_op_id_t *op_id)
{
    return na_class->ops->msg_send_expected(na_class, context, callback, arg,
        buf, buf_size, plugin_data, dest_addr, dest_id, tag, op_id);
}

/*---------------------------------------------------------------------------*/
static NA_INLINE na_return_t
NA_Msg_recv_expected(na_class_t *na_class, na_context_t *context,
    na_cb_t callback, void *arg, void *buf, size_t buf_size, void *plugin_data,
    na_addr_t source_addr, uint8_t source_id, na_tag_t tag, na_op_id_t *op_id)
{
    return na_class->ops->msg_recv_expected(na_class, context, callback, arg,
        buf, buf_size, plugin_data, source_addr, source_id, tag, op_id);
}

/*---------------------------------------------------------------------------*/
static NA_INLINE size_t
NA_Mem_handle_get_max_segments(const na_class_t *na_class)
{
    return (na_class->ops->mem_handle_get_max_segments)
               ? na_class->ops->mem_handle_get_max_segments(na_class)
               : 1;
}

/*---------------------------------------------------------------------------*/
static NA_INLINE size_t
NA_Mem_handle_get_serialize_size(
    na_class_t *na_class, na_mem_handle_t mem_handle)
{
    return na_class->ops->mem_handle_get_serialize_size(na_class, mem_handle);
}

/*---------------------------------------------------------------------------*/
static NA_INLINE na_return_t
NA_Put(na_class_t *na_class, na_context_t *context, na_cb_t callback, void *arg,
    na_mem_handle_t local_mem_handle, na_offset_t local_offset,
    na_mem_handle_t remote_mem_handle, na_offset_t remote_offset,
    size_t data_size, na_addr_t remote_addr, uint8_t remote_id,
    na_op_id_t *op_id)
{
    return na_class->ops->put(na_class, context, callback, arg,
        local_mem_handle, local_offset, remote_mem_handle, remote_offset,
        data_size, remote_addr, remote_id, op_id);
}

/*---------------------------------------------------------------------------*/
static NA_INLINE na_return_t
NA_Get(na_class_t *na_class, na_context_t *context, na_cb_t callback, void *arg,
    na_mem_handle_t local_mem_handle, na_offset_t local_offset,
    na_mem_handle_t remote_mem_handle, na_offset_t remote_offset,
    size_t data_size, na_addr_t remote_addr, uint8_t remote_id,
    na_op_id_t *op_id)
{
    return na_class->ops->get(na_class, context, callback, arg,
        local_mem_handle, local_offset, remote_mem_handle, remote_offset,
        data_size, remote_addr, remote_id, op_id);
}

/*---------------------------------------------------------------------------*/
static NA_INLINE int
NA_Poll_get_fd(na_class_t *na_class, na_context_t *context)
{
    return (na_class->ops->na_poll_get_fd)
               ? na_class->ops->na_poll_get_fd(na_class, context)
               : -1;
}

#ifdef __cplusplus
}
#endif

#endif /* NA_H */
