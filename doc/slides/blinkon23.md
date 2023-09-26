<!--
theme: default
headingDivider: 3
style: |
  section {
    background-color: #CDEFFF;
    --color-canvas-subtle: #a9a9a9;
  }
  th {
    background-color: #CDEFFF;
  }
  th:empty {
    visibility: hidden;
  }
  img {
    display: block;
    margin-left: auto;
    margin-right: auto;
  }
  h1 {
    text-align: center;
  }
  h2 {
    text-align: center;
  }
  h3 {
    text-align: left;
  }
--> 

# Verifying IPFS Client in Chromium

<table style="margin-left:auto;margin-right:auto;">
<tr >
<td colspan="2">

![Chromium](chromium.png)
</td><td colspan="2">

![IPFS](ipfs.png) 
</td></tr>

<tr>
<td>

![LBL](lbl.png)
</td><td>

![arc](arc.png)
</td><td>

![FIL](fil.png)
</td><td>

![PL](pl.png)
</tr></table>

## What is IPFS?

Content-addressed distributed storage.

Relevant URL types:

* ipfs://bafybeidjtwun7gvpfjymhquloxzzwhpctgsw3vxampxr46du2we4yhiqje/reference/en/index.html
  - Refers to immutable content/site
  - Origin contains hash for root of Merkel tree
* ipns://k51qzi5uqu5dijv526o4z2z10ejylnel0bfvrtw53itcmsecffo8yf0zb4g9gi/links.html
  - Mutable pointer to immutable data
  - Origin contains public key to verify the 'pointer' record
* ipns://en.wikipedia-on-ipfs.org/wiki/Book
  - DNSLink uses DNS TXT records to redirect to /ipns/ or /ipfs/
  - Origin contains hostname

## Let's step through loading a page
<script type="module">
  import mermaid from 'https://cdn.jsdelivr.net/npm/mermaid@10/dist/mermaid.esm.min.mjs';
  mermaid.initialize({ startOnLoad: true });
</script>

### User enters URL
ipns://k51qzi5uqu5dku8zqce3b7kmpcw6uua9w00b5boyaevowmzls2rpie0itokett/
<div class="mermaid">
flowchart LR;
    ipns["/ipns/k51qzi5uqu5dku8zqce3b7kmpcw6uua9w00b5boyaevowmzls2rpie0itokett"]
    style ipns fill:#f9f
</div>

### Request IPNS Record
http://localhost:8080/ipns/k51qzi5uqu5dku8zqce3b7kmpcw6uua9w00b5boyaevowmzls2rpie0itokett?format=ipns-record
Record points at a DNSLink!
<div class="mermaid">
flowchart LR;
    ipns["/ipns/k51...ett"]-->dnsl["/ipns/ipfs.tech"]
    style ipns fill:#f9f
    style dnsl fill:#fa9
</div> 

### DNS TXT Record Lookup
_dnslink.ipfs.tech has TXT "dnslink=/ipfs/QmRE3dyFsbhC1dAthPBvgo4w15dGwppCAybgmJDB5m2SRy"
<div class="mermaid">
flowchart LR;
    ipns["/ipns/k51...ett"]-->dnsl["/ipns/ipfs.tech"]-->root["/ipfs/QmR...2SRy"]
    style ipns fill:#f9f
    style dnsl fill:#fa9
</div>

### We have our immutable content
Request root node.
https://ipfs.io/ipfs/QmRE3dyFsbhC1dAthPBvgo4w15dGwppCAybgmJDB5m2SRy?format=raw
<div class="mermaid">
flowchart LR;
    ipns["/ipns/k51...ett"]-->dnsl["/ipns/ipfs.tech"]-->root["/ipfs/QmR...2SRy"]
    style ipns fill:#f9f
    style dnsl fill:#fa9
    root-->e1["..."]
    root-->nuxt["_nuxt = QmYpxEzdbNbohung4gQoZFoQEzLNeBT8oGCQGPtKgjDbnT"]
    root-->e3["..."]
    root-->index["index.html = QmTzVjzGMG4LwBY9ArxcNcokPfbpG2biT8Gy9AiecxKtju"]
    root-->e5["..."]
    style root fill:#9fb
</div>

### It's a directory that contains index.html, so request that
https://gateway.pinata.cloud/ipfs/QmTzVjzGMG4LwBY9ArxcNcokPfbpG2biT8Gy9AiecxKtju?format=raw
Note the recursive verification.
<div class="mermaid">
flowchart LR;
    ipns["/ipns/k51...ett"]-->dnsl["/ipns/ipfs.tech"]-->root["/ipfs/QmR...2SRy"]
    style ipns fill:#f9f
    style dnsl fill:#fa9
    root-->e1["..."]
    root-->nuxt["_nuxt = QmYpxEzdbNbohung4gQoZFoQEzLNeBT8oGCQGPtKgjDbnT"]
    root-->e3["..."]
    root-->index["index.html = QmTzVjzGMG4LwBY9ArxcNcokPfbpG2biT8Gy9AiecxKtju"]
    root-->e5["..."]
    style root fill:#9fb
    style index fill:#9fb
</div>

### index.html fits into a single node
So just deserialize and return its content! 
The HTML starts rendering, and requests a resource.
`<img style="" class="max-w-40 mx-auto" src="/_nuxt/ipfs-logo.a313bcee.svg" data-v-6d12c5c1>`
This is relative to the ipns root, so start over with a request for:
`ipns://k51qzi5uqu5dku8zqce3b7kmpcw6uua9w00b5boyaevowmzls2rpie0itokett/_nuxt/ipfs-logo.a313bcee.svg`

### Lots of stuff is in hot in cache. Requesting the entry _nuxt:
https://ipfs.joaoleitao.org/QmYpxEzdbNbohung4gQoZFoQEzLNeBT8oGCQGPtKgjDbnT?format=raw
<div class="mermaid">
flowchart LR;
    ipns["/ipns/k51...ett"]-->dnsl["/ipns/ipfs.tech"]-->root["/ipfs/QmR...2SRy"]
    style ipns fill:#f9f
    style dnsl fill:#fa9
    root["/ipfs/QmR...SRy"]
    root-->e1["..."]
    root-->nuxt["_nuxt = QmY...bnT"]
    root-->e3["..."]
    root-->index["index.html = QmT...tju"]
    root-->e5["..."]
    style root fill:#9fb
    style index fill:#9fb
</div>

### _nuxt is also a directory, with 164 entries
Request the file we're after.
https://ipfs.fleek.co/ipfs/QmWTW21hadJGoAcmGZC18mCFq71UUVGPtJSX5PeBubnQCQ?format=raw
<div class="mermaid">
flowchart LR;
    ipns["/ipns/k51...ett"]-->dnsl["/ipns/ipfs.tech"]-->root["/ipfs/QmR...2SRy"]
    style ipns fill:#f9f
    style dnsl fill:#fa9
    root["/ipfs/QmR...SRy"]
    root-->e1["..."]
    root-->nuxt["_nuxt = QmY...bnT"]
    root-->e3["..."]
    root-->index["index.html = QmT...tju"]
    root-->e5["..."]
    nuxt-->f0["..."]
    nuxt-->img["ipfs-logo.a313bcee.svg = QmWTW21hadJGoAcmGZC18mCFq71UUVGPtJSX5PeBubnQCQ"]
    nuxt-->f1["..."]
    style root fill:#9fb
    style index fill:#9fb
    style nuxt fill:#9fb
</div>

### If this file were big
Request the file parts in parallel
https://gateway.ipfs.io/ipfs/Qm...1
https://jcsl.hopto.org/ipfs/Qm...2
<div class="mermaid">
flowchart LR;
    ipns["/ipns/k51...ett"]-->dnsl["/ipns/ipfs.tech"]-->root["/ipfs/QmR...2SRy"]
    style ipns fill:#f9f
    style dnsl fill:#fa9
    root["/ipfs/QmR...SRy"]
    root-->e1["..."]
    root-->nuxt["_nuxt = QmY...bnT"]
    root-->e3["..."]
    root-->index["index.html = QmT...tju"]
    root-->e5["..."]
    nuxt-->f1["..."]
    nuxt-->img["ipfs-logo.a313bcee.svg = QmWTW21hadJGoAcmGZC18mCFq71UUVGPtJSX5PeBubnQCQ"]
    nuxt-->f2["..."]
    style root fill:#9fb
    style index fill:#9fb
    style nuxt fill:#9fb
    style img fill:#9fb
    img-->c0["Qm...1"]
    img-->c1["Qm...2"]
</div>

## Since the HTTP response can come from any source...

Make the same request to multiple gateways in parallel. 
One will return first, others are cancelled. 
Reduces worst-case times. 

e.g.:

- https://ipfs.io/ipfs/QmRE3dyFsbhC1dAthPBvgo4w15dGwppCAybgmJDB5m2SRy?format=raw
- https://dweb.link/ipfs/QmRE3dyFsbhC1dAthPBvgo4w15dGwppCAybgmJDB5m2SRy?format=raw
- https://jorropo.net/ipfs/QmRE3dyFsbhC1dAthPBvgo4w15dGwppCAybgmJDB5m2SRy?format=raw

## Possible future considerations

* Design & implementation changes necessary for upstreaming
* User configuration (e.g. gateways to use & discovery enabled)
* IPFS V1 HTTP Routing API - Helping with missing data and exhausted known gateways
* Partial CAR requests - fewer network round-trips
* IPFS-specific DevTools (e.g.: DAG explorer)
* Possibly connecting to webtransport IPFS peers

## POC Screenshot

![ScreenShot](icss.png)

## Questions & Contact/Further reading

| what                 | where                                                         |
|----------------------|---------------------------------------------------------------|
| John's Email         | john@littlebearlabs.io                                        |
| ChromeStatus Feature | https://chromestatus.com/feature/5105580464668672             |
| Tracking Issue       | https://bugs.chromium.org/p/chromium/issues/detail?id=1440503 |
| POC Repo             | https://github.com/little-bear-labs/ipfs-chromium             |
| IPFS Specs           | https://specs.ipfs.tech                                       |
