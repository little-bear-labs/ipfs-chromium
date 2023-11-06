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
* If the gateway already has a request for that data pending, or has already failed to return successfully, it's skipped.
* If the gateways score is less than the number of pending requests currently sent to that gateway, it's skipped as it's considered already-too-busy
* The "need" is generally calculated as the target number of gateways desired to be involved (based on request parallel), subtracting the number of gateways currently processing a request for this data.
* If the "need" is less than half the count of pending requests on this gateway, it's skipped as it's simply not urgent enough to justify further overloading this gateway.


On a side-note, the hard-coded starting points for the scoring effectively encodes known information about those gateways.
For example: http://localhost:8080/ is scored extremely highly. There's a good chance it has the resource you're looking for, and if it doesn't you may want to send a request that way anyhow so that it will in the future.
Conversely, https://ipfs.anonymize.com/ is rarely helpful and is barely hanging on at the bottom.
https://jcsl.hopto.org/ is scored higher than one might imagine, given that it's not even commercially-hosted. But ipfs-chromium today is disproportionately used on the same set of test links, and jcsl.hopto will generally have those because it's John's home and his node.
