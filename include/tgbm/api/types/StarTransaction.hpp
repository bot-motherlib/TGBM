#pragma once

#include "all_fwd.hpp"

namespace tgbm::api {

/*Describes a Telegram Star transaction.*/
struct StarTransaction {
  /* Unique identifier of the transaction. Coincides with the identifier of the original transaction for
   * refund transactions. Coincides with SuccessfulPayment.telegram_payment_charge_id for successful incoming
   * payments from users. */
  String id;
  /* Number of Telegram Stars transferred by the transaction */
  Integer amount;
  /* Date the transaction was created in Unix time */
  Integer date;
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
