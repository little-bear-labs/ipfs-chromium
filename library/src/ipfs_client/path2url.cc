#include "path2url.h"

#include "log_macros.h"

std::string ipfs::path2url(std::string p) {
  while (!p.empty() && p[0] == '/') {
    p.erase(0UL, 1UL);
  }
  DCHECK_EQ(p.at(0), 'i');
  DCHECK_EQ(p.at(1), 'p');
  DCHECK(p.at(2) == 'f' || p.at(2) == 'n');
  DCHECK_EQ(p.at(3), 's');
  DCHECK_EQ(p.at(4), '/');
  p.insert(4, ":/");
  return p;
}
