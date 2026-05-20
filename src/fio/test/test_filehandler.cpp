#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>

#include <filehandler.h>

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

namespace {

/// RAII helper: creates a real, named temp file on construction and removes it
/// on destruction.
struct TempFile {
  std::string path;

  explicit TempFile(const std::string &name = "test_fh_tmp.txt")
      : path((std::filesystem::temp_directory_path() / name).string()) {
    // Create the file so it exists and is readable/writable
    FILE *f = std::fopen(path.c_str(), "w");
    if (f) {
      std::fputs("hello", f);
      std::fclose(f);
    }
  }

  ~TempFile() { std::filesystem::remove(path); }

  // Non-copyable
  TempFile(const TempFile &) = delete;
  TempFile &operator=(const TempFile &) = delete;
};

} // namespace

TEST_SUITE("FileCloser") {

  TEST_CASE("operator() closes a valid FILE*") {
    // Open a real file so fclose does not crash
    TempFile tmp;
    FILE *f = std::fopen(tmp.path.c_str(), "r");
    REQUIRE(f != nullptr);

    fio::FileCloser closer;
    // Must not throw / crash
    CHECK_NOTHROW(closer(f));
  }

  TEST_CASE("operator() is a no-op for nullptr") {
    fio::FileCloser closer;
    CHECK_NOTHROW(closer(nullptr));
  }
}

TEST_SUITE("FileHandler open — success") {

  TEST_CASE("opens an existing file for reading") {
    TempFile tmp;
    auto result = fio::FileHandler::open(tmp.path, "r");

    REQUIRE(result.has_value());
    CHECK(result->get() != nullptr);
  }

  TEST_CASE("opens an existing file for writing") {
    TempFile tmp;
    auto result = fio::FileHandler::open(tmp.path, "w");

    REQUIRE(result.has_value());
    CHECK(result->get() != nullptr);
  }

  TEST_CASE("opens a new file for writing (file need not exist)") {
    namespace fs = std::filesystem;
    auto path = (fs::temp_directory_path() / "test_fh_new_file.txt").string();
    fs::remove(path); // ensure it does not exist

    auto result = fio::FileHandler::open(path, "w");
    REQUIRE(result.has_value());
    CHECK(result->get() != nullptr);

    fs::remove(path); // cleanup
  }

  TEST_CASE("get() return the same non-null pointer") {
    TempFile tmp;
    auto result = fio::FileHandler::open(tmp.path, "r");
    REQUIRE(result.has_value());

    FILE *via_get = result->get();

    CHECK(via_get != nullptr);
  }

  TEST_CASE("underlying FILE* is usable for reading") {
    TempFile tmp; // contains "hello"
    auto result = fio::FileHandler::open(tmp.path, "r");
    REQUIRE(result.has_value());

    char buf[16]{};
    std::size_t n = std::fread(buf, 1, sizeof(buf) - 1, result->get());
    CHECK(n > 0);
    CHECK(std::string(buf, n) == "hello");
  }

  TEST_CASE("underlying FILE* is usable for writing") {
    TempFile tmp;
    auto result = fio::FileHandler::open(tmp.path, "w");
    REQUIRE(result.has_value());

    const char *msg = "world";
    std::size_t written = std::fwrite(msg, 1, std::strlen(msg), result->get());
    CHECK(written == std::strlen(msg));
  }
}

TEST_SUITE("FileHandler open — failure") {

  TEST_CASE("returns error for non-existent file opened for reading") {
    auto result = fio::FileHandler::open("/no/such/path/file_xyz.txt", "r");

    REQUIRE_FALSE(result.has_value());
    CHECK(result.error().type == fio::FileHandlerError::FileOpenError);
  }

  TEST_CASE("returns error for invalid path") {
    // Empty path is always invalid on POSIX and Windows
    auto result = fio::FileHandler::open("", "r");

    REQUIRE_FALSE(result.has_value());
    CHECK(result.error().type == fio::FileHandlerError::FileOpenError);
  }

  TEST_CASE("returns error for unwritable directory path") {
    // /proc is readable-only on Linux; adjust if testing on other platforms
    auto result = fio::FileHandler::open("/proc/fh_test_write.txt", "w");

    REQUIRE_FALSE(result.has_value());
    CHECK(result.error().type == fio::FileHandlerError::FileOpenError);
  }
}
