# makefile for muon
#
# For MinGW modify the EXEEXT
CXX = g++
#CXXFLAGS = -Wall -DWIN32 -D_WIN32
CXXFLAGS = -Wall
LIBS =
OBJS =
EXEEXT =
#EXEEXT = .exe

.SUFFIXES: .cpp .o

all: gencoinz$(EXEEXT) coinanaz$(EXEEXT) coinratez$(EXEEXT) coinrate$(EXEEXT) \
	coinimg$(EXEEXT) coinimgs$(EXEEXT) testhits$(EXEEXT) testrec1$(EXEEXT) \
	coinana$(EXEEXT) genacc2$(EXEEXT) gencoinlist$(EXEEXT)

gencoinz$(EXEEXT): gencoinz.o coinopt.o coinmake.o databank.o datarecord.o xyunitdata.o \
		gzfstream.o gzipfilebuf.o $(OBJS)
	$(CXX) -s -o $@ gencoinz.o coinopt.o coinmake.o databank.o datarecord.o xyunitdata.o \
		gzfstream.o gzipfilebuf.o $(OBJS) -lz $(LIBS)

coinanaz$(EXEEXT): coinanaz.o gzfstream.o gzipfilebuf.o $(OBJS)
	$(CXX) -s -o $@ coinanaz.o gzfstream.o gzipfilebuf.o $(OBJS) -lz $(LIBS)

coinratez$(EXEEXT): coinratez.o gzfstream.o gzipfilebuf.o $(OBJS)
	$(CXX) -s -o $@ coinratez.o gzfstream.o gzipfilebuf.o $(OBJS) -lz $(LIBS)

coinrate$(EXEEXT): coinrate.o coinrecord.o gzfstream.o gzipfilebuf.o mytimer.o $(OBJS)
	$(CXX) -s -o $@ coinrate.o coinrecord.o gzfstream.o gzipfilebuf.o mytimer.o $(OBJS) -lz $(LIBS)

coinimg$(EXEEXT): coinimg.o coinrecord.o hist1d.o hist2d.o gzfstream.o gzipfilebuf.o ncpng.o mytimer.o $(OBJS)
	$(CXX) -s -o $@ coinimg.o coinrecord.o hist1d.o hist2d.o gzfstream.o gzipfilebuf.o ncpng.o mytimer.o $(OBJS) -lz $(LIBS)

coinimgs$(EXEEXT): coinimgs.o coinrecord.o gzfstream.o gzipfilebuf.o ncpng.o mytimer.o $(OBJS)
	$(CXX) -s -o $@ coinimgs.o coinrecord.o gzfstream.o gzipfilebuf.o ncpng.o mytimer.o $(OBJS) -lz $(LIBS)

testhits$(EXEEXT): testhits.o sglrate.o xyunitdata.o gzfstream.o gzipfilebuf.o $(OBJS)
	$(CXX) -s -o $@ testhits.o sglrate.o xyunitdata.o gzfstream.o gzipfilebuf.o $(OBJS) -lz $(LIBS)

testrec1$(EXEEXT): testrec1.o gzfstream.o gzipfilebuf.o $(OBJS)
	$(CXX) -s -o $@ testrec1.o gzfstream.o gzipfilebuf.o $(OBJS) -lz $(LIBS)

genacc2$(EXEEXT): genacc2.o accopt.o xyunitgeom.o hist2d.o hist1d.o ncpng.o mytimer.o $(OBJS)
	$(CXX) -s -o $@ genacc2.o accopt.o xyunitgeom.o hist2d.o hist1d.o ncpng.o mytimer.o $(LIBS)

coinana$(EXEEXT): coinana.o coinrecord.o hist1d.o hist2d.o ncpng.o gzfstream.o gzipfilebuf.o mytimer.o $(OBJS)
	$(CXX) -s -o $@ coinana.o coinrecord.o hist1d.o hist2d.o ncpng.o gzfstream.o gzipfilebuf.o mytimer.o $(OBJS) -lz $(LIBS)

gencoinlist$(EXEEXT): gencoinlist.o
	$(CXX) -s -o $@ gencoinlist.o

# Suffix rule
.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

# Cleanup rule
clean:
	$(RM) $(OBJS)

# dependencies
coinopt.o:	coinopt.h
coinmake.o:	coinmake.h coinopt.h datarecord.h databank.h xyunitdata.h
coinrecord.o:	coinrecord.h
gencoinz.o:	coinopt.h gzfstream.h gzipfilebuf.h
databank.o:	databank.h datarecord.h xyunitdata.h
datarecord.o:	datarecord.h xyunitdata.h
xyunitdata.o:	xyunitdata.h
gzstream.o:	gzfstream.h gzipfilebuf,h
gzipfilebuf.o:	gzipfilebuf.h
coinanaz.o:	gzfstream.h gzipfilebuf.h
coinratez.o:	gzfstream.h gzipfilebuf.h
coinrate.o:	coinrecord.h gzfstream.h gzipfilebuf.h mytimer.h
coinimg.o:	coinrecord.h hist1d.h hist2d.h gzfstream.h gzipfilebuf.h ncpng.h mytimer.h
coinimgs.o:	coinrecord.h gzfstream.h gzipfilebuf.h ncpng.h mytimer.h
coinrecord.o:	coinrecord.h
dlimg.o:	coinrecord.h hist1d.h hist2d.h gzfstream.h gzipfilebuf.h ncpng.h mytimer.h
hist1d.o:	hist1d.h
hist2d.o:	hist2d.h
ncpng.o:	ncpng.h
mytimer.o:	mytimer.h
xyunitgeom.o:	xyunitgeom.h
sglrate.o:	sglrate.h xyunitdata.h
testhits.o:	sglrate.h gzfstream.h
testrec1.o:	gzfstream.h
accopt.o:	accopt.h
genacc2.o:	accopt.h hist2d.h hist1d.h ncpng.h mytimer.h
coinana.o:	coinrecord.h hist1d.h hist2d.h gzfstream.h gzipfilebuf.h ncpng.h mytimer.h

#-- end of makefile
