This directory becomes //components/ipfs inside of a Chromium source tree.

It's the primary spot for Chromium-specific code involved in integrating trustless IPFS client functionality.

# Usage notes:

## IPFS disabled by default
Visit chrome://flags/ and flip "Enable IPFS" to on.

## Gateway choices

These are stored in your profile's preferences. 
Unfortunately, there is no UI for IPFS preferences yet.

If your profile doesn't have IPFS preferences set up yet, 
it will check the environment variable IFPS_GATEWAY expecting it to be a space-separated list of URL prefixes, 
e.g. `https://gateway.ipfs.io/ https://ipfs.joaoleitao.org/`

If that environment variable isn't set, it'll use a hard-coded list to initialize preferences.

If the preferences are already there, [you may edit the JSON file directly](https://superuser.com/questions/554233/how-to-change-preferences-in-chrome-by-modifying-files)

Personally, I would:
* close any version of Chromium that calls itself that so they won't overwrite the file while I edit
* jq . ~/.config/chromium/Default/Preferences > pretty_printed.json # Location changes with OS & profile name
* ${VISUAL} pretty_printed.json
* mv pretty_printed.json ~/.config/chromium/Default/Preferences

### The relevants settings are:

#### ipfs.discovery.rate
Unsigned integer.
The requests-per-minute to be used for gateways when they're first discovered through the routing V1 api.
A value of 0 will disable that discovery.
The default value is 120 (i.e. 2 requests per second on average)

#### ipfs.discovery.http
Boolean.
Whether gateways discovered with an http:// scheme (that is, not https i.e. not encrypted) will be added to the list.
Default true.

#### ipfs.gateway

A dictionary where the keys are URL prefixes for different gateways, 
and the values are dictionaries of settings for that gateay.

#### `ipfs.gateway["https://ipfs.io/"].max_requests_per_minute`

Unsigned integer.
Not actually a strict maximum. 
However, if there have been more requests sent there recently than the rate will suggest,
other gateways will be strongly preferred over this one.
Does help with rate limits set by gateway providers.

#### `ipfs.gateway["https://ipfs.io/"].Car`

Automatically set, not generally expected to be user-facing.
There are several of these, actually. 
They track the history of that gateway with requests of that type,
as a soft way of inferring the capabilities of that gateway.
