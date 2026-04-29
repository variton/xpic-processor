
#ifndef JERR_H
#define JERR_H

#include <csetjmp>
#include <cstddef>
#include <cstdio>

extern "C" {
#include <jerror.h>
#include <jpeglib.h>
}

namespace img {

/**
 * @brief JPEG error manager with jump buffer and formatted message storage.
 *
 * @details
 * Extends libjpeg's @c jpeg_error_mgr with a @c setjmp/@c longjmp recovery
 * buffer and a fixed-size buffer for the formatted error message.
 */
struct JpegError : jpeg_error_mgr {
  /** @brief Jump buffer used for non-local error recovery. */
  jmp_buf setjmp_buf;

  /** @brief Buffer holding the formatted libjpeg error message. */
  char message[JMSG_LENGTH_MAX];
};

/**
 * @brief Libjpeg fatal error callback.
 *
 * @details
 * Formats the current libjpeg error message into @ref JpegError::message and
 * transfers control back to the caller via @c longjmp.
 *
 * @param cinfo Pointer to the libjpeg common structure.
 *
 * @warning This function performs a non-local jump using @c longjmp. The caller
 *          is expected to establish a matching @c setjmp context.
 */
static void jpeg_error_exit(j_common_ptr cinfo) {
  auto *err = reinterpret_cast<JpegError *>(cinfo->err);
  (*cinfo->err->format_message)(cinfo, err->message);
  longjmp(err->setjmp_buf, 1);
}

} // namespace img

#endif
