# Planned Design (unimplemented)

## Changes/Additions

1. Outside of components/ipfs
   1. chrome/browser gains a dependency on components/ipfs
      1. Other apps may opt in as well.
   2. in ChromeContentBrowserClient::WillCreateURLLoaderRequestInterceptors
      1. Instantiate an ipfs::Interceptor and add it to the list.
   3. ipfs:// and ipns:// URLs should be recongized in the omnibar
2. New component "ipfs" contains classes:
   1. URLLoaderRequestInterceptor
      1. checks the scheme against ipfs/ipns to decide whether to intercept
      2. Creates Loader for each IPFS/IPNS request
   2. Gateways - maintains a list of known http gateways
      1. Some are hard-coded
      2. An embedder has the option to add more
      3. Possible future work
         1. browser settings for end-users to manage gateway list
         2. Zeroconf
      4. Maintains dynamic scoring of gateways based on performance
      5. get_list() generates an ordered (prioritized) list to be used for a given request
         1. The specific algo could be configured and/or extended
   3. A URLLoader for ipfs:// and ipns:// URLs
      1. Currently most of the actual mechanism is planned to go here
      2. Could be further decomposed
   4. Data validation utilities (*)
      1. IPFS validation
         1. Parse the CID to get hash & hash algo
         2. Hash the block and check that they match
      2. IPNS validation
         1. Parse the name as CIDv1, if fails assume DNSLink
         2. Fetch the IPNS name record, see https://github.com/ipfs/specs/issues/320
         3. Validate signature
         4. Hash key & check it matches hash from the name CID
      4. DNSLink URLs
         1. Attempt to fetch the TXT record by standard Chromium means
         2. Do the substitution as described [here](http://chomp:8080/ipfs/bafybeidkrrsdauzr6lfkfr7so625myv7njbzvkptrrj2a4ir4ejlg3sieu/concepts/dnslink/#resolve-dnslink-name)

## Navigation Request Flow

### Top-Level
```mermaid
graph TD;
    start(["ChromeContentBrowserClient::WillCreateURLLoaderRequestInterceptors()"])
    style start fill:#9CF
    start --"(creates)"----> inter[[ipfs::Interceptor]]
    prev[[Previous Interceptor]] --> req0[/"URLRequest(1)"/]
    style req0 fill:#FFE
    req0 --> inter
    inter --> is_ipfs{"Is scheme ip_s?"}
    is_ipfs --NO--> req1[/"URLRequest(1)"/]
    style req1 fill:#FFE
    req1 --> next[["Next Interceptor(not handled)"]]
    is_ipfs --"YES (creates)"--> loader[["ipfs::Loader(URLLoader)"]]
    state[["ipfs::InterRequestState"]] --> gwlist[/"Prioritized list of gateways"/] --> loader
    style gwlist fill:#FFE
    req2[/"URLRequest(2)"/] --"Maybe modified, but not scheme"--> loader
    style req2 fill:#FFE
    errored(["Call back to client code with error"])
    style errored fill:#9CF
    loader --> is_ipns{"URL Scheme"} --IPNS--> parse_ipns[/"Attempt to parse name as CIDV1"/] --FAIL--> dns_txt[/"Attempt to fetch dnslink= from TXT"/] --FAIL--> errored
    dns_txt --PASS--> dns_sub["Replace URL prefix based on value"] --> is_ipns
    parse_ipns --PASS--> detnrurl["Determine URL to get name record"] --> gwreq>"Gateway Request (see next chart)"]
    style gwreq fill:#FDC
    gwreq --> validate --> sub["Substitute into URL"] --> is_ipns
    is_ipns --IPFS--> hdr["Set 'Accept: application/vnd.ipld.raw'"] --> gwreq0>"Gateway Request"]
    style gwreq0 fill:#FDC
    gwreq0 --> v["validate"] --> store["Store/append the block if we're at the end of the path"] --> links{"Are there links in the block?"} --YES--> filter["If URL contains path, only follow appropriate DAG links"] --"For Each"--> hdr
    gwreq --ALL FAIL--> errored
    gwreq0 --ALL FAIL--> errored
    links --NO--> reconstitute --> successed(["Call back to the client with 200 and body"])
    style successed fill:#9CF    
 ```

### Gateway Request


