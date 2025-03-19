#ifndef IPFS_PREFERENCES_H_INCLUDED
#define IPFS_PREFERENCES_H_INCLUDED 1

#include "export.h"

#include <base/memory/raw_ptr.h>
#include <base/values.h>

#include <ipfs_client/ctx/gateway_config.h>

#include <ranges>

class PrefRegistrySimple;
class PrefService;

namespace ipfs {

/*!
 *  \brief Register IPFS-specific preferences
 *  \param registry Where to register them
 */
COMPONENT_EXPORT(IPFS) void RegisterPreferences(PrefRegistrySimple*);
bool DnsFallbackPref(PrefService const*);

/*! Configuration of gateways using Chromium preferences
 */
class ChromiumIpfsGatewayConfig final : public ipfs::ctx::GatewayConfig {
  raw_ptr<PrefService> prefs_;
  base::Value::Dict last_;
  base::Value::Dict curr_;
  std::size_t changes = 0;
  std::size_t update_thresh = 1;

  void save();
  std::size_t delta() const;

 public:

  /*! \brief construct
   *  \param prefs The underlying preference service for persisting configuration
   */
  ChromiumIpfsGatewayConfig(PrefService* prefs);

  unsigned GetGatewayRate(std::string_view) override;
  void SetGatewayRate(std::string_view, unsigned) override;
  std::optional<GatewaySpec> GetGateway(std::size_t index) const override;
  void AddGateway(std::string_view, unsigned) override;
  //  std::pair<std::string const*, unsigned> at(std::size_t index) const;
  unsigned RoutingApiDiscoveryDefaultRate() const override;
  bool RoutingApiDiscoveryOfUnencryptedGateways() const override;
  int GetTypeAffinity(std::string_view url_prefix,
                      gw::GatewayRequestType) const override;
  void SetTypeAffinity(std::string_view url_prefix,
                       gw::GatewayRequestType,
                       int) override;
};
}

#endif
