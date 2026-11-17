; ---------------------------------------------------------------------------
; VM6_StartFromTrigger - stage trigger callback that starts the a6 loader script given by the trigger entry's argument long (loads the next section's graphics while the stage scrolls)
; In:  a1 -> long script address (the trigger entry's argument, after the callback long)
; Out: falls into VM6_Start: d0 = 0 (trigger done)
; Evidence: dc.l $7092 in the trigger tables at $21D8C (script $33050: `70E0 0000 772E 2300` sets section $2300, then DMA ops) and $21D98 (script $3308A), $35598; Trigger_Fire calls the callback with a1 past the long; TAS f46235-97872 (3 frames), 1 run
; ---------------------------------------------------------------------------
VM6_StartFromTrigger:
	movea.l	(a1)+,a6
