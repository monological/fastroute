# Makefile for generic list package.
#
# Last edited: Tue Jul 28 15:40:05 1992 by bcs (Bradley C. Spatz) on wasp
#
# Copyright (C) 1992 Bradley C. Spatz, bcs@ufl.edu
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#

#CFLAGS	= $(DEFINES) -fstrength-reduce -finline-functions
CFLAGS	+= $(DEFINES)
#DEFINES = -DUSE_MACROS
LIBS	= -L. -llist
#LIBS	= -L . -llist   # Use this for HP-UX; great loader guys!
CPP	= cpp -E -P -C
MANDIR	= /usr/share/man
LIBDIR	= /usr/lib
INCDIR	= /usr/include

#add macro for ranlib 4/96 *kob* - ranlib doesn't exist on solaris
RANLIB = ls

# We specify some goofy dependencies between the man pages and the source,
# because the man page reflects whether USE_MACROS was specified.  Thus,
# the dependencies that follow insure the library and the man page
# are always in sync.  A similar argument goes for the header file and it's
# prototype.  To be sure, you can always make clean and make.

all:		liblist.a 
		(cd examples; make)

liblist.a:	list.o
		ar rc liblist.a list.o
		$(RANLIB) liblist.a

list.o:		list.h list.3 Makefile

list.h:		list.h.proto Makefile
		$(CPP) $(DEFINES) list.h.proto > list.h

list.3:		list.man list.c Makefile
		$(CPP) $(DEFINES) list.man > list.3

queue.3:	queue.man list.c Makefile
		$(CPP) $(DEFINES) queue.man > queue.3

stack.3:	stack.man list.c Makefile
		$(CPP) $(DEFINES) stack.man > stack.3

install:
		install -c liblist.a $(DESTDIR)$(LIBDIR)
		$(RANLIB) $(DESTDIR)$(LIBDIR)/liblist.a
		install -c list.h $(DESTDIR)$(INCDIR)
		install -c queue.h $(DESTDIR)$(INCDIR)
		install -c stack.h $(DESTDIR)$(INCDIR)
		(cd examples/cache; make install)

install.man:
		install -c list.3 $(DESTDIR)$(MANDIR)/man3
		install -c queue.3 $(DESTDIR)$(MANDIR)/man3
		install -c stack.3 $(DESTDIR)$(MANDIR)/man3

clean:
		rm -f *.o *.a list.h *.3 core
		(cd examples; make clean)
