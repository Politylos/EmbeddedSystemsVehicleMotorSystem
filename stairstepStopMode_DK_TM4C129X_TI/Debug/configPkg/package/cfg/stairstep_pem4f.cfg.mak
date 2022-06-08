# invoke SourceDir generated makefile for stairstep.pem4f
stairstep.pem4f: .libraries,stairstep.pem4f
.libraries,stairstep.pem4f: package/cfg/stairstep_pem4f.xdl
	$(MAKE) -f C:\Users\polit\Documents\EmbeddedSystemsVehicleMotorSystem\stairstepStopMode_DK_TM4C129X_TI/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\polit\Documents\EmbeddedSystemsVehicleMotorSystem\stairstepStopMode_DK_TM4C129X_TI/src/makefile.libs clean

