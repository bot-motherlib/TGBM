#pragma once

#include "tgbm/export.h"

#include <string>

namespace tgbm {

/**
 * @brief This class parses a string with the url
 *
 * @ingroup net
 */
class TGBOT_API Url {
 public:
  Url(const std::string& url);

  /**
   * @brief Protocol part of an url. Example: https://
   */
  std::string protocol;

  /**
   * @brief Host part of an url. Example: www.example.com
   */
  std::string host;

  /**
   * @brief Path part of an url including preceding '/' char. Example: /index.html
   */
  std::string path;

  /**
   * @brief Query part of an url without '?' char. Example: a=1&b=2&c=3
   */
  std::string query;

  /**
   * @brief Fragment part of an url without '#' char. Example: section1
   */
  std::string fragment;
};

}  // namespace tgbm
