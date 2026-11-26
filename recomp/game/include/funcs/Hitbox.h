/* funcs/Hitbox.h - prototypes of the Hitbox module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Hitbox_TestAttackBox(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t *a1, uint32_t a3, uint32_t *a4, uint32_t a5, uint32_t *a6);	/* $004DEC regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,a0,&a1,a3,&a4,a5,&a6) clobbers(a0,a2) */
void Hitbox_TestAttackBoxAt(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t *a1, uint32_t a3, uint32_t *a4, uint32_t a5, uint32_t *a6);	/* $004DF4 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,a0,&a1,a3,&a4,a5,&a6) clobbers(a0,a2) */
void Hitbox_KindFirstPlayer(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t *a1, uint32_t a3, uint32_t *a4, uint32_t a5, uint32_t *a6);	/* $004E22 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,a0,&a1,a3,&a4,a5,&a6) clobbers(a0,a2) */
void Hitbox_KindAllPlayers(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t *a1, uint32_t a3, uint32_t *a4, uint32_t a5, uint32_t *a6);	/* $004E26 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,a0,&a1,a3,&a4,a5,&a6) clobbers(a0,a2) */
void Hitbox_KindFirstEnemy(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t *a1, uint32_t a3, uint32_t *a4, uint32_t a5, uint32_t *a6);	/* $004E2A regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,a0,&a1,a3,&a4,a5,&a6) clobbers(a0,a2) */
void Hitbox_HitFirstPlayerBox(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t *a1, uint32_t *a2, uint32_t a3, uint32_t *a4, uint32_t a5, uint32_t *a6);	/* $004E32 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,a0,&a1,&a2,a3,&a4,a5,&a6) clobbers(a0) */
void Hitbox_Strike(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, const HitBox *attack, uint32_t *a1, HitBox *struck, uint32_t a3, uint32_t *a4, uint32_t a5, uint32_t *a6);	/* $004E52 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,a0,&a1,a2,a3,&a4,a5,&a6) clobbers(a0) */
void Hitbox_LinkByKind_Enemy(uint32_t a0, uint32_t a1);	/* $004F46 regs(a0,a1) ccout(NZVC) */
void Hitbox_InitLists(void);	/* $008C1A regs() clobbers(a0,a1) */
void Hitbox_LinkPlayerList(uint32_t a0, uint32_t a1, dk::Flags *ccr);	/* $008C3C regs(a0,a1,&ccr) ccout(NZVC) */
void Hitbox_LinkEnemyList(uint32_t a0, uint32_t a1, dk::Flags *ccr);	/* $008C54 regs(a0,a1,&ccr) ccout(NZVC) */
void Hitbox_Unlink(uint32_t a0, uint32_t a1);	/* $008C6C regs(a0,a1) */
bool Hitbox_FindPlayerBox(uint32_t *d0, uint16_t d4, uint16_t d5, uint16_t d6, uint16_t d7, uint32_t *a2);	/* $008C80 regs(&d0,d4,d5,d6,d7,&a2)->Z */
bool Hitbox_FindEnemyBox(uint32_t *d0, uint16_t d4, uint16_t d5, uint16_t d6, uint16_t d7, uint32_t *a2);	/* $008C8A regs(&d0,d4,d5,d6,d7,&a2)->Z */
void Hitbox_FindNextBox(uint32_t *d0, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7, HitBox **box, dk::Flags *found);	/* $008C94 regs(&d0,d4,d5,d6,d7,&a2,&ccr) ccout(Z) */
bool Hitbox_FindPlayerBoxOfId(uint32_t *d0, uint16_t d3, uint16_t d4, uint16_t d5, uint16_t d6, uint16_t d7, uint32_t *a2);	/* $008CB6 regs(&d0,d3,d4,d5,d6,d7,&a2)->Z */
void Hitbox_FindNextBoxOfId(uint32_t *d0, uint16_t d3, uint16_t d4, uint16_t d5, uint16_t d6, uint16_t d7, uint32_t *a2, dk::Flags *ccr);	/* $008CCA regs(&d0,d3,d4,d5,d6,d7,&a2,&ccr) ccout(NZVC) */
void Hitbox_WipeEnemies(uint16_t *d0, uint16_t *d1);	/* $008FFE regs(&d0,&d1) ccout(NVC) clobbers(a0) */
void Hitbox_CanHitTarget(uint32_t *d0, uint16_t *d1, uint32_t *a1, HitBox *box, dk::Flags *ccr);	/* $0107DE regs(&d0,&d1,&a1,a2,&ccr) ccout(NZVC) */
void Hitbox_CanHitPlayer(uint32_t *d0, uint16_t *d1, uint32_t *a1, HitBox *box, dk::Flags *ccr);	/* $0107E8 regs(&d0,&d1,&a1,a2,&ccr) ccout(NZVC) */
void Hitbox_SpawnAtContact(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t d3, uint32_t d4, uint32_t d5, uint32_t d6, uint32_t d7, uint32_t a0, uint32_t script, const HitBox *box, uint32_t a3, uint32_t *a4, uint32_t a5, uint32_t *a6);	/* $01088A regs(&d0,&d1,&d2,d3,d4,d5,d6,d7,a0,a1,a2,a3,&a4,a5,&a6) */
void Hitbox_OverlapCenter(uint32_t *d0, uint32_t *d1, uint16_t *x1, uint16_t *y1, uint16_t *x2, uint16_t *y2, const HitBox *box);	/* $0108A6 regs(&d0,&d1,&d4,&d5,&d6,&d7,a2) clobbers(d2,d3) */
void Hitbox_HitAllPlayerBoxes(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t *a1, uint32_t *a2, uint32_t a3, uint32_t *a4, uint32_t a5, uint32_t *a6);	/* $004E3A regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,a0,&a1,&a2,a3,&a4,a5,&a6) clobbers(a0) */
void Hitbox_HitFirstEnemyBox(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t *a1, uint32_t *a2, uint32_t a3, uint32_t *a4, uint32_t a5, uint32_t *a6);	/* $004E42 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,a0,&a1,&a2,a3,&a4,a5,&a6) clobbers(a0) */
void Hitbox_StrikeAll(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, const HitBox *attack, uint32_t *a1, HitBox *struck, uint32_t a3, uint32_t *a4, uint32_t a5, uint32_t *a6);	/* $004E64 regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,a0,&a1,a2,a3,&a4,a5,&a6) clobbers(a0,a2) */
void Hitbox_LinkByKind(uint32_t a0, uint32_t a1);	/* $004F3E regs(a0,a1) ccout(NZVC) */
void Hitbox_DamageAllEnemies(uint16_t d0, uint16_t d1, dk::Flags *ccr);	/* $008CF2 regs(d0,d1,&ccr) ccout(NVC) clobbers(a0) */

/* a box's hit callback (HitBox.on_hit): the union of the convention its targets have in the corpus's
 * call-site log ($4E5C, $4E6E: PlayerAtk_HitPause, EnemyAtk_HitKnockSpeed, ...); usp passes through */
void HitCallback(uint32_t code, uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t *a0, uint32_t a2, uint32_t a3, uint32_t *a4, uint32_t a5, uint32_t *a6);	/* type regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&a0,a2,a3,&a4,a5,&a6) ccout(Z) */
void Hitbox_CanHitOwner(uint32_t *d0, uint16_t *d1, uint32_t *a1, HitBox *box, dk::Flags *ccr);	/* $0107FA regs(&d0,&d1,&a1,a2,&ccr) ccout(NZVC) */
void HitboxLinkFn(uint32_t code, uint32_t a0, uint32_t a1);	/* type regs(a0,a1) */
void Hitbox_TestBox(uint32_t *d0, uint16_t d4, uint16_t d5, uint16_t d6, uint16_t d7, uint32_t *a2, dk::Flags *ccr);	/* $008C9A regs(&d0,d4,d5,d6,d7,&a2,&ccr) ccout(NZVC) */
void Hitbox_TestBoxOfId(uint32_t *d0, uint16_t d3, uint16_t d4, uint16_t d5, uint16_t d6, uint16_t d7, uint32_t *a2, dk::Flags *ccr);	/* $008CD0 regs(&d0,d3,d4,d5,d6,d7,&a2,&ccr) ccout(NZVC) */
void Hitbox_ScanDone(void);	/* $008CB4 regs() */
void Hitbox_ScanOfIdDone(void);	/* $008CF0 regs() */
void Hitbox_KindAllEnemies(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t *a1, uint32_t a3, uint32_t *a4, uint32_t a5, uint32_t *a6);	/* $004E2E regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,a0,&a1,a3,&a4,a5,&a6) clobbers(a0,a2) */
void Hitbox_HitAllEnemyBoxes(uint32_t *d0, uint32_t *d1, uint32_t *d2, uint32_t *d3, uint32_t *d4, uint32_t *d5, uint32_t *d6, uint32_t *d7, uint32_t a0, uint32_t *a1, uint32_t *a2, uint32_t a3, uint32_t *a4, uint32_t a5, uint32_t *a6);	/* $004E4A regs(&d0,&d1,&d2,&d3,&d4,&d5,&d6,&d7,a0,&a1,&a2,a3,&a4,a5,&a6) clobbers(a0) */
void Hitbox_LinkByKind_AllPlayers(uint32_t a0, uint32_t a1);	/* $004F42 regs(a0,a1) ccout(NZVC) */
void Hitbox_LinkByKind_AllEnemies(uint32_t a0, uint32_t a1);	/* $004F4A regs(a0,a1) ccout(NZVC) */
