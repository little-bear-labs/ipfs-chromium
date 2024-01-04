# As-Implemented Design Notes

## Caching

### Classes involved

The term can mean a few different things. There are essentially 3 layers of what could be called "caching" at play here.

1. Immediate object storage
   * These are in-memory collections of parsed, conveniently-laid-out objects written for-purpose
   * There are 2:
     1. `BlockStorage` contains the most recently-accessed blocks (as `ipfs::PbDag`), and a mapping of CID string representation[^1] to them.
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

## Scoring

This is meant to document how gateway scoring is done in ipfs-chromium _today_.

This is *not* meant to imply that it's the best approach, or that we will maintain it this way long-term.

Each gateway has an associated score, used to determine preference for sending requests there and also how many requests should be sent concurrently.

It's an unsigned integer in two forms:

* A canonical score.
    - It begins as a hand-written, hard-coded constant.
    - When a gateway successfully returns a useful result before cancellation, its score increases by 1.
    - If a gateway fails to return a useful response for any reason (timeout, doesn't support fetching IPNS records, etc.):
        - If the score is already zero, the gateway is removed from the list entirely
        - Otherwise the score is decreased by 1
    - Having a request cancelled because some other gateway successfully returned a result for an identical request first does _not_ alter the score.
    - These changes are intended to be persisted in the future
* A temporary score
    * When a top-level ipfs:// or ipns:// request begins, it fetches a (gateway request) scheduler
        * If there is already a busy scheduler, it is re-used
        * If ipfs traffic is starting up from nil, instead a new scheduler is instantiated
    * When a new scheduler is instantiate for this, it requests the list of gateways, with their scores.
        * The scores retrieved are the canonical score _plus_ a small non-negative random integer (geometric distribution tending toward zero)
        * This helps to ensure you won't always be sending requests to the same gateways in the same order, or in a deterministic way.

When issuing a request to another gateway, it checks the gateways in descending-score order:
* If the gateway already has a request for that data startup_pending_, or has already failed to return successfully, it's skipped.
* If the gateways score is less than the number of startup_pending_ requests currently sent to that gateway, it's skipped as it's considered already-too-busy
* The "need" is generally calculated as the target number of gateways desired to be involved (based on request parallel), subtracting the number of gateways currently processing a request for this data.
* If the "need" is less than half the count of startup_pending_ requests on this gateway, it's skipped as it's simply not urgent enough to justify further overloading this gateway.

On a side-note, the hard-coded starting points for the scoring effectively encodes known information about those gateways.
For example: http://localhost:8080/ is scored extremely highly. There's a good chance it has the resource you're looking for, and if it doesn't you may want to send a request that way anyhow so that it will in the future.
Conversely, https://ipfs.anonymize.com/ is rarely helpful and is barely hanging on at the bottom.
https://jcsl.hopto.org/ is scored higher than one might imagine, given that it's not even commercially-hosted. But ipfs-chromium today is disproportionately used on the same set of test links, and jcsl.hopto will generally have those because it's John's home and his node.

[^1]: At some point this should be fixed. A difference in choice of multibase encoding or even codec should not cause a new entry. Different hash algos, on the other hand, are unavoidably incomparable.
