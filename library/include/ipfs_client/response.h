#ifndef IPFS_RESPONSE_H_
#define IPFS_RESPONSE_H_

#include "ipld/dag_headers.h"

#include <vocab/byte.h>

#include <cstdint>
#include <string>
#include <vector>

namespace ipfs {

/*! Response to an IpfsRequest
 *  @details Roughly mimics some aspects of an HTTP response, as some frameworks want to handle it as one.
 */
struct Response {
  std::string mime_;
  std::uint16_t status_;
  std::string body_;
  std::string location_;
  ipld::DagHeaders headers_;

  static Response PLAIN_NOT_FOUND;
  static Response IMMUTABLY_GONE;
  static Response HOST_NOT_FOUND_RESPONSE;
  /*! @param body HTML text (the response of the body, not the `<body>`
   *  @param location The Location header
   */
  static Response html(std::string_view body, std::string_view location = {});
};

}  // namespace ipfs

#endif  // IPFS_RESPONSE_H_
