#ifndef IPFS_REQUESTOR_H_
#define IPFS_REQUESTOR_H_

#include <functional>
#include <memory>
#include <string_view>

namespace ipfs::ipld {
class DagNode;
}
namespace ipfs {
class Client;
struct Response;
}  // namespace ipfs

namespace ipfs::gw {
class GatewayRequest;
using RequestPtr = std::shared_ptr<GatewayRequest>;

/*! Interface for classes that respond to IPFSRequest
 */
class Requestor : public std::enable_shared_from_this<Requestor> {
 public:
  enum class HandleOutcome : char {
    NOT_HANDLED = 'N',
    PENDING = 'P',
    DONE = 'D',
    PARALLEL = 'L',
    MAYBE_LATER = 'M'
  };

 protected:
  Requestor() {}

  virtual HandleOutcome handle(RequestPtr) = 0;

  static void definitive_failure(RequestPtr) ;
  void forward(RequestPtr) const;

  std::shared_ptr<Client> api_;

 public:
  using RequestPtr = ::ipfs::gw::RequestPtr;
  /*! @return The requestor's name, for debugging.
   *    Typically this is 1:1 with the concrete type.
   */
  virtual std::string_view name() const = 0;
  
  virtual ~Requestor() noexcept {}
  void request(std::shared_ptr<GatewayRequest>);
  Requestor& or_else(std::shared_ptr<Requestor> p);
  Requestor& api(std::shared_ptr<Client>);

  void TestAccess(void*);

 private:
  std::shared_ptr<Requestor> next_;
};
}  // namespace ipfs::gw

#endif  // IPFS_REQUESTOR_H_
