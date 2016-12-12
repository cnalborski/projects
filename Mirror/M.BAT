@echo off

rem wcl386.exe -s /l=pmodew read3ds.c
rem wcl386.exe -s /l=pmodew readvue.c

tasm.exe /zi virtual.asm
wcc386.exe main.c
wlink.exe system pmodew debug all option caseexact file main,virtual