; Raw, not-yet-explored ROM bytes. Replace with real code/data as it is understood.
incrom macro start,len
	binclude "batmanrobin.bin",start,len
	endm
