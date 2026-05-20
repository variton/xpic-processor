#include <cstdio>
#include <filehandler.h>

namespace fio {

/**
 * Constructor
 *
 * Takes ownership of an already-open FILE* and stores it in FilePtr.
 *
 * Ownership model:
 * - FileHandler owns the file through FilePtr
 * - FilePtr uses FileCloser as its custom deleter
 * - When the owning pointer is destroyed, FileCloser calls fclose()
 */
FileHandler::FileHandler(FILE *f) noexcept : fp_{f} {}

/**
 * Open a file and wrap it in a FileHandler.
 *
 * @param path Path to the file to open
 * @param mode C stdio open mode, e.g. "rb", "wb", "a"
 *
 * @return FileHandler on success
 * @return FileHandlerErrorInfo if fopen() fails
 *
 * Notes:
 * - Uses C stdio because downstream code expects FILE*
 * - Returned FileHandler owns the handle exclusively
 * - The file will be closed automatically via FileCloser
 */
tl::expected<FileHandler, FileHandlerErrorInfo>
FileHandler::open(const std::string &path, const char *mode) noexcept {
  // Attempt to open the file using the requested mode
  FILE *f = std::fopen(path.c_str(), mode);

  // Return a structured error instead of a null handle
  if (!f) {
    return tl::unexpected(FileHandlerErrorInfo{FileHandlerError::FileOpenError,
                                               "Cannot open file: " + path});
  }

  // Wrap the raw FILE* in RAII ownership
  return FileHandler{f};
}

/**
 * Get the underlying FILE* without transferring ownership.
 *
 * @return Raw FILE* managed by this object
 *
 * Important:
 * - This is non-owning access only
 * - Caller must not call fclose() on the returned pointer
 * - Pointer remains valid only while this FileHandler still owns it
 */
FILE *FileHandler::get() const noexcept { return fp_.get(); }

/**
 * Release ownership of the managed file handle.
 *
 * @return Owning FilePtr containing the FILE*
 *
 * After this call:
 * - FileHandler no longer owns the file
 * - The returned FilePtr becomes solely responsible for closing it
 *
 * This is useful when ownership must be passed to another component.
 */
FilePtr FileHandler::release() noexcept { return std::move(fp_); }

} // namespace fio
