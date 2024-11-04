#pragma once

#include <string>

#include "tgbm/api/common.hpp"
#include "tgbm/tools/pfr_extension.hpp"

namespace types {
using namespace tgbm::api;

struct Address {
  std::string street;             // String
  Integer houseNumber;            // Integer
  optional<std::string> zipCode;  // optional<String>

  consteval static bool is_mandatory_field(std::string_view name) {
    return tgbm::utils::string_switch<bool>(name)
        .case_("street", true)
        .case_("houseNumber", true)
        .or_default(false);
  }
};

struct Contact {
  std::string type;   // String (например, "email" или "phone")
  std::string value;  // String (например, "example@example.com")
  consteval static bool is_mandatory_field(std::string_view name) {
    return true;
  }
};

struct Company {
  std::string name;                  // String
  arrayof<Address> locations;        // arrayof<Address>
  optional<Contact> primaryContact;  // optional<Contact>
  consteval static bool is_mandatory_field(std::string_view name) {
    return true;
  }
};

struct Employee {
  std::string name;                // String
  Integer age;                     // Integer
  optional<std::string> nickname;  // optional<String>
  arrayof<Contact> contacts;       // arrayof<Contact>
  Address homeAddress;             // Address
  optional<Company> employer;      // optional<Company>

  consteval static bool is_mandatory_field(std::string_view name) {
    return name != "contacts";
  }
};

struct Department {
  std::string name;         // String
  arrayof<Employee> staff;  // arrayof<Employee>
  Company affiliate;

  consteval static bool is_mandatory_field(std::string_view name) {
    return true;
  }
};

struct Organization {
  std::string name;                     // String
  arrayof<Department> departments;      // arrayof<Department>
  optional<arrayof<Company>> partners;  // optional<arrayof<Company>>
  Integer foundingYear;
  std::string description;  // String
  tgbm::box<Organization> parent;

  consteval static bool is_mandatory_field(std::string_view name) {
    return name != "parent" && name != "partners";
  }
};

struct TreeNode {
  std::string value;
  tgbm::box<TreeNode> left;
  tgbm::box<TreeNode> right;

  consteval static bool is_mandatory_field(std::string_view name) {
    return name == "value";
  }
};

}  // namespace types
