#pragma once

#include <boost/asio/ssl/context.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/intrusive_ptr.hpp>

#include <filesystem>

namespace tgbm {

namespace asio = boost::asio;

struct ssl_context;
// must be used only in one thread, multithread using of ssl_context is not safe bcs of open ssl
using ssl_context_ptr = boost::intrusive_ptr<ssl_context>;

struct ssl_context {
 private:
  size_t refcount = 0;

  explicit ssl_context(asio::ssl::context_base::method);

 public:
  asio::ssl::context ctx;

  static ssl_context_ptr create(asio::ssl::context_base::method);

  ssl_context(ssl_context&&) = delete;
  void operator=(ssl_context&&) = delete;

  friend void intrusive_ptr_add_ref(ssl_context* p) noexcept {
    ++p->refcount;
  }
  friend void intrusive_ptr_release(ssl_context* p) noexcept {
    --p->refcount;
    if (p == 0)
      delete p;
  }
};

ssl_context_ptr make_ssl_context_for_http2();

ssl_context_ptr make_ssl_context_for_http11();

ssl_context_ptr make_ssl_context_for_server(std::filesystem::path certificate,
                                            std::filesystem::path server_private_key);

}  // namespace tgbm
