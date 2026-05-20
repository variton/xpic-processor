
#ifndef FILE_WRITER_H
#define FILE_WRITER_H

#include <cstdio>
#include <memory>
#include <string>

#include <error_info.h>
#include <errty.h>
#include <nc.h>
#include <tl/expected.hpp>

/**
 * @file file_writer.h
 * @brief Defines RAII utilities for owning and accessing C file handles.
 */

namespace fio {

/**
 * @brief Custom deleter for @c FILE pointers.
 *
 * Closes the file with @c fclose when the pointer is non-null.
 */
struct FileCloser {
  /**
   * @brief Closes the given file handle.
   *
   * @param f File handle to close. No action is taken if null.
   */
  void operator()(FILE *f) const noexcept {
    if (f)
      fclose(f);
  }
};

/**
 * @brief Alias to the default non-copyable wrapper type.
 *
 * @tparam T Wrapped type.
 */
template <typename T> using Default = core::NC<T>;

/**
 * @brief Owning smart pointer for @c FILE handles.
 *
 * Uses @ref FileCloser to automatically close the file.
 */
using FilePtr = std::unique_ptr<FILE, FileCloser>;

/**
 * @brief Error codes produced by @ref FileHandler.
 */
enum class FileHandlerError {
  /** @brief The file could not be opened. */
  FileOpenError,
};

/**
 * @brief Structured error information for @ref FileHandler operations.
 */
using FileHandlerErrorInfo = err::ErrorInfo<FileHandlerError>;

static_assert(topology::ErrorInfoTy<FileHandlerErrorInfo>);

/**
 * @brief RAII wrapper for an owned C @c FILE handle.
 *
 * @details
 * `FileHandler` manages exclusive ownership of a file handle and ensures it is
 * closed automatically when ownership ends. Instances are created through
 * @ref open.
 */
class FileHandler {

public:
  /**
   * @brief Opens a file and returns an owning handler on success.
   *
   * @param path Path to the file to open.
   * @param mode File open mode passed to @c fopen.
   * @return `tl::expected` containing a @ref FileHandler on success, or
   *         @ref FileHandlerErrorInfo on failure.
   *
   * @retval FileHandlerError::FileOpenError The file could not be opened.
   */
  static tl::expected<FileHandler, FileHandlerErrorInfo>
  open(const std::string &path, const char *mode) noexcept;

  /**
   * @brief Returns the underlying file handle without releasing ownership.
   *
   * @return Managed raw @c FILE pointer, or null if no file is owned.
   */
  FILE *get() const noexcept;

  /**
   * @brief Releases ownership of the underlying file handle.
   *
   * @return Owning smart pointer containing the released @c FILE handle.
   *
   * @post This object no longer owns a file handle.
   */
  FilePtr release() noexcept;

private:
  FilePtr fp_;

  /**
   * @brief Constructs a handler from an already opened file handle.
   *
   * @param f File handle to take ownership of.
   */
  explicit FileHandler(FILE *f) noexcept;
};

} // namespace fio

#endif
