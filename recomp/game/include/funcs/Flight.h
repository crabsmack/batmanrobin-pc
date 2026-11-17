/* funcs/Flight.h - prototypes of the Flight module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Flight_DrawCorridor(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint16_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a4);	/* $024900 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a4) */
void Flight_CopyBandLines(uint16_t vscroll, uint16_t *count, u16 **hlines, u16 **vlines, const u16 **bands, dk::Flags *ccr);	/* $024ABC regs(d2,&d7,&a0,&a1,&a4,&ccr) ccout(NZVC) */
void Flight_AirshipSetMapOrigin(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);	/* $02F580 regs(&d0,&d1,&d2,d3,&d4,&d5,&d6,&d7,a1,a2,a3,a4,a5,a6) clobbers(a0) */
void Flight_AirshipRunTriggers(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);	/* $02F58C regs(&d0,&d1,&d2,d3,&d4,&d5,&d6,&d7,a1,a2,a3,a4,a5,a6) clobbers(a0) */
void Flight_AirshipMapToWorld(uint16_t *d4, uint16_t *d5, uint32_t a6);	/* $02F5DA regs(&d4,&d5,a6) */
void Flight_SpawnAirshipPathObj(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t a6);	/* $02F5F4 regs(&d0,&d1,d4,d5,&a2,&a3,&a4,&a5,a6) clobbers(d7,a0,a6) */
void Flight_AirshipPathObjPark(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7, uint32_t a1, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6);	/* $02F602 regs(d0,d1,d2,d3,d4,d5,d6,d7,a1,a3,a4,a5,a6) */
void Flight_AirshipStartPath(uint16_t *d1, uint32_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a6);	/* $02F60E regs(&d1,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a6) clobbers(d0) */
void Flight_SpawnStrobeLight(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *a0, uint32_t *a2, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $02F69E regs(&d1,d4,d5,&a0,&a2,&a4,&a5,&a6) ccout(Z) clobbers(d0,d7,a3) */
void Flight_SpawnRedNavLight(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *a0, uint32_t *a2, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $02F6CE regs(&d1,d4,d5,&a0,&a2,&a4,&a5,&a6) ccout(Z) clobbers(d0,d7,a3) */
void Flight_SpawnRedNavLightFlip(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *a0, uint32_t *a2, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $02F6E6 regs(&d1,d4,d5,&a0,&a2,&a4,&a5,&a6) ccout(Z) clobbers(d0,d7,a3) */
void Flight_SpawnGreenNavLight(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *a0, uint32_t *a2, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $02F6FE regs(&d1,d4,d5,&a0,&a2,&a4,&a5,&a6) ccout(Z) clobbers(d0,d7,a3) */
void Flight_SpawnSearchlight(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *a0, uint32_t *a2, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $02F72E regs(&d1,d4,d5,&a0,&a2,&a4,&a5,&a6) ccout(Z) clobbers(d0,d7,a3) */
void Flight_RunScene(uint32_t a5);	/* $033274 regs(a5) */
void Flight_StartCorridorNoWaves(uint32_t *d0, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3);	/* $033322 regs(&d0,&d6,&d7,&a0,&a1,&a2,&a3) tst(d0.l) ccout(Z) */
void Flight_FrameCorridor(uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $03334A regs(&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a2,&a3,&a4,&a5,&a6) clobbers(d0,a0,a1) */
uint32_t Flight_StartSkyOnly(void);	/* $033382 regs()->d0 tst(d0.l) ccout(Z) */
void Flight_StartAirship(uint32_t *d0, uint16_t *d1, uint32_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a6);	/* $033398 regs(&d0,&d1,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a6) tst(d0.l) ccout(Z) */
void Flight_FrameCorridorToClouds(uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $033402 regs(&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a2,&a3,&a4,&a5,&a6) clobbers(d0,a0,a1) */
void Flight_DrawSky(uint32_t *d0, uint32_t d1, uint32_t *d2, uint32_t *d3, uint16_t *d7, uint32_t *a0, uint32_t a1);	/* $033448 regs(&d0,d1,&d2,&d3,&d7,&a0,a1) clobbers(a1) */
void Flight_ScrollCloudBank(uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint32_t *d5, uint16_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a6);	/* $033490 regs(&d1,&d2,&d3,&d4,&d5,&d6,&a1,&a2,&a3,&a6) clobbers(d0,d7,a0) */
void Flight_BuildWobbleTable(uint16_t *d7, uint32_t *a0, uint32_t *a1);	/* $0334B4 regs(&d7,&a0,&a1) clobbers(d0) */
void Flight_CopyWobbleWords(uint16_t *d7, uint32_t *a0, uint32_t *a1);	/* $0334D2 regs(&d7,&a0,&a1) clobbers(d0) */
void Flight_UpdateCorridor(uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint16_t *d6, uint32_t *d7, uint32_t *a4);	/* $024824 regs(&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a4) clobbers(d0,a0,a1) */
void Flight_AirshipLoadMapPos(uint32_t a6);	/* $0303F8 regs(a6) */
void Flight_AirshipStoreMapPos(uint16_t *d4, uint16_t *d5, uint32_t a6);	/* $030406 regs(&d4,&d5,a6) */
void Flight_StartCorridor(uint32_t *d0, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3);	/* $0332FA regs(&d0,&d6,&d7,&a0,&a1,&a2,&a3) tst(d0.l) ccout(Z) */
void Flight_StartClouds(uint32_t *d0, uint32_t *d6, uint16_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3);	/* $033356 regs(&d0,&d6,&d7,&a0,&a1,&a2,&a3) tst(d0.l) ccout(Z) */
uint8_t Flight_WaitCloudsPassed(void);	/* $03337C regs()->d0 tst(d0.b) ccout(Z) */
void Flight_FrameAirship(uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint16_t *d6, uint16_t *d7, uint32_t a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $0333B2 regs(&d1,&d2,&d3,&d4,&d5,&d6,&d7,a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,a0,a1) */
void Flight_FrameClouds(uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint32_t *d5, uint16_t *d6, uint16_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a6);	/* $03342E regs(&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a2,&a3,&a6) clobbers(d0,a0,a1) */
void Flight_StartBeamShip(uint32_t *d0, uint32_t *d1, uint16_t *d4, uint16_t *d5, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $033A62 regs(&d0,&d1,&d4,&d5,&d7,&a0,&a1,&a3,&a4,&a5,&a6) tst(d0.l) ccout(Z) */
void Flight_FrameBeamShip(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3);	/* $033A88 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3) */
void Flight_FrameCorridorClouds(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint16_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a6);	/* $03348A regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a6) ccout(NZVC) */
void Flight_CloudBankLines(uint32_t *d0, uint16_t *d1, uint32_t *a0, uint32_t *a1, uint32_t a6);	/* $0334E2 regs(&d0,&d1,&a0,&a1,a6) tst(d0.w) ccout(NZVC) */
/* the flying section's frame callback (`jsr (a0)` at $332F0 in Flight_RunScene, a0 = g_SceneVars+$98:
 * Flight_FrameCorridor, FrameCorridorToClouds, FrameClouds, DrawSky, FrameAirship, FrameBeamShip):
 * the union of their conventions, every register in and out */
void FlightFrameFn(uint32_t code, uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* type regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,&a1,&a2,&a3,&a4,&a5,&a6) */
void Flight_SpawnGreenNavLightFlip(uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *a0, uint32_t *a2, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $02F716 regs(&d1,d4,d5,&a0,&a2,&a4,&a5,&a6) ccout(Z) clobbers(d0,d7,a3) */
