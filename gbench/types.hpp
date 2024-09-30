#pragma once 
#include <string>
#include <tgbm/api/common.hpp>
#include "tgbm/tools/pfr_extension.hpp"

namespace types{
    using namespace tgbm::api;

    template <typename T>
    consteval bool check_is_optional_field(){
        bool ok = true;
        pfr_extension::visit_struct<T>([&ok]<typename Info, typename Field>{
            auto val = T::is_optional_field(Info::name.AsStringView());
        });
        return ok;
    }

    struct Address {
        std::string street;            // String
        Integer houseNumber;           // Integer
        optional<std::string> zipCode; // optional<String>

        consteval static bool is_optional_field(std::string_view name) {
            return tgbm::utils::string_switch<std::optional<bool>>(name)
                .case_("street", false)
                .case_("houseNumber", false)
                .case_("zipCode", true)
                .or_default(std::nullopt).value();
        }
    };

    static_assert(check_is_optional_field<Address>());

    struct Contact {
        std::string type;              // String (например, "email" или "phone")
        std::string value;             // String (например, "example@example.com")
        consteval static bool is_optional_field(std::string_view name) {
            return tgbm::utils::string_switch<std::optional<bool>>(name)
                .case_("type", false)
                .case_("value", false)
                .or_default(std::nullopt).value();
        }
    };

    static_assert(check_is_optional_field<Contact>());

    struct Company {
        std::string name;              // String
        arrayof<Address> locations;    // arrayof<Address>
        optional<Contact> primaryContact; // optional<Contact>
        consteval static bool is_optional_field(std::string_view name) {
            return tgbm::utils::string_switch<std::optional<bool>>(name)
                .case_("name", false)
                .case_("locations", false)
                .case_("primaryContact", false)
                .or_default(std::nullopt).value();
        }
    };

    static_assert(check_is_optional_field<Company>());

    struct Employee {
        std::string name;                    // String
        Integer age;                         // Integer
        optional<std::string> nickname;      // optional<String>
        arrayof<Contact> contacts;           // arrayof<Contact>
        Address homeAddress;                 // Address
        optional<Company> employer;          // optional<Company>

        consteval static bool is_optional_field(std::string_view name) {
            return tgbm::utils::string_switch<std::optional<bool>>(name)
                .case_("name", false)
                .case_("age", false)
                .case_("nickname", false)
                .case_("contacts", true)
                .case_("homeAddress", false)
                .case_("employer", false)
                .or_default(std::nullopt).value();
        }
    };

    static_assert(check_is_optional_field<Employee>());

    struct Department {
        std::string name;                    // String
        arrayof<Employee> staff;             // arrayof<Employee>
        Company affiliate; 

        consteval static bool is_optional_field(std::string_view name) {
             return tgbm::utils::string_switch<std::optional<bool>>(name)
                .case_("name", false)
                .case_("staff", false)
                .case_("affiliate", false)
                .or_default(std::nullopt).value();
        }
    };

    static_assert(check_is_optional_field<Department>());

    struct Organization {
        std::string name;                       // String
        arrayof<Department> departments;        // arrayof<Department>
        optional<arrayof<Company>> partners;    // optional<arrayof<Company>>
        Integer foundingYear; 
        std::string description;                // String
        tgbm::box<Organization> parent;

        consteval static bool is_optional_field(std::string_view name) {
             return tgbm::utils::string_switch<std::optional<bool>>(name)
                .case_("name", false)
                .case_("parent", true)
                .case_("departments", false)
                .case_("partners", true)
                .case_("foundingYear", false)
                .case_("description", false)
                .case_("parent", true)
                .or_default(std::nullopt).value();
        }
    };

    static_assert(check_is_optional_field<Organization>());


    struct TreeNode{
        std::string value;
        tgbm::box<TreeNode> left;
        tgbm::box<TreeNode> right;

        consteval static bool is_optional_field(std::string_view name) {
             return tgbm::utils::string_switch<std::optional<bool>>(name)
                .case_("value", false)
                .case_("left", true)
                .case_("right", true)
                .or_default(std::nullopt).value();
        }
    };

    static_assert(check_is_optional_field<TreeNode>());


}