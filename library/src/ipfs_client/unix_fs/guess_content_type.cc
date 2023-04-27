#include "guess_content_type.h"

#include <ipfs_client/networking_api.h>

#include "log_macros.h"

std::string ipfs::unix_fs::GuessContentType(NetworkingApi& api,
                                            std::string_view path,
                                            std::string_view content) {
  auto dot = path.rfind('.');
  auto slash = path.rfind('/');
  std::string ext;
  if (dot < path.size() && (slash < dot || slash == std::string::npos)) {
    ext = path.substr(dot + 1);
  }
  std::string url{"ipfs://"};
  url.append(path);
  auto mime = api.MimeType(ext, content, url);
  if (mime.size()) {
    // TODO, store mime in block
    LOG(INFO) << "Detected mime " << mime << " for " << path

              << " from URL " << url;
    return mime;
  }
  // TODO fetch the mime from block if available
  LOG(ERROR) << "\n\t###\tTODO:\tCould not determine mime type for '" << path
             << "'.\t###\n\n";
  return "TODO";
}
