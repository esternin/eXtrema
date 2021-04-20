<h2>Contributors to eXtrema</h2>

eXtrema has evolved from software developed over the years at TRIUMF (Tri-University 
Meson Facility), Canada's national laboratory for particle and nuclear physics in 
Vancouver, BC.  The need for a general purpose graphics and analysis package was seen 
early and the first such program was OPDATA, created, using FORTRAN, by Corrie J. Kost, 
Philip Bennett, and Arthur Haynes.

The next program, PLOTDATA, also in FORTRAN, used many ideas from OPDATA, but had a
different, more user friendly, interface, and was created by Joseph Chuma in 1983.  At
the heart of OPDATA and PLOTDATA was the expression evaluator, but it was limited in
that only element-by-element operations were possible, and variable indices were not
allowed in expressions.  Philip Bennett and Joseph Chuma re-wrote the expression
evaluator, again in FORTRAN, allowing for array operations and indices on variables,
functions, and entire expressions.  This work was incorporated into a new program,
PHYSICA, which also introduced many other new graphics and analysis features. After
a few aborted attempts at creating a GUI for PHYSICA, eXtrema for Microsoft Windows
was born, followed by eXtrema for Linux.

eXtrema was written completely in C++ by Joseph L. Chuma (@jchuma) using Borland C++ 
Builder 5 and later ported to Linux using wxWidgets, an open source cross-platform
toolkit.

![Joe Chuma c.2010](/Images/JC.jpg)

Joe Chuma passed away in 2011. This is from https://www.triumf.ca/alumni/joe-chuma:

> It is with much sadness that TRIUMF announces the death of Joe Chuma on Friday 
> August 5/2011. He passed away unexpectedly, peacefully, and suddenly at his home 
> in North Vancouver at the age of 63. He left behind his wife Charlene and devoted 
> son Jonathan.
>
> Joe started his career at TRIUMF in October 1981 and retired in March 2008. He was 
> instrumental in the development of the data analysis and display program Physica and 
> its successor Extrema. The latter was released as an Open Source project in 2006 
> for both Windows and Linux. It was particularly popular at many international academic 
> institutions (including UBC). Joe also contributed significantly to the TRIUMF studies 
> of Ring Cyclotrons (as a potential Kaon Factory). He also contributed to the development 
> of Roody, a GUI written in C++ based on CERN-ROOT for on and off-line data display. 
> He devoted himself in assisting others at TRIUMF in performing numerical analysis of 
> their data. His dedication will be remembered by the many of those who benefited from his work.

Since 2012, Edward Sternin has maintained the code so that it could be compiled, 
but otherwise made no changes.  In 2021, eXtrema was moved to GitHub and Vadim Zeitlin ([@vadz](https://github.com/vadz/)) 
helped to update it for GTK3/wxWidgets 3.x. See the ChangeLog for details.
