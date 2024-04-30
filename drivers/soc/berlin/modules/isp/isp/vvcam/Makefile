all:
	@cd i2c;   make || exit $$?;
	@cd mipi;  make || exit $$?;
	@cd isp;   make || exit $$?;
	@cd vb;    make || exit $$?;
	@cd dw200; make || exit $$?;
#ifeq ($(ISP_VERSION), ISP8200_V2307)
	@cd videoin;    make || exit $$?;
#endif
	@cd mi_subsystem;   make || exit $$?;
ifdef V4L2
	@cd v4l2;    make || exit $$?;
endif

clean:
	@cd i2c;   make clean;
	@cd mipi;  make clean;
	@cd isp;   make clean;
	@cd vb;    make clean;
	@cd dw200; make clean;
#ifeq ($(ISP_VERSION), ISP8200_V2307)
	@cd videoin;    make clean;
#endif
	@cd mi_subsystem;   make clean;
ifdef V4L2
	@cd v4l2; make clean;
endif

