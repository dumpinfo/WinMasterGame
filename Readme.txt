Main README.TXT file for "Tricks of Windows Game Programming Gurus - Vol. I, Revision I"

------------

CD-ROM and software copyright (C) 1999-2002 Sams Publishing & Pearson Education Inc., 
Andre' LaMothe, Xtreme Games LLC. All rights reserved. Individual programs are 
copyrighted by their respective owners and may require separate licensing. 
This CD-ROM may not be redistributed without prior written permission from 
the publisher. The right to redistribute the individual programs on the CD-ROM 
depends on each program's license. Consult each program for details.

------------

Table of Contents:

I.    INTRODUCTION
II.   THE BONUS ARTICLES and BOOKS on General 3D and Direct3D (PLEASE READ !!!!!!!)
III.  THE GENESIS 3D ENGINE FROM ECLIPSE SOFTWARE
IV.   INSTALLING DIRECTX
V.    INSTALLING THE SOURCE CODE FROM THE BOOK
VI.   INSTALLING THE APPLICATIONS
VII.  PLAYING THE GAMES :)
VIII. PROBLEMS YOU MIGHT ENCOUNTER (PLEASE READ!!!!!!)


I. INTRODUCTION

   Welcome to the wonderful world of Tricks Next Gen! On this CD is a
   plethora of digital treasures from source code, games, and DirectX, to
   applications, and electronic bonus books on 3D, Direct3D, and general topics.

   The first thing you might notice is that isn't any kind of main installation
   program on the CD. I have found that 9 out of 10 times the best installer is
   the reader since only he/she knows what and where to install things, so I'm
   going to leave the actuall file copying and installation to you.

   However, within each directory there is a README.TXT file that explains what
   the files are and what they are for. The CD is set up like this:

   T3DGAMER1 <DIR> - The main directory.
      |
      \DIRECTX       <DIR> - Contains DirectX SDK.
      \APPLICATIONS  <DIR> - Contains the Applications.
      \ONLINEBOOKS   <DIR> - Contains the On-Line books on 3D.
      \ARTICLES      <DIR> - Contains the extra articles.
      \ARTWORK       <DIR> - Contains stock art media for your use.
      \SOUND         <DIR> - Contains stock sound and MIDI for your use.
      \SOURCE        <DIR> - Contains the entire source code for the book.
      \GAMES         <DIR> - A lot of cool games!
      \ENGINES       <DIR> - This directory contains numerous 2D/3D engines including
                             the entire Genesis3D engine from Eclipse Software 
                             along with on-line API and usuage manuals written by
                             David Dougher.

II.   THE BONUS ARTICLES and BOOKS on General 3D and Direct3D

      Volume II of Tricks will contain chapters on the mathematics and software     
      algorithms on 3D graphics written by me (Andre' LaMothe). However, to
      get you started on 3D, so you don't have to wait for Volume II, two contributing
      authors have written complete on-line books on the subject of General 3D and
      Direct3D. These are really part of the physical book and you should of course 
      read them when you are ready. They cyber-books can each be found in the following
      directories:

      T3DGAMER1\ONLINEBOOKS\DIRECT3D_ONLINE\ - The chapters, source, etc. for the complete 
      on-line D3D Retained\Immediate Mode book by the brilliant young 
      programmer\author -- Matthew Ellis.

      T3DGAMER1\ONLINEBOOKS\GENERAL3D_ONLINE\ - The chapters, and support files for the 
      complete on-line general 3D book by Sergei Savchenko, one of the worlds leading
      3D engine developers and the developer of the 3DGPL engine.

      Both books consist of a number of chapters in either .HTML, or .DOC. We put the
      books on the CD for two reasons: to save you money in the printing of the book
      and keep the page count down, and since Direct3D is changing so quickly I 
      didn't want to date the internal material inside the book.

      In addition to the books there are numerous articles, white papers, and
      a bonus Chapter on game programming on the CD also. They can be found in:

      T3DGAMER1\ARTICLES\


III. THE GENESIS 3D ENGINE FROM ECLIPSE SOFTWARE

     Also contained on the CD is the latest edition of one on the most advanced 3D 
     engines on the market -- The Genesis 3D engine from Eclipse Software.
     This engine along with the tools will allow you to create Quake level
     games without any programming -- or very little. In addition to the engine
     and its supporting documentation and tools are a number of on-line manuals
     explaining the Genesis3D engine in detail along with the use of its API 
     written by David Dougher.

IV. INSTALLING DIRECTX

     To run the DirectX programs from this book and to create your own, you
     must have the DirectX 8.0 (or better) Run-Time system and Software
     Development Kit (SDK) loaded. Both the Run-Time files and the SDK
     are contained on this CD within the DIRECTX\ directory. 

     The README.TXT within the DIRECTX\ sub-directory will give you more
     details on the setup and installation of DirectX, but basically you
     must first install the run-time files and then the SDK. The SDK is
     needed by your compiler, so it can access the header, and library
     files needed to build DirectX applications.


V. INSTALLING THE SOURCE CODE FROM THE BOOK

     The source code and data for each chapter of this book are contained
     within the SOURCE\ sub-directory. I suggest simply copying the entire
     SOURCE\ directory as-is onto your hard drive by "dragging" it from
     the this CD or by using XCOPY or other similar technique.
     
VI.   INSTALLING THE APPLICATIONS

     There are a number of awesome applications that are contained on this
     CD such as TrueSpace, Paint Shop Pro, WinZip, sound editors, MS Word Reader, 
     and Acrobat Reader which you need to read anything in .PDF format.

     Each one of these applications installs as a self extracting .EXE and
     takes just a couple minutes. Take a look at the README.TXT file for
     each application for details.


VII.  PLAYING THE GAMES :)

     Last, but definetely not least are the games!!! I have placed a number
     of games on the CD for your enjoyment. They are contained within
     the GAMES\ sub-directory. Take a look at each to see how to install them.


VIII. PROBLEMS YOU MIGHT ENCOUNTER

     * Read Only Flag *

     This is a very important detail, so read on. When creating a CD ROM disk
     all the files will be written with the READ-ONLY flag enabled. This is 
     fine in most cases unless you copy the files to your hard drive (which you will)
     and then edit and try to write the files back to disk. You will get a 
     READ-ONLY protection error. 

     Fixing this is a snap. You simply need to clear the READ-ONLY flag on any 
     files that you want to modify. There are 3 ways to do it. First, you can 
     do it with Windows and simply navigate into the directory with the file 
     you want to clear the READ-ONLY flag and then select the file, press 
     the RIGHT mouse button to get the file properties and then clear the 
     READ-ONLY flag and APPLY you change. You can do this will more than one 
     file at once by selecting a group of files.

     The second way is to use the File Manager and perform a similar set of 
     operations as in the example above. The third and best way is to the DOS 
     ATTRIB command with a DOS prompt. Here's how: 

     Assume you have copied the entire SOURCE directory on your hard drive to the 
     location C:\SOURCE. To reset all the READ-ONLY attributes in one fell 
     swoop you can use the ATTRIB command. Here is the syntax:
    
     C:\SOURCE\ATTRIB -r *.* /s

     This says to clear the READ-ONLY flag "r" from all files "*.*" and all lower 
     sub-directories "/s".
                       
     * DirectX Driver Problems *

     DirectX may not have a driver for your video or sound card. If this is the case, 
     you will be notified during installation. But don't worry if DirectX doesn't 
     install a driver, it will still work, just not as good. If this happens you'll 
     have to go to the manufacturer of your video or sound card and look for DirectX 
     drivers, or else, keep an eye out at the Microsoft DirectX site for new updates.
 

     * Compilation Problems *

     99% of compiler problems are "pilot" errors. So before you think that something 
     is wrong, make sure that you have all your DirectX library, and header paths 
     set up in the compiler. Make sure that you are creating a standard Win32 .EXE 
     (if you are making a DirectX app). Make sure you have manually included the 
     DirectX .LIB files in your link list. And finally, make sure that you can compile 
     a basic "hello world" program. Don't jump into DirectX Windows programming 
     without getting your compiler and environment setup properly.

     * Video Problems *

     There are some video cards that even with a DirectX driver, still don't work 
     correctly. This will appear as a sudden flash of black or white video
     when you try to run some of the demos. If this happens, try going into the
     display properties and reseting your machine's video to desired resolution and 
     bit depth, for example: 640x480 in 256 colors, or 24-bit mode, etc. Also, you 
     MUST have your desktop set to 16-bit mode for the 16-bit windowed mode demos 
     since they can't change video modes and must use the current desktop mode. 
     
     To make the switching quick, there is a little utility 
     called "Quickres" that is part of the "PowerTools" utility suite. Check out
     the Web for the program. It sits in your system tray and allows you to instantly
     change video modes without re-booting.

     * Reading the Articles and On-Line Books
 
       Everything on the CD is either in .DOC, .TXT, .PDF, or .HTML. Hence, you will
       need something that can read .HTML such as Internet Explorer, or Netscape 
       Communicator, Adobe Acrobat Reader for .PDF, and MS WORD 7.0+, or the MS WORD
       Reader to read the .DOC files. If you don't have a browser then you're own your
       own, but the Adobe Acrobat Reader and the MS WORD 7.0 Reader are located in the
       APPLICATIONS\ directory on this CD. Hence, you can install them from there,
       after which you will be able to read .PDF and .DOC files (if you can't already).




What is the MaTriX?

Lord Necron - 2002



