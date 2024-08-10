#pragma once

#include <string>
#include <memory>

namespace tgbm {

/**
 * @brief This object represents a phone contact.
 *
 * @ingroup types
 */
class Contact {
 public:
  typedef std::shared_ptr<Contact> Ptr;

  /**
   * @brief Contact's phone number.
   */
  std::string phoneNumber;

  /**
   * @brief Contact's first name.
   */
  std::string firstName;

  /**
   * @brief Optional. Contact's last name.
   */
  std::string lastName;

  /**
   * @brief Optional. Contact's user identifier in Telegram.
   */
  std::int64_t userId;

  /**
   * @brief Optional. Additional data about the contact in the form of a vCard.
   */
  std::string vcard;
};

}  // namespace tgbm
