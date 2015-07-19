#include "crop_image.h"

#include <cstring>

#include <vector>

#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "phpcpp.h"

Php::Value CropImage(Php::Parameters& parameters) {
  const char* encoded_image_pointer = parameters[0];
  std::vector<char> encoded_image{encoded_image_pointer, encoded_image_pointer + parameters[0].size()};
  cv::Mat image = cv::imdecode(encoded_image, CV_LOAD_IMAGE_GRAYSCALE);
  cv::Rect_<int> rectangle{parameters[1]["x"], parameters[1]["y"], parameters[1]["width"], parameters[1]["height"]};
  cv::Mat cropped_image{image(rectangle)};
  std::vector<uchar> buffer{};
  cv::imencode(".png", cropped_image, buffer);
  Php::Value result{};
  std::memcpy(result.reserve(buffer.size()), buffer.data(), buffer.size());
  return result;
}
