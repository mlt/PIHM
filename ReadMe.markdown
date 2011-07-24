PGS -- PIHM plugin for QGis, modified
=====================================

This is a modified version of PIHM plugin for QGis.
It is a by-product of my unsuccessful attempt to run (and use with a custom data) PIHM plugin for QGis under MS Windows.

Major changes comparing to last public version available:

 * Can be built against recent version of QGis (1.7)
   + Project settings are stored in dot qgs project file (QGis 1.3+ required) instead of a standalone dot pihmgis
     - project.txt in home folder is not used anymore
 * Sundials 2.4.0 is used
 * triangle and pihm executables are called by plugin using QgsRunProcess and QProcess instead of being linked in
   + Increases overall software stability
     - triangle may crash when incapabale of satisfying constraints (way too bad angles)
     - pihm potentially may crash as well due to bad user input
 * Numerous memory leaks clean-up in both PIHM code and QGis plug-in (many to go, especially when dealing with QwtPlot)
 * Fixed a typo in file descriptor used to read dot geol file in PIHM
 * Got rid of some temporarily files used to parse data in favor of QRegExp
 * Tested on MS Windows with OSGeo4W distribution and on Ubuntu Linux 11.04 (qgis.org repository with 1.7 QGis)

Disclaimer:

 This is a modified version.
 While only clean up was the goal as original code failed to run properly for me on MS Windows,
 bugs may have been unintentionally introduced in this software.
 Neither I nor PIHM authors are responsible for wrong results and implications you may get when using this version.

Note:

While original PIHMgis on sourceforge is listed as a GPL software, it contains code for `triangle` software which is `not so free` and puts a limitation on the right to develop commercial free software.
See the following:
http://www.gnu.org/philosophy/words-to-avoid.html#FreelyAvailable
http://www.gnu.org/philosophy/words-to-avoid.html#Commercial
http://www.gnu.org/philosophy/selling.html
http://www.gnu.org/licenses/gpl-faq.html#FSWithNFLibs

This derived work is free of that issue as it doesn't statically link with `triangle` code.
