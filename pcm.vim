" this is syntax highlighting file for eXtrema command macro files *.pcm
" should reside in /usr/share/vim/vimXX/syntax/pcm.vim or in ~/.vim/syntax/pcm.vim

" Quit if a syntax file was already loaded.
if exists('b:current_syntax') | finish|  endif

syntax match  extremaOption /\\\S*\s*/
syntax match  extremaLabel /\s*[^,:]*:\s*$/
syntax match  extremaExecute /@\w*\W*\|execute\W*\w*\W*/
syntax region extremaString matchgroup=normal start=/`/ end=/'/
syntax region extremaComment matchgroup=normal start=/.*!/ end=/\n/
syntax match  extremaFunction /abs(\|sqrt(\|exp(\|log(\|log10(\|ln(\|sin(\|sind(\|cos(\|cosd(\|tan(\|tand(\|cot(\|sec(\|csc(\|asin(\|asind(\|acos(\|acosd(\|atan(\|atand(\|acot(\|asec(\|acsc(\|sinh(\|cosh(\|tanh(\|coth(\|sech(\|csch(\|asinh(\|acosh(\|atanh(\|acoth(\|asech(\|acsch(\|atan2(\|atan2d(\|eltime(\|factorial(\|int(\|mod(\|nint(\|pdiff(\|ran(\|sign(\|date(\|time(\|ucase(\|lcase(\|tcase(\|char(\|ichar(\|varname(\|string(\|rchar(\|eqs(\|nes(\|sub(\|sup(\|index(\|evaluate(\|expand(\|canopen(\|airy(\|biry(\|area(\|besj0(\|besj1(\|besy0(\|besy1(\|besi0(\|besi1(\|besk0(\|besk1(\|beta(\|lnbeta(\|betain(\|cheby(\|cosint(\|dawson(\|deconvolute(\|deriv(\|det(\|dilog(\|eigen(\|finellic(\|ellick(\|einellic(\|ellice(\|expint(\|ei(\|expn(\|ferdirac(\|fft(\|frec1(\|fres1(\|frec2(\|fres2(\|gamma(\|gammln(\|logam(\|lnagamma(\|gammacin(\|gammain(\|gammatin(\|gammq(\|gaussj(\|hermite(\|hypgeo(\|chlogu(\|identity(\|ifft(\|integral(\|inverse(\|jacobi(\|join(\|ber(\|bei(\|ker(\|kei(\|laguerre(\|legendre(\|plmu(\|plmn(\|pfactors(\|poica(\|psi(\|digamma(\|radmac(\|savgol(\|sinint(\|smooth(\|splsmooth(\|bivsmooth(\|struve0(\|struve1(\|tina(\|clesg(\|wign3j(\|wign6j(\|wign9j(\|racah(\|jahnuf(\|voigt(\|volume(\|walsh(\|adev(\|binom(\|bivarnor(\|chisq(\|chisqinv(\|erf(\|aerf(\|erfc(\|aerfc(\|fisher(\|gauss(\|gaussin(\|normal(\|gmean(\|imax(\|imin(\|kurt(\|max(\|mean(\|median(\|min(\|poisson(\|prod(\|rms(\|rprod(\|rsum(\|skew(\|stdev(\|student(\|studenti(\|sum(\|var(\|len(\|vlen(\|clen(\|vartype(\|exist(\|iclose(\|iequal(\|where(\|roll(\|step(\|wrap(\|fold(\|unfold/he=e-1
syntax match extremaCommand /^\s*alias\|^\s*bestfit\|^\s*bin\|^\s*bin2d\|^\s*clear\|^\s*contour\|^\s*dealias\|^\s*defaults\|^\s*destroy\|^\s*display\|^\s*do\|^\s*ellipse\|^\s*endif\|^\s*enddo\|^\s*figure\|^\s*filter\|^\s*fit\|^\s*generate\|^\s*get\|^\s*goto\|^\s*graph\|^\s*grid\|^\s*hardcopy\|^\s*if\|^\s*inquire\|^\s*list\|^\s*matrix\|^\s*minuit\|^\s*page\|^\s*pause\|^\s*polygon\|^\s*quit\|^\s*read\|^\s*rebin\|^\s*replot\|^\s*restore\|^\s*return\|^\s*save\|^\s*scalar\|^\s*scales\|^\s*set\|^\s*show\|^\s*sort\|^\s*stack\|^\s*statistics\|^\s*terminal\|then\|^\s*vector\|^\s*world\|^\s*write\|^\s*zerolines/

hi def link extremaCommand      Operator
hi def link extremaFunction     Function
hi def link extremaExecute	Macro
hi def link extremaOption       Type
hi def link extremaString	String
hi def link extremaConditional	Conditional
hi def link extremaLabel	Label
hi def link extremaComment	Comment

let b:current_syntax = 'pcm'
