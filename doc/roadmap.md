#Upcoming Tasks

# Upstreaming

## Seek feedback
* Comment in ticket about blog, and link to repo
* Email select individuals for feedback
* Draft formal intent-to-prototype

## Anticipated work
* First and foremost, allow feedback from Chromium authors to guide future action toward this goal
* Stylistic changes
* Testing
* Formal review
* Possibly reimplement large chunks of the current functionality in lower-level components of the Chromium codebase, e.g. //net
  - Only if it is determined by the Chromium community to be preferable

# Seek engagement through other channels
Upstreaming into Chromium remains the primary goal and will take priority over any of this other work. 

Whenever that process is stalled, however, we will look to gain visibility, demonstrate usage, and possibly even gain users of the project through other channels. 

## As a library
Currently little more than a directory, `library/` is code that does not depend upon Chromium at build time.
The idea is to have it become a proper reusable library.
* Possibly become a separate project & repo
* Usability improvements (class-level documentation, etc.)
* A README that explains which interfaces one needs to implement as a user, for what purposes.
* Package & publish the library
  - In github releases, as a tar
  - In conan, possibly even conancenter
  - Possibly others

## As an application
Currently `component/` is a directory that depends on various scripts in `cmake/` mostly not actually used by `library/`, as it involves managing a Chromium repository.
The idea is that it would become a standalone project that builds a Chromium variant.
* Become a separate project & repo
* Gain a dependency on the library, ipfs_client
* Create (possibly branded) releases for Mac, Linux & Windows as usability improves
  - It should be abundantly clear from metadata such as app name that this is experimental 
* Distribute builds through
  - Github releases
  - Third-party one-off app repos, such as COPR & PPA
  - Attempt to submit to various wider-audience community repos (e.g. Universe, Homebrew, etc.)

## Integrated as a Demo Variant on another Browser
The code should be applicable in other contexts.
* Choose one that's somewhat diverged but opens ource: perhaps Brave, Falkon, or similar
* Create a demo repository that largely mirrors what exists as component/ today
  - Look for opportunities for code reuse
* Implement only basic functionality as a demonstration
  - May involve library changes to easy integration
* Publish a blog with findings
