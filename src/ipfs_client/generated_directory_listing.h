#ifndef IPFS_GENERATED_DIRECTORY_LISTING_H_
#define IPFS_GENERATED_DIRECTORY_LISTING_H_

#include <string>
#include <string_view>

namespace ipfs {

/*!
 * \brief An index.html listing out a directory node's content
 */
class GeneratedDirectoryListing {
 public:

  /*!
   * \brief Get the HTML preamble going
   * \param base_path - The path _to_ this directory
   */
  explicit GeneratedDirectoryListing(std::string_view base_path);

  /*!
   * \brief Add an entry to the list
   * \param name - The directory's way of referring to that CID
   */
  void AddEntry(std::string_view name);

  /*!
   * \brief Finish up all the HTML stuff at the end.
   * \return The generated HTML
   */
  std::string const& Finish();

 private:
  std::string html_;
  std::string base_path_;

  void AddLink(std::string_view name, std::string_view path);
};
}  // namespace ipfs

#endif  // IPFS_GENERATED_DIRECTORY_LISTING_H_
