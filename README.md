Gensouband (English Translation)

First of all, I am not the actual author of the variant itself. Only thing I
did was translating it into English and putting it on GitHub; it was
originally available through the author's website.

Here's the author's page with most recent build available:
http://www.miyamasa.net/heng_th_katte.html

Keep in mind the website blocks several IP ranges; try using a VPN/proxy
if you can't normally access it.


Notes:

- Keep in mind I'm not that proficient in Japanese, and English isn't my
native language either. This is an amateur translation at best; expect
mistranslations and/or awkward wording. I apologize in advance.

- As of now, this translation is functional but still not quite complete.
Majority of the game content should be translated; here's what left:

  - Speech lines for Gensoukyou uniques (Touhou Project characters)
  - Auto-picker/destroyer help file, testing
  - Any lines I might've missed

- I've also taken some pages from the Japanese wiki for this project and
translated them in text form. They're in the Documentation folder.
Link to wiki: https://w.atwiki.jp/hengtouhou/
Changed from base Hengband.txt is the author's list of main differences
between this variant and original Hengband.

- To build on Windows, I didn't have Visual Studio on hand, but I managed to
compile using Borland C++. The makefile I'm using is makefile.bcc. Keep in
mind you have to run it in Japanese locale in order to make it work.

- To build on Linux/Unix, I used src/makefile.std.  Having the nkf utility,
https://osdn.net/projects/nkf/ , in your path is a prerequisite for compilation
to work.  On Debian and Ubuntu, you can get nkf by installing the nkf package;
on OpenBSD, you can get it by running "pkg_add ja-nkf".  To compile the
English version, edit src/makefile.std to comment out (i.e. put a '#' as the
first character in the line) the line "JP_OPT= ..." which is for the Japanese
version (if you're on OpenBSD you also need to add "-DSGI" to the uncommented
line which sets CFLAGS, "CFLAGS = ..." and change the default compiler from
gcc to cc in the "CC = ..." line).  To compile, open a terminal, change
directories to the src subdirectory of the Touhou Katteban Hengband files, and
run "make -f makefile.std".  The resulting executable is named "hengband" and
is placed at the top level of the Touhou Katteban Hengband files.  To run it
from a terminal, change directories to the top level of the Touhou Katteban
Hengband files and run "./hengband". Note that if you have your `MAKEFLAGS`
environment variable set to use multiple jobs (-j2 or higher), there is a good 
chance compilation will fail. If you encounter an error during compilation,
consider using `unset MAKEFLAGS && make -f makefile.std` instead.
