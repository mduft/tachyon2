# Copyright (c) 2010 by Markus Duft <mduft@gentoo.org>
# This file is part of the 'tachyon' operating system.

KERNEL_SOURCES	:= \
	$(wildcard $(SOURCEDIR)/core/*/*.c) \
	$(wildcard $(SOURCEDIR)/core/*/*.cc) \
	$(wildcard $(SOURCEDIR)/core/*/*.S) \
	$(wildcard $(SOURCEDIR)/core/*.c) \
	$(wildcard $(SOURCEDIR)/core/*.cc) \
	$(wildcard $(SOURCEDIR)/core/*.S) \
	$(KERNEL_ADD)

KERNEL_OBJECTS	:= $(KERNEL_SOURCES:.cc=.o)
KERNEL_OBJECTS	:= $(KERNEL_OBJECTS:.c=.o)
KERNEL_OBJECTS	:= $(KERNEL_OBJECTS:.S=.o)
KERNEL_OBJECTS	:= $(subst $(SOURCEDIR),$(BUILDDIR),$(KERNEL_OBJECTS))
