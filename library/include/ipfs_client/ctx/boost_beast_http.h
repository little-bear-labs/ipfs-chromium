#ifndef BOOST_BEAST_HTTP_H_INCLUDED
#define BOOST_BEAST_HTTP_H_INCLUDED 1

#include "http_api.h"

#if __has_include(<boost/beast/core/tcp_stream.hpp>)
#define HAS_BOOST_BEAST 1

#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>

namespace ipfs::ctx {
class BoostBeastHttp : public HttpApi {
  boost::asio::io_context& io_;
  boost::asio::ssl::context mutable ssl_ctx_ =
      boost::asio::ssl::context{boost::asio::ssl::context::sslv23_client};

 public:
  BoostBeastHttp(boost::asio::io_context&);
  ~BoostBeastHttp() noexcept override {}
  void SendHttpRequest(HttpRequestDescription,
                       HttpCompleteCallback cb) const override;
};
}  // namespace ipfs::ctx

#endif  // tcp_stream.hpp
#endif  // BOOST_BEAST_HTTP_H_INCLUDED
