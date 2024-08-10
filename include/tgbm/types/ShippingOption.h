#pragma once

#include "tgbm/types/LabeledPrice.h"

#include <string>
#include <memory>
#include <vector>

namespace tgbm {

/**
 * @brief This object represents one shipping option.
 *
 * https://core.telegram.org/bots/api#shippingoption
 *
 * @ingroup types
 */
class ShippingOption {
 public:
  typedef std::shared_ptr<ShippingOption> Ptr;

  /**
   * @brief Shipping option identifier.
   */
  std::string id;

  /**
   * @brief Option title.
   */
  std::string title;

  /**
   * @brief List of price options.
   */
  std::vector<LabeledPrice::Ptr> prices;
};
}  // namespace tgbm
