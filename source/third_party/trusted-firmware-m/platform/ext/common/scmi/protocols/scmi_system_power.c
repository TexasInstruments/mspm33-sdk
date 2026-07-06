/*
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "scmi_common.h"
#include "scmi_protocol.h"
#include "scmi_system_power.h"

/*
 Bits[31:1] Reserved, must be zero
 Bit[0]     Graceful request
 */
#define SYSTEM_POWER_STATE_SET_FLAGS_MASK (1U << 0)

scmi_protocol_err_t scmi_message_sys_power_protocol_version(
    struct scmi_message_t *msg)
{
    if (msg == NULL) {
        return SCMI_PROTOCOL_INVALID_ARGUMENT;
    }

    msg->header = scmi_message_header(SCMI_MESSAGE_ID_SYS_POWER_PROTOCOL_VERSION,
                                      SCMI_MESSAGE_TYPE_COMMAND,
                                      SCMI_PROTOCOL_ID_SYS_POWER_STATE,
                                      0);

    msg->payload_len = 0; /* this message has no payload*/

    return SCMI_PROTOCOL_SUCCESS;
}

scmi_protocol_err_t scmi_message_sys_power_state_set(
    struct scmi_message_t *msg,
    uint32_t flags,
    uint32_t system_state)
{
    if (msg == NULL) {
        return SCMI_PROTOCOL_INVALID_ARGUMENT;
    }
    if ((flags & ~SYSTEM_POWER_STATE_SET_FLAGS_MASK) > 0) {
        return SCMI_PROTOCOL_INVALID_ARGUMENT;
    }
    if ((system_state >= 0x05) && (system_state <= 0x7FFFFFFF)) {
        /* Reserved range, must not be used */
        return SCMI_PROTOCOL_INVALID_ARGUMENT;
    }

    msg->header = scmi_message_header(SCMI_MESSAGE_ID_SYS_POWER_STATE_SET,
                                      SCMI_MESSAGE_TYPE_COMMAND,
                                      SCMI_PROTOCOL_ID_SYS_POWER_STATE,
                                      0);

    assert(sizeof(struct scmi_sys_power_state_set_t) <= sizeof(msg->payload));

    /* Copy the payload to the message structure */
    memcpy(msg->payload,
           &(struct scmi_sys_power_state_set_t) {
                .flags = flags,
                .system_state = system_state},
           sizeof(struct scmi_sys_power_state_set_t));

    /* Set the payload length */
    msg->payload_len = sizeof(struct scmi_sys_power_state_set_t);

    return SCMI_PROTOCOL_SUCCESS;
}

scmi_protocol_err_t scmi_message_sys_power_state_notify(
    struct scmi_message_t *msg)
{
    if (msg == NULL) {
        return SCMI_PROTOCOL_INVALID_ARGUMENT;
    }

    msg->header =
        scmi_message_header(SCMI_MESSAGE_ID_SYS_POWER_STATE_NOTIFY,
                            SCMI_MESSAGE_TYPE_COMMAND,
                            SCMI_PROTOCOL_ID_SYS_POWER_STATE,
                            0);

    assert(sizeof(struct scmi_sys_power_state_notify_t) <=
        sizeof(msg->payload));

    (void)memcpy(msg->payload,
        &(struct scmi_sys_power_state_notify_t) {
            .notify_enable = UINT32_C(1),
        },
        sizeof(struct scmi_sys_power_state_notify_t));

    msg->payload_len = sizeof(struct scmi_sys_power_state_notify_t);

    return SCMI_PROTOCOL_SUCCESS;
}
