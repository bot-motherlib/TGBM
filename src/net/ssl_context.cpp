#include "tgbm/net/ssl_context.hpp"
#include "tgbm/logger.hpp"
#include "tgbm/net/errors.hpp"

#include <filesystem>

#ifdef TGBM_SSL_KEYS_FILE
  #define TGBM_ENABLE_WIRESHARK_SUPPORT
  #include <fstream>
#endif

namespace tgbm {

ssl_context::ssl_context(asio::ssl::context_base::method m) : ctx(m) {
}

#ifdef TGBM_ENABLE_WIRESHARK_SUPPORT

static void keylog_callback(const SSL*, const char* line) {
  std::filesystem::path keylog_file_path(TGBM_SSL_KEYS_FILE);
  std::ofstream keylog_file(keylog_file_path, std::ios_base::app | std::ios_base::out);
  if (keylog_file)
    keylog_file << std::string_view(line);
  else
    TGBM_LOG_ERROR("cannot open keylog file for wireshark support, path: {}", keylog_file_path.string());
}

#endif

ssl_context_ptr make_ssl_context_for_http11(std::span<const std::filesystem::path> additional_certs) {
  namespace ssl = asio::ssl;
  asio::ssl::context_base::method method =
#ifndef TGBM_ENABLE_WIRESHARK_SUPPORT
      ssl::context::tlsv13_client;
#else
      ssl::context::tlsv12_client;
#endif

  ssl_context_ptr sslctx = new ssl_context(method);
#ifdef TGBM_ENABLE_WIRESHARK_SUPPORT
  TGBM_LOG_INFO("SSL keys store enabled, file path: {}", TGBM_SSL_KEYS_FILE);
  SSL_CTX_set_keylog_callback(sslctx->ctx.native_handle(), &keylog_callback);
#endif
  sslctx->ctx.set_default_verify_paths();

  for (io_error_code ec; const auto& p : additional_certs) {
    std::filesystem::path ap = std::filesystem::absolute(p);
    ec = sslctx->ctx.load_verify_file(ap.string(), ec);
    if (ec)
      TGBM_LOG_ERROR("error while loading ssl verify file, err: {}, path: {}", ec.what(), p.string());
    else
      TGBM_LOG_INFO("additional SSL certificate loaded, path: {}", p.string());
  }

  sslctx->ctx.set_options(ssl::context::default_workarounds | ssl::context::no_sslv2 |
                          ssl::context::no_sslv3 | ssl::context::single_dh_use |
                          ssl::context::no_compression | ssl::context::no_tlsv1 | ssl::context::no_tlsv1_1);

  if (!SSL_CTX_set_cipher_list(sslctx->ctx.native_handle(),
                               "ECDHE+AESGCM:ECDHE+CHACHA20:!aNULL:!MD5:DEFAULT"))
    TGBM_LOG_WARN("ssl cipher cannot be selected");

  return sslctx;
}

ssl_context_ptr make_ssl_context_for_http2(std::span<const std::filesystem::path> additional_certs) {
  ssl_context_ptr sslctx = make_ssl_context_for_http11(additional_certs);
  const unsigned char alpn_protos[] = {0x02, 'h', '2'};  // HTTP/2
  if (0 != SSL_CTX_set_alpn_protos(sslctx->ctx.native_handle(), alpn_protos, sizeof(alpn_protos)))
    throw network_exception{"ALPN ctx broken {}", ERR_error_string(ERR_get_error(), nullptr)};
  return sslctx;
}

ssl_context_ptr make_ssl_context_for_server(std::filesystem::path certificate,
                                            std::filesystem::path server_private_key) {
  ssl_context_ptr ctx = new ssl_context(asio::ssl::context_base::tls_server);
  ctx->ctx.set_options(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::no_sslv2 |
                       boost::asio::ssl::context::single_dh_use);

  io_error_code ec;
  ec = ctx->ctx.use_certificate_chain_file(std::filesystem::absolute(certificate).string(), ec);
  if (ec) {
    TGBM_LOG_ERROR("cannot load certificate, path {}, err: {}", certificate.string(), ec.what());
    return nullptr;
  }
  ec = ctx->ctx.use_private_key_file(std::filesystem::absolute(server_private_key).string(),
                                     asio::ssl::context::pem, ec);
  if (ec) {
    TGBM_LOG_ERROR("cannot load private file, path {}, err: {}", server_private_key.string(), ec.what());
    return nullptr;
  }
  return ctx;
}

}  // namespace tgbm
