# ------------------------------------------------------------
# Makefile for CMPUT 379 A1
#
# Usage: make -f Makefile compile  // compile program
#        make -f Makefile run      // run the program
#	      	 	     	      // Note: making this target may trigger
#				      // compiling the program	 
#        make -f Makefile tar      // create a 'tar.gz' archive of 'allFiles'
#        make -f Makefile clean     // remove unneeded files
# ------------------------------------------------------------

target=		submit
allFiles=       Makefile a1mon.cpp a1jobs.cpp
# ------------------------------------------------------------


compile:  main.cpp
	g++ -std=c++11 -Wall a1jobs.cpp -o a1jobs
	g++ -std=c++11 -Wall a1mon.cpp -o a1mon

tar:
	touch $(target).tar.gz
	mv $(target).tar.gz  x$(target).tar.gz
	tar -cvf $(target).tar $(allFiles)
	gzip $(target).tar

clean:
	rm *~ out.* *.o