bin_PROGRAMS = congratbot
dist_noinst_DATA = config.proto
BUILT_SOURCES = config.pb.h

congratbot_SOURCES = main.cc config.pb.cc config.pb.h
EXTRA_CPPFLAGS = -I$(top_builddir)
MOSTLYCLEANFILES = $(congratbot_SOURCES)

%.pb.cc %.pb.h: %.proto
	$(PROTOC) --cpp_out=$(builddir) $^
