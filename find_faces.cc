#include "find_faces.h"

#include <cstring>

#include <string>
#include <vector>

#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "phpcpp.h"

Php::Value FindFaces(Php::Parameters& parameters) {
  std::string cascade_filepath = parameters[0];
  const char* encoded_image_pointer = parameters[1];
  std::vector<char> encoded_image{encoded_image_pointer, encoded_image_pointer + parameters[1].size()};
  cv::Mat image = cv::imdecode(encoded_image, CV_LOAD_IMAGE_GRAYSCALE);
  cv::CascadeClassifier cascade{cascade_filepath};
  std::vector<cv::Rect_<int>> face_rectangles{};
  cascade.detectMultiScale(image, face_rectangles);
  std::vector<cv::Mat> faces{};
  for (const auto face_rectangle : face_rectangles) {
    cv::Mat resized_face{image(face_rectangle)};
    faces.push_back(resized_face);
  }
  Php::Value result{};
  for (std::size_t i{0}; i != faces.size(); ++i) {
    std::vector<uchar> buffer{};
    cv::imencode(".png", faces[i], buffer);
    Php::Value face_buffer{};
    std::memcpy(face_buffer.reserve(buffer.size()), buffer.data(), buffer.size());
    result[i]["face"] = face_buffer;
    result[i]["rectangle"]["x"] = face_rectangles[i].x;
    result[i]["rectangle"]["y"] = face_rectangles[i].y;
    result[i]["rectangle"]["width"] = face_rectangles[i].width;
    result[i]["rectangle"]["height"] = face_rectangles[i].height;
  }
  return result;
}
