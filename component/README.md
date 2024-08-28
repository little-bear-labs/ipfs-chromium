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

### User Preferences

For more info see [Preferences](../doc/preferences.md).
