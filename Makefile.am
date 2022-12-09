bin_PROGRAMS = congratbot query_profile
dist_noinst_DATA = config.proto
BUILT_SOURCES = config.pb.cc config.pb.h

congratbot_SOURCES = main.cc config.pb.cc config.pb.h
query_profile_SOURCES = query_profile.cc config.pb.cc config.pb.h
EXTRA_CPPFLAGS = -I$(top_builddir)
MOSTLYCLEANFILES = $(BUILT_SOURCES)

%.pb.cc %.pb.h: %.proto
	$(PROTOC) --cpp_out=$(builddir) $^
