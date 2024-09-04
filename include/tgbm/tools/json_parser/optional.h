#pragma once

#if 0

#include <tgbm/tools/box.hpp>
#include <tgbm/tools/json_parser/basic.h>
#include <cassert>
#include <optional>

namespace tgbm::json {

template <typename T>
struct parser<box<T>> : parser<T> {
  box<T>* box_;
  bool start_parsing_{};

  static constexpr bool required = false;

  parser<box<T>>(box<T>& t_) : parser<T>(nullptr), box_(std::addressof(t_)) {
  }

  void rebind(box<T>* ptr) {
    start_parsing_ = {};
    box_ = ptr;
    parser<T>::rebind(box_->get());
  }

  void on_start_parsing() {
    assert(box_);
    start_parsing_ = true;
    box_->emplace();
    parser<T>::rebind(box_->get());
  }

  ResultParse null() {
    assert(box_);
    box_->reset();
    assert(start_parsing_);
    start_parsing_ = false;
    return ResultParse::kEnd;
  }
};

template <typename T>
struct parser<std::optional<T>> : parser<T> {
  std::optional<T>* opt_;
  bool start_parsing_;

  static constexpr bool required = false;

  parser<std::optional<T>>(std::optional<T>& t_) : opt_(&t_), parser<T>(t_ ? std::addressof(*t_) : nullptr) {
  }

  parser<std::optional<T>>(std::optional<T>* t_) : opt_(t_), parser<T>(t_ ? std::addressof(*t_) : nullptr) {
  }

  void rebind(std::optional<T>* ptr) {
    start_parsing_ = {};
    opt_ = ptr;
  }

  void on_start_parsing() {
    assert(opt_);
    start_parsing_ = true;
    opt_->emplace();
    parser<T>::rebind(std::addressof(opt_->value()));
  }

  ResultParse null() {
    assert(opt_);
    opt_->reset();
    assert(start_parsing_);
    start_parsing_ = false;
    return ResultParse::kEnd;
  }
};

}  // namespace tgbm::json

#endif
