/*
 * Copyright (c) 2004, PADL Software Pty Ltd.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of PADL Software nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY PADL SOFTWARE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL PADL SOFTWARE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 *  glue routine for _gsskrb5_inquire_sec_context_by_oid
 */

#include "gsskrb5_locl.h"

RCSID("$Id$");

static OM_uint32
set_compat_des3_mic_context_option
           (OM_uint32 *minor_status,
            gss_ctx_id_t *context_handle,
            const gss_buffer_t value)
{
    gsskrb5_ctx ctx;
    const char *p;

    if (*context_handle == GSS_C_NO_CONTEXT) {
	*minor_status = EINVAL;
	return GSS_S_NO_CONTEXT;
    }

    if (value->value == NULL || value->length != 1) {
	*minor_status = EINVAL;
	return GSS_S_FAILURE;
    }
    p = (const char *)value->value;

    ctx = (gsskrb5_ctx)*context_handle;
    HEIMDAL_MUTEX_lock(&ctx->ctx_id_mutex);
    if (*p) {
	ctx->more_flags |= COMPAT_OLD_DES3;
    } else {
	ctx->more_flags &= ~COMPAT_OLD_DES3;
    }
    ctx->more_flags |= COMPAT_OLD_DES3_SELECTED;
    HEIMDAL_MUTEX_unlock(&ctx->ctx_id_mutex);

    return GSS_S_COMPLETE;
}

OM_uint32
_gsskrb5_set_sec_context_option
           (OM_uint32 *minor_status,
            gss_ctx_id_t *context_handle,
            const gss_OID desired_object,
            const gss_buffer_t value)
{
    GSSAPI_KRB5_INIT ();

    if (value == GSS_C_NO_BUFFER) {
	*minor_status = EINVAL;
	return GSS_S_FAILURE;
    }

    if (gss_oid_equal(desired_object, GSS_KRB5_COMPAT_DES3_MIC_X)) {
	return set_compat_des3_mic_context_option(minor_status,
						  context_handle,
						  value);
    }

    *minor_status = EINVAL;
    return GSS_S_FAILURE;
}
