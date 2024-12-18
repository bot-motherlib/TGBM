#pragma once

#include <tgbm/api/types/all_fwd.hpp>

namespace tgbm::api {

/*Describes a Telegram Star transaction.*/
struct StarTransaction {
  /* Unique identifier of the transaction. Coincides with the identifier of the original transaction for
   * refund transactions. Coincides with SuccessfulPayment.telegram_payment_charge_id for successful incoming
   * payments from users. */
  String id;
  /* Integer amount of Telegram Stars transferred by the transaction */
  Integer amount;
  /* Date the transaction was created in Unix time */
  Integer date;
  /* Optional. The number of 1/1000000000 shares of Telegram Stars transferred by the transaction; from 0 to
   * 999999999 */
  optional<Integer> nanostar_amount;
  /* Optional. Source of an incoming transaction (e.g., a user purchasing goods or services, Fragment
   * refunding a failed withdrawal). Only for incoming transactions */
  box<TransactionPartner> source;
  /* Optional. Receiver of an outgoing transaction (e.g., a user for a purchase refund, Fragment for a
   * withdrawal). Only for outgoing transactions */
  box<TransactionPartner> receiver;

  consteval static bool is_mandatory_field(std::string_view name) {
    return string_switch<bool>(name)
        .case_("id", true)
        .case_("amount", true)
        .case_("date", true)
        .or_default(false);
  }
};

}  // namespace tgbm::api
