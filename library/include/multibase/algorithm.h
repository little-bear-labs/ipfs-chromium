#pragma once

#include <string>

namespace multibase {

class algorithm {
 public:
  /** Tag identifying algorithms which operate on blocks */
  class block_tag {};

  /** Tag identifying algorithms which operate on continuous data */
  class stream_tag {};

  virtual ~algorithm() = default;

  /** Returns the input size required to decode a single block */
  virtual std::size_t block_size() { return 0; }

  /** Returns the size of a processed block */
  virtual std::size_t output_size() { return 0; }

  /** Processes an input block returning any intermediate result */
  virtual std::string process(std::string_view input) = 0;
};

}  // namespace multibase
