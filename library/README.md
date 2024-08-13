# ipfs-client

## Usage

For a rough example of how this is done, see [this command-line tester](https://github.com/John-LittleBearLabs/ipfs_client_clitester)

### Instantiate ipfs::Client

Using code should center on the `ipfs::Client` class found in `include/ipfs_client/client.h`
There are a number of customization points for that class, particularly those things which might vary depending on 
which dependencies you'd like to use and/or which behaviors you'd like. 

Some concrete classes are provided with the library, each turned off if the corresponding dependency does not appear to 
be available. If you're using the Conan package, and not on Windows, all those libraries should be already linked in for you. 
In that case, you may use `ipfs::start_default` found in `include/ipfs_client/opinionated_context.h` , which is suitable for some non-critical uses.

If any of these choices don't make sense for your project, you may create a concrete class for whichever interface you need to alter, and call `with()` on the Client object, passing in an instance of your class.
If you don't call `with()`, usually you get an implementation that simply doesn't do anything (null object), or does something very simplistic.

To see the sorts of extension points available, look at the various `with()` methods in `ipfs::Client`. For example:

| Interface                         | Function                                                             | Provided example implementation                         | Design choice in example                                                                                                |
|-----------------------------------|----------------------------------------------------------------------|---------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------------|
| ipfs::ctx::HttpApi                | Make HTTP(s) requests                                                | ipfs::ctx::BoostBeastHttp                               | Uses [Boost.Beast](https://www.boost.org/doc/libs/1_84_0/libs/beast/doc/html/index.html)                                |
| ipfs::ctx::DnsTxtLookup           | DNS TXT requests (not getting IPs for gateways)                      | ipfs::ctx::AresDnsTxtLookup                             | Uses [c-ares](https://c-ares.org/)                                                                                      |
| ipfs::ctx::GatewayConfig          | Configures which IPFS gateways you use and how                       | ipfs::ctx::TransitoryGatewayConfig                      | Loads IPFS_GATEWAY environment variable, or if not set a static list of gateways. Changes at runtime are not persisted. |
| ipfs::ctx::JsonParser             | Parses JSON (String -> ipfs::DagJsonValue)                           | ipfs::ctx::NlohmannJsonParser                           | Uses N. Lohmann's [JSON for Modern C++](https://github.com/nlohmann/json)                                               |
| ipfs::ctx::CborParser             | Parses CBOR (Bytes -> ipfs::DagCborValue)                            | ipfs::ctx::NlohmannCborParser                           | Uses N. Lohmann's [JSON for Modern C++](https://github.com/nlohmann/json), but for CBOR                                 |
| ipfs::gw::Requestor               | A chain-of-responsibility for processing IPFS requests               | ipfs::gw::default_requestor() returns common requestors | If you don't provide an argument, this chain does no caching.                                                           |  
| MimeTypeDeduction                 | A function that guesses the mime type of a doc given its head & path | ipfs::util::TrivialMimeGuess                            | Doesn't even look at file content, just uses extension. Fine for cases that don't really need the mime type.            |
| UrlUnescaping                     | Given a URL component (i.e. in-between `/`s), deal with % escaping   | ipfs::util::RoughlyUnescapeUrlComponent                 | Not a complete implementation, but deals with some common cases.                                                        |
| ipfs::crypto::SignatureVerifier   | Verifies a public key signature for a particular type of key         | ipfs::crypto::OpensslSignatureVerifier                  | Uses OpenSSL, but only for RSA & Ed25519 keys (others are dropped)                                                      |

### Get a partition

`ipfs::Client::partition()` takes an arbitrary string to use as a key for that partition. 
The scheme for the keys is whatever you'd like it to be, but if you pass in an identical key you'll get the same partition back each time.
If it's the first time for that key, a new object is created, including an isolated view of the DAGs.

If you are not in a security-critical case, it's entirely reasonable to pass in `""` every time.
You might also consider using URL origins to define partitioning.

### Make a request

`std::make_shared<IpfsRequest>(path, callback)`

* path - A string of the form /ipfs/<cid>/path/to/file or /ipns/<key or hostname>/path/to/file
* callback - What to call when a final response is formed.

Pass the request to `partition->build_response()`

### Drive your HttpApi and DnsTxtLookup implementations

Many implementations will need some sort of event loop. Do that.

### Handle response

The callback you passed to create the request. 
Be sure to check the second parameter's (the response's) status_ field, which contains HTTP statuses like:

* 200 - success
* 502 - unable to find it from known gateways
* 404 - That path does not exist in that DAG, provably. 
* 503 - Unable to resolve IPNS name (either DNSLink failed to resolve or gateways didn't have the IPNS record, as appropriate)

Depending on quirks of your various implementations you may need to do extra logic here.
