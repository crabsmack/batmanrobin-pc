; ---------------------------------------------------------------------------
; Op6_RedrawLayerA - a6 op: redraws a full screen of map layer g_LayerA one block column per frame through the tile queue, parking the script until done
; In:  no operands
; Out: parks (VM6_StartRedraw with a0 = g_LayerA)
; Evidence: 2 script uses ($330C4, $331E6 `73EE`, after Op6_MoveLayerAToCamera); the raw $73F6/$73FE twins use the layer at $EDE8 and the direct Layer_DrawScreen path $7406; TAS f67239
; ---------------------------------------------------------------------------
Op6_RedrawLayerA:
	lea.l	(g_LayerA).l,a0
	bra.s	VM6_StartRedraw
