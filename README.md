PWB/UNIX `troff` source code (1977)
===================================

This is the source code for the original AT&T version of `troff`, dated
May 1977-1978. It was extracted from a tarball holding the original PWB
source [published][1] by The Unix Heritage Society:

> [http://www.tuhs.org/Archive/PDP-11/Distributions/usdl/bostic_pwb.tar.gz][1]

The files in this repository's root directory are garnish added for VCS
or GitHub-specific purposes. Everything in the immediate subdirectories
has been left untouched from their extraction. The file timestamps date
are back to October 25th, 1978.

Building
--------
Run `./setup.sh` while network access is available to install the  build cleanly on modern compilers. Afterwards the code can be built using `make`.
Object files are compiled with the `-std=c90` option so that the legacy
sources build cleanly on modern compilers.  The target CPU can be
specified via the `CPU` variable, for example:

```
make              # build using the host CPU
make CPU=x86-64   # build using generic x86‑64 settings
make CPU=haswell  # build for newer Intel chips
```

Building individual components
------------------------------
Subprojects can be compiled separately by invoking dedicated make
targets.  The `CROFF_TERMS` variable controls which device drivers are
built for `troff`:

```
make croff                  # build croff and all term drivers
make croff CROFF_TERMS=croff/term/tab37.c  # build for the TTY37 only
make tbl                    # build the tbl preprocessor
make neqn                   # build the neqn equation formatter
```


Setup environment
-----------------
Run `./setup.sh` while the environment still has network access to
install modern troff tools along with BCPL and ACK compilers and their
dependencies. The script also installs Python and Node utilities useful
for disassembling and fuzzing troff-based software.

Repository configuration
-----------------------
After cloning the repository you should run `./gitconfigure` once to
enable the `stripnul` filter defined in `.gitattributes`. This ensures
that zero bytes are stripped from source files when checking out and
committing, matching the behaviour used by the maintainer.

[1]: https://web.archive.org/web/20060823034049/http://www.tuhs.org/Archive/PDP-11/Distributions/usdl/bostic_pwb.tar.gz
[2]: https://github.com/Alhadis/otroff/pulls

License
-------
The contents of this repository are made available under the terms of the
Caldera "Ancient UNIX" license released in 2002.  This permissive
BSD-style license allows modification and redistribution so the sources
can be modernized and built on contemporary hardware.  See the LICENSE
file for full details.
