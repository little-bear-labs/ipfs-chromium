# Planned Design (unimplemented)
    
1. Outside of components/ipfs
   1. chrome/browser gains a dependency on components/ipfs
      1. Other apps may opt in as well.
   2. in ChromeContentBrowserClient::WillCreateURLLoaderRequestInterceptors
      1. Instantiate an ipfs::Interceptor and add it to the list.
2. New component "ipfs" contains classes:
   1. Contains a URLLoaderRequestInterceptor
      1. This class simply checks the scheme against ipfs/ipns to decide whether to intercept
      2. Creates Loader for each IPFS request
   2. Gateways - maintains a list of known http gateways
      1. Some are hard-coded
      2. An embedder has the option to add more
         1. Possible future work - browser settings for users of upstream Chromium to manage these
      3. Maintains scoring of gateways based on performance
      4. get_list() generates an ordered (prioritized) list to be used for a given request
         1. The specific algo could be configured and/or extended
         2. For a reasonable first pass, I suggest a pseudorandom ordering with higher-scoring gateways having a higher probability of being earlier in the list
   3. A URLLoader for ipfs:// URLs
      1. Upon start gets a list of gateways as above.
      2. Delegates http requests to a URL loader
         1. Creates N (default=10 as per spec) simultaneous request squences to the first N gateways
         2. Upon error:
            1. indicate to Gateways (2.2) to decrease the score of that gateway
            2. start an http request for the next gateway in the list, if any remain
         3. Upon "success" (200)
            1. Attempt validation (2.4)
               1. If validation fails, treat similarly to an http error (2.3.2.2)
               2. If validation succeeds
                  1. Increase the score of the corresponding gateway
                  2. Cancel other pending http requests
                  3. Call back the response through the mojom interface
         4. If the list of gateways is exhausted and all http responses finished, report most recent error without body
   4. Data validation utilities (*)
      1. For URLs of the form ipfs://bafybeihn2f7lhumh4grizksi2fl233cyszqadkn424ptjajfenykpsaiw4/
         1. See [Trustless Gateway Spec](https://github.com/ipfs/specs/blob/main/http-gateways/TRUSTLESS_GATEWAY.md)
         2. Parse the CID to get hash & hash algo
         3. Apply that algo to the body of the response
         4. check that they match
      2. For URLs with UnixFS paths - ip_s://cid/a/b
         1. Fetch the root, verify it as above
         2. Examine the response body for the CID (x) for the first element (a) in the path from the response
         3. recurse with ipfs://x/b
      3. Investigate verification for [IPNS](https://github.com/ipfs/specs/issues/320) 
      4. DNSLink URLs
         1. Attempt to fetch the TXT record by standard Chromium means
         2. Do the substitution as described [here](http://chomp:8080/ipfs/bafybeidkrrsdauzr6lfkfr7so625myv7njbzvkptrrj2a4ir4ejlg3sieu/concepts/dnslink/#resolve-dnslink-name)
         3. Recurse (up a level, since it might now be ipfs/ipns with or without path)
3. Possible future(?) work
   1. If multiple blocks are known at once, request them from different gateways in parallel TODO: either substantiate that this can happen or remove this point
   2. If a download is underway and appears healthy (content being received with an x-ipfs-path header) and the expected length is large, pause concurrent downloads from other gateways.

## Tmptest

```mermaid
  graph TD;
      A-->B;
      A-->C;
      B-->D;
      C-->D;
```

