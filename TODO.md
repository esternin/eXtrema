- [x]  improve performance on large multi-line graphs: smooth out the rendering, eliminate 
extra flashes of grey or white, etc. This presumably means getting rid of at least the Draw 
calls that seem to do not much (since all commands end with a Refresh), and/or eliminating clientDC calls altogether.

- [x] Fix font handling, part A. Using the old MS .ttf font files, fix the special
characters in screen/png/eps. The fix that worked for wx v.2.8.12 is just a bit off,
but the font name are no longer recognized even though fc-cache knows them. The characters
are: sum,times,div,equiv,neg,cdot,copyright,registered

- [ ] Fix fonts, part B. re-do font handling, to finding the type 42 PostScript fonts 
available on the system, and dynamically populating the font list.

- [ ] Compile for Windows.

- [ ] Compile for Mac OS.

- [ ] Modify the toolchain to have options to create a static executable and/or an AppImage, for common architectures

- [ ] Add ourselves back into Ubuntu/Debian apt distribution. 
