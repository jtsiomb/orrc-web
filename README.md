Own-Renderer Render Competition website and tools
=================================================

The Own-Renderer Render Competition (ORRC) is a recurring on-line competition
for 3D rendered images produced by the pariticpants' own custom renderers.

This repository hosts the ORRC website, including static pages and the CGI
program responsible for voting, and generation of gallery and entry showcase
pages, as well as any other tools used to run the competition. No binary files
are included in the repo.

 - Competition website: https://orrc.mutantstargoat.com
 - Github repo: https://github.com/jtsiomb/orrc-web

License
-------
Copyright (C) 2026 John Tsiombikas <nuclear@mutantstargoat.com>

Any included programs are Free Software. Feel free to use, modify, and/or
distribute them under the terms of the GNU Affero General Public License v3,
or at your option any later version published by the Free Software Foundation.
See COPYING for details.

Notes
-----
Each round gets a `roundNN` directory. To start the voting, create the `entries`
file, and symlink the correct `roundNN` as `current`. See the example
`round00/entries` file. When voting ends, sum the scores in `current/score`,
which may include duplicates that need to be discarded first. 

TODO
----
 - Write the final score counting program, and make it update the entries file.
 - Update the gallery page to show the final scores if they exist, and sort
   entries accordingly.
