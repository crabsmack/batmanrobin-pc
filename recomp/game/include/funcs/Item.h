/* funcs/Item.h - prototypes of the Item module: one line per function, its register
 * convention in the tag (dkit genesis recomp bridge). First written from the measured
 * contracts (dkit disasm regs); edited by the C's authors from then on. */
#pragma once
void Item_FoundBoxPlayer(PlayerRec **player, const void **box);	/* $012FB8 regs(&a0,&a1) ccout(NZVC) */
void Item_BoxPlayer(PlayerRec **player, const void **who, dk::Flags *flags);	/* $012FBC regs(&a0,&a1,&ccr) ccout(NZVC) */
uint32_t Item_LaunchFromHit(Vm2Motion *motion);	/* $0132BE regs(a3)->d0 clobbers(a0) */
void Item_SpawnBigHealth(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script);	/* $0133DE regs(&d0,&d1,d4,d5,&d7,a4,a6,&a2) clobbers(a0,a3,a4,a5,a6) */
void Item_SpawnBigHealthFromHit(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *motion, uint32_t a4, uint32_t a6);	/* $0133F2 regs(&d0,&d1,d4,d5,&d7,&a3,a4,a6) clobbers(a0,a2,a4,a5,a6) */
void Item_SpawnBigHealthDrifting(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script);	/* $01340A regs(&d0,&d1,d4,d5,&d7,a4,a6,&a2) clobbers(a0,a3,a4,a5,a6) */
const void *Item_CollectBigHealth(void);	/* $013440 regs()->a1 clobbers(a0) */
void Item_SpawnExtraLife(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script);	/* $01358E regs(&d0,&d1,d4,d5,&d7,a4,a6,&a2) clobbers(a0,a3,a4,a5,a6) */
void Item_SpawnExtraLifeFromHit(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *motion, uint32_t a4, uint32_t a6);	/* $0135A2 regs(&d0,&d1,d4,d5,&d7,&a3,a4,a6) clobbers(a0,a2,a4,a5,a6) */
void Item_SpawnExtraLifeDrifting(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script);	/* $0135BA regs(&d0,&d1,d4,d5,&d7,a4,a6,&a2) clobbers(a0,a3,a4,a5,a6) */
const void *Item_CollectExtraLife(void);	/* $0135F4 regs()->a1 clobbers(a0) */
void Item_SpawnBomb(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script);	/* $013706 regs(&d0,&d1,d4,d5,&d7,a4,a6,&a2) clobbers(a0,a3,a4,a5,a6) */
void Item_SpawnBombFromHit(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *motion, uint32_t a4, uint32_t a6);	/* $01371A regs(&d0,&d1,d4,d5,&d7,&a3,a4,a6) clobbers(a0,a2,a4,a5,a6) */
void Item_SpawnBombDrifting(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script);	/* $013732 regs(&d0,&d1,d4,d5,&d7,a4,a6,&a2) clobbers(a0,a3,a4,a5,a6) */
void Item_CollectBomb(uint16_t *damage, uint16_t *attacker, const void **owner);	/* $01375E regs(&d0,&d1,&a1) clobbers(a0) */
void Item_SpawnRapidCharge(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script);	/* $013876 regs(&d0,&d1,d4,d5,&d7,a4,a6,&a2) clobbers(a0,a3,a4,a5,a6) */
void Item_SpawnRapidChargeFromHit(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *motion, uint32_t a4, uint32_t a6);	/* $01388A regs(&d0,&d1,d4,d5,&d7,&a3,a4,a6) clobbers(a0,a2,a4,a5,a6) */
void Item_SpawnRapidChargeDrifting(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script);	/* $0138A2 regs(&d0,&d1,d4,d5,&d7,a4,a6,&a2) clobbers(a0,a3,a4,a5,a6) */
const void *Item_CollectRapidCharge(void);	/* $0138D8 regs()->a1 clobbers(a0) */
void Item_SpawnWeapon2(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script);	/* $01398A regs(&d0,&d1,d4,d5,&d7,a4,a6,&a2) clobbers(a0,a3,a4,a5,a6) */
void Item_SpawnWeaponRandom(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script);	/* $0139A4 regs(&d0,&d1,d4,d5,&d7,a4,a6,&a2) clobbers(a0,a3,a4,a5,a6) */
void Item_SpawnWeaponRandomOrDown(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script);	/* $0139BE regs(&d0,&d1,d4,d5,&d7,a4,a6,&a2) clobbers(a0,a3,a4,a5,a6) */
void Item_SpawnWeapon0FromHit(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *motion, uint32_t a4, uint32_t a6);	/* $0139F6 regs(&d0,&d1,d4,d5,&d7,&a3,a4,a6) clobbers(a0,a2,a4,a5,a6) */
void Item_SpawnWeaponRandomFromHit(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *motion, uint32_t a4, uint32_t a6);	/* $013A50 regs(&d0,&d1,d4,d5,&d7,&a3,a4,a6) clobbers(a0,a2,a4,a5,a6) */
void Item_SpawnWeaponRandomOrDownFromHit(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *motion, uint32_t a4, uint32_t a6);	/* $013A6E regs(&d0,&d1,d4,d5,&d7,&a3,a4,a6) clobbers(a0,a2,a4,a5,a6) */
void Item_SpawnWeapon0Drifting(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script);	/* $013AA6 regs(&d0,&d1,d4,d5,&d7,a4,a6,&a2) clobbers(a0,a3,a4,a5,a6) */
void Item_SpawnWeapon1Drifting(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script);	/* $013AC0 regs(&d0,&d1,d4,d5,&d7,a4,a6,&a2) clobbers(a0,a3,a4,a5,a6) */
void Item_SpawnWeapon2Drifting(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script);	/* $013ADA regs(&d0,&d1,d4,d5,&d7,a4,a6,&a2) clobbers(a0,a3,a4,a5,a6) */
void Item_SpawnWeaponRandomDrifting(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script);	/* $013AF4 regs(&d0,&d1,d4,d5,&d7,a4,a6,&a2) clobbers(a0,a3,a4,a5,a6) */
void Item_SpawnPowerUpFalling(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script);	/* $013B28 regs(&d0,&d1,d4,d5,&d7,a4,a6,&a2) clobbers(a0,a3,a4,a5,a6) */
void Item_SpawnPowerUpDrifting(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script);	/* $013B4A regs(&d0,&d1,d4,d5,&d7,a4,a6,&a2) clobbers(a0,a3,a4,a5,a6) */
void Item_WeaponItemGone(void);	/* $013C8A regs() */
void Item_CollectWeaponLevelUp(uint32_t *player_at, uint32_t *owner_at, dk::Flags *ccr);	/* $013D18 regs(&a0,&a1,&ccr) ccout(NZVC) */
void Item_WeaponItemInit(uint32_t *rand, const s16 **script, ItemObj *item);	/* $013C96 regs(&d0,&a2,a6) clobbers(a0) */
uint16_t Item_WeaponItemNextValue(ItemObj *item);	/* $013CBA regs(a6)->d0 clobbers(a0) */
uint16_t Item_WeaponItemValue(const ItemObj *item);	/* $013CD0 regs(a6)->d0 */
uint16_t Item_WeaponItemShowNext(ItemObj *item);	/* $013CD6 regs(a6)->d0 */
void Item_CollectWeapon(uint16_t *type, uint16_t *level, uint16_t *value, const void **owner, const ItemObj *item);	/* $013CE0 regs(&d0,&d1,&d2,&a1,a6) clobbers(a0) */
void Item_SpawnSmallHealth(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script);	/* $0134DE regs(&d0,&d1,d4,d5,&d7,a4,a6,&a2) clobbers(a0,a3,a4,a5,a6) */
void Item_SpawnSmallHealthFromHit(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *motion, uint32_t a4, uint32_t a6);	/* $0134F2 regs(&d0,&d1,d4,d5,&d7,&a3,a4,a6) clobbers(a0,a2,a4,a5,a6) */
void Item_SpawnSmallHealthDrifting(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script);	/* $01350A regs(&d0,&d1,d4,d5,&d7,a4,a6,&a2) clobbers(a0,a3,a4,a5,a6) */
const void *Item_CollectSmallHealth(void);	/* $013540 regs()->a1 clobbers(a0) */
void Item_SpawnWeaponDown(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script);	/* $01393C regs(&d0,&d1,d4,d5,&d7,a4,a6,&a2) clobbers(a0,a3,a4,a5,a6) */
void Item_SpawnWeapon0(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script);	/* $013956 regs(&d0,&d1,d4,d5,&d7,a4,a6,&a2) clobbers(a0,a3,a4,a5,a6) */
void Item_SpawnWeapon1(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script);	/* $013970 regs(&d0,&d1,d4,d5,&d7,a4,a6,&a2) clobbers(a0,a3,a4,a5,a6) */
void Item_SpawnWeaponDownFromHit(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *motion, uint32_t a4, uint32_t a6);	/* $0139D8 regs(&d0,&d1,d4,d5,&d7,&a3,a4,a6) clobbers(a0,a2,a4,a5,a6) */
void Item_SpawnWeapon1FromHit(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *motion, uint32_t a4, uint32_t a6);	/* $013A14 regs(&d0,&d1,d4,d5,&d7,&a3,a4,a6) clobbers(a0,a2,a4,a5,a6) */
void Item_SpawnWeapon2FromHit(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t *motion, uint32_t a4, uint32_t a6);	/* $013A32 regs(&d0,&d1,d4,d5,&d7,&a3,a4,a6) clobbers(a0,a2,a4,a5,a6) */
void Item_SpawnWeaponDownDrifting(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script);	/* $013A8C regs(&d0,&d1,d4,d5,&d7,a4,a6,&a2) clobbers(a0,a3,a4,a5,a6) */
void Item_SpawnWeaponRandomOrDownDrifting(uint32_t *result, uint16_t *d1, uint16_t x, uint16_t y, uint32_t *d7, uint32_t a4, uint32_t a6, uint32_t *script);	/* $013B0E regs(&d0,&d1,d4,d5,&d7,a4,a6,&a2) clobbers(a0,a3,a4,a5,a6) */
