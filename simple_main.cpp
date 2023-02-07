#include <ipfs_client/gateways.h>
#include <ipfs_client/gen/pb-dag.pb.h>

#include <libp2p/multi/multibase_codec/codecs/base58.hpp>

#include <filesystem>
#include <fstream>

void parse_block_file(char const* file_name);

int main(int argc, char const* const argv[] )
{
    ipfs::Gateways gs;
    std::for_each(std::next(argv),std::next(argv,argc), parse_block_file);
}

void parse_block_file(char const* file_name)
{
    std::ifstream file{file_name};
    PBNode node;
    if ( !node.ParseFromIstream(&file) ) {
        std::clog << "Failed to parse '" << file_name << "'\n";
        return;
    }
    std::clog
        << "Parsed out '" << file_name
        << "', data=";
    for ( char c : node.data() ) {
        if ( std::isgraph(c) || c == ' ' ) {
            std::clog.put(c);
        } else {
            std::clog << "\\x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned>(c);
        }
    }
    std::clog.put('\n');
    for ( auto& link : node.links() ) {
        std::clog << "  link: " << link.name()
                  << " has a hash with "
                  << std::dec << link.hash().size()
                  << " bytes, decodes to "
//                  << " 0x" << std::hex << static_cast<unsigned>(link.hash().at(0))
//                  << " 0x" << std::hex << static_cast<unsigned>(link.hash().at(1))
//                  << " \n";
                  << libp2p::multi::detail::encodeBase58(link.hash())
                  << '\n';
    }
}
