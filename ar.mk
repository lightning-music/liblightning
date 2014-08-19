
include objs.mk

LIBLIGHTNING_AR=liblightning.a

$(LIBLIGHTNING_AR): $(OBJS)
	$(AR) rcs $(LIBLIGHTNING_AR) $^
