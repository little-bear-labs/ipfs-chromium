## Caching

### Classes involved

The term can mean a few different things. There are essentially 3 layers of what could be called "caching" at play here.

1. Immediate object storage
   * These are in-memory collections of parsed, conveniently-laid-out objects written for-purpose
   * There are 2:
     1. `BlockStorage` contains the most recently-accessed blocks (as `ipfs::Block`), and a mapping of CID string representation[^1] to them.
       - When a new entry is being saved, it may overwrite an existing entry if that one hasn't been accessed for 5 minutes.
       - If there's no stale entry convenient, it may instead expand memory usage and create an entirely new entry.
     2. `IpnsNames`
       - Actually conflates 2 types of 'names' - DNSLink hostnames (without the _dnslink. prefix) and IPNS keys
       - Stores a mapping from name to a partial IPNS record, or in the case of DNSLink simply a CID
2. Serialized cache, via `CacheRequestor`
   * This is using Chromium's `disk_cache` mechanisms to store blocks of bytes.
   * Note: this is ITS OWN instance. This is not http cache, nor bytecode cache, nor font cache, etc.. 
   * Both IPFS blocks and IPNS names are stored in the same caches.
   * The keys are the same as for the immediate object storage, the values are serialized forms in one channel, HTTP headers in the other
   * There are 2 instantiations:
     1. A memory-only cache
     2. An on-disk cache (directory name `IpfsBlockCache`)

### Process 
The APIs are generally async, in a couple cases by necessity and in other cases to not overly complicate things.

To "get" a block or resolve an IPNS name:
1.  Check the appropriate immediate object storage
    * This part is actually synchronous, essentially just doing a hash map lookup
    * If it's there, great continue with logic. If not...
2.  "Request" it from ChainedRequestor
    * Think of this as short-circuit logic like logical-or operators
    * The first step is to check the in-memory logic. If it's there:
       - It get entered into immediate object storage
       - A callback of sorts happens to restart the process back at 1
    * Next check is on-disk cache
       - If it's there, it gets written into memory cache AND immediate storage
       - The same callback mechanism is used
    * Next after THAT is sending it over to the scheduler to issue network requests to gateways. If it gets found
       - It gets written into all 3 levels
       - Other requests get cancelled
       - The very same callback mechanism occurs

### Expiration

* IPFS blocks never expire, as they are immutable. They can be evicted for lack of use, though the rules for that differ by type of cache.
* DNSLink resolutions expire after 5 minutes.
* IPNS records expire at the time specified in the record, or time-received + TTL whichever comes later.
  - Curious side note, an entry is not removed from caches _when_ it expires. It's "doomed" when someone tries to access an already-expired entry.

[^1]: At some point this should be fixed. A difference in choice of multibase encoding or even codec should not cause a new entry. Different hash algos, on the other hand, are unavoidably incomparable.
