### Changes/Additions

1. Outside of components/ipfs
    1. chrome/browser gains a dependency on components/ipfs
        1. Other apps may opt in as well.
    2. In ChromeContentBrowserClient::WillCreateURLLoaderRequestInterceptors
        1. Instantiate an ipfs::Interceptor and add it to the list.
    3. ipfs:// and ipns:// URLs should be recongized in the omnibar
2. New component "ipfs" contains classes:
    1. URLLoaderRequestInterceptor
        1. Checks the scheme against ipfs/ipns to decide whether to intercept
        2. Creates Loader for each IPFS/IPNS request
    2. Gateways - maintains a list of known http gateways
        1. Some are hard-coded
        2. An embedder has the option to add more
        3. Possible future work
            1. Browser settings for end-users to manage gateway list
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
            2. Fetch the IPNS name record, see https://gist.github.com/hacdias/f8935decee0c6a57513f2221c5a2fd7c for examples.
        4. DNSLink URLs
            1. Attempt to fetch the TXT record by standard Chromium means
            2. Do the substitution as described [here](http://chomp:8080/ipfs/bafybeidkrrsdauzr6lfkfr7so625myv7njbzvkptrrj2a4ir4ejlg3sieu/concepts/dnslink/#resolve-dnslink-name)

### Navigation Request Flow

#### Top-Level
```mermaid
graph TD;
    start(["ChromeContentBrowserClient::WillCreateURLLoaderRequestInterceptors()"])
    style start fill:#9CF,color:black
    start --"(creates)"----> inter[[ipfs::Interceptor]]
    prev[[Previous Interceptor]] --> req0[/"URLRequest(1)"/]
    style req0 fill:#FFE,color:black
    req0 --> inter
    inter --> is_ipfs{"Is scheme ip_s?"}
    is_ipfs --NO--> req1[/"URLRequest(1)"/]
    style req1 fill:#FFE,color:black
    req1 --> next[["Next Interceptor(not handled)"]]
    is_ipfs --"YES (creates)"--> loader[["ipfs::Loader(URLLoader)"]]
    state[["Gateways"]] --> gwlist[/"Prioritized list of gateways"/] --> loader
    style gwlist fill:#FFE,color:black
    req2[/"URLRequest(2)"/] --"Maybe modified, but not scheme"--> loader
    style req2 fill:#FFE,color:black
    errored(["Call back to client code with error"])
    style errored fill:#9CF,color:black
    loader --> is_ipns{"URL Scheme"} --IPNS--> parse_ipns[/"Attempt to parse name as CIDV1"/] --FAIL--> dns_txt[/"Attempt to fetch dnslink= from TXT"/] --FAIL--> errored
    dns_txt --PASS--> dns_sub["Replace URL prefix based on value"] --> is_ipns
    parse_ipns --PASS--> detnrurl["Set 'Accept: application/vnd.ipfs.ipns-record'"] --> gwreq>"Gateway Request (see next chart)"]
    style gwreq fill:#FDC,color:black
    gwreq --> validate --> sub["Substitute into URL"] --> is_ipns
    is_ipns --IPFS--> hdr["Set 'Accept: application/vnd.ipld.raw'"] --> gwreq0>"Gateway Request"]
    style gwreq0 fill:#FDC,color:black
    gwreq0 --> v["validate"] --> store["Store/append the block if we're at the end of the path"] --> links{"Are there links in the block?"} --YES--> filtr["If URL contains path, only follow appropriate DAG links"] --"For Each"--> hdr
    gwreq --ALL FAIL--> errored
    gwreq0 --ALL FAIL--> errored
    links --NO--> reconstitute --> successed(["Call back with status 200 and response body"])
    style successed fill:#9CF,color:black
 ```

#### Gateway Request

The class maintains:
* Pending requests: A list where each member has:
    * URL suffix (everything after the gateway)
    * enum/callback for what to do with the response
    * duplicate count: how many gateways it's been requested from already
* A list of gateways marked or segregated based on:
    * GOOD/BAD - whether the gateway has returned a successful http response during _this_ class's lifetime yet.
    * BUSY/FREE - whether an HTTP request to that gateway is outstanding
* For each gateway, a set of TaskFailed requests (URL suffixes)

```mermaid
graph TD;
    start(["URL suffix from above flow"]) --> add[("Add to startup_pending_ requests")]
    style start fill:#9CF,color:black
    add --> selreq["Select the startup_pending_ request with the lowest dup count"] --> incdup["Increase its dup count"] 
    incdup --> goodfree{"Is there a gateway that is both GOOD and FREE"} 
    goodfree --NO--> badfree{"Are there any FREE that have not already TaskFailed this request?"} 
    badfree --NO--> anybusy{"Are there any BUSY gateways?"} 
    anybusy --YES--> wait(("Wait for startup_pending_ requests to finish (return flow control)"))
    badfree --YES--> selbf["Select the one with the fewest TaskFailed requests, initial parallel tiebreaks"] 
    selbf --> mark_busy["Mark the gateway as BUSY"]
    goodfree --YES--> selbf
    mark_busy --> send["Create and send an HTTP URLRequest"]
    send -.callback.-> resp
    send --> conc{"Do we have the max concurrent requests already?"} --NO--> selreq
    conc --YES--> wait
    resp[/"URL response"/] --> markfree["Mark the gateway as FREE"] --> success{"status=200 + body?"} 
    style resp fill:#FFE,color:black
    success --YES--> cancel["Cancel identical requests"] --> markgood["Mark the gateway as GOOD"] 
    markgood --> incprio["Indicate to Gateways that this gateway's score/parallel should be a bit higher"] 
    incprio ----> successed(["Return response (see previous diagram)"])
    style successed fill:#9CF,color:black
    success --NO--> addbad["Add the URL suffix to this gateway's set of failures."] 
    addbad --> decprio[Indicate to Gateways that this gateway's score/parallel should be a bit lower] --> selreq
    anybusy --NO---> all_failed(["As all gateways have TaskFailed on a given request, report that failure"])
    style all_failed fill:#9CF,color:black
 ```

### Class Diagram

```mermaid
classDiagram
   class ChromeContentBrowserClient {
      WillCreateURLLoaderRequestInterceptors()
      ...
   }
   class ipfs_Loader {
      pending_requests
      gateway_list
      ...
   }
   class Gateways {
      get_list()
      increase_priority()
      decrease_priority()
   }
   ChromeContentBrowserClient ..> ipfs_Interceptor
   ipfs_Interceptor ..> Gateways
   ipfs_Interceptor ..> ipfs_Loader
   ipfs_Loader ..> Gateways   
```