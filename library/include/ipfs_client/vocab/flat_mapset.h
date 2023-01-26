#ifndef CHROMIUM_IPFS_VOCAB_MAP_SET_H_
#define CHROMIUM_IPFS_VOCAB_MAP_SET_H_

#if __has_include("base/containers/flat_map.h")
  #include "base/containers/flat_map.h"
  #include "base/containers/flat_set.h"
  namespace ipfs {
    using base::flat_map;
    using base::flat_set;
  }
#elif __has_include(<boost/container/flat_map.hpp>)
  #include <boost/container/flat_map.hpp>
  #include <boost/container/flat_set.hpp>
  namespace ipfs {
    using boost::container::flat_map;
    using boost::container::flat_set;
  }
#else
  #warning "TODO: Provide an implementation for flat_map and flat_set. Will be using very un-flat implementations."
  #include <map>
  #include <set>
  namespace ipfs {
    template<class K,class V> using flat_map = std::map<K,V>;
    template<class KV>        using flat_set = std::set<KV >;
  }
#endif

#endif  // CHROMIUM_IPFS_VOCAB_MAP_SET_H_
