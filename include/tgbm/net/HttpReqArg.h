#pragma once

#include "tgbm/export.h"

#include <boost/lexical_cast.hpp>

#include <string>

namespace tgbm {
// TODO rm this file etc
/**
 * @brief This class represents argument in POST http requests.
 *
 * @ingroup net
 */
class TGBM_API HttpReqArg {
 public:
  template <typename T>
  HttpReqArg(std::string name, const T& value, bool isFile = false, std::string mimeType = "text/plain",
             std::string fileName = "")
      : name(std::move(name)),
        value(boost::lexical_cast<std::string>(value)),
        isFile(isFile),
        mimeType(std::move(mimeType)),
        fileName(std::move(fileName)) {
  }

  /**
   * @brief Name of an argument.
   */
  std::string name;

  /**
   * @brief Value of an argument.
   */
  std::string value;

  /**
   * @brief Should be true if an argument value hold some file contents
   */
  bool isFile = false;

  /**
   * @brief Mime type of an argument value. This field makes sense only if isFile is true.
   */
  std::string mimeType = "text/plain";

  /**
   * @brief Should be set if an argument value hold some file contents
   */
  std::string fileName;
};

}  // namespace tgbm
