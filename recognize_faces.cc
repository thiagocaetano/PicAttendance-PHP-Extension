#include "recognize_faces.h"

#include <cstring>

#include <string>
#include <vector>

#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "phpcpp.h"

Php::Value RecognizeFaces(Php::Parameters& parameters) {
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
  
  cv::Ptr<cv::FaceRecognizer> model{cv::createLBPHFaceRecognizer()};
  std::vector<cv::Mat> images{};
  std::vector<int> labels{};
  for (const auto& labeled_image : parameters[2]) {
    const char* encoded_image_pointer = labeled_image.second["face"];
    std::vector<char> encoded_image{encoded_image_pointer, encoded_image_pointer + labeled_image.second["face"].size()};
    cv::Mat image = cv::imdecode(encoded_image, CV_LOAD_IMAGE_GRAYSCALE);
    images.push_back(image);
    labels.push_back(labeled_image.second["label"]);
  }
  model->train(images, labels);
  std::map<int, std::pair<std::size_t, double>> best_matches{};
  std::vector<int> predicted_labels(faces.size(), 0);
  for (std::size_t i{0}; i != faces.size(); ++i) {
    int predicted_label;
    double confidence;
    model->predict(faces[i], predicted_label, confidence);
    if (predicted_label == -1) {
      continue;
    }
    const auto current_best = best_matches.find(predicted_label);
    if (current_best == best_matches.end() || current_best->second.second > confidence) {
      best_matches[predicted_label].first = i;
      best_matches[predicted_label].second = confidence;
      predicted_labels[i] = predicted_label;
    }
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
    if (predicted_labels[i] != 0 && best_matches[predicted_labels[i]].first == i) {
      result[i]["label"] = predicted_labels[i];
    } else {
      result[i]["label"] = 0;
    }
  }
  return result;
}
