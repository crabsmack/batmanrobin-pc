; ---------------------------------------------------------------------------
; Enemy_GasOrb_DeathReleaseFrame - native fragment at the end of the gas orb's death branch: releases the jet-puff sprite frame held in $2A(a6) (Spr_ReleaseFrame), then resumes the script at Op2_Kill ($041ED0)
; In:  a6 = orb object (+$2A puff frame), a2 = script
; Out: re-enters the script with jsr $7FE (Op2_Kill)
; Evidence: word $07FC at $41EC0 after Op2_AwardScoreToHitter and the explosion Fx_SpawnAtOfs $14CAC in Scr2_041EA4, reached by Op2_TakeHit's death offset at $41E86; $2A(a6) is the frame Enemy_GasOrb_StepJet swaps with Spr_ChangeFrame; TAS 18 frames 124216-130141 (orb kills)
; ---------------------------------------------------------------------------
Enemy_GasOrb_DeathReleaseFrame:
	move.w	$2A(a6),d2
	jsr	(Spr_ReleaseFrame).l
	jsr	($7FE).w
