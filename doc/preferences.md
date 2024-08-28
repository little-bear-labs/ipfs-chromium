# User Preferences 

## Library

Users of the library configure behavior directly, via `ipfs::Client`.

All of the overloads to `with(X)` are there for you to alter/extend behavior. 
The ones related to the user preferences in Chromium are `ipfs::ctx::GatewayConfig` and `DnslinkFallbackSwitch`.

## Chromium (components/ipfs)

As of today there is no UI for editing the user preferences that change the behavior. 
Some values alter themselves from feedback, e.g. if a gateway returns desired content successfully more often it will be used more frequently in the future.
Others are simply left to their default values, unless one manually intervenes by changing the `Preferences` file.

### Preferences file.

Find the file named Prefernces in your 
[profile directory](https://chromium.googlesource.com/chromium/src.git/+/HEAD/docs/user_data_dir.md#Current-Location).
For example, for me it would be `~/.config/chromium/Default/Preferences`

Don't write to it while that profile is active. Race conditions are a bad idea.

All IPFS-related are under the ipfs key. 
The file is not pretty-printed when the browser reads it, but if you reformat it the browser can still read it. 
So you may want to do that before hand-editing.
To view my current settings, for example, I'd use 

`jq .ipfs ~/.config/chromium/Default/Preferences`

The keys are as follows:


1. ipfs : All IPFS settings are under ipfs
    1. dnslink : settings related to resolving human-readable names to IPFS CIDs, including [DNSLink](https://dnslink.dev/)
        1. fallback_to_gateway : boolean (default true) - In cases where the in-browser DNSClient fails to resolve the `_dnslink` name via system DNS setup... is it OK to send a request to gateways to have them resolve the name for you
    2. discovery : settings related to discovering new gateways
        1. rate : integer (default 120) - the requests-per-minit to initialize new gateways with when they are discovered
        2. http : bool (default true) - is it OK to start using insecure (HTTP not HTTPS) gateways when discovered
    3. gateway : the list of gateways to use, and settings for them. This section is updated at runtime.
        1. "gateway URL prefix" : The keys in gateway can be pasted in front of requests to form a full http(s) URL, e.g. "https://ipfs.io/". Naturally it has to include the scheme and host, it may also contain the beginning of a path if there's strange routiung rules going on. User info is also acceptable. Should end in /
            1. max_requests_per_minute : integer - the number of requests per minute. Not a hard limit, but it will get de-prioritized if on average more than this is being sent.
            2. Block    : integer - how preferred this gateway should be for `format=raw`, single-block trustless block requests.
            3. Car      : integer - how preferred this gateway should be for `?dag-scope=entity` CAR requests
            4. DnsLink  : integer - how preferred this gateway should be for requests meant to resolve names. These are, in fact, trusted block requests. However different gateways have different resolvers enabled.
            5. Identity : integer - completely irrelevant. Inline CIDs contain the data for the response, so they should not be sent to a gateway.
            6. Ipns     : integer - how preferred this gateway should be for `format=raw`, trustless requests for an IPNS record.
            7. Providers: integer - how preferred this gateway should be for `/routing/v1` requests.
