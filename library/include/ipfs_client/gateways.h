#ifndef CHROMIUM_IPFS_GATEWAYS_H_
#define CHROMIUM_IPFS_GATEWAYS_H_

#include "export.h"
#include "gateway.h"
#include "vocab/flat_mapset.h"

#include <ctime>
#include <memory>
#include <random>
#include <string>
#include <string_view>
#include <vector>

namespace ipfs {
struct GatewaySpec {
  std::string prefix;
  unsigned strength;
  bool operator<(GatewaySpec const& r) const {
    if (strength == r.strength) {
      return prefix < r.prefix;
    }
    return strength > r.strength;
  }
};
using GatewayList = std::vector<GatewaySpec>;
class ContextApi;

/*!
 * \brief All known IPFS gateways
 */
class Gateways {
  flat_map<std::string, int> known_gateways_;
  std::default_random_engine random_engine_;
  std::geometric_distribution<unsigned> dist_;
  int up_log_ = 1;

 public:
  /*!
   * \brief The hard-coded list of gateways at startup
   */
  static GatewayList DefaultGateways();

  Gateways();
  ~Gateways();
  GatewayList GenerateList();  ///< Get a sorted list of gateways for requesting

  /*!
   * \brief Good gateway, handle more!
   * \param prefix - identify the gateway by its URL prefix
   */
  void promote(std::string const& prefix);

  /*!
   * \brief Bad gateway, move toward the back of the line.
   * \param prefix - identify the gateway by its URL prefix
   */
  void demote(std::string const& prefix);

  /*!
   * \brief Bulk load a bunch of new gateways
   * \param prefices - list of URL gateways by prefix
   */
  void AddGateways(std::vector<std::string> prefices);
};
}  // namespace ipfs

#endif  // CHROMIUM_IPFS_GATEWAYS_H_
