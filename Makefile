#
# top level makefile for drss
#
# //
# //  Copyright (C) 2010 - Bernd H Stramm 
# //
# // This program is distributed under the terms of 
# // the GNU General Public License version 3 
# //
# // This software is distributed in the hope that it will be useful, 
# // but WITHOUT ANY WARRANTY; without even the implied warranty 
# // of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
# //


EXEC = drss
MAKEFILE = Make_Q

all:
	sh build.sh
  
clean:
	if [ -e $(MAKEFILE) ]; then $(MAKE) -i -f $(MAKEFILE) clean; fi 
	if [ -e $(MAKEFILE) ]; then $(RM) $(MAKEFILE)* ; fi 
	$(RM) $(EXEC)
  
install:
	install $(EXEC) $(DESTDIR)

