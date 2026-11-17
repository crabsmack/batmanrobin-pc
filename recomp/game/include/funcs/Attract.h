/* funcs/Attract.h - prototypes of the Attract module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Attract_Skyscraper(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $048AD2 regs(&d1,&d3,&d4,&d5,&d6,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
void Attract_SpawnSkyscraperCamera(uint16_t d4, uint16_t d5, uint32_t *a3, uint32_t *a4, uint32_t *a6);	/* $048B1E regs(d4,d5,&a3,&a4,&a6) clobbers(d0,d1,d7,a0,a2,a5) */
void Attract_SpawnPlaneBDriver(uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a6);	/* $048B8E regs(d4,d5,&d7,&a2,&a3,&a4,a6) clobbers(d0,d1,a0,a5,a6) */
void Attract_SkyscraperScroll(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint16_t *d4, uint32_t *d5, uint16_t *d6, uint16_t *d7, const Vm2Obj *self);	/* $048C36 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,a6) clobbers(a0,a2,a3,a6) */
void Attract_SpawnRooftopHeroes(uint32_t *d0, uint16_t *d1, uint16_t d4, uint16_t d5, uint32_t *d7, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t a6);	/* $048CD6 regs(&d0,&d1,d4,d5,&d7,&a2,&a3,&a4,a6) clobbers(a0,a5,a6) */
void Attract_RooftopLightning(uint16_t *d0, uint32_t *d1, Vm2Sprite *spr, dk::Flags *ccr);	/* $048E80 regs(&d0,&d1,a5,&ccr) ccout(NVC) */
uint16_t Attract_PlaneBFollowObj(const Vm2Obj *self);	/* $048CA6 regs(a6)->d0 */
void Attract_Rooftop(uint32_t *d1, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *a1, uint32_t *a2, uint32_t *a3, uint32_t *a4, uint32_t *a5, uint32_t *a6);	/* $048CBA regs(&d1,&d3,&d4,&d5,&d6,&a1,&a2,&a3,&a4,&a5,&a6) clobbers(d0,d2,d7,a0) */
