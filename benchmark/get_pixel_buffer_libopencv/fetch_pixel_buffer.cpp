
#include <vector>
#include <fmt/core.h>
#include <runtime.h>

#include <opencv2/opencv.hpp>

//quality check to gather picture metric
//argv[1] input path original
//argv[2] input path watermarked
MILLI_CLOCK_RUNTIME_MAIN {
  clock();
  
  cv::Mat image = cv::imread(argv[1], cv::IMREAD_COLOR);
  if (image.empty()) { // Handle the load failure before accessing pixels.
    return -1;
  }
  const unsigned char* pixels = image.data;
  // Read pixel (x, y) from this 8-bit, 3-channel image.
  // const unsigned char* pixel = pixels + y * stride + x * image.elemSize();
  // unsigned char blue  = pixel[0];
  // unsigned char green = pixel[1];
  // unsigned char red   = pixel[2];

  cv::Mat packed = image.isContinuous() ? image : image.clone();
  
  const std::size_t byteCount = packed.total() * packed.elemSize();

  std::vector<unsigned char> buffer(packed.data, packed.data + byteCount);

  clock();
  fmt::println("time elasped: {} ms", clock.elapsed());
  return 0;
}
