#Upcoming Tasks

## Upstreaming

### Seek feedback
* Comment in ticket about blog, and link to repo.
* Email select individuals for feedback.
* Draft formal intent-to-prototype.

### Anticipated work
* Allow feedback from Chromium authors to guide future action toward this goal.
  - Discussions may result in significant design changes.
* Stylistic changes
* Testing
  - Currently, only the most minimal of unit testing exists and only for `library/`
* Formal review
* Possibly reimplement large chunks of the current functionality in lower-level components of the Chromium codebase, e.g. //net
  - Only if it is determined by the Chromium community to be preferable
  - Will involve reworking significant parts of the code. For example, no more UrlLoader

## Extending / Expanding Devtools 
* Creating a new tab/panel for IPFS-specific tooling
* Possibly include a partial DAG visualization
* 

# Seek engagement through other channels
Upstreaming into Chromium remains the primary goal and will take priority over any of this other work. 

Whenever that process is stalled, however, we will look to gain visibility, demonstrate usage, and possibly even gain users of the project through other channels. 

### Integrated as a Demo Variant on another Browser
The code should be applicable in other contexts.
* Choose one that's also open source, for which there may be interest: perhaps Brave.
* Create a demo repository that largely mirrors what exists as component/ today
  - Look for opportunities for code reuse
* Implement basic functionality as a demonstration
  - May involve library changes to easy integration
* Perhaps open an issue/PR as a conversation venue
* Publish a blog with findings

### Integrate into Electron
This is a similar effort to above, but targeting a particular widely-used and important embedder.

The steps listed above are all relevant here, including hoping to open a PR to Electron. 

### As a library
Currently little more than a directory, `library/` is code that does not depend upon Chromium at build time.
The idea is to have it become a proper reusable library.

The primary use case is actually the previous two efforts: a secondary browser implementation and Electron integration. However, there has also been [outside interest](https://github.com/little-bear-labs/ipfs-chromium/issues/2)

* Possibly become a separate project & repo
* Usability improvements (class-level documentation, etc.)
* A README that explains which interfaces one needs to implement as a user, for what purposes.
* Package & publish the library
  - In Github releases, as a tar
  - In conan, including Conancenter
  - System package managers (apt,dnf,nix,brew)

### As an application
Currently `component/` is a directory that depends on various scripts in `cmake/` mostly not actually used by `library/`, as it involves managing a Chromium repository.
The idea is that it would become a standalone project that builds a Chromium variant.

The primary target here are users who explore more obscure/experimental browsers, either in their app repo or searching the web. They tend to be more technical and interested in web tech in particular, due to self-selection bias. This makes them a good audience to seek exposure with.

* Become a separate project & repo
* Gain a dependency on the library, ipfs_client
* Create (possibly branded) releases for Mac, Linux & Windows as usability improves
  - It should be abundantly clear from metadata such as app name that this is experimental 
* Distribute builds through
  - Github releases
  - Third-party one-off software repositories, such as COPR & PPA
  - Attempt to submit to various wider-audience community software repositories/"app stores" (e.g. Universe, Homebrew, etc.)
  - If those efforts go well, perhaps target the some more restrictive software repostiories like the Windows Store.
