[ ]  improve performance on large multi-line graphs: smooth out the rendering, eliminate extra flashes of grey or white, etc.

This presumably means getting rid of at least the Draw calls that seem to do not much 
(since all commands end with a Refresh), and/or eliminating clientDC calls altogether.

[ ] Fix font handling, part A. Using the old MS .ttf font files, fix the four special
characters in screen/png/eps. The fix that worked for wx v.2.8.12 is just a bit off,
but the font name are no longer recognized even though fc-cache knows them.

[ ] Fix fonts, part B. re-do font handling.  Start by finding the PostScript fonts 
available on the system, and dynamically populating the font list.  To maintain 
compatibility, recognize all the existing special symbol names, ideally in every font.
Perhaps the way forward is to allow html-style names in strings? or have name/hexcode 
assignments be dynamically created/saved/imported back from persistent in ~/.extrema? 
Not sure what is the best moving forward at this point, because PostScript does not 
allow UTF16 characters, so cannot just use full UTF16 string entry and expect it to work. 
Perhaps abandon EPS altogether, replacing it with .pdf generation of vector graphics, 
I do believe PDF allows full UTF16 dictionaries? Or SVG? 
 
The font-handling scheme should also take into account what will be necessary in Windows, 
is their font handling different? 

[ ] Compile for Windows.

[ ] Compile for Mac OS.
