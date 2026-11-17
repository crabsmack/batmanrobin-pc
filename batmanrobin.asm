; ===========================================================================
; The Adventures of Batman and Robin - Sega Genesis / Mega Drive
; Byte-matching disassembly. Build ONLY with: python build.py
; ROM SHA-1 1da4b3fc92a6572e70d748ebad346aeb291f89f3 (supply your own dump; never commit it)
; ===========================================================================
	cpu 68000
	padding off		; don't pad dc.b
	supmode on		; no privileged-instruction warnings
	listing purecode
	page 0
	include "src/macros.asm"
	include "src/ram.asm"

	org 0
	include "src/header.asm"
Scr2_000200:	; a2-VM script by its format: 32 of 128 words are a2 handler entry addresses; never run in the corpus
	incrom	$000200,$100
	include "src/f/000300.asm"
Tbl_00038E:	; read by Boot_Reset (pc $000314)
	incrom	$00038E,$6C
	include "src/f/0003FA.asm"
	include "src/f/000500.asm"
Tbl_RegionByVersion:	; region letter per console version (bits 7-6 of $A10001: J, 0, U, E), indexed by the boot region check (Boot_CheckRegion, pc $000412)
	incrom	$000536,$4
Tbl_RegionLockout:	; region-lockout table walked by the boot region check (Boot_CheckRegion: lea $53A(pc),a2 / adda.l (a2)+,a0) when the console region is not in the header's region string; never read in the corpus
	incrom	$00053A,$14
Str_LockoutDeveloped:	; region-lockout line: x=$06, "DEVELOPED FOR USE ONLY WITH", 0-terminated; never shown in the corpus
	incrom	$00054E,$1D
Str_LockoutAmp:	; region-lockout line: x=$12, "&", 0-terminated (between two console names)
	incrom	$00056B,$3
Str_LockoutSystems:	; region-lockout line: x=$0F, "SYSTEMS.", 0-terminated
	incrom	$00056E,$A
Str_LockoutNtscMegaDrive:	; region-lockout console name: x=$0C, "NTSC MEGA DRIVE", 0-terminated
	incrom	$000578,$11
Str_LockoutNtscGenesis:	; region-lockout console name: x=$0D, "NTSC GENESIS", 0-terminated
	incrom	$000589,$E
Str_LockoutPalSecam:	; region-lockout console name: x=$04, "PAL AND FRENCH SECAM MEGA DRIVE", 0-terminated
	incrom	$000597,$21
Art_LockoutFont:	; 1bpp 8x8 font for the region-lockout message, 59 glyphs from " " ($20: 8 zero bytes) on; drawn by Boot_DrawLockoutText (inferred from the boot code); never read in the corpus
	incrom	$0005B8,$1D8
	include "src/f/000790.asm"
	include "src/f/0007D2.asm"
	include "src/f/0007E4.asm"
	incrom	$0007EA,$12
	include "src/f/0007FC.asm"
	include "src/f/000980.asm"
	include "src/f/0009C4.asm"
	incrom	$000ABE,$E
	include "src/f/000ACC.asm"
	incrom	$000B52,$C
	include "src/f/000B5E.asm"
	incrom	$000BA6,$2A
	include "src/f/000BD0.asm"
	incrom	$000C20,$E
	include "src/f/000C2E.asm"
	incrom	$000C46,$18
	include "src/f/000C5E.asm"
	incrom	$000C76,$18
	include "src/f/000C8E.asm"
	incrom	$000CF6,$30
	include "src/f/000D26.asm"
	incrom	$000D34,$E
	include "src/f/000D42.asm"
	incrom	$000D5E,$8A
	include "src/f/000DE8.asm"
	incrom	$000DF6,$48
	include "src/f/000E3E.asm"
	incrom	$000F0E,$20
	include "src/f/000F2E.asm"
	incrom	$000F44,$3C
	include "src/f/000F80.asm"
	incrom	$000F88,$12
	include "src/f/000F9A.asm"
	incrom	$000FB8,$48
	include "src/f/001000.asm"
	incrom	$00101E,$3C
	include "src/f/00105A.asm"
	incrom	$001092,$50
	include "src/f/0010E2.asm"
	incrom	$0010F8,$3C
	include "src/f/001134.asm"
	incrom	$00114E,$8
	include "src/f/001156.asm"
	incrom	$00116C,$48
	include "src/f/0011B4.asm"
	incrom	$0011D2,$3C
	include "src/f/00120E.asm"
	incrom	$001236,$A
	include "src/f/001240.asm"
	incrom	$00126E,$14
	include "src/f/001282.asm"
	incrom	$0012D6,$E
	include "src/f/0012E4.asm"
	incrom	$00134A,$1A
	include "src/f/001364.asm"
	include "src/f/0013CA.asm"
	incrom	$001406,$4A
	include "src/f/001450.asm"
	incrom	$00149C,$54
	include "src/f/0014F0.asm"
	include "src/f/00156A.asm"
	incrom	$0016EE,$1A
	include "src/f/001708.asm"
	incrom	$00172C,$A
	include "src/f/001736.asm"
	incrom	$0017E2,$8
	include "src/f/0017EA.asm"
	incrom	$001846,$14
	include "src/f/00185A.asm"
	include "src/f/001872.asm"
	include "src/f/001884.asm"
	incrom	$001920,$2
	include "src/f/001922.asm"
	incrom	$00194C,$2
	include "src/f/00194E.asm"
	incrom	$0019C0,$2C
	include "src/f/0019EC.asm"
	incrom	$0019F0,$2
	include "src/f/0019F2.asm"
	incrom	$001A36,$26
	include "src/f/001A5C.asm"
	incrom	$001A60,$2
	include "src/f/001A62.asm"
	incrom	$001A8C,$2
	include "src/f/001A8E.asm"
	incrom	$001B00,$2C
	include "src/f/001B2C.asm"
	incrom	$001B30,$2
	include "src/f/001B32.asm"
	incrom	$001B76,$22
	include "src/f/001B98.asm"
	incrom	$001BCC,$E
	include "src/f/001BDA.asm"
	incrom	$001BE2,$A
	include "src/f/001BEC.asm"
	incrom	$001C10,$2C
	include "src/f/001C3C.asm"
	include "src/f/001D7E.asm"
	include "src/f/001D82.asm"
	include "src/f/001DBC.asm"
	incrom	$001E30,$36
	include "src/f/001E66.asm"
	incrom	$001E9E,$E
	include "src/f/001EAC.asm"
	incrom	$001EBC,$2E
	include "src/f/001EEA.asm"
	incrom	$001F08,$30
	include "src/f/001F38.asm"
	incrom	$001F46,$20
	include "src/f/001F66.asm"
	incrom	$001F84,$30
	include "src/f/001FB4.asm"
	incrom	$001FF8,$A
	include "src/f/002002.asm"
	incrom	$002012,$E
	include "src/f/002020.asm"
	incrom	$00205E,$30
	include "src/f/00208E.asm"
	incrom	$0020DC,$16
	include "src/f/0020F2.asm"
	incrom	$002130,$20
	include "src/f/002150.asm"
	incrom	$002160,$20
	include "src/f/002180.asm"
	incrom	$002192,$40
	include "src/f/0021D2.asm"
	incrom	$0021E2,$50
	include "src/f/002232.asm"
	incrom	$00224A,$8
	include "src/f/002252.asm"
	incrom	$00226E,$AE
	include "src/f/00231C.asm"
	incrom	$002328,$C
	include "src/f/002334.asm"
	incrom	$002350,$A
	include "src/f/00235A.asm"
	incrom	$002370,$C
	include "src/f/00237C.asm"
	incrom	$002390,$12
	include "src/f/0023A2.asm"
	incrom	$0023B8,$C
	include "src/f/0023C4.asm"
	incrom	$0023D8,$8
	include "src/f/0023E0.asm"
	incrom	$002400,$C
	include "src/f/00240C.asm"
	incrom	$002420,$12
	include "src/f/002432.asm"
	incrom	$00243C,$E
	include "src/f/00244A.asm"
	incrom	$002476,$10
	include "src/f/002486.asm"
	include "src/f/002550.asm"
	incrom	$0025E4,$98
	include "src/f/00267C.asm"
	incrom	$00269A,$10
	include "src/f/0026AA.asm"
	incrom	$0026B2,$3F4
	include "src/f/002AA6.asm"
	incrom	$002AB6,$34
	include "src/f/002AEA.asm"
	incrom	$002AFA,$1C
	include "src/f/002B16.asm"
	incrom	$002B26,$60
	include "src/f/002B86.asm"
	incrom	$002B96,$60
	include "src/f/002BF6.asm"
	incrom	$002C06,$34
	include "src/f/002C3A.asm"
	incrom	$002C6A,$12
	include "src/f/002C7C.asm"
	incrom	$002C92,$12
	include "src/f/002CA4.asm"
	incrom	$002CD6,$10
	include "src/f/002CE6.asm"
Scr2_002D28:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$002D28,$2
	include "src/f/002D2A.asm"
	incrom	$002F36,$46
	include "src/f/002F7C.asm"
	incrom	$002F9C,$32
	include "src/f/002FCE.asm"
	incrom	$002FE0,$16
	include "src/f/002FF6.asm"
	incrom	$0031C6,$17C
	include "src/f/003342.asm"
	incrom	$003402,$32
	include "src/f/003434.asm"
	incrom	$0034A2,$4
	include "src/f/0034A6.asm"
	incrom	$003574,$1A0
	include "src/f/003714.asm"
	incrom	$003756,$4
	include "src/f/00375A.asm"
	include "src/f/003798.asm"
	include "src/f/00385A.asm"
	incrom	$00390C,$1C
	include "src/f/003928.asm"
	incrom	$003944,$1C
	include "src/f/003960.asm"
	include "src/f/00398A.asm"
	incrom	$003A60,$14
	include "src/f/003A74.asm"
	incrom	$003AA6,$18
	include "src/f/003ABE.asm"
	incrom	$003ACE,$10
	include "src/f/003ADE.asm"
	incrom	$003B3C,$14
	include "src/f/003B50.asm"
	incrom	$003B5A,$1C
	include "src/f/003B76.asm"
	incrom	$003C8C,$3E
	include "src/f/003CCA.asm"
	incrom	$003CDC,$12
	include "src/f/003CEE.asm"
	incrom	$003CF4,$64
	include "src/f/003D58.asm"
	incrom	$003D64,$F4
	include "src/f/003E58.asm"
	incrom	$003F46,$26
	include "src/f/003F6C.asm"
	incrom	$003FAE,$36
	include "src/f/003FE4.asm"
	incrom	$003FF6,$8
	include "src/f/003FFE.asm"
	incrom	$004006,$92
	include "src/f/004098.asm"
	incrom	$0040A0,$92
	include "src/f/004132.asm"
	incrom	$00413A,$1C4
	include "src/f/0042FE.asm"
	incrom	$004318,$E
	include "src/f/004326.asm"
	incrom	$004330,$E
	include "src/f/00433E.asm"
	incrom	$00436C,$E
	include "src/f/00437A.asm"
	incrom	$0043B6,$2C
	include "src/f/0043E2.asm"
	incrom	$00442C,$28
	include "src/f/004454.asm"
	incrom	$004644,$88
	include "src/f/0046CC.asm"
	incrom	$00474A,$42
	include "src/f/00478C.asm"
	incrom	$00486E,$2A
	include "src/f/004898.asm"
	incrom	$004926,$18E
	include "src/f/004AB4.asm"
	incrom	$004AD2,$A2
	include "src/f/004B74.asm"
	incrom	$004C5E,$7C
	include "src/f/004CDA.asm"
	include "src/f/004DEC.asm"
	include "src/f/004DF4.asm"
	incrom	$004E2E,$4
	include "src/f/004E32.asm"
	incrom	$004E4A,$8
	include "src/f/004E52.asm"
	incrom	$004E7E,$2A
	include "src/f/004EA8.asm"
	incrom	$004F42,$4
	include "src/f/004F46.asm"
	incrom	$004F4A,$4
	include "src/f/004F4E.asm"
	include "src/f/005078.asm"
	include "src/f/0050A2.asm"
	incrom	$0050BA,$14
	include "src/f/0050CE.asm"
	incrom	$0051D6,$1C
	include "src/f/0051F2.asm"
	incrom	$0052D0,$20
	include "src/f/0052F0.asm"
	incrom	$0052FA,$C
	include "src/f/005306.asm"
	incrom	$005370,$8
	include "src/f/005378.asm"
	incrom	$005396,$1E
	include "src/f/0053B4.asm"
	include "src/f/0053BA.asm"
	incrom	$0053CC,$8
	include "src/f/0053D4.asm"
	incrom	$005440,$10
	include "src/f/005450.asm"
	incrom	$0054F0,$30
	include "src/f/005520.asm"
	incrom	$005538,$22
	include "src/f/00555A.asm"
	incrom	$005586,$3C
	include "src/f/0055C2.asm"
	incrom	$0055D8,$2E
	include "src/f/005606.asm"
	incrom	$0056D0,$16
	include "src/f/0056E6.asm"
	incrom	$005716,$C
	include "src/f/005722.asm"
	incrom	$005746,$A
	include "src/f/005750.asm"
	incrom	$005760,$A
	include "src/f/00576A.asm"
	incrom	$005774,$18
	include "src/f/00578C.asm"
	incrom	$0057A8,$18
	include "src/f/0057C0.asm"
	incrom	$00582C,$10
	include "src/f/00583C.asm"
	incrom	$005884,$1C
	include "src/f/0058A0.asm"
	incrom	$0058AE,$1C
	include "src/f/0058CA.asm"
	include "src/f/0058D4.asm"
	include "src/f/0058DA.asm"
	incrom	$0058E4,$A
	include "src/f/0058EE.asm"
	incrom	$00595A,$E
	include "src/f/005968.asm"
	incrom	$005976,$1A
	include "src/f/005990.asm"
	incrom	$0059A8,$42
	include "src/f/0059EA.asm"
	incrom	$0059F8,$1A
	include "src/f/005A12.asm"
	incrom	$005A42,$12
	include "src/f/005A54.asm"
	incrom	$005A6E,$12
	include "src/f/005A80.asm"
	incrom	$005ADC,$1E
	include "src/f/005AFA.asm"
	incrom	$005B26,$5C
	include "src/f/005B82.asm"
	incrom	$005B94,$30
	include "src/f/005BC4.asm"
	incrom	$005BF0,$30
	include "src/f/005C20.asm"
	incrom	$005C3A,$42
	include "src/f/005C7C.asm"
	incrom	$005C96,$5C
	include "src/f/005CF2.asm"
	incrom	$005D00,$1A
	include "src/f/005D1A.asm"
	incrom	$005D32,$42
	include "src/f/005D74.asm"
	incrom	$005DCC,$12
	include "src/f/005DDE.asm"
	incrom	$005DF8,$30
	include "src/f/005E28.asm"
	incrom	$005E54,$12
	include "src/f/005E66.asm"
	incrom	$005E84,$88
	include "src/f/005F0C.asm"
	incrom	$005F3C,$12
	include "src/f/005F4E.asm"
	incrom	$005F7A,$1E
	include "src/f/005F98.asm"
	include "src/f/005FAA.asm"
	incrom	$005FC4,$B8
	include "src/f/00607C.asm"
	incrom	$00608E,$10
	include "src/f/00609E.asm"
	incrom	$0060BC,$6
	include "src/f/0060C2.asm"
	include "src/f/0060C4.asm"
	include "src/f/00613A.asm"
	incrom	$006208,$30
	include "src/f/006238.asm"
	include "src/f/00624C.asm"
	incrom	$0063AC,$A
	include "src/f/0063B6.asm"
	incrom	$0063D2,$10
	include "src/f/0063E2.asm"
	incrom	$006460,$E
	include "src/f/00646E.asm"
	incrom	$006542,$34
	include "src/f/006576.asm"
	incrom	$0065A4,$10
	include "src/f/0065B4.asm"
	incrom	$00664E,$42
	include "src/f/006690.asm"
	incrom	$0066AC,$36
	include "src/f/0066E2.asm"
	incrom	$00676C,$A
	include "src/f/006776.asm"
	incrom	$006950,$2
	include "src/f/006952.asm"
	incrom	$006A5A,$E
	include "src/f/006A68.asm"
	incrom	$006AA8,$1C
	include "src/f/006AC4.asm"
	incrom	$006AE6,$17C
	include "src/f/006C62.asm"
	incrom	$006C8C,$24
	include "src/f/006CB0.asm"
	incrom	$006CC2,$4
	include "src/f/006CC6.asm"
	incrom	$006CCE,$4
	include "src/f/006CD2.asm"
	incrom	$006CD6,$C
	include "src/f/006CE2.asm"
	incrom	$006CEC,$24
	include "src/f/006D10.asm"
	incrom	$006D5A,$62
	include "src/f/006DBC.asm"
	incrom	$006DEC,$8
	include "src/f/006DF4.asm"
	incrom	$006E08,$10
	include "src/f/006E18.asm"
	incrom	$006E3E,$36
	include "src/f/006E74.asm"
	incrom	$006E88,$12
	include "src/f/006E9A.asm"
	incrom	$006EB2,$8
	include "src/f/006EBA.asm"
	include "src/f/006F56.asm"
	include "src/f/006F9A.asm"
	include "src/f/006FEA.asm"
	include "src/f/007086.asm"
	include "src/f/007092.asm"
	include "src/f/007094.asm"
	include "src/f/0070A4.asm"
	include "src/f/0070A8.asm"
	incrom	$0070D2,$E
	include "src/f/0070E0.asm"
	include "src/f/007186.asm"
	incrom	$0071E8,$1A
	include "src/f/007202.asm"
	incrom	$00721E,$1C
	include "src/f/00723A.asm"
	include "src/f/007256.asm"
	incrom	$007296,$3C
	include "src/f/0072D2.asm"
	incrom	$007366,$16
	include "src/f/00737C.asm"
	incrom	$0073A6,$48
	include "src/f/0073EE.asm"
	incrom	$0073F6,$20
	include "src/f/007416.asm"
	include "src/f/0074A8.asm"
	incrom	$007734,$2
	include "src/f/007736.asm"
	include "src/f/007836.asm"
	include "src/f/00784A.asm"
	include "src/f/00785A.asm"
	include "src/f/00786A.asm"
	include "src/f/0078B2.asm"
	include "src/f/00799C.asm"
	include "src/f/0079C0.asm"
	include "src/f/007A08.asm"
	include "src/f/007A4A.asm"
	include "src/f/007B1C.asm"
	include "src/f/007B50.asm"
	include "src/f/007B72.asm"
	include "src/f/007D5E.asm"
	include "src/f/007D76.asm"
	include "src/f/007EB8.asm"
	include "src/f/007EF6.asm"
	include "src/f/007F5E.asm"
	include "src/f/008000.asm"
	include "src/f/008042.asm"
Tbl_0080D4:	; read by Spr_QueueArtDMA (pc $0080A6), Spr_QueueArtDMA (pc $0080CA), Spr_QueueArtDMA (pc $00809A); 28% read in the corpus
	incrom	$0080D4,$440
	include "src/f/008514.asm"
	include "src/f/008580.asm"
	incrom	$008584,$2
	include "src/f/008586.asm"
	include "src/f/0085F8.asm"
	include "src/f/0086AA.asm"
	include "src/f/0086E6.asm"
	include "src/f/0086FE.asm"
	include "src/f/008718.asm"
	include "src/f/008764.asm"
	include "src/f/0087B6.asm"
	include "src/f/0088BE.asm"
	include "src/f/0088C2.asm"
	include "src/f/0088DA.asm"
	incrom	$0088F6,$14
	include "src/f/00890A.asm"
	include "src/f/008912.asm"
	incrom	$008948,$4
	include "src/f/00894C.asm"
	include "src/f/00896C.asm"
	include "src/f/008972.asm"
	include "src/f/00899A.asm"
	include "src/f/0089CA.asm"
	include "src/f/008A08.asm"
	include "src/f/008A48.asm"
	include "src/f/008ABE.asm"
	include "src/f/008ADA.asm"
	include "src/f/008B22.asm"
	include "src/f/008B26.asm"
	incrom	$008B3E,$40
	include "src/f/008B7E.asm"
	include "src/f/008BB8.asm"
	include "src/f/008BF6.asm"
	include "src/f/008C1A.asm"
	include "src/f/008C3C.asm"
	include "src/f/008C54.asm"
	include "src/f/008C6C.asm"
	include "src/f/008C80.asm"
	include "src/f/008C8A.asm"
	include "src/f/008C94.asm"
	include "src/f/008CB6.asm"
	incrom	$008CC0,$A
	include "src/f/008CCA.asm"
	include "src/f/008DB6.asm"
	include "src/f/008DD0.asm"
	include "src/f/008DD2.asm"
	include "src/f/008E22.asm"
	incrom	$008E3C,$1C
	include "src/f/008E58.asm"
	include "src/f/008E72.asm"
	incrom	$008EA8,$52
	include "src/f/008EFA.asm"
	include "src/f/008F08.asm"
	include "src/f/008F0E.asm"
	include "src/f/008F1E.asm"
	include "src/f/008F24.asm"
	incrom	$008F36,$6
	include "src/f/008F3C.asm"
	include "src/f/008F4E.asm"
	include "src/f/008FFE.asm"
	incrom	$009030,$2E
	include "src/f/00905E.asm"
	incrom	$00907C,$2E
	include "src/f/0090AA.asm"
	incrom	$0090B0,$18
	include "src/f/0090C8.asm"
	incrom	$0090FC,$60
	include "src/f/00915C.asm"
	incrom	$0091C8,$28
	include "src/f/0091F0.asm"
	include "src/f/0091F2.asm"
	include "src/f/00923C.asm"
	include "src/f/00924A.asm"
	include "src/f/009266.asm"
	include "src/f/00932E.asm"
	incrom	$00939E,$2
	include "src/f/0093A0.asm"
	include "src/f/0093D2.asm"
	include "src/f/0093DC.asm"
	include "src/f/00943E.asm"
	incrom	$00957C,$8
	include "src/f/009584.asm"
	incrom	$009590,$C
	include "src/f/00959C.asm"
	include "src/f/0095A2.asm"
	include "src/f/0095EC.asm"
	include "src/f/009642.asm"
	include "src/f/0096B6.asm"
	incrom	$0096CA,$14
	include "src/f/0096DE.asm"
Tbl_00970A:	; read by Scroll_FillBands (pc $009930), ScrollCmd_LayerA (pc $009826), Scroll_NextCmd (pc $0097F6); referenced by 01FC50
	incrom	$00970A,$22
Tbl_00972C:	; read by Scroll_FillBands (pc $009930), Scroll_NextCmd (pc $0097F6), ScrollCmd_LayerA (pc $009826); referenced by 01FD76
	incrom	$00972C,$22
Tbl_00974E:	; read by Scroll_FillBands (pc $009930), Scroll_NextCmd (pc $0097F6), ScrollCmd_LayerA (pc $009826); referenced by 03FE94
	incrom	$00974E,$1E
Tbl_00976C:	; read by Scroll_FillBands (pc $009930), Scroll_NextCmd (pc $0097F6), ScrollCmd_LayerA (pc $009826); referenced by 0400E4
	incrom	$00976C,$22
Tbl_00978E:	; read by Scroll_FillBands (pc $009930), Scroll_NextCmd (pc $0097F6), ScrollCmd_CameraB (pc $00988A); referenced by 0403F6
	incrom	$00978E,$1C
Tbl_0097AA:	; read by Scroll_FillBands (pc $009930), Scroll_NextCmd (pc $0097F6), ScrollCmd_CameraB (pc $00988A); 20% read in the corpus; referenced by 0475C2
	incrom	$0097AA,$46
	include "src/f/0097F0.asm"
	include "src/f/0097F6.asm"
	incrom	$0097FE,$2
	include "src/f/009800.asm"
	incrom	$009804,$4
	include "src/f/009808.asm"
	incrom	$00980C,$4
	include "src/f/009810.asm"
	incrom	$009818,$8
	include "src/f/009820.asm"
	incrom	$009834,$14
	include "src/f/009848.asm"
	incrom	$009864,$20
	include "src/f/009884.asm"
	incrom	$0098C2,$3C
	include "src/f/0098FE.asm"
	include "src/f/009928.asm"
	include "src/f/00995A.asm"
	include "src/f/009982.asm"
	include "src/f/00998E.asm"
	include "src/f/00999E.asm"
	include "src/f/0099D4.asm"
	include "src/f/009AA4.asm"
	include "src/f/009AAC.asm"
	include "src/f/009B86.asm"
	include "src/f/009BAC.asm"
	include "src/f/009BC0.asm"
	include "src/f/009BF0.asm"
	include "src/f/009C50.asm"
Tbl_009CDC:	; read by Pause_CheatWaitPress (pc $009CB8); referenced by 009C50
	incrom	$009CDC,$A
	include "src/f/009CE6.asm"
	include "src/f/009D0E.asm"
	include "src/f/009D24.asm"
	include "src/f/009DC6.asm"
	include "src/f/009DE4.asm"
	include "src/f/009E02.asm"
	include "src/f/009F74.asm"
	include "src/f/009FBC.asm"
	include "src/f/009FE6.asm"
Tbl_009FFC:	; read by Section_RunNext (pc $009FC0); referenced by 009E02
	incrom	$009FFC,$60
Tbl_00A05C:	; read by Section_RunNext (pc $009FC0); 25% read in the corpus; referenced by 009E02
	incrom	$00A05C,$50
Tbl_00A0AC:	; read by Section_RunNext (pc $009FD6), Scene_RunList (pc $009FE6); referenced by 009FBC
	incrom	$00A0AC,$8A
Tbl_00A136:	; read by Scene_RunList (pc $009FE6), Section_RunNext (pc $009FD6); referenced by 009E02
	incrom	$00A136,$1C
Tbl_00A152:	; read by Scene_RunList (pc $009FE6); referenced by 009E02
	incrom	$00A152,$14
Tbl_00A166:	; read by Scene_RunList (pc $009FE6); referenced by 009F74
	incrom	$00A166,$C
Tbl_00A172:	; read by Scene_RunList (pc $009FE6); referenced by 009F74
	incrom	$00A172,$C
Tbl_00A17E:	; read by Scene_RunList (pc $009FE6); referenced by 009F74
	incrom	$00A17E,$C
Tbl_00A18A:	; read by Scene_RunList (pc $009FE6); referenced by 009F74
	incrom	$00A18A,$C
	include "src/f/00A196.asm"
	include "src/f/00A1BE.asm"
	include "src/f/00A1F0.asm"
	include "src/f/00A20C.asm"
	include "src/f/00A228.asm"
	include "src/f/00A23E.asm"
	include "src/f/00A3AE.asm"
	include "src/f/00A3D4.asm"
	include "src/f/00A3E8.asm"
	include "src/f/00A400.asm"
	include "src/f/00A43C.asm"
Tbl_00A670:	; read by Fade_Step (pc $00A502), Fade_Step (pc $00A57A), Fade_Step (pc $00A63E); 32% read in the corpus
	incrom	$00A670,$38
Tbl_00A6A8:	; read by Fade_Step (pc $00A502), Fade_Step (pc $00A506), Fade_Step (pc $00A57A); 47% read in the corpus; referenced by 00A43C, 00AD26
	incrom	$00A6A8,$80
Tbl_00A728:	; read by Fade_Step (pc $00A506), Fade_Step (pc $00A50A), Fade_Step (pc $00A582); 45% read in the corpus; referenced by 00A43C, 00AD26
	incrom	$00A728,$80
Tbl_00A7A8:	; read by Fade_Step (pc $00A50A), Fade_Step (pc $00A582), Fade_Step (pc $00A646); 58% read in the corpus; referenced by 00A43C, 00AD26
	incrom	$00A7A8,$48
	include "src/f/00A7F0.asm"
Tbl_00A87E:	; read by Pal_VIntUpload (pc $00A862); 43% read in the corpus
	incrom	$00A87E,$178
	include "src/f/00A9F6.asm"
	include "src/f/00AA1A.asm"
	include "src/f/00AA2A.asm"
	include "src/f/00AA4C.asm"
	incrom	$00AA6E,$C
	include "src/f/00AA7A.asm"
	include "src/f/00AA9C.asm"
	incrom	$00AB02,$22
	include "src/f/00AB24.asm"
	incrom	$00AB3C,$30
	include "src/f/00AB6C.asm"
	include "src/f/00AB92.asm"
	incrom	$00ABB8,$12
	include "src/f/00ABCA.asm"
	include "src/f/00ABE6.asm"
	include "src/f/00AC02.asm"
	include "src/f/00AC1E.asm"
	incrom	$00AC3A,$2C
	include "src/f/00AC66.asm"
	incrom	$00AD02,$12
	include "src/f/00AD14.asm"
	include "src/f/00AD26.asm"
	include "src/f/00AD84.asm"
	include "src/f/00ADA6.asm"
	include "src/f/00ADBA.asm"
	include "src/f/00ADD4.asm"
	include "src/f/00ADE8.asm"
	include "src/f/00ADFC.asm"
	include "src/f/00AE10.asm"
	include "src/f/00AE2C.asm"
	include "src/f/00AE44.asm"
	incrom	$00AE5C,$18
	include "src/f/00AE74.asm"
	include "src/f/00AE7C.asm"
	incrom	$00AE90,$14
	include "src/f/00AEA4.asm"
	include "src/f/00AEBC.asm"
	include "src/f/00AEC4.asm"
	incrom	$00AEC8,$4
	include "src/f/00AECC.asm"
	incrom	$00AED8,$14
	include "src/f/00AEEC.asm"
	incrom	$00AEF4,$4
	include "src/f/00AEF8.asm"
	incrom	$00AF00,$4
	include "src/f/00AF04.asm"
	incrom	$00AF56,$2E
	include "src/f/00AF84.asm"
	incrom	$00AFD6,$4
	include "src/f/00AFDA.asm"
	include "src/f/00AFDE.asm"
	include "src/f/00AFFC.asm"
	include "src/f/00B03E.asm"
	include "src/f/00B064.asm"
	include "src/f/00B08A.asm"
	include "src/f/00B0A8.asm"
	incrom	$00B0EA,$BA
	include "src/f/00B1A4.asm"
Tbl_HIntHandlerSrc:	; the RAM H-int handler source: HInt_Install (pc $00B1C4) copies $4C bytes from here to g_HIntCode; the handler is $B20C-$B231 (move.w (xxxx).w,$C00000, step the operand, self-patching bcs, H-int off tail)
	incrom	$00B20C,$26
	include "src/f/00B232.asm"
	include "src/f/00C908.asm"
	incrom	$00C9FA,$180
	include "src/f/00CB7A.asm"
Scr2_00CD86:	; a2-VM script by its format: 224 of 683 words are a2 handler entry addresses; never run in the corpus
	incrom	$00CD86,$556
	include "src/f/00D2DC.asm"
	include "src/f/00D81E.asm"
	incrom	$00D82C,$288
	include "src/f/00DAB4.asm"
	incrom	$00E81A,$288
	include "src/f/00EAA2.asm"
	include "src/f/00F808.asm"
Art_00F86A:	; DMA source to VRAM
	incrom	$00F86A,$100
	include "src/f/00F96A.asm"
	include "src/f/00FE0E.asm"
	incrom	$00FFC2,$148
	include "src/f/01010A.asm"
	include "src/f/010122.asm"
	incrom	$01015C,$4C
	include "src/f/0101A8.asm"
	include "src/f/0101CE.asm"
	include "src/f/0102D6.asm"
Tbl_WeaponChargeRate:	; 3 weapon types x 8 words: weapon charge added per frame by Player_Run (pc $0103C4): entry 0 = $280 while the record +$26 countdown runs, else entry level+1 ($80-$B8)
	incrom	$0103CE,$30
	include "src/f/0103FE.asm"
	include "src/f/01048E.asm"
	include "src/f/01054A.asm"
	include "src/f/010580.asm"
	include "src/f/0105AE.asm"
	incrom	$010692,$16
	include "src/f/0106A8.asm"
	incrom	$010724,$2
	include "src/f/010726.asm"
	include "src/f/010742.asm"
	incrom	$010782,$2E
	include "src/f/0107B0.asm"
	include "src/f/0107DE.asm"
	include "src/f/0107E8.asm"
	include "src/f/01088A.asm"
	include "src/f/0108A6.asm"
Scr4_0108CE:	; a4-VM script: read by the a4 VM's handlers through a4; referenced by 0107E8
	incrom	$0108CE,$26
Scr4_0108F4:	; a4-VM script: read by the a4 VM's handlers through a4; referenced by 0107E8
	incrom	$0108F4,$26
Scr4_01091A:	; a4-VM script: read by the a4 VM's handlers through a4; referenced by 010742
	incrom	$01091A,$26
Scr4_010940:	; a4-VM script: read by the a4 VM's handlers through a4; referenced by 010742
	incrom	$010940,$24
Scr4_010964:	; a4-VM script: read by the a4 VM's handlers through a4; referenced by 0105AE, 0106A8, 041F0E
	incrom	$010964,$26
	include "src/f/01098A.asm"
	include "src/f/0109A0.asm"
Tbl_010A1E:	; addressed as data by code (operand in 0109A0); never read in the corpus
	incrom	$010A1E,$60
	include "src/f/010A7E.asm"
Tbl_010AF8:	; addressed as data by code (operand in 010A7E); never read in the corpus
	incrom	$010AF8,$180
	include "src/f/010C78.asm"
	incrom	$010CE0,$32
	include "src/f/010D12.asm"
Scr4_010D9A:	; a4-VM script: read by the a4 VM's handlers through a4; referenced by 010D12
	incrom	$010D9A,$12
Scr4_010DAC:	; a4-VM script: read by the a4 VM's handlers through a4; referenced by 010D12
	incrom	$010DAC,$1E
	include "src/f/010DCA.asm"
	include "src/f/010E0A.asm"
Tbl_010F38:	; read by HUD_DrawPanelText (pc $011716); 97% read in the corpus
	incrom	$010F38,$3E
Tbl_010F76:	; read by HUD_RunPanelScript (pc $01175C), HudOp_ShowCredits (pc $0117EA), HudOp_Wait (pc $011796)
	incrom	$010F76,$E
Tbl_010F84:	; read by HUD_RunPanelScript (pc $01175C), HudOp_ShowText (pc $0117D0), HudOp_Goto (pc $0117A0)
	incrom	$010F84,$6
Tbl_010F8A:	; read by HUD_RunPanelScript (pc $01175C), HudOp_ShowText (pc $0117D0), HudOp_Goto (pc $0117A0)
	incrom	$010F8A,$28
	incrom	$010FB2,$20
Tbl_010FD2:	; read by HUD_RunPanelScript (pc $01175C), HudOp_ShowText (pc $0117D0)
	incrom	$010FD2,$6
Tbl_010FD8:	; read by HUD_RunPanelScript (pc $01175C), HudOp_ShowText (pc $0117D0); 75% read in the corpus
	incrom	$010FD8,$8
Tbl_010FE0:	; read by HudOp_ShowText (pc $0117D0), HUD_RunPanelScript (pc $01175C), HudOp_Goto (pc $0117A0); 45% read in the corpus
	incrom	$010FE0,$16
Tbl_010FF6:	; read by HudOp_ShowText (pc $0117D0), HUD_RunPanelScript (pc $01175C), HudOp_Goto (pc $0117A0)
	incrom	$010FF6,$1C
Tbl_011012:	; read by HudOp_ShowText (pc $0117D0), HUD_RunPanelScript (pc $01175C), HudOp_Goto (pc $0117A0)
	incrom	$011012,$26
	include "src/f/011038.asm"
	include "src/f/011050.asm"
	include "src/f/0110A2.asm"
	include "src/f/0110BC.asm"
	include "src/f/011100.asm"
	include "src/f/011140.asm"
	include "src/f/0111C2.asm"
	include "src/f/0111FC.asm"
	include "src/f/011214.asm"
	include "src/f/011252.asm"
	include "src/f/0112C8.asm"
	include "src/f/011310.asm"
	include "src/f/011314.asm"
	include "src/f/0114D2.asm"
	include "src/f/01156E.asm"
	include "src/f/0115F0.asm"
	include "src/f/01161C.asm"
	include "src/f/011638.asm"
	include "src/f/011710.asm"
	include "src/f/01174A.asm"
Tbl_011764:	; addressed as data by code (operand in 01174A); never read in the corpus
	incrom	$011764,$24
	include "src/f/011788.asm"
	include "src/f/011796.asm"
	include "src/f/0117A0.asm"
	include "src/f/0117A8.asm"
	include "src/f/0117AE.asm"
	include "src/f/0117B4.asm"
	include "src/f/0117C8.asm"
	include "src/f/011834.asm"
	include "src/f/01189C.asm"
	include "src/f/0118E2.asm"
	include "src/f/01191C.asm"
Tbl_011974:	; read by HUD_DrawPanelSprites (pc $01194C), HUD_DrawPanelSprites (pc $011954); referenced by 0112C8
	incrom	$011974,$38
Tbl_0119AC:	; read by HUD_DrawPanelSprites (pc $011954), HUD_DrawPanelSprites (pc $01194C); 86% read in the corpus; referenced by 0112C8
	incrom	$0119AC,$38
	include "src/f/0119E4.asm"
	include "src/f/0119EA.asm"
	include "src/f/011A0C.asm"
	include "src/f/011A1C.asm"
	include "src/f/011AEE.asm"
	include "src/f/011B06.asm"
	include "src/f/011B32.asm"
Scr2_011B3E:	; a2-VM script: read by the a2 VM's handlers through a2; 92% read in the corpus; referenced by 011A1C, 011B32
	incrom	$011B3E,$50
Scr2_011B8E:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$011B8E,$2C
	include "src/f/011BBA.asm"
	include "src/f/011C5E.asm"
	include "src/f/011D5E.asm"
	include "src/f/0120A2.asm"
	incrom	$012540,$32
	include "src/f/012572.asm"
	incrom	$0125D6,$FC
	include "src/f/0126D2.asm"
	incrom	$0127B8,$18
	include "src/f/0127D0.asm"
	incrom	$012898,$4
	include "src/f/01289C.asm"
	include "src/f/0128F0.asm"
	incrom	$01297C,$AC
	include "src/f/012A28.asm"
	include "src/f/012A30.asm"
	include "src/f/012A84.asm"
	include "src/f/012CC6.asm"
	include "src/f/012CDA.asm"
	incrom	$012CF6,$10
	include "src/f/012D06.asm"
	include "src/f/012E02.asm"
	include "src/f/012E36.asm"
	include "src/f/012E5C.asm"
Tbl_012ECE:	; read by Trigger_LoadX (pc $012DD0); referenced by 033322
	incrom	$012ECE,$2
	include "src/f/012ED0.asm"
Tbl_012EF0:	; addressed as data by code (operand in 012ED0); never read in the corpus
	incrom	$012EF0,$C8
	include "src/f/012FB8.asm"
	include "src/f/012FBC.asm"
	incrom	$012FE4,$2A
	include "src/f/01300E.asm"
	incrom	$013032,$2
	include "src/f/013034.asm"
	incrom	$013036,$2
	include "src/f/013038.asm"
	incrom	$013060,$A
	include "src/f/01306A.asm"
Scr4_0130B0:	; a4-VM script by its format: 4 of 16 words are a4 handler entry addresses; never run in the corpus
	incrom	$0130B0,$20
Scr4_0130D0:	; a4-VM script: read by the a4 VM's handlers through a4; referenced by 013038
	incrom	$0130D0,$10
Scr4_0130E0:	; a4-VM script: read by the a4 VM's handlers through a4; referenced by 013038
	incrom	$0130E0,$8
Scr4_0130E8:	; a4-VM script: read by the a4 VM's handlers through a4; referenced by 013038
	incrom	$0130E8,$8
Scr4_0130F0:	; a4-VM script: read by the a4 VM's handlers through a4; referenced by 013038
	incrom	$0130F0,$A
Scr4_0130FA:	; a4-VM script: read by the a4 VM's handlers through a4; referenced by 01306A
	incrom	$0130FA,$20
Scr4_01311A:	; a4-VM script: read by the a4 VM's handlers through a4; referenced by 01306A
	incrom	$01311A,$10
Scr4_01312A:	; a4-VM script: read by the a4 VM's handlers through a4; referenced by 01306A
	incrom	$01312A,$8
Scr4_013132:	; a4-VM script: read by the a4 VM's handlers through a4; referenced by 01306A
	incrom	$013132,$8
Scr4_01313A:	; a4-VM script: read by the a4 VM's handlers through a4; referenced by 01306A
	incrom	$01313A,$A
Scr4_013144:	; a4-VM script: read by the a4 VM's handlers through a4; referenced by 01306A
	incrom	$013144,$6E
Scr4_0131B2:	; a4-VM script: read by the a4 VM's handlers through a4; referenced by 01306A
	incrom	$0131B2,$D6
	include "src/f/013288.asm"
Scr4_0132B2:	; a4-VM script: read by the a4 VM's handlers through a4
	incrom	$0132B2,$C
	include "src/f/0132BE.asm"
Scr2_0132D0:	; a2-VM script: read by the a2 VM's handlers through a2; 99% read in the corpus
	incrom	$0132D0,$10E
	include "src/f/0133DE.asm"
Scr2_0133EA:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 0133DE
	incrom	$0133EA,$8
	include "src/f/0133F2.asm"
Scr2_013402:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 0133F2
	incrom	$013402,$8
	include "src/f/01340A.asm"
Scr2_013416:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 01340A
	incrom	$013416,$2A
	include "src/f/013440.asm"
Scr2_01345A:	; a2-VM script: read by the a2 VM's handlers through a2; 43% read in the corpus
	incrom	$01345A,$84
	include "src/f/0134DE.asm"
Scr2_0134EA:	; a2-VM script of Item_SpawnSmallHealth: coroutine 1 $133AE (the drop), Goto $1341E: the large health pickup's body (a ROM slip: the small one is at $1351E)
	incrom	$0134EA,$8
	include "src/f/0134F2.asm"
Scr2_013502:	; a2-VM script of Item_SpawnSmallHealthFromHit: coroutine 1 $13378 (pops up), Goto the small pickup's body $1351E
	incrom	$013502,$8
	include "src/f/01350A.asm"
Scr2_013516:	; a2-VM script of Item_SpawnSmallHealthDrifting (coroutine 1 $133CA, the drift) and the small health pickup's bodies $1351E / $1352A (anim, touch coroutine $13536: sound $12, Item_CollectSmallHealth)
	incrom	$013516,$2A
	include "src/f/013540.asm"
Scr2_01355A:	; the small health pickup's anim scripts (Op2_SetAnim targets of its bodies $1351E / $1352A), as Scr2_01345A's for the large one; never read in the corpus (no corpus run drops a small health pickup)
	incrom	$01355A,$34
	include "src/f/01358E.asm"
Scr2_01359A:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 01358E
	incrom	$01359A,$8
	include "src/f/0135A2.asm"
Scr2_0135B2:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 0135A2
	incrom	$0135B2,$8
	include "src/f/0135BA.asm"
Scr2_0135C6:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 0135BA
	incrom	$0135C6,$2E
	include "src/f/0135F4.asm"
Scr2_0135FE:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$0135FE,$108
	include "src/f/013706.asm"
Scr2_013712:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 013706
	incrom	$013712,$8
	include "src/f/01371A.asm"
Scr2_01372A:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 01371A
	incrom	$01372A,$8
	include "src/f/013732.asm"
Scr2_01373E:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 013732
	incrom	$01373E,$20
	include "src/f/01375E.asm"
Scr2_013776:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$013776,$100
	include "src/f/013876.asm"
Scr2_013882:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 013876
	incrom	$013882,$8
	include "src/f/01388A.asm"
Scr2_01389A:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 01388A
	incrom	$01389A,$8
	include "src/f/0138A2.asm"
Scr2_0138AE:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 0138A2
	incrom	$0138AE,$2A
	include "src/f/0138D8.asm"
Scr2_0138E4:	; a2-VM script: read by the a2 VM's handlers through a2; 53% read in the corpus
	incrom	$0138E4,$58
	include "src/f/01393C.asm"
Scr2_013948:	; a2-VM script of Item_SpawnWeaponDown
	incrom	$013948,$E
	include "src/f/013956.asm"
Scr2_013962:	; a2-VM script of Item_SpawnWeapon0
	incrom	$013962,$E
	include "src/f/013970.asm"
Scr2_01397C:	; a2-VM script of Item_SpawnWeapon1
	incrom	$01397C,$E
	include "src/f/01398A.asm"
Scr2_013996:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 01398A
	incrom	$013996,$E
	include "src/f/0139A4.asm"
Scr2_0139B0:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 0139A4
	incrom	$0139B0,$E
	include "src/f/0139BE.asm"
Scr2_0139CA:	; a2-VM script: read by the a2 VM's handlers through a2; 32% read in the corpus; referenced by 0139BE
	incrom	$0139CA,$E
	include "src/f/0139D8.asm"
Scr2_0139E8:	; a2-VM script of Item_SpawnWeaponDownFromHit
	incrom	$0139E8,$E
	include "src/f/0139F6.asm"
Scr2_013A06:	; a2-VM script: read by the a2 VM's handlers through a2; 19% read in the corpus; referenced by 0139F6
	incrom	$013A06,$E
	include "src/f/013A14.asm"
Scr2_013A24:	; a2-VM script of Item_SpawnWeapon1FromHit
	incrom	$013A24,$E
	include "src/f/013A32.asm"
Scr2_013A42:	; a2-VM script of Item_SpawnWeapon2FromHit
	incrom	$013A42,$E
	include "src/f/013A50.asm"
Scr2_013A60:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 013A50
	incrom	$013A60,$E
	include "src/f/013A6E.asm"
Scr2_013A7E:	; a2-VM script: read by the a2 VM's handlers through a2; 35% read in the corpus; referenced by 013A6E
	incrom	$013A7E,$E
	include "src/f/013A8C.asm"
Scr2_013A98:	; a2-VM script of Item_SpawnWeaponDownDrifting
	incrom	$013A98,$E
	include "src/f/013AA6.asm"
Scr2_013AB2:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 013AA6
	incrom	$013AB2,$E
	include "src/f/013AC0.asm"
Scr2_013ACC:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 013AC0
	incrom	$013ACC,$E
	include "src/f/013ADA.asm"
Scr2_013AE6:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 013ADA
	incrom	$013AE6,$E
	include "src/f/013AF4.asm"
Scr2_013B00:	; a2-VM script: read by the a2 VM's handlers through a2; 35% read in the corpus; referenced by 013AF4
	incrom	$013B00,$E
	include "src/f/013B0E.asm"
Scr2_013B1A:	; a2-VM script of Item_SpawnWeaponRandomOrDownDrifting
	incrom	$013B1A,$E
	include "src/f/013B28.asm"
Scr2_013B34:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 013B28
	incrom	$013B34,$16
	include "src/f/013B4A.asm"
Scr2_013B56:	; a2-VM script: read by the a2 VM's handlers through a2; 92% read in the corpus; referenced by 013B4A
	incrom	$013B56,$134
	include "src/f/013C8A.asm"
	include "src/f/013D18.asm"
Scr2_013D2A:	; a2-VM script: read by the a2 VM's handlers through a2 (the span after the native Item_CollectWeaponLevelUp, labelled Scr2_013D18 until the P4 kickoff found code at its start)
	incrom	$013D2A,$1C6
	include "src/f/013EF0.asm"
	include "src/f/013F02.asm"
Scr2_013F2A:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 013F02
	incrom	$013F2A,$C
	include "src/f/013F36.asm"
Scr2_013F3E:	; a2-VM script: read by the a2 VM's handlers through a2; 6% read in the corpus
	incrom	$013F3E,$20
	include "src/f/013F5E.asm"
	include "src/f/013F92.asm"
	include "src/f/013FA4.asm"
Scr2_013FC4:	; a2-VM script: read by the a2 VM's handlers through a2; 97% read in the corpus
	incrom	$013FC4,$3C
Scr2_014000:	; a2-VM script: read by the a2 VM's handlers through a2; 61% read in the corpus
	incrom	$014000,$7A
Tbl_01407A:	; read by Text_DrawSpritesVertical (pc $01416C); referenced by 049C88
	incrom	$01407A,$7
Tbl_014081:	; read by Text_DrawSprites (pc $014130), Text_DrawSpritesVertical (pc $01416C); 88% read in the corpus; referenced by 049C88
	incrom	$014081,$8D
	include "src/f/01410E.asm"
	include "src/f/01412E.asm"
	include "src/f/01416A.asm"
Tbl_0141B2:	; read by Text_DrawSprites (pc $014140), Text_DrawSpritesVertical (pc $014182); 42% read in the corpus; referenced by 01412E, 01416A
	incrom	$0141B2,$1D8
	include "src/f/01438A.asm"
	include "src/f/01438C.asm"
	include "src/f/0143AA.asm"
	incrom	$0143B4,$4
	include "src/f/0143B8.asm"
	incrom	$0143C0,$C
	include "src/f/0143CC.asm"
	include "src/f/0143E4.asm"
	include "src/f/014402.asm"
	include "src/f/014408.asm"
	include "src/f/014440.asm"
	include "src/f/014480.asm"
	include "src/f/014494.asm"
	include "src/f/0144B2.asm"
	include "src/f/0144D4.asm"
	include "src/f/01458C.asm"
	include "src/f/0145BE.asm"
Scr4_014612:	; a4-VM script: read by the a4 VM's handlers through a4; 69% read in the corpus
	incrom	$014612,$8B4
	include "src/f/014EC6.asm"
	incrom	$014EE2,$2
	include "src/f/014EE4.asm"
	include "src/f/014F78.asm"
Scr4_015046:	; a4-VM script: read by the a4 VM's handlers through a4; 72% read in the corpus
	incrom	$015046,$388
	include "src/f/0153CE.asm"
Tbl_0155A8:	; read by VDP_LoadRegs (pc $00AE32); 95% read in the corpus
	incrom	$0155A8,$14
Tbl_0155BC:	; read by VDP_LoadRegs (pc $00AE32); 95% read in the corpus; referenced by 009E02
	incrom	$0155BC,$78
Tbl_015634:	; read by Pal_LoadLine0 (pc $00ABD0), Pal_LoadLine3 (pc $00AC24); referenced by 0145BE, 01FB3A, 01FC50, 01FD76 ...
	incrom	$015634,$40
	include "src/f/015674.asm"
	incrom	$015676,$26
	include "src/f/01569C.asm"
	include "src/f/0156FC.asm"
	include "src/f/015730.asm"
	incrom	$01590A,$C
	include "src/f/015916.asm"
	include "src/f/015940.asm"
Tbl_015A7E:	; read by Stage1_VIntFireStep (pc $0159F6); referenced by 015940, 01FB3A
	incrom	$015A7E,$1E0
	include "src/f/015C5E.asm"
	include "src/f/015CA6.asm"
Tbl_015CF4:	; read by Stage1_DrawSignPanel (pc $015CAA)
	incrom	$015CF4,$40
	include "src/f/015D34.asm"
	include "src/f/015D90.asm"
Tbl_015F5A:	; read by Trigger_LoadX (pc $012DF4), Trigger_Fire (pc $012E3C), Wave_StartFromEntry (pc $00932C); 87% read in the corpus; referenced by 01FB3A
	incrom	$015F5A,$460
Tbl_0163BA:	; read by Trigger_LoadX (pc $012DF4), Trigger_Fire (pc $012E3C), Wave_StartFromEntry (pc $00932C); 87% read in the corpus; referenced by 01FC50
	incrom	$0163BA,$278
Tbl_016632:	; read by Trigger_LoadX (pc $012DF4), Trigger_Fire (pc $012E3C), Wave_StartFromEntry (pc $00932C); 88% read in the corpus; referenced by 01FD76
	incrom	$016632,$B8
Tbl_0166EA:	; read by Trigger_LoadX (pc $012DF4), Trigger_Fire (pc $012E3C), VM5_Next (pc $006A56); 76% read in the corpus; referenced by 01FECC
	incrom	$0166EA,$BD4
	incrom	$0172BE,$232
Tbl_0174F0:	; read by VM5_Next (pc $006A56), VM5_SpawnEntry (pc $006FEC), Op5_Wait (pc $006AC6); 79% read in the corpus
	incrom	$0174F0,$94A
	incrom	$017E3A,$200
Tbl_01803A:	; read by Pal_LoadLine2 (pc $00AC08)
	incrom	$01803A,$20
	include "src/f/01805A.asm"
Scr2_018066:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 01805A
	incrom	$018066,$6
	include "src/f/01806C.asm"
Scr2_018076:	; a2-VM script: read by the a2 VM's handlers through a2; 98% read in the corpus
	incrom	$018076,$1F2
Scr2_018268:	; a2-VM script by its format: 123 of 352 words are a2 handler entry addresses; never run in the corpus
	incrom	$018268,$2C0
Scr2_018528:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$018528,$C
	incrom	$018534,$910
Scr2_018E44:	; a2-VM script: read by the a2 VM's handlers through a2; 80% read in the corpus
	incrom	$018E44,$344
	incrom	$019188,$2EC
Scr2_019474:	; a2-VM script: read by the a2 VM's handlers through a2; 86% read in the corpus
	incrom	$019474,$1B2
	include "src/f/019626.asm"
Scr2_01963A:	; a2-VM script by its format: 58 of 132 words are a2 handler entry addresses; never run in the corpus
	incrom	$01963A,$108
Scr2_019742:	; a2-VM script: read by the a2 VM's handlers through a2; 86% read in the corpus
	incrom	$019742,$BA
	include "src/f/0197FC.asm"
Scr2_01980A:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$01980A,$4
	include "src/f/01980E.asm"
Scr2_01981C:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$01981C,$2
	include "src/f/01981E.asm"
Scr2_019822:	; a2-VM script: read by the a2 VM's handlers through a2; 94% read in the corpus
	incrom	$019822,$82A
	include "src/f/01A04C.asm"
Tbl_01A28E:	; read by Boss_Harley_FireAlongAim (pc $01A27C); referenced by 01A04C
	incrom	$01A28E,$C0
	include "src/f/01A34E.asm"
Scr4_01A388:	; a4-VM script: read by the a4 VM's handlers through a4; 100% read in the corpus
	incrom	$01A388,$7EE
Scr4_01AB76:	; a4-VM script: read by the a4 VM's handlers through a4; 47% read in the corpus; referenced by 01A34E
	incrom	$01AB76,$16A
	include "src/f/01ACE0.asm"
Scr2_01ACEC:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 01ACE0
	incrom	$01ACEC,$32
	include "src/f/01AD1E.asm"
Scr2_01AD2A:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 01AD1E
	incrom	$01AD2A,$32
	include "src/f/01AD5C.asm"
Scr2_01AD68:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 01AD5C
	incrom	$01AD68,$32
Scr2_01AD9A:	; a2-VM script by its format: 55 of 155 words are a2 handler entry addresses; never run in the corpus
	incrom	$01AD9A,$136
	include "src/f/01AED0.asm"
Scr2_01AEDC:	; a2-VM script: read by the a2 VM's handlers through a2; 84% read in the corpus; referenced by 01AED0
	incrom	$01AEDC,$32
	include "src/f/01AF0E.asm"
Scr2_01AF1A:	; a2-VM script: read by the a2 VM's handlers through a2; 79% read in the corpus; referenced by 01AF0E
	incrom	$01AF1A,$71A
	include "src/f/01B634.asm"
Scr2_01B640:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 01B634
	incrom	$01B640,$4E
	include "src/f/01B68E.asm"
Scr2_01B69A:	; a2-VM script: read by the a2 VM's handlers through a2; 98% read in the corpus; referenced by 01B68E
	incrom	$01B69A,$60
	include "src/f/01B6FA.asm"
Scr2_01B706:	; a2-VM script: read by the a2 VM's handlers through a2; 42% read in the corpus; referenced by 01B6FA
	incrom	$01B706,$4E
	include "src/f/01B754.asm"
Scr2_01B760:	; a2-VM script of Enemy_Clown_SpawnRanges: Scr2_01B69A's words (Enemy_Clown_SpawnGreen: random approach range) with Op2_ModSprAttr $9860 (palette line 3), energy $80, score $64
	incrom	$01B760,$60
	include "src/f/01B7C0.asm"
Scr2_01B7CC:	; a2-VM script: read by the a2 VM's handlers through a2; 91% read in the corpus; referenced by 01B7C0
	incrom	$01B7CC,$792
	include "src/f/01BF5E.asm"
Scr2_01BF6A:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 01BF5E
	incrom	$01BF6A,$4E
	include "src/f/01BFB8.asm"
Scr2_01BFC4:	; a2-VM script: read by the a2 VM's handlers through a2; 95% read in the corpus; referenced by 01BFB8
	incrom	$01BFC4,$42E
	include "src/f/01C3F2.asm"
Scr2_01C4E6:	; a2-VM script: read by the a2 VM's handlers through a2; 97% read in the corpus; referenced by 01C3F2
	incrom	$01C4E6,$108
	include "src/f/01C5EE.asm"
Scr2_01C602:	; a2-VM script: read by the a2 VM's handlers through a2; 97% read in the corpus
	incrom	$01C602,$114
Scr2_01C716:	; a2-VM script by its format: 201 of 687 words are a2 handler entry addresses; never run in the corpus
	incrom	$01C716,$55E
Scr2_01CC74:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$01CC74,$2C
	incrom	$01CCA0,$7D0
Scr2_01D470:	; a2-VM script: read by the a2 VM's handlers through a2; 75% read in the corpus
	incrom	$01D470,$88E
	include "src/f/01DCFE.asm"
Tbl_01DD48:	; Boss_JokerBalloon_ThrowByArg's throws, 8 bytes each by g_VM2Arg0: dx, dy (from the boss), heading word, kind x 4 (Tbl_01DE8A index)
	incrom	$01DD48,$58
Scr4_01DDA0:	; a4-VM script: the puff Boss_JokerBalloon_ThrowByArg runs (Fx_RunScript) where a throw starts
	incrom	$01DDA0,$CA
	include "src/f/01DE6A.asm"
Tbl_01DE8A:	; Boss_JokerBalloon_SpawnThrown's 11 a2 scripts by kind (dc.l): thrown left at their own cruise x with an arcing Op2_JumpAnim
	incrom	$01DE8A,$2C
Scr2_01DEB6:	; a2-VM scripts of the Joker balloon boss's thrown objects (Tbl_01DE8A: Op2_SetSpeed, SetAccelXForDist / SetCruiseX, CruiseLeft, SetCruiseY, JumpAnim, Goto a shared body), then the boss's later scripts
	incrom	$01DEB6,$318
Scr4_01E1CE:	; a4-VM script: read by the a4 VM's handlers through a4; 99% read in the corpus
	incrom	$01E1CE,$3BE
	include "src/f/01E58C.asm"
Scr2_01E5C2:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$01E5C2,$148
	include "src/f/01E70A.asm"
Tbl_01E748:	; read by Pal_LoadLine3 (pc $00AC24); referenced by 01E70A
	incrom	$01E748,$20
	include "src/f/01E768.asm"
Scr2_01E778:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$01E778,$EC
	include "src/f/01E864.asm"
Scr2_01E86E:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$01E86E,$C
	include "src/f/01E87A.asm"
Scr2_01E87E:	; a2-VM script: read by the a2 VM's handlers through a2; 98% read in the corpus
	incrom	$01E87E,$126
	include "src/f/01E9A4.asm"
	include "src/f/01EAD0.asm"
	include "src/f/01EB06.asm"
	include "src/f/01EB2C.asm"
	include "src/f/01EB86.asm"
Tbl_01EBCE:	; addressed as data by code (operand in 01EB86); never read in the corpus
	incrom	$01EBCE,$40
Tbl_ClawChainPoints:	; Harley's crane claw: 12 chain points of 10 bytes (3D), drawn by Boss_Claw_DrawChain; datamap first took it for an a4 script (an a4 handler read it through a4)
	incrom	$01EC0E,$78
Tbl_ClawFingerPoints:	; Harley's crane claw: finger points (3 per finger, 3D), transformed by Boss_Claw_PlaceFinger; 95% read in the corpus; datamap first took it for an a4 script
	incrom	$01EC86,$162
	include "src/f/01EDE8.asm"
Scr2_01EDF4:	; a2-VM script: read by the a2 VM's handlers through a2; 95% read in the corpus; referenced by 01EDE8
	incrom	$01EDF4,$2A
	include "src/f/01EE1E.asm"
Scr2_01EE34:	; a2-VM script: read by the a2 VM's handlers through a2; 94% read in the corpus; referenced by 01EE1E
	incrom	$01EE34,$24
	include "src/f/01EE58.asm"
Scr2_01EE64:	; a2-VM script: read by the a2 VM's handlers through a2; 95% read in the corpus; referenced by 01EE58
	incrom	$01EE64,$162
	include "src/f/01EFC6.asm"
Scr2_01EFCE:	; a2-VM script: read by the a2 VM's handlers through a2; 99% read in the corpus
	incrom	$01EFCE,$FE
	include "src/f/01F0CC.asm"
Scr4_01F0DC:	; a4-VM script: read by the a4 VM's handlers through a4; 92% read in the corpus
	incrom	$01F0DC,$3A4
	include "src/f/01F480.asm"
Scr2_01F496:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$01F496,$19E
	include "src/f/01F634.asm"
Scr2_01F64A:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$01F64A,$112
	include "src/f/01F75C.asm"
Scr2_01F772:	; a2-VM script: read by the a2 VM's handlers through a2; 99% read in the corpus
	incrom	$01F772,$110
	include "src/f/01F882.asm"
Scr2_01F898:	; a2-VM script: read by the a2 VM's handlers through a2; 72% read in the corpus
	incrom	$01F898,$19E
Tbl_01FA36:	; read by Pal_LoadLine3 (pc $00AC24), Op6_Decompress (pc $00710E), Scene_InitStage (pc $0099D8); referenced by 01FB3A, 01FC50, 01FECC
	incrom	$01FA36,$40
Tbl_01FA76:	; read by Op6_Decompress (pc $00710E), Scene_InitStage (pc $0099D8), Pal_LoadLine2 (pc $00AC08); referenced by 01FB3A
	incrom	$01FA76,$3C
Tbl_01FAB2:	; read by Op6_Decompress (pc $00710E), Scene_InitStage (pc $0099D8), Pal_LoadLine2 (pc $00AC08); referenced by 01FB3A
	incrom	$01FAB2,$88
	include "src/f/01FB3A.asm"
Scr6_01FBCA:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 01FC50
	incrom	$01FBCA,$3C
Scr6_01FC06:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 01FC50
	incrom	$01FC06,$4A
	include "src/f/01FC50.asm"
Scr6_01FCE6:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 01FD76
	incrom	$01FCE6,$3C
Scr6_01FD22:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 01FD76
	incrom	$01FD22,$54
	include "src/f/01FD76.asm"
Scr6_01FDF6:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 01FECC
	incrom	$01FDF6,$3C
Scr6_01FE32:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 01FECC
	incrom	$01FE32,$9A
	include "src/f/01FECC.asm"
Scr4_01FF8C:	; a4-VM script: read by the a4 VM's handlers through a4; referenced by 01FECC
	incrom	$01FF8C,$1A
	include "src/f/01FFA6.asm"
Scr2_01FFB8:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 01FFA6
	incrom	$01FFB8,$48
Scr2_020000:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$020000,$82
	include "src/f/020082.asm"
Scr2_020094:	; a2-VM script: read by the a2 VM's handlers through a2; 81% read in the corpus; referenced by 020082
	incrom	$020094,$1E2
	include "src/f/020276.asm"
	incrom	$02028E,$C
	include "src/f/02029A.asm"
	include "src/f/020316.asm"
Tbl_020354:	; read by Stage1_LaunchBalloonA (pc $0202BE), Stage1_LaunchBalloonB (pc $0202FC); referenced by 020276, 02029A
	incrom	$020354,$1A
Tbl_02036E:	; read by Stage1_LaunchBalloonA (pc $0202BE), Stage1_LaunchBalloonB (pc $0202FC); 43% read in the corpus; referenced by 020276, 02029A
	incrom	$02036E,$1A
Tbl_020388:	; Stage1_LaunchBalloonC's height pairs (word height1, height2) ended by a negative word, the third table beside Tbl_020354 / Tbl_02036E
	incrom	$020388,$22
	include "src/f/0203AA.asm"
Scr2_0203E0:	; a2-VM script: read by the a2 VM's handlers through a2; 97% read in the corpus; referenced by 02029A
	incrom	$0203E0,$E8
	include "src/f/0204C8.asm"
Scr2_0204D4:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 0204C8
	incrom	$0204D4,$24
	include "src/f/0204F8.asm"
Scr2_020504:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 0204F8
	incrom	$020504,$4C
	include "src/f/020550.asm"
Scr4_02055C:	; a4-VM script: read by the a4 VM's handlers through a4; 99% read in the corpus
	incrom	$02055C,$C0
	include "src/f/02061C.asm"
Scr2_020628:	; a2-VM script: read by the a2 VM's handlers through a2; 99% read in the corpus; referenced by 02061C
	incrom	$020628,$16A
	include "src/f/020792.asm"
Scr2_02079E:	; a2-VM script: read by the a2 VM's handlers through a2; 99% read in the corpus; referenced by 020792
	incrom	$02079E,$C8
	include "src/f/020866.asm"
	include "src/f/0208BE.asm"
	include "src/f/0208F8.asm"
Tbl_02095E:	; read by Stage1_InitSignDebris (pc $0208F8); referenced by 0208BE
	incrom	$02095E,$B4
	include "src/f/020A12.asm"
	include "src/f/020A48.asm"
	incrom	$020C24,$2
Tbl_020C26:	; read by Stage1_RubblePieceRun (pc $020BE2), Stage1_RubbleRainRun (pc $020B18); referenced by 020A48
	incrom	$020C26,$B6
	include "src/f/020CDC.asm"
Scr2_020CF6:	; a2-VM script: read by the a2 VM's handlers through a2; 95% read in the corpus; referenced by 020CDC
	incrom	$020CF6,$80
	include "src/f/020D76.asm"
	incrom	$020DC4,$16
	include "src/f/020DDA.asm"
Pal_FireFlicker:	; two 32-byte palettes, loaded alternately (+$20) by Stage1_StartFireFlicker / the fire-flicker loop at $020DDA; datamap first took it for an a4 script
	incrom	$020E1C,$40
Scr4_020E5C:	; a4-VM script: read by the a4 VM's handlers through a4; referenced by 020D76
	incrom	$020E5C,$CC
Scr4_020F28:	; a4-VM script: read by the a4 VM's handlers through a4; referenced by 020D76
	incrom	$020F28,$132
Tbl_02105A:	; read by Trigger_LoadY (pc $012E80), Trigger_Fire (pc $012E3C), Trigger_LoadY (pc $012E92); 78% read in the corpus; referenced by 032F14
	incrom	$02105A,$9B6
	incrom	$021A10,$116
Tbl_021B26:	; read by VM5_Next (pc $006A56), VM5_SpawnEntry (pc $006FEE), Op5_Wait (pc $006AC6); 92% read in the corpus
	incrom	$021B26,$200
Tbl_021D26:	; read by Trigger_LoadX (pc $012DD0), Trigger_Fire (pc $012E3C), Wave_StartFromEntry (pc $00932C); 90% read in the corpus; referenced by 033274
	incrom	$021D26,$14
Scr5_021D3A:	; a5-VM script: read by the a5 VM's handlers through a5; 65% read in the corpus; referenced by 03334A
	incrom	$021D3A,$A46
Scr5_022780:	; a5-VM script by its format: 107 of 367 words are a5 handler entry addresses; never run in the corpus
	incrom	$022780,$2DE
Scr5_022A5E:	; a5-VM script: read by the a5 VM's handlers through a5; 97% read in the corpus
	incrom	$022A5E,$80
Scr5_022ADE:	; a5-VM script by its format: 66 of 208 words are a5 handler entry addresses; never run in the corpus
	incrom	$022ADE,$1A0
Scr5_022C7E:	; a5-VM script: read by the a5 VM's handlers through a5; 99% read in the corpus
	incrom	$022C7E,$12C
	incrom	$022DAA,$1FA
Tbl_022FA4:	; read by VM5_Next (pc $006A56), Op5_SpawnRandom (pc $006FD2), VM5_SpawnEntry (pc $006FEC); 91% read in the corpus
	incrom	$022FA4,$8C
Scr5_023030:	; a5-VM script by its format: 41 of 147 words are a5 handler entry addresses; never run in the corpus
	incrom	$023030,$126
Scr5_023156:	; a5-VM script: read by the a5 VM's handlers through a5; 84% read in the corpus
	incrom	$023156,$20E
Scr5_023364:	; a5-VM script by its format: 63 of 188 words are a5 handler entry addresses; never run in the corpus
	incrom	$023364,$178
Tbl_0234DC:	; read by VM5_Next (pc $006A56), VM5_SpawnEntry (pc $006FEC), Op5_Wait (pc $006AC6); 97% read in the corpus
	incrom	$0234DC,$D8
Scr5_0235B4:	; a5-VM script by its format: 72 of 216 words are a5 handler entry addresses; never run in the corpus
	incrom	$0235B4,$1B0
Tbl_023764:	; read by VM5_Next (pc $006A56), VM5_SpawnEntry (pc $006FEC), Op5_Wait (pc $006AC6); 98% read in the corpus
	incrom	$023764,$1FC
Scr5_023960:	; a5-VM script by its format: 54 of 146 words are a5 handler entry addresses; never run in the corpus
	incrom	$023960,$124
Tbl_023A84:	; read by Op5_SpawnRandom (pc $006FD2), VM5_Next (pc $006A56), VM5_SpawnEntry (pc $006FEC); 99% read in the corpus
	incrom	$023A84,$B4
Scr5_023B38:	; a5-VM script by its format: 42 of 160 words are a5 handler entry addresses; never run in the corpus
	incrom	$023B38,$140
Scr5_023C78:	; a5-VM script: read by the a5 VM's handlers through a5; 91% read in the corpus
	incrom	$023C78,$476
	include "src/f/0240EE.asm"
	include "src/f/02417E.asm"
Scr4_024360:	; a4-VM script: read by the a4 VM's handlers through a4; 99% read in the corpus; referenced by 02417E
	incrom	$024360,$148
	include "src/f/0244A8.asm"
	include "src/f/024576.asm"
	include "src/f/0245E0.asm"
Tbl_024642:	; read by Stage2_LiftColumnScroll (pc $02462E); referenced by 0245E0
	incrom	$024642,$1E
	include "src/f/024660.asm"
Tbl_024880:	; read by Flight_UpdateCorridor (pc $024872); referenced by 024660
	incrom	$024880,$80
	include "src/f/024900.asm"
	include "src/f/024ABC.asm"
Scr2_024AE2:	; a2-VM script: read by the a2 VM's handlers through a2; 89% read in the corpus; referenced by 024ABC
	incrom	$024AE2,$36
	include "src/f/024B18.asm"
Scr2_024B24:	; a2-VM script: read by the a2 VM's handlers through a2; 89% read in the corpus; referenced by 024B18
	incrom	$024B24,$36
	include "src/f/024B5A.asm"
Scr2_024B66:	; a2-VM script: read by the a2 VM's handlers through a2; 89% read in the corpus; referenced by 024B5A
	incrom	$024B66,$38
	include "src/f/024B9E.asm"
Scr2_024BAA:	; a2-VM script: read by the a2 VM's handlers through a2; 63% read in the corpus; referenced by 024B9E
	incrom	$024BAA,$36
	include "src/f/024BE0.asm"
Scr2_024BEC:	; a2-VM script: read by the a2 VM's handlers through a2; 31% read in the corpus; referenced by 024BE0
	incrom	$024BEC,$34
	include "src/f/024C20.asm"
Scr2_024C2C:	; a2-VM script: read by the a2 VM's handlers through a2; 88% read in the corpus; referenced by 024C20
	incrom	$024C2C,$34
	include "src/f/024C60.asm"
Scr2_024C6C:	; a2-VM script: read by the a2 VM's handlers through a2; 87% read in the corpus; referenced by 024C60
	incrom	$024C6C,$2E
	include "src/f/024C9A.asm"
Scr2_024CA6:	; a2-VM script: read by the a2 VM's handlers through a2; 97% read in the corpus; referenced by 024C9A
	incrom	$024CA6,$2CA
	include "src/f/024F70.asm"
Scr2_024FE4:	; a2-VM script: read by the a2 VM's handlers through a2; 100% read in the corpus
	incrom	$024FE4,$38A
	include "src/f/02536E.asm"
	include "src/f/0253D0.asm"
Scr2_0253DC:	; a2-VM script: read by the a2 VM's handlers through a2; 91% read in the corpus; referenced by 0253D0
	incrom	$0253DC,$74
	include "src/f/025450.asm"
Scr2_025466:	; the barrel's script after Enemy_Barrel_KnockByHit's jsr VM2_Enter: its per-frame loop (step coroutine 0, velocity, terrain, anim, boxes, draw) and its break-up
	incrom	$025466,$EE
	include "src/f/025554.asm"
Scr2_025570:	; a2-VM script: read by the a2 VM's handlers through a2; 91% read in the corpus; referenced by 025554
	incrom	$025570,$42
	include "src/f/0255B2.asm"
Scr2_0255C2:	; a2-VM script: read by the a2 VM's handlers through a2; 77% read in the corpus; referenced by 0255B2
	incrom	$0255C2,$5E
	include "src/f/025620.asm"
Scr2_025630:	; a2-VM script: read by the a2 VM's handlers through a2; 82% read in the corpus; referenced by 025620
	incrom	$025630,$4C
	include "src/f/02567C.asm"
Scr2_02568C:	; a2-VM script: read by the a2 VM's handlers through a2; 88% read in the corpus; referenced by 02567C
	incrom	$02568C,$32
	include "src/f/0256BE.asm"
Scr2_0256CE:	; a2-VM script: read by the a2 VM's handlers through a2; 55% read in the corpus; referenced by 0256BE
	incrom	$0256CE,$3E
	include "src/f/02570C.asm"
Scr2_02571C:	; a2-VM script of Enemy_PoleThug_SpawnE
	incrom	$02571C,$184
	include "src/f/0258A0.asm"
Scr2_0258C2:	; a2-VM script: read by the a2 VM's handlers through a2; 93% read in the corpus
	incrom	$0258C2,$1CA
	include "src/f/025A8C.asm"
Scr4_025AC0:	; a4-VM script: read by the a4 VM's handlers through a4; 90% read in the corpus; referenced by 025A8C
	incrom	$025AC0,$122
	include "src/f/025BE2.asm"
Scr2_025BEE:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 025BE2
	incrom	$025BEE,$3E
	include "src/f/025C2C.asm"
Scr2_025C38:	; a2-VM script: read by the a2 VM's handlers through a2; 97% read in the corpus
	incrom	$025C38,$48
	include "src/f/025C80.asm"
Scr2_025C8A:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$025C8A,$76
	include "src/f/025D00.asm"
Scr2_025D0A:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$025D0A,$4
	include "src/f/025D0E.asm"
Scr2_025D18:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$025D18,$2
	include "src/f/025D1A.asm"
Scr2_025D1E:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$025D1E,$C
	include "src/f/025D2A.asm"
Scr2_025D32:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$025D32,$3C
	include "src/f/025D6E.asm"
Scr2_025D7E:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$025D7E,$2
	include "src/f/025D80.asm"
Scr2_025D8E:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$025D8E,$2
	include "src/f/025D90.asm"
Scr2_025D94:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$025D94,$44
	include "src/f/025DD8.asm"
Scr2_025DE0:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$025DE0,$6C
	include "src/f/025E4C.asm"
Scr2_025E62:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$025E62,$1EA
	include "src/f/02604C.asm"
	include "src/f/0260F2.asm"
Scr2_026148:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 0260F2
	incrom	$026148,$10
	include "src/f/026158.asm"
Scr2_026164:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 026158
	incrom	$026164,$10
	include "src/f/026174.asm"
Scr2_026180:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 026174
	incrom	$026180,$10
	include "src/f/026190.asm"
Scr2_02619C:	; a2-VM script: read by the a2 VM's handlers through a2; 98% read in the corpus; referenced by 026190
	incrom	$02619C,$76
	include "src/f/026212.asm"
Scr2_02621E:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$02621E,$370
	include "src/f/02658E.asm"
Scr2_0265A4:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 02658E
	incrom	$0265A4,$38
	include "src/f/0265DC.asm"
Scr2_0265F2:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$0265F2,$2A
	include "src/f/02661C.asm"
Scr2_026626:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$026626,$1E
	include "src/f/026644.asm"
Scr2_026648:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$026648,$E
	include "src/f/026656.asm"
Scr2_026666:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$026666,$C
	include "src/f/026672.asm"
Scr2_02668C:	; a2-VM script: read by the a2 VM's handlers through a2; 94% read in the corpus
	incrom	$02668C,$266
	include "src/f/0268F2.asm"
Scr4_026A46:	; a4-VM script: read by the a4 VM's handlers through a4; 90% read in the corpus
	incrom	$026A46,$41C
Scr2_026E62:	; a2-VM script by its format: 82 of 192 words are a2 handler entry addresses; never run in the corpus
	incrom	$026E62,$180
Scr2_026FE2:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$026FE2,$A0
Scr2_027082:	; a2-VM script by its format: 109 of 242 words are a2 handler entry addresses; never run in the corpus
	incrom	$027082,$1E4
Scr2_027266:	; a2-VM script: read by the a2 VM's handlers through a2; 58% read in the corpus
	incrom	$027266,$350
Scr2_0275B6:	; a2-VM script by its format: 151 of 256 words are a2 handler entry addresses; never run in the corpus
	incrom	$0275B6,$200
Scr2_0277B6:	; a2-VM script: read by the a2 VM's handlers through a2; 48% read in the corpus
	incrom	$0277B6,$A8
	include "src/f/02785E.asm"
Scr2_027872:	; a2-VM script: read by the a2 VM's handlers through a2; 12% read in the corpus; referenced by 02785E
	incrom	$027872,$8
	include "src/f/02787A.asm"
Scr2_02788E:	; a2-VM script of Enemy_LadderThug_SpawnD
	incrom	$02788E,$8
	include "src/f/027896.asm"
Scr2_0278AA:	; a2-VM script of Enemy_LadderThug_SpawnE
	incrom	$0278AA,$8
	include "src/f/0278B2.asm"
Scr2_0278C6:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 0278B2
	incrom	$0278C6,$8
	include "src/f/0278CE.asm"
Scr4_0278E2:	; a4-VM script: read by the a4 VM's handlers through a4; 99% read in the corpus; referenced by 0278CE
	incrom	$0278E2,$23A
	incrom	$027B1C,$10C
Scr4_027C28:	; a4-VM script: read by the a4 VM's handlers through a4; 23% read in the corpus
	incrom	$027C28,$6A
	include "src/f/027C92.asm"
Scr2_027CA2:	; a2-VM script: read by the a2 VM's handlers through a2; 73% read in the corpus
	incrom	$027CA2,$19C
	include "src/f/027E3E.asm"
Scr2_027E58:	; a2-VM script: read by the a2 VM's handlers through a2; 97% read in the corpus
	incrom	$027E58,$310
	include "src/f/028168.asm"
Scr2_028176:	; a2-VM script: read by the a2 VM's handlers through a2; 98% read in the corpus
	incrom	$028176,$18C
	include "src/f/028302.asm"
Scr2_028326:	; a2-VM script: read by the a2 VM's handlers through a2; 100% read in the corpus
	incrom	$028326,$46E
	include "src/f/028794.asm"
Scr2_0287B0:	; a2-VM script: read by the a2 VM's handlers through a2; 81% read in the corpus
	incrom	$0287B0,$5C0
	include "src/f/028D70.asm"
Scr2_028D80:	; a2-VM script: read by the a2 VM's handlers through a2; 100% read in the corpus
	incrom	$028D80,$302
	include "src/f/029082.asm"
	include "src/f/0290B6.asm"
Scr2_02915A:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$02915A,$F6
	include "src/f/029250.asm"
Scr2_02928A:	; a2-VM script: read by the a2 VM's handlers through a2; 100% read in the corpus
	incrom	$02928A,$1A8
	include "src/f/029432.asm"
Scr2_029454:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$029454,$10
	include "src/f/029464.asm"
Scr2_029472:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$029472,$16
	include "src/f/029488.asm"
Scr2_0294A8:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$0294A8,$44
Scr2_0294EC:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$0294EC,$32
	include "src/f/02951E.asm"
Scr2_02952E:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$02952E,$6
	include "src/f/029534.asm"
	include "src/f/029572.asm"
Scr2_029598:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$029598,$24
	include "src/f/0295BC.asm"
Scr2_0295C6:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$0295C6,$E
Scr2_0295D4:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$0295D4,$34
	include "src/f/029608.asm"
Scr2_02961E:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$02961E,$18
	include "src/f/029636.asm"
Scr2_029640:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$029640,$4
	include "src/f/029644.asm"
Scr2_02964C:	; a2-VM script: the beam-ship radius object's last ops after Boss_BeamShip_ClearRadius's jsr $7FE.w (VM2_Enter): $2CBC Op2_CallNative Section_StartExit ($010528), $095C Op2_Kill; TAS f79108
	incrom	$02964C,$8
Scr4_029654:	; a4-VM script: read by the a4 VM's handlers through a4 (the span datamap measured from $02964C, 92% read in the corpus; its first 8 bytes are Scr2_02964C)
	incrom	$029654,$22E
	include "src/f/029882.asm"
Scr4_0298AE:	; a4-VM script: read by the a4 VM's handlers through a4; 36% read in the corpus
	incrom	$0298AE,$124
	include "src/f/0299D2.asm"
Scr2_0299DE:	; a2-VM script: read by the a2 VM's handlers through a2; 55% read in the corpus; referenced by 0299D2
	incrom	$0299DE,$C6
	include "src/f/029AA4.asm"
Scr2_029AB0:	; a2-VM script: read by the a2 VM's handlers through a2; 56% read in the corpus; referenced by 029AA4
	incrom	$029AB0,$10A
	include "src/f/029BBA.asm"
Scr2_029BC6:	; a2-VM script: read by the a2 VM's handlers through a2; 77% read in the corpus; referenced by 029BBA
	incrom	$029BC6,$146
	include "src/f/029D0C.asm"
Scr2_029D18:	; a2-VM script: read by the a2 VM's handlers through a2; 98% read in the corpus; referenced by 029D0C
	incrom	$029D18,$B4
	include "src/f/029DCC.asm"
Scr2_029DDC:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 029DCC
	incrom	$029DDC,$16
Scr2_029DF2:	; a2-VM script by its format: 34 of 133 words are a2 handler entry addresses; never run in the corpus
	include "src/f/029DF2.asm"
Scr2_029DFE:	; a2-VM script of Enemy_Jet_SpawnMissileLauncherB
	incrom	$029DFE,$AC
	include "src/f/029EAA.asm"
Scr2_029EBA:	; a2-VM script of Enemy_Missile_SpawnRandomTurn
	incrom	$029EBA,$42
	include "src/f/029EFC.asm"
Scr2_029F0C:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 029EFC
	incrom	$029F0C,$6E
	include "src/f/029F7A.asm"
Scr2_029F8A:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 029F7A
	incrom	$029F8A,$6E
	include "src/f/029FF8.asm"
Scr2_02A008:	; a2-VM script: read by the a2 VM's handlers through a2; 31% read in the corpus; referenced by 029FF8
	incrom	$02A008,$6A
	include "src/f/02A072.asm"
Scr2_02A082:	; a2-VM script of Enemy_BlackDrone_SpawnHoverBelow
	incrom	$02A082,$5A
	include "src/f/02A0DC.asm"
Scr2_02A0EC:	; a2-VM script of Enemy_BlackDrone_SpawnHoverAbove
	incrom	$02A0EC,$5A
	include "src/f/02A146.asm"
Scr2_02A156:	; a2-VM script: read by the a2 VM's handlers through a2; 96% read in the corpus; referenced by 02A146
	incrom	$02A156,$70
	include "src/f/02A1C6.asm"
Scr2_02A1D6:	; a2-VM script: read by the a2 VM's handlers through a2; 98% read in the corpus; referenced by 02A1C6
	incrom	$02A1D6,$A0
	include "src/f/02A276.asm"
Scr2_02A286:	; a2-VM script: read by the a2 VM's handlers through a2; 78% read in the corpus; referenced by 02A276
	incrom	$02A286,$64
	include "src/f/02A2EA.asm"
Scr2_02A2FA:	; a2-VM script: read by the a2 VM's handlers through a2; 96% read in the corpus; referenced by 02A2EA
	incrom	$02A2FA,$60
	include "src/f/02A35A.asm"
Scr2_02A36A:	; a2-VM script: read by the a2 VM's handlers through a2; 97% read in the corpus; referenced by 02A35A
	incrom	$02A36A,$90
Scr2_02A3FA:	; a2-VM script by its format: 74 of 250 words are a2 handler entry addresses; never run in the corpus
	include "src/f/02A3FA.asm"
Scr2_02A40A:	; a2-VM script of Enemy_BlackDrone_SpawnWaypointsE
	incrom	$02A40A,$80
	include "src/f/02A48A.asm"
Scr2_02A49A:	; a2-VM script of Enemy_BlackDrone_SpawnWaypointsF
	incrom	$02A49A,$5A
	include "src/f/02A4F4.asm"
Scr2_02A504:	; a2-VM script of Enemy_BlackDrone_SpawnWaypointsG
	incrom	$02A504,$80
	include "src/f/02A584.asm"
Scr2_02A594:	; a2-VM script of Enemy_BlackDrone_SpawnWaypointsH
	incrom	$02A594,$5A
	include "src/f/02A5EE.asm"
Scr2_02A5FE:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 02A5EE
	incrom	$02A5FE,$6E
	include "src/f/02A66C.asm"
Scr2_02A67C:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 02A66C
	incrom	$02A67C,$9C
Scr2_02A718:	; a2-VM script by its format: 96 of 319 words are a2 handler entry addresses; never run in the corpus
	include "src/f/02A718.asm"
Scr2_02A728:	; a2-VM script of Enemy_BlackDrone_SpawnHoverRandomA
	incrom	$02A728,$94
	include "src/f/02A7BC.asm"
Scr2_02A7CC:	; a2-VM script of Enemy_BlackDrone_SpawnHoverRandomB
	incrom	$02A7CC,$94
	include "src/f/02A860.asm"
Scr2_02A870:	; a2-VM script of Enemy_BlackDrone_SpawnHoverRandomC
	incrom	$02A870,$94
	include "src/f/02A904.asm"
Scr2_02A914:	; a2-VM script of Enemy_RedDrone_SpawnTrackYB
	incrom	$02A914,$82
	include "src/f/02A996.asm"
Scr2_02A9A6:	; a2-VM script: read by the a2 VM's handlers through a2; 94% read in the corpus; referenced by 02A996
	incrom	$02A9A6,$82
	include "src/f/02AA28.asm"
Scr2_02AA38:	; a2-VM script: read by the a2 VM's handlers through a2; 93% read in the corpus; referenced by 02AA28
	incrom	$02AA38,$70
	include "src/f/02AAA8.asm"
Scr2_02AAB8:	; a2-VM script: read by the a2 VM's handlers through a2; 71% read in the corpus; referenced by 02AAA8
	incrom	$02AAB8,$90
	include "src/f/02AB48.asm"
Scr2_02AB58:	; a2-VM script: read by the a2 VM's handlers through a2; 96% read in the corpus; referenced by 02AB48
	incrom	$02AB58,$6A
	include "src/f/02ABC2.asm"
Scr2_02ABD2:	; a2-VM script: read by the a2 VM's handlers through a2; 61% read in the corpus; referenced by 02ABC2
	incrom	$02ABD2,$10E
	include "src/f/02ACE0.asm"
Scr2_02ACF0:	; a2-VM script: read by the a2 VM's handlers through a2; 35% read in the corpus; referenced by 02ACE0
	incrom	$02ACF0,$14C
	include "src/f/02AE3C.asm"
Scr2_02AE4C:	; a2-VM script: read by the a2 VM's handlers through a2; 83% read in the corpus; referenced by 02AE3C
	incrom	$02AE4C,$10E
	include "src/f/02AF5A.asm"
Scr2_02AF6A:	; a2-VM script: read by the a2 VM's handlers through a2; 30% read in the corpus; referenced by 02AF5A
	incrom	$02AF6A,$134
	include "src/f/02B09E.asm"
Scr2_02B0AE:	; a2-VM script of Enemy_BlackDrone_SpawnCircuitC
	incrom	$02B0AE,$1DC
	include "src/f/02B28A.asm"
Scr2_02B29A:	; a2-VM script of Enemy_RedDrone_SpawnCircuitC
	incrom	$02B29A,$1F4
	include "src/f/02B48E.asm"
Scr2_02B49E:	; a2-VM script of Enemy_BlackDrone_SpawnCircuitD
	incrom	$02B49E,$1DC
	include "src/f/02B67A.asm"
Scr2_02B68A:	; a2-VM script: read by the a2 VM's handlers through a2; 58% read in the corpus; referenced by 02B67A
	incrom	$02B68A,$6E
	include "src/f/02B6F8.asm"
Scr2_02B708:	; a2-VM script: read by the a2 VM's handlers through a2; 48% read in the corpus; referenced by 02B6F8
	incrom	$02B708,$A4
	include "src/f/02B7AC.asm"
Scr2_02B7BC:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 02B7AC
	incrom	$02B7BC,$1A
Scr2_02B7D6:	; a2-VM script by its format: 85 of 307 words are a2 handler entry addresses; never run in the corpus
	include "src/f/02B7D6.asm"
Scr2_02B7E6:	; a2-VM script of Enemy_RedDrone_SpawnHoverRight
	incrom	$02B7E6,$9A
	include "src/f/02B880.asm"
Scr2_02B890:	; a2-VM script of Enemy_RedDrone_SpawnSweepA
	incrom	$02B890,$126
	include "src/f/02B9B6.asm"
Scr2_02B9C6:	; a2-VM script of Enemy_RedDrone_SpawnSweepB
	incrom	$02B9C6,$76
Scr2_02BA3C:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$02BA3C,$3E
	include "src/f/02BA7A.asm"
Scr2_02BA8A:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 02BA7A
	incrom	$02BA8A,$6C
	incrom	$02BAF6,$390
	include "src/f/02BE86.asm"
Scr2_02BE96:	; a2-VM script of Enemy_RedDrone_SpawnZigzag
	incrom	$02BE96,$18A
	include "src/f/02C020.asm"
Scr2_02C030:	; a2-VM script: read by the a2 VM's handlers through a2; 97% read in the corpus; referenced by 02C020
	incrom	$02C030,$9A
	include "src/f/02C0CA.asm"
Scr2_02C0DA:	; a2-VM script: read by the a2 VM's handlers through a2; 96% read in the corpus; referenced by 02C0CA
	incrom	$02C0DA,$D8
	include "src/f/02C1B2.asm"
Scr2_02C1C2:	; a2-VM script: read by the a2 VM's handlers through a2; 96% read in the corpus; referenced by 02C1B2
	incrom	$02C1C2,$D8
	include "src/f/02C29A.asm"
Scr2_02C2AA:	; a2-VM script: read by the a2 VM's handlers through a2; 96% read in the corpus; referenced by 02C29A
	incrom	$02C2AA,$D8
	include "src/f/02C382.asm"
Scr2_02C392:	; a2-VM script: read by the a2 VM's handlers through a2; 58% read in the corpus; referenced by 02C382
	incrom	$02C392,$64
	include "src/f/02C3F6.asm"
Scr2_02C406:	; a2-VM script: read by the a2 VM's handlers through a2; 58% read in the corpus; referenced by 02C3F6
	incrom	$02C406,$64
	include "src/f/02C46A.asm"
Scr2_02C47A:	; a2-VM script: read by the a2 VM's handlers through a2; 73% read in the corpus; referenced by 02C46A
	incrom	$02C47A,$9E
Scr2_02C518:	; a2-VM script by its format: 72 of 231 words are a2 handler entry addresses; never run in the corpus
	incrom	$02C518,$4
	include "src/f/02C51C.asm"
Scr2_02C52C:	; a2-VM script of Enemy_RedDrone_SpawnHoverAboveLevel
	incrom	$02C52C,$74
	include "src/f/02C5A0.asm"
Scr2_02C5B0:	; a2-VM script of Enemy_RedDrone_SpawnHoverLevelBelow
	incrom	$02C5B0,$74
	include "src/f/02C624.asm"
Scr2_02C634:	; a2-VM script of Enemy_RedDrone_SpawnHoverBelowAbove
	incrom	$02C634,$B2
	include "src/f/02C6E6.asm"
Scr2_02C6F2:	; a2-VM script: read by the a2 VM's handlers through a2; 82% read in the corpus; referenced by 02C6E6
	incrom	$02C6F2,$27C
	incrom	$02C96E,$2E4
	include "src/f/02CC52.asm"
Scr2_02CC5E:	; a2-VM script: read by the a2 VM's handlers through a2; 94% read in the corpus; referenced by 02CC52
	incrom	$02CC5E,$220
	include "src/f/02CE7E.asm"
Scr2_02CE8E:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 02CE7E
	incrom	$02CE8E,$16
	include "src/f/02CEA4.asm"
Scr2_02CEB4:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 02CEA4
	incrom	$02CEB4,$16
	include "src/f/02CECA.asm"
Scr2_02CEDA:	; a2-VM script: read by the a2 VM's handlers through a2; 85% read in the corpus; referenced by 02CECA
	incrom	$02CEDA,$1A
	include "src/f/02CEF4.asm"
Scr2_02CF04:	; a2-VM script: read by the a2 VM's handlers through a2; 85% read in the corpus; referenced by 02CEF4
	incrom	$02CF04,$C8
	include "src/f/02CFCC.asm"
Scr2_02CFDC:	; a2-VM script: read by the a2 VM's handlers through a2; 37% read in the corpus; referenced by 02CFCC
	incrom	$02CFDC,$16
	include "src/f/02CFF2.asm"
Scr2_02D002:	; a2-VM script of Enemy_Missile_SpawnStraightFast
	incrom	$02D002,$16
	include "src/f/02D018.asm"
Scr2_02D024:	; a2-VM script: read by the a2 VM's handlers through a2; 90% read in the corpus; referenced by 02D018
	incrom	$02D024,$17A
	include "src/f/02D19E.asm"
Scr2_02D1AA:	; a2-VM script: read by the a2 VM's handlers through a2; 97% read in the corpus; referenced by 02D19E
	incrom	$02D1AA,$12C
	include "src/f/02D2D6.asm"
Scr2_02D2E2:	; a2-VM script: read by the a2 VM's handlers through a2; 51% read in the corpus; referenced by 02D2D6
	incrom	$02D2E2,$126
	include "src/f/02D408.asm"
Scr2_02D414:	; a2-VM script of Enemy_DomeCraft_SpawnC
	incrom	$02D414,$82
	include "src/f/02D496.asm"
Scr2_02D4A2:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 02D496
	incrom	$02D4A2,$A8
	incrom	$02D54A,$116
	include "src/f/02D660.asm"
Scr2_02D66C:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 02D660
	incrom	$02D66C,$F6
	include "src/f/02D762.asm"
Scr2_02D76E:	; a2-VM script: read by the a2 VM's handlers through a2; 70% read in the corpus; referenced by 02D762
	incrom	$02D76E,$118
	include "src/f/02D886.asm"
Scr2_02D892:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 02D886
	incrom	$02D892,$EC
	include "src/f/02D97E.asm"
Scr2_02D98A:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 02D97E
	incrom	$02D98A,$EC
	include "src/f/02DA76.asm"
Scr2_02DA82:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 02DA76
	incrom	$02DA82,$10
	include "src/f/02DA92.asm"
Scr2_02DA9E:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 02DA92
	incrom	$02DA9E,$CC
	include "src/f/02DB6A.asm"
Scr2_02DB76:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 02DB6A
	incrom	$02DB76,$19E
	include "src/f/02DD14.asm"
Scr2_02DD20:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 02DD14
	incrom	$02DD20,$E2
	include "src/f/02DE02.asm"
Scr2_02DE0E:	; a2-VM script: read by the a2 VM's handlers through a2; 58% read in the corpus; referenced by 02DE02
	incrom	$02DE0E,$186
Scr2_02DF94:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$02DF94,$68
Scr2_02DFFC:	; a2-VM script by its format: 123 of 342 words are a2 handler entry addresses; never run in the corpus
	incrom	$02DFFC,$286
	include "src/f/02E282.asm"
Scr2_02E292:	; a2-VM script of Enemy_Missile_SpawnFromPodC
	incrom	$02E292,$16
Scr2_02E2A8:	; a2-VM script: read by the a2 VM's handlers through a2; 100% read in the corpus
	incrom	$02E2A8,$2FE
Scr2_02E5A6:	; a2-VM script by its format: 104 of 414 words are a2 handler entry addresses; never run in the corpus
	incrom	$02E5A6,$33C
Scr2_02E8E2:	; a2-VM script: read by the a2 VM's handlers through a2; 33% read in the corpus
	incrom	$02E8E2,$13C
	include "src/f/02EA1E.asm"
Scr2_02EA2E:	; a2-VM script: read by the a2 VM's handlers through a2; 35% read in the corpus; referenced by 02EA1E
	incrom	$02EA2E,$60A
	include "src/f/02F038.asm"
Scr2_02F044:	; a2-VM script: read by the a2 VM's handlers through a2; 91% read in the corpus; referenced by 02F038
	incrom	$02F044,$16
	include "src/f/02F05A.asm"
Scr2_02F066:	; a2-VM script: read by the a2 VM's handlers through a2; 91% read in the corpus; referenced by 02F05A
	incrom	$02F066,$16
	include "src/f/02F07C.asm"
Scr2_02F088:	; a2-VM script: read by the a2 VM's handlers through a2; 67% read in the corpus; referenced by 02F07C
	incrom	$02F088,$48C
	include "src/f/02F514.asm"
Scr2_02F524:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 02F514
	incrom	$02F524,$18
	include "src/f/02F53C.asm"
Scr2_02F54C:	; a2-VM script: read by the a2 VM's handlers through a2; 46% read in the corpus; referenced by 02F53C
	incrom	$02F54C,$34
	include "src/f/02F580.asm"
	include "src/f/02F58C.asm"
	include "src/f/02F5DA.asm"
	include "src/f/02F5F4.asm"
Tbl_02F600:	; read by VM2_RunObject (pc $0007E0); referenced by 02F5F4
	incrom	$02F600,$2
	include "src/f/02F602.asm"
Scr2_02F60A:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$02F60A,$4
	include "src/f/02F60E.asm"
Scr2_02F644:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$02F644,$5A
	include "src/f/02F69E.asm"
Scr2_02F6AA:	; a2-VM script: read by the a2 VM's handlers through a2; 33% read in the corpus; referenced by 02F69E
	incrom	$02F6AA,$24
	include "src/f/02F6CE.asm"
Scr2_02F6DA:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 02F6CE
	incrom	$02F6DA,$C
	include "src/f/02F6E6.asm"
Scr2_02F6F2:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 02F6E6
	incrom	$02F6F2,$C
	include "src/f/02F6FE.asm"
Scr2_02F70A:	; a2-VM script: read by the a2 VM's handlers through a2; 33% read in the corpus; referenced by 02F6FE
	incrom	$02F70A,$C
	include "src/f/02F716.asm"
Scr2_02F722:	; a2-VM script of Flight_SpawnGreenNavLightFlip
	incrom	$02F722,$C
	include "src/f/02F72E.asm"
Scr2_02F73A:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 02F72E
	incrom	$02F73A,$2BE
	include "src/f/02F9F8.asm"
Scr2_02FA0C:	; a2-VM script: read by the a2 VM's handlers through a2; 100% read in the corpus
	incrom	$02FA0C,$5F4
Scr2_030000:	; a2-VM script: read by the a2 VM's handlers through a2; 100% read in the corpus
	incrom	$030000,$3EC
	include "src/f/0303EC.asm"
Scr2_030416:	; a2-VM script: read by the a2 VM's handlers through a2; 99% read in the corpus
	incrom	$030416,$3AC
Tbl_0307C2:	; read by Trigger_Fire (pc $012E3C), Trigger_LoadX (pc $012DF4), Spawn_SetTimer0FromEntry (pc $027C92); 87% read in the corpus; referenced by 02F60E
	incrom	$0307C2,$826
	include "src/f/030FE8.asm"
Scr2_030FF4:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 030FE8
	incrom	$030FF4,$74
Scr2_031068:	; a2-VM script by its format: 75 of 231 words are a2 handler entry addresses; never run in the corpus
	incrom	$031068,$1CE
	include "src/f/031236.asm"
Scr2_031242:	; a2-VM script: read by the a2 VM's handlers through a2; 47% read in the corpus; referenced by 031236
	incrom	$031242,$BA
	include "src/f/0312FC.asm"
Scr2_03130E:	; a2-VM script: read by the a2 VM's handlers through a2; 59% read in the corpus; referenced by 0312FC
	incrom	$03130E,$144
Scr4_031452:	; a4-VM script by its format: 47 of 158 words are a4 handler entry addresses; never run in the corpus
	incrom	$031452,$13C
	include "src/f/03158E.asm"
Scr2_0315A0:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 03158E
	incrom	$0315A0,$4E
	include "src/f/0315EE.asm"
Scr2_031600:	; a2-VM script: read by the a2 VM's handlers through a2; 47% read in the corpus; referenced by 0315EE
	incrom	$031600,$5E
	include "src/f/03165E.asm"
Scr2_031670:	; a2-VM script: read by the a2 VM's handlers through a2; 42% read in the corpus; referenced by 03165E
	incrom	$031670,$2E
	include "src/f/03169E.asm"
Scr2_0316B0:	; a2-VM script of Enemy_Missile_SpawnAgileSlow
	incrom	$0316B0,$2E
	include "src/f/0316DE.asm"
Scr2_0316F0:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 0316DE
	incrom	$0316F0,$30
	include "src/f/031720.asm"
Scr2_031732:	; a2-VM script: read by the a2 VM's handlers through a2; 32% read in the corpus; referenced by 031720
	incrom	$031732,$2E
	include "src/f/031760.asm"
Scr2_031772:	; a2-VM script of Enemy_Missile_SpawnTurning
	incrom	$031772,$1E
	include "src/f/031790.asm"
Scr2_0317A2:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 031790
	incrom	$0317A2,$46
	include "src/f/0317E8.asm"
Scr2_0317FA:	; a2-VM script: read by the a2 VM's handlers through a2; 76% read in the corpus; referenced by 0317E8
	incrom	$0317FA,$98
	include "src/f/031892.asm"
Scr2_03189E:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 031892
	incrom	$03189E,$B2
	include "src/f/031950.asm"
Scr2_03195C:	; a2-VM script: read by the a2 VM's handlers through a2; 88% read in the corpus; referenced by 031950
	incrom	$03195C,$108
	include "src/f/031A64.asm"
Scr2_031A70:	; a2-VM script: read by the a2 VM's handlers through a2; 85% read in the corpus; referenced by 031A64
	incrom	$031A70,$D2
	include "src/f/031B42.asm"
Scr2_031B4E:	; a2-VM script: read by the a2 VM's handlers through a2; 72% read in the corpus; referenced by 031B42
	incrom	$031B4E,$1E2
	include "src/f/031D30.asm"
Scr2_031D3C:	; a2-VM script: read by the a2 VM's handlers through a2; 48% read in the corpus; referenced by 031D30
	incrom	$031D3C,$288
Scr2_031FC4:	; a2-VM script by its format: 54 of 141 words are a2 handler entry addresses; never run in the corpus
	incrom	$031FC4,$11A
	include "src/f/0320DE.asm"
Scr2_0320EA:	; a2-VM script: read by the a2 VM's handlers through a2; 92% read in the corpus; referenced by 0320DE
	incrom	$0320EA,$2B4
Scr2_03239E:	; a2-VM script by its format: 47 of 176 words are a2 handler entry addresses; never run in the corpus
	incrom	$03239E,$160
Scr2_0324FE:	; a2-VM script: read by the a2 VM's handlers through a2; 48% read in the corpus
	incrom	$0324FE,$300
	include "src/f/0327FE.asm"
Scr2_03280A:	; a2-VM script: read by the a2 VM's handlers through a2; 48% read in the corpus; referenced by 0327FE
	incrom	$03280A,$2A0
	include "src/f/032AAA.asm"
Scr2_032AB6:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 032AAA
	incrom	$032AB6,$C
	include "src/f/032AC2.asm"
Scr2_032ACE:	; a2-VM script: read by the a2 VM's handlers through a2; 81% read in the corpus; referenced by 032AC2
	incrom	$032ACE,$280
	include "src/f/032D4E.asm"
Scr2_032D5A:	; a2-VM script: read by the a2 VM's handlers through a2; 90% read in the corpus; referenced by 032D4E
	incrom	$032D5A,$54
Scr6_032DAE:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 032F14
	incrom	$032DAE,$20
Scr6_032DCE:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 033274
	incrom	$032DCE,$40
Scr6_032E0E:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 032F14
	incrom	$032E0E,$3C
Scr6_032E4A:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 032F14
	incrom	$032E4A,$CA
	include "src/f/032F14.asm"
Tbl_032FE2:	; read by Scene_InitStage (pc $0099D8), Scene_InitCommon (pc $009B2C), Section_SetIdInline (pc $00772E); referenced by 033274
	incrom	$032FE2,$3C
Tbl_03301E:	; read by Scene_InitStage (pc $0099D8), Scene_InitCommon (pc $009B2C), Section_SetIdInline (pc $00772E); referenced by 03324A
	incrom	$03301E,$DE
Tbl_0330FC:	; read by VM6_Next (pc $0070B4), Op6_QueueDMAEntry (pc $007688), Op6_DecompressBg (pc $007126); referenced by 033402
	incrom	$0330FC,$24
Scr6_033120:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 03325A
	incrom	$033120,$62
Scr6_033182:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 03325A
	incrom	$033182,$6E
Scr6_0331F0:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 03325A
	incrom	$0331F0,$5A
	include "src/f/03324A.asm"
	include "src/f/03325A.asm"
	include "src/f/033274.asm"
	include "src/f/033322.asm"
	include "src/f/03334A.asm"
	include "src/f/033382.asm"
	include "src/f/033398.asm"
Tbl_0333F2:	; read by ScrollCmd_LayerA (pc $009826), Scroll_NextCmd (pc $0097F6), Scroll_FillBands (pc $009930); referenced by 033398
	incrom	$0333F2,$10
	include "src/f/033402.asm"
	include "src/f/033448.asm"
	include "src/f/033490.asm"
	include "src/f/0334B4.asm"
	include "src/f/0334D2.asm"
Scr2_033BB8:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 0334D2
	incrom	$033BB8,$20
	include "src/f/033BD8.asm"
Scr2_033BF4:	; a2-VM script: read by the a2 VM's handlers through a2; 99% read in the corpus
	incrom	$033BF4,$CA
Tbl_033CBE:	; read by Trigger_LoadX (pc $012DF4), Trigger_Fire (pc $012E3C), Enemy_BombDropper_Spawn (pc $036044); 89% read in the corpus; referenced by 03FE94
	incrom	$033CBE,$262
Tbl_033F20:	; read by Trigger_LoadX (pc $012DD0), Trigger_Fire (pc $012E3C), Wave_StartFromEntry (pc $00932C); 88% read in the corpus; referenced by 03FFBE
	incrom	$033F20,$10
Tbl_033F30:	; read by Trigger_LoadX (pc $012DF4), Trigger_Fire (pc $012E3C), Wave_StartFromEntry (pc $00932C); 87% read in the corpus; referenced by 0400E4
	incrom	$033F30,$1FE
Tbl_03412E:	; read by Trigger_LoadX (pc $012DD0), Trigger_Fire (pc $012E3C), Wave_StartFromEntry (pc $00932C); 90% read in the corpus; referenced by 0401DA
	incrom	$03412E,$14
Tbl_034142:	; read by Trigger_LoadX (pc $012DF4), VM5_Next (pc $006A56), VM5_SpawnEntry (pc $006FEC); 61% read in the corpus; referenced by 0403F6
	incrom	$034142,$7F0
Scr5_034932:	; a5-VM script: read by the a5 VM's handlers through a5; 62% read in the corpus; referenced by 040574
	incrom	$034932,$998
Scr5_0352CA:	; a5-VM script by its format: 111 of 321 words are a5 handler entry addresses; never run in the corpus
	incrom	$0352CA,$282
Scr2_03554C:	; a2-VM script: read by the a2 VM's handlers through a2; 90% read in the corpus
	incrom	$03554C,$1F4
	include "src/f/035740.asm"
Scr4_03575E:	; a4-VM script: read by the a4 VM's handlers through a4; 99% read in the corpus
	incrom	$03575E,$38A
	include "src/f/035AE8.asm"
Scr2_035AF6:	; a2-VM script: read by the a2 VM's handlers through a2; 95% read in the corpus
	incrom	$035AF6,$1C2
	include "src/f/035CB8.asm"
Scr2_035CC4:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 035CB8
	incrom	$035CC4,$2E
	include "src/f/035CF2.asm"
Scr2_035CFE:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 035CF2
	incrom	$035CFE,$2E
	include "src/f/035D2C.asm"
Scr2_035D38:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 035D2C
	incrom	$035D38,$30
	include "src/f/035D68.asm"
Scr2_035D74:	; a2-VM script: read by the a2 VM's handlers through a2; 93% read in the corpus; referenced by 035D68
	incrom	$035D74,$118
	include "src/f/035E8C.asm"
Scr2_035E96:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$035E96,$2
	include "src/f/035E98.asm"
Scr2_035EA2:	; a2-VM script: read by the a2 VM's handlers through a2; 98% read in the corpus
	incrom	$035EA2,$5A
	include "src/f/035EFC.asm"
Scr4_035F1E:	; a4-VM script: read by the a4 VM's handlers through a4
	incrom	$035F1E,$11A
	include "src/f/036038.asm"
Scr2_036050:	; a2-VM script: read by the a2 VM's handlers through a2; 98% read in the corpus; referenced by 036038
	incrom	$036050,$F6
	include "src/f/036146.asm"
	include "src/f/03619C.asm"
Scr2_0361A8:	; a2-VM script: read by the a2 VM's handlers through a2; 98% read in the corpus; referenced by 03619C
	incrom	$0361A8,$54
	include "src/f/0361FC.asm"
Scr4_036210:	; a4-VM script: read by the a4 VM's handlers through a4; 93% read in the corpus
	incrom	$036210,$206
	include "src/f/036416.asm"
Scr2_036422:	; a2-VM script of Enemy_PuffPlant_SpawnH
	incrom	$036422,$1A
	include "src/f/03643C.asm"
Scr2_036448:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 03643C
	incrom	$036448,$1A
	include "src/f/036462.asm"
Scr2_03646E:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 036462
	incrom	$03646E,$20
	include "src/f/03648E.asm"
Scr2_03649A:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 03648E
	incrom	$03649A,$20
	include "src/f/0364BA.asm"
Scr2_0364C6:	; a2-VM script: read by the a2 VM's handlers through a2; 41% read in the corpus; referenced by 0364BA
	incrom	$0364C6,$40
	include "src/f/036506.asm"
Scr2_036512:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 036506
	incrom	$036512,$20
	include "src/f/036532.asm"
Scr2_03653E:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 036532
	incrom	$03653E,$1A
	include "src/f/036558.asm"
Scr2_036564:	; a2-VM script: read by the a2 VM's handlers through a2; 39% read in the corpus; referenced by 036558
	incrom	$036564,$C4
	include "src/f/036628.asm"
Scr2_036634:	; a2-VM script of Enemy_RabbitPot_SpawnE
	incrom	$036634,$28
	include "src/f/03665C.asm"
Scr2_036668:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 03665C
	incrom	$036668,$26
	include "src/f/03668E.asm"
Scr2_03669A:	; a2-VM script: read by the a2 VM's handlers through a2; 51% read in the corpus; referenced by 03668E
	incrom	$03669A,$4A
	include "src/f/0366E4.asm"
Scr2_0366F0:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 0366E4
	incrom	$0366F0,$18
	include "src/f/036708.asm"
Scr2_036714:	; a2-VM script: read by the a2 VM's handlers through a2; 58% read in the corpus; referenced by 036708
	incrom	$036714,$220
	include "src/f/036934.asm"
Scr2_036940:	; a2-VM script of Enemy_DropCarrier_SpawnRabbitsB
	incrom	$036940,$28
	include "src/f/036968.asm"
Scr2_036974:	; a2-VM script of Enemy_DropCarrier_SpawnRabbitsC
	incrom	$036974,$28
	include "src/f/03699C.asm"
Scr2_0369A8:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 03699C
	incrom	$0369A8,$28
	include "src/f/0369D0.asm"
Scr2_0369DC:	; a2-VM script: read by the a2 VM's handlers through a2; 52% read in the corpus; referenced by 0369D0
	incrom	$0369DC,$2F8
	include "src/f/036CD4.asm"
Tbl_036DA8:	; read by ToyFigure_DrawIndex (pc $036D7E), ToyFigure_FxSpinOrKill (pc $03711C), Boss_Hatter_ThrownToyFrame (pc $03DA50); referenced by 036CD4, 0370DC, 03730E, 037708 ...
	incrom	$036DA8,$F0
Tbl_ToyFigureChargedFrames:	; 60-rotation frame table of the toy figure with the outlined 'charged' frames, a copy of Tbl_036DA8's layout (worker 25); read by the a4 figure-draw ops; datamap first took it as the head of an a4 script
	incrom	$036E98,$F0
Scr4_036F88:	; a4-VM script(s) after the charged-frame table (the rest of the span datamap first labelled as one a4 script at $036E98); read by the a4 VM's handlers through a4
	incrom	$036F88,$154
	include "src/f/0370DC.asm"
	include "src/f/0371E6.asm"
	include "src/f/037204.asm"
	include "src/f/037260.asm"
	include "src/f/0372B8.asm"
	include "src/f/0372C0.asm"
	include "src/f/03730E.asm"
	include "src/f/03747C.asm"
	include "src/f/0374C2.asm"
	include "src/f/03750C.asm"
Scr2_037588:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$037588,$6E
	include "src/f/0375F6.asm"
Scr2_037604:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$037604,$88
	include "src/f/03768C.asm"
	include "src/f/037708.asm"
Scr2_0379BC:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$0379BC,$AE
	include "src/f/037A6A.asm"
Scr2_037B12:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$037B12,$A2
	include "src/f/037BB4.asm"
Scr2_037BC4:	; a2-VM script: read by the a2 VM's handlers through a2; 70% read in the corpus
	incrom	$037BC4,$9D8
Scr2_03859C:	; a2-VM script by its format: 56 of 152 words are a2 handler entry addresses; never run in the corpus
	incrom	$03859C,$130
Scr2_0386CC:	; a2-VM script: read by the a2 VM's handlers through a2; 34% read in the corpus
	incrom	$0386CC,$42C
	include "src/f/038AF8.asm"
Scr2_038B04:	; a2-VM script of ToyFigure_SpawnB: Op2_SetSpeedRand $33 $E6, StartCoroutine3Rel $038074, StartCoroutine2Rel $038B40, Goto $036BCE (the figure body)
	incrom	$038B04,$12
Scr2_038B16:	; toy-figure spawner stub $038B16 (lea $038B22 / jmp Obj_SpawnEnemy; no reference found) and a2-VM scripts, part of Scr2_0386CC's span until 2026-10-30
	incrom	$038B16,$6A
	include "src/f/038B80.asm"
Scr2_038B8C:	; a2-VM script of ToyFigure_SpawnC: Op2_SetSpeedRand $33 $E6, StartCoroutine3Rel $0387DA (the second charge schedule), StartCoroutine2Rel $038D9A (ToyFigure_SpawnRandCharge's pause-then-random-hop), Goto $036BCE
	incrom	$038B8C,$12
	include "src/f/038B9E.asm"
Scr2_038BAA:	; a2-VM script: read by the a2 VM's handlers through a2; 16% read in the corpus; referenced by 038B9E
	incrom	$038BAA,$12
	include "src/f/038BBC.asm"
Scr2_038BC8:	; a2-VM script of ToyFigure_SpawnD: Op2_SetSpeedRand $33 $E6, StartCoroutine3Rel $03811E, StartCoroutine2Rel $038DD8, Goto $036BCE
	incrom	$038BC8,$12
	include "src/f/038BDA.asm"
Scr2_038BE6:	; a2-VM script of ToyFigure_SpawnE: Op2_SetSpeedRand $33 $E6, StartCoroutine3Rel $03811E, StartCoroutine2Rel $038DE2, Goto $036BCE
	incrom	$038BE6,$12
Scr2_038BF8:	; toy-figure spawner stub $038BF8 (lea $038C04 / jmp Obj_SpawnEnemy; no reference found) and its a2-VM script, part of Scr2_038BAA's span until 2026-10-30
	incrom	$038BF8,$22
	include "src/f/038C1A.asm"
Scr2_038C26:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 038C1A
	incrom	$038C26,$60
	include "src/f/038C86.asm"
Scr2_038C92:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 038C86
	incrom	$038C92,$12
	include "src/f/038CA4.asm"
Scr2_038CB0:	; a2-VM script: read by the a2 VM's handlers through a2; 52% read in the corpus; referenced by 038CA4
	incrom	$038CB0,$44
	include "src/f/038CF4.asm"
Scr2_038D00:	; a2-VM script of ToyFigure_SpawnF: Op2_SetSpeed $100, StartCoroutine3Rel $0380B8 (the standard charge cycle), StartCoroutine2Rel $038D10, Goto $036BCE
	incrom	$038D00,$10
Scr2_038D10:	; a2-VM script: ToyFigure_SpawnF's hop coroutine (its StartCoroutine2Rel), part of Scr2_038CB0's span until 2026-10-30
	incrom	$038D10,$38
	include "src/f/038D48.asm"
Scr2_038D54:	; a2-VM script of ToyFigure_SpawnG: Op2_SetSpeed $100, StartCoroutine3Rel $0380B8 (the standard charge cycle), StartCoroutine2Rel $038D64, Goto $036BCE
	incrom	$038D54,$10
Scr2_038D64:	; a2-VM scripts: ToyFigure_SpawnG's hop coroutine $038D64 and the toy figures' coroutines after it ($038D9A, $038DD8, $038DE2 among them), part of Scr2_038CB0's span until 2026-10-30
	incrom	$038D64,$2B4
	include "src/f/039018.asm"
Scr2_039028:	; a2-VM script of ToyRing_SpawnBigB: CallNativeRel ToyRing_Init (16 figures, radius $6E, stiffness $40, 3, $CCCC: Scr2_03905E's ring), StartCoroutine2Rel $03958C, StartCoroutine3Rel $0395D2, StartCoroutine1Rel $039630, SetScoreValue $12C, SetEnergy $200 (ToyRing_SpawnBig: $3E8 and $400), Goto $037586
	incrom	$039028,$26
	include "src/f/03904E.asm"
Scr2_03905E:	; a2-VM script: read by the a2 VM's handlers through a2; 15% read in the corpus; referenced by 03904E
	incrom	$03905E,$118
	include "src/f/039176.asm"
Scr2_039186:	; a2-VM script: read by the a2 VM's handlers through a2; 38% read in the corpus; referenced by 039176
	incrom	$039186,$24
	include "src/f/0391AA.asm"
Scr2_0391BA:	; a2-VM script of ToyRing_SpawnSixB: CallNativeRel ToyRing_Init (6 figures, radius $32, stiffness $60, 1, $9248), StartCoroutine2Rel $0393CA (stop, hop in place, then bouts of 3 hops rolling at cruise x $2AA), StartCoroutine3Rel $0380B8 (charge cycle), StartCoroutine1Rel $039612 (break-up), SetScoreValue $12C, SetEnergy $C0, SetTimer1 $1A40, Goto $037588; as Scr2_0391F4 but no SetSpawnAction (drops nothing) and its own coroutine 2
	incrom	$0391BA,$2A
	include "src/f/0391E4.asm"
Scr2_0391F4:	; a2-VM script: read by the a2 VM's handlers through a2; 44% read in the corpus; referenced by 0391E4
	incrom	$0391F4,$68
	include "src/f/03925C.asm"
Scr2_03926C:	; a2-VM script: read by the a2 VM's handlers through a2; 32% read in the corpus; referenced by 03925C
	incrom	$03926C,$24
	include "src/f/039290.asm"
Scr2_0392A0:	; a2-VM script of ToyRing_SpawnSixC: CallNativeRel ToyRing_Init (6 figures, radius $32, stiffness $80, 1, $9248), StartCoroutine2Rel $03952C, StartCoroutine3Rel $0380A8, StartCoroutine1Rel $039612 (break-up), SetScoreValue $12C, SetEnergy $C0, the op at $000F88 ($0780, $0780), Goto
	incrom	$0392A0,$2C
Scr2_0392CC:	; a2-VM scripts of the toy rings, part of Scr2_03926C's span until 2026-10-30: $0392CC (Scr2_039186's Op2_Gosub: Op2_SwitchRandom over four ToyRing_Init calls), then the rings' coroutines ($039378, $0393CA, $03941E, $03952C, $03958C, $0395D2, $039612, $039630 among them)
	incrom	$0392CC,$3D2
	include "src/f/03969E.asm"
Scr2_0396AA:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 03969E
	incrom	$0396AA,$18
	include "src/f/0396C2.asm"
Scr2_0396CE:	; a2-VM script: read by the a2 VM's handlers through a2; 12% read in the corpus; referenced by 0396C2
	incrom	$0396CE,$18
	include "src/f/0396E6.asm"
Scr2_0396F2:	; a2-VM script of ToyChain_Spawn10C: Op2_SpawnChild $2A ToyChain_MakeChild10, StartChildCoroutine3Rel $2A $038578 (charge), StartChildCoroutine0Rel $2A $038788 (shedding), Goto the base loop Scr2_0379BC
	incrom	$0396F2,$18
	include "src/f/03970A.asm"
Scr2_039716:	; a2-VM script of ToyChain_Spawn10D: the words of Scr2_0396F2 (child ToyChain_MakeChild10, charge $038578, shedding $038788), its own copy
	incrom	$039716,$18
	include "src/f/03972E.asm"
Scr2_03973A:	; a2-VM script of ToyChain_Spawn10E: Op2_SpawnChild $2A ToyChain_MakeChild10, charge $038596, shedding $0387CC, Goto Scr2_0379BC. After it, still raw: the base stub $039752 (no static reference) and its script $03975E (child $039A88)
	incrom	$03973A,$18
	incrom	$039752,$24
	include "src/f/039776.asm"
Scr2_039782:	; a2-VM script of ToyChain_Spawn7C: Op2_SpawnChild $2A ToyChain_MakeChild7C, charge $0385B4, shedding $0385F0, Goto Scr2_0379BC
	incrom	$039782,$18
	include "src/f/03979A.asm"
Scr2_0397A6:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 03979A
	incrom	$0397A6,$18
	include "src/f/0397BE.asm"
Scr2_0397CA:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 0397BE
	incrom	$0397CA,$18
	include "src/f/0397E2.asm"
Scr2_0397EE:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 0397E2
	incrom	$0397EE,$18
	include "src/f/039806.asm"
Scr2_039812:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 039806
	incrom	$039812,$18
	include "src/f/03982A.asm"
Scr2_039836:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 03982A
	incrom	$039836,$18
	include "src/f/03984E.asm"
Scr2_03985A:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 03984E
	incrom	$03985A,$18
	include "src/f/039872.asm"
Scr2_03987E:	; a2-VM script: read by the a2 VM's handlers through a2; 40% read in the corpus; referenced by 039872
	incrom	$03987E,$18
	include "src/f/039896.asm"
Scr2_0398A2:	; a2-VM script of ToyChain_Spawn8B: Op2_SpawnChild $2A ToyChain_MakeChild8B, charge $0380D8, shedding $03870C (ToyChain_Spawn5B-7B's fixed-delay schedules), Goto Scr2_0379BC
	incrom	$0398A2,$18
	include "src/f/0398BA.asm"
Scr2_0398BE:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$0398BE,$1E
	include "src/f/0398DC.asm"
Scr2_0398E0:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$0398E0,$1E
	include "src/f/0398FE.asm"
Scr2_039902:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$039902,$1E
	include "src/f/039920.asm"
Scr2_039924:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$039924,$1E
	include "src/f/039942.asm"
Scr2_039946:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$039946,$1E
	include "src/f/039964.asm"
Scr2_039968:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$039968,$1E
	include "src/f/039986.asm"
Scr2_03998A:	; a2-VM script: read by the a2 VM's handlers through a2; 23% read in the corpus
	incrom	$03998A,$1C
	include "src/f/0399A6.asm"
Scr2_0399AA:	; ToyChain_MakeChild8B's inline arguments (dc.w 8, dc.l $039AC4, dc.l $0399B4) and its chain script $0399B4 (fixed speed $100, cruise $255, $17AC $255 $50, Goto the chain body Scr2_0379DA). After it, still raw: the makers $0399C6 and $0399E6 (10 links)
	incrom	$0399AA,$1C
	include "src/f/0399C6.asm"
Scr2_0399CA:	; inline arguments of ToyChain_MakeChild10B (dc.w links, dc.l link table, dc.l chain script), then its chain script $0399D4, and at $0399E6 another inline maker (jsr ToyChain_SpawnInline + 10 links, table $039AE0) that nothing names
	incrom	$0399CA,$3C
	include "src/f/039A06.asm"
Tbl_039A0A:	; read by ToyChain_SpawnInline (pc $037176), ToyChain_SpawnInline (pc $03715C), Op2_SetCruiseX (pc $0017B8); 23% read in the corpus
	incrom	$039A0A,$5C
	include "src/f/039A66.asm"
Scr2_039A6A:	; ToyChain_MakeChild7C's inline arguments (dc.w 7, dc.l $039AE0, dc.l $039A74) and its chain script $039A74 (random speed $4C-$100, cruise $155, $17AC $155 $78, Goto the chain body Scr2_0379DA)
	incrom	$039A6A,$1E
	include "src/f/039A88.asm"
Scr2_039A8C:	; inline arguments of ToyChain_MakeChild5C (dc.w links, dc.l link table, dc.l chain script), then its chain script
	incrom	$039A8C,$1C
Tbl_039AA8:	; toy-chain parameter records, 6 x $1C bytes, ToyChain_SpawnInline's a1 (words into the chain object: +0 -> $22(a4), +4 speed, +$A/+$C x256 accelerations, +$E link energy ($20/$90/$200/$90/$1000/$100), ...): $039AC4 for ToyChain_MakeChild7B/8B, $039AE0 for ToyChain_MakeChild7C, $039AFC for ToyChain_MakeChild10, the rest for the raw makers $0399C6 ($039AA8), $039A46 ($039B18), $039A88 ($039B34); $039AC4/$039AFC read by ToyChain_SpawnInline ($03715C) in the tas and stage-3 fuzz runs
	incrom	$039AA8,$A8
	incrom	$039B50,$28
	include "src/f/039B78.asm"
Scr2_039B84:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 039B78
	incrom	$039B84,$1C
	include "src/f/039BA0.asm"
Scr2_039BAC:	; a2-VM script: read by the a2 VM's handlers through a2; 65% read in the corpus; referenced by 039BA0
	incrom	$039BAC,$1A
	include "src/f/039BC6.asm"
Scr2_039BCA:	; inline arguments of ToyChain_MakeDropIn12B (dc.w links, dc.l link table, dc.l chain script), then its chain script
	incrom	$039BCA,$A
	include "src/f/039BD4.asm"
Tbl_039BD8:	; read by ToyChain_SpawnInline (pc $03715C)
	incrom	$039BD8,$A
	include "src/f/039BE2.asm"
Tbl_039BE6:	; read by ToyChain_SpawnInline (pc $03715C), ToyChain_SpawnInline (pc $037176); 33% read in the corpus
	incrom	$039BE6,$5E
	include "src/f/039C44.asm"
Scr2_039C50:	; a2-VM script of Enemy_GiftBox_SpawnStay10C
	incrom	$039C50,$5C
	include "src/f/039CAC.asm"
Scr2_039CB8:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 039CAC
	incrom	$039CB8,$54
	include "src/f/039D0C.asm"
Scr2_039D18:	; a2-VM script: read by the a2 VM's handlers through a2; 88% read in the corpus; referenced by 039D0C
	incrom	$039D18,$54
	include "src/f/039D6C.asm"
Scr2_039D78:	; a2-VM script: read by the a2 VM's handlers through a2; 51% read in the corpus; referenced by 039D6C
	incrom	$039D78,$C2
	include "src/f/039E3A.asm"
Scr2_039E46:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 039E3A
	incrom	$039E46,$52
	include "src/f/039E98.asm"
Scr2_039EA4:	; a2-VM script: read by the a2 VM's handlers through a2; 46% read in the corpus; referenced by 039E98
	incrom	$039EA4,$B2
	include "src/f/039F56.asm"
Scr2_039F62:	; a2-VM script: read by the a2 VM's handlers through a2; 95% read in the corpus; referenced by 039F56
	incrom	$039F62,$54
	include "src/f/039FB6.asm"
Scr2_039FC2:	; a2-VM script: read by the a2 VM's handlers through a2; 88% read in the corpus; referenced by 039FB6
	incrom	$039FC2,$54
	include "src/f/03A016.asm"
Scr2_03A022:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 03A016
	incrom	$03A022,$4E
	include "src/f/03A070.asm"
Scr2_03A07C:	; a2-VM script: read by the a2 VM's handlers through a2; 39% read in the corpus; referenced by 03A070
	incrom	$03A07C,$DA
	include "src/f/03A156.asm"
Scr2_03A162:	; a2-VM script: read by the a2 VM's handlers through a2; 87% read in the corpus; referenced by 03A156
	incrom	$03A162,$4E
	include "src/f/03A1B0.asm"
Scr2_03A1BC:	; a2-VM script: read by the a2 VM's handlers through a2; 87% read in the corpus; referenced by 03A1B0
	incrom	$03A1BC,$4E
	include "src/f/03A20A.asm"
Scr2_03A216:	; a2-VM script: read by the a2 VM's handlers through a2; 46% read in the corpus; referenced by 03A20A
	incrom	$03A216,$AA
	include "src/f/03A2C0.asm"
Scr2_03A2C4:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$03A2C4,$22
	include "src/f/03A2E6.asm"
Scr2_03A2EA:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$03A2EA,$16
	include "src/f/03A300.asm"
Scr2_03A304:	; a2-VM script: read by the a2 VM's handlers through a2; 47% read in the corpus
	incrom	$03A304,$22
	include "src/f/03A326.asm"
Scr2_03A32A:	; inline arguments of ToyChain_MakeBox10C (dc.w links, dc.l link table, dc.l chain script), then its chain script
	incrom	$03A32A,$22
	include "src/f/03A34C.asm"
Scr2_03A350:	; a2-VM script: read by the a2 VM's handlers through a2; 47% read in the corpus
	incrom	$03A350,$22
	include "src/f/03A372.asm"
Scr2_03A376:	; inline arguments of ToyChain_MakeBox10D (dc.w links, dc.l link table, dc.l chain script), then its chain script
	incrom	$03A376,$22
	include "src/f/03A398.asm"
Scr2_03A39C:	; a2-VM script: read by the a2 VM's handlers through a2; 31% read in the corpus
	incrom	$03A39C,$22
	include "src/f/03A3BE.asm"
Scr2_03A3C2:	; inline arguments of ToyChain_MakeBox4B (dc.w links, dc.l link table, dc.l chain script), then its chain script
	incrom	$03A3C2,$22
	include "src/f/03A3E4.asm"
Scr2_03A3E8:	; inline arguments of ToyChain_MakeBox4C (dc.w links, dc.l link table, dc.l chain script), then its chain script
	incrom	$03A3E8,$22
	include "src/f/03A40A.asm"
Scr2_03A40E:	; a2-VM script: read by the a2 VM's handlers through a2; 47% read in the corpus
	incrom	$03A40E,$22
	include "src/f/03A430.asm"
Scr2_03A434:	; inline arguments of ToyChain_MakeBox6 (dc.w links, dc.l link table, dc.l chain script), then its chain script
	incrom	$03A434,$22
	include "src/f/03A456.asm"
Tbl_03A45A:	; read by ToyChain_SpawnInline (pc $037176), Op2_AllocOsc (pc $005046), ToyChain_SpawnInline (pc $03715C); 65% read in the corpus
	incrom	$03A45A,$160
	include "src/f/03A5BA.asm"
Scr2_03A5C6:	; a2-VM script: read by the a2 VM's handlers through a2; 91% read in the corpus; referenced by 03A5BA
	incrom	$03A5C6,$D4
	include "src/f/03A69A.asm"
Scr2_03A6A6:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 03A69A
	incrom	$03A6A6,$30
	include "src/f/03A6D6.asm"
Scr2_03A6E2:	; a2-VM script: read by the a2 VM's handlers through a2; 85% read in the corpus; referenced by 03A6D6
	incrom	$03A6E2,$5C
	include "src/f/03A73E.asm"
Scr2_03A742:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$03A742,$2A
	include "src/f/03A76C.asm"
Scr2_03A778:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 03A76C
	incrom	$03A778,$50
	include "src/f/03A7C8.asm"
Scr2_03A7CC:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$03A7CC,$46
	include "src/f/03A812.asm"
Scr2_03A81E:	; a2-VM script: read by the a2 VM's handlers through a2; 88% read in the corpus; referenced by 03A812
	incrom	$03A81E,$76
	include "src/f/03A894.asm"
Scr2_03A898:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$03A898,$84
	include "src/f/03A91C.asm"
Scr2_03A928:	; a2-VM script: read by the a2 VM's handlers through a2; 34% read in the corpus; referenced by 03A91C
	incrom	$03A928,$2A
	include "src/f/03A952.asm"
Scr2_03A95E:	; a2-VM script of ToyFigure_SpawnShed
	incrom	$03A95E,$4A
	include "src/f/03A9A8.asm"
Scr2_03A9B4:	; a2-VM script: read by the a2 VM's handlers through a2; 90% read in the corpus; referenced by 03A9A8
	incrom	$03A9B4,$66
	include "src/f/03AA1A.asm"
Scr2_03AA26:	; a2-VM script: read by the a2 VM's handlers through a2; 90% read in the corpus; referenced by 03AA1A
	incrom	$03AA26,$62
	include "src/f/03AA88.asm"
Scr2_03AA94:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 03AA88
	incrom	$03AA94,$7A
Scr2_03AB0E:	; a2-VM script by its format: 62 of 170 words are a2 handler entry addresses; never run in the corpus
	incrom	$03AB0E,$10
	include "src/f/03AB1E.asm"
Scr2_03AB2A:	; a2-VM script of Enemy_PropToy_SpawnChain4Right
	incrom	$03AB2A,$52
	include "src/f/03AB7C.asm"
Scr2_03AB88:	; a2-VM script of Enemy_PropToy_SpawnTwinChain8
	incrom	$03AB88,$5A
	include "src/f/03ABE2.asm"
Scr2_03ABEE:	; a2-VM script of Enemy_PropToy_SpawnChain5RightC
	incrom	$03ABEE,$74
	include "src/f/03AC62.asm"
Scr2_03AC6E:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 03AC62
	incrom	$03AC6E,$14
	include "src/f/03AC82.asm"
Scr2_03AC8E:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 03AC82
	incrom	$03AC8E,$14
	include "src/f/03ACA2.asm"
Scr2_03ACAE:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 03ACA2
	incrom	$03ACAE,$10
	include "src/f/03ACBE.asm"
Scr2_03ACCA:	; a2-VM script: read by the a2 VM's handlers through a2; 35% read in the corpus; referenced by 03ACBE
	incrom	$03ACCA,$F0
	include "src/f/03ADBA.asm"
Scr2_03ADC6:	; a2-VM script of Enemy_PropToy_SpawnTwinChain6
	incrom	$03ADC6,$80
	include "src/f/03AE46.asm"
Scr2_03AE52:	; a2-VM script: read by the a2 VM's handlers through a2; 49% read in the corpus; referenced by 03AE46
	incrom	$03AE52,$68
	include "src/f/03AEBA.asm"
Scr2_03AEBE:	; inline arguments of ToyChain_MakeHanging5E (dc.w links, dc.l link table, dc.l chain script), then its chain script
	incrom	$03AEBE,$2C
	include "src/f/03AEEA.asm"
Scr2_03AEEE:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$03AEEE,$2C
	include "src/f/03AF1A.asm"
Scr2_03AF1E:	; a2-VM script: read by the a2 VM's handlers through a2; 32% read in the corpus
	incrom	$03AF1E,$2C
	include "src/f/03AF4A.asm"
Scr2_03AF4E:	; inline arguments of ToyChain_MakeHanging4 (dc.w links, dc.l link table, dc.l chain script), then its chain script
	incrom	$03AF4E,$2A
	include "src/f/03AF78.asm"
Scr2_03AF7C:	; inline arguments of ToyChain_MakeHanging8B (dc.w links, dc.l link table, dc.l chain script), then its chain script
	incrom	$03AF7C,$2C
	include "src/f/03AFA8.asm"
Scr2_03AFAC:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$03AFAC,$2C
	include "src/f/03AFD8.asm"
Scr2_03AFDC:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$03AFDC,$28
	include "src/f/03B004.asm"
Scr2_03B008:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$03B008,$2C
	include "src/f/03B034.asm"
Scr2_03B038:	; a2-VM script: read by the a2 VM's handlers through a2; 32% read in the corpus
	incrom	$03B038,$2C
	include "src/f/03B064.asm"
Scr2_03B068:	; inline arguments of ToyChain_MakeHanging6C (dc.w links, dc.l link table, dc.l chain script), then its chain script
	incrom	$03B068,$2A
	include "src/f/03B092.asm"
Scr2_03B096:	; inline arguments of ToyChain_MakeHanging6D (dc.w links, dc.l link table, dc.l chain script), then its chain script
	incrom	$03B096,$2A
	include "src/f/03B0C0.asm"
Scr2_03B0C4:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$03B0C4,$2A
	include "src/f/03B0EE.asm"
Scr2_03B0F2:	; a2-VM script: read by the a2 VM's handlers through a2; 77% read in the corpus
	incrom	$03B0F2,$16E
	include "src/f/03B260.asm"
Scr2_03B28C:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$03B28C,$4
	include "src/f/03B290.asm"
Scr2_03B294:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$03B294,$16
	include "src/f/03B2AA.asm"
Scr2_03B2BA:	; a2-VM script: read by the a2 VM's handlers through a2; 80% read in the corpus
	incrom	$03B2BA,$FE
	include "src/f/03B3B8.asm"
Scr2_03B3C4:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 03B3B8
	incrom	$03B3C4,$2E
	include "src/f/03B3F2.asm"
Scr2_03B3FE:	; a2-VM script: read by the a2 VM's handlers through a2; 99% read in the corpus; referenced by 03B3F2
	incrom	$03B3FE,$2FC
	include "src/f/03B6FA.asm"
Scr4_03B726:	; a4-VM script: read by the a4 VM's handlers through a4
	incrom	$03B726,$3A
	include "src/f/03B760.asm"
	include "src/f/03B79E.asm"
	include "src/f/03B7AA.asm"
	include "src/f/03B7B6.asm"
	include "src/f/03B7BC.asm"
	include "src/f/03B7C0.asm"
	include "src/f/03B7C6.asm"
Scr4_03B7F2:	; a4-VM script of the cat-head's eye sparks (Boss_CatHead_EyeSpark*): Op4_ForkRestAndEnd, JitterHeading $80, AddMotionRadius, SetAnim frame $15314, Move, Draw
	incrom	$03B7F2,$20
Scr4_03B812:	; a4-VM script of the cat-head's eye shots (Boss_CatHead_EyeShot*): PlaySound $58, ForkRestAndEnd, SetDamage $10, AddMotionRadius, SetDepth, SetAnim, Move, TerrainMove
	incrom	$03B812,$74
Scr4_03B886:	; a4-VM script: read by the a4 VM's handlers through a4; 64% read in the corpus
	incrom	$03B886,$98
	include "src/f/03B91E.asm"
Scr2_03B956:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 03B91E
	incrom	$03B956,$18
	include "src/f/03B96E.asm"
Scr2_03B978:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$03B978,$E4
	include "src/f/03BA5C.asm"
Scr2_03BA74:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$03BA74,$40
	include "src/f/03BAB4.asm"
Scr2_03BAF2:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$03BAF2,$6
	include "src/f/03BAF8.asm"
Scr2_03BB06:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$03BB06,$82
	include "src/f/03BB88.asm"
Scr2_03BDC6:	; a2-VM script: read by the a2 VM's handlers through a2; 90% read in the corpus; referenced by 03BB88
	incrom	$03BDC6,$64
Scr2_03BE2A:	; a2-VM script by its format: 98 of 223 words are a2 handler entry addresses; never run in the corpus
	incrom	$03BE2A,$1BE
Scr2_03BFE8:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$03BFE8,$7E
Scr2_03C066:	; a2-VM script by its format: 67 of 154 words are a2 handler entry addresses; never run in the corpus
	incrom	$03C066,$134
Scr2_03C19A:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$03C19A,$2C
Scr2_03C1C6:	; a2-VM script by its format: 91 of 177 words are a2 handler entry addresses; never run in the corpus
	incrom	$03C1C6,$162
Scr2_03C328:	; a2-VM script: read by the a2 VM's handlers through a2; 94% read in the corpus
	incrom	$03C328,$24
Scr2_03C34C:	; a2-VM script by its format: 87 of 174 words are a2 handler entry addresses; never run in the corpus
	incrom	$03C34C,$15C
Scr5_03C4A8:	; a5-VM script: read by the a5 VM's handlers through a5; 26% read in the corpus
	incrom	$03C4A8,$158
	include "src/f/03C600.asm"
Scr2_03C60C:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 03C600
	incrom	$03C60C,$12
	include "src/f/03C61E.asm"
Scr2_03C62A:	; a2-VM script: read by the a2 VM's handlers through a2; 33% read in the corpus; referenced by 03C61E
	incrom	$03C62A,$C
	include "src/f/03C636.asm"
Scr2_03C642:	; a2-VM script of Enemy_ArenaDebris_SpawnBounceAtX
	incrom	$03C642,$C
	include "src/f/03C64E.asm"
Scr2_03C65A:	; a2-VM script: read by the a2 VM's handlers through a2; 89% read in the corpus; referenced by 03C64E
	incrom	$03C65A,$52A
	include "src/f/03CB84.asm"
Scr2_03CB90:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 03CB84
	incrom	$03CB90,$F2
	include "src/f/03CC82.asm"
	incrom	$03CCC0,$14
	include "src/f/03CCD4.asm"
	include "src/f/03CDDE.asm"
Scr2_03CDEA:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 03CDDE
	incrom	$03CDEA,$54
Scr4_03CE3E:	; a4-VM script (worker 27; datamap's reader rule first took it for an a2 script); 46% read in the corpus; referenced by 03CCD4
	incrom	$03CE3E,$BA
	include "src/f/03CEF8.asm"
	include "src/f/03D000.asm"
Scr2_03D032:	; a2-VM script: read by the a2 VM's handlers through a2; 99% read in the corpus; referenced by 03D000
	incrom	$03D032,$220
	include "src/f/03D252.asm"
Scr2_03D260:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$03D260,$4
	include "src/f/03D264.asm"
Scr2_03D2A6:	; a2-VM script: read by the a2 VM's handlers through a2; 94% read in the corpus
	incrom	$03D2A6,$212
	include "src/f/03D4B8.asm"
Scr2_03D508:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$03D508,$276
	include "src/f/03D77E.asm"
Scr2_03D79E:	; a2-VM script: read by the a2 VM's handlers through a2; 99% read in the corpus
	incrom	$03D79E,$13A
	include "src/f/03D8D8.asm"
Scr4_03D8EC:	; a4-VM script: read by the a4 VM's handlers through a4; referenced by 03D8D8
	incrom	$03D8EC,$14
	include "src/f/03D900.asm"
Scr4_03D908:	; a4-VM script: read by the a4 VM's handlers through a4
	incrom	$03D908,$C
	include "src/f/03D914.asm"
Scr4_03D932:	; a4-VM script: read by the a4 VM's handlers through a4
	incrom	$03D932,$2
	include "src/f/03D934.asm"
Scr2_03D940:	; a2-VM script: read by the a2 VM's handlers through a2; 95% read in the corpus; referenced by 03D934
	incrom	$03D940,$6E
	include "src/f/03D9AE.asm"
Scr4_03D9BC:	; a4-VM script: read by the a4 VM's handlers through a4; 98% read in the corpus
	incrom	$03D9BC,$72
	include "src/f/03DA2E.asm"
Scr2_03DA8C:	; a2-VM script: read by the a2 VM's handlers through a2; 81% read in the corpus; referenced by 03DA2E
	incrom	$03DA8C,$C4
	include "src/f/03DB50.asm"
Scr2_03DBC6:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 03DB50
	incrom	$03DBC6,$A4
	include "src/f/03DC6A.asm"
	include "src/f/03DCDC.asm"
	include "src/f/03DCE6.asm"
	include "src/f/03DCEC.asm"
	include "src/f/03DCF4.asm"
	include "src/f/03DD02.asm"
	include "src/f/03DD0E.asm"
	include "src/f/03DD1E.asm"
	include "src/f/03DD32.asm"
	include "src/f/03DD34.asm"
	include "src/f/03DD36.asm"
	include "src/f/03DD38.asm"
	include "src/f/03DD3A.asm"
	include "src/f/03DD3C.asm"
	include "src/f/03DD3E.asm"
	include "src/f/03DD40.asm"
	include "src/f/03DD42.asm"
	include "src/f/03DD44.asm"
	include "src/f/03DD46.asm"
	include "src/f/03DD48.asm"
	include "src/f/03DD4A.asm"
	include "src/f/03DD4C.asm"
	include "src/f/03DD4E.asm"
	include "src/f/03DD50.asm"
	include "src/f/03DD52.asm"
	include "src/f/03DD5A.asm"
	include "src/f/03DD62.asm"
	include "src/f/03DD6A.asm"
	include "src/f/03DD72.asm"
	include "src/f/03DD7A.asm"
	include "src/f/03DD82.asm"
	include "src/f/03DD8A.asm"
	include "src/f/03DD90.asm"
Tbl_03DDD0:	; read by Boss_Hatter_StepRoadSchedule (pc $03DCBE)
	incrom	$03DDD0,$2
	include "src/f/03DDD2.asm"
	include "src/f/03DDE6.asm"
Scr2_03DDFA:	; a2-VM scripts of the Hatter road objects after Stage3_RoadPlaceNear (part of the span labelled Scr2_03DDE6 until 2026-10-30): coroutine pieces, then at $03DE16 the road hazard's body (Goto target of Scr2_03FB1E and the kind scripts)
	incrom	$03DDFA,$1E
	include "src/f/03DE18.asm"
Scr2_03DE22:	; a2-VM script: read by the a2 VM's handlers through a2; 97% read in the corpus
	incrom	$03DE22,$40
	include "src/f/03DE62.asm"
	incrom	$03DF14,$120
Scr2_03E034:	; a2-VM script: read by the a2 VM's handlers through a2; 48% read in the corpus
	incrom	$03E034,$E6
Scr2_03E11A:	; a2-VM script by its format: 147 of 320 words are a2 handler entry addresses; never run in the corpus
	incrom	$03E11A,$280
Scr2_03E39A:	; a2-VM script: read by the a2 VM's handlers through a2; 81% read in the corpus
	incrom	$03E39A,$198
Scr2_03E532:	; a2-VM script by its format: 646 of 1295 words are a2 handler entry addresses; never run in the corpus
	incrom	$03E532,$A1E
Scr2_03EF50:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$03EF50,$30
Scr2_03EF80:	; a2-VM script by its format: 193 of 383 words are a2 handler entry addresses; never run in the corpus
	incrom	$03EF80,$2FE
Scr2_03F27E:	; a2-VM script: read by the a2 VM's handlers through a2; 29% read in the corpus
	incrom	$03F27E,$35A
	incrom	$03F5D8,$520
	include "src/f/03FAF8.asm"
Scr2_03FB08:	; a2-VM script of Boss_Hatter_SpawnRoadHazardNear
	incrom	$03FB08,$6
	include "src/f/03FB0E.asm"
Scr2_03FB1E:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 03FB0E
	incrom	$03FB1E,$6
Scr2_03FB24:	; a2-VM script by its format: 101 of 354 words are a2 handler entry addresses; never run in the corpus
	include "src/f/03FB24.asm"
Scr2_03FB34:	; a2-VM script of Boss_Hatter_SpawnRoadSplitRight
	incrom	$03FB34,$34
	include "src/f/03FB68.asm"
Scr2_03FB74:	; a2-VM script of Boss_Hatter_SpawnRoadPieceRight1
	incrom	$03FB74,$12
	include "src/f/03FB86.asm"
Scr2_03FB92:	; a2-VM script of Boss_Hatter_SpawnRoadPieceRight2
	incrom	$03FB92,$12
	include "src/f/03FBA4.asm"
Scr2_03FBB0:	; a2-VM script of Boss_Hatter_SpawnRoadPieceRight3
	incrom	$03FBB0,$12
	include "src/f/03FBC2.asm"
Scr2_03FBD2:	; a2-VM script of Boss_Hatter_SpawnRoadSplitLeft
	incrom	$03FBD2,$34
	include "src/f/03FC06.asm"
Scr2_03FC12:	; a2-VM script of Boss_Hatter_SpawnRoadPieceLeft1
	incrom	$03FC12,$12
	include "src/f/03FC24.asm"
Scr2_03FC30:	; a2-VM script of Boss_Hatter_SpawnRoadPieceLeft2
	incrom	$03FC30,$12
	include "src/f/03FC42.asm"
Scr2_03FC4E:	; a2-VM script of Boss_Hatter_SpawnRoadPieceLeft3
	incrom	$03FC4E,$12
	include "src/f/03FC60.asm"
Scr2_03FC70:	; a2-VM script of Boss_Hatter_SpawnRoadSplitRightSoon
	incrom	$03FC70,$34
	include "src/f/03FCA4.asm"
Scr2_03FCB0:	; a2-VM script of Boss_Hatter_SpawnRoadPieceRightSlow1
	incrom	$03FCB0,$12
	include "src/f/03FCC2.asm"
Scr2_03FCCE:	; a2-VM script of Boss_Hatter_SpawnRoadPieceRightSlow2
	incrom	$03FCCE,$12
	include "src/f/03FCE0.asm"
Scr2_03FCEC:	; a2-VM script of Boss_Hatter_SpawnRoadPieceRightSlow3
	incrom	$03FCEC,$12
	include "src/f/03FCFE.asm"
Scr2_03FD0E:	; a2-VM script of Boss_Hatter_SpawnRoadSplitLeftSoon
	incrom	$03FD0E,$34
	include "src/f/03FD42.asm"
Scr2_03FD4E:	; a2-VM script of Boss_Hatter_SpawnRoadPieceLeftSlow1
	incrom	$03FD4E,$12
	include "src/f/03FD60.asm"
Scr2_03FD6C:	; a2-VM script of Boss_Hatter_SpawnRoadPieceLeftSlow2
	incrom	$03FD6C,$12
	include "src/f/03FD7E.asm"
Scr2_03FD8A:	; a2-VM script of Boss_Hatter_SpawnRoadPieceLeftSlow3
	incrom	$03FD8A,$12
	include "src/f/03FD9C.asm"
Scr2_03FDAC:	; a2-VM script of Boss_Hatter_SpawnRoadDriftRight
	incrom	$03FDAC,$16
	include "src/f/03FDC2.asm"
Scr2_03FDD2:	; a2-VM script of Boss_Hatter_SpawnRoadDriftLeft
	incrom	$03FDD2,$16
Tbl_03FDE8:	; read by Pal_LoadLine3 (pc $00AC24), Scene_InitStage (pc $0099D8), Op6_Decompress (pc $00710E); referenced by 03FE94, 0400E4, 0401DA, 0403F6
	incrom	$03FDE8,$20
Tbl_03FE08:	; read by Scene_InitStage (pc $0099D8), Pal_LoadLine3 (pc $00AC24), Op6_Decompress (pc $00710E); referenced by 03FE94
	incrom	$03FE08,$3C
Tbl_03FE44:	; read by Scene_InitStage (pc $0099D8), Pal_LoadLine3 (pc $00AC24), Op6_Decompress (pc $00710E); referenced by 03FE94
	incrom	$03FE44,$50
	include "src/f/03FE94.asm"
Scr6_03FF2A:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 03FFBE
	incrom	$03FF2A,$3C
Scr6_03FF66:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 03FFBE
	incrom	$03FF66,$58
	include "src/f/03FFBE.asm"
Scr6_040018:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 0400E4
	incrom	$040018,$3C
Scr6_040054:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 0400E4
	incrom	$040054,$90
	include "src/f/0400E4.asm"
Tbl_04017A:	; read by Scene_InitStage (pc $0099D8), Scene_InitCommon (pc $009B2C), Op6_DMAEntry (pc $00760A); referenced by 0401DA
	incrom	$04017A,$3C
Tbl_0401B6:	; read by Scene_InitStage (pc $0099D8), Scene_InitCommon (pc $009B2C), Op6_DMAEntry (pc $00760A); referenced by 0401DA
	incrom	$0401B6,$24
	include "src/f/0401DA.asm"
	include "src/f/040258.asm"
	include "src/f/040272.asm"
Tbl_04036E:	; read by Scene_InitStage (pc $0099D8), Op6_DMAEntry (pc $00760A), Op6_Decompress (pc $00710E); referenced by 0403F6
	incrom	$04036E,$3C
Tbl_0403AA:	; read by Scene_InitStage (pc $0099D8), Op6_DMAEntry (pc $00760A), Op6_Decompress (pc $00710E); referenced by 0403F6
	incrom	$0403AA,$4C
	include "src/f/0403F6.asm"
	include "src/f/040460.asm"
Tbl_0404E4:	; read by Scene_InitStage (pc $0099D8), Op6_DMAEntry (pc $00760A), Op6_Decompress (pc $00710E); referenced by 040574
	incrom	$0404E4,$3C
Tbl_040520:	; read by Scene_InitStage (pc $0099D8), Op6_DMAEntry (pc $00760A), Op6_Decompress (pc $00710E); referenced by 040574
	incrom	$040520,$54
	include "src/f/040574.asm"
	include "src/f/0405F2.asm"
	include "src/f/04060E.asm"
Scr2_04061A:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 04060E
	incrom	$04061A,$10
	include "src/f/04062A.asm"
	incrom	$040944,$3C
	include "src/f/040980.asm"
Tbl_040C86:	; read by Stage3_DrawRoad (pc $0408C8), Stage3_DrawRoad (pc $0407FA)
	incrom	$040C86,$230
Tbl_040EB6:	; read by Stage3_DrawRoad (pc $0407FA), Stage3_DrawRoad (pc $0408C8); 10% read in the corpus; referenced by 04062A
	incrom	$040EB6,$14
Tbl_040ECA:	; read by Stage3_DrawRoad (pc $040904), Trigger_Fire (pc $012E3C), Trigger_LoadX (pc $012DE2); 98% read in the corpus; referenced by 04062A
	incrom	$040ECA,$80
Tbl_040F4A:	; read by Stage3_DrawRoad (pc $040904), Trigger_LoadX (pc $012DF4), Trigger_Fire (pc $012E3C); referenced by 0475C2
	incrom	$040F4A,$B6
Tbl_041000:	; read by Trigger_LoadX (pc $012DF4), Stage3_DrawRoad (pc $040904), Trigger_Fire (pc $012E3C); 99% read in the corpus; referenced by 047772
	incrom	$041000,$124
Tbl_041124:	; read by Stage3_DrawRoad (pc $040904), Wave_StartFromEntry (pc $00932C), Trigger_LoadX (pc $012DD0); 90% read in the corpus; referenced by 0479EE
	incrom	$041124,$14
Tbl_041138:	; read by Stage3_DrawRoad (pc $040904), VM5_Next (pc $006A56), Op5_Wait (pc $006AC6); 67% read in the corpus; referenced by 047BD0
	incrom	$041138,$274
Scr5_0413AC:	; a5-VM script by its format: 144 of 528 words are a5 handler entry addresses; never run in the corpus
	incrom	$0413AC,$420
Scr2_0417CC:	; a2-VM script: read by the a2 VM's handlers through a2; 65% read in the corpus
	incrom	$0417CC,$6CE
	include "src/f/041E9A.asm"
Scr2_041EA4:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$041EA4,$1E
	include "src/f/041EC2.asm"
Scr2_041ED0:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$041ED0,$2
	include "src/f/041ED2.asm"
	include "src/f/041F0E.asm"
Scr2_04204C:	; a2-VM script: read by the a2 VM's handlers through a2; 82% read in the corpus; referenced by 041F0E
	incrom	$04204C,$3D4
	include "src/f/042420.asm"
Scr2_04242C:	; the jetpack trooper's body loop after Enemy_JetTrooper_RideTubeScroll (KeepOrPickPlayerTarget, the coroutine steps, draw) and its subroutines $04246C-$04247C (part of Scr2_04204C's span until 2026-10-30)
	incrom	$04242C,$58
	include "src/f/042484.asm"
Scr2_042498:	; a jetpack trooper coroutine after Enemy_JetTrooper_SteerEighthTurn, then the stage 4 enemies' scripts up to the tunnel effect a4 script $042900 (part of Scr2_04204C's span until 2026-10-30)
	incrom	$042498,$482
	include "src/f/04291A.asm"
Scr4_042926:	; the tunnel effect a4 script after Fx_RideTubeScroll (Op4_MoveXY, Op4_Draw) and the effect scripts after it up to Enemy_JetTrooper_ShotHit (part of Scr2_04204C's span until 2026-10-30)
	incrom	$042926,$96
	include "src/f/0429BC.asm"
Scr2_0429CE:	; a2-VM script: read by the a2 VM's handlers through a2; 94% read in the corpus
	incrom	$0429CE,$446
	include "src/f/042E14.asm"
Scr4_042E24:	; a4-VM script: read by the a4 VM's handlers through a4; 75% read in the corpus
	incrom	$042E24,$182
	include "src/f/042FA6.asm"
Scr2_042FC2:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$042FC2,$DA
	include "src/f/04309C.asm"
Scr2_0430C2:	; a2-VM script: read by the a2 VM's handlers through a2; 96% read in the corpus
	incrom	$0430C2,$2AA
	include "src/f/04336C.asm"
Scr4_043378:	; a4-VM script: read by the a4 VM's handlers through a4; 63% read in the corpus
	incrom	$043378,$166
	include "src/f/0434DE.asm"
Scr2_0434EA:	; a2-VM script of Enemy_GasOrb_SpawnSideSweep
	incrom	$0434EA,$CA
	include "src/f/0435B4.asm"
Scr2_0435C0:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 0435B4
	incrom	$0435C0,$BE
	include "src/f/04367E.asm"
Scr2_04368A:	; a2-VM script: read by the a2 VM's handlers through a2; 43% read in the corpus; referenced by 04367E
	incrom	$04368A,$BE
	include "src/f/043748.asm"
Scr2_043754:	; a2-VM script: read by the a2 VM's handlers through a2; 37% read in the corpus; referenced by 043748
	incrom	$043754,$BE
	include "src/f/043812.asm"
Scr2_04381E:	; a2-VM script: read by the a2 VM's handlers through a2; 51% read in the corpus; referenced by 043812
	incrom	$04381E,$2AE
	include "src/f/043ACC.asm"
Scr2_043AD8:	; a2-VM script: read by the a2 VM's handlers through a2; 52% read in the corpus; referenced by 043ACC
	incrom	$043AD8,$AC
Scr2_043B84:	; a2-VM script by its format: 77 of 226 words are a2 handler entry addresses; never run in the corpus
	incrom	$043B84,$1C
	include "src/f/043BA0.asm"
Scr2_043BAC:	; a2-VM script of Enemy_GasOrb_SpawnBossTripleJets: as Scr2_043AD8 with coroutine 1 $43C44 (vector-1 attacks $4425E / $441CE, the slower-puff triple set); never run in the corpus
	incrom	$043BAC,$C8
	include "src/f/043C74.asm"
Scr2_043C80:	; a2-VM script of Enemy_GasOrb_SpawnBossPairJetsFar: as Scr2_043AD8 with coroutine 1 $43D18 (vector-1 attacks $43E1C / $43E66, the faster-puff pair set); run in scenario-sec-4401-freeze
	incrom	$043C80,$C8
	include "src/f/043D48.asm"
Scr2_043D54:	; a2-VM script: read by the a2 VM's handlers through a2; 65% read in the corpus; referenced by 043D48
	incrom	$043D54,$67E
Scr2_0443D2:	; a2-VM script by its format: 45 of 143 words are a2 handler entry addresses; never run in the corpus
	incrom	$0443D2,$A
	include "src/f/0443DC.asm"
Scr2_0443E8:	; a2-VM script of Enemy_HoverPod_Spawn7G
	incrom	$0443E8,$30
	include "src/f/044418.asm"
Scr2_044424:	; a2-VM script of Enemy_HoverPod_Spawn4D
	incrom	$044424,$3C
	include "src/f/044460.asm"
Scr2_04446C:	; a2-VM script of Enemy_HoverPod_Spawn5E
	incrom	$04446C,$3C
	include "src/f/0444A8.asm"
Scr2_0444B4:	; a2-VM script of Enemy_HoverPod_Spawn6F
	incrom	$0444B4,$3C
	include "src/f/0444F0.asm"
Scr2_0444FC:	; a2-VM script: read by the a2 VM's handlers through a2; 97% read in the corpus; referenced by 0444F0
	incrom	$0444FC,$3C
	include "src/f/044538.asm"
Scr2_044544:	; a2-VM script: read by the a2 VM's handlers through a2; 97% read in the corpus; referenced by 044538
	incrom	$044544,$3C
	include "src/f/044580.asm"
Scr2_04458C:	; a2-VM script: read by the a2 VM's handlers through a2; 97% read in the corpus; referenced by 044580
	incrom	$04458C,$3C
	include "src/f/0445C8.asm"
Scr2_0445D4:	; a2-VM script: read by the a2 VM's handlers through a2; 97% read in the corpus; referenced by 0445C8
	incrom	$0445D4,$3C
	include "src/f/044610.asm"
Scr2_04461C:	; a2-VM script: read by the a2 VM's handlers through a2; 97% read in the corpus; referenced by 044610
	incrom	$04461C,$3C
	include "src/f/044658.asm"
Scr2_044664:	; a2-VM script: read by the a2 VM's handlers through a2; 97% read in the corpus; referenced by 044658
	incrom	$044664,$3C
	include "src/f/0446A0.asm"
Scr2_0446AC:	; a2-VM script: read by the a2 VM's handlers through a2; 97% read in the corpus; referenced by 0446A0
	incrom	$0446AC,$3C
	include "src/f/0446E8.asm"
Scr2_0446F4:	; a2-VM script: read by the a2 VM's handlers through a2; 97% read in the corpus; referenced by 0446E8
	incrom	$0446F4,$3C
	include "src/f/044730.asm"
Scr2_04473C:	; a2-VM script: read by the a2 VM's handlers through a2; 38% read in the corpus; referenced by 044730
	incrom	$04473C,$160
Scr2_04489C:	; a2-VM script by its format: 332 of 614 words are a2 handler entry addresses; never run in the corpus
	incrom	$04489C,$4CC
Scr2_044D68:	; a2-VM script: read by the a2 VM's handlers through a2; 65% read in the corpus
	incrom	$044D68,$166
Scr2_044ECE:	; a2-VM script by its format: 196 of 372 words are a2 handler entry addresses; never run in the corpus
	incrom	$044ECE,$2E8
Scr2_0451B6:	; a2-VM script: read by the a2 VM's handlers through a2; 29% read in the corpus
	incrom	$0451B6,$23E
	include "src/f/0453F4.asm"
Scr2_045400:	; a2-VM script of Enemy_JetTrooper_SpawnHoverOnJitter
	incrom	$045400,$54
	include "src/f/045454.asm"
Scr2_045460:	; a2-VM script: read by the a2 VM's handlers through a2; 98% read in the corpus; referenced by 045454
	incrom	$045460,$56
	include "src/f/0454B6.asm"
Scr2_0454C2:	; a2-VM script: read by the a2 VM's handlers through a2; 76% read in the corpus; referenced by 0454B6
	incrom	$0454C2,$52
	include "src/f/045514.asm"
Scr2_045520:	; a2-VM script: read by the a2 VM's handlers through a2; 98% read in the corpus; referenced by 045514
	incrom	$045520,$52
	include "src/f/045572.asm"
Scr2_04557E:	; a2-VM script: read by the a2 VM's handlers through a2; 46% read in the corpus; referenced by 045572
	incrom	$04557E,$74
	include "src/f/0455F2.asm"
Scr2_0455FE:	; a2-VM script of Enemy_JetTrooper_SpawnHoverOnFast
	incrom	$0455FE,$78
	include "src/f/045676.asm"
Scr2_045682:	; a2-VM script: read by the a2 VM's handlers through a2; 98% read in the corpus; referenced by 045676
	incrom	$045682,$74
	include "src/f/0456F6.asm"
Scr2_045702:	; a2-VM script: read by the a2 VM's handlers through a2; 45% read in the corpus; referenced by 0456F6
	incrom	$045702,$100
	include "src/f/045802.asm"
Scr2_04580E:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 045802
	incrom	$04580E,$42
Scr2_045850:	; a2-VM script by its format: 56 of 151 words are a2 handler entry addresses; never run in the corpus
	include "src/f/045850.asm"
Scr2_04585C:	; a2-VM script of Enemy_VentTank_SpawnFaster
	incrom	$04585C,$64
	include "src/f/0458C0.asm"
Scr2_0458CC:	; a2-VM script of Enemy_VentTank_SpawnFastest
	incrom	$0458CC,$60
	include "src/f/04592C.asm"
Scr2_045938:	; a2-VM script of Enemy_VentTank_SpawnRandomSpeed
	incrom	$045938,$46
	include "src/f/04597E.asm"
Scr2_04598A:	; a2-VM script: read by the a2 VM's handlers through a2; 50% read in the corpus; referenced by 04597E
	incrom	$04598A,$78
	include "src/f/045A02.asm"
Scr2_045A0E:	; a2-VM script: read by the a2 VM's handlers through a2; 67% read in the corpus; referenced by 045A02
	incrom	$045A0E,$66
	include "src/f/045A74.asm"
Scr2_045A80:	; a2-VM script: read by the a2 VM's handlers through a2; 68% read in the corpus; referenced by 045A74
	incrom	$045A80,$17C
Scr2_045BFC:	; a2-VM script by its format: 101 of 221 words are a2 handler entry addresses; never run in the corpus
	include "src/f/045BFC.asm"
Scr2_045C08:	; a2-VM script of Enemy_GunTank_SpawnHeavy
	incrom	$045C08,$FE
	include "src/f/045D06.asm"
Scr2_045D12:	; a2-VM script of Enemy_GunTank_SpawnToughMid
	incrom	$045D12,$A4
	include "src/f/045DB6.asm"
Scr2_045DC2:	; a2-VM script: read by the a2 VM's handlers through a2; 43% read in the corpus; referenced by 045DB6
	incrom	$045DC2,$82
	include "src/f/045E44.asm"
Scr2_045E50:	; a2-VM script of Enemy_GunTank_SpawnLow
	incrom	$045E50,$82
	include "src/f/045ED2.asm"
Scr2_045EDE:	; a2-VM script: read by the a2 VM's handlers through a2; 98% read in the corpus; referenced by 045ED2
	incrom	$045EDE,$7C
Scr2_045F5A:	; a2-VM script by its format: 71 of 183 words are a2 handler entry addresses; never run in the corpus
	include "src/f/045F5A.asm"
Scr2_045F66:	; a2-VM script of Enemy_GunTank_SpawnToughFast
	incrom	$045F66,$B4
	include "src/f/04601A.asm"
Scr2_046026:	; a2-VM script of Enemy_GunTank_SpawnToughSlow
	incrom	$046026,$A2
Scr4_0460C8:	; a4-VM script: read by the a4 VM's handlers through a4; 54% read in the corpus
	incrom	$0460C8,$38
	include "src/f/046100.asm"
Scr2_04610C:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 046100
	incrom	$04610C,$28
	include "src/f/046134.asm"
Scr2_046140:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 046134
	incrom	$046140,$2A
	include "src/f/04616A.asm"
Scr2_046176:	; a2-VM script: read by the a2 VM's handlers through a2; 100% read in the corpus; referenced by 04616A
	incrom	$046176,$386
	include "src/f/0464FC.asm"
Scr2_046524:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$046524,$C0
	include "src/f/0465E4.asm"
Scr2_0465EA:	; a2-VM script: read by the a2 VM's handlers through a2; 80% read in the corpus
	incrom	$0465EA,$1D6
	include "src/f/0467C0.asm"
Scr2_0467CA:	; Mr. Freeze's firing animation after Boss_Freeze_ShotRecoilLeft: Fade_InFromPale6, the shot effect, frames (part of Scr2_0465EA's span until 2026-10-30)
	incrom	$0467CA,$60
	include "src/f/04682A.asm"
Scr4_046834:	; a4-VM script: read by the a4 VM's handlers through a4; 83% read in the corpus
	incrom	$046834,$6A
	include "src/f/04689E.asm"
Scr2_0468A8:	; Mr. Freeze's firing animation after Boss_Freeze_ShotRecoilDown: Fade_InFromPale6, the shot effect, frames (part of Scr4_046834's span until 2026-10-30)
	incrom	$0468A8,$4C
	include "src/f/0468F4.asm"
Scr2_0468FE:	; Mr. Freeze's firing animation after Boss_Freeze_ShotRecoilUp, then his fight's scripts up to Boss_Freeze_Spawn (part of Scr4_046834's span until 2026-10-30)
	incrom	$0468FE,$428
	include "src/f/046D26.asm"
Scr2_046D32:	; a2-VM script: read by the a2 VM's handlers through a2; 44% read in the corpus; referenced by 046D26
	incrom	$046D32,$24A
Scr2_046F7C:	; a2-VM script by its format: 112 of 232 words are a2 handler entry addresses; never run in the corpus
	incrom	$046F7C,$1D0
Scr2_04714C:	; a2-VM script: read by the a2 VM's handlers through a2; 95% read in the corpus
	incrom	$04714C,$74
	include "src/f/0471C0.asm"
Tbl_047508:	; read by Stage4_DrawCones (pc $04720E), Scene_InitStage (pc $0099D8), Pal_LoadLine3 (pc $00AC24); referenced by 0471C0
	incrom	$047508,$18
Tbl_047520:	; read by Pal_LoadLine3 (pc $00AC24), Scene_InitStage (pc $0099D8), Op6_DMAEntry (pc $00760A); referenced by 0475C2, 047772, 0479EE, 047BD0
	incrom	$047520,$20
Tbl_047540:	; read by Scene_InitStage (pc $0099D8), Pal_LoadLine3 (pc $00AC24), Op6_DMAEntry (pc $00760A); referenced by 0475C2
	incrom	$047540,$3C
Tbl_04757C:	; read by Scene_InitStage (pc $0099D8), Pal_LoadLine3 (pc $00AC24), Op6_DMAEntry (pc $00760A); referenced by 0475C2
	incrom	$04757C,$46
	include "src/f/0475C2.asm"
	include "src/f/047674.asm"
Tbl_0476EA:	; read by Scene_InitStage (pc $0099D8), Op6_DMAEntry (pc $00760A), Op6_Decompress (pc $00710E); referenced by 047772
	incrom	$0476EA,$3C
Tbl_047726:	; read by Scene_InitStage (pc $0099D8), Op6_DMAEntry (pc $00760A), Op6_Decompress (pc $00710E); referenced by 047772
	incrom	$047726,$4C
	include "src/f/047772.asm"
	include "src/f/0477D4.asm"
	include "src/f/047842.asm"
	include "src/f/04788C.asm"
Tbl_04795A:	; read by Scene_InitStage (pc $0099D8), Op6_DMAEntry (pc $00760A), Op6_Decompress (pc $00710E); referenced by 0479EE
	incrom	$04795A,$3C
Tbl_047996:	; read by Scene_InitStage (pc $0099D8), Op6_DMAEntry (pc $00760A), Op6_Decompress (pc $00710E); referenced by 0479EE
	incrom	$047996,$58
	include "src/f/0479EE.asm"
	incrom	$047A66,$C
	include "src/f/047A72.asm"
Tbl_047B48:	; read by Scene_InitStage (pc $0099D8), Op6_DMAEntry (pc $00760A), Op6_Decompress (pc $00710E); referenced by 047BD0
	incrom	$047B48,$3C
Tbl_047B84:	; read by Scene_InitStage (pc $0099D8), Op6_DMAEntry (pc $00760A), Op6_Decompress (pc $00710E); referenced by 047BD0
	incrom	$047B84,$4C
	include "src/f/047BD0.asm"
	include "src/f/047C6A.asm"
Scr6_047CD0:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 0484F6, 048512
	incrom	$047CD0,$12
Scr6_047CE2:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 04857E
	incrom	$047CE2,$12
Scr6_047CF4:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 048656
	incrom	$047CF4,$12
Scr6_047D06:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 0487A6
	incrom	$047D06,$12
Scr6_047D18:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 048838
	incrom	$047D18,$12
Scr6_047D2A:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 0484F6, 048512
	incrom	$047D2A,$4C
Scr6_047D76:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 04857E
	incrom	$047D76,$4C
Scr6_047DC2:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 048656
	incrom	$047DC2,$32
Scr6_047DF4:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 0487A6
	incrom	$047DF4,$3E
Scr6_047E32:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 048838
	incrom	$047E32,$6C4
	include "src/f/0484F6.asm"
	include "src/f/048512.asm"
	include "src/f/04852E.asm"
Scr2_04853A:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 04852E
	incrom	$04853A,$44
	include "src/f/04857E.asm"
	include "src/f/0485D8.asm"
Scr2_0485E4:	; a2-VM script: read by the a2 VM's handlers through a2; 96% read in the corpus; referenced by 0485D8
	incrom	$0485E4,$72
	include "src/f/048656.asm"
	include "src/f/0486D2.asm"
Scr2_0486DE:	; a2-VM script: read by the a2 VM's handlers through a2; 99% read in the corpus; referenced by 0486D2
	incrom	$0486DE,$C8
	include "src/f/0487A6.asm"
	include "src/f/0487CE.asm"
Scr2_0487DA:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 0487CE
	incrom	$0487DA,$5E
	include "src/f/048838.asm"
	include "src/f/048878.asm"
Scr2_048884:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 048878
	incrom	$048884,$114
	include "src/f/048998.asm"
Scr6_0489E2:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 048AD2
	incrom	$0489E2,$12
Scr6_0489F4:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 048C36
	incrom	$0489F4,$12
Scr6_048A06:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 048AD2
	incrom	$048A06,$80
Scr6_048A86:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 048C36
	incrom	$048A86,$4C
	include "src/f/048AD2.asm"
	include "src/f/048B1E.asm"
Scr2_048B2A:	; a2-VM script: read by the a2 VM's handlers through a2; 96% read in the corpus; referenced by 048B1E
	incrom	$048B2A,$64
	include "src/f/048B8E.asm"
Scr2_048B9A:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 048B8E
	incrom	$048B9A,$9C
	include "src/f/048C36.asm"
	include "src/f/048CD6.asm"
Scr2_048CE2:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 048CD6
	incrom	$048CE2,$19E
	include "src/f/048E80.asm"
Scr6_048EDE:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 04916E
	incrom	$048EDE,$12
Scr6_048EF0:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 0494BE
	incrom	$048EF0,$12
Scr6_048F02:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 0494BE
	incrom	$048F02,$4C
Scr6_048F4E:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 04916E
	incrom	$048F4E,$220
	include "src/f/04916E.asm"
	include "src/f/049194.asm"
Scr2_0491A0:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 049194
	incrom	$0491A0,$1C
	include "src/f/0491BC.asm"
	incrom	$0491C6,$C
	include "src/f/0491D2.asm"
Scr2_0491D6:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$0491D6,$26
	include "src/f/0491FC.asm"
Scr2_049204:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$049204,$E
	include "src/f/049212.asm"
Scr2_049232:	; a2-VM script: read by the a2 VM's handlers through a2; 63% read in the corpus
	incrom	$049232,$4
	include "src/f/049236.asm"
Scr2_049256:	; the continue screen's script after Continue_DrawNoCreditsText (Op2_Return), then its other coroutines up to Continue_SlowWhiteFade (part of Scr2_049232's span until 2026-10-30)
	incrom	$049256,$17C
	include "src/f/0493D2.asm"
Scr2_0493DC:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$0493DC,$1E
	include "src/f/0493FA.asm"
Tbl_049428:	; read by TextMap_PutChars (pc $01440A), TextMap_NextCmd (pc $0143A4), TextMap_CmdSetPos (pc $0143CC); 58% read in the corpus; referenced by 049212
	incrom	$049428,$58
Tbl_049480:	; text-map command list of the continue screen with no credits left: 'NO CREDITS REMAINING' and its blank twin (blinking); run by Continue_DrawNoCreditsText
	incrom	$049480,$3E
	include "src/f/0494BE.asm"
	include "src/f/0494DE.asm"
Scr2_0494EA:	; a4-VM script: read by the a4 VM's handlers through a4; 99% read in the corpus; referenced by 0494DE
	incrom	$0494EA,$11C
	include "src/f/049606.asm"
Tbl_049616:	; read by Scroll_FillBands (pc $009930), Scroll_NextCmd (pc $0097F6), ScrollCmd_CameraA (pc $00984E); referenced by 049606
	incrom	$049616,$1A
	include "src/f/049630.asm"
Scr2_04963C:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 049630
	incrom	$04963C,$1E
	include "src/f/04965A.asm"
Scr2_049664:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$049664,$10
Tbl_049674:	; read by Scene_InitCommon (pc $009B2C), Op6_DMAEntry (pc $00760A), Op6_Decompress (pc $00710E); referenced by 0496A2
	incrom	$049674,$12
Tbl_049686:	; read by Scene_InitCommon (pc $009B2C), Op6_DMAEntry (pc $00760A), Op6_Decompress (pc $00710E); referenced by 0496A2
	incrom	$049686,$1C
	include "src/f/0496A2.asm"
	include "src/f/0496CC.asm"
Scr2_0496D8:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 0496CC
	incrom	$0496D8,$14
	include "src/f/0496EC.asm"
Scr2_049700:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$049700,$12
	include "src/f/049712.asm"
Scr2_049720:	; a2-VM script: read by the a2 VM's handlers through a2; 95% read in the corpus
	incrom	$049720,$50
	include "src/f/049770.asm"
	include "src/f/049798.asm"
	include "src/f/0497DA.asm"
	include "src/f/049816.asm"
Tbl_04984C:	; read by Title_StartPaletteFade (pc $0497BE), Title_PaletteFadeChannel (pc $049834), TextMap_NextCmd (pc $0143A4); referenced by 049798
	incrom	$04984C,$62
Tbl_0498AE:	; read by TextMap_NextCmd (pc $0143A4), TextMap_CmdGoto (pc $0143B8), TextMap_CmdSetPos (pc $0143CC); referenced by 0499B6
	incrom	$0498AE,$3C
Tbl_0498EA:	; read by TextMap_PutChars (pc $01440A), TextMap_NextCmd (pc $0143A4), TextMap_CmdSetPos (pc $0143CC); 99% read in the corpus; referenced by 0499E8
	incrom	$0498EA,$5E
Tbl_049948:	; read by TextMap_NextCmd (pc $0143A4), TextMap_PutChars (pc $01440A), TextMap_CmdGoto (pc $0143B8); 99% read in the corpus; referenced by 0499E8
	incrom	$049948,$4C
Tbl_049994:	; read by Menu_Update (pc $01458C), TextMap_NextCmd (pc $0143A4), Menu_Update (pc $0145AA); 94% read in the corpus; referenced by 0499E8
	incrom	$049994,$22
	include "src/f/0499B6.asm"
	include "src/f/0499E8.asm"
Scr6_049A56:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 049AA6
	incrom	$049A56,$12
Scr6_049A68:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 049AA6
	incrom	$049A68,$3E
	include "src/f/049AA6.asm"
	include "src/f/049AD4.asm"
	include "src/f/049B18.asm"
Scr2_049B24:	; a2-VM script: read by the a2 VM's handlers through a2; 97% read in the corpus; referenced by 049B18
	incrom	$049B24,$4E
	include "src/f/049B72.asm"
Scr2_049B7A:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$049B7A,$26
	include "src/f/049BA0.asm"
	include "src/f/049C3C.asm"
Scr2_049C48:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 049C3C
	incrom	$049C48,$40
	include "src/f/049C88.asm"
Tbl_049CCA:	; read by Scene_InitCommon (pc $009B2C); referenced by 049DA6
	incrom	$049CCA,$12
Tbl_049CDC:	; read by Scene_InitCommon (pc $009B2C); referenced by 049F62
	incrom	$049CDC,$12
Tbl_049CEE:	; read by Scene_InitCommon (pc $009B2C); 69% read in the corpus; referenced by 049DEC
	incrom	$049CEE,$1A
Scr6_049D08:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 049DA6
	incrom	$049D08,$30
Scr6_049D38:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 049F62
	incrom	$049D38,$30
Scr6_049D68:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 049DEC
	incrom	$049D68,$3E
	include "src/f/049DA6.asm"
	include "src/f/049DC2.asm"
Scr2_049DCE:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 049DC2
	incrom	$049DCE,$1E
	include "src/f/049DEC.asm"
	include "src/f/049E2E.asm"
Scr2_049E3A:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 049E2E
	incrom	$049E3A,$5E
	include "src/f/049E98.asm"
Tbl_049ED2:	; read by ClockworkLogo_ScrollPicture (pc $049EC6), Pal_LoadLine3 (pc $00AC24); referenced by 049DEC
	incrom	$049ED2,$A
Tbl_049EDC:	; read by ClockworkLogo_ScrollPicture (pc $049EC6), Pal_LoadLine3 (pc $00AC24); 97% read in the corpus; referenced by 049E98
	incrom	$049EDC,$86
	include "src/f/049F62.asm"
	include "src/f/049F88.asm"
Scr2_049F94:	; read by Op2_CallNative (pc $002CBC), Scene_EndAfterDelay (pc $009CF4), VM2_RunObject (pc $0007E0); referenced by 049F88
	incrom	$049F94,$C
	include "src/f/049FA0.asm"
Tbl_049FB6:	; read by TextMap_PutChars (pc $01440A), TextMap_NextCmd (pc $0143A4), TextMap_CmdSetPos (pc $0143CC)
	incrom	$049FB6,$2
Tbl_049FB8:	; read by TextMap_PutChars (pc $01440A), TextMap_NextCmd (pc $0143A4), TextMap_CmdSetPos (pc $0143CC); referenced by 049FA0
	incrom	$049FB8,$98
Tbl_04A050:	; read by Scene_InitCommon (pc $009B2C), TextMap_PutChars (pc $01440A), TextMap_NextCmd (pc $0143A4); referenced by 04A180, 04A3A4
	incrom	$04A050,$12
Tbl_04A062:	; read by TextMap_PutChars (pc $01440A), Scene_InitCommon (pc $009B2C), TextMap_NextCmd (pc $0143A4); referenced by 04A216
	incrom	$04A062,$12
Tbl_04A074:	; read by TextMap_PutChars (pc $01440A), Scene_InitCommon (pc $009B2C), TextMap_NextCmd (pc $0143A4); 50% read in the corpus; referenced by 04A2DC
	incrom	$04A074,$24
Scr6_04A098:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 04A464
	incrom	$04A098,$12
Scr6_04A0AA:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 04A180
	incrom	$04A0AA,$46
Scr6_04A0F0:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 04A216
	incrom	$04A0F0,$24
Scr6_04A114:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 04A2DC
	incrom	$04A114,$24
Scr6_04A138:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 04A3A4
	incrom	$04A138,$24
Scr6_04A15C:	; a6-VM script: read by the a6 VM's handlers through a6; referenced by 04A464
	incrom	$04A15C,$24
	include "src/f/04A180.asm"
	include "src/f/04A19C.asm"
Scr2_04A1A8:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 04A19C
	incrom	$04A1A8,$20
	include "src/f/04A1C8.asm"
	include "src/f/04A216.asm"
	include "src/f/04A232.asm"
Scr2_04A23E:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 04A232
	incrom	$04A23E,$36
	include "src/f/04A274.asm"
Scr2_04A280:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 04A274
	incrom	$04A280,$28
	include "src/f/04A2A8.asm"
Scr2_04A2B4:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 04A2A8
	incrom	$04A2B4,$28
	include "src/f/04A2DC.asm"
	include "src/f/04A2FA.asm"
Scr2_04A306:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 04A2FA
	incrom	$04A306,$36
	include "src/f/04A33C.asm"
Scr2_04A348:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 04A33C
	incrom	$04A348,$28
	include "src/f/04A370.asm"
Scr2_04A37C:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 04A370
	incrom	$04A37C,$28
	include "src/f/04A3A4.asm"
	include "src/f/04A3C0.asm"
Scr2_04A3CC:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 04A3C0
	incrom	$04A3CC,$30
	include "src/f/04A3FC.asm"
Scr2_04A408:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 04A3FC
	incrom	$04A408,$28
	include "src/f/04A430.asm"
Scr2_04A43C:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 04A430
	incrom	$04A43C,$28
	include "src/f/04A464.asm"
	include "src/f/04A480.asm"
Scr2_04A48C:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 04A480
	incrom	$04A48C,$36
	include "src/f/04A4C2.asm"
Scr2_04A4CE:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 04A4C2
	incrom	$04A4CE,$28
	include "src/f/04A4F6.asm"
Scr2_04A502:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 04A4F6
	incrom	$04A502,$28
Scr2_04A52A:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 04A5AA
	incrom	$04A52A,$12
Scr2_04A53C:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 04A626
	incrom	$04A53C,$12
Scr2_04A54E:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 04A5AA
	incrom	$04A54E,$24
Scr2_04A572:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 04A626
	incrom	$04A572,$38
	include "src/f/04A5AA.asm"
	include "src/f/04A5D0.asm"
Scr2_04A5DC:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 04A5D0
	incrom	$04A5DC,$C
	include "src/f/04A5E8.asm"
Scr2_04A5FE:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$04A5FE,$28
	include "src/f/04A626.asm"
	include "src/f/04A64C.asm"
Scr2_04A658:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 04A64C
	incrom	$04A658,$C4
	include "src/f/04A71C.asm"
Scr2_04A728:	; a2-VM script: read by the a2 VM's handlers through a2; referenced by 04A71C
	incrom	$04A728,$C
	include "src/f/04A734.asm"
Tbl_04A74A:	; read by TextMap_PutChars (pc $01440A), TextMap_NextCmd (pc $0143A4), TextMap_CmdSetPos (pc $0143CC); 99% read in the corpus
	incrom	$04A74A,$586
Scr2_04ACD0:	; a2-VM script: read by the a2 VM's handlers through a2; 97% read in the corpus; referenced by 0101CE
	incrom	$04ACD0,$2672
	include "src/f/04D342.asm"
Scr2_04D34C:	; a2-VM script: read by the a2 VM's handlers through a2; 99% read in the corpus
	incrom	$04D34C,$4C0
Scr2_04D80C:	; a2-VM script: read by the a2 VM's handlers through a2; 92% read in the corpus; referenced by 0101CE
	incrom	$04D80C,$7C
Scr2_04D888:	; a2-VM script by its format: 114 of 273 words are a2 handler entry addresses; never run in the corpus
	incrom	$04D888,$222
Scr2_04DAAA:	; a2-VM script: read by the a2 VM's handlers through a2; 48% read in the corpus
	incrom	$04DAAA,$2DA
Scr2_04DD84:	; a2-VM script by its format: 110 of 268 words are a2 handler entry addresses; never run in the corpus
	incrom	$04DD84,$218
Scr2_04DF9C:	; a2-VM script: read by the a2 VM's handlers through a2; 48% read in the corpus
	incrom	$04DF9C,$248
Scr2_04E1E4:	; a2-VM script by its format: 116 of 284 words are a2 handler entry addresses; never run in the corpus
	incrom	$04E1E4,$238
Scr2_04E41C:	; a2-VM script: read by the a2 VM's handlers through a2; 69% read in the corpus
	incrom	$04E41C,$1B2
Scr2_04E5CE:	; a2-VM script by its format: 246 of 492 words are a2 handler entry addresses; never run in the corpus
	incrom	$04E5CE,$3D8
Scr2_04E9A6:	; a2-VM script: read by the a2 VM's handlers through a2; 65% read in the corpus
	incrom	$04E9A6,$11A
Scr2_04EAC0:	; a2-VM script by its format: 150 of 325 words are a2 handler entry addresses; never run in the corpus
	incrom	$04EAC0,$28A
Scr2_04ED4A:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$04ED4A,$7C
Scr2_04EDC6:	; a2-VM script by its format: 82 of 170 words are a2 handler entry addresses; never run in the corpus
	incrom	$04EDC6,$154
Scr2_04EF1A:	; a2-VM script: read by the a2 VM's handlers through a2
	incrom	$04EF1A,$9C
Scr2_04EFB6:	; a2-VM script by its format: 69 of 168 words are a2 handler entry addresses; never run in the corpus
	incrom	$04EFB6,$150
Scr2_04F106:	; a2-VM script: read by the a2 VM's handlers through a2; 12% read in the corpus
	incrom	$04F106,$15E
Scr2_04F264:	; a2-VM script by its format: 388 of 823 words are a2 handler entry addresses; never run in the corpus
	incrom	$04F264,$66E
Scr2_04F8D2:	; a2-VM script: read by the a2 VM's handlers through a2; 73% read in the corpus
	incrom	$04F8D2,$1C8
Scr2_04FA9A:	; a2-VM script by its format: 553 of 1112 words are a2 handler entry addresses; never run in the corpus
	incrom	$04FA9A,$3E4
	include "src/f/04FE7E.asm"
Scr2_04FE88:	; Robin's jetpack-tunnel script after Player_CopyPadToVar2Robin (Op2_SwitchVar2Bits -> poses) up to Scr4_05034A (part of Scr2_04FA9A's span until 2026-10-30)
	incrom	$04FE88,$4C2
Scr4_05034A:	; a4-VM script: read by the a4 VM's handlers through a4; 84% read in the corpus
	incrom	$05034A,$E54
Scr4_05119E:	; a4-VM script by its format: 68 of 148 words are a4 handler entry addresses; never run in the corpus
	incrom	$05119E,$128
Scr4_0512C6:	; a4-VM script: read by the a4 VM's handlers through a4; 49% read in the corpus
	incrom	$0512C6,$4CC
Tbl_051792:	; read by HUD_DrawWeaponBar (pc $011542), Op4_Draw (pc $0067FC), Op4_Goto (pc $005432); 98% read in the corpus; referenced by 011314
	incrom	$051792,$D80
Tbl_052512:	; read by HUD_DrawWeaponBar (pc $01154A); 99% read in the corpus; referenced by 011314
	incrom	$052512,$2300
Tbl_054812:	; read by HUD_DrawWeaponBar (pc $011542); 76% read in the corpus; referenced by 011314
	incrom	$054812,$474
	incrom	$054C86,$90C
Tbl_055592:	; addressed as data by code (operand in 011314); never read in the corpus
	incrom	$055592,$20
Tbl_0555B2:	; read by HUD_DrawWeaponBar (pc $01154A); 85% read in the corpus
	incrom	$0555B2,$8E0
Tbl_055E92:	; 4bpp tile data like Tbl_0555B2/Tbl_056012 around it (read by HUD_DrawWeaponBar at $01154A); this part never read in the corpus (inferred same kind)
	incrom	$055E92,$180
Tbl_056012:	; read by HUD_DrawWeaponBar (pc $01154A); 57% read in the corpus
	incrom	$056012,$1C0
Tbl_0561D2:	; 4bpp tile data, 182 tiles, continuing Tbl_056012 (read by HUD_DrawWeaponBar at $01154A); never read in the corpus (inferred same kind)
	incrom	$0561D2,$16C0
Tbl_057892:	; read by BossMeter_AnimRing (pc $011BEE); referenced by 011BBA
	incrom	$057892,$100
Tbl_057992:	; read by BossMeter_AnimRing (pc $011BF6); 98% read in the corpus; referenced by 011BBA
	incrom	$057992,$AC0
Art_058452:	; DMA source to VRAM
	incrom	$058452,$2A0
Tbl_0586F2:	; read by HUD_DrawPanelText (pc $01171E), TextMap_PutChars (pc $01440E), HUD_DrawScoreDigit (pc $0115FA); 44% read in the corpus; referenced by 011314, 0145BE
	incrom	$0586F2,$20
Tbl_058712:	; read by HUD_DrawPanelText (pc $01171E), HUD_DrawScoreDigit (pc $0115FA), TextMap_PutChars (pc $01440E); 91% read in the corpus; referenced by 01156E, 011C5E
	incrom	$058712,$56
Tbl_058768:	; read by HUD_DrawLifeRing (pc $011686), TextMap_PutChars (pc $01440E); 86% read in the corpus; referenced by 011638
	incrom	$058768,$162
Tbl_0588CA:	; read by HUD_DrawLifeRing (pc $011686); 82% read in the corpus; referenced by 011638
	incrom	$0588CA,$176
Tbl_058A40:	; read by HUD_DrawLifeRing (pc $0116CE); 14% read in the corpus; referenced by 011638
	incrom	$058A40,$62
Art_058AA2:	; DMA source to VRAM; referenced by 011314, 01156E, 011638, 011C5E
	incrom	$058AA2,$1080
Tbl_059B22:	; read by Op4_Move (pc $0062D0), Op4_Move (pc $0062D4), Mot_SteerToHeading (pc $0015F4); referenced by 0014F0, 00156A, 0057C0, 00624C ...
	incrom	$059B22,$500
Lz_05A022:	; LZ block ($00A23E): 17946 bytes packed, 29508 unpacked
	incrom	$05A022,$461A
Lz_05E63C:	; LZ block ($00A23E): 2021 bytes packed, 5972 unpacked
	incrom	$05E63C,$7E6
Lz_05EE22:	; LZ block ($00A23E): 13149 bytes packed, 29348 unpacked
	incrom	$05EE22,$335E
Lz_062180:	; LZ block ($00A23E): 4347 bytes packed, 11284 unpacked
	incrom	$062180,$10FC
Lz_06327C:	; LZ block ($00A23E): 1886 bytes packed, 6322 unpacked
	incrom	$06327C,$75E
Lz_0639DA:	; LZ block ($00A23E): 9888 bytes packed, 22100 unpacked
	incrom	$0639DA,$26A0
Lz_06607A:	; LZ block ($00A23E): 10112 bytes packed, 26786 unpacked
	incrom	$06607A,$2780
Lz_0687FA:	; LZ block ($00A23E): 6837 bytes packed, 16148 unpacked
	incrom	$0687FA,$1AB6
Lz_06A2B0:	; LZ block ($00A23E): 5186 bytes packed, 8116 unpacked
	incrom	$06A2B0,$1442
Lz_06B6F2:	; LZ block ($00A23E): 8963 bytes packed, 20824 unpacked
	incrom	$06B6F2,$2304
Lz_06D9F6:	; LZ block ($00A23E): 1612 bytes packed, 3064 unpacked
	incrom	$06D9F6,$64C
Lz_06E042:	; LZ block ($00A23E): 13552 bytes packed, 24766 unpacked
	incrom	$06E042,$34F0
Lz_071532:	; LZ block ($00A23E): 2507 bytes packed, 5206 unpacked
	incrom	$071532,$9CC
Lz_071EFE:	; LZ block ($00A23E): 8380 bytes packed, 20100 unpacked
	incrom	$071EFE,$20BC
Lz_073FBA:	; LZ block ($00A23E): 7221 bytes packed, 13880 unpacked
	incrom	$073FBA,$1C36
Lz_075BF0:	; LZ block ($00A23E): 9979 bytes packed, 29942 unpacked
	incrom	$075BF0,$26FC
Lz_0782EC:	; LZ block ($00A23E): 4487 bytes packed, 8724 unpacked
	incrom	$0782EC,$1188
Lz_079474:	; LZ block ($00A23E): 1804 bytes packed, 4324 unpacked
	incrom	$079474,$70C
Lz_079B80:	; LZ block ($00A23E): 6863 bytes packed, 13300 unpacked
	incrom	$079B80,$1AD0
Lz_07B650:	; LZ block ($00A23E): 8703 bytes packed, 21620 unpacked
	incrom	$07B650,$2200
Lz_07D850:	; LZ block ($00A23E): 3794 bytes packed, 8838 unpacked
	incrom	$07D850,$ED2
Lz_07E722:	; LZ block ($00A23E): 12252 bytes packed, 19450 unpacked
	incrom	$07E722,$2FDC
Lz_0816FE:	; LZ block ($00A23E): 982 bytes packed, 2502 unpacked
	incrom	$0816FE,$3D6
Lz_081AD4:	; LZ block ($00A23E): 2342 bytes packed, 8244 unpacked
	incrom	$081AD4,$926
Lz_0823FA:	; LZ block ($00A23E): 9956 bytes packed, 16710 unpacked
	incrom	$0823FA,$26E4
Lz_084ADE:	; LZ block ($00A23E): 7517 bytes packed, 22732 unpacked
	incrom	$084ADE,$1D5E
Lz_08683C:	; LZ block ($00A23E): 5753 bytes packed, 12528 unpacked
	incrom	$08683C,$167A
Lz_087EB6:	; LZ block ($00A23E): 6347 bytes packed, 12790 unpacked
	incrom	$087EB6,$18CC
Lz_089782:	; LZ block ($00A23E): 4729 bytes packed, 20622 unpacked
	incrom	$089782,$127A
Lz_08A9FC:	; LZ block ($00A23E): 10223 bytes packed, 28692 unpacked
	incrom	$08A9FC,$27F0
Lz_08D1EC:	; LZ block ($00A23E): 7763 bytes packed, 16468 unpacked
	incrom	$08D1EC,$1E54
Lz_08F040:	; LZ block ($00A23E): 6746 bytes packed, 10068 unpacked
	incrom	$08F040,$1A5A
Lz_090A9A:	; LZ block ($00A23E): 5153 bytes packed, 15296 unpacked
	incrom	$090A9A,$1422
Lz_091EBC:	; LZ block ($00A23E): 11054 bytes packed, 17030 unpacked
	incrom	$091EBC,$2B2E
Lz_0949EA:	; LZ block ($00A23E): 6446 bytes packed, 16596 unpacked
	incrom	$0949EA,$192E
Lz_096318:	; LZ block ($00A23E): 2878 bytes packed, 7110 unpacked
	incrom	$096318,$B3E
Lz_096E56:	; LZ block ($00A23E): 1981 bytes packed, 1812 unpacked
	incrom	$096E56,$7BE
Lz_097614:	; LZ block ($00A23E): 6864 bytes packed, 13620 unpacked
	incrom	$097614,$1AD0
Lz_0990E4:	; LZ block ($00A23E): 6197 bytes packed, 18580 unpacked
	incrom	$0990E4,$1836
Lz_09A91A:	; LZ block ($00A23E): 8177 bytes packed, 21908 unpacked
	incrom	$09A91A,$1FF2
Lz_09C90C:	; LZ block ($00A23E): 9520 bytes packed, 23444 unpacked
	incrom	$09C90C,$2530
Lz_09EE3C:	; LZ block ($00A23E): 5287 bytes packed, 8628 unpacked
	incrom	$09EE3C,$14A8
Lz_0A02E4:	; LZ block ($00A23E): 1474 bytes packed, 4676 unpacked
	incrom	$0A02E4,$5C2
Lz_0A08A6:	; LZ block ($00A23E): 899 bytes packed, 1846 unpacked
	incrom	$0A08A6,$384
Lz_0A0C2A:	; LZ block ($00A23E): 7377 bytes packed, 14494 unpacked
	incrom	$0A0C2A,$1CD2
Lz_0A28FC:	; LZ block ($00A23E): 8883 bytes packed, 18576 unpacked
	incrom	$0A28FC,$22B4
Lz_0A4BB0:	; LZ block ($00A23E): 13914 bytes packed, 29762 unpacked
	incrom	$0A4BB0,$365A
Lz_0A820A:	; LZ block ($00A23E): 5521 bytes packed, 13364 unpacked
	incrom	$0A820A,$1592
Lz_0A979C:	; LZ block ($00A23E): 4949 bytes packed, 15124 unpacked
	incrom	$0A979C,$1356
Lz_0AAAF2:	; LZ block ($00A23E): 4963 bytes packed, 17044 unpacked
	incrom	$0AAAF2,$1364
Lz_0ABE56:	; LZ block ($00A23E): 6043 bytes packed, 16468 unpacked
	incrom	$0ABE56,$179C
Lz_0AD5F2:	; LZ block ($00A23E): 3398 bytes packed, 10436 unpacked
	incrom	$0AD5F2,$D46
Lz_0AE338:	; LZ block ($00A23E): 6769 bytes packed, 16692 unpacked
	incrom	$0AE338,$1A72
Lz_0AFDAA:	; LZ block ($00A23E): 11291 bytes packed, 19700 unpacked
	incrom	$0AFDAA,$2C1C
Tbl_0B29C6:	; read by Layer_AnimateTiles (pc $012728), Layer_SetAnimTiles (pc $0126DE); 94% read in the corpus; referenced by 047BD0
	incrom	$0B29C6,$24
Art_0B29EA:	; DMA source to VRAM
	incrom	$0B29EA,$4C0
Art_0B2EAA:	; DMA source to VRAM
	incrom	$0B2EAA,$4C0
Art_0B336A:	; DMA source to VRAM
	incrom	$0B336A,$4C0
Art_0B382A:	; DMA source to VRAM
	incrom	$0B382A,$4C0
Lz_0B3CEA:	; LZ block ($00A23E): 7092 bytes packed, 15560 unpacked
	incrom	$0B3CEA,$1BB4
Lz_0B589E:	; LZ block ($00A23E): 7912 bytes packed, 17000 unpacked
	incrom	$0B589E,$1EE8
Lz_0B7786:	; LZ block ($00A23E): 14543 bytes packed, 28270 unpacked
	incrom	$0B7786,$38D0
Lz_0BB056:	; LZ block ($00A23E): 1701 bytes packed, 3380 unpacked
	incrom	$0BB056,$6A6
Lz_0BB6FC:	; LZ block ($00A23E): 3938 bytes packed, 6132 unpacked
	incrom	$0BB6FC,$F62
Lz_0BC65E:	; LZ block ($00A23E): 5451 bytes packed, 13812 unpacked
	incrom	$0BC65E,$154C
Lz_0BDBAA:	; LZ block ($00A23E): 7260 bytes packed, 12884 unpacked
	incrom	$0BDBAA,$1C5C
Lz_0BF806:	; LZ block ($00A23E): 8124 bytes packed, 16692 unpacked
	incrom	$0BF806,$1FBC
Lz_0C17C2:	; LZ block ($00A23E): 4056 bytes packed, 9300 unpacked
	incrom	$0C17C2,$FD8
Lz_0C279A:	; LZ block ($00A23E): 7455 bytes packed, 14676 unpacked
	incrom	$0C279A,$1D20
Lz_0C44BA:	; LZ block ($00A23E): 10292 bytes packed, 23366 unpacked
	incrom	$0C44BA,$2834
Lz_0C6CEE:	; LZ block ($00A23E): 4971 bytes packed, 9658 unpacked
	incrom	$0C6CEE,$136C
Lz_0C805A:	; LZ block ($00A23E): 2419 bytes packed, 5460 unpacked
	incrom	$0C805A,$974
Lz_0C89CE:	; LZ block ($00A23E): 8863 bytes packed, 16212 unpacked
	incrom	$0C89CE,$22A0
Lz_0CAC6E:	; LZ block ($00A23E): 3263 bytes packed, 13044 unpacked
	incrom	$0CAC6E,$CC0
Lz_0CB92E:	; LZ block ($00A23E): 5176 bytes packed, 12372 unpacked
	incrom	$0CB92E,$1438
Lz_0CCD66:	; LZ block ($00A23E): 17561 bytes packed, 27956 unpacked
	incrom	$0CCD66,$449A
Lz_0D1200:	; LZ block ($00A23E): 16661 bytes packed, 28468 unpacked
	incrom	$0D1200,$4116
Lz_0D5316:	; LZ block ($00A23E): 12078 bytes packed, 24212 unpacked
	incrom	$0D5316,$2F2E
Lz_0D8244:	; LZ block ($00A23E): 14918 bytes packed, 29044 unpacked
	incrom	$0D8244,$3A46
Lz_0DBC8A:	; LZ block ($00A23E): 7071 bytes packed, 16820 unpacked
	incrom	$0DBC8A,$1BA0
Lz_0DD82A:	; LZ block ($00A23E): 9148 bytes packed, 21300 unpacked
	incrom	$0DD82A,$23BC
Lz_0DFBE6:	; LZ block ($00A23E): 4628 bytes packed, 9332 unpacked
	incrom	$0DFBE6,$1214
Lz_0E0DFA:	; LZ block ($00A23E): 14449 bytes packed, 27700 unpacked
	incrom	$0E0DFA,$3872
Lz_0E466C:	; LZ block ($00A23E): 8684 bytes packed, 23284 unpacked
	incrom	$0E466C,$21EC
Lz_0E6858:	; LZ block ($00A23E): 4514 bytes packed, 12692 unpacked
	incrom	$0E6858,$11A2
Lz_0E79FA:	; LZ block ($00A23E): 9201 bytes packed, 20244 unpacked
	incrom	$0E79FA,$23F2
Lz_0E9DEC:	; LZ block ($00A23E): 6756 bytes packed, 15444 unpacked
	incrom	$0E9DEC,$1A64
Lz_0EB850:	; LZ block ($00A23E): 4277 bytes packed, 16020 unpacked
	incrom	$0EB850,$10B6
Lz_0EC906:	; LZ block ($00A23E): 2029 bytes packed, 8276 unpacked
	incrom	$0EC906,$7EE
Lz_0ED0F4:	; LZ block ($00A23E): 2464 bytes packed, 11060 unpacked
	incrom	$0ED0F4,$9A0
Lz_0EDA94:	; LZ block ($00A23E): 7776 bytes packed, 15460 unpacked
	incrom	$0EDA94,$1E60
Lz_0EF8F4:	; LZ block ($00A23E): 12103 bytes packed, 21188 unpacked
	incrom	$0EF8F4,$2F48
Lz_0F283C:	; LZ block ($00A23E): 4813 bytes packed, 11828 unpacked
	incrom	$0F283C,$12CE
Lz_0F3B0A:	; LZ block ($00A23E): 4712 bytes packed, 7476 unpacked
	incrom	$0F3B0A,$1268
Lz_0F4D72:	; LZ block ($00A23E): 1871 bytes packed, 3092 unpacked
	incrom	$0F4D72,$750
Lz_0F54C2:	; LZ block ($00A23E): 6419 bytes packed, 11092 unpacked
	incrom	$0F54C2,$1914
Lz_0F6DD6:	; LZ block ($00A23E): 9595 bytes packed, 17492 unpacked
	incrom	$0F6DD6,$257C
Lz_0F9352:	; LZ block ($00A23E): 4095 bytes packed, 11380 unpacked
	incrom	$0F9352,$1000
Lz_0FA352:	; LZ block ($00A23E): 7017 bytes packed, 15508 unpacked
	incrom	$0FA352,$1B6A
Lz_0FBEBC:	; LZ block ($00A23E): 2630 bytes packed, 4620 unpacked
	incrom	$0FBEBC,$A46
Lz_0FC902:	; LZ block ($00A23E): 2856 bytes packed, 8790 unpacked
	incrom	$0FC902,$B28
Lz_0FD42A:	; LZ block ($00A23E): 1662 bytes packed, 3950 unpacked
	incrom	$0FD42A,$67E
Lz_0FDAA8:	; LZ block ($00A23E): 475 bytes packed, 698 unpacked
	incrom	$0FDAA8,$1DC
Lz_0FDC84:	; LZ block ($00A23E): 2617 bytes packed, 10158 unpacked
	incrom	$0FDC84,$A3A
Lz_0FE6BE:	; LZ block ($00A23E): 4905 bytes packed, 14874 unpacked
	incrom	$0FE6BE,$132A
Lz_0FF9E8:	; LZ block ($00A23E): 363 bytes packed, 904 unpacked
	incrom	$0FF9E8,$16B
	incrom	$0FFB53,$1
Lz_0FFB54:	; LZ block ($00A23E): 258 bytes packed, 2196 unpacked
	incrom	$0FFB54,$102
Lz_0FFC56:	; LZ block ($00A23E): 232 bytes packed, 4244 unpacked
	incrom	$0FFC56,$E8
	incrom	$0FFD3E,$2C2
Art_100000:	; sprite art block: 16 tiles in 1 chunk(s), DMA cost 16 (frame table entry $1DDB6A); never displayed in the corpus
	incrom	$100000,$206
Art_100206:	; sprite art block: 16 tiles in 1 chunk(s), DMA cost 16 (frame table entry $1DDC0A); never displayed in the corpus
	incrom	$100206,$206
Tbl_10040C:	; read by Spr_SwapFrameArt (pc $007F70), Spr_QueueArtDMA (pc $0080BE)
	incrom	$10040C,$6
Art_100412:	; DMA source to VRAM
	incrom	$100412,$106
Art_100518:	; DMA source to VRAM
	incrom	$100518,$106
Art_10061E:	; DMA source to VRAM
	incrom	$10061E,$106
Art_100724:	; DMA source to VRAM
	incrom	$100724,$106
Art_10082A:	; DMA source to VRAM
	incrom	$10082A,$106
Art_100930:	; DMA source to VRAM
	incrom	$100930,$C6
Art_1009F6:	; DMA source to VRAM
	incrom	$1009F6,$106
Art_100AFC:	; DMA source to VRAM
	incrom	$100AFC,$106
Art_100C02:	; DMA source to VRAM
	incrom	$100C02,$86
Art_100C88:	; DMA source to VRAM
	incrom	$100C88,$C0
Art_100D48:	; sprite art block: 8 tiles in 1 chunk(s), DMA cost 8 (frame table entry $1DDCD2); never displayed in the corpus
	incrom	$100D48,$106
Tbl_100E4E:	; read by Spr_SwapFrameArt (pc $007F70), Spr_QueueArtDMA (pc $0080BE)
	incrom	$100E4E,$6
Art_100E54:	; DMA source to VRAM
	incrom	$100E54,$A6
Art_100EFA:	; DMA source to VRAM
	incrom	$100EFA,$206
Art_101100:	; DMA source to VRAM
	incrom	$101100,$106
Art_101206:	; DMA source to VRAM
	incrom	$101206,$106
Art_10130C:	; DMA source to VRAM
	incrom	$10130C,$E0
Art_1013EC:	; sprite art block: 8 tiles in 1 chunk(s), DMA cost 8 (frame table entry $1DDD0E); never displayed in the corpus
	incrom	$1013EC,$106
Tbl_1014F2:	; read by Spr_SwapFrameArt (pc $007F70), Spr_QueueArtDMA (pc $0080BE)
	incrom	$1014F2,$6
Art_1014F8:	; DMA source to VRAM
	incrom	$1014F8,$106
Art_1015FE:	; DMA source to VRAM
	incrom	$1015FE,$106
Art_101704:	; DMA source to VRAM
	incrom	$101704,$106
Art_10180A:	; DMA source to VRAM
	incrom	$10180A,$106
Art_101910:	; DMA source to VRAM
	incrom	$101910,$106
Art_101A16:	; DMA source to VRAM
	incrom	$101A16,$1C0
Art_101BD6:	; sprite art block: 8 tiles in 1 chunk(s), DMA cost 8 (frame table entry $1DDD54); never displayed in the corpus
	incrom	$101BD6,$106
Tbl_101CDC:	; read by Spr_SwapFrameArt (pc $007F70), Spr_QueueArtDMA (pc $0080BE)
	incrom	$101CDC,$6
Art_101CE2:	; DMA source to VRAM
	incrom	$101CE2,$100
Art_101DE2:	; sprite art block: 8 tiles in 1 chunk(s), DMA cost 8 (frame table entry $1DDD68); never displayed in the corpus
	incrom	$101DE2,$106
Tbl_101EE8:	; read by Spr_SwapFrameArt (pc $007F70), Spr_QueueArtDMA (pc $0080BE)
	incrom	$101EE8,$6
Art_101EEE:	; DMA source to VRAM
	incrom	$101EEE,$86
Art_101F74:	; DMA source to VRAM
	incrom	$101F74,$108
Art_10207C:	; DMA source to VRAM
	incrom	$10207C,$200
Art_10227C:	; DMA source to VRAM
	incrom	$10227C,$108
Art_102384:	; DMA source to VRAM
	incrom	$102384,$200
Art_102584:	; DMA source to VRAM
	incrom	$102584,$108
Art_10268C:	; DMA source to VRAM
	incrom	$10268C,$200
Art_10288C:	; DMA source to VRAM
	incrom	$10288C,$106
Art_102992:	; DMA source to VRAM
	incrom	$102992,$1C6
Art_102B58:	; DMA source to VRAM
	incrom	$102B58,$C6
Art_102C1E:	; DMA source to VRAM
	incrom	$102C1E,$C6
Art_102CE4:	; DMA source to VRAM
	incrom	$102CE4,$C6
Art_102DAA:	; DMA source to VRAM
	incrom	$102DAA,$C6
Art_102E70:	; DMA source to VRAM
	incrom	$102E70,$106
Art_102F76:	; DMA source to VRAM
	incrom	$102F76,$106
Art_10307C:	; DMA source to VRAM
	incrom	$10307C,$126
Art_1031A2:	; DMA source to VRAM
	incrom	$1031A2,$126
Art_1032C8:	; DMA source to VRAM
	incrom	$1032C8,$C6
Art_10338E:	; DMA source to VRAM
	incrom	$10338E,$106
Art_103494:	; DMA source to VRAM
	incrom	$103494,$186
Art_10361A:	; DMA source to VRAM
	incrom	$10361A,$186
Art_1037A0:	; DMA source to VRAM
	incrom	$1037A0,$C6
Art_103866:	; DMA source to VRAM
	incrom	$103866,$E6
Art_10394C:	; DMA source to VRAM
	incrom	$10394C,$C6
Art_103A12:	; DMA source to VRAM
	incrom	$103A12,$C8
Art_103ADA:	; DMA source to VRAM
	incrom	$103ADA,$200
Art_103CDA:	; DMA source to VRAM
	incrom	$103CDA,$C8
Art_103DA2:	; DMA source to VRAM
	incrom	$103DA2,$200
Art_103FA2:	; DMA source to VRAM
	incrom	$103FA2,$A8
Art_10404A:	; DMA source to VRAM
	incrom	$10404A,$200
Art_10424A:	; DMA source to VRAM
	incrom	$10424A,$66
Art_1042B0:	; DMA source to VRAM
	incrom	$1042B0,$1C6
Art_104476:	; DMA source to VRAM
	incrom	$104476,$106
Art_10457C:	; DMA source to VRAM
	incrom	$10457C,$106
Art_104682:	; DMA source to VRAM
	incrom	$104682,$106
Art_104788:	; DMA source to VRAM
	incrom	$104788,$106
Art_10488E:	; DMA source to VRAM
	incrom	$10488E,$146
Art_1049D4:	; DMA source to VRAM
	incrom	$1049D4,$E6
Art_104ABA:	; DMA source to VRAM
	incrom	$104ABA,$E6
Art_104BA0:	; DMA source to VRAM
	incrom	$104BA0,$C6
Art_104C66:	; DMA source to VRAM
	incrom	$104C66,$106
Art_104D6C:	; DMA source to VRAM
	incrom	$104D6C,$C6
Art_104E32:	; DMA source to VRAM
	incrom	$104E32,$146
Art_104F78:	; DMA source to VRAM
	incrom	$104F78,$146
Art_1050BE:	; DMA source to VRAM
	incrom	$1050BE,$C6
Art_105184:	; DMA source to VRAM
	incrom	$105184,$E6
Art_10526A:	; DMA source to VRAM
	incrom	$10526A,$146
Art_1053B0:	; DMA source to VRAM
	incrom	$1053B0,$146
Art_1054F6:	; DMA source to VRAM
	incrom	$1054F6,$160
Art_105656:	; sprite art block: 6 tiles in 1 chunk(s), DMA cost 6 (frame table entry $1DDF20); never displayed in the corpus
	incrom	$105656,$C6
Art_10571C:	; sprite art block: 9 tiles in 1 chunk(s), DMA cost 9 (frame table entry $1DDF2A); never displayed in the corpus
	incrom	$10571C,$126
Art_105842:	; sprite art block: 9 tiles in 1 chunk(s), DMA cost 9 (frame table entry $1DDF34); never displayed in the corpus
	incrom	$105842,$126
Tbl_105968:	; read by Spr_SwapFrameArt (pc $007F70), Spr_QueueArtDMA (pc $0080BE)
	incrom	$105968,$6
Art_10596E:	; DMA source to VRAM
	incrom	$10596E,$106
Art_105A74:	; DMA source to VRAM
	incrom	$105A74,$C6
Art_105B3A:	; DMA source to VRAM
	incrom	$105B3A,$106
Art_105C40:	; DMA source to VRAM
	incrom	$105C40,$108
Art_105D48:	; DMA source to VRAM
	incrom	$105D48,$200
Art_105F48:	; DMA source to VRAM
	incrom	$105F48,$28
Art_105F70:	; DMA source to VRAM
	incrom	$105F70,$200
Art_106170:	; DMA source to VRAM
	incrom	$106170,$28
Art_106198:	; DMA source to VRAM
	incrom	$106198,$200
Art_106398:	; DMA source to VRAM
	incrom	$106398,$86
Art_10641E:	; DMA source to VRAM
	incrom	$10641E,$186
Art_1065A4:	; DMA source to VRAM
	incrom	$1065A4,$C6
Art_10666A:	; DMA source to VRAM
	incrom	$10666A,$C6
Art_106730:	; DMA source to VRAM
	incrom	$106730,$C6
Art_1067F6:	; DMA source to VRAM
	incrom	$1067F6,$C6
Art_1068BC:	; DMA source to VRAM
	incrom	$1068BC,$126
Art_1069E2:	; DMA source to VRAM
	incrom	$1069E2,$106
Art_106AE8:	; DMA source to VRAM
	incrom	$106AE8,$E6
Art_106BCE:	; DMA source to VRAM
	incrom	$106BCE,$106
Art_106CD4:	; DMA source to VRAM
	incrom	$106CD4,$C6
Art_106D9A:	; DMA source to VRAM
	incrom	$106D9A,$E6
Art_106E80:	; DMA source to VRAM
	incrom	$106E80,$E6
Art_106F66:	; DMA source to VRAM
	incrom	$106F66,$A8
Art_10700E:	; DMA source to VRAM
	incrom	$10700E,$200
Art_10720E:	; DMA source to VRAM
	incrom	$10720E,$148
Art_107356:	; DMA source to VRAM
	incrom	$107356,$200
Art_107556:	; DMA source to VRAM
	incrom	$107556,$12A
Art_107680:	; DMA source to VRAM
	incrom	$107680,$180
Art_107800:	; DMA source to VRAM
	incrom	$107800,$180
Art_107980:	; DMA source to VRAM
	incrom	$107980,$C8
Art_107A48:	; DMA source to VRAM
	incrom	$107A48,$1E0
Art_107C28:	; DMA source to VRAM
	incrom	$107C28,$108
Art_107D30:	; DMA source to VRAM
	incrom	$107D30,$200
Art_107F30:	; DMA source to VRAM
	incrom	$107F30,$68
Art_107F98:	; DMA source to VRAM
	incrom	$107F98,$200
Art_108198:	; DMA source to VRAM
	incrom	$108198,$88
Art_108220:	; DMA source to VRAM
	incrom	$108220,$200
Art_108420:	; DMA source to VRAM
	incrom	$108420,$188
Art_1085A8:	; DMA source to VRAM
	incrom	$1085A8,$200
Art_1087A8:	; DMA source to VRAM
	incrom	$1087A8,$148
Art_1088F0:	; DMA source to VRAM
	incrom	$1088F0,$200
Art_108AF0:	; DMA source to VRAM
	incrom	$108AF0,$188
Art_108C78:	; DMA source to VRAM
	incrom	$108C78,$200
Art_108E78:	; DMA source to VRAM
	incrom	$108E78,$208
Art_109080:	; DMA source to VRAM
	incrom	$109080,$200
Art_109280:	; DMA source to VRAM
	incrom	$109280,$168
Art_1093E8:	; DMA source to VRAM
	incrom	$1093E8,$200
Art_1095E8:	; DMA source to VRAM
	incrom	$1095E8,$88
Art_109670:	; DMA source to VRAM
	incrom	$109670,$200
Art_109870:	; DMA source to VRAM
	incrom	$109870,$C8
Art_109938:	; DMA source to VRAM
	incrom	$109938,$200
Art_109B38:	; DMA source to VRAM
	incrom	$109B38,$10A
Art_109C42:	; DMA source to VRAM
	incrom	$109C42,$200
Art_109E42:	; DMA source to VRAM
	incrom	$109E42,$200
Art_10A042:	; DMA source to VRAM
	incrom	$10A042,$68
Art_10A0AA:	; DMA source to VRAM
	incrom	$10A0AA,$200
Art_10A2AA:	; DMA source to VRAM
	incrom	$10A2AA,$1C8
Art_10A472:	; DMA source to VRAM
	incrom	$10A472,$1E0
Art_10A652:	; DMA source to VRAM
	incrom	$10A652,$C8
Art_10A71A:	; DMA source to VRAM
	incrom	$10A71A,$200
Art_10A91A:	; DMA source to VRAM
	incrom	$10A91A,$C8
Art_10A9E2:	; DMA source to VRAM
	incrom	$10A9E2,$200
Art_10ABE2:	; DMA source to VRAM
	incrom	$10ABE2,$148
Art_10AD2A:	; DMA source to VRAM
	incrom	$10AD2A,$1C0
Art_10AEEA:	; DMA source to VRAM
	incrom	$10AEEA,$188
Art_10B072:	; DMA source to VRAM
	incrom	$10B072,$1C0
Art_10B232:	; DMA source to VRAM
	incrom	$10B232,$1C8
Art_10B3FA:	; DMA source to VRAM
	incrom	$10B3FA,$1A0
Art_10B59A:	; DMA source to VRAM
	incrom	$10B59A,$108
Art_10B6A2:	; DMA source to VRAM
	incrom	$10B6A2,$200
Art_10B8A2:	; DMA source to VRAM
	incrom	$10B8A2,$1C8
Art_10BA6A:	; DMA source to VRAM
	incrom	$10BA6A,$1E0
Art_10BC4A:	; DMA source to VRAM
	incrom	$10BC4A,$1CA
Art_10BE14:	; DMA source to VRAM
	incrom	$10BE14,$200
Art_10C014:	; DMA source to VRAM
	incrom	$10C014,$200
Art_10C214:	; DMA source to VRAM
	incrom	$10C214,$2A
Art_10C23E:	; DMA source to VRAM
	incrom	$10C23E,$1C0
Art_10C3FE:	; DMA source to VRAM
	incrom	$10C3FE,$180
Art_10C57E:	; DMA source to VRAM
	incrom	$10C57E,$CA
Art_10C648:	; DMA source to VRAM
	incrom	$10C648,$200
Art_10C848:	; DMA source to VRAM
	incrom	$10C848,$200
Art_10CA48:	; DMA source to VRAM
	incrom	$10CA48,$28
Art_10CA70:	; DMA source to VRAM
	incrom	$10CA70,$200
Art_10CC70:	; DMA source to VRAM
	incrom	$10CC70,$208
Art_10CE78:	; DMA source to VRAM
	incrom	$10CE78,$200
Art_10D078:	; DMA source to VRAM
	incrom	$10D078,$20A
Art_10D282:	; DMA source to VRAM
	incrom	$10D282,$200
Art_10D482:	; DMA source to VRAM
	incrom	$10D482,$200
Art_10D682:	; DMA source to VRAM
	incrom	$10D682,$28
Art_10D6AA:	; DMA source to VRAM
	incrom	$10D6AA,$200
Art_10D8AA:	; DMA source to VRAM
	incrom	$10D8AA,$14A
Art_10D9F4:	; DMA source to VRAM
	incrom	$10D9F4,$160
Art_10DB54:	; DMA source to VRAM
	incrom	$10DB54,$120
Art_10DC74:	; DMA source to VRAM
	incrom	$10DC74,$108
Art_10DD7C:	; DMA source to VRAM
	incrom	$10DD7C,$200
Art_10DF7C:	; DMA source to VRAM
	incrom	$10DF7C,$208
Art_10E184:	; DMA source to VRAM
	incrom	$10E184,$1E0
Art_10E364:	; DMA source to VRAM
	incrom	$10E364,$206
Art_10E56A:	; DMA source to VRAM
	incrom	$10E56A,$186
Art_10E6F0:	; DMA source to VRAM
	incrom	$10E6F0,$1C8
Art_10E8B8:	; DMA source to VRAM
	incrom	$10E8B8,$1E0
Art_10EA98:	; DMA source to VRAM
	incrom	$10EA98,$10A
Art_10EBA2:	; DMA source to VRAM
	incrom	$10EBA2,$180
Art_10ED22:	; DMA source to VRAM
	incrom	$10ED22,$180
Art_10EEA2:	; DMA source to VRAM
	incrom	$10EEA2,$C8
Art_10EF6A:	; DMA source to VRAM
	incrom	$10EF6A,$200
Art_10F16A:	; DMA source to VRAM
	incrom	$10F16A,$10A
Art_10F274:	; DMA source to VRAM
	incrom	$10F274,$200
Art_10F474:	; DMA source to VRAM
	incrom	$10F474,$200
Art_10F674:	; DMA source to VRAM
	incrom	$10F674,$AA
Art_10F71E:	; DMA source to VRAM
	incrom	$10F71E,$200
Art_10F91E:	; DMA source to VRAM
	incrom	$10F91E,$200
Art_10FB1E:	; DMA source to VRAM
	incrom	$10FB1E,$A8
Art_10FBC6:	; DMA source to VRAM
	incrom	$10FBC6,$200
Art_10FDC6:	; DMA source to VRAM
	incrom	$10FDC6,$18A
Art_10FF50:	; DMA source to VRAM
	incrom	$10FF50,$200
Art_110150:	; DMA source to VRAM
	incrom	$110150,$1E0
Art_110330:	; DMA source to VRAM
	incrom	$110330,$14A
Art_11047A:	; DMA source to VRAM
	incrom	$11047A,$200
Art_11067A:	; DMA source to VRAM
	incrom	$11067A,$200
Art_11087A:	; DMA source to VRAM
	incrom	$11087A,$10A
Art_110984:	; DMA source to VRAM
	incrom	$110984,$200
Art_110B84:	; DMA source to VRAM
	incrom	$110B84,$200
Art_110D84:	; DMA source to VRAM
	incrom	$110D84,$12A
Art_110EAE:	; DMA source to VRAM
	incrom	$110EAE,$200
Art_1110AE:	; DMA source to VRAM
	incrom	$1110AE,$1E0
Art_11128E:	; DMA source to VRAM
	incrom	$11128E,$6A
Art_1112F8:	; DMA source to VRAM
	incrom	$1112F8,$1C0
Art_1114B8:	; DMA source to VRAM
	incrom	$1114B8,$200
Art_1116B8:	; DMA source to VRAM
	incrom	$1116B8,$CA
Art_111782:	; DMA source to VRAM
	incrom	$111782,$200
Art_111982:	; DMA source to VRAM
	incrom	$111982,$1E0
Art_111B62:	; DMA source to VRAM
	incrom	$111B62,$10A
Art_111C6C:	; DMA source to VRAM
	incrom	$111C6C,$200
Art_111E6C:	; DMA source to VRAM
	incrom	$111E6C,$200
Art_11206C:	; DMA source to VRAM
	incrom	$11206C,$14A
Art_1121B6:	; DMA source to VRAM
	incrom	$1121B6,$200
Art_1123B6:	; DMA source to VRAM
	incrom	$1123B6,$200
Art_1125B6:	; DMA source to VRAM
	incrom	$1125B6,$18A
Art_112740:	; DMA source to VRAM
	incrom	$112740,$200
Art_112940:	; DMA source to VRAM
	incrom	$112940,$1C0
Art_112B00:	; DMA source to VRAM
	incrom	$112B00,$C8
Art_112BC8:	; DMA source to VRAM
	incrom	$112BC8,$200
Art_112DC8:	; DMA source to VRAM
	incrom	$112DC8,$20A
Art_112FD2:	; DMA source to VRAM
	incrom	$112FD2,$200
Art_1131D2:	; DMA source to VRAM
	incrom	$1131D2,$200
Art_1133D2:	; DMA source to VRAM
	incrom	$1133D2,$4A
Art_11341C:	; DMA source to VRAM
	incrom	$11341C,$200
Art_11361C:	; DMA source to VRAM
	incrom	$11361C,$160
Art_11377C:	; DMA source to VRAM
	incrom	$11377C,$10A
Art_113886:	; DMA source to VRAM
	incrom	$113886,$200
Art_113A86:	; DMA source to VRAM
	incrom	$113A86,$1C0
Art_113C46:	; DMA source to VRAM
	incrom	$113C46,$208
Art_113E4E:	; DMA source to VRAM
	incrom	$113E4E,$1E0
Art_11402E:	; DMA source to VRAM
	incrom	$11402E,$1A8
Art_1141D6:	; DMA source to VRAM
	incrom	$1141D6,$200
Art_1143D6:	; DMA source to VRAM
	incrom	$1143D6,$128
Art_1144FE:	; DMA source to VRAM
	incrom	$1144FE,$200
Art_1146FE:	; DMA source to VRAM
	incrom	$1146FE,$1CA
Art_1148C8:	; DMA source to VRAM
	incrom	$1148C8,$1E0
Art_114AA8:	; DMA source to VRAM
	incrom	$114AA8,$1C0
Art_114C68:	; DMA source to VRAM
	incrom	$114C68,$88
Art_114CF0:	; DMA source to VRAM
	incrom	$114CF0,$200
Art_114EF0:	; DMA source to VRAM
	incrom	$114EF0,$20A
Art_1150FA:	; DMA source to VRAM
	incrom	$1150FA,$200
Art_1152FA:	; DMA source to VRAM
	incrom	$1152FA,$200
Art_1154FA:	; DMA source to VRAM
	incrom	$1154FA,$4A
Art_115544:	; DMA source to VRAM
	incrom	$115544,$200
Art_115744:	; DMA source to VRAM
	incrom	$115744,$200
Art_115944:	; DMA source to VRAM
	incrom	$115944,$C8
Art_115A0C:	; DMA source to VRAM
	incrom	$115A0C,$200
Art_115C0C:	; DMA source to VRAM
	incrom	$115C0C,$188
Art_115D94:	; DMA source to VRAM
	incrom	$115D94,$200
Art_115F94:	; DMA source to VRAM
	incrom	$115F94,$1CA
Art_11615E:	; DMA source to VRAM
	incrom	$11615E,$200
Art_11635E:	; DMA source to VRAM
	incrom	$11635E,$200
Art_11655E:	; DMA source to VRAM
	incrom	$11655E,$8A
Art_1165E8:	; DMA source to VRAM
	incrom	$1165E8,$1E0
Art_1167C8:	; DMA source to VRAM
	incrom	$1167C8,$120
Art_1168E8:	; DMA source to VRAM
	incrom	$1168E8,$10A
Art_1169F2:	; DMA source to VRAM
	incrom	$1169F2,$200
Art_116BF2:	; DMA source to VRAM
	incrom	$116BF2,$200
Art_116DF2:	; DMA source to VRAM
	incrom	$116DF2,$8A
Art_116E7C:	; DMA source to VRAM
	incrom	$116E7C,$200
Art_11707C:	; DMA source to VRAM
	incrom	$11707C,$200
Art_11727C:	; DMA source to VRAM
	incrom	$11727C,$48
Art_1172C4:	; DMA source to VRAM
	incrom	$1172C4,$200
Art_1174C4:	; DMA source to VRAM
	incrom	$1174C4,$128
Art_1175EC:	; DMA source to VRAM
	incrom	$1175EC,$1A0
Art_11778C:	; DMA source to VRAM
	incrom	$11778C,$188
Art_117914:	; DMA source to VRAM
	incrom	$117914,$200
Art_117B14:	; DMA source to VRAM
	incrom	$117B14,$1CA
Art_117CDE:	; DMA source to VRAM
	incrom	$117CDE,$1E0
Art_117EBE:	; DMA source to VRAM
	incrom	$117EBE,$180
Art_11803E:	; DMA source to VRAM
	incrom	$11803E,$188
Art_1181C6:	; DMA source to VRAM
	incrom	$1181C6,$200
Art_1183C6:	; DMA source to VRAM
	incrom	$1183C6,$1A8
Art_11856E:	; DMA source to VRAM
	incrom	$11856E,$200
Art_11876E:	; DMA source to VRAM
	incrom	$11876E,$148
Art_1188B6:	; DMA source to VRAM
	incrom	$1188B6,$200
Art_118AB6:	; DMA source to VRAM
	incrom	$118AB6,$1EA
Art_118CA0:	; DMA source to VRAM
	incrom	$118CA0,$200
Art_118EA0:	; DMA source to VRAM
	incrom	$118EA0,$1E0
Art_119080:	; DMA source to VRAM
	incrom	$119080,$4A
Art_1190CA:	; DMA source to VRAM
	incrom	$1190CA,$200
Art_1192CA:	; DMA source to VRAM
	incrom	$1192CA,$200
Art_1194CA:	; DMA source to VRAM
	incrom	$1194CA,$8A
Art_119554:	; DMA source to VRAM
	incrom	$119554,$200
Art_119754:	; DMA source to VRAM
	incrom	$119754,$200
Art_119954:	; DMA source to VRAM
	incrom	$119954,$CA
Art_119A1E:	; DMA source to VRAM
	incrom	$119A1E,$200
Art_119C1E:	; DMA source to VRAM
	incrom	$119C1E,$1E0
Art_119DFE:	; DMA source to VRAM
	incrom	$119DFE,$48
Art_119E46:	; DMA source to VRAM
	incrom	$119E46,$1A0
Art_119FE6:	; DMA source to VRAM
	incrom	$119FE6,$188
Art_11A16E:	; DMA source to VRAM
	incrom	$11A16E,$200
Art_11A36E:	; DMA source to VRAM
	incrom	$11A36E,$128
Art_11A496:	; DMA source to VRAM
	incrom	$11A496,$200
Art_11A696:	; DMA source to VRAM
	incrom	$11A696,$148
Art_11A7DE:	; DMA source to VRAM
	incrom	$11A7DE,$200
Art_11A9DE:	; DMA source to VRAM
	incrom	$11A9DE,$1A8
Art_11AB86:	; DMA source to VRAM
	incrom	$11AB86,$1C0
Art_11AD46:	; DMA source to VRAM
	incrom	$11AD46,$108
Art_11AE4E:	; DMA source to VRAM
	incrom	$11AE4E,$200
Art_11B04E:	; DMA source to VRAM
	incrom	$11B04E,$188
Art_11B1D6:	; DMA source to VRAM
	incrom	$11B1D6,$1C0
Art_11B396:	; DMA source to VRAM
	incrom	$11B396,$188
Art_11B51E:	; DMA source to VRAM
	incrom	$11B51E,$1E0
Art_11B6FE:	; DMA source to VRAM
	incrom	$11B6FE,$108
Art_11B806:	; DMA source to VRAM
	incrom	$11B806,$1E0
Art_11B9E6:	; DMA source to VRAM
	incrom	$11B9E6,$168
Art_11BB4E:	; DMA source to VRAM
	incrom	$11BB4E,$200
Art_11BD4E:	; DMA source to VRAM
	incrom	$11BD4E,$168
Art_11BEB6:	; DMA source to VRAM
	incrom	$11BEB6,$200
Art_11C0B6:	; DMA source to VRAM
	incrom	$11C0B6,$168
Art_11C21E:	; DMA source to VRAM
	incrom	$11C21E,$1C0
Art_11C3DE:	; DMA source to VRAM
	incrom	$11C3DE,$188
Art_11C566:	; DMA source to VRAM
	incrom	$11C566,$200
Art_11C766:	; DMA source to VRAM
	incrom	$11C766,$128
Art_11C88E:	; DMA source to VRAM
	incrom	$11C88E,$200
Art_11CA8E:	; DMA source to VRAM
	incrom	$11CA8E,$108
Art_11CB96:	; DMA source to VRAM
	incrom	$11CB96,$200
Art_11CD96:	; DMA source to VRAM
	incrom	$11CD96,$108
Art_11CE9E:	; DMA source to VRAM
	incrom	$11CE9E,$180
Art_11D01E:	; DMA source to VRAM
	incrom	$11D01E,$C6
Art_11D0E4:	; DMA source to VRAM
	incrom	$11D0E4,$1EA
Art_11D2CE:	; DMA source to VRAM
	incrom	$11D2CE,$200
Art_11D4CE:	; DMA source to VRAM
	incrom	$11D4CE,$200
Art_11D6CE:	; DMA source to VRAM
	incrom	$11D6CE,$4A
Art_11D718:	; DMA source to VRAM
	incrom	$11D718,$200
Art_11D918:	; DMA source to VRAM
	incrom	$11D918,$200
Art_11DB18:	; DMA source to VRAM
	incrom	$11DB18,$148
Art_11DC60:	; DMA source to VRAM
	incrom	$11DC60,$1C0
Art_11DE20:	; DMA source to VRAM
	incrom	$11DE20,$1E8
Art_11E008:	; DMA source to VRAM
	incrom	$11E008,$200
Art_11E208:	; DMA source to VRAM
	incrom	$11E208,$1AA
Art_11E3B2:	; DMA source to VRAM
	incrom	$11E3B2,$200
Art_11E5B2:	; DMA source to VRAM
	incrom	$11E5B2,$180
Art_11E732:	; DMA source to VRAM
	incrom	$11E732,$C8
Art_11E7FA:	; DMA source to VRAM
	incrom	$11E7FA,$200
Art_11E9FA:	; DMA source to VRAM
	incrom	$11E9FA,$1A8
Art_11EBA2:	; DMA source to VRAM
	incrom	$11EBA2,$180
Art_11ED22:	; DMA source to VRAM
	incrom	$11ED22,$108
Art_11EE2A:	; DMA source to VRAM
	incrom	$11EE2A,$180
Art_11EFAA:	; DMA source to VRAM
	incrom	$11EFAA,$108
Art_11F0B2:	; DMA source to VRAM
	incrom	$11F0B2,$180
Art_11F232:	; DMA source to VRAM
	incrom	$11F232,$106
Art_11F338:	; DMA source to VRAM
	incrom	$11F338,$146
Art_11F47E:	; DMA source to VRAM
	incrom	$11F47E,$1E6
Art_11F664:	; DMA source to VRAM
	incrom	$11F664,$1C6
Art_11F82A:	; DMA source to VRAM
	incrom	$11F82A,$1A6
Art_11F9D0:	; DMA source to VRAM
	incrom	$11F9D0,$1E6
Art_11FBB6:	; DMA source to VRAM
	incrom	$11FBB6,$C6
Art_11FC7C:	; DMA source to VRAM
	incrom	$11FC7C,$C6
Art_11FD42:	; DMA source to VRAM
	incrom	$11FD42,$66
Art_11FDA8:	; DMA source to VRAM
	incrom	$11FDA8,$C6
Art_11FE6E:	; DMA source to VRAM
	incrom	$11FE6E,$66
Art_11FED4:	; DMA source to VRAM; 65% read in the corpus
	incrom	$11FED4,$132
Art_120006:	; DMA source to VRAM
	incrom	$120006,$E6
Art_1200EC:	; DMA source to VRAM
	incrom	$1200EC,$A6
Art_120192:	; DMA source to VRAM
	incrom	$120192,$86
Art_120218:	; DMA source to VRAM
	incrom	$120218,$C6
Art_1202DE:	; DMA source to VRAM
	incrom	$1202DE,$C6
Art_1203A4:	; DMA source to VRAM
	incrom	$1203A4,$C6
Art_12046A:	; DMA source to VRAM
	incrom	$12046A,$46
Art_1204B0:	; DMA source to VRAM
	incrom	$1204B0,$66
Art_120516:	; DMA source to VRAM
	incrom	$120516,$C6
Art_1205DC:	; DMA source to VRAM
	incrom	$1205DC,$1C8
Art_1207A4:	; DMA source to VRAM
	incrom	$1207A4,$120
Art_1208C4:	; DMA source to VRAM
	incrom	$1208C4,$106
Art_1209CA:	; DMA source to VRAM
	incrom	$1209CA,$206
Art_120BD0:	; DMA source to VRAM
	incrom	$120BD0,$206
Art_120DD6:	; DMA source to VRAM
	incrom	$120DD6,$206
Art_120FDC:	; DMA source to VRAM
	incrom	$120FDC,$146
Art_121122:	; DMA source to VRAM
	incrom	$121122,$186
Art_1212A8:	; DMA source to VRAM
	incrom	$1212A8,$1C6
Art_12146E:	; DMA source to VRAM
	incrom	$12146E,$1A6
Art_121614:	; DMA source to VRAM
	incrom	$121614,$1E6
Art_1217FA:	; DMA source to VRAM
	incrom	$1217FA,$186
Art_121980:	; DMA source to VRAM
	incrom	$121980,$188
Art_121B08:	; DMA source to VRAM
	incrom	$121B08,$200
Art_121D08:	; DMA source to VRAM
	incrom	$121D08,$1C8
Art_121ED0:	; DMA source to VRAM
	incrom	$121ED0,$1E0
Art_1220B0:	; DMA source to VRAM
	incrom	$1220B0,$18A
Art_12223A:	; DMA source to VRAM
	incrom	$12223A,$200
Art_12243A:	; DMA source to VRAM
	incrom	$12243A,$200
Art_12263A:	; DMA source to VRAM
	incrom	$12263A,$68
Art_1226A2:	; DMA source to VRAM
	incrom	$1226A2,$200
Art_1228A2:	; DMA source to VRAM
	incrom	$1228A2,$208
Art_122AAA:	; DMA source to VRAM
	incrom	$122AAA,$200
Art_122CAA:	; DMA source to VRAM
	incrom	$122CAA,$18A
Art_122E34:	; DMA source to VRAM
	incrom	$122E34,$1C0
Art_122FF4:	; DMA source to VRAM
	incrom	$122FF4,$180
Art_123174:	; DMA source to VRAM
	incrom	$123174,$10A
Art_12327E:	; DMA source to VRAM
	incrom	$12327E,$1C0
Art_12343E:	; DMA source to VRAM
	incrom	$12343E,$180
Art_1235BE:	; DMA source to VRAM
	incrom	$1235BE,$106
Art_1236C4:	; DMA source to VRAM
	incrom	$1236C4,$186
Art_12384A:	; DMA source to VRAM
	incrom	$12384A,$186
Art_1239D0:	; DMA source to VRAM
	incrom	$1239D0,$146
Art_123B16:	; DMA source to VRAM
	incrom	$123B16,$106
Art_123C1C:	; DMA source to VRAM
	incrom	$123C1C,$106
Art_123D22:	; DMA source to VRAM
	incrom	$123D22,$106
Art_123E28:	; DMA source to VRAM
	incrom	$123E28,$E6
Art_123F0E:	; DMA source to VRAM
	incrom	$123F0E,$106
Art_124014:	; DMA source to VRAM
	incrom	$124014,$E6
Art_1240FA:	; DMA source to VRAM
	incrom	$1240FA,$186
Art_124280:	; DMA source to VRAM
	incrom	$124280,$A6
Art_124326:	; DMA source to VRAM
	incrom	$124326,$126
Art_12444C:	; DMA source to VRAM
	incrom	$12444C,$106
Art_124552:	; DMA source to VRAM
	incrom	$124552,$106
Art_124658:	; DMA source to VRAM
	incrom	$124658,$E6
Art_12473E:	; DMA source to VRAM
	incrom	$12473E,$126
Art_124864:	; DMA source to VRAM
	incrom	$124864,$126
Art_12498A:	; DMA source to VRAM
	incrom	$12498A,$126
Art_124AB0:	; DMA source to VRAM
	incrom	$124AB0,$E0
Art_124B90:	; sprite art block: 9 tiles in 1 chunk(s), DMA cost 9 (frame table entry $1DE664); never displayed in the corpus
	incrom	$124B90,$126
Art_124CB6:	; sprite art block: 9 tiles in 1 chunk(s), DMA cost 9 (frame table entry $1DE66E); never displayed in the corpus
	incrom	$124CB6,$126
Art_124DDC:	; sprite art block: 6 tiles in 1 chunk(s), DMA cost 6 (frame table entry $1DE678); never displayed in the corpus
	incrom	$124DDC,$C6
Art_124EA2:	; sprite art block: 8 tiles in 1 chunk(s), DMA cost 8 (frame table entry $1DE682); never displayed in the corpus
	incrom	$124EA2,$106
Tbl_124FA8:	; read by Spr_SwapFrameArt (pc $007F70), Spr_QueueArtDMA (pc $0080BE)
	incrom	$124FA8,$6
Art_124FAE:	; DMA source to VRAM
	incrom	$124FAE,$208
Art_1251B6:	; DMA source to VRAM
	incrom	$1251B6,$180
Art_125336:	; DMA source to VRAM
	incrom	$125336,$C8
Art_1253FE:	; DMA source to VRAM
	incrom	$1253FE,$1A0
Art_12559E:	; DMA source to VRAM
	incrom	$12559E,$C8
Art_125666:	; DMA source to VRAM
	incrom	$125666,$1C0
Art_125826:	; DMA source to VRAM
	incrom	$125826,$C8
Art_1258EE:	; DMA source to VRAM
	incrom	$1258EE,$1C0
Art_125AAE:	; DMA source to VRAM
	incrom	$125AAE,$C8
Art_125B76:	; DMA source to VRAM
	incrom	$125B76,$200
Art_125D76:	; DMA source to VRAM
	incrom	$125D76,$C8
Art_125E3E:	; DMA source to VRAM
	incrom	$125E3E,$200
Art_12603E:	; DMA source to VRAM
	incrom	$12603E,$C8
Art_126106:	; DMA source to VRAM
	incrom	$126106,$180
Art_126286:	; DMA source to VRAM
	incrom	$126286,$C6
Art_12634C:	; DMA source to VRAM
	incrom	$12634C,$208
Art_126554:	; DMA source to VRAM
	incrom	$126554,$1C0
Art_126714:	; DMA source to VRAM
	incrom	$126714,$108
Art_12681C:	; DMA source to VRAM
	incrom	$12681C,$200
Art_126A1C:	; DMA source to VRAM
	incrom	$126A1C,$108
Art_126B24:	; DMA source to VRAM
	incrom	$126B24,$200
Art_126D24:	; DMA source to VRAM
	incrom	$126D24,$148
Art_126E6C:	; DMA source to VRAM
	incrom	$126E6C,$200
Art_12706C:	; DMA source to VRAM
	incrom	$12706C,$128
Art_127194:	; DMA source to VRAM
	incrom	$127194,$1C0
Art_127354:	; DMA source to VRAM
	incrom	$127354,$C8
Art_12741C:	; DMA source to VRAM
	incrom	$12741C,$200
Art_12761C:	; DMA source to VRAM
	incrom	$12761C,$E8
Art_127704:	; DMA source to VRAM
	incrom	$127704,$200
Art_127904:	; DMA source to VRAM
	incrom	$127904,$E8
Art_1279EC:	; DMA source to VRAM
	incrom	$1279EC,$1E0
Art_127BCC:	; DMA source to VRAM
	incrom	$127BCC,$108
Art_127CD4:	; DMA source to VRAM
	incrom	$127CD4,$1C0
Art_127E94:	; DMA source to VRAM
	incrom	$127E94,$106
Art_127F9A:	; DMA source to VRAM
	incrom	$127F9A,$186
Art_128120:	; DMA source to VRAM
	incrom	$128120,$186
Art_1282A6:	; DMA source to VRAM
	incrom	$1282A6,$186
Art_12842C:	; DMA source to VRAM
	incrom	$12842C,$C6
Art_1284F2:	; DMA source to VRAM
	incrom	$1284F2,$C6
Art_1285B8:	; DMA source to VRAM
	incrom	$1285B8,$C6
Art_12867E:	; DMA source to VRAM
	incrom	$12867E,$C8
Art_128746:	; DMA source to VRAM
	incrom	$128746,$200
Art_128946:	; DMA source to VRAM
	incrom	$128946,$148
Art_128A8E:	; DMA source to VRAM
	incrom	$128A8E,$200
Art_128C8E:	; DMA source to VRAM
	incrom	$128C8E,$126
Art_128DB4:	; DMA source to VRAM
	incrom	$128DB4,$206
Art_128FBA:	; DMA source to VRAM
	incrom	$128FBA,$206
Art_1291C0:	; DMA source to VRAM
	incrom	$1291C0,$206
Art_1293C6:	; DMA source to VRAM
	incrom	$1293C6,$206
Art_1295CC:	; DMA source to VRAM
	incrom	$1295CC,$208
Art_1297D4:	; DMA source to VRAM
	incrom	$1297D4,$200
Art_1299D4:	; DMA source to VRAM
	incrom	$1299D4,$12A
Art_129AFE:	; DMA source to VRAM
	incrom	$129AFE,$200
Art_129CFE:	; DMA source to VRAM
	incrom	$129CFE,$1C0
Art_129EBE:	; DMA source to VRAM
	incrom	$129EBE,$10A
Art_129FC8:	; DMA source to VRAM
	incrom	$129FC8,$200
Art_12A1C8:	; DMA source to VRAM
	incrom	$12A1C8,$1E0
Art_12A3A8:	; DMA source to VRAM
	incrom	$12A3A8,$48
Art_12A3F0:	; DMA source to VRAM
	incrom	$12A3F0,$200
Art_12A5F0:	; DMA source to VRAM
	incrom	$12A5F0,$128
Art_12A718:	; DMA source to VRAM
	incrom	$12A718,$200
Art_12A918:	; DMA source to VRAM
	incrom	$12A918,$128
Art_12AA40:	; DMA source to VRAM
	incrom	$12AA40,$200
Art_12AC40:	; DMA source to VRAM
	incrom	$12AC40,$128
Art_12AD68:	; DMA source to VRAM
	incrom	$12AD68,$200
Art_12AF68:	; DMA source to VRAM
	incrom	$12AF68,$128
Art_12B090:	; DMA source to VRAM
	incrom	$12B090,$200
Art_12B290:	; DMA source to VRAM
	incrom	$12B290,$126
Art_12B3B6:	; DMA source to VRAM
	incrom	$12B3B6,$146
Art_12B4FC:	; DMA source to VRAM
	incrom	$12B4FC,$126
Art_12B622:	; DMA source to VRAM
	incrom	$12B622,$188
Art_12B7AA:	; DMA source to VRAM
	incrom	$12B7AA,$200
Art_12B9AA:	; DMA source to VRAM
	incrom	$12B9AA,$1E8
Art_12BB92:	; DMA source to VRAM
	incrom	$12BB92,$1E0
Art_12BD72:	; DMA source to VRAM
	incrom	$12BD72,$108
Art_12BE7A:	; DMA source to VRAM
	incrom	$12BE7A,$200
Art_12C07A:	; DMA source to VRAM
	incrom	$12C07A,$1EA
Art_12C264:	; DMA source to VRAM
	incrom	$12C264,$200
Art_12C464:	; DMA source to VRAM
	incrom	$12C464,$1E0
Art_12C644:	; DMA source to VRAM
	incrom	$12C644,$18A
Art_12C7CE:	; DMA source to VRAM
	incrom	$12C7CE,$200
Art_12C9CE:	; DMA source to VRAM
	incrom	$12C9CE,$200
Art_12CBCE:	; DMA source to VRAM
	incrom	$12CBCE,$108
Art_12CCD6:	; DMA source to VRAM
	incrom	$12CCD6,$200
Art_12CED6:	; DMA source to VRAM
	incrom	$12CED6,$1E8
Art_12D0BE:	; DMA source to VRAM
	incrom	$12D0BE,$200
Art_12D2BE:	; DMA source to VRAM
	incrom	$12D2BE,$148
Art_12D406:	; DMA source to VRAM
	incrom	$12D406,$200
Art_12D606:	; DMA source to VRAM
	incrom	$12D606,$148
Art_12D74E:	; DMA source to VRAM
	incrom	$12D74E,$1A0
Art_12D8EE:	; DMA source to VRAM
	incrom	$12D8EE,$108
Art_12D9F6:	; DMA source to VRAM
	incrom	$12D9F6,$200
Art_12DBF6:	; DMA source to VRAM
	incrom	$12DBF6,$12A
Art_12DD20:	; DMA source to VRAM
	incrom	$12DD20,$200
Art_12DF20:	; DMA source to VRAM
	incrom	$12DF20,$200
Art_12E120:	; DMA source to VRAM
	incrom	$12E120,$48
Art_12E168:	; DMA source to VRAM
	incrom	$12E168,$200
Art_12E368:	; DMA source to VRAM
	incrom	$12E368,$208
Art_12E570:	; DMA source to VRAM
	incrom	$12E570,$200
Art_12E770:	; DMA source to VRAM
	incrom	$12E770,$188
Art_12E8F8:	; DMA source to VRAM
	incrom	$12E8F8,$200
Art_12EAF8:	; DMA source to VRAM
	incrom	$12EAF8,$1E8
Art_12ECE0:	; DMA source to VRAM
	incrom	$12ECE0,$200
Art_12EEE0:	; DMA source to VRAM
	incrom	$12EEE0,$128
Art_12F008:	; DMA source to VRAM
	incrom	$12F008,$1A0
Art_12F1A8:	; DMA source to VRAM
	incrom	$12F1A8,$188
Art_12F330:	; DMA source to VRAM
	incrom	$12F330,$1A0
Art_12F4D0:	; DMA source to VRAM
	incrom	$12F4D0,$108
Art_12F5D8:	; DMA source to VRAM
	incrom	$12F5D8,$228
Art_12F800:	; DMA source to VRAM
	incrom	$12F800,$180
Art_12F980:	; DMA source to VRAM
	incrom	$12F980,$106
Art_12FA86:	; DMA source to VRAM
	incrom	$12FA86,$1C0
Art_12FC46:	; sprite art block: 9 tiles in 1 chunk(s), DMA cost 9 (frame table entry $1DE902); never displayed in the corpus
	incrom	$12FC46,$126
Tbl_12FD6C:	; read by Spr_SwapFrameArt (pc $007F70), Spr_QueueArtDMA (pc $0080BE)
	incrom	$12FD6C,$6
Art_12FD72:	; DMA source to VRAM
	incrom	$12FD72,$C6
Art_12FE38:	; DMA source to VRAM
	incrom	$12FE38,$C6
Art_12FEFE:	; DMA source to VRAM
	incrom	$12FEFE,$C6
Art_12FFC4:	; DMA source to VRAM
	incrom	$12FFC4,$C6
Art_13008A:	; DMA source to VRAM
	incrom	$13008A,$C6
Art_130150:	; DMA source to VRAM
	incrom	$130150,$C6
Art_130216:	; DMA source to VRAM
	incrom	$130216,$C6
Art_1302DC:	; DMA source to VRAM
	incrom	$1302DC,$120
Art_1303FC:	; sprite art block: 6 tiles in 1 chunk(s), DMA cost 6 (frame table entry $1DE95C); never displayed in the corpus
	incrom	$1303FC,$C6
Art_1304C2:	; sprite art block: 7 tiles in 1 chunk(s), DMA cost 7 (frame table entry $1DE966); never displayed in the corpus
	incrom	$1304C2,$E6
Art_1305A8:	; sprite art block: 9 tiles in 1 chunk(s), DMA cost 9 (frame table entry $1DE970); never displayed in the corpus
	incrom	$1305A8,$126
Art_1306CE:	; sprite art block: 9 tiles in 1 chunk(s), DMA cost 9 (frame table entry $1DE97A); never displayed in the corpus
	incrom	$1306CE,$126
Art_1307F4:	; sprite art block: 6 tiles in 1 chunk(s), DMA cost 6 (frame table entry $1DE984); never displayed in the corpus
	incrom	$1307F4,$C6
Art_1308BA:	; sprite art block: 7 tiles in 1 chunk(s), DMA cost 7 (frame table entry $1DE98E); never displayed in the corpus
	incrom	$1308BA,$E6
Art_1309A0:	; sprite art block: 6 tiles in 1 chunk(s), DMA cost 6 (frame table entry $1DE998); never displayed in the corpus
	incrom	$1309A0,$C6
Art_130A66:	; sprite art block: 6 tiles in 1 chunk(s), DMA cost 6 (frame table entry $1DE9A2); never displayed in the corpus
	incrom	$130A66,$C6
Art_130B2C:	; sprite art block: 18 tiles in 2 chunk(s), DMA cost 18 (frame table entry $1DE9AC); never displayed in the corpus
	incrom	$130B2C,$248
Art_130D74:	; sprite art block: 18 tiles in 2 chunk(s), DMA cost 18 (frame table entry $1DE9B6); never displayed in the corpus
	incrom	$130D74,$248
Art_130FBC:	; sprite art block: 22 tiles in 2 chunk(s), DMA cost 22 (frame table entry $1DE9C0); never displayed in the corpus
	incrom	$130FBC,$2C8
Art_131284:	; sprite art block: 13 tiles in 1 chunk(s), DMA cost 13 (frame table entry $1DE9CA); never displayed in the corpus
	incrom	$131284,$1A6
Art_13142A:	; sprite art block: 10 tiles in 1 chunk(s), DMA cost 10 (frame table entry $1DE9D4); never displayed in the corpus
	incrom	$13142A,$146
Art_131570:	; sprite art block: 7 tiles in 1 chunk(s), DMA cost 7 (frame table entry $1DE9DE); never displayed in the corpus
	incrom	$131570,$E6
Art_131656:	; sprite art block: 10 tiles in 1 chunk(s), DMA cost 10 (frame table entry $1DE9E8); never displayed in the corpus
	incrom	$131656,$146
Art_13179C:	; sprite art block: 7 tiles in 1 chunk(s), DMA cost 7 (frame table entry $1DE9F2); never displayed in the corpus
	incrom	$13179C,$E6
Art_131882:	; sprite art block: 10 tiles in 1 chunk(s), DMA cost 10 (frame table entry $1DE9FC); never displayed in the corpus
	incrom	$131882,$146
Art_1319C8:	; sprite art block: 9 tiles in 1 chunk(s), DMA cost 9 (frame table entry $1DEA06); never displayed in the corpus
	incrom	$1319C8,$126
Art_131AEE:	; sprite art block: 10 tiles in 1 chunk(s), DMA cost 10 (frame table entry $1DEA10); never displayed in the corpus
	incrom	$131AEE,$146
Art_131C34:	; sprite art block: 9 tiles in 1 chunk(s), DMA cost 9 (frame table entry $1DEA1A); never displayed in the corpus
	incrom	$131C34,$126
Art_131D5A:	; sprite art block: 10 tiles in 1 chunk(s), DMA cost 10 (frame table entry $1DEA24); never displayed in the corpus
	incrom	$131D5A,$146
Art_131EA0:	; sprite art block: 9 tiles in 1 chunk(s), DMA cost 9 (frame table entry $1DEA2E); never displayed in the corpus
	incrom	$131EA0,$126
Art_131FC6:	; sprite art block: 10 tiles in 1 chunk(s), DMA cost 10 (frame table entry $1DEA38); never displayed in the corpus
	incrom	$131FC6,$146
Art_13210C:	; sprite art block: 10 tiles in 1 chunk(s), DMA cost 10 (frame table entry $1DEA42); never displayed in the corpus
	incrom	$13210C,$146
Art_132252:	; sprite art block: 10 tiles in 1 chunk(s), DMA cost 10 (frame table entry $1DEA4C); never displayed in the corpus
	incrom	$132252,$146
Art_132398:	; sprite art block: 8 tiles in 1 chunk(s), DMA cost 8 (frame table entry $1DEA56); never displayed in the corpus
	incrom	$132398,$106
Art_13249E:	; sprite art block: 12 tiles in 1 chunk(s), DMA cost 12 (frame table entry $1DEA60); never displayed in the corpus
	incrom	$13249E,$186
Art_132624:	; sprite art block: 12 tiles in 1 chunk(s), DMA cost 12 (frame table entry $1DEA6A); never displayed in the corpus
	incrom	$132624,$186
Art_1327AA:	; sprite art block: 10 tiles in 1 chunk(s), DMA cost 10 (frame table entry $1DEA74); never displayed in the corpus
	incrom	$1327AA,$146
Art_1328F0:	; sprite art block: 8 tiles in 1 chunk(s), DMA cost 8 (frame table entry $1DEA7E); never displayed in the corpus
	incrom	$1328F0,$106
Art_1329F6:	; sprite art block: 10 tiles in 1 chunk(s), DMA cost 10 (frame table entry $1DEA88); never displayed in the corpus
	incrom	$1329F6,$146
Art_132B3C:	; sprite art block: 10 tiles in 1 chunk(s), DMA cost 10 (frame table entry $1DEA92); never displayed in the corpus
	incrom	$132B3C,$146
Art_132C82:	; sprite art block: 10 tiles in 1 chunk(s), DMA cost 10 (frame table entry $1DEA9C); never displayed in the corpus
	incrom	$132C82,$146
Art_132DC8:	; sprite art block: 8 tiles in 1 chunk(s), DMA cost 8 (frame table entry $1DEAA6); never displayed in the corpus
	incrom	$132DC8,$106
Art_132ECE:	; sprite art block: 8 tiles in 1 chunk(s), DMA cost 8 (frame table entry $1DEAB0); never displayed in the corpus
	incrom	$132ECE,$106
Art_132FD4:	; sprite art block: 8 tiles in 1 chunk(s), DMA cost 8 (frame table entry $1DEABA); never displayed in the corpus
	incrom	$132FD4,$106
Art_1330DA:	; sprite art block: 17 tiles in 2 chunk(s), DMA cost 17 (frame table entry $1DEAC4); never displayed in the corpus
	incrom	$1330DA,$228
Art_133302:	; sprite art block: 16 tiles in 1 chunk(s), DMA cost 16 (frame table entry $1DEACE); never displayed in the corpus
	incrom	$133302,$206
Art_133508:	; sprite art block: 16 tiles in 1 chunk(s), DMA cost 16 (frame table entry $1DEAD8); never displayed in the corpus
	incrom	$133508,$206
Art_13370E:	; sprite art block: 8 tiles in 1 chunk(s), DMA cost 8 (frame table entry $1DEAE2); never displayed in the corpus
	incrom	$13370E,$106
Art_133814:	; sprite art block: 4 tiles in 1 chunk(s), DMA cost 4 (frame table entry $1DEAEC); never displayed in the corpus
	incrom	$133814,$86
Art_13389A:	; sprite art block: 4 tiles in 1 chunk(s), DMA cost 4 (frame table entry $1DEAF6); never displayed in the corpus
	incrom	$13389A,$86
Art_133920:	; sprite art block: 4 tiles in 1 chunk(s), DMA cost 4 (frame table entry $1DEB00); never displayed in the corpus
	incrom	$133920,$86
Art_1339A6:	; sprite art block: 4 tiles in 1 chunk(s), DMA cost 4 (frame table entry $1DEB0A); never displayed in the corpus
	incrom	$1339A6,$86
Art_133A2C:	; sprite art block: 5 tiles in 1 chunk(s), DMA cost 5 (frame table entry $1DEB14); never displayed in the corpus
	incrom	$133A2C,$A6
Art_133AD2:	; sprite art block: 6 tiles in 1 chunk(s), DMA cost 6 (frame table entry $1DEB1E); never displayed in the corpus
	incrom	$133AD2,$C6
Art_133B98:	; sprite art block: 6 tiles in 1 chunk(s), DMA cost 6 (frame table entry $1DEB28); never displayed in the corpus
	incrom	$133B98,$C6
Art_133C5E:	; sprite art block: 9 tiles in 1 chunk(s), DMA cost 9 (frame table entry $1DEB32); never displayed in the corpus
	incrom	$133C5E,$126
Art_133D84:	; sprite art block: 6 tiles in 1 chunk(s), DMA cost 6 (frame table entry $1DEB3C); never displayed in the corpus
	incrom	$133D84,$C6
Art_133E4A:	; sprite art block: 6 tiles in 1 chunk(s), DMA cost 6 (frame table entry $1DEB46); never displayed in the corpus
	incrom	$133E4A,$C6
Art_133F10:	; sprite art block: 8 tiles in 1 chunk(s), DMA cost 8 (frame table entry $1DEB50); never displayed in the corpus
	incrom	$133F10,$106
Art_134016:	; sprite art block: 8 tiles in 1 chunk(s), DMA cost 8 (frame table entry $1DEB5A); never displayed in the corpus
	incrom	$134016,$106
Art_13411C:	; sprite art block: 24 tiles in 2 chunk(s), DMA cost 24 (frame table entry $1DEB64); never displayed in the corpus
	incrom	$13411C,$308
Art_134424:	; sprite art block: 20 tiles in 2 chunk(s), DMA cost 20 (frame table entry $1DEB6E); never displayed in the corpus
	incrom	$134424,$288
Art_1346AC:	; sprite art block: 26 tiles in 2 chunk(s), DMA cost 26 (frame table entry $1DEB78); never displayed in the corpus
	incrom	$1346AC,$348
Art_1349F4:	; sprite art block: 20 tiles in 2 chunk(s), DMA cost 20 (frame table entry $1DEB82); never displayed in the corpus
	incrom	$1349F4,$288
Art_134C7C:	; sprite art block: 20 tiles in 2 chunk(s), DMA cost 20 (frame table entry $1DEB8C); never displayed in the corpus
	incrom	$134C7C,$288
Art_134F04:	; sprite art block: 16 tiles in 1 chunk(s), DMA cost 16 (frame table entry $1DEB96); never displayed in the corpus
	incrom	$134F04,$206
Art_13510A:	; sprite art block: 20 tiles in 2 chunk(s), DMA cost 20 (frame table entry $1DEBA0); never displayed in the corpus
	incrom	$13510A,$288
Art_135392:	; sprite art block: 20 tiles in 2 chunk(s), DMA cost 20 (frame table entry $1DEBAA); never displayed in the corpus
	incrom	$135392,$288
Art_13561A:	; sprite art block: 25 tiles in 2 chunk(s), DMA cost 25 (frame table entry $1DEBB4); never displayed in the corpus
	incrom	$13561A,$328
Art_135942:	; sprite art block: 28 tiles in 2 chunk(s), DMA cost 28 (frame table entry $1DEBBE); never displayed in the corpus
	incrom	$135942,$388
Art_135CCA:	; sprite art block: 23 tiles in 2 chunk(s), DMA cost 23 (frame table entry $1DEBC8); never displayed in the corpus
	incrom	$135CCA,$2E8
Art_135FB2:	; sprite art block: 16 tiles in 1 chunk(s), DMA cost 16 (frame table entry $1DEBD2); never displayed in the corpus
	incrom	$135FB2,$206
Art_1361B8:	; sprite art block: 21 tiles in 2 chunk(s), DMA cost 21 (frame table entry $1DEBDC); never displayed in the corpus
	incrom	$1361B8,$2A8
Tbl_136460:	; read by Spr_SwapFrameArt (pc $007F70), Spr_QueueArtDMA (pc $008098)
	incrom	$136460,$8
Art_136468:	; DMA source to VRAM
	incrom	$136468,$200
Art_136668:	; DMA source to VRAM
	incrom	$136668,$168
Art_1367D0:	; DMA source to VRAM
	incrom	$1367D0,$200
Art_1369D0:	; DMA source to VRAM
	incrom	$1369D0,$148
Art_136B18:	; DMA source to VRAM
	incrom	$136B18,$200
Art_136D18:	; DMA source to VRAM
	incrom	$136D18,$A8
Art_136DC0:	; DMA source to VRAM
	incrom	$136DC0,$200
Art_136FC0:	; DMA source to VRAM
	incrom	$136FC0,$68
Art_137028:	; DMA source to VRAM
	incrom	$137028,$200
Art_137228:	; DMA source to VRAM
	incrom	$137228,$A8
Art_1372D0:	; DMA source to VRAM
	incrom	$1372D0,$200
Art_1374D0:	; DMA source to VRAM
	incrom	$1374D0,$88
Art_137558:	; DMA source to VRAM
	incrom	$137558,$1E0
Art_137738:	; DMA source to VRAM
	incrom	$137738,$88
Art_1377C0:	; DMA source to VRAM
	incrom	$1377C0,$200
Art_1379C0:	; DMA source to VRAM
	incrom	$1379C0,$88
Art_137A48:	; DMA source to VRAM
	incrom	$137A48,$200
Art_137C48:	; DMA source to VRAM
	incrom	$137C48,$E0
Art_137D28:	; sprite art block: 25 tiles in 2 chunk(s), DMA cost 25 (frame table entry $1DEC40); never displayed in the corpus
	incrom	$137D28,$328
Art_138050:	; sprite art block: 28 tiles in 2 chunk(s), DMA cost 28 (frame table entry $1DEC4A); never displayed in the corpus
	incrom	$138050,$388
Tbl_1383D8:	; read by Spr_SwapFrameArt (pc $007F70), Spr_QueueArtDMA (pc $008098)
	incrom	$1383D8,$8
Art_1383E0:	; DMA source to VRAM
	incrom	$1383E0,$200
Art_1385E0:	; DMA source to VRAM
	incrom	$1385E0,$148
Art_138728:	; DMA source to VRAM
	incrom	$138728,$200
Art_138928:	; DMA source to VRAM
	incrom	$138928,$C8
Art_1389F0:	; DMA source to VRAM
	incrom	$1389F0,$200
Art_138BF0:	; DMA source to VRAM
	incrom	$138BF0,$E8
Art_138CD8:	; DMA source to VRAM
	incrom	$138CD8,$200
Art_138ED8:	; DMA source to VRAM
	incrom	$138ED8,$128
Art_139000:	; DMA source to VRAM
	incrom	$139000,$200
Art_139200:	; DMA source to VRAM
	incrom	$139200,$128
Art_139328:	; DMA source to VRAM
	incrom	$139328,$3E8
Art_139710:	; DMA source to VRAM
	incrom	$139710,$1C0
Art_1398D0:	; DMA source to VRAM
	incrom	$1398D0,$108
Art_1399D8:	; DMA source to VRAM
	incrom	$1399D8,$1A0
Art_139B78:	; DMA source to VRAM
	incrom	$139B78,$128
Art_139CA0:	; DMA source to VRAM
	incrom	$139CA0,$200
Art_139EA0:	; DMA source to VRAM
	incrom	$139EA0,$128
Art_139FC8:	; DMA source to VRAM
	incrom	$139FC8,$300
Art_13A2C8:	; sprite art block: 8 tiles in 1 chunk(s), DMA cost 8 (frame table entry $1DECB8); never displayed in the corpus
	incrom	$13A2C8,$106
Art_13A3CE:	; sprite art block: 9 tiles in 1 chunk(s), DMA cost 9 (frame table entry $1DECC2); never displayed in the corpus
	incrom	$13A3CE,$126
Art_13A4F4:	; sprite art block: 22 tiles in 2 chunk(s), DMA cost 22 (frame table entry $1DECCC); never displayed in the corpus
	incrom	$13A4F4,$2C8
Art_13A7BC:	; sprite art block: 24 tiles in 2 chunk(s), DMA cost 24 (frame table entry $1DECD6); never displayed in the corpus
	incrom	$13A7BC,$308
Art_13AAC4:	; sprite art block: 25 tiles in 2 chunk(s), DMA cost 25 (frame table entry $1DECE0); never displayed in the corpus
	incrom	$13AAC4,$328
Art_13ADEC:	; sprite art block: 28 tiles in 2 chunk(s), DMA cost 28 (frame table entry $1DECEA); never displayed in the corpus
	incrom	$13ADEC,$388
Art_13B174:	; sprite art block: 39 tiles in 3 chunk(s), DMA cost 39 (frame table entry $1DECF4); never displayed in the corpus
	incrom	$13B174,$4EA
Art_13B65E:	; sprite art block: 24 tiles in 2 chunk(s), DMA cost 24 (frame table entry $1DECFE); never displayed in the corpus
	incrom	$13B65E,$308
Art_13B966:	; sprite art block: 34 tiles in 3 chunk(s), DMA cost 34 (frame table entry $1DED08); never displayed in the corpus
	incrom	$13B966,$44A
Art_13BDB0:	; sprite art block: 37 tiles in 3 chunk(s), DMA cost 37 (frame table entry $1DED12); never displayed in the corpus
	incrom	$13BDB0,$4AA
Art_13C25A:	; sprite art block: 32 tiles in 2 chunk(s), DMA cost 32 (frame table entry $1DED1C); never displayed in the corpus
	incrom	$13C25A,$408
Art_13C662:	; sprite art block: 27 tiles in 2 chunk(s), DMA cost 27 (frame table entry $1DED26); never displayed in the corpus
	incrom	$13C662,$368
Art_13C9CA:	; sprite art block: 24 tiles in 2 chunk(s), DMA cost 24 (frame table entry $1DED30); never displayed in the corpus
	incrom	$13C9CA,$308
Art_13CCD2:	; sprite art block: 27 tiles in 2 chunk(s), DMA cost 27 (frame table entry $1DED3A); never displayed in the corpus
	incrom	$13CCD2,$368
Art_13D03A:	; sprite art block: 32 tiles in 2 chunk(s), DMA cost 32 (frame table entry $1DED44); never displayed in the corpus
	incrom	$13D03A,$408
Art_13D442:	; sprite art block: 27 tiles in 2 chunk(s), DMA cost 27 (frame table entry $1DED4E); never displayed in the corpus
	incrom	$13D442,$368
Art_13D7AA:	; sprite art block: 27 tiles in 2 chunk(s), DMA cost 27 (frame table entry $1DED58); never displayed in the corpus
	incrom	$13D7AA,$368
Art_13DB12:	; sprite art block: 27 tiles in 2 chunk(s), DMA cost 27 (frame table entry $1DED62); never displayed in the corpus
	incrom	$13DB12,$368
Art_13DE7A:	; sprite art block: 28 tiles in 3 chunk(s), DMA cost 28 (frame table entry $1DED6C); never displayed in the corpus
	incrom	$13DE7A,$38A
Art_13E204:	; sprite art block: 26 tiles in 2 chunk(s), DMA cost 26 (frame table entry $1DED76); never displayed in the corpus
	incrom	$13E204,$348
Art_13E54C:	; sprite art block: 35 tiles in 3 chunk(s), DMA cost 35 (frame table entry $1DED80); never displayed in the corpus
	incrom	$13E54C,$46A
Art_13E9B6:	; sprite art block: 23 tiles in 2 chunk(s), DMA cost 23 (frame table entry $1DED8A); never displayed in the corpus
	incrom	$13E9B6,$2E8
Art_13EC9E:	; sprite art block: 22 tiles in 2 chunk(s), DMA cost 22 (frame table entry $1DED94); never displayed in the corpus
	incrom	$13EC9E,$2C8
Art_13EF66:	; sprite art block: 16 tiles in 1 chunk(s), DMA cost 16 (frame table entry $1DED9E); never displayed in the corpus
	incrom	$13EF66,$206
Art_13F16C:	; sprite art block: 28 tiles in 2 chunk(s), DMA cost 28 (frame table entry $1DEDA8); never displayed in the corpus
	incrom	$13F16C,$388
Art_13F4F4:	; sprite art block: 26 tiles in 2 chunk(s), DMA cost 26 (frame table entry $1DEDB2); never displayed in the corpus
	incrom	$13F4F4,$348
Art_13F83C:	; sprite art block: 24 tiles in 2 chunk(s), DMA cost 24 (frame table entry $1DEDBC); never displayed in the corpus
	incrom	$13F83C,$308
Art_13FB44:	; sprite art block: 26 tiles in 2 chunk(s), DMA cost 26 (frame table entry $1DEDC6); never displayed in the corpus
	incrom	$13FB44,$348
	incrom	$13FE8C,$174
Tbl_140000:	; read by Spr_SwapFrameArt (pc $007F70), Spr_QueueArtDMA (pc $008098)
	incrom	$140000,$8
Art_140008:	; DMA source to VRAM
	incrom	$140008,$200
Art_140208:	; DMA source to VRAM
	incrom	$140208,$E8
Art_1402F0:	; DMA source to VRAM
	incrom	$1402F0,$200
Art_1404F0:	; DMA source to VRAM
	incrom	$1404F0,$108
Art_1405F8:	; DMA source to VRAM
	incrom	$1405F8,$200
Art_1407F8:	; DMA source to VRAM
	incrom	$1407F8,$208
Art_140A00:	; DMA source to VRAM
	incrom	$140A00,$200
Art_140C00:	; DMA source to VRAM
	incrom	$140C00,$148
Art_140D48:	; DMA source to VRAM
	incrom	$140D48,$200
Art_140F48:	; DMA source to VRAM
	incrom	$140F48,$188
Art_1410D0:	; DMA source to VRAM
	incrom	$1410D0,$200
Art_1412D0:	; DMA source to VRAM
	incrom	$1412D0,$100
Art_1413D0:	; sprite art block: 20 tiles in 2 chunk(s), DMA cost 20 (frame table entry $1DEE0C); never displayed in the corpus
	incrom	$1413D0,$288
Art_141658:	; sprite art block: 21 tiles in 2 chunk(s), DMA cost 21 (frame table entry $1DEE16); never displayed in the corpus
	incrom	$141658,$2A8
Art_141900:	; sprite art block: 32 tiles in 2 chunk(s), DMA cost 32 (frame table entry $1DEE20); never displayed in the corpus
	incrom	$141900,$408
Art_141D08:	; sprite art block: 30 tiles in 2 chunk(s), DMA cost 30 (frame table entry $1DEE2A); never displayed in the corpus
	incrom	$141D08,$3C8
Art_1420D0:	; sprite art block: 27 tiles in 2 chunk(s), DMA cost 27 (frame table entry $1DEE34); never displayed in the corpus
	incrom	$1420D0,$368
Art_142438:	; sprite art block: 22 tiles in 2 chunk(s), DMA cost 22 (frame table entry $1DEE3E); never displayed in the corpus
	incrom	$142438,$2C8
Art_142700:	; sprite art block: 24 tiles in 2 chunk(s), DMA cost 24 (frame table entry $1DEE48); never displayed in the corpus
	incrom	$142700,$308
Art_142A08:	; sprite art block: 28 tiles in 2 chunk(s), DMA cost 28 (frame table entry $1DEE52); never displayed in the corpus
	incrom	$142A08,$388
Art_142D90:	; sprite art block: 34 tiles in 3 chunk(s), DMA cost 34 (frame table entry $1DEE5C); never displayed in the corpus
	incrom	$142D90,$44A
Art_1431DA:	; sprite art block: 26 tiles in 2 chunk(s), DMA cost 26 (frame table entry $1DEE66); never displayed in the corpus
	incrom	$1431DA,$348
Art_143522:	; sprite art block: 22 tiles in 2 chunk(s), DMA cost 22 (frame table entry $1DEE70); never displayed in the corpus
	incrom	$143522,$2C8
Tbl_1437EA:	; read by Spr_QueueArtDMA (pc $008098), Spr_SwapFrameArt (pc $007F70)
	incrom	$1437EA,$8
Art_1437F2:	; DMA source to VRAM
	incrom	$1437F2,$200
Art_1439F2:	; DMA source to VRAM
	incrom	$1439F2,$108
Art_143AFA:	; DMA source to VRAM
	incrom	$143AFA,$200
Art_143CFA:	; DMA source to VRAM
	incrom	$143CFA,$48
Art_143D42:	; DMA source to VRAM
	incrom	$143D42,$1A0
Art_143EE2:	; DMA source to VRAM
	incrom	$143EE2,$C8
Art_143FAA:	; DMA source to VRAM
	incrom	$143FAA,$1C0
Art_14416A:	; DMA source to VRAM
	incrom	$14416A,$C8
Art_144232:	; DMA source to VRAM
	incrom	$144232,$1C0
Art_1443F2:	; DMA source to VRAM
	incrom	$1443F2,$CA
Art_1444BC:	; DMA source to VRAM
	incrom	$1444BC,$1E0
Art_14469C:	; DMA source to VRAM
	incrom	$14469C,$180
Art_14481C:	; DMA source to VRAM
	incrom	$14481C,$108
Art_144924:	; DMA source to VRAM
	incrom	$144924,$1A0
Art_144AC4:	; DMA source to VRAM
	incrom	$144AC4,$188
Art_144C4C:	; DMA source to VRAM
	incrom	$144C4C,$1C0
Art_144E0C:	; DMA source to VRAM
	incrom	$144E0C,$108
Art_144F14:	; DMA source to VRAM
	incrom	$144F14,$200
Art_145114:	; DMA source to VRAM
	incrom	$145114,$48
Art_14515C:	; DMA source to VRAM
	incrom	$14515C,$1A0
Art_1452FC:	; DMA source to VRAM
	incrom	$1452FC,$188
Art_145484:	; DMA source to VRAM
	incrom	$145484,$1E0
Art_145664:	; DMA source to VRAM
	incrom	$145664,$148
Art_1457AC:	; DMA source to VRAM
	incrom	$1457AC,$200
Art_1459AC:	; DMA source to VRAM
	incrom	$1459AC,$68
Art_145A14:	; DMA source to VRAM
	incrom	$145A14,$200
Art_145C14:	; DMA source to VRAM
	incrom	$145C14,$108
Art_145D1C:	; DMA source to VRAM
	incrom	$145D1C,$200
Art_145F1C:	; DMA source to VRAM
	incrom	$145F1C,$88
Art_145FA4:	; DMA source to VRAM
	incrom	$145FA4,$200
Art_1461A4:	; DMA source to VRAM
	incrom	$1461A4,$C6
Art_14626A:	; DMA source to VRAM
	incrom	$14626A,$1E6
Art_146450:	; DMA source to VRAM
	incrom	$146450,$1EA
Art_14663A:	; DMA source to VRAM
	incrom	$14663A,$200
Art_14683A:	; DMA source to VRAM
	incrom	$14683A,$1E0
Art_146A1A:	; DMA source to VRAM
	incrom	$146A1A,$14A
Art_146B64:	; DMA source to VRAM
	incrom	$146B64,$200
Art_146D64:	; DMA source to VRAM
	incrom	$146D64,$200
Art_146F64:	; DMA source to VRAM
	incrom	$146F64,$28
Art_146F8C:	; DMA source to VRAM
	incrom	$146F8C,$1A0
Art_14712C:	; DMA source to VRAM
	incrom	$14712C,$188
Art_1472B4:	; DMA source to VRAM
	incrom	$1472B4,$180
Art_147434:	; DMA source to VRAM
	incrom	$147434,$188
Art_1475BC:	; DMA source to VRAM
	incrom	$1475BC,$200
Art_1477BC:	; DMA source to VRAM
	incrom	$1477BC,$148
Art_147904:	; DMA source to VRAM
	incrom	$147904,$1A0
Art_147AA4:	; DMA source to VRAM
	incrom	$147AA4,$120
Art_147BC4:	; sprite art block: 17 tiles in 2 chunk(s), DMA cost 17 (frame table entry $1DEF60); never displayed in the corpus
	incrom	$147BC4,$228
Art_147DEC:	; sprite art block: 20 tiles in 2 chunk(s), DMA cost 20 (frame table entry $1DEF6A); never displayed in the corpus
	incrom	$147DEC,$288
Art_148074:	; sprite art block: 14 tiles in 1 chunk(s), DMA cost 14 (frame table entry $1DEF74); never displayed in the corpus
	incrom	$148074,$1C6
Art_14823A:	; sprite art block: 10 tiles in 1 chunk(s), DMA cost 10 (frame table entry $1DEF7E); never displayed in the corpus
	incrom	$14823A,$146
Art_148380:	; sprite art block: 8 tiles in 1 chunk(s), DMA cost 8 (frame table entry $1DEF88); never displayed in the corpus
	incrom	$148380,$106
Art_148486:	; sprite art block: 10 tiles in 1 chunk(s), DMA cost 10 (frame table entry $1DEF92); never displayed in the corpus
	incrom	$148486,$146
Art_1485CC:	; sprite art block: 9 tiles in 1 chunk(s), DMA cost 9 (frame table entry $1DEF9C); never displayed in the corpus
	incrom	$1485CC,$126
Art_1486F2:	; sprite art block: 7 tiles in 1 chunk(s), DMA cost 7 (frame table entry $1DEFA6); never displayed in the corpus
	incrom	$1486F2,$E6
Tbl_1487D8:	; read by Spr_SwapFrameArt (pc $007F70), Spr_QueueArtDMA (pc $0080BE)
	incrom	$1487D8,$6
Art_1487DE:	; DMA source to VRAM
	incrom	$1487DE,$C6
Art_1488A4:	; DMA source to VRAM
	incrom	$1488A4,$86
Art_14892A:	; DMA source to VRAM
	incrom	$14892A,$66
Art_148990:	; DMA source to VRAM
	incrom	$148990,$66
Art_1489F6:	; DMA source to VRAM
	incrom	$1489F6,$66
Art_148A5C:	; DMA source to VRAM
	incrom	$148A5C,$66
Art_148AC2:	; DMA source to VRAM
	incrom	$148AC2,$86
Art_148B48:	; DMA source to VRAM
	incrom	$148B48,$80
Art_148BC8:	; sprite art block: 4 tiles in 1 chunk(s), DMA cost 4 (frame table entry $1DF000); never displayed in the corpus
	incrom	$148BC8,$86
Art_148C4E:	; sprite art block: 2 tiles in 1 chunk(s), DMA cost 2 (frame table entry $1DF00A); never displayed in the corpus
	incrom	$148C4E,$46
Art_148C94:	; sprite art block: 6 tiles in 1 chunk(s), DMA cost 6 (frame table entry $1DF014); never displayed in the corpus
	incrom	$148C94,$C6
Art_148D5A:	; sprite art block: 4 tiles in 1 chunk(s), DMA cost 4 (frame table entry $1DF01E); never displayed in the corpus
	incrom	$148D5A,$86
Art_148DE0:	; sprite art block: 2 tiles in 1 chunk(s), DMA cost 2 (frame table entry $1DF028); never displayed in the corpus
	incrom	$148DE0,$46
Art_148E26:	; sprite art block: 3 tiles in 1 chunk(s), DMA cost 3 (frame table entry $1DF032); never displayed in the corpus
	incrom	$148E26,$66
Art_148E8C:	; sprite art block: 4 tiles in 1 chunk(s), DMA cost 4 (frame table entry $1DF03C); never displayed in the corpus
	incrom	$148E8C,$86
Art_148F12:	; sprite art block: 9 tiles in 1 chunk(s), DMA cost 9 (frame table entry $1DF046); never displayed in the corpus
	incrom	$148F12,$126
Art_149038:	; sprite art block: 12 tiles in 1 chunk(s), DMA cost 12 (frame table entry $1DF050); never displayed in the corpus
	incrom	$149038,$186
Art_1491BE:	; sprite art block: 16 tiles in 1 chunk(s), DMA cost 16 (frame table entry $1DF05A); never displayed in the corpus
	incrom	$1491BE,$206
Art_1493C4:	; sprite art block: 16 tiles in 1 chunk(s), DMA cost 16 (frame table entry $1DF064); never displayed in the corpus
	incrom	$1493C4,$206
Art_1495CA:	; sprite art block: 16 tiles in 1 chunk(s), DMA cost 16 (frame table entry $1DF06E); never displayed in the corpus
	incrom	$1495CA,$206
Art_1497D0:	; sprite art block: 10 tiles in 1 chunk(s), DMA cost 10 (frame table entry $1DF078); never displayed in the corpus
	incrom	$1497D0,$146
Art_149916:	; sprite art block: 10 tiles in 1 chunk(s), DMA cost 10 (frame table entry $1DF082); never displayed in the corpus
	incrom	$149916,$146
Art_149A5C:	; sprite art block: 10 tiles in 1 chunk(s), DMA cost 10 (frame table entry $1DF08C); never displayed in the corpus
	incrom	$149A5C,$146
Art_149BA2:	; sprite art block: 9 tiles in 1 chunk(s), DMA cost 9 (frame table entry $1DF096); never displayed in the corpus
	incrom	$149BA2,$126
Art_149CC8:	; sprite art block: 7 tiles in 1 chunk(s), DMA cost 7 (frame table entry $1DF0A0); never displayed in the corpus
	incrom	$149CC8,$E6
Art_149DAE:	; sprite art block: 9 tiles in 1 chunk(s), DMA cost 9 (frame table entry $1DF0AA); never displayed in the corpus
	incrom	$149DAE,$126
Art_149ED4:	; sprite art block: 8 tiles in 1 chunk(s), DMA cost 8 (frame table entry $1DF0B4); never displayed in the corpus
	incrom	$149ED4,$106
Art_149FDA:	; sprite art block: 27 tiles in 2 chunk(s), DMA cost 27 (frame table entry $1DF0BE); never displayed in the corpus
	incrom	$149FDA,$368
Art_14A342:	; sprite art block: 26 tiles in 2 chunk(s), DMA cost 26 (frame table entry $1DF0C8); never displayed in the corpus
	incrom	$14A342,$348
Art_14A68A:	; sprite art block: 32 tiles in 3 chunk(s), DMA cost 32 (frame table entry $1DF0D2); never displayed in the corpus
	incrom	$14A68A,$40A
Art_14AA94:	; sprite art block: 24 tiles in 2 chunk(s), DMA cost 24 (frame table entry $1DF0DC); never displayed in the corpus
	incrom	$14AA94,$308
Art_14AD9C:	; sprite art block: 23 tiles in 2 chunk(s), DMA cost 23 (frame table entry $1DF0E6); never displayed in the corpus
	incrom	$14AD9C,$2E8
Art_14B084:	; sprite art block: 32 tiles in 3 chunk(s), DMA cost 32 (frame table entry $1DF0F0); never displayed in the corpus
	incrom	$14B084,$40A
Art_14B48E:	; sprite art block: 23 tiles in 2 chunk(s), DMA cost 23 (frame table entry $1DF0FA); never displayed in the corpus
	incrom	$14B48E,$2E8
Art_14B776:	; sprite art block: 12 tiles in 1 chunk(s), DMA cost 12 (frame table entry $1DF104); never displayed in the corpus
	incrom	$14B776,$186
Art_14B8FC:	; sprite art block: 12 tiles in 1 chunk(s), DMA cost 12 (frame table entry $1DF10E); never displayed in the corpus
	incrom	$14B8FC,$186
Art_14BA82:	; sprite art block: 12 tiles in 1 chunk(s), DMA cost 12 (frame table entry $1DF118); never displayed in the corpus
	incrom	$14BA82,$186
Art_14BC08:	; sprite art block: 6 tiles in 1 chunk(s), DMA cost 6 (frame table entry $1DF122); never displayed in the corpus
	incrom	$14BC08,$C6
Art_14BCCE:	; sprite art block: 6 tiles in 1 chunk(s), DMA cost 6 (frame table entry $1DF12C); never displayed in the corpus
	incrom	$14BCCE,$C6
Art_14BD94:	; sprite art block: 9 tiles in 1 chunk(s), DMA cost 9 (frame table entry $1DF136); never displayed in the corpus
	incrom	$14BD94,$126
Art_14BEBA:	; sprite art block: 6 tiles in 1 chunk(s), DMA cost 6 (frame table entry $1DF140); never displayed in the corpus
	incrom	$14BEBA,$C6
Art_14BF80:	; sprite art block: 6 tiles in 1 chunk(s), DMA cost 6 (frame table entry $1DF14A); never displayed in the corpus
	incrom	$14BF80,$C6
Art_14C046:	; sprite art block: 6 tiles in 1 chunk(s), DMA cost 6 (frame table entry $1DF154); never displayed in the corpus
	incrom	$14C046,$C6
Art_14C10C:	; sprite art block: 8 tiles in 1 chunk(s), DMA cost 8 (frame table entry $1DF15E); never displayed in the corpus
	incrom	$14C10C,$106
Art_14C212:	; sprite art block: 8 tiles in 1 chunk(s), DMA cost 8 (frame table entry $1DF168); never displayed in the corpus
	incrom	$14C212,$106
Art_14C318:	; sprite art block: 6 tiles in 1 chunk(s), DMA cost 6 (frame table entry $1DF172); never displayed in the corpus
	incrom	$14C318,$C6
Art_14C3DE:	; sprite art block: 6 tiles in 1 chunk(s), DMA cost 6 (frame table entry $1DF17C); never displayed in the corpus
	incrom	$14C3DE,$C6
Art_14C4A4:	; sprite art block: 9 tiles in 1 chunk(s), DMA cost 9 (frame table entry $1DF186); never displayed in the corpus
	incrom	$14C4A4,$126
Art_14C5CA:	; sprite art block: 5 tiles in 1 chunk(s), DMA cost 5 (frame table entry $1DF190); never displayed in the corpus
	incrom	$14C5CA,$A6
Art_14C670:	; sprite art block: 6 tiles in 1 chunk(s), DMA cost 6 (frame table entry $1DF19A); never displayed in the corpus
	incrom	$14C670,$C6
Art_14C736:	; sprite art block: 9 tiles in 1 chunk(s), DMA cost 9 (frame table entry $1DF1A4); never displayed in the corpus
	incrom	$14C736,$126
Art_14C85C:	; sprite art block: 6 tiles in 1 chunk(s), DMA cost 6 (frame table entry $1DF1AE); never displayed in the corpus
	incrom	$14C85C,$C6
Art_14C922:	; sprite art block: 6 tiles in 1 chunk(s), DMA cost 6 (frame table entry $1DF1B8); never displayed in the corpus
	incrom	$14C922,$C6
Art_14C9E8:	; sprite art block: 4 tiles in 1 chunk(s), DMA cost 4 (frame table entry $1DF1C2); never displayed in the corpus
	incrom	$14C9E8,$86
Art_14CA6E:	; sprite art block: 7 tiles in 1 chunk(s), DMA cost 7 (frame table entry $1DF1CC); never displayed in the corpus
	incrom	$14CA6E,$E6
Art_14CB54:	; sprite art block: 8 tiles in 1 chunk(s), DMA cost 8 (frame table entry $1DF1D6); never displayed in the corpus
	incrom	$14CB54,$106
Art_14CC5A:	; sprite art block: 6 tiles in 1 chunk(s), DMA cost 6 (frame table entry $1DF1E0); never displayed in the corpus
	incrom	$14CC5A,$C6
Art_14CD20:	; sprite art block: 8 tiles in 1 chunk(s), DMA cost 8 (frame table entry $1DF1EA); never displayed in the corpus
	incrom	$14CD20,$106
Art_14CE26:	; sprite art block: 8 tiles in 1 chunk(s), DMA cost 8 (frame table entry $1DF1F4); never displayed in the corpus
	incrom	$14CE26,$106
Art_14CF2C:	; sprite art block: 8 tiles in 1 chunk(s), DMA cost 8 (frame table entry $1DF1FE); never displayed in the corpus
	incrom	$14CF2C,$106
Art_14D032:	; sprite art block: 6 tiles in 1 chunk(s), DMA cost 6 (frame table entry $1DF208); never displayed in the corpus
	incrom	$14D032,$C6
Art_14D0F8:	; sprite art block: 6 tiles in 1 chunk(s), DMA cost 6 (frame table entry $1DF212); never displayed in the corpus
	incrom	$14D0F8,$C6
Art_14D1BE:	; sprite art block: 6 tiles in 1 chunk(s), DMA cost 6 (frame table entry $1DF21C); never displayed in the corpus
	incrom	$14D1BE,$C6
Art_14D284:	; sprite art block: 4 tiles in 1 chunk(s), DMA cost 4 (frame table entry $1DF226); never displayed in the corpus
	incrom	$14D284,$86
Art_14D30A:	; sprite art block: 21 tiles in 2 chunk(s), DMA cost 21 (frame table entry $1DF230); never displayed in the corpus
	incrom	$14D30A,$2A8
Art_14D5B2:	; sprite art block: 24 tiles in 2 chunk(s), DMA cost 24 (frame table entry $1DF23A); never displayed in the corpus
	incrom	$14D5B2,$308
Art_14D8BA:	; sprite art block: 12 tiles in 1 chunk(s), DMA cost 12 (frame table entry $1DF244); never displayed in the corpus
	incrom	$14D8BA,$186
Art_14DA40:	; sprite art block: 12 tiles in 1 chunk(s), DMA cost 12 (frame table entry $1DF24E); never displayed in the corpus
	incrom	$14DA40,$186
Art_14DBC6:	; sprite art block: 12 tiles in 1 chunk(s), DMA cost 12 (frame table entry $1DF258); never displayed in the corpus
	incrom	$14DBC6,$186
Art_14DD4C:	; sprite art block: 12 tiles in 1 chunk(s), DMA cost 12 (frame table entry $1DF262); never displayed in the corpus
	incrom	$14DD4C,$186
Art_14DED2:	; sprite art block: 12 tiles in 1 chunk(s), DMA cost 12 (frame table entry $1DF26C); never displayed in the corpus
	incrom	$14DED2,$186
Art_14E058:	; sprite art block: 22 tiles in 2 chunk(s), DMA cost 22 (frame table entry $1DF276); never displayed in the corpus
	incrom	$14E058,$2C8
Art_14E320:	; sprite art block: 29 tiles in 2 chunk(s), DMA cost 29 (frame table entry $1DF280); never displayed in the corpus
	incrom	$14E320,$3A8
Art_14E6C8:	; sprite art block: 30 tiles in 2 chunk(s), DMA cost 30 (frame table entry $1DF28A); never displayed in the corpus
	incrom	$14E6C8,$3C8
Art_14EA90:	; sprite art block: 22 tiles in 2 chunk(s), DMA cost 22 (frame table entry $1DF294); never displayed in the corpus
	incrom	$14EA90,$2C8
Art_14ED58:	; sprite art block: 22 tiles in 2 chunk(s), DMA cost 22 (frame table entry $1DF29E); never displayed in the corpus
	incrom	$14ED58,$2C8
Art_14F020:	; sprite art block: 22 tiles in 2 chunk(s), DMA cost 22 (frame table entry $1DF2A8); never displayed in the corpus
	incrom	$14F020,$2C8
Art_14F2E8:	; sprite art block: 22 tiles in 2 chunk(s), DMA cost 22 (frame table entry $1DF2B2); never displayed in the corpus
	incrom	$14F2E8,$2C8
Art_14F5B0:	; sprite art block: 22 tiles in 2 chunk(s), DMA cost 22 (frame table entry $1DF2BC); never displayed in the corpus
	incrom	$14F5B0,$2C8
Art_14F878:	; sprite art block: 12 tiles in 1 chunk(s), DMA cost 12 (frame table entry $1DF2C6); never displayed in the corpus
	incrom	$14F878,$186
Art_14F9FE:	; sprite art block: 12 tiles in 1 chunk(s), DMA cost 12 (frame table entry $1DF2D0); never displayed in the corpus
	incrom	$14F9FE,$186
Art_14FB84:	; sprite art block: 12 tiles in 1 chunk(s), DMA cost 12 (frame table entry $1DF2DA); never displayed in the corpus
	incrom	$14FB84,$186
Art_14FD0A:	; sprite art block: 16 tiles in 1 chunk(s), DMA cost 16 (frame table entry $1DF2E4); never displayed in the corpus
	incrom	$14FD0A,$206
Art_14FF10:	; sprite art block: 18 tiles in 2 chunk(s), DMA cost 18 (frame table entry $1DF2EE); never displayed in the corpus
	incrom	$14FF10,$248
Art_150158:	; sprite art block: 19 tiles in 2 chunk(s), DMA cost 19 (frame table entry $1DF2F8); never displayed in the corpus
	incrom	$150158,$268
Art_1503C0:	; sprite art block: 24 tiles in 2 chunk(s), DMA cost 24 (frame table entry $1DF302); never displayed in the corpus
	incrom	$1503C0,$308
Art_1506C8:	; sprite art block: 22 tiles in 2 chunk(s), DMA cost 22 (frame table entry $1DF30C); never displayed in the corpus
	incrom	$1506C8,$2C8
Art_150990:	; sprite art block: 21 tiles in 2 chunk(s), DMA cost 21 (frame table entry $1DF316); never displayed in the corpus
	incrom	$150990,$2A8
Art_150C38:	; sprite art block: 19 tiles in 2 chunk(s), DMA cost 19 (frame table entry $1DF320); never displayed in the corpus
	incrom	$150C38,$268
Art_150EA0:	; sprite art block: 18 tiles in 2 chunk(s), DMA cost 18 (frame table entry $1DF32A); never displayed in the corpus
	incrom	$150EA0,$248
Art_1510E8:	; sprite art block: 16 tiles in 1 chunk(s), DMA cost 16 (frame table entry $1DF334); never displayed in the corpus
	incrom	$1510E8,$206
Art_1512EE:	; sprite art block: 22 tiles in 2 chunk(s), DMA cost 22 (frame table entry $1DF33E); never displayed in the corpus
	incrom	$1512EE,$2C8
Art_1515B6:	; sprite art block: 22 tiles in 2 chunk(s), DMA cost 22 (frame table entry $1DF348); never displayed in the corpus
	incrom	$1515B6,$2C8
Art_15187E:	; sprite art block: 24 tiles in 2 chunk(s), DMA cost 24 (frame table entry $1DF352); never displayed in the corpus
	incrom	$15187E,$308
Art_151B86:	; sprite art block: 22 tiles in 2 chunk(s), DMA cost 22 (frame table entry $1DF35C); never displayed in the corpus
	incrom	$151B86,$2C8
Art_151E4E:	; sprite art block: 22 tiles in 2 chunk(s), DMA cost 22 (frame table entry $1DF366); never displayed in the corpus
	incrom	$151E4E,$2C8
Art_152116:	; sprite art block: 23 tiles in 2 chunk(s), DMA cost 23 (frame table entry $1DF370); never displayed in the corpus
	incrom	$152116,$2E8
Art_1523FE:	; sprite art block: 24 tiles in 2 chunk(s), DMA cost 24 (frame table entry $1DF37A); never displayed in the corpus
	incrom	$1523FE,$308
Art_152706:	; sprite art block: 22 tiles in 2 chunk(s), DMA cost 22 (frame table entry $1DF384); never displayed in the corpus
	incrom	$152706,$2C8
Art_1529CE:	; sprite art block: 21 tiles in 2 chunk(s), DMA cost 21 (frame table entry $1DF38E); never displayed in the corpus
	incrom	$1529CE,$2A8
Art_152C76:	; sprite art block: 25 tiles in 2 chunk(s), DMA cost 25 (frame table entry $1DF398); never displayed in the corpus
	incrom	$152C76,$328
Art_152F9E:	; sprite art block: 21 tiles in 2 chunk(s), DMA cost 21 (frame table entry $1DF3A2); never displayed in the corpus
	incrom	$152F9E,$2A8
Art_153246:	; sprite art block: 22 tiles in 2 chunk(s), DMA cost 22 (frame table entry $1DF3AC); never displayed in the corpus
	incrom	$153246,$2C8
Art_15350E:	; sprite art block: 18 tiles in 2 chunk(s), DMA cost 18 (frame table entry $1DF3B6); never displayed in the corpus
	incrom	$15350E,$248
Art_153756:	; sprite art block: 21 tiles in 2 chunk(s), DMA cost 21 (frame table entry $1DF3C0); never displayed in the corpus
	incrom	$153756,$2A8
Art_1539FE:	; sprite art block: 24 tiles in 2 chunk(s), DMA cost 24 (frame table entry $1DF3CA); never displayed in the corpus
	incrom	$1539FE,$308
Art_153D06:	; sprite art block: 24 tiles in 2 chunk(s), DMA cost 24 (frame table entry $1DF3D4); never displayed in the corpus
	incrom	$153D06,$308
Art_15400E:	; sprite art block: 24 tiles in 2 chunk(s), DMA cost 24 (frame table entry $1DF3DE); never displayed in the corpus
	incrom	$15400E,$308
Art_154316:	; sprite art block: 24 tiles in 2 chunk(s), DMA cost 24 (frame table entry $1DF3E8); never displayed in the corpus
	incrom	$154316,$308
Art_15461E:	; sprite art block: 31 tiles in 2 chunk(s), DMA cost 31 (frame table entry $1DF3F2); never displayed in the corpus
	incrom	$15461E,$3E8
Art_154A06:	; sprite art block: 30 tiles in 2 chunk(s), DMA cost 30 (frame table entry $1DF3FC); never displayed in the corpus
	incrom	$154A06,$3C8
Art_154DCE:	; sprite art block: 31 tiles in 2 chunk(s), DMA cost 31 (frame table entry $1DF406); never displayed in the corpus
	incrom	$154DCE,$3E8
Art_1551B6:	; sprite art block: 27 tiles in 2 chunk(s), DMA cost 27 (frame table entry $1DF410); never displayed in the corpus
	incrom	$1551B6,$368
Art_15551E:	; sprite art block: 26 tiles in 2 chunk(s), DMA cost 26 (frame table entry $1DF41A); never displayed in the corpus
	incrom	$15551E,$348
Art_155866:	; sprite art block: 24 tiles in 2 chunk(s), DMA cost 24 (frame table entry $1DF424); never displayed in the corpus
	incrom	$155866,$308
Art_155B6E:	; sprite art block: 24 tiles in 2 chunk(s), DMA cost 24 (frame table entry $1DF42E); never displayed in the corpus
	incrom	$155B6E,$308
Art_155E76:	; sprite art block: 24 tiles in 2 chunk(s), DMA cost 24 (frame table entry $1DF438); never displayed in the corpus
	incrom	$155E76,$308
Scr4_15617E:	; a4-VM script: read by the a4 VM's handlers through a4
	incrom	$15617E,$6
Art_156184:	; DMA source to VRAM
	incrom	$156184,$86
Art_15620A:	; DMA source to VRAM
	incrom	$15620A,$126
Art_156330:	; DMA source to VRAM
	incrom	$156330,$186
Art_1564B6:	; DMA source to VRAM
	incrom	$1564B6,$206
Art_1566BC:	; DMA source to VRAM
	incrom	$1566BC,$206
Art_1568C2:	; DMA source to VRAM
	incrom	$1568C2,$206
Art_156AC8:	; DMA source to VRAM
	incrom	$156AC8,$206
Art_156CCE:	; DMA source to VRAM
	incrom	$156CCE,$186
Art_156E54:	; DMA source to VRAM
	incrom	$156E54,$206
Art_15705A:	; DMA source to VRAM
	incrom	$15705A,$206
Art_157260:	; DMA source to VRAM
	incrom	$157260,$206
Art_157466:	; DMA source to VRAM
	incrom	$157466,$206
Art_15766C:	; DMA source to VRAM
	incrom	$15766C,$86
Art_1576F2:	; DMA source to VRAM
	incrom	$1576F2,$126
Art_157818:	; DMA source to VRAM
	incrom	$157818,$126
Art_15793E:	; DMA source to VRAM
	incrom	$15793E,$206
Art_157B44:	; DMA source to VRAM
	incrom	$157B44,$206
Art_157D4A:	; DMA source to VRAM
	incrom	$157D4A,$206
Art_157F50:	; DMA source to VRAM
	incrom	$157F50,$206
Art_158156:	; DMA source to VRAM
	incrom	$158156,$206
Art_15835C:	; DMA source to VRAM
	incrom	$15835C,$206
Art_158562:	; DMA source to VRAM
	incrom	$158562,$206
Art_158768:	; DMA source to VRAM
	incrom	$158768,$1C6
Art_15892E:	; DMA source to VRAM
	incrom	$15892E,$1C6
Art_158AF4:	; DMA source to VRAM
	incrom	$158AF4,$86
Art_158B7A:	; DMA source to VRAM
	incrom	$158B7A,$126
Art_158CA0:	; DMA source to VRAM
	incrom	$158CA0,$186
Art_158E26:	; DMA source to VRAM
	incrom	$158E26,$206
Art_15902C:	; DMA source to VRAM
	incrom	$15902C,$206
Art_159232:	; DMA source to VRAM
	incrom	$159232,$206
Art_159438:	; DMA source to VRAM
	incrom	$159438,$206
Art_15963E:	; DMA source to VRAM
	incrom	$15963E,$186
Art_1597C4:	; DMA source to VRAM
	incrom	$1597C4,$206
Art_1599CA:	; DMA source to VRAM
	incrom	$1599CA,$206
Art_159BD0:	; DMA source to VRAM
	incrom	$159BD0,$206
Art_159DD6:	; DMA source to VRAM
	incrom	$159DD6,$208
Art_159FDE:	; DMA source to VRAM
	incrom	$159FDE,$200
Art_15A1DE:	; DMA source to VRAM
	incrom	$15A1DE,$12A
Art_15A308:	; DMA source to VRAM
	incrom	$15A308,$200
Art_15A508:	; DMA source to VRAM
	incrom	$15A508,$200
Art_15A708:	; DMA source to VRAM
	incrom	$15A708,$28
Art_15A730:	; DMA source to VRAM
	incrom	$15A730,$200
Art_15A930:	; DMA source to VRAM
	incrom	$15A930,$126
Art_15AA56:	; DMA source to VRAM
	incrom	$15AA56,$200
Art_15AC56:	; sprite art block: 31 tiles in 2 chunk(s), DMA cost 31 (frame table entry $1DF758); never displayed in the corpus
	incrom	$15AC56,$3E8
Scr4_15B03E:	; a4-VM script: read by the a4 VM's handlers through a4
	incrom	$15B03E,$6
Art_15B044:	; DMA source to VRAM
	incrom	$15B044,$126
Art_15B16A:	; DMA source to VRAM
	incrom	$15B16A,$126
Art_15B290:	; DMA source to VRAM
	incrom	$15B290,$186
Art_15B416:	; DMA source to VRAM
	incrom	$15B416,$186
Art_15B59C:	; DMA source to VRAM
	incrom	$15B59C,$186
Art_15B722:	; DMA source to VRAM
	incrom	$15B722,$146
Art_15B868:	; DMA source to VRAM
	incrom	$15B868,$1C6
Art_15BA2E:	; DMA source to VRAM
	incrom	$15BA2E,$1C6
Art_15BBF4:	; DMA source to VRAM
	incrom	$15BBF4,$186
Art_15BD7A:	; DMA source to VRAM
	incrom	$15BD7A,$126
Art_15BEA0:	; DMA source to VRAM
	incrom	$15BEA0,$206
Art_15C0A6:	; DMA source to VRAM
	incrom	$15C0A6,$206
Art_15C2AC:	; DMA source to VRAM
	incrom	$15C2AC,$206
Art_15C4B2:	; DMA source to VRAM
	incrom	$15C4B2,$206
Art_15C6B8:	; DMA source to VRAM
	incrom	$15C6B8,$206
Art_15C8BE:	; DMA source to VRAM
	incrom	$15C8BE,$206
Art_15CAC4:	; DMA source to VRAM
	incrom	$15CAC4,$206
Art_15CCCA:	; DMA source to VRAM
	incrom	$15CCCA,$186
Art_15CE50:	; DMA source to VRAM
	incrom	$15CE50,$126
Art_15CF76:	; DMA source to VRAM
	incrom	$15CF76,$208
Art_15D17E:	; DMA source to VRAM
	incrom	$15D17E,$200
Art_15D37E:	; DMA source to VRAM
	incrom	$15D37E,$48
Art_15D3C6:	; DMA source to VRAM
	incrom	$15D3C6,$200
Art_15D5C6:	; DMA source to VRAM
	incrom	$15D5C6,$68
Art_15D62E:	; DMA source to VRAM
	incrom	$15D62E,$200
Art_15D82E:	; DMA source to VRAM
	incrom	$15D82E,$48
Art_15D876:	; DMA source to VRAM
	incrom	$15D876,$200
Art_15DA76:	; DMA source to VRAM
	incrom	$15DA76,$68
Art_15DADE:	; DMA source to VRAM
	incrom	$15DADE,$200
Art_15DCDE:	; DMA source to VRAM
	incrom	$15DCDE,$66
Art_15DD44:	; DMA source to VRAM
	incrom	$15DD44,$206
Art_15DF4A:	; DMA source to VRAM
	incrom	$15DF4A,$180
Art_15E0CA:	; sprite art block: 17 tiles in 2 chunk(s), DMA cost 17 (frame table entry $1DF8C0); never displayed in the corpus
	incrom	$15E0CA,$228
Art_15E2F2:	; sprite art block: 19 tiles in 2 chunk(s), DMA cost 19 (frame table entry $1DF8CA); never displayed in the corpus
	incrom	$15E2F2,$268
Art_15E55A:	; sprite art block: 19 tiles in 2 chunk(s), DMA cost 19 (frame table entry $1DF8D4); never displayed in the corpus
	incrom	$15E55A,$268
Art_15E7C2:	; sprite art block: 18 tiles in 2 chunk(s), DMA cost 18 (frame table entry $1DF8DE); never displayed in the corpus
	incrom	$15E7C2,$248
Art_15EA0A:	; sprite art block: 16 tiles in 1 chunk(s), DMA cost 16 (frame table entry $1DF8E8); never displayed in the corpus
	incrom	$15EA0A,$206
Art_15EC10:	; sprite art block: 10 tiles in 1 chunk(s), DMA cost 10 (frame table entry $1DF8F2); never displayed in the corpus
	incrom	$15EC10,$146
Art_15ED56:	; sprite art block: 10 tiles in 1 chunk(s), DMA cost 10 (frame table entry $1DF8FC); never displayed in the corpus
	incrom	$15ED56,$146
Art_15EE9C:	; sprite art block: 3 tiles in 1 chunk(s), DMA cost 3 (frame table entry $1DF906); never displayed in the corpus
	incrom	$15EE9C,$66
Scr4_15EF02:	; a4-VM script: read by the a4 VM's handlers through a4
	incrom	$15EF02,$8
Art_15EF0A:	; DMA source to VRAM
	incrom	$15EF0A,$200
Art_15F10A:	; DMA source to VRAM
	incrom	$15F10A,$208
Art_15F312:	; DMA source to VRAM
	incrom	$15F312,$200
Art_15F512:	; DMA source to VRAM
	incrom	$15F512,$1C0
Art_15F6D2:	; sprite art block: 24 tiles in 2 chunk(s), DMA cost 24 (frame table entry $1DFA00); never displayed in the corpus
	incrom	$15F6D2,$308
Scr4_15F9DA:	; a4-VM script: read by the a4 VM's handlers through a4
	incrom	$15F9DA,$6
Art_15F9E0:	; DMA source to VRAM
	incrom	$15F9E0,$186
Art_15FB66:	; DMA source to VRAM
	incrom	$15FB66,$206
Art_15FD6C:	; DMA source to VRAM; 68% read in the corpus
	incrom	$15FD6C,$29A
Art_160006:	; DMA source to VRAM
	incrom	$160006,$1C8
Art_1601CE:	; DMA source to VRAM
	incrom	$1601CE,$1A0
Art_16036E:	; DMA source to VRAM
	incrom	$16036E,$180
Art_1604EE:	; sprite art block: 25 tiles in 2 chunk(s), DMA cost 25 (frame table entry $1DFB72); never displayed in the corpus
	incrom	$1604EE,$328
Scr4_160816:	; a4-VM script: read by the a4 VM's handlers through a4
	incrom	$160816,$6
Art_16081C:	; DMA source to VRAM
	incrom	$16081C,$1E0
Art_1609FC:	; sprite art block: 38 tiles in 3 chunk(s), DMA cost 38 (frame table entry $1DFBCC); never displayed in the corpus
	incrom	$1609FC,$4CA
Scr4_160EC6:	; a4-VM script: read by the a4 VM's handlers through a4
	incrom	$160EC6,$6
Art_160ECC:	; DMA source to VRAM
	incrom	$160ECC,$26
Art_160EF2:	; DMA source to VRAM
	incrom	$160EF2,$26
Art_160F18:	; DMA source to VRAM
	incrom	$160F18,$86
Art_160F9E:	; DMA source to VRAM
	incrom	$160F9E,$86
Art_161024:	; DMA source to VRAM
	incrom	$161024,$86
Art_1610AA:	; DMA source to VRAM
	incrom	$1610AA,$86
Art_161130:	; DMA source to VRAM
	incrom	$161130,$126
Art_161256:	; DMA source to VRAM
	incrom	$161256,$126
Art_16137C:	; DMA source to VRAM
	incrom	$16137C,$126
Art_1614A2:	; DMA source to VRAM
	incrom	$1614A2,$126
Art_1615C8:	; DMA source to VRAM
	incrom	$1615C8,$166
Art_16172E:	; DMA source to VRAM
	incrom	$16172E,$206
Art_161934:	; DMA source to VRAM
	incrom	$161934,$206
Art_161B3A:	; DMA source to VRAM
	incrom	$161B3A,$206
Art_161D40:	; DMA source to VRAM
	incrom	$161D40,$206
Art_161F46:	; DMA source to VRAM
	incrom	$161F46,$86
Art_161FCC:	; DMA source to VRAM
	incrom	$161FCC,$186
Art_162152:	; DMA source to VRAM
	incrom	$162152,$208
Art_16235A:	; DMA source to VRAM
	incrom	$16235A,$200
Art_16255A:	; DMA source to VRAM
	incrom	$16255A,$48
Art_1625A2:	; DMA source to VRAM
	incrom	$1625A2,$200
Art_1627A2:	; DMA source to VRAM
	incrom	$1627A2,$68
Art_16280A:	; DMA source to VRAM
	incrom	$16280A,$200
Art_162A0A:	; DMA source to VRAM
	incrom	$162A0A,$88
Art_162A92:	; DMA source to VRAM
	incrom	$162A92,$200
Art_162C92:	; DMA source to VRAM
	incrom	$162C92,$60
Art_162CF2:	; sprite art block: 2 tiles in 1 chunk(s), DMA cost 2 (frame table entry $1DFD0C); never displayed in the corpus
	incrom	$162CF2,$46
Art_162D38:	; sprite art block: 12 tiles in 1 chunk(s), DMA cost 12 (frame table entry $1DFD16); never displayed in the corpus
	incrom	$162D38,$186
Art_162EBE:	; sprite art block: 14 tiles in 1 chunk(s), DMA cost 14 (frame table entry $1DFD20); never displayed in the corpus
	incrom	$162EBE,$1C6
Art_163084:	; sprite art block: 19 tiles in 2 chunk(s), DMA cost 19 (frame table entry $1DFD2A); never displayed in the corpus
	incrom	$163084,$268
Scr4_1632EC:	; a4-VM script: read by the a4 VM's handlers through a4
	incrom	$1632EC,$6
Art_1632F2:	; DMA source to VRAM
	incrom	$1632F2,$1A6
Art_163498:	; DMA source to VRAM
	incrom	$163498,$C6
Art_16355E:	; DMA source to VRAM
	incrom	$16355E,$188
Art_1636E6:	; DMA source to VRAM
	incrom	$1636E6,$180
Art_163866:	; DMA source to VRAM
	incrom	$163866,$C6
Art_16392C:	; DMA source to VRAM
	incrom	$16392C,$C6
Art_1639F2:	; DMA source to VRAM
	incrom	$1639F2,$C6
Scr4_163AB8:	; a4-VM script: read by the a4 VM's handlers through a4; 46% read in the corpus
	incrom	$163AB8,$1AC
Art_163C64:	; DMA source to VRAM
	incrom	$163C64,$C6
Art_163D2A:	; DMA source to VRAM
	incrom	$163D2A,$146
Art_163E70:	; DMA source to VRAM
	incrom	$163E70,$140
Art_163FB0:	; sprite art block: 28 tiles in 2 chunk(s), DMA cost 28 (frame table entry $1DFDE8); never displayed in the corpus
	incrom	$163FB0,$388
Scr4_164338:	; a4-VM script: read by the a4 VM's handlers through a4
	incrom	$164338,$8
Art_164340:	; DMA source to VRAM
	incrom	$164340,$200
Art_164540:	; DMA source to VRAM
	incrom	$164540,$106
Art_164646:	; DMA source to VRAM
	incrom	$164646,$C6
Art_16470C:	; DMA source to VRAM
	incrom	$16470C,$146
Art_164852:	; DMA source to VRAM
	incrom	$164852,$140
Art_164992:	; sprite art block: 8 tiles in 1 chunk(s), DMA cost 8 (frame table entry $1DFE60); never displayed in the corpus
	incrom	$164992,$106
Tbl_164A98:	; read by Op4_DrawFrame (pc $0068AC), Spr_QueueArtDMA (pc $0080BE), Spr_SwapFrameArt (pc $007F70)
	incrom	$164A98,$6
Art_164A9E:	; DMA source to VRAM
	incrom	$164A9E,$C6
Art_164B64:	; DMA source to VRAM
	incrom	$164B64,$C6
Art_164C2A:	; DMA source to VRAM
	incrom	$164C2A,$C8
Art_164CF2:	; DMA source to VRAM
	incrom	$164CF2,$1E0
Art_164ED2:	; DMA source to VRAM
	incrom	$164ED2,$108
Art_164FDA:	; DMA source to VRAM
	incrom	$164FDA,$200
Art_1651DA:	; DMA source to VRAM
	incrom	$1651DA,$1CA
Art_1653A4:	; DMA source to VRAM
	incrom	$1653A4,$200
Art_1655A4:	; DMA source to VRAM
	incrom	$1655A4,$200
Art_1657A4:	; DMA source to VRAM
	incrom	$1657A4,$68
Art_16580C:	; DMA source to VRAM
	incrom	$16580C,$200
Art_165A0C:	; DMA source to VRAM
	incrom	$165A0C,$188
Art_165B94:	; DMA source to VRAM
	incrom	$165B94,$200
Art_165D94:	; DMA source to VRAM
	incrom	$165D94,$208
Art_165F9C:	; DMA source to VRAM
	incrom	$165F9C,$1E0
Art_16617C:	; DMA source to VRAM
	incrom	$16617C,$1E6
Art_166362:	; DMA source to VRAM
	incrom	$166362,$106
Art_166468:	; DMA source to VRAM
	incrom	$166468,$126
Art_16658E:	; DMA source to VRAM
	incrom	$16658E,$126
Art_1666B4:	; DMA source to VRAM
	incrom	$1666B4,$126
Art_1667DA:	; DMA source to VRAM
	incrom	$1667DA,$126
Art_166900:	; DMA source to VRAM
	incrom	$166900,$1C8
Art_166AC8:	; DMA source to VRAM
	incrom	$166AC8,$200
Art_166CC8:	; DMA source to VRAM
	incrom	$166CC8,$68
Art_166D30:	; DMA source to VRAM
	incrom	$166D30,$200
Art_166F30:	; DMA source to VRAM
	incrom	$166F30,$68
Art_166F98:	; DMA source to VRAM
	incrom	$166F98,$200
Art_167198:	; DMA source to VRAM
	incrom	$167198,$68
Art_167200:	; DMA source to VRAM
	incrom	$167200,$200
Art_167400:	; DMA source to VRAM
	incrom	$167400,$88
Art_167488:	; DMA source to VRAM
	incrom	$167488,$1E0
Art_167668:	; DMA source to VRAM
	incrom	$167668,$1EC
Art_167854:	; DMA source to VRAM
	incrom	$167854,$120
Art_167974:	; DMA source to VRAM
	incrom	$167974,$120
Art_167A94:	; DMA source to VRAM
	incrom	$167A94,$120
Art_167BB4:	; DMA source to VRAM
	incrom	$167BB4,$12C
Art_167CE0:	; DMA source to VRAM
	incrom	$167CE0,$200
Art_167EE0:	; DMA source to VRAM
	incrom	$167EE0,$200
Art_1680E0:	; DMA source to VRAM
	incrom	$1680E0,$200
Art_1682E0:	; DMA source to VRAM
	incrom	$1682E0,$200
Art_1684E0:	; sprite art block: 24 tiles in 2 chunk(s), DMA cost 24 (frame table entry $1E011C); never displayed in the corpus
	incrom	$1684E0,$308
Scr4_1687E8:	; a4-VM script: read by the a4 VM's handlers through a4
	incrom	$1687E8,$6
Art_1687EE:	; DMA source to VRAM
	incrom	$1687EE,$126
Art_168914:	; DMA source to VRAM
	incrom	$168914,$126
Art_168A3A:	; DMA source to VRAM
	incrom	$168A3A,$126
Art_168B60:	; DMA source to VRAM
	incrom	$168B60,$126
Art_168C86:	; DMA source to VRAM
	incrom	$168C86,$126
Art_168DAC:	; DMA source to VRAM
	incrom	$168DAC,$126
Art_168ED2:	; DMA source to VRAM
	incrom	$168ED2,$126
Art_168FF8:	; DMA source to VRAM
	incrom	$168FF8,$126
Art_16911E:	; DMA source to VRAM
	incrom	$16911E,$206
Art_169324:	; DMA source to VRAM
	incrom	$169324,$206
Art_16952A:	; DMA source to VRAM
	incrom	$16952A,$206
Art_169730:	; DMA source to VRAM
	incrom	$169730,$206
Art_169936:	; DMA source to VRAM
	incrom	$169936,$E6
Art_169A1C:	; DMA source to VRAM
	incrom	$169A1C,$186
Art_169BA2:	; DMA source to VRAM
	incrom	$169BA2,$108
Art_169CAA:	; DMA source to VRAM
	incrom	$169CAA,$200
Art_169EAA:	; DMA source to VRAM
	incrom	$169EAA,$206
Art_16A0B0:	; DMA source to VRAM
	incrom	$16A0B0,$C6
Art_16A176:	; DMA source to VRAM
	incrom	$16A176,$200
Art_16A376:	; sprite art block: 16 tiles in 1 chunk(s), DMA cost 16 (frame table entry $1E0360); never displayed in the corpus
	incrom	$16A376,$206
Art_16A57C:	; sprite art block: 16 tiles in 1 chunk(s), DMA cost 16 (frame table entry $1E0388); never displayed in the corpus
	incrom	$16A57C,$206
Art_16A782:	; sprite art block: 16 tiles in 1 chunk(s), DMA cost 16 (frame table entry $1E03B0); never displayed in the corpus
	incrom	$16A782,$206
Art_16A988:	; sprite art block: 16 tiles in 1 chunk(s), DMA cost 16 (frame table entry $1E03D8); never displayed in the corpus
	incrom	$16A988,$206
Tbl_16AB8E:	; read by Spr_QueueArtDMA (pc $008098), Op4_DrawFrame (pc $0068AC), Spr_SwapFrameArt (pc $007F70)
	incrom	$16AB8E,$8
Art_16AB96:	; DMA source to VRAM
	incrom	$16AB96,$1E0
Art_16AD76:	; DMA source to VRAM
	incrom	$16AD76,$1A8
Art_16AF1E:	; DMA source to VRAM
	incrom	$16AF1E,$1A0
Art_16B0BE:	; DMA source to VRAM
	incrom	$16B0BE,$1AC
Art_16B26A:	; DMA source to VRAM
	incrom	$16B26A,$120
Art_16B38A:	; DMA source to VRAM
	incrom	$16B38A,$120
Art_16B4AA:	; DMA source to VRAM
	incrom	$16B4AA,$120
Art_16B5CA:	; DMA source to VRAM
	incrom	$16B5CA,$12C
Art_16B6F6:	; DMA source to VRAM
	incrom	$16B6F6,$120
Art_16B816:	; DMA source to VRAM
	incrom	$16B816,$120
Art_16B936:	; DMA source to VRAM
	incrom	$16B936,$120
Art_16BA56:	; DMA source to VRAM
	incrom	$16BA56,$126
Art_16BB7C:	; DMA source to VRAM
	incrom	$16BB7C,$86
Art_16BC02:	; DMA source to VRAM
	incrom	$16BC02,$A6
Art_16BCA8:	; DMA source to VRAM
	incrom	$16BCA8,$86
Art_16BD2E:	; DMA source to VRAM
	incrom	$16BD2E,$C6
Art_16BDF4:	; DMA source to VRAM
	incrom	$16BDF4,$86
Art_16BE7A:	; DMA source to VRAM
	incrom	$16BE7A,$86
Art_16BF00:	; DMA source to VRAM
	incrom	$16BF00,$C6
Art_16BFC6:	; DMA source to VRAM
	incrom	$16BFC6,$C6
Art_16C08C:	; DMA source to VRAM
	incrom	$16C08C,$C6
Art_16C152:	; DMA source to VRAM
	incrom	$16C152,$86
Art_16C1D8:	; DMA source to VRAM
	incrom	$16C1D8,$C6
Art_16C29E:	; DMA source to VRAM
	incrom	$16C29E,$86
Art_16C324:	; DMA source to VRAM
	incrom	$16C324,$C6
Art_16C3EA:	; DMA source to VRAM
	incrom	$16C3EA,$86
Art_16C470:	; DMA source to VRAM
	incrom	$16C470,$C6
Art_16C536:	; DMA source to VRAM
	incrom	$16C536,$86
Art_16C5BC:	; DMA source to VRAM
	incrom	$16C5BC,$86
Art_16C642:	; DMA source to VRAM
	incrom	$16C642,$86
Art_16C6C8:	; DMA source to VRAM
	incrom	$16C6C8,$86
Art_16C74E:	; DMA source to VRAM
	incrom	$16C74E,$86
Art_16C7D4:	; DMA source to VRAM
	incrom	$16C7D4,$86
Art_16C85A:	; DMA source to VRAM
	incrom	$16C85A,$86
Art_16C8E0:	; DMA source to VRAM
	incrom	$16C8E0,$86
Art_16C966:	; DMA source to VRAM
	incrom	$16C966,$86
Art_16C9EC:	; DMA source to VRAM
	incrom	$16C9EC,$86
Art_16CA72:	; DMA source to VRAM
	incrom	$16CA72,$86
Art_16CAF8:	; DMA source to VRAM
	incrom	$16CAF8,$86
Art_16CB7E:	; DMA source to VRAM
	incrom	$16CB7E,$86
Tbl_16CC04:	; read by Spr_QueueArtDMA (pc $0080BE), Op4_DrawFrame (pc $0068AC), Spr_SwapFrameArt (pc $007F70); 50% read in the corpus
	incrom	$16CC04,$10C
Art_16CD10:	; DMA source to VRAM
	incrom	$16CD10,$86
Art_16CD96:	; DMA source to VRAM
	incrom	$16CD96,$86
Art_16CE1C:	; DMA source to VRAM
	incrom	$16CE1C,$86
Art_16CEA2:	; DMA source to VRAM
	incrom	$16CEA2,$86
Art_16CF28:	; DMA source to VRAM
	incrom	$16CF28,$86
Art_16CFAE:	; DMA source to VRAM
	incrom	$16CFAE,$86
Art_16D034:	; DMA source to VRAM
	incrom	$16D034,$86
Art_16D0BA:	; DMA source to VRAM
	incrom	$16D0BA,$86
Art_16D140:	; DMA source to VRAM
	incrom	$16D140,$86
Art_16D1C6:	; DMA source to VRAM
	incrom	$16D1C6,$86
Art_16D24C:	; DMA source to VRAM
	incrom	$16D24C,$86
Art_16D2D2:	; DMA source to VRAM
	incrom	$16D2D2,$86
Art_16D358:	; DMA source to VRAM
	incrom	$16D358,$86
Art_16D3DE:	; DMA source to VRAM
	incrom	$16D3DE,$126
Art_16D504:	; DMA source to VRAM
	incrom	$16D504,$126
Art_16D62A:	; DMA source to VRAM
	incrom	$16D62A,$C6
Art_16D6F0:	; DMA source to VRAM
	incrom	$16D6F0,$C6
Art_16D7B6:	; DMA source to VRAM
	incrom	$16D7B6,$C6
Art_16D87C:	; DMA source to VRAM
	incrom	$16D87C,$66
Art_16D8E2:	; DMA source to VRAM
	incrom	$16D8E2,$126
Art_16DA08:	; DMA source to VRAM
	incrom	$16DA08,$126
Art_16DB2E:	; DMA source to VRAM
	incrom	$16DB2E,$C6
Art_16DBF4:	; DMA source to VRAM
	incrom	$16DBF4,$C6
Art_16DCBA:	; DMA source to VRAM
	incrom	$16DCBA,$C6
Art_16DD80:	; DMA source to VRAM
	incrom	$16DD80,$126
Art_16DEA6:	; DMA source to VRAM
	incrom	$16DEA6,$126
Art_16DFCC:	; DMA source to VRAM
	incrom	$16DFCC,$C6
Art_16E092:	; DMA source to VRAM
	incrom	$16E092,$C6
Art_16E158:	; DMA source to VRAM
	incrom	$16E158,$C6
Art_16E21E:	; DMA source to VRAM
	incrom	$16E21E,$126
Art_16E344:	; DMA source to VRAM
	incrom	$16E344,$126
Art_16E46A:	; DMA source to VRAM
	incrom	$16E46A,$C6
Art_16E530:	; DMA source to VRAM
	incrom	$16E530,$C6
Art_16E5F6:	; DMA source to VRAM
	incrom	$16E5F6,$C0
Art_16E6B6:	; sprite art block: 9 tiles in 1 chunk(s), DMA cost 9 (frame table entry $1E0766); never displayed in the corpus
	incrom	$16E6B6,$126
Art_16E7DC:	; sprite art block: 9 tiles in 1 chunk(s), DMA cost 9 (frame table entry $1E0770); never displayed in the corpus
	incrom	$16E7DC,$126
Art_16E902:	; sprite art block: 9 tiles in 1 chunk(s), DMA cost 9 (frame table entry $1E077A); never displayed in the corpus
	incrom	$16E902,$126
Art_16EA28:	; sprite art block: 9 tiles in 1 chunk(s), DMA cost 9 (frame table entry $1E0784); never displayed in the corpus
	incrom	$16EA28,$126
Art_16EB4E:	; sprite art block: 9 tiles in 1 chunk(s), DMA cost 9 (frame table entry $1E078E); never displayed in the corpus
	incrom	$16EB4E,$126
Art_16EC74:	; sprite art block: 9 tiles in 1 chunk(s), DMA cost 9 (frame table entry $1E0798); never displayed in the corpus
	incrom	$16EC74,$126
Art_16ED9A:	; sprite art block: 9 tiles in 1 chunk(s), DMA cost 9 (frame table entry $1E07A2); never displayed in the corpus
	incrom	$16ED9A,$126
Tbl_16EEC0:	; read by Spr_QueueArtDMA (pc $0080BE), Spr_SwapFrameArt (pc $007F70), Op4_DrawFrame (pc $0068AC)
	incrom	$16EEC0,$6
Art_16EEC6:	; DMA source to VRAM
	incrom	$16EEC6,$86
Art_16EF4C:	; DMA source to VRAM
	incrom	$16EF4C,$86
Art_16EFD2:	; DMA source to VRAM
	incrom	$16EFD2,$86
Art_16F058:	; DMA source to VRAM
	incrom	$16F058,$86
Art_16F0DE:	; DMA source to VRAM
	incrom	$16F0DE,$86
Art_16F164:	; DMA source to VRAM
	incrom	$16F164,$48
Art_16F1AC:	; DMA source to VRAM
	incrom	$16F1AC,$200
Art_16F3AC:	; DMA source to VRAM
	incrom	$16F3AC,$1A8
Art_16F554:	; DMA source to VRAM
	incrom	$16F554,$200
Art_16F754:	; DMA source to VRAM
	incrom	$16F754,$1A8
Art_16F8FC:	; DMA source to VRAM
	incrom	$16F8FC,$200
Art_16FAFC:	; DMA source to VRAM
	incrom	$16FAFC,$1A8
Art_16FCA4:	; DMA source to VRAM
	incrom	$16FCA4,$200
Art_16FEA4:	; DMA source to VRAM
	incrom	$16FEA4,$1A8
Art_17004C:	; DMA source to VRAM
	incrom	$17004C,$200
Art_17024C:	; DMA source to VRAM
	incrom	$17024C,$1C8
Art_170414:	; DMA source to VRAM
	incrom	$170414,$200
Art_170614:	; DMA source to VRAM
	incrom	$170614,$1C8
Art_1707DC:	; DMA source to VRAM
	incrom	$1707DC,$200
Art_1709DC:	; DMA source to VRAM
	incrom	$1709DC,$1C8
Art_170BA4:	; DMA source to VRAM
	incrom	$170BA4,$200
Art_170DA4:	; DMA source to VRAM
	incrom	$170DA4,$1C6
Art_170F6A:	; DMA source to VRAM
	incrom	$170F6A,$206
Art_171170:	; DMA source to VRAM
	incrom	$171170,$186
Art_1712F6:	; DMA source to VRAM
	incrom	$1712F6,$206
Art_1714FC:	; DMA source to VRAM
	incrom	$1714FC,$186
Art_171682:	; DMA source to VRAM
	incrom	$171682,$C6
Art_171748:	; DMA source to VRAM
	incrom	$171748,$166
Art_1718AE:	; DMA source to VRAM
	incrom	$1718AE,$C6
Art_171974:	; DMA source to VRAM
	incrom	$171974,$C6
Art_171A3A:	; DMA source to VRAM
	incrom	$171A3A,$86
Art_171AC0:	; DMA source to VRAM
	incrom	$171AC0,$146
Art_171C06:	; DMA source to VRAM
	incrom	$171C06,$146
Art_171D4C:	; DMA source to VRAM
	incrom	$171D4C,$186
Art_171ED2:	; DMA source to VRAM
	incrom	$171ED2,$146
Art_172018:	; DMA source to VRAM
	incrom	$172018,$148
Art_172160:	; DMA source to VRAM
	incrom	$172160,$200
Art_172360:	; DMA source to VRAM
	incrom	$172360,$1C8
Art_172528:	; DMA source to VRAM
	incrom	$172528,$200
Art_172728:	; DMA source to VRAM
	incrom	$172728,$C8
Art_1727F0:	; DMA source to VRAM
	incrom	$1727F0,$200
Art_1729F0:	; DMA source to VRAM
	incrom	$1729F0,$108
Art_172AF8:	; DMA source to VRAM
	incrom	$172AF8,$1E0
Art_172CD8:	; DMA source to VRAM
	incrom	$172CD8,$108
Art_172DE0:	; DMA source to VRAM
	incrom	$172DE0,$200
Art_172FE0:	; DMA source to VRAM
	incrom	$172FE0,$A8
Art_173088:	; DMA source to VRAM
	incrom	$173088,$200
Art_173288:	; DMA source to VRAM
	incrom	$173288,$128
Art_1733B0:	; DMA source to VRAM
	incrom	$1733B0,$1E0
Art_173590:	; DMA source to VRAM
	incrom	$173590,$1A6
Art_173736:	; DMA source to VRAM
	incrom	$173736,$106
Art_17383C:	; DMA source to VRAM
	incrom	$17383C,$108
Art_173944:	; DMA source to VRAM
	incrom	$173944,$1E0
Art_173B24:	; DMA source to VRAM
	incrom	$173B24,$108
Art_173C2C:	; DMA source to VRAM
	incrom	$173C2C,$1A0
Art_173DCC:	; DMA source to VRAM
	incrom	$173DCC,$188
Art_173F54:	; DMA source to VRAM
	incrom	$173F54,$1C0
Art_174114:	; DMA source to VRAM
	incrom	$174114,$1CA
Art_1742DE:	; DMA source to VRAM
	incrom	$1742DE,$1E0
Art_1744BE:	; DMA source to VRAM
	incrom	$1744BE,$180
Art_17463E:	; DMA source to VRAM
	incrom	$17463E,$C8
Art_174706:	; DMA source to VRAM
	incrom	$174706,$200
Art_174906:	; DMA source to VRAM
	incrom	$174906,$148
Art_174A4E:	; DMA source to VRAM
	incrom	$174A4E,$200
Art_174C4E:	; DMA source to VRAM
	incrom	$174C4E,$1A8
Art_174DF6:	; DMA source to VRAM
	incrom	$174DF6,$200
Art_174FF6:	; DMA source to VRAM
	incrom	$174FF6,$188
Art_17517E:	; DMA source to VRAM
	incrom	$17517E,$200
Art_17537E:	; DMA source to VRAM
	incrom	$17537E,$188
Art_175506:	; DMA source to VRAM
	incrom	$175506,$200
Art_175706:	; DMA source to VRAM
	incrom	$175706,$128
Art_17582E:	; DMA source to VRAM
	incrom	$17582E,$200
Art_175A2E:	; DMA source to VRAM
	incrom	$175A2E,$100
Art_175B2E:	; sprite art block: 21 tiles in 2 chunk(s), DMA cost 21 (frame table entry $1E09D2); never displayed in the corpus
	incrom	$175B2E,$2A8
Tbl_175DD6:	; read by Spr_QueueArtDMA (pc $008098), Spr_SwapFrameArt (pc $007F70)
	incrom	$175DD6,$8
Art_175DDE:	; DMA source to VRAM
	incrom	$175DDE,$1E0
Art_175FBE:	; DMA source to VRAM
	incrom	$175FBE,$10C
Art_1760CA:	; DMA source to VRAM
	incrom	$1760CA,$1C0
Art_17628A:	; DMA source to VRAM
	incrom	$17628A,$180
Art_17640A:	; DMA source to VRAM
	incrom	$17640A,$1E0
Art_1765EA:	; DMA source to VRAM
	incrom	$1765EA,$C8
Art_1766B2:	; DMA source to VRAM
	incrom	$1766B2,$200
Art_1768B2:	; DMA source to VRAM
	incrom	$1768B2,$1E8
Art_176A9A:	; DMA source to VRAM
	incrom	$176A9A,$200
Art_176C9A:	; DMA source to VRAM
	incrom	$176C9A,$1AA
Art_176E44:	; DMA source to VRAM
	incrom	$176E44,$200
Art_177044:	; DMA source to VRAM
	incrom	$177044,$200
Art_177244:	; DMA source to VRAM
	incrom	$177244,$18A
Art_1773CE:	; DMA source to VRAM
	incrom	$1773CE,$1C0
Art_17758E:	; DMA source to VRAM
	incrom	$17758E,$180
Art_17770E:	; DMA source to VRAM
	incrom	$17770E,$C8
Art_1777D6:	; DMA source to VRAM
	incrom	$1777D6,$1E0
Art_1779B6:	; DMA source to VRAM
	incrom	$1779B6,$108
Art_177ABE:	; DMA source to VRAM
	incrom	$177ABE,$200
Art_177CBE:	; DMA source to VRAM
	incrom	$177CBE,$C8
Art_177D86:	; DMA source to VRAM
	incrom	$177D86,$200
Art_177F86:	; DMA source to VRAM
	incrom	$177F86,$C8
Art_17804E:	; DMA source to VRAM
	incrom	$17804E,$200
Art_17824E:	; DMA source to VRAM
	incrom	$17824E,$68
Art_1782B6:	; DMA source to VRAM
	incrom	$1782B6,$180
Art_178436:	; DMA source to VRAM
	incrom	$178436,$106
Art_17853C:	; DMA source to VRAM
	incrom	$17853C,$66
Art_1785A2:	; DMA source to VRAM
	incrom	$1785A2,$86
Art_178628:	; DMA source to VRAM
	incrom	$178628,$66
Art_17868E:	; DMA source to VRAM
	incrom	$17868E,$126
Art_1787B4:	; DMA source to VRAM
	incrom	$1787B4,$126
Art_1788DA:	; DMA source to VRAM
	incrom	$1788DA,$14A
Art_178A24:	; DMA source to VRAM
	incrom	$178A24,$200
Art_178C24:	; DMA source to VRAM
	incrom	$178C24,$200
Art_178E24:	; DMA source to VRAM
	incrom	$178E24,$EA
Art_178F0E:	; DMA source to VRAM
	incrom	$178F0E,$200
Art_17910E:	; DMA source to VRAM
	incrom	$17910E,$200
Art_17930E:	; DMA source to VRAM
	incrom	$17930E,$EA
Art_1793F8:	; DMA source to VRAM
	incrom	$1793F8,$200
Art_1795F8:	; DMA source to VRAM
	incrom	$1795F8,$200
Art_1797F8:	; DMA source to VRAM
	incrom	$1797F8,$4A
Art_179842:	; DMA source to VRAM
	incrom	$179842,$200
Art_179A42:	; DMA source to VRAM
	incrom	$179A42,$200
Art_179C42:	; DMA source to VRAM
	incrom	$179C42,$1CA
Art_179E0C:	; DMA source to VRAM
	incrom	$179E0C,$200
Art_17A00C:	; DMA source to VRAM
	incrom	$17A00C,$200
Art_17A20C:	; DMA source to VRAM
	incrom	$17A20C,$14A
Art_17A356:	; DMA source to VRAM
	incrom	$17A356,$200
Art_17A556:	; DMA source to VRAM
	incrom	$17A556,$200
Art_17A756:	; DMA source to VRAM
	incrom	$17A756,$8A
Art_17A7E0:	; DMA source to VRAM
	incrom	$17A7E0,$200
Art_17A9E0:	; DMA source to VRAM
	incrom	$17A9E0,$1E0
Art_17ABC0:	; DMA source to VRAM
	incrom	$17ABC0,$EA
Art_17ACAA:	; DMA source to VRAM
	incrom	$17ACAA,$200
Art_17AEAA:	; DMA source to VRAM
	incrom	$17AEAA,$200
Art_17B0AA:	; DMA source to VRAM
	incrom	$17B0AA,$1CA
Art_17B274:	; DMA source to VRAM
	incrom	$17B274,$200
Art_17B474:	; DMA source to VRAM
	incrom	$17B474,$200
Art_17B674:	; DMA source to VRAM
	incrom	$17B674,$18A
Art_17B7FE:	; DMA source to VRAM
	incrom	$17B7FE,$200
Art_17B9FE:	; DMA source to VRAM
	incrom	$17B9FE,$200
Art_17BBFE:	; DMA source to VRAM
	incrom	$17BBFE,$EA
Art_17BCE8:	; DMA source to VRAM
	incrom	$17BCE8,$200
Art_17BEE8:	; DMA source to VRAM
	incrom	$17BEE8,$200
Art_17C0E8:	; DMA source to VRAM
	incrom	$17C0E8,$6A
Art_17C152:	; DMA source to VRAM
	incrom	$17C152,$200
Art_17C352:	; DMA source to VRAM
	incrom	$17C352,$200
Art_17C552:	; DMA source to VRAM
	incrom	$17C552,$6A
Art_17C5BC:	; DMA source to VRAM
	incrom	$17C5BC,$200
Art_17C7BC:	; DMA source to VRAM
	incrom	$17C7BC,$200
Art_17C9BC:	; DMA source to VRAM
	incrom	$17C9BC,$6A
Art_17CA26:	; DMA source to VRAM
	incrom	$17CA26,$200
Art_17CC26:	; DMA source to VRAM
	incrom	$17CC26,$200
Art_17CE26:	; DMA source to VRAM
	incrom	$17CE26,$14A
Art_17CF70:	; DMA source to VRAM
	incrom	$17CF70,$200
Art_17D170:	; DMA source to VRAM
	incrom	$17D170,$200
Art_17D370:	; DMA source to VRAM
	incrom	$17D370,$16A
Art_17D4DA:	; DMA source to VRAM
	incrom	$17D4DA,$200
Art_17D6DA:	; DMA source to VRAM
	incrom	$17D6DA,$200
Art_17D8DA:	; DMA source to VRAM
	incrom	$17D8DA,$14A
Art_17DA24:	; DMA source to VRAM
	incrom	$17DA24,$200
Art_17DC24:	; DMA source to VRAM
	incrom	$17DC24,$1E0
Art_17DE04:	; DMA source to VRAM
	incrom	$17DE04,$CA
Art_17DECE:	; DMA source to VRAM
	incrom	$17DECE,$200
Art_17E0CE:	; DMA source to VRAM
	incrom	$17E0CE,$200
Art_17E2CE:	; DMA source to VRAM
	incrom	$17E2CE,$E0
Art_17E3AE:	; sprite art block: 33 tiles in 3 chunk(s), DMA cost 33 (frame table entry $1E0B3A); never displayed in the corpus
	incrom	$17E3AE,$42A
Art_17E7D8:	; sprite art block: 34 tiles in 3 chunk(s), DMA cost 34 (frame table entry $1E0B44); never displayed in the corpus
	incrom	$17E7D8,$44A
Tbl_17EC22:	; read by Spr_QueueArtDMA (pc $008098), Spr_SwapFrameArt (pc $007F70)
	incrom	$17EC22,$A
Art_17EC2C:	; DMA source to VRAM
	incrom	$17EC2C,$200
Art_17EE2C:	; DMA source to VRAM
	incrom	$17EE2C,$200
Art_17F02C:	; DMA source to VRAM
	incrom	$17F02C,$1AA
Art_17F1D6:	; DMA source to VRAM
	incrom	$17F1D6,$200
Art_17F3D6:	; DMA source to VRAM
	incrom	$17F3D6,$200
Art_17F5D6:	; DMA source to VRAM
	incrom	$17F5D6,$12A
Art_17F700:	; DMA source to VRAM
	incrom	$17F700,$200
Art_17F900:	; DMA source to VRAM
	incrom	$17F900,$200
Art_17FB00:	; DMA source to VRAM
	incrom	$17FB00,$140
	incrom	$17FC40,$3C0
Tbl_180000:	; read by Spr_SwapFrameArt (pc $007F70), Spr_QueueArtDMA (pc $008098), Op4_DrawFrame (pc $0068AC)
	incrom	$180000,$8
Art_180008:	; DMA source to VRAM
	incrom	$180008,$200
Art_180208:	; DMA source to VRAM
	incrom	$180208,$20A
Art_180412:	; DMA source to VRAM
	incrom	$180412,$200
Art_180612:	; DMA source to VRAM
	incrom	$180612,$1E0
Art_1807F2:	; DMA source to VRAM
	incrom	$1807F2,$88
Art_18087A:	; DMA source to VRAM
	incrom	$18087A,$1E0
Art_180A5A:	; DMA source to VRAM
	incrom	$180A5A,$46
Art_180AA0:	; DMA source to VRAM
	incrom	$180AA0,$208
Art_180CA8:	; DMA source to VRAM
	incrom	$180CA8,$200
Art_180EA8:	; DMA source to VRAM
	incrom	$180EA8,$E8
Art_180F90:	; DMA source to VRAM
	incrom	$180F90,$200
Art_181190:	; DMA source to VRAM
	incrom	$181190,$C8
Art_181258:	; DMA source to VRAM
	incrom	$181258,$200
Art_181458:	; DMA source to VRAM
	incrom	$181458,$108
Art_181560:	; DMA source to VRAM
	incrom	$181560,$200
Art_181760:	; DMA source to VRAM
	incrom	$181760,$E8
Art_181848:	; DMA source to VRAM
	incrom	$181848,$200
Art_181A48:	; DMA source to VRAM
	incrom	$181A48,$148
Art_181B90:	; DMA source to VRAM
	incrom	$181B90,$200
Art_181D90:	; DMA source to VRAM
	incrom	$181D90,$C8
Art_181E58:	; DMA source to VRAM
	incrom	$181E58,$200
Art_182058:	; DMA source to VRAM
	incrom	$182058,$C8
Art_182120:	; DMA source to VRAM
	incrom	$182120,$200
Art_182320:	; DMA source to VRAM
	incrom	$182320,$148
Art_182468:	; DMA source to VRAM
	incrom	$182468,$200
Art_182668:	; DMA source to VRAM
	incrom	$182668,$18A
Art_1827F2:	; DMA source to VRAM
	incrom	$1827F2,$1E0
Art_1829D2:	; DMA source to VRAM
	incrom	$1829D2,$180
Art_182B52:	; DMA source to VRAM
	incrom	$182B52,$108
Art_182C5A:	; DMA source to VRAM
	incrom	$182C5A,$200
Art_182E5A:	; DMA source to VRAM
	incrom	$182E5A,$10A
Art_182F64:	; DMA source to VRAM
	incrom	$182F64,$200
Art_183164:	; DMA source to VRAM
	incrom	$183164,$180
Art_1832E4:	; DMA source to VRAM
	incrom	$1832E4,$108
Art_1833EC:	; DMA source to VRAM
	incrom	$1833EC,$200
Art_1835EC:	; DMA source to VRAM
	incrom	$1835EC,$168
Art_183754:	; DMA source to VRAM
	incrom	$183754,$200
Art_183954:	; DMA source to VRAM
	incrom	$183954,$208
Art_183B5C:	; DMA source to VRAM
	incrom	$183B5C,$200
Art_183D5C:	; DMA source to VRAM
	incrom	$183D5C,$168
Art_183EC4:	; DMA source to VRAM
	incrom	$183EC4,$200
Art_1840C4:	; DMA source to VRAM
	incrom	$1840C4,$1C8
Art_18428C:	; DMA source to VRAM
	incrom	$18428C,$200
Art_18448C:	; DMA source to VRAM
	incrom	$18448C,$148
Art_1845D4:	; DMA source to VRAM
	incrom	$1845D4,$200
Art_1847D4:	; DMA source to VRAM
	incrom	$1847D4,$188
Art_18495C:	; DMA source to VRAM
	incrom	$18495C,$180
Art_184ADC:	; DMA source to VRAM
	incrom	$184ADC,$188
Art_184C64:	; DMA source to VRAM
	incrom	$184C64,$200
Art_184E64:	; DMA source to VRAM
	incrom	$184E64,$A6
Art_184F0A:	; DMA source to VRAM
	incrom	$184F0A,$1A6
Art_1850B0:	; DMA source to VRAM
	incrom	$1850B0,$186
Art_185236:	; DMA source to VRAM
	incrom	$185236,$186
Art_1853BC:	; DMA source to VRAM
	incrom	$1853BC,$186
Art_185542:	; DMA source to VRAM
	incrom	$185542,$186
Art_1856C8:	; DMA source to VRAM
	incrom	$1856C8,$186
Art_18584E:	; DMA source to VRAM
	incrom	$18584E,$186
Art_1859D4:	; DMA source to VRAM
	incrom	$1859D4,$120
Art_185AF4:	; sprite art block: 12 tiles in 1 chunk(s), DMA cost 12 (frame table entry $1E0CD4); never displayed in the corpus
	incrom	$185AF4,$186
Tbl_185C7A:	; read by Spr_SwapFrameArt (pc $007F70), Spr_QueueArtDMA (pc $0080BE)
	incrom	$185C7A,$6
Art_185C80:	; DMA source to VRAM
	incrom	$185C80,$1A6
Art_185E26:	; DMA source to VRAM
	incrom	$185E26,$186
Art_185FAC:	; DMA source to VRAM
	incrom	$185FAC,$186
Art_186132:	; DMA source to VRAM
	incrom	$186132,$186
Art_1862B8:	; DMA source to VRAM
	incrom	$1862B8,$186
Art_18643E:	; DMA source to VRAM
	incrom	$18643E,$186
Art_1865C4:	; DMA source to VRAM
	incrom	$1865C4,$186
Art_18674A:	; DMA source to VRAM
	incrom	$18674A,$120
Art_18686A:	; sprite art block: 12 tiles in 1 chunk(s), DMA cost 12 (frame table entry $1E0D2E); never displayed in the corpus
	incrom	$18686A,$186
Tbl_1869F0:	; read by Spr_QueueArtDMA (pc $008098), Spr_SwapFrameArt (pc $007F70), Op4_DrawFrame (pc $0068AC)
	incrom	$1869F0,$8
Art_1869F8:	; DMA source to VRAM
	incrom	$1869F8,$1C0
Art_186BB8:	; DMA source to VRAM
	incrom	$186BB8,$C8
Art_186C80:	; DMA source to VRAM
	incrom	$186C80,$120
Art_186DA0:	; DMA source to VRAM
	incrom	$186DA0,$128
Art_186EC8:	; DMA source to VRAM
	incrom	$186EC8,$120
Art_186FE8:	; DMA source to VRAM
	incrom	$186FE8,$128
Art_187110:	; DMA source to VRAM
	incrom	$187110,$180
Art_187290:	; DMA source to VRAM
	incrom	$187290,$128
Art_1873B8:	; DMA source to VRAM
	incrom	$1873B8,$1E0
Art_187598:	; DMA source to VRAM
	incrom	$187598,$C8
Art_187660:	; DMA source to VRAM
	incrom	$187660,$1E0
Art_187840:	; DMA source to VRAM
	incrom	$187840,$108
Art_187948:	; DMA source to VRAM
	incrom	$187948,$200
Art_187B48:	; DMA source to VRAM
	incrom	$187B48,$1C8
Art_187D10:	; DMA source to VRAM
	incrom	$187D10,$180
Art_187E90:	; DMA source to VRAM
	incrom	$187E90,$128
Art_187FB8:	; DMA source to VRAM
	incrom	$187FB8,$200
Art_1881B8:	; DMA source to VRAM
	incrom	$1881B8,$108
Art_1882C0:	; DMA source to VRAM
	incrom	$1882C0,$200
Art_1884C0:	; DMA source to VRAM
	incrom	$1884C0,$C8
Art_188588:	; DMA source to VRAM
	incrom	$188588,$200
Art_188788:	; DMA source to VRAM
	incrom	$188788,$C6
Art_18884E:	; DMA source to VRAM
	incrom	$18884E,$1E8
Art_188A36:	; DMA source to VRAM
	incrom	$188A36,$1C0
Art_188BF6:	; DMA source to VRAM
	incrom	$188BF6,$66
Art_188C5C:	; DMA source to VRAM
	incrom	$188C5C,$208
Art_188E64:	; DMA source to VRAM
	incrom	$188E64,$180
Art_188FE4:	; DMA source to VRAM
	incrom	$188FE4,$C6
Art_1890AA:	; DMA source to VRAM
	incrom	$1890AA,$1E8
Art_189292:	; DMA source to VRAM
	incrom	$189292,$200
Art_189492:	; DMA source to VRAM
	incrom	$189492,$68
Art_1894FA:	; DMA source to VRAM
	incrom	$1894FA,$1C0
Art_1896BA:	; DMA source to VRAM
	incrom	$1896BA,$128
Art_1897E2:	; DMA source to VRAM
	incrom	$1897E2,$200
Art_1899E2:	; DMA source to VRAM
	incrom	$1899E2,$46
Art_189A28:	; DMA source to VRAM
	incrom	$189A28,$188
Art_189BB0:	; DMA source to VRAM
	incrom	$189BB0,$200
Art_189DB0:	; DMA source to VRAM
	incrom	$189DB0,$14A
Art_189EFA:	; DMA source to VRAM
	incrom	$189EFA,$1C0
Art_18A0BA:	; DMA source to VRAM
	incrom	$18A0BA,$180
Art_18A23A:	; DMA source to VRAM
	incrom	$18A23A,$108
Art_18A342:	; DMA source to VRAM
	incrom	$18A342,$1E0
Art_18A522:	; DMA source to VRAM
	incrom	$18A522,$208
Art_18A72A:	; DMA source to VRAM
	incrom	$18A72A,$1E0
Art_18A90A:	; DMA source to VRAM
	incrom	$18A90A,$200
Art_18AB0A:	; sprite art block: 50 tiles in 4 chunk(s), DMA cost 50 (frame table entry $1E0E46); never displayed in the corpus
	incrom	$18AB0A,$64C
Tbl_18B156:	; read by Spr_QueueArtDMA (pc $008098), Spr_SwapFrameArt (pc $007F70)
	incrom	$18B156,$A
Art_18B160:	; DMA source to VRAM
	incrom	$18B160,$200
Art_18B360:	; DMA source to VRAM
	incrom	$18B360,$200
Art_18B560:	; DMA source to VRAM
	incrom	$18B560,$8A
Art_18B5EA:	; DMA source to VRAM
	incrom	$18B5EA,$200
Art_18B7EA:	; DMA source to VRAM
	incrom	$18B7EA,$200
Art_18B9EA:	; DMA source to VRAM
	incrom	$18B9EA,$180
Art_18BB6A:	; sprite art block: 24 tiles in 2 chunk(s), DMA cost 24 (frame table entry $1E0F5E); never displayed in the corpus
	incrom	$18BB6A,$308
Art_18BE72:	; sprite art block: 11 tiles in 1 chunk(s), DMA cost 11 (frame table entry $1E0FAE); never displayed in the corpus
	incrom	$18BE72,$166
Art_18BFD8:	; sprite art block: 32 tiles in 2 chunk(s), DMA cost 32 (frame table entry $1E0FFE); never displayed in the corpus
	incrom	$18BFD8,$408
Art_18C3E0:	; sprite art block: 8 tiles in 1 chunk(s), DMA cost 8 (frame table entry $1E104E); never displayed in the corpus
	incrom	$18C3E0,$106
Art_18C4E6:	; sprite art block: 127 tiles in 8 chunk(s), DMA cost 127 (frame table entry $1E109E); never displayed in the corpus
	incrom	$18C4E6,$FF4
Tbl_18D4DA:	; read by Spr_QueueArtDMA (pc $008098), Spr_SwapFrameArt (pc $007F70), Op4_DrawFrame (pc $0068AC)
	incrom	$18D4DA,$C
Art_18D4E6:	; DMA source to VRAM
	incrom	$18D4E6,$200
Art_18D6E6:	; DMA source to VRAM
	incrom	$18D6E6,$200
Art_18D8E6:	; DMA source to VRAM
	incrom	$18D8E6,$1C0
Art_18DAA6:	; DMA source to VRAM
	incrom	$18DAA6,$108
Art_18DBAE:	; DMA source to VRAM
	incrom	$18DBAE,$200
Art_18DDAE:	; DMA source to VRAM
	incrom	$18DDAE,$206
Art_18DFB4:	; DMA source to VRAM
	incrom	$18DFB4,$86
Art_18E03A:	; DMA source to VRAM
	incrom	$18E03A,$28
Art_18E062:	; DMA source to VRAM
	incrom	$18E062,$200
Art_18E262:	; DMA source to VRAM
	incrom	$18E262,$1A8
Art_18E40A:	; DMA source to VRAM
	incrom	$18E40A,$1E0
Art_18E5EA:	; DMA source to VRAM
	incrom	$18E5EA,$1CA
Art_18E7B4:	; DMA source to VRAM
	incrom	$18E7B4,$200
Art_18E9B4:	; DMA source to VRAM
	incrom	$18E9B4,$1C0
Art_18EB74:	; DMA source to VRAM
	incrom	$18EB74,$68
Art_18EBDC:	; DMA source to VRAM
	incrom	$18EBDC,$1E0
Art_18EDBC:	; DMA source to VRAM
	incrom	$18EDBC,$1C8
Art_18EF84:	; DMA source to VRAM
	incrom	$18EF84,$1E0
Art_18F164:	; DMA source to VRAM
	incrom	$18F164,$128
Art_18F28C:	; DMA source to VRAM
	incrom	$18F28C,$1E0
Art_18F46C:	; DMA source to VRAM
	incrom	$18F46C,$168
Art_18F5D4:	; DMA source to VRAM
	incrom	$18F5D4,$200
Art_18F7D4:	; DMA source to VRAM
	incrom	$18F7D4,$1C8
Art_18F99C:	; DMA source to VRAM
	incrom	$18F99C,$200
Art_18FB9C:	; DMA source to VRAM
	incrom	$18FB9C,$1C8
Art_18FD64:	; DMA source to VRAM
	incrom	$18FD64,$200
Art_18FF64:	; DMA source to VRAM
	incrom	$18FF64,$1C8
Art_19012C:	; DMA source to VRAM
	incrom	$19012C,$200
Art_19032C:	; DMA source to VRAM
	incrom	$19032C,$18A
Art_1904B6:	; DMA source to VRAM
	incrom	$1904B6,$1E0
Art_190696:	; DMA source to VRAM
	incrom	$190696,$1C0
Art_190856:	; DMA source to VRAM
	incrom	$190856,$68
Art_1908BE:	; DMA source to VRAM
	incrom	$1908BE,$180
Art_190A3E:	; DMA source to VRAM
	incrom	$190A3E,$128
Art_190B66:	; DMA source to VRAM
	incrom	$190B66,$180
Art_190CE6:	; DMA source to VRAM
	incrom	$190CE6,$188
Art_190E6E:	; DMA source to VRAM
	incrom	$190E6E,$200
Art_19106E:	; DMA source to VRAM
	incrom	$19106E,$188
Art_1911F6:	; DMA source to VRAM
	incrom	$1911F6,$200
Art_1913F6:	; DMA source to VRAM
	incrom	$1913F6,$168
Art_19155E:	; DMA source to VRAM
	incrom	$19155E,$200
Art_19175E:	; DMA source to VRAM
	incrom	$19175E,$188
Art_1918E6:	; DMA source to VRAM
	incrom	$1918E6,$200
Art_191AE6:	; DMA source to VRAM
	incrom	$191AE6,$188
Art_191C6E:	; DMA source to VRAM
	incrom	$191C6E,$200
Art_191E6E:	; DMA source to VRAM
	incrom	$191E6E,$108
Art_191F76:	; DMA source to VRAM
	incrom	$191F76,$200
Art_192176:	; DMA source to VRAM
	incrom	$192176,$E8
Art_19225E:	; DMA source to VRAM
	incrom	$19225E,$200
Art_19245E:	; DMA source to VRAM
	incrom	$19245E,$88
Art_1924E6:	; DMA source to VRAM
	incrom	$1924E6,$1C0
Art_1926A6:	; DMA source to VRAM
	incrom	$1926A6,$108
Art_1927AE:	; DMA source to VRAM
	incrom	$1927AE,$180
Art_19292E:	; DMA source to VRAM
	incrom	$19292E,$1C8
Art_192AF6:	; DMA source to VRAM
	incrom	$192AF6,$200
Art_192CF6:	; DMA source to VRAM
	incrom	$192CF6,$68
Art_192D5E:	; DMA source to VRAM
	incrom	$192D5E,$1E0
Art_192F3E:	; DMA source to VRAM
	incrom	$192F3E,$48
Art_192F86:	; DMA source to VRAM
	incrom	$192F86,$200
Art_193186:	; DMA source to VRAM
	incrom	$193186,$148
Art_1932CE:	; DMA source to VRAM
	incrom	$1932CE,$1E0
Art_1934AE:	; DMA source to VRAM
	incrom	$1934AE,$E8
Art_193596:	; DMA source to VRAM
	incrom	$193596,$200
Art_193796:	; DMA source to VRAM
	incrom	$193796,$146
Art_1938DC:	; DMA source to VRAM
	incrom	$1938DC,$46
Art_193922:	; DMA source to VRAM
	incrom	$193922,$86
Art_1939A8:	; DMA source to VRAM
	incrom	$1939A8,$48
Art_1939F0:	; DMA source to VRAM
	incrom	$1939F0,$200
Art_193BF0:	; DMA source to VRAM
	incrom	$193BF0,$108
Art_193CF8:	; DMA source to VRAM
	incrom	$193CF8,$200
Art_193EF8:	; DMA source to VRAM
	incrom	$193EF8,$A8
Art_193FA0:	; DMA source to VRAM
	incrom	$193FA0,$200
Art_1941A0:	; DMA source to VRAM
	incrom	$1941A0,$46
Art_1941E6:	; DMA source to VRAM
	incrom	$1941E6,$188
Art_19436E:	; DMA source to VRAM
	incrom	$19436E,$200
Art_19456E:	; DMA source to VRAM
	incrom	$19456E,$188
Art_1946F6:	; DMA source to VRAM
	incrom	$1946F6,$200
Art_1948F6:	; DMA source to VRAM
	incrom	$1948F6,$188
Art_194A7E:	; DMA source to VRAM
	incrom	$194A7E,$200
Art_194C7E:	; DMA source to VRAM
	incrom	$194C7E,$1C8
Art_194E46:	; DMA source to VRAM
	incrom	$194E46,$200
Art_195046:	; DMA source to VRAM
	incrom	$195046,$168
Art_1951AE:	; DMA source to VRAM
	incrom	$1951AE,$160
Art_19530E:	; DMA source to VRAM
	incrom	$19530E,$208
Art_195516:	; DMA source to VRAM
	incrom	$195516,$200
Art_195716:	; DMA source to VRAM
	incrom	$195716,$1A8
Art_1958BE:	; DMA source to VRAM
	incrom	$1958BE,$200
Art_195ABE:	; DMA source to VRAM
	incrom	$195ABE,$188
Art_195C46:	; DMA source to VRAM
	incrom	$195C46,$200
Art_195E46:	; DMA source to VRAM
	incrom	$195E46,$1AA
Art_195FF0:	; DMA source to VRAM
	incrom	$195FF0,$1C0
Art_1961B0:	; DMA source to VRAM
	incrom	$1961B0,$120
Art_1962D0:	; DMA source to VRAM
	incrom	$1962D0,$106
Art_1963D6:	; DMA source to VRAM
	incrom	$1963D6,$A6
Art_19647C:	; DMA source to VRAM
	incrom	$19647C,$CA
Art_196546:	; DMA source to VRAM
	incrom	$196546,$180
Art_1966C6:	; DMA source to VRAM
	incrom	$1966C6,$1E0
Art_1968A6:	; DMA source to VRAM
	incrom	$1968A6,$C8
Art_19696E:	; DMA source to VRAM
	incrom	$19696E,$200
Art_196B6E:	; DMA source to VRAM
	incrom	$196B6E,$1A6
Art_196D14:	; DMA source to VRAM
	incrom	$196D14,$46
Art_196D5A:	; DMA source to VRAM
	incrom	$196D5A,$46
Art_196DA0:	; DMA source to VRAM
	incrom	$196DA0,$46
Art_196DE6:	; DMA source to VRAM
	incrom	$196DE6,$46
Art_196E2C:	; DMA source to VRAM
	incrom	$196E2C,$C6
Art_196EF2:	; DMA source to VRAM
	incrom	$196EF2,$E6
Art_196FD8:	; DMA source to VRAM
	incrom	$196FD8,$106
Art_1970DE:	; DMA source to VRAM
	incrom	$1970DE,$E6
Art_1971C4:	; DMA source to VRAM
	incrom	$1971C4,$E6
Art_1972AA:	; DMA source to VRAM
	incrom	$1972AA,$126
Art_1973D0:	; DMA source to VRAM
	incrom	$1973D0,$A6
Art_197476:	; DMA source to VRAM
	incrom	$197476,$E6
Art_19755C:	; DMA source to VRAM
	incrom	$19755C,$126
Art_197682:	; DMA source to VRAM
	incrom	$197682,$106
Art_197788:	; DMA source to VRAM
	incrom	$197788,$C6
Art_19784E:	; DMA source to VRAM
	incrom	$19784E,$C6
Art_197914:	; DMA source to VRAM
	incrom	$197914,$106
Art_197A1A:	; DMA source to VRAM
	incrom	$197A1A,$1A6
Art_197BC0:	; DMA source to VRAM
	incrom	$197BC0,$1C6
Art_197D86:	; DMA source to VRAM
	incrom	$197D86,$186
Art_197F0C:	; DMA source to VRAM
	incrom	$197F0C,$166
Art_198072:	; DMA source to VRAM
	incrom	$198072,$186
Art_1981F8:	; DMA source to VRAM
	incrom	$1981F8,$186
Art_19837E:	; DMA source to VRAM
	incrom	$19837E,$146
Art_1984C4:	; DMA source to VRAM
	incrom	$1984C4,$106
Art_1985CA:	; DMA source to VRAM
	incrom	$1985CA,$166
Art_198730:	; DMA source to VRAM
	incrom	$198730,$146
Art_198876:	; DMA source to VRAM
	incrom	$198876,$106
Art_19897C:	; DMA source to VRAM
	incrom	$19897C,$86
Art_198A02:	; DMA source to VRAM
	incrom	$198A02,$126
Art_198B28:	; DMA source to VRAM
	incrom	$198B28,$126
Art_198C4E:	; DMA source to VRAM
	incrom	$198C4E,$126
Art_198D74:	; DMA source to VRAM
	incrom	$198D74,$126
Art_198E9A:	; DMA source to VRAM
	incrom	$198E9A,$126
Art_198FC0:	; DMA source to VRAM
	incrom	$198FC0,$126
Art_1990E6:	; DMA source to VRAM
	incrom	$1990E6,$120
Art_199206:	; sprite art block: 9 tiles in 1 chunk(s), DMA cost 9 (frame table entry $1E1418); never displayed in the corpus
	incrom	$199206,$126
Art_19932C:	; sprite art block: 9 tiles in 1 chunk(s), DMA cost 9 (frame table entry $1E1422); never displayed in the corpus
	incrom	$19932C,$126
Tbl_199452:	; read by Spr_SwapFrameArt (pc $007F70), Spr_QueueArtDMA (pc $0080BE)
	incrom	$199452,$6
Art_199458:	; DMA source to VRAM
	incrom	$199458,$126
Art_19957E:	; DMA source to VRAM
	incrom	$19957E,$126
Art_1996A4:	; DMA source to VRAM
	incrom	$1996A4,$126
Art_1997CA:	; DMA source to VRAM
	incrom	$1997CA,$126
Art_1998F0:	; DMA source to VRAM
	incrom	$1998F0,$126
Art_199A16:	; DMA source to VRAM
	incrom	$199A16,$126
Art_199B3C:	; DMA source to VRAM
	incrom	$199B3C,$126
Art_199C62:	; DMA source to VRAM
	incrom	$199C62,$C6
Art_199D28:	; DMA source to VRAM
	incrom	$199D28,$C6
Art_199DEE:	; DMA source to VRAM
	incrom	$199DEE,$186
Art_199F74:	; DMA source to VRAM
	incrom	$199F74,$146
Art_19A0BA:	; DMA source to VRAM
	incrom	$19A0BA,$146
Art_19A200:	; DMA source to VRAM
	incrom	$19A200,$186
Art_19A386:	; DMA source to VRAM
	incrom	$19A386,$126
Art_19A4AC:	; DMA source to VRAM
	incrom	$19A4AC,$C6
Art_19A572:	; DMA source to VRAM
	incrom	$19A572,$C6
Art_19A638:	; DMA source to VRAM
	incrom	$19A638,$D4
Art_19A70C:	; DMA source to VRAM
	incrom	$19A70C,$200
Art_19A90C:	; DMA source to VRAM
	incrom	$19A90C,$200
Art_19AB0C:	; DMA source to VRAM
	incrom	$19AB0C,$200
Art_19AD0C:	; DMA source to VRAM
	incrom	$19AD0C,$1E0
Art_19AEEC:	; DMA source to VRAM
	incrom	$19AEEC,$180
Art_19B06C:	; DMA source to VRAM
	incrom	$19B06C,$180
Art_19B1EC:	; DMA source to VRAM
	incrom	$19B1EC,$1E0
Art_19B3CC:	; DMA source to VRAM
	incrom	$19B3CC,$106
Art_19B4D2:	; DMA source to VRAM
	incrom	$19B4D2,$206
Art_19B6D8:	; DMA source to VRAM
	incrom	$19B6D8,$206
Art_19B8DE:	; DMA source to VRAM
	incrom	$19B8DE,$C6
Art_19B9A4:	; DMA source to VRAM
	incrom	$19B9A4,$106
Art_19BAAA:	; DMA source to VRAM
	incrom	$19BAAA,$186
Art_19BC30:	; DMA source to VRAM
	incrom	$19BC30,$146
Art_19BD76:	; DMA source to VRAM
	incrom	$19BD76,$1E8
Art_19BF5E:	; DMA source to VRAM
	incrom	$19BF5E,$1E0
Art_19C13E:	; DMA source to VRAM
	incrom	$19C13E,$C8
Art_19C206:	; DMA source to VRAM
	incrom	$19C206,$1E0
Art_19C3E6:	; DMA source to VRAM
	incrom	$19C3E6,$C8
Art_19C4AE:	; DMA source to VRAM
	incrom	$19C4AE,$1E0
Art_19C68E:	; DMA source to VRAM
	incrom	$19C68E,$48
Art_19C6D6:	; DMA source to VRAM
	incrom	$19C6D6,$200
Art_19C8D6:	; DMA source to VRAM
	incrom	$19C8D6,$108
Art_19C9DE:	; DMA source to VRAM
	incrom	$19C9DE,$1E0
Art_19CBBE:	; DMA source to VRAM
	incrom	$19CBBE,$48
Art_19CC06:	; DMA source to VRAM
	incrom	$19CC06,$200
Art_19CE06:	; DMA source to VRAM
	incrom	$19CE06,$E8
Art_19CEEE:	; DMA source to VRAM
	incrom	$19CEEE,$1E0
Art_19D0CE:	; DMA source to VRAM
	incrom	$19D0CE,$68
Art_19D136:	; DMA source to VRAM
	incrom	$19D136,$180
Art_19D2B6:	; DMA source to VRAM
	incrom	$19D2B6,$1C8
Art_19D47E:	; DMA source to VRAM
	incrom	$19D47E,$200
Art_19D67E:	; DMA source to VRAM
	incrom	$19D67E,$146
Art_19D7C4:	; DMA source to VRAM
	incrom	$19D7C4,$86
Art_19D84A:	; DMA source to VRAM
	incrom	$19D84A,$86
Art_19D8D0:	; DMA source to VRAM
	incrom	$19D8D0,$E6
Art_19D9B6:	; DMA source to VRAM
	incrom	$19D9B6,$E6
Art_19DA9C:	; DMA source to VRAM
	incrom	$19DA9C,$86
Art_19DB22:	; DMA source to VRAM
	incrom	$19DB22,$146
Art_19DC68:	; DMA source to VRAM
	incrom	$19DC68,$126
Art_19DD8E:	; DMA source to VRAM
	incrom	$19DD8E,$86
Art_19DE14:	; DMA source to VRAM
	incrom	$19DE14,$146
Art_19DF5A:	; DMA source to VRAM
	incrom	$19DF5A,$126
Art_19E080:	; DMA source to VRAM
	incrom	$19E080,$E6
Art_19E166:	; DMA source to VRAM
	incrom	$19E166,$E6
Art_19E24C:	; DMA source to VRAM
	incrom	$19E24C,$E6
Art_19E332:	; DMA source to VRAM
	incrom	$19E332,$106
Art_19E438:	; DMA source to VRAM
	incrom	$19E438,$106
Art_19E53E:	; DMA source to VRAM
	incrom	$19E53E,$106
Art_19E644:	; DMA source to VRAM
	incrom	$19E644,$E6
Art_19E72A:	; DMA source to VRAM
	incrom	$19E72A,$E6
Art_19E810:	; DMA source to VRAM
	incrom	$19E810,$E6
Art_19E8F6:	; DMA source to VRAM
	incrom	$19E8F6,$86
Art_19E97C:	; DMA source to VRAM
	incrom	$19E97C,$86
Art_19EA02:	; DMA source to VRAM
	incrom	$19EA02,$86
Art_19EA88:	; DMA source to VRAM
	incrom	$19EA88,$86
Art_19EB0E:	; DMA source to VRAM
	incrom	$19EB0E,$A6
Art_19EBB4:	; DMA source to VRAM
	incrom	$19EBB4,$86
Art_19EC3A:	; DMA source to VRAM
	incrom	$19EC3A,$86
Art_19ECC0:	; DMA source to VRAM
	incrom	$19ECC0,$86
Art_19ED46:	; DMA source to VRAM
	incrom	$19ED46,$86
Art_19EDCC:	; DMA source to VRAM
	incrom	$19EDCC,$208
Art_19EFD4:	; DMA source to VRAM
	incrom	$19EFD4,$200
Art_19F1D4:	; DMA source to VRAM
	incrom	$19F1D4,$46
Art_19F21A:	; DMA source to VRAM
	incrom	$19F21A,$188
Art_19F3A2:	; DMA source to VRAM
	incrom	$19F3A2,$200
Art_19F5A2:	; DMA source to VRAM
	incrom	$19F5A2,$86
Art_19F628:	; DMA source to VRAM
	incrom	$19F628,$106
Art_19F72E:	; DMA source to VRAM
	incrom	$19F72E,$C6
Art_19F7F4:	; DMA source to VRAM
	incrom	$19F7F4,$86
Art_19F87A:	; DMA source to VRAM
	incrom	$19F87A,$26
Art_19F8A0:	; DMA source to VRAM
	incrom	$19F8A0,$26
Art_19F8C6:	; DMA source to VRAM
	incrom	$19F8C6,$188
Art_19FA4E:	; DMA source to VRAM
	incrom	$19FA4E,$200
Art_19FC4E:	; DMA source to VRAM
	incrom	$19FC4E,$180
	incrom	$19FDCE,$232
Tbl_1A0000:	; read by Spr_QueueArtDMA (pc $008098), Spr_SwapFrameArt (pc $007F70)
	incrom	$1A0000,$8
Art_1A0008:	; DMA source to VRAM
	incrom	$1A0008,$200
Art_1A0208:	; DMA source to VRAM
	incrom	$1A0208,$208
Art_1A0410:	; DMA source to VRAM
	incrom	$1A0410,$200
Art_1A0610:	; DMA source to VRAM
	incrom	$1A0610,$188
Art_1A0798:	; DMA source to VRAM
	incrom	$1A0798,$200
Art_1A0998:	; DMA source to VRAM
	incrom	$1A0998,$1A8
Art_1A0B40:	; DMA source to VRAM
	incrom	$1A0B40,$200
Art_1A0D40:	; DMA source to VRAM
	incrom	$1A0D40,$18A
Art_1A0ECA:	; DMA source to VRAM
	incrom	$1A0ECA,$180
Art_1A104A:	; DMA source to VRAM
	incrom	$1A104A,$180
Art_1A11CA:	; DMA source to VRAM
	incrom	$1A11CA,$188
Art_1A1352:	; DMA source to VRAM
	incrom	$1A1352,$200
Art_1A1552:	; DMA source to VRAM
	incrom	$1A1552,$14A
Art_1A169C:	; DMA source to VRAM
	incrom	$1A169C,$200
Art_1A189C:	; DMA source to VRAM
	incrom	$1A189C,$1C0
Art_1A1A5C:	; DMA source to VRAM
	incrom	$1A1A5C,$8A
Art_1A1AE6:	; DMA source to VRAM
	incrom	$1A1AE6,$180
Art_1A1C66:	; DMA source to VRAM
	incrom	$1A1C66,$1C0
Art_1A1E26:	; DMA source to VRAM
	incrom	$1A1E26,$C8
Art_1A1EEE:	; DMA source to VRAM
	incrom	$1A1EEE,$200
Art_1A20EE:	; DMA source to VRAM
	incrom	$1A20EE,$1CA
Art_1A22B8:	; DMA source to VRAM
	incrom	$1A22B8,$200
Art_1A24B8:	; DMA source to VRAM
	incrom	$1A24B8,$1C0
Art_1A2678:	; DMA source to VRAM
	incrom	$1A2678,$6A
Art_1A26E2:	; DMA source to VRAM
	incrom	$1A26E2,$200
Art_1A28E2:	; DMA source to VRAM
	incrom	$1A28E2,$1E0
Art_1A2AC2:	; DMA source to VRAM
	incrom	$1A2AC2,$6A
Art_1A2B2C:	; DMA source to VRAM
	incrom	$1A2B2C,$200
Art_1A2D2C:	; DMA source to VRAM
	incrom	$1A2D2C,$200
Art_1A2F2C:	; DMA source to VRAM
	incrom	$1A2F2C,$6A
Art_1A2F96:	; DMA source to VRAM
	incrom	$1A2F96,$200
Art_1A3196:	; DMA source to VRAM
	incrom	$1A3196,$1C0
Art_1A3356:	; DMA source to VRAM
	incrom	$1A3356,$88
Art_1A33DE:	; DMA source to VRAM
	incrom	$1A33DE,$200
Art_1A35DE:	; DMA source to VRAM
	incrom	$1A35DE,$1CA
Art_1A37A8:	; DMA source to VRAM
	incrom	$1A37A8,$200
Art_1A39A8:	; DMA source to VRAM
	incrom	$1A39A8,$1C0
Art_1A3B68:	; DMA source to VRAM
	incrom	$1A3B68,$6A
Art_1A3BD2:	; DMA source to VRAM
	incrom	$1A3BD2,$200
Art_1A3DD2:	; DMA source to VRAM
	incrom	$1A3DD2,$1E0
Art_1A3FB2:	; DMA source to VRAM
	incrom	$1A3FB2,$6A
Art_1A401C:	; DMA source to VRAM
	incrom	$1A401C,$200
Art_1A421C:	; DMA source to VRAM
	incrom	$1A421C,$200
Art_1A441C:	; DMA source to VRAM
	incrom	$1A441C,$6A
Art_1A4486:	; DMA source to VRAM
	incrom	$1A4486,$200
Art_1A4686:	; DMA source to VRAM
	incrom	$1A4686,$1C0
Art_1A4846:	; DMA source to VRAM
	incrom	$1A4846,$88
Art_1A48CE:	; DMA source to VRAM
	incrom	$1A48CE,$200
Art_1A4ACE:	; DMA source to VRAM
	incrom	$1A4ACE,$1CE
Art_1A4C9C:	; DMA source to VRAM
	incrom	$1A4C9C,$200
Art_1A4E9C:	; DMA source to VRAM
	incrom	$1A4E9C,$200
Art_1A509C:	; DMA source to VRAM
	incrom	$1A509C,$200
Art_1A529C:	; DMA source to VRAM
	incrom	$1A529C,$200
Art_1A549C:	; DMA source to VRAM
	incrom	$1A549C,$188
Art_1A5624:	; DMA source to VRAM
	incrom	$1A5624,$200
Art_1A5824:	; DMA source to VRAM
	incrom	$1A5824,$168
Art_1A598C:	; DMA source to VRAM
	incrom	$1A598C,$200
Art_1A5B8C:	; DMA source to VRAM
	incrom	$1A5B8C,$206
Art_1A5D92:	; DMA source to VRAM
	incrom	$1A5D92,$46
Art_1A5DD8:	; DMA source to VRAM
	incrom	$1A5DD8,$106
Art_1A5EDE:	; DMA source to VRAM
	incrom	$1A5EDE,$86
Art_1A5F64:	; DMA source to VRAM
	incrom	$1A5F64,$106
Art_1A606A:	; DMA source to VRAM
	incrom	$1A606A,$86
Art_1A60F0:	; DMA source to VRAM
	incrom	$1A60F0,$46
Art_1A6136:	; DMA source to VRAM
	incrom	$1A6136,$46
Art_1A617C:	; DMA source to VRAM
	incrom	$1A617C,$46
Art_1A61C2:	; DMA source to VRAM
	incrom	$1A61C2,$46
Art_1A6208:	; DMA source to VRAM
	incrom	$1A6208,$46
Art_1A624E:	; DMA source to VRAM
	incrom	$1A624E,$46
Art_1A6294:	; DMA source to VRAM
	incrom	$1A6294,$126
Art_1A63BA:	; DMA source to VRAM
	incrom	$1A63BA,$46
Art_1A6400:	; DMA source to VRAM
	incrom	$1A6400,$46
Art_1A6446:	; DMA source to VRAM
	incrom	$1A6446,$46
Art_1A648C:	; DMA source to VRAM
	incrom	$1A648C,$86
Art_1A6512:	; DMA source to VRAM
	incrom	$1A6512,$86
Art_1A6598:	; DMA source to VRAM
	incrom	$1A6598,$C6
Art_1A665E:	; DMA source to VRAM
	incrom	$1A665E,$C6
Art_1A6724:	; DMA source to VRAM
	incrom	$1A6724,$86
Art_1A67AA:	; DMA source to VRAM
	incrom	$1A67AA,$86
Tbl_1A6830:	; read by Spr_SwapFrameArt (pc $007F70), Op4_DrawFrame (pc $0068AC), Spr_QueueArtDMA (pc $008098); 50% read in the corpus
	incrom	$1A6830,$10C
Art_1A693C:	; DMA source to VRAM
	incrom	$1A693C,$46
Art_1A6982:	; DMA source to VRAM
	incrom	$1A6982,$86
Art_1A6A08:	; DMA source to VRAM
	incrom	$1A6A08,$66
Art_1A6A6E:	; DMA source to VRAM
	incrom	$1A6A6E,$26
Art_1A6A94:	; DMA source to VRAM
	incrom	$1A6A94,$26
Art_1A6ABA:	; DMA source to VRAM
	incrom	$1A6ABA,$26
Art_1A6AE0:	; DMA source to VRAM
	incrom	$1A6AE0,$26
Art_1A6B06:	; DMA source to VRAM
	incrom	$1A6B06,$26
Art_1A6B2C:	; DMA source to VRAM
	incrom	$1A6B2C,$26
Art_1A6B52:	; DMA source to VRAM
	incrom	$1A6B52,$126
Art_1A6C78:	; DMA source to VRAM
	incrom	$1A6C78,$126
Art_1A6D9E:	; DMA source to VRAM
	incrom	$1A6D9E,$126
Art_1A6EC4:	; DMA source to VRAM
	incrom	$1A6EC4,$126
Art_1A6FEA:	; DMA source to VRAM
	incrom	$1A6FEA,$126
Art_1A7110:	; DMA source to VRAM
	incrom	$1A7110,$126
Art_1A7236:	; DMA source to VRAM
	incrom	$1A7236,$126
Art_1A735C:	; DMA source to VRAM
	incrom	$1A735C,$126
Art_1A7482:	; DMA source to VRAM
	incrom	$1A7482,$126
Art_1A75A8:	; DMA source to VRAM
	incrom	$1A75A8,$126
Art_1A76CE:	; DMA source to VRAM
	incrom	$1A76CE,$126
Art_1A77F4:	; DMA source to VRAM
	incrom	$1A77F4,$126
Art_1A791A:	; DMA source to VRAM
	incrom	$1A791A,$126
Art_1A7A40:	; DMA source to VRAM
	incrom	$1A7A40,$126
Art_1A7B66:	; DMA source to VRAM
	incrom	$1A7B66,$126
Art_1A7C8C:	; DMA source to VRAM
	incrom	$1A7C8C,$126
Art_1A7DB2:	; DMA source to VRAM
	incrom	$1A7DB2,$126
Art_1A7ED8:	; DMA source to VRAM
	incrom	$1A7ED8,$126
Art_1A7FFE:	; DMA source to VRAM
	incrom	$1A7FFE,$126
Art_1A8124:	; DMA source to VRAM
	incrom	$1A8124,$126
Art_1A824A:	; DMA source to VRAM
	incrom	$1A824A,$126
Art_1A8370:	; DMA source to VRAM
	incrom	$1A8370,$126
Art_1A8496:	; DMA source to VRAM
	incrom	$1A8496,$126
Art_1A85BC:	; DMA source to VRAM
	incrom	$1A85BC,$126
Art_1A86E2:	; DMA source to VRAM
	incrom	$1A86E2,$126
Art_1A8808:	; DMA source to VRAM
	incrom	$1A8808,$126
Art_1A892E:	; DMA source to VRAM
	incrom	$1A892E,$126
Art_1A8A54:	; DMA source to VRAM
	incrom	$1A8A54,$126
Art_1A8B7A:	; DMA source to VRAM
	incrom	$1A8B7A,$126
Art_1A8CA0:	; DMA source to VRAM
	incrom	$1A8CA0,$126
Art_1A8DC6:	; DMA source to VRAM
	incrom	$1A8DC6,$126
Art_1A8EEC:	; DMA source to VRAM
	incrom	$1A8EEC,$126
Art_1A9012:	; DMA source to VRAM
	incrom	$1A9012,$206
Art_1A9218:	; DMA source to VRAM
	incrom	$1A9218,$206
Art_1A941E:	; DMA source to VRAM
	incrom	$1A941E,$206
Art_1A9624:	; DMA source to VRAM
	incrom	$1A9624,$206
Art_1A982A:	; DMA source to VRAM
	incrom	$1A982A,$206
Art_1A9A30:	; DMA source to VRAM
	incrom	$1A9A30,$206
Art_1A9C36:	; DMA source to VRAM
	incrom	$1A9C36,$206
Art_1A9E3C:	; DMA source to VRAM
	incrom	$1A9E3C,$206
Art_1AA042:	; DMA source to VRAM
	incrom	$1AA042,$206
Art_1AA248:	; DMA source to VRAM
	incrom	$1AA248,$206
Art_1AA44E:	; DMA source to VRAM
	incrom	$1AA44E,$206
Art_1AA654:	; DMA source to VRAM
	incrom	$1AA654,$206
Art_1AA85A:	; DMA source to VRAM
	incrom	$1AA85A,$206
Art_1AAA60:	; DMA source to VRAM
	incrom	$1AAA60,$86
Art_1AAAE6:	; DMA source to VRAM
	incrom	$1AAAE6,$206
Art_1AACEC:	; DMA source to VRAM
	incrom	$1AACEC,$206
Art_1AAEF2:	; DMA source to VRAM
	incrom	$1AAEF2,$206
Art_1AB0F8:	; DMA source to VRAM
	incrom	$1AB0F8,$206
Art_1AB2FE:	; DMA source to VRAM
	incrom	$1AB2FE,$206
Art_1AB504:	; DMA source to VRAM
	incrom	$1AB504,$206
Art_1AB70A:	; DMA source to VRAM
	incrom	$1AB70A,$206
Art_1AB910:	; DMA source to VRAM
	incrom	$1AB910,$206
Art_1ABB16:	; DMA source to VRAM
	incrom	$1ABB16,$206
Art_1ABD1C:	; DMA source to VRAM
	incrom	$1ABD1C,$206
Art_1ABF22:	; DMA source to VRAM
	incrom	$1ABF22,$206
Art_1AC128:	; DMA source to VRAM
	incrom	$1AC128,$206
Art_1AC32E:	; DMA source to VRAM
	incrom	$1AC32E,$206
Art_1AC534:	; DMA source to VRAM
	incrom	$1AC534,$206
Art_1AC73A:	; DMA source to VRAM
	incrom	$1AC73A,$206
Art_1AC940:	; DMA source to VRAM
	incrom	$1AC940,$206
Art_1ACB46:	; DMA source to VRAM
	incrom	$1ACB46,$C6
Art_1ACC0C:	; DMA source to VRAM
	incrom	$1ACC0C,$C6
Art_1ACCD2:	; DMA source to VRAM
	incrom	$1ACCD2,$126
Art_1ACDF8:	; DMA source to VRAM
	incrom	$1ACDF8,$C6
Art_1ACEBE:	; DMA source to VRAM
	incrom	$1ACEBE,$C6
Art_1ACF84:	; DMA source to VRAM
	incrom	$1ACF84,$206
Art_1AD18A:	; DMA source to VRAM
	incrom	$1AD18A,$206
Art_1AD390:	; DMA source to VRAM
	incrom	$1AD390,$206
Art_1AD596:	; DMA source to VRAM
	incrom	$1AD596,$206
Art_1AD79C:	; DMA source to VRAM
	incrom	$1AD79C,$206
Art_1AD9A2:	; DMA source to VRAM
	incrom	$1AD9A2,$206
Art_1ADBA8:	; DMA source to VRAM
	incrom	$1ADBA8,$206
Art_1ADDAE:	; DMA source to VRAM
	incrom	$1ADDAE,$206
Art_1ADFB4:	; DMA source to VRAM
	incrom	$1ADFB4,$206
Art_1AE1BA:	; DMA source to VRAM
	incrom	$1AE1BA,$206
Art_1AE3C0:	; DMA source to VRAM
	incrom	$1AE3C0,$206
Art_1AE5C6:	; DMA source to VRAM
	incrom	$1AE5C6,$206
Art_1AE7CC:	; DMA source to VRAM
	incrom	$1AE7CC,$206
Art_1AE9D2:	; DMA source to VRAM
	incrom	$1AE9D2,$206
Art_1AEBD8:	; DMA source to VRAM
	incrom	$1AEBD8,$206
Art_1AEDDE:	; DMA source to VRAM
	incrom	$1AEDDE,$206
Art_1AEFE4:	; DMA source to VRAM
	incrom	$1AEFE4,$186
Art_1AF16A:	; DMA source to VRAM
	incrom	$1AF16A,$186
Art_1AF2F0:	; DMA source to VRAM
	incrom	$1AF2F0,$186
Art_1AF476:	; DMA source to VRAM
	incrom	$1AF476,$186
Art_1AF5FC:	; DMA source to VRAM
	incrom	$1AF5FC,$1A6
Art_1AF7A2:	; DMA source to VRAM; 72% read in the corpus
	incrom	$1AF7A2,$2D2
Art_1AFA74:	; DMA source to VRAM
	incrom	$1AFA74,$106
Art_1AFB7A:	; DMA source to VRAM
	incrom	$1AFB7A,$106
Art_1AFC80:	; DMA source to VRAM
	incrom	$1AFC80,$146
Art_1AFDC6:	; DMA source to VRAM
	incrom	$1AFDC6,$146
Art_1AFF0C:	; DMA source to VRAM
	incrom	$1AFF0C,$188
Art_1B0094:	; DMA source to VRAM
	incrom	$1B0094,$180
Art_1B0214:	; DMA source to VRAM
	incrom	$1B0214,$126
Art_1B033A:	; DMA source to VRAM
	incrom	$1B033A,$106
Art_1B0440:	; DMA source to VRAM
	incrom	$1B0440,$1A6
Art_1B05E6:	; DMA source to VRAM
	incrom	$1B05E6,$1A6
Art_1B078C:	; DMA source to VRAM
	incrom	$1B078C,$1C6
Art_1B0952:	; DMA source to VRAM
	incrom	$1B0952,$1C6
Art_1B0B18:	; DMA source to VRAM
	incrom	$1B0B18,$206
Art_1B0D1E:	; DMA source to VRAM
	incrom	$1B0D1E,$126
Art_1B0E44:	; DMA source to VRAM
	incrom	$1B0E44,$1A6
Art_1B0FEA:	; DMA source to VRAM
	incrom	$1B0FEA,$1C6
Art_1B11B0:	; DMA source to VRAM
	incrom	$1B11B0,$1C6
Art_1B1376:	; DMA source to VRAM
	incrom	$1B1376,$1C8
Art_1B153E:	; DMA source to VRAM
	incrom	$1B153E,$1E0
Art_1B171E:	; DMA source to VRAM
	incrom	$1B171E,$46
Art_1B1764:	; DMA source to VRAM
	incrom	$1B1764,$106
Art_1B186A:	; DMA source to VRAM
	incrom	$1B186A,$1A6
Art_1B1A10:	; DMA source to VRAM
	incrom	$1B1A10,$1A6
Art_1B1BB6:	; DMA source to VRAM
	incrom	$1B1BB6,$1C6
Art_1B1D7C:	; DMA source to VRAM
	incrom	$1B1D7C,$1C6
Art_1B1F42:	; DMA source to VRAM
	incrom	$1B1F42,$206
Art_1B2148:	; DMA source to VRAM
	incrom	$1B2148,$106
Art_1B224E:	; DMA source to VRAM
	incrom	$1B224E,$106
Art_1B2354:	; DMA source to VRAM
	incrom	$1B2354,$126
Art_1B247A:	; DMA source to VRAM
	incrom	$1B247A,$146
Art_1B25C0:	; DMA source to VRAM
	incrom	$1B25C0,$188
Art_1B2748:	; DMA source to VRAM
	incrom	$1B2748,$180
Art_1B28C8:	; DMA source to VRAM
	incrom	$1B28C8,$126
Art_1B29EE:	; DMA source to VRAM
	incrom	$1B29EE,$186
Art_1B2B74:	; DMA source to VRAM
	incrom	$1B2B74,$206
Art_1B2D7A:	; DMA source to VRAM
	incrom	$1B2D7A,$186
Art_1B2F00:	; DMA source to VRAM
	incrom	$1B2F00,$206
Art_1B3106:	; DMA source to VRAM
	incrom	$1B3106,$186
Art_1B328C:	; DMA source to VRAM
	incrom	$1B328C,$208
Art_1B3494:	; DMA source to VRAM
	incrom	$1B3494,$200
Art_1B3694:	; DMA source to VRAM
	incrom	$1B3694,$C8
Art_1B375C:	; DMA source to VRAM
	incrom	$1B375C,$200
Art_1B395C:	; DMA source to VRAM
	incrom	$1B395C,$26
Art_1B3982:	; DMA source to VRAM
	incrom	$1B3982,$208
Art_1B3B8A:	; DMA source to VRAM
	incrom	$1B3B8A,$200
Art_1B3D8A:	; DMA source to VRAM
	incrom	$1B3D8A,$C8
Art_1B3E52:	; DMA source to VRAM
	incrom	$1B3E52,$200
Art_1B4052:	; DMA source to VRAM
	incrom	$1B4052,$C6
Art_1B4118:	; DMA source to VRAM
	incrom	$1B4118,$126
Art_1B423E:	; DMA source to VRAM
	incrom	$1B423E,$126
Art_1B4364:	; DMA source to VRAM
	incrom	$1B4364,$186
Art_1B44EA:	; DMA source to VRAM
	incrom	$1B44EA,$186
Art_1B4670:	; DMA source to VRAM
	incrom	$1B4670,$186
Art_1B47F6:	; DMA source to VRAM
	incrom	$1B47F6,$106
Art_1B48FC:	; DMA source to VRAM
	incrom	$1B48FC,$106
Art_1B4A02:	; DMA source to VRAM
	incrom	$1B4A02,$E6
Art_1B4AE8:	; DMA source to VRAM
	incrom	$1B4AE8,$106
Art_1B4BEE:	; DMA source to VRAM
	incrom	$1B4BEE,$106
Art_1B4CF4:	; DMA source to VRAM
	incrom	$1B4CF4,$106
Art_1B4DFA:	; DMA source to VRAM
	incrom	$1B4DFA,$146
Art_1B4F40:	; DMA source to VRAM
	incrom	$1B4F40,$186
Art_1B50C6:	; DMA source to VRAM
	incrom	$1B50C6,$186
Art_1B524C:	; DMA source to VRAM
	incrom	$1B524C,$186
Art_1B53D2:	; DMA source to VRAM
	incrom	$1B53D2,$126
Art_1B54F8:	; DMA source to VRAM
	incrom	$1B54F8,$206
Art_1B56FE:	; DMA source to VRAM
	incrom	$1B56FE,$206
Art_1B5904:	; DMA source to VRAM
	incrom	$1B5904,$206
Art_1B5B0A:	; DMA source to VRAM
	incrom	$1B5B0A,$206
Art_1B5D10:	; DMA source to VRAM
	incrom	$1B5D10,$206
Art_1B5F16:	; DMA source to VRAM
	incrom	$1B5F16,$1C6
Art_1B60DC:	; DMA source to VRAM
	incrom	$1B60DC,$1C6
Art_1B62A2:	; DMA source to VRAM
	incrom	$1B62A2,$1C6
Art_1B6468:	; DMA source to VRAM
	incrom	$1B6468,$1E6
Art_1B664E:	; DMA source to VRAM
	incrom	$1B664E,$1E6
Art_1B6834:	; DMA source to VRAM
	incrom	$1B6834,$1E6
Art_1B6A1A:	; DMA source to VRAM
	incrom	$1B6A1A,$206
Art_1B6C20:	; DMA source to VRAM
	incrom	$1B6C20,$206
Art_1B6E26:	; DMA source to VRAM
	incrom	$1B6E26,$206
Art_1B702C:	; DMA source to VRAM
	incrom	$1B702C,$206
Art_1B7232:	; DMA source to VRAM
	incrom	$1B7232,$206
Art_1B7438:	; DMA source to VRAM
	incrom	$1B7438,$206
Art_1B763E:	; DMA source to VRAM
	incrom	$1B763E,$66
Art_1B76A4:	; DMA source to VRAM
	incrom	$1B76A4,$66
Art_1B770A:	; DMA source to VRAM
	incrom	$1B770A,$C6
Art_1B77D0:	; DMA source to VRAM
	incrom	$1B77D0,$C6
Art_1B7896:	; DMA source to VRAM
	incrom	$1B7896,$C6
Art_1B795C:	; DMA source to VRAM
	incrom	$1B795C,$C6
Art_1B7A22:	; DMA source to VRAM
	incrom	$1B7A22,$C6
Art_1B7AE8:	; DMA source to VRAM
	incrom	$1B7AE8,$C6
Art_1B7BAE:	; DMA source to VRAM
	incrom	$1B7BAE,$C6
Art_1B7C74:	; DMA source to VRAM
	incrom	$1B7C74,$C6
Art_1B7D3A:	; DMA source to VRAM
	incrom	$1B7D3A,$C6
Art_1B7E00:	; DMA source to VRAM
	incrom	$1B7E00,$A6
Art_1B7EA6:	; DMA source to VRAM
	incrom	$1B7EA6,$D0
Art_1B7F76:	; DMA source to VRAM
	incrom	$1B7F76,$200
Art_1B8176:	; DMA source to VRAM
	incrom	$1B8176,$200
Art_1B8376:	; DMA source to VRAM
	incrom	$1B8376,$200
Art_1B8576:	; DMA source to VRAM
	incrom	$1B8576,$200
Art_1B8776:	; DMA source to VRAM
	incrom	$1B8776,$200
Art_1B8976:	; DMA source to VRAM
	incrom	$1B8976,$4A
Art_1B89C0:	; DMA source to VRAM
	incrom	$1B89C0,$200
Art_1B8BC0:	; DMA source to VRAM
	incrom	$1B8BC0,$200
Art_1B8DC0:	; DMA source to VRAM
	incrom	$1B8DC0,$A6
Art_1B8E66:	; DMA source to VRAM
	incrom	$1B8E66,$C6
Art_1B8F2C:	; DMA source to VRAM
	incrom	$1B8F2C,$126
Art_1B9052:	; DMA source to VRAM
	incrom	$1B9052,$126
Art_1B9178:	; DMA source to VRAM
	incrom	$1B9178,$186
Art_1B92FE:	; DMA source to VRAM
	incrom	$1B92FE,$1C6
Art_1B94C4:	; DMA source to VRAM
	incrom	$1B94C4,$1E8
Art_1B96AC:	; DMA source to VRAM
	incrom	$1B96AC,$180
Art_1B982C:	; DMA source to VRAM
	incrom	$1B982C,$188
Art_1B99B4:	; DMA source to VRAM
	incrom	$1B99B4,$200
Art_1B9BB4:	; DMA source to VRAM
	incrom	$1B9BB4,$188
Art_1B9D3C:	; DMA source to VRAM
	incrom	$1B9D3C,$200
Art_1B9F3C:	; DMA source to VRAM
	incrom	$1B9F3C,$188
Art_1BA0C4:	; DMA source to VRAM
	incrom	$1BA0C4,$200
Art_1BA2C4:	; DMA source to VRAM
	incrom	$1BA2C4,$180
Art_1BA444:	; sprite art block: 6 tiles in 1 chunk(s), DMA cost 6 (frame table entry $1E20E8); never displayed in the corpus
	incrom	$1BA444,$C6
Art_1BA50A:	; sprite art block: 9 tiles in 1 chunk(s), DMA cost 9 (frame table entry $1E20F2); never displayed in the corpus
	incrom	$1BA50A,$126
Art_1BA630:	; sprite art block: 9 tiles in 1 chunk(s), DMA cost 9 (frame table entry $1E20FC); never displayed in the corpus
	incrom	$1BA630,$126
Art_1BA756:	; sprite art block: 16 tiles in 1 chunk(s), DMA cost 16 (frame table entry $1E2106); never displayed in the corpus
	incrom	$1BA756,$206
Art_1BA95C:	; sprite art block: 24 tiles in 2 chunk(s), DMA cost 24 (frame table entry $1E2110); never displayed in the corpus
	incrom	$1BA95C,$308
Art_1BAC64:	; sprite art block: 16 tiles in 1 chunk(s), DMA cost 16 (frame table entry $1E211A); never displayed in the corpus
	incrom	$1BAC64,$206
Art_1BAE6A:	; sprite art block: 13 tiles in 1 chunk(s), DMA cost 13 (frame table entry $1E2124); never displayed in the corpus
	incrom	$1BAE6A,$1A6
Art_1BB010:	; sprite art block: 12 tiles in 1 chunk(s), DMA cost 12 (frame table entry $1E212E); never displayed in the corpus
	incrom	$1BB010,$186
Tbl_1BB196:	; read by Spr_SwapFrameArt (pc $007F70), Spr_QueueArtDMA (pc $008098), Op4_DrawFrame (pc $0068AC)
	incrom	$1BB196,$8
Art_1BB19E:	; DMA source to VRAM
	incrom	$1BB19E,$200
Art_1BB39E:	; DMA source to VRAM
	incrom	$1BB39E,$1C6
Art_1BB564:	; DMA source to VRAM
	incrom	$1BB564,$1E8
Art_1BB74C:	; DMA source to VRAM
	incrom	$1BB74C,$200
Art_1BB94C:	; DMA source to VRAM
	incrom	$1BB94C,$66
Art_1BB9B2:	; DMA source to VRAM
	incrom	$1BB9B2,$1E8
Art_1BBB9A:	; DMA source to VRAM
	incrom	$1BBB9A,$180
Art_1BBD1A:	; DMA source to VRAM
	incrom	$1BBD1A,$108
Art_1BBE22:	; DMA source to VRAM
	incrom	$1BBE22,$180
Art_1BBFA2:	; DMA source to VRAM
	incrom	$1BBFA2,$1E8
Art_1BC18A:	; DMA source to VRAM
	incrom	$1BC18A,$200
Art_1BC38A:	; DMA source to VRAM
	incrom	$1BC38A,$26
Art_1BC3B0:	; DMA source to VRAM
	incrom	$1BC3B0,$C6
Art_1BC476:	; DMA source to VRAM
	incrom	$1BC476,$206
Art_1BC67C:	; DMA source to VRAM
	incrom	$1BC67C,$146
Art_1BC7C2:	; DMA source to VRAM
	incrom	$1BC7C2,$146
Art_1BC908:	; DMA source to VRAM
	incrom	$1BC908,$146
Art_1BCA4E:	; DMA source to VRAM
	incrom	$1BCA4E,$146
Art_1BCB94:	; DMA source to VRAM
	incrom	$1BCB94,$166
Art_1BCCFA:	; DMA source to VRAM
	incrom	$1BCCFA,$146
Art_1BCE40:	; DMA source to VRAM
	incrom	$1BCE40,$166
Art_1BCFA6:	; DMA source to VRAM
	incrom	$1BCFA6,$166
Art_1BD10C:	; DMA source to VRAM
	incrom	$1BD10C,$188
Art_1BD294:	; DMA source to VRAM
	incrom	$1BD294,$40A
Art_1BD69E:	; DMA source to VRAM
	incrom	$1BD69E,$606
Art_1BDCA4:	; DMA source to VRAM
	incrom	$1BDCA4,$1CE
Art_1BDE72:	; DMA source to VRAM
	incrom	$1BDE72,$200
Art_1BE072:	; DMA source to VRAM
	incrom	$1BE072,$200
Art_1BE272:	; DMA source to VRAM
	incrom	$1BE272,$200
Art_1BE472:	; DMA source to VRAM
	incrom	$1BE472,$1A0
Art_1BE612:	; DMA source to VRAM
	incrom	$1BE612,$C6
Art_1BE6D8:	; DMA source to VRAM
	incrom	$1BE6D8,$14A
Art_1BE822:	; DMA source to VRAM
	incrom	$1BE822,$200
Art_1BEA22:	; DMA source to VRAM
	incrom	$1BEA22,$1E0
Art_1BEC02:	; DMA source to VRAM
	incrom	$1BEC02,$48
Art_1BEC4A:	; DMA source to VRAM
	incrom	$1BEC4A,$200
Art_1BEE4A:	; DMA source to VRAM
	incrom	$1BEE4A,$206
Art_1BF050:	; DMA source to VRAM
	incrom	$1BF050,$1E6
Art_1BF236:	; DMA source to VRAM
	incrom	$1BF236,$1C6
Art_1BF3FC:	; DMA source to VRAM
	incrom	$1BF3FC,$1E6
Art_1BF5E2:	; DMA source to VRAM
	incrom	$1BF5E2,$1E8
Art_1BF7CA:	; DMA source to VRAM
	incrom	$1BF7CA,$200
Art_1BF9CA:	; DMA source to VRAM
	incrom	$1BF9CA,$46
Art_1BFA10:	; DMA source to VRAM
	incrom	$1BFA10,$1E6
Art_1BFBF6:	; DMA source to VRAM
	incrom	$1BFBF6,$1E6
Art_1BFDDC:	; DMA source to VRAM; 88% read in the corpus
	incrom	$1BFDDC,$22C
Art_1C0008:	; DMA source to VRAM
	incrom	$1C0008,$200
Art_1C0208:	; DMA source to VRAM
	incrom	$1C0208,$26
Art_1C022E:	; DMA source to VRAM
	incrom	$1C022E,$206
Art_1C0434:	; DMA source to VRAM
	incrom	$1C0434,$1E6
Art_1C061A:	; DMA source to VRAM
	incrom	$1C061A,$1EC
Art_1C0806:	; DMA source to VRAM
	incrom	$1C0806,$180
Art_1C0986:	; DMA source to VRAM
	incrom	$1C0986,$180
Art_1C0B06:	; DMA source to VRAM
	incrom	$1C0B06,$180
Art_1C0C86:	; DMA source to VRAM
	incrom	$1C0C86,$CC
Art_1C0D52:	; DMA source to VRAM
	incrom	$1C0D52,$180
Art_1C0ED2:	; DMA source to VRAM
	incrom	$1C0ED2,$180
Art_1C1052:	; DMA source to VRAM
	incrom	$1C1052,$180
Art_1C11D2:	; DMA source to VRAM
	incrom	$1C11D2,$CC
Art_1C129E:	; DMA source to VRAM
	incrom	$1C129E,$180
Art_1C141E:	; DMA source to VRAM
	incrom	$1C141E,$180
Art_1C159E:	; DMA source to VRAM
	incrom	$1C159E,$180
Art_1C171E:	; DMA source to VRAM
	incrom	$1C171E,$CC
Art_1C17EA:	; DMA source to VRAM
	incrom	$1C17EA,$180
Art_1C196A:	; DMA source to VRAM
	incrom	$1C196A,$180
Art_1C1AEA:	; DMA source to VRAM
	incrom	$1C1AEA,$180
Art_1C1C6A:	; DMA source to VRAM
	incrom	$1C1C6A,$CC
Art_1C1D36:	; DMA source to VRAM
	incrom	$1C1D36,$180
Art_1C1EB6:	; DMA source to VRAM
	incrom	$1C1EB6,$180
Art_1C2036:	; DMA source to VRAM
	incrom	$1C2036,$180
Art_1C21B6:	; DMA source to VRAM
	incrom	$1C21B6,$CC
Art_1C2282:	; DMA source to VRAM
	incrom	$1C2282,$180
Art_1C2402:	; DMA source to VRAM
	incrom	$1C2402,$180
Art_1C2582:	; DMA source to VRAM
	incrom	$1C2582,$180
Art_1C2702:	; DMA source to VRAM
	incrom	$1C2702,$CC
Art_1C27CE:	; DMA source to VRAM
	incrom	$1C27CE,$180
Art_1C294E:	; DMA source to VRAM
	incrom	$1C294E,$180
Art_1C2ACE:	; DMA source to VRAM
	incrom	$1C2ACE,$180
Art_1C2C4E:	; DMA source to VRAM
	incrom	$1C2C4E,$CC
Art_1C2D1A:	; DMA source to VRAM
	incrom	$1C2D1A,$180
Art_1C2E9A:	; DMA source to VRAM
	incrom	$1C2E9A,$180
Art_1C301A:	; DMA source to VRAM
	incrom	$1C301A,$180
Art_1C319A:	; DMA source to VRAM
	incrom	$1C319A,$CC
Art_1C3266:	; DMA source to VRAM
	incrom	$1C3266,$180
Art_1C33E6:	; DMA source to VRAM
	incrom	$1C33E6,$180
Art_1C3566:	; DMA source to VRAM
	incrom	$1C3566,$180
Art_1C36E6:	; DMA source to VRAM
	incrom	$1C36E6,$CE
Art_1C37B4:	; DMA source to VRAM
	incrom	$1C37B4,$8CE
Art_1C4082:	; DMA source to VRAM
	incrom	$1C4082,$400
Art_1C4482:	; DMA source to VRAM
	incrom	$1C4482,$200
Art_1C4682:	; DMA source to VRAM
	incrom	$1C4682,$200
Art_1C4882:	; DMA source to VRAM
	incrom	$1C4882,$AE
Art_1C4930:	; DMA source to VRAM
	incrom	$1C4930,$200
Art_1C4B30:	; DMA source to VRAM
	incrom	$1C4B30,$200
Art_1C4D30:	; DMA source to VRAM
	incrom	$1C4D30,$200
Art_1C4F30:	; DMA source to VRAM
	incrom	$1C4F30,$36E
Art_1C529E:	; DMA source to VRAM
	incrom	$1C529E,$200
Art_1C549E:	; DMA source to VRAM
	incrom	$1C549E,$200
Art_1C569E:	; DMA source to VRAM
	incrom	$1C569E,$200
Art_1C589E:	; DMA source to VRAM
	incrom	$1C589E,$200
Art_1C5A9E:	; DMA source to VRAM
	incrom	$1C5A9E,$1B0
Art_1C5C4E:	; DMA source to VRAM
	incrom	$1C5C4E,$200
Art_1C5E4E:	; DMA source to VRAM
	incrom	$1C5E4E,$200
Art_1C604E:	; DMA source to VRAM
	incrom	$1C604E,$200
Art_1C624E:	; DMA source to VRAM
	incrom	$1C624E,$200
Art_1C644E:	; DMA source to VRAM
	incrom	$1C644E,$200
Art_1C664E:	; DMA source to VRAM
	incrom	$1C664E,$72
Art_1C66C0:	; DMA source to VRAM
	incrom	$1C66C0,$200
Art_1C68C0:	; DMA source to VRAM
	incrom	$1C68C0,$200
Art_1C6AC0:	; DMA source to VRAM
	incrom	$1C6AC0,$200
Art_1C6CC0:	; DMA source to VRAM
	incrom	$1C6CC0,$200
Art_1C6EC0:	; DMA source to VRAM
	incrom	$1C6EC0,$200
Art_1C70C0:	; DMA source to VRAM
	incrom	$1C70C0,$200
Art_1C72C0:	; DMA source to VRAM
	incrom	$1C72C0,$32
Art_1C72F2:	; DMA source to VRAM
	incrom	$1C72F2,$200
Art_1C74F2:	; DMA source to VRAM
	incrom	$1C74F2,$200
Art_1C76F2:	; DMA source to VRAM
	incrom	$1C76F2,$200
Art_1C78F2:	; DMA source to VRAM
	incrom	$1C78F2,$200
Art_1C7AF2:	; DMA source to VRAM
	incrom	$1C7AF2,$200
Art_1C7CF2:	; DMA source to VRAM
	incrom	$1C7CF2,$200
Art_1C7EF2:	; DMA source to VRAM
	incrom	$1C7EF2,$32
Art_1C7F24:	; DMA source to VRAM
	incrom	$1C7F24,$200
Art_1C8124:	; DMA source to VRAM
	incrom	$1C8124,$200
Art_1C8324:	; DMA source to VRAM
	incrom	$1C8324,$200
Art_1C8524:	; DMA source to VRAM
	incrom	$1C8524,$200
Art_1C8724:	; DMA source to VRAM
	incrom	$1C8724,$200
Art_1C8924:	; DMA source to VRAM
	incrom	$1C8924,$200
Art_1C8B24:	; DMA source to VRAM
	incrom	$1C8B24,$2A
Art_1C8B4E:	; DMA source to VRAM
	incrom	$1C8B4E,$180
Art_1C8CCE:	; DMA source to VRAM
	incrom	$1C8CCE,$180
Art_1C8E4E:	; DMA source to VRAM
	incrom	$1C8E4E,$188
Art_1C8FD6:	; DMA source to VRAM
	incrom	$1C8FD6,$200
Art_1C91D6:	; DMA source to VRAM
	incrom	$1C91D6,$1C8
Art_1C939E:	; DMA source to VRAM
	incrom	$1C939E,$200
Art_1C959E:	; DMA source to VRAM
	incrom	$1C959E,$108
Art_1C96A6:	; DMA source to VRAM
	incrom	$1C96A6,$200
Art_1C98A6:	; DMA source to VRAM
	incrom	$1C98A6,$108
Art_1C99AE:	; DMA source to VRAM
	incrom	$1C99AE,$200
Art_1C9BAE:	; DMA source to VRAM
	incrom	$1C9BAE,$108
Art_1C9CB6:	; DMA source to VRAM
	incrom	$1C9CB6,$200
Art_1C9EB6:	; DMA source to VRAM
	incrom	$1C9EB6,$108
Art_1C9FBE:	; DMA source to VRAM
	incrom	$1C9FBE,$200
Art_1CA1BE:	; DMA source to VRAM
	incrom	$1CA1BE,$108
Art_1CA2C6:	; DMA source to VRAM
	incrom	$1CA2C6,$200
Art_1CA4C6:	; DMA source to VRAM
	incrom	$1CA4C6,$108
Art_1CA5CE:	; DMA source to VRAM
	incrom	$1CA5CE,$200
Art_1CA7CE:	; DMA source to VRAM
	incrom	$1CA7CE,$108
Art_1CA8D6:	; DMA source to VRAM
	incrom	$1CA8D6,$200
Art_1CAAD6:	; DMA source to VRAM
	incrom	$1CAAD6,$108
Art_1CABDE:	; DMA source to VRAM
	incrom	$1CABDE,$200
Art_1CADDE:	; DMA source to VRAM
	incrom	$1CADDE,$108
Art_1CAEE6:	; DMA source to VRAM
	incrom	$1CAEE6,$200
Art_1CB0E6:	; DMA source to VRAM
	incrom	$1CB0E6,$108
Art_1CB1EE:	; DMA source to VRAM
	incrom	$1CB1EE,$200
Art_1CB3EE:	; DMA source to VRAM
	incrom	$1CB3EE,$108
Art_1CB4F6:	; DMA source to VRAM
	incrom	$1CB4F6,$200
Art_1CB6F6:	; DMA source to VRAM
	incrom	$1CB6F6,$108
Art_1CB7FE:	; DMA source to VRAM
	incrom	$1CB7FE,$200
Art_1CB9FE:	; DMA source to VRAM
	incrom	$1CB9FE,$108
Art_1CBB06:	; DMA source to VRAM
	incrom	$1CBB06,$200
Art_1CBD06:	; DMA source to VRAM
	incrom	$1CBD06,$10E
Art_1CBE14:	; DMA source to VRAM
	incrom	$1CBE14,$1A0
Art_1CBFB4:	; DMA source to VRAM
	incrom	$1CBFB4,$1A0
Art_1CC154:	; DMA source to VRAM
	incrom	$1CC154,$1A0
Art_1CC2F4:	; DMA source to VRAM
	incrom	$1CC2F4,$1A0
Art_1CC494:	; DMA source to VRAM; 56% read in the corpus
	incrom	$1CC494,$20C
Art_1CC6A0:	; DMA source to VRAM
	incrom	$1CC6A0,$1E6
Art_1CC886:	; DMA source to VRAM
	incrom	$1CC886,$1E6
Art_1CCA6C:	; DMA source to VRAM
	incrom	$1CCA6C,$1E0
Art_1CCC4C:	; sprite art block: 15 tiles in 1 chunk(s), DMA cost 15 (frame table entry $1E2598); never displayed in the corpus
	incrom	$1CCC4C,$1E6
Art_1CCE32:	; sprite art block: 15 tiles in 1 chunk(s), DMA cost 15 (frame table entry $1E25A2); never displayed in the corpus
	incrom	$1CCE32,$1E6
Art_1CD018:	; sprite art block: 15 tiles in 1 chunk(s), DMA cost 15 (frame table entry $1E25AC); never displayed in the corpus
	incrom	$1CD018,$1E6
Tbl_1CD1FE:	; read by Spr_SwapFrameArt (pc $007F70), Spr_QueueArtDMA (pc $0080BE)
	incrom	$1CD1FE,$6
Art_1CD204:	; DMA source to VRAM
	incrom	$1CD204,$1E0
Art_1CD3E4:	; sprite art block: 15 tiles in 1 chunk(s), DMA cost 15 (frame table entry $1E25C0); never displayed in the corpus
	incrom	$1CD3E4,$1E6
Tbl_1CD5CA:	; read by Spr_SwapFrameArt (pc $007F70), Spr_QueueArtDMA (pc $0080BE)
	incrom	$1CD5CA,$6
Art_1CD5D0:	; DMA source to VRAM
	incrom	$1CD5D0,$126
Art_1CD6F6:	; DMA source to VRAM
	incrom	$1CD6F6,$126
Art_1CD81C:	; DMA source to VRAM
	incrom	$1CD81C,$86
Art_1CD8A2:	; DMA source to VRAM
	incrom	$1CD8A2,$86
Art_1CD928:	; DMA source to VRAM
	incrom	$1CD928,$86
Art_1CD9AE:	; DMA source to VRAM
	incrom	$1CD9AE,$80
Art_1CDA2E:	; sprite art block: 15 tiles in 1 chunk(s), DMA cost 15 (frame table entry $1E2606); never displayed in the corpus
	incrom	$1CDA2E,$1E6
Tbl_1CDC14:	; read by Spr_SwapFrameArt (pc $007F70), Spr_QueueArtDMA (pc $008098)
	incrom	$1CDC14,$8
Art_1CDC1C:	; DMA source to VRAM
	incrom	$1CDC1C,$200
Art_1CDE1C:	; DMA source to VRAM
	incrom	$1CDE1C,$146
Art_1CDF62:	; DMA source to VRAM
	incrom	$1CDF62,$166
Art_1CE0C8:	; DMA source to VRAM
	incrom	$1CE0C8,$166
Art_1CE22E:	; DMA source to VRAM
	incrom	$1CE22E,$E6
Art_1CE314:	; DMA source to VRAM
	incrom	$1CE314,$E6
Art_1CE3FA:	; DMA source to VRAM
	incrom	$1CE3FA,$E6
Art_1CE4E0:	; DMA source to VRAM
	incrom	$1CE4E0,$86
Art_1CE566:	; DMA source to VRAM
	incrom	$1CE566,$86
Art_1CE5EC:	; DMA source to VRAM
	incrom	$1CE5EC,$86
Art_1CE672:	; DMA source to VRAM
	incrom	$1CE672,$86
Art_1CE6F8:	; DMA source to VRAM
	incrom	$1CE6F8,$86
Art_1CE77E:	; DMA source to VRAM
	incrom	$1CE77E,$86
Art_1CE804:	; DMA source to VRAM
	incrom	$1CE804,$86
Art_1CE88A:	; DMA source to VRAM
	incrom	$1CE88A,$86
Art_1CE910:	; DMA source to VRAM
	incrom	$1CE910,$86
Art_1CE996:	; DMA source to VRAM
	incrom	$1CE996,$86
Art_1CEA1C:	; DMA source to VRAM
	incrom	$1CEA1C,$86
Art_1CEAA2:	; DMA source to VRAM
	incrom	$1CEAA2,$188
Art_1CEC2A:	; DMA source to VRAM
	incrom	$1CEC2A,$180
Art_1CEDAA:	; DMA source to VRAM
	incrom	$1CEDAA,$C8
Art_1CEE72:	; DMA source to VRAM
	incrom	$1CEE72,$180
Art_1CEFF2:	; DMA source to VRAM
	incrom	$1CEFF2,$C8
Art_1CF0BA:	; DMA source to VRAM
	incrom	$1CF0BA,$180
Art_1CF23A:	; DMA source to VRAM
	incrom	$1CF23A,$C0
Art_1CF2FA:	; sprite art block: 12 tiles in 1 chunk(s), DMA cost 12 (frame table entry $1E2732); never displayed in the corpus
	incrom	$1CF2FA,$186
Art_1CF480:	; sprite art block: 18 tiles in 2 chunk(s), DMA cost 18 (frame table entry $1E273C); never displayed in the corpus
	incrom	$1CF480,$248
Art_1CF6C8:	; sprite art block: 18 tiles in 2 chunk(s), DMA cost 18 (frame table entry $1E2746); never displayed in the corpus
	incrom	$1CF6C8,$248
Art_1CF910:	; sprite art block: 18 tiles in 2 chunk(s), DMA cost 18 (frame table entry $1E2750); never displayed in the corpus
	incrom	$1CF910,$248
Art_1CFB58:	; sprite art block: 34 tiles in 3 chunk(s), DMA cost 34 (frame table entry $1E275A); never displayed in the corpus
	incrom	$1CFB58,$44A
Tbl_1CFFA2:	; read by Spr_SwapFrameArt (pc $007F70), Spr_QueueArtDMA (pc $0080BE)
	incrom	$1CFFA2,$6
Art_1CFFA8:	; DMA source to VRAM
	incrom	$1CFFA8,$126
Art_1D00CE:	; DMA source to VRAM
	incrom	$1D00CE,$106
Art_1D01D4:	; DMA source to VRAM
	incrom	$1D01D4,$146
Art_1D031A:	; DMA source to VRAM
	incrom	$1D031A,$186
Art_1D04A0:	; DMA source to VRAM
	incrom	$1D04A0,$106
Art_1D05A6:	; DMA source to VRAM
	incrom	$1D05A6,$186
Art_1D072C:	; DMA source to VRAM
	incrom	$1D072C,$146
Art_1D0872:	; DMA source to VRAM
	incrom	$1D0872,$166
Art_1D09D8:	; DMA source to VRAM
	incrom	$1D09D8,$126
Art_1D0AFE:	; DMA source to VRAM
	incrom	$1D0AFE,$106
Art_1D0C04:	; DMA source to VRAM
	incrom	$1D0C04,$E6
Art_1D0CEA:	; DMA source to VRAM
	incrom	$1D0CEA,$106
Art_1D0DF0:	; DMA source to VRAM
	incrom	$1D0DF0,$E6
Art_1D0ED6:	; DMA source to VRAM
	incrom	$1D0ED6,$C0
Map_1D0F96:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DDB6A); never displayed in the corpus
	incrom	$1D0F96,$E
Map_1D0FA4:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DDB74); never displayed in the corpus
	incrom	$1D0FA4,$E
Map_1D0FB2:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DDB7E); never displayed in the corpus
	incrom	$1D0FB2,$E
Map_1D0FC0:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DDB88); never displayed in the corpus
	incrom	$1D0FC0,$E
Map_1D0FCE:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DDB92); never displayed in the corpus
	incrom	$1D0FCE,$E
Map_1D0FDC:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DDB9C); never displayed in the corpus
	incrom	$1D0FDC,$E
Map_1D0FEA:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DDBA6); never displayed in the corpus
	incrom	$1D0FEA,$E
Map_1D0FF8:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DDBB0); never displayed in the corpus
	incrom	$1D0FF8,$E
Map_1D1006:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DDBBA); never displayed in the corpus
	incrom	$1D1006,$E
Map_1D1014:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DDBC4); never displayed in the corpus
	incrom	$1D1014,$E
Map_1D1022:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DDBCE); never displayed in the corpus
	incrom	$1D1022,$E
Map_1D1030:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DDBD8); never displayed in the corpus
	incrom	$1D1030,$E
Map_1D103E:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DDBE2); never displayed in the corpus
	incrom	$1D103E,$E
Map_1D104C:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DDBEC); never displayed in the corpus
	incrom	$1D104C,$E
Map_1D105A:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DDBF6); never displayed in the corpus
	incrom	$1D105A,$E
Map_1D1068:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DDC00); never displayed in the corpus
	incrom	$1D1068,$E
Map_1D1076:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DDC0A); never displayed in the corpus
	incrom	$1D1076,$E
Map_1D1084:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DDC14); never displayed in the corpus
	incrom	$1D1084,$E
Map_1D1092:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DDC1E); never displayed in the corpus
	incrom	$1D1092,$E
Map_1D10A0:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DDC28); never displayed in the corpus
	incrom	$1D10A0,$E
Map_1D10AE:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DDC32); never displayed in the corpus
	incrom	$1D10AE,$E
Map_1D10BC:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DDC3C); never displayed in the corpus
	incrom	$1D10BC,$E
Map_1D10CA:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DDC46); never displayed in the corpus
	incrom	$1D10CA,$E
Map_1D10D8:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DDC50); never displayed in the corpus
	incrom	$1D10D8,$E
Map_1D10E6:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DDC5A); never displayed in the corpus
	incrom	$1D10E6,$E
Map_1D10F4:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DDC64); never displayed in the corpus
	incrom	$1D10F4,$E
Tbl_1D1102:	; read by Spr_BuildSAT (pc $007BC8); 98% read in the corpus
	incrom	$1D1102,$2DF2
Map_1D3EF4:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DE95C); never displayed in the corpus
	incrom	$1D3EF4,$E
Map_1D3F02:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DE966); never displayed in the corpus
	incrom	$1D3F02,$1A
Map_1D3F1C:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DE970); never displayed in the corpus
	incrom	$1D3F1C,$E
Map_1D3F2A:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DE97A); never displayed in the corpus
	incrom	$1D3F2A,$E
Map_1D3F38:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DE984); never displayed in the corpus
	incrom	$1D3F38,$E
Map_1D3F46:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DE98E); never displayed in the corpus
	incrom	$1D3F46,$1A
Map_1D3F60:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DE998); never displayed in the corpus
	incrom	$1D3F60,$E
Map_1D3F6E:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DE9A2); never displayed in the corpus
	incrom	$1D3F6E,$E
Map_1D3F7C:	; sprite piece list: 3 piece(s) of 12 bytes (frame table entry $1DE9AC); never displayed in the corpus
	incrom	$1D3F7C,$26
Map_1D3FA2:	; sprite piece list: 3 piece(s) of 12 bytes (frame table entry $1DE9B6); never displayed in the corpus
	incrom	$1D3FA2,$26
Map_1D3FC8:	; sprite piece list: 3 piece(s) of 12 bytes (frame table entry $1DE9C0); never displayed in the corpus
	incrom	$1D3FC8,$26
Map_1D3FEE:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DE9CA); never displayed in the corpus
	incrom	$1D3FEE,$1A
Map_1D4008:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DE9D4); never displayed in the corpus
	incrom	$1D4008,$1A
Map_1D4022:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DE9DE); never displayed in the corpus
	incrom	$1D4022,$1A
Map_1D403C:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DE9E8); never displayed in the corpus
	incrom	$1D403C,$1A
Map_1D4056:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DE9F2); never displayed in the corpus
	incrom	$1D4056,$1A
Map_1D4070:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DE9FC); never displayed in the corpus
	incrom	$1D4070,$1A
Map_1D408A:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DEA06); never displayed in the corpus
	incrom	$1D408A,$1A
Map_1D40A4:	; sprite piece list: 3 piece(s) of 12 bytes (frame table entry $1DEA10); never displayed in the corpus
	incrom	$1D40A4,$26
Map_1D40CA:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DEA1A); never displayed in the corpus
	incrom	$1D40CA,$1A
Map_1D40E4:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DEA24); never displayed in the corpus
	incrom	$1D40E4,$1A
Map_1D40FE:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DEA2E); never displayed in the corpus
	incrom	$1D40FE,$1A
Map_1D4118:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DEA38); never displayed in the corpus
	incrom	$1D4118,$1A
Map_1D4132:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DEA42); never displayed in the corpus
	incrom	$1D4132,$1A
Map_1D414C:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DEA4C); never displayed in the corpus
	incrom	$1D414C,$1A
Map_1D4166:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DEA56); never displayed in the corpus
	incrom	$1D4166,$E
Map_1D4174:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DEA60); never displayed in the corpus
	incrom	$1D4174,$1A
Map_1D418E:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DEA6A); never displayed in the corpus
	incrom	$1D418E,$1A
Map_1D41A8:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DEA74); never displayed in the corpus
	incrom	$1D41A8,$1A
Map_1D41C2:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DEA7E); never displayed in the corpus
	incrom	$1D41C2,$E
Map_1D41D0:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DEA88); never displayed in the corpus
	incrom	$1D41D0,$1A
Map_1D41EA:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DEA92); never displayed in the corpus
	incrom	$1D41EA,$1A
Map_1D4204:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DEA9C); never displayed in the corpus
	incrom	$1D4204,$1A
Map_1D421E:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DEAA6); never displayed in the corpus
	incrom	$1D421E,$E
Map_1D422C:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DEAB0); never displayed in the corpus
	incrom	$1D422C,$E
Map_1D423A:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DEABA); never displayed in the corpus
	incrom	$1D423A,$E
Map_1D4248:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DEAC4); never displayed in the corpus
	incrom	$1D4248,$1A
Map_1D4262:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DEACE); never displayed in the corpus
	incrom	$1D4262,$1A
Map_1D427C:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DEAD8); never displayed in the corpus
	incrom	$1D427C,$1A
Map_1D4296:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DEAE2); never displayed in the corpus
	incrom	$1D4296,$1A
Map_1D42B0:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DEAEC); never displayed in the corpus
	incrom	$1D42B0,$E
Map_1D42BE:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DEAF6); never displayed in the corpus
	incrom	$1D42BE,$E
Map_1D42CC:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DEB00); never displayed in the corpus
	incrom	$1D42CC,$E
Map_1D42DA:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DEB0A); never displayed in the corpus
	incrom	$1D42DA,$E
Map_1D42E8:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DEB14); never displayed in the corpus
	incrom	$1D42E8,$1A
Map_1D4302:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DEB1E); never displayed in the corpus
	incrom	$1D4302,$E
Map_1D4310:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DEB28); never displayed in the corpus
	incrom	$1D4310,$1A
Map_1D432A:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DEB32); never displayed in the corpus
	incrom	$1D432A,$E
Map_1D4338:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DEB3C); never displayed in the corpus
	incrom	$1D4338,$1A
Map_1D4352:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DEB46); never displayed in the corpus
	incrom	$1D4352,$1A
Map_1D436C:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DEB50); never displayed in the corpus
	incrom	$1D436C,$E
Map_1D437A:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DEB5A); never displayed in the corpus
	incrom	$1D437A,$E
Map_1D4388:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DEB64); never displayed in the corpus
	incrom	$1D4388,$1A
Map_1D43A2:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DEB6E); never displayed in the corpus
	incrom	$1D43A2,$1A
Map_1D43BC:	; sprite piece list: 3 piece(s) of 12 bytes (frame table entry $1DEB78); never displayed in the corpus
	incrom	$1D43BC,$26
Map_1D43E2:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DEB82); never displayed in the corpus
	incrom	$1D43E2,$1A
Map_1D43FC:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DEB8C); never displayed in the corpus
	incrom	$1D43FC,$1A
Map_1D4416:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DEB96); never displayed in the corpus
	incrom	$1D4416,$E
Map_1D4424:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DEBA0); never displayed in the corpus
	incrom	$1D4424,$1A
Map_1D443E:	; sprite piece list: 4 piece(s) of 12 bytes (frame table entry $1DEBAA); never displayed in the corpus
	incrom	$1D443E,$32
Map_1D4470:	; sprite piece list: 3 piece(s) of 12 bytes (frame table entry $1DEBB4); never displayed in the corpus
	incrom	$1D4470,$26
Map_1D4496:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DEBBE); never displayed in the corpus
	incrom	$1D4496,$1A
Map_1D44B0:	; sprite piece list: 4 piece(s) of 12 bytes (frame table entry $1DEBC8); never displayed in the corpus
	incrom	$1D44B0,$32
Map_1D44E2:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DEBD2); never displayed in the corpus
	incrom	$1D44E2,$E
Map_1D44F0:	; sprite piece list: 3 piece(s) of 12 bytes (frame table entry $1DEBDC); never displayed in the corpus
	incrom	$1D44F0,$26
Tbl_1D4516:	; read by Spr_BuildSAT (pc $007BC8); 91% read in the corpus
	incrom	$1D4516,$3F6
Map_1D490C:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DECB8); never displayed in the corpus
	incrom	$1D490C,$1A
Map_1D4926:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DECC2); never displayed in the corpus
	incrom	$1D4926,$1A
Map_1D4940:	; sprite piece list: 3 piece(s) of 12 bytes (frame table entry $1DECCC); never displayed in the corpus
	incrom	$1D4940,$26
Map_1D4966:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DECD6); never displayed in the corpus
	incrom	$1D4966,$1A
Map_1D4980:	; sprite piece list: 4 piece(s) of 12 bytes (frame table entry $1DECE0); never displayed in the corpus
	incrom	$1D4980,$32
Map_1D49B2:	; sprite piece list: 5 piece(s) of 12 bytes (frame table entry $1DECEA); never displayed in the corpus
	incrom	$1D49B2,$3E
Map_1D49F0:	; sprite piece list: 5 piece(s) of 12 bytes (frame table entry $1DECF4); never displayed in the corpus
	incrom	$1D49F0,$3E
Map_1D4A2E:	; sprite piece list: 3 piece(s) of 12 bytes (frame table entry $1DECFE); never displayed in the corpus
	incrom	$1D4A2E,$26
Map_1D4A54:	; sprite piece list: 7 piece(s) of 12 bytes (frame table entry $1DED08); never displayed in the corpus
	incrom	$1D4A54,$56
Map_1D4AAA:	; sprite piece list: 5 piece(s) of 12 bytes (frame table entry $1DED12); never displayed in the corpus
	incrom	$1D4AAA,$3E
Map_1D4AE8:	; sprite piece list: 9 piece(s) of 12 bytes (frame table entry $1DED1C); never displayed in the corpus
	incrom	$1D4AE8,$6E
Map_1D4B56:	; sprite piece list: 6 piece(s) of 12 bytes (frame table entry $1DED26); never displayed in the corpus
	incrom	$1D4B56,$4A
Map_1D4BA0:	; sprite piece list: 3 piece(s) of 12 bytes (frame table entry $1DED30); never displayed in the corpus
	incrom	$1D4BA0,$26
Map_1D4BC6:	; sprite piece list: 6 piece(s) of 12 bytes (frame table entry $1DED3A); never displayed in the corpus
	incrom	$1D4BC6,$4A
Map_1D4C10:	; sprite piece list: 6 piece(s) of 12 bytes (frame table entry $1DED44); never displayed in the corpus
	incrom	$1D4C10,$4A
Map_1D4C5A:	; sprite piece list: 7 piece(s) of 12 bytes (frame table entry $1DED4E); never displayed in the corpus
	incrom	$1D4C5A,$56
Map_1D4CB0:	; sprite piece list: 7 piece(s) of 12 bytes (frame table entry $1DED58); never displayed in the corpus
	incrom	$1D4CB0,$56
Map_1D4D06:	; sprite piece list: 6 piece(s) of 12 bytes (frame table entry $1DED62); never displayed in the corpus
	incrom	$1D4D06,$4A
Map_1D4D50:	; sprite piece list: 4 piece(s) of 12 bytes (frame table entry $1DED6C); never displayed in the corpus
	incrom	$1D4D50,$32
Map_1D4D82:	; sprite piece list: 4 piece(s) of 12 bytes (frame table entry $1DED76); never displayed in the corpus
	incrom	$1D4D82,$32
Map_1D4DB4:	; sprite piece list: 7 piece(s) of 12 bytes (frame table entry $1DED80); never displayed in the corpus
	incrom	$1D4DB4,$56
Map_1D4E0A:	; sprite piece list: 5 piece(s) of 12 bytes (frame table entry $1DED8A); never displayed in the corpus
	incrom	$1D4E0A,$3E
Map_1D4E48:	; sprite piece list: 4 piece(s) of 12 bytes (frame table entry $1DED94); never displayed in the corpus
	incrom	$1D4E48,$32
Map_1D4E7A:	; sprite piece list: 3 piece(s) of 12 bytes (frame table entry $1DED9E); never displayed in the corpus
	incrom	$1D4E7A,$26
Map_1D4EA0:	; sprite piece list: 3 piece(s) of 12 bytes (frame table entry $1DEDA8); never displayed in the corpus
	incrom	$1D4EA0,$26
Map_1D4EC6:	; sprite piece list: 4 piece(s) of 12 bytes (frame table entry $1DEDB2); never displayed in the corpus
	incrom	$1D4EC6,$32
Map_1D4EF8:	; sprite piece list: 4 piece(s) of 12 bytes (frame table entry $1DEDBC); never displayed in the corpus
	incrom	$1D4EF8,$32
Map_1D4F2A:	; sprite piece list: 5 piece(s) of 12 bytes (frame table entry $1DEDC6); never displayed in the corpus
	incrom	$1D4F2A,$3E
Tbl_1D4F68:	; read by Spr_BuildSAT (pc $007BC8)
	incrom	$1D4F68,$150
Map_1D50B8:	; sprite piece list: 3 piece(s) of 12 bytes (frame table entry $1DEE0C); never displayed in the corpus
	incrom	$1D50B8,$26
Map_1D50DE:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DEE16); never displayed in the corpus
	incrom	$1D50DE,$1A
Map_1D50F8:	; sprite piece list: 4 piece(s) of 12 bytes (frame table entry $1DEE20); never displayed in the corpus
	incrom	$1D50F8,$32
Map_1D512A:	; sprite piece list: 4 piece(s) of 12 bytes (frame table entry $1DEE2A); never displayed in the corpus
	incrom	$1D512A,$32
Map_1D515C:	; sprite piece list: 5 piece(s) of 12 bytes (frame table entry $1DEE34); never displayed in the corpus
	incrom	$1D515C,$3E
Map_1D519A:	; sprite piece list: 4 piece(s) of 12 bytes (frame table entry $1DEE3E); never displayed in the corpus
	incrom	$1D519A,$32
Map_1D51CC:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DEE48); never displayed in the corpus
	incrom	$1D51CC,$1A
Map_1D51E6:	; sprite piece list: 3 piece(s) of 12 bytes (frame table entry $1DEE52); never displayed in the corpus
	incrom	$1D51E6,$26
Map_1D520C:	; sprite piece list: 5 piece(s) of 12 bytes (frame table entry $1DEE5C); never displayed in the corpus
	incrom	$1D520C,$3E
Map_1D524A:	; sprite piece list: 4 piece(s) of 12 bytes (frame table entry $1DEE66); never displayed in the corpus
	incrom	$1D524A,$32
Map_1D527C:	; sprite piece list: 5 piece(s) of 12 bytes (frame table entry $1DEE70); never displayed in the corpus
	incrom	$1D527C,$3E
Tbl_1D52BA:	; read by Spr_BuildSAT (pc $007BC8); 83% read in the corpus
	incrom	$1D52BA,$52E
Map_1D57E8:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF000); never displayed in the corpus
	incrom	$1D57E8,$E
Map_1D57F6:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF00A); never displayed in the corpus
	incrom	$1D57F6,$E
Map_1D5804:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF014); never displayed in the corpus
	incrom	$1D5804,$E
Map_1D5812:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF01E); never displayed in the corpus
	incrom	$1D5812,$E
Map_1D5820:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF028); never displayed in the corpus
	incrom	$1D5820,$E
Map_1D582E:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF032); never displayed in the corpus
	incrom	$1D582E,$E
Map_1D583C:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF03C); never displayed in the corpus
	incrom	$1D583C,$E
Map_1D584A:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF046); never displayed in the corpus
	incrom	$1D584A,$1A
Map_1D5864:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF050); never displayed in the corpus
	incrom	$1D5864,$E
Map_1D5872:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF05A); never displayed in the corpus
	incrom	$1D5872,$E
Map_1D5880:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF064); never displayed in the corpus
	incrom	$1D5880,$E
Map_1D588E:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF06E); never displayed in the corpus
	incrom	$1D588E,$E
Map_1D589C:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF078); never displayed in the corpus
	incrom	$1D589C,$1A
Map_1D58B6:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF082); never displayed in the corpus
	incrom	$1D58B6,$1A
Map_1D58D0:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF08C); never displayed in the corpus
	incrom	$1D58D0,$1A
Map_1D58EA:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF096); never displayed in the corpus
	incrom	$1D58EA,$1A
Map_1D5904:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF0A0); never displayed in the corpus
	incrom	$1D5904,$1A
Map_1D591E:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF0AA); never displayed in the corpus
	incrom	$1D591E,$1A
Map_1D5938:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF0B4); never displayed in the corpus
	incrom	$1D5938,$1A
Map_1D5952:	; sprite piece list: 4 piece(s) of 12 bytes (frame table entry $1DF0BE); never displayed in the corpus
	incrom	$1D5952,$32
Map_1D5984:	; sprite piece list: 5 piece(s) of 12 bytes (frame table entry $1DF0C8); never displayed in the corpus
	incrom	$1D5984,$3E
Map_1D59C2:	; sprite piece list: 4 piece(s) of 12 bytes (frame table entry $1DF0D2); never displayed in the corpus
	incrom	$1D59C2,$32
Map_1D59F4:	; sprite piece list: 4 piece(s) of 12 bytes (frame table entry $1DF0DC); never displayed in the corpus
	incrom	$1D59F4,$32
Map_1D5A26:	; sprite piece list: 5 piece(s) of 12 bytes (frame table entry $1DF0E6); never displayed in the corpus
	incrom	$1D5A26,$3E
Map_1D5A64:	; sprite piece list: 3 piece(s) of 12 bytes (frame table entry $1DF0F0); never displayed in the corpus
	incrom	$1D5A64,$26
Map_1D5A8A:	; sprite piece list: 4 piece(s) of 12 bytes (frame table entry $1DF0FA); never displayed in the corpus
	incrom	$1D5A8A,$32
Map_1D5ABC:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF104); never displayed in the corpus
	incrom	$1D5ABC,$E
Map_1D5ACA:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF10E); never displayed in the corpus
	incrom	$1D5ACA,$E
Map_1D5AD8:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF118); never displayed in the corpus
	incrom	$1D5AD8,$E
Map_1D5AE6:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF122); never displayed in the corpus
	incrom	$1D5AE6,$E
Map_1D5AF4:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF12C); never displayed in the corpus
	incrom	$1D5AF4,$E
Map_1D5B02:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF136); never displayed in the corpus
	incrom	$1D5B02,$E
Map_1D5B10:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF140); never displayed in the corpus
	incrom	$1D5B10,$1A
Map_1D5B2A:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF14A); never displayed in the corpus
	incrom	$1D5B2A,$E
Map_1D5B38:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF154); never displayed in the corpus
	incrom	$1D5B38,$E
Map_1D5B46:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF15E); never displayed in the corpus
	incrom	$1D5B46,$E
Map_1D5B54:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF168); never displayed in the corpus
	incrom	$1D5B54,$E
Map_1D5B62:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF172); never displayed in the corpus
	incrom	$1D5B62,$1A
Map_1D5B7C:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF17C); never displayed in the corpus
	incrom	$1D5B7C,$E
Map_1D5B8A:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF186); never displayed in the corpus
	incrom	$1D5B8A,$E
Map_1D5B98:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF190); never displayed in the corpus
	incrom	$1D5B98,$1A
Map_1D5BB2:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF19A); never displayed in the corpus
	incrom	$1D5BB2,$E
Map_1D5BC0:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF1A4); never displayed in the corpus
	incrom	$1D5BC0,$E
Map_1D5BCE:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF1AE); never displayed in the corpus
	incrom	$1D5BCE,$E
Map_1D5BDC:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF1B8); never displayed in the corpus
	incrom	$1D5BDC,$E
Map_1D5BEA:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF1C2); never displayed in the corpus
	incrom	$1D5BEA,$1A
Map_1D5C04:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF1CC); never displayed in the corpus
	incrom	$1D5C04,$1A
Map_1D5C1E:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF1D6); never displayed in the corpus
	incrom	$1D5C1E,$1A
Map_1D5C38:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF1E0); never displayed in the corpus
	incrom	$1D5C38,$E
Map_1D5C46:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF1EA); never displayed in the corpus
	incrom	$1D5C46,$E
Map_1D5C54:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF1F4); never displayed in the corpus
	incrom	$1D5C54,$E
Map_1D5C62:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF1FE); never displayed in the corpus
	incrom	$1D5C62,$E
Map_1D5C70:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF208); never displayed in the corpus
	incrom	$1D5C70,$E
Map_1D5C7E:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF212); never displayed in the corpus
	incrom	$1D5C7E,$E
Map_1D5C8C:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF21C); never displayed in the corpus
	incrom	$1D5C8C,$E
Map_1D5C9A:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF226); never displayed in the corpus
	incrom	$1D5C9A,$E
Map_1D5CA8:	; sprite piece list: 4 piece(s) of 12 bytes (frame table entry $1DF230); never displayed in the corpus
	incrom	$1D5CA8,$32
Map_1D5CDA:	; sprite piece list: 3 piece(s) of 12 bytes (frame table entry $1DF23A); never displayed in the corpus
	incrom	$1D5CDA,$26
Map_1D5D00:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF244); never displayed in the corpus
	incrom	$1D5D00,$E
Map_1D5D0E:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF24E); never displayed in the corpus
	incrom	$1D5D0E,$E
Map_1D5D1C:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF258); never displayed in the corpus
	incrom	$1D5D1C,$E
Map_1D5D2A:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF262); never displayed in the corpus
	incrom	$1D5D2A,$E
Map_1D5D38:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF26C); never displayed in the corpus
	incrom	$1D5D38,$E
Map_1D5D46:	; sprite piece list: 3 piece(s) of 12 bytes (frame table entry $1DF276); never displayed in the corpus
	incrom	$1D5D46,$26
Map_1D5D6C:	; sprite piece list: 4 piece(s) of 12 bytes (frame table entry $1DF280); never displayed in the corpus
	incrom	$1D5D6C,$32
Map_1D5D9E:	; sprite piece list: 5 piece(s) of 12 bytes (frame table entry $1DF28A); never displayed in the corpus
	incrom	$1D5D9E,$3E
Map_1D5DDC:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF294); never displayed in the corpus
	incrom	$1D5DDC,$1A
Map_1D5DF6:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF29E); never displayed in the corpus
	incrom	$1D5DF6,$1A
Map_1D5E10:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF2A8); never displayed in the corpus
	incrom	$1D5E10,$1A
Map_1D5E2A:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF2B2); never displayed in the corpus
	incrom	$1D5E2A,$1A
Map_1D5E44:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF2BC); never displayed in the corpus
	incrom	$1D5E44,$1A
Map_1D5E5E:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF2C6); never displayed in the corpus
	incrom	$1D5E5E,$E
Map_1D5E6C:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF2D0); never displayed in the corpus
	incrom	$1D5E6C,$E
Map_1D5E7A:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1DF2DA); never displayed in the corpus
	incrom	$1D5E7A,$E
Map_1D5E88:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF2E4); never displayed in the corpus
	incrom	$1D5E88,$1A
Map_1D5EA2:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF2EE); never displayed in the corpus
	incrom	$1D5EA2,$1A
Map_1D5EBC:	; sprite piece list: 3 piece(s) of 12 bytes (frame table entry $1DF2F8); never displayed in the corpus
	incrom	$1D5EBC,$26
Map_1D5EE2:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF302); never displayed in the corpus
	incrom	$1D5EE2,$1A
Map_1D5EFC:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF30C); never displayed in the corpus
	incrom	$1D5EFC,$1A
Map_1D5F16:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF316); never displayed in the corpus
	incrom	$1D5F16,$1A
Map_1D5F30:	; sprite piece list: 3 piece(s) of 12 bytes (frame table entry $1DF320); never displayed in the corpus
	incrom	$1D5F30,$26
Map_1D5F56:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF32A); never displayed in the corpus
	incrom	$1D5F56,$1A
Map_1D5F70:	; sprite piece list: 3 piece(s) of 12 bytes (frame table entry $1DF334); never displayed in the corpus
	incrom	$1D5F70,$26
Map_1D5F96:	; sprite piece list: 3 piece(s) of 12 bytes (frame table entry $1DF33E); never displayed in the corpus
	incrom	$1D5F96,$26
Map_1D5FBC:	; sprite piece list: 3 piece(s) of 12 bytes (frame table entry $1DF348); never displayed in the corpus
	incrom	$1D5FBC,$26
Map_1D5FE2:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF352); never displayed in the corpus
	incrom	$1D5FE2,$1A
Map_1D5FFC:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF35C); never displayed in the corpus
	incrom	$1D5FFC,$1A
Map_1D6016:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF366); never displayed in the corpus
	incrom	$1D6016,$1A
Map_1D6030:	; sprite piece list: 3 piece(s) of 12 bytes (frame table entry $1DF370); never displayed in the corpus
	incrom	$1D6030,$26
Map_1D6056:	; sprite piece list: 3 piece(s) of 12 bytes (frame table entry $1DF37A); never displayed in the corpus
	incrom	$1D6056,$26
Map_1D607C:	; sprite piece list: 3 piece(s) of 12 bytes (frame table entry $1DF384); never displayed in the corpus
	incrom	$1D607C,$26
Map_1D60A2:	; sprite piece list: 3 piece(s) of 12 bytes (frame table entry $1DF38E); never displayed in the corpus
	incrom	$1D60A2,$26
Map_1D60C8:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF398); never displayed in the corpus
	incrom	$1D60C8,$1A
Map_1D60E2:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF3A2); never displayed in the corpus
	incrom	$1D60E2,$1A
Map_1D60FC:	; sprite piece list: 3 piece(s) of 12 bytes (frame table entry $1DF3AC); never displayed in the corpus
	incrom	$1D60FC,$26
Map_1D6122:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF3B6); never displayed in the corpus
	incrom	$1D6122,$1A
Map_1D613C:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF3C0); never displayed in the corpus
	incrom	$1D613C,$1A
Map_1D6156:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF3CA); never displayed in the corpus
	incrom	$1D6156,$1A
Map_1D6170:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF3D4); never displayed in the corpus
	incrom	$1D6170,$1A
Map_1D618A:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF3DE); never displayed in the corpus
	incrom	$1D618A,$1A
Map_1D61A4:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF3E8); never displayed in the corpus
	incrom	$1D61A4,$1A
Map_1D61BE:	; sprite piece list: 3 piece(s) of 12 bytes (frame table entry $1DF3F2); never displayed in the corpus
	incrom	$1D61BE,$26
Map_1D61E4:	; sprite piece list: 4 piece(s) of 12 bytes (frame table entry $1DF3FC); never displayed in the corpus
	incrom	$1D61E4,$32
Map_1D6216:	; sprite piece list: 5 piece(s) of 12 bytes (frame table entry $1DF406); never displayed in the corpus
	incrom	$1D6216,$3E
Map_1D6254:	; sprite piece list: 5 piece(s) of 12 bytes (frame table entry $1DF410); never displayed in the corpus
	incrom	$1D6254,$3E
Map_1D6292:	; sprite piece list: 4 piece(s) of 12 bytes (frame table entry $1DF41A); never displayed in the corpus
	incrom	$1D6292,$32
Map_1D62C4:	; sprite piece list: 3 piece(s) of 12 bytes (frame table entry $1DF424); never displayed in the corpus
	incrom	$1D62C4,$26
Map_1D62EA:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF42E); never displayed in the corpus
	incrom	$1D62EA,$1A
Map_1D6304:	; sprite piece list: 2 piece(s) of 12 bytes (frame table entry $1DF438); never displayed in the corpus
	incrom	$1D6304,$1A
Tbl_1D631E:	; read by Spr_BuildSAT (pc $007BC8), Spr_EmitClipped (pc $007D86); 87% read in the corpus
	incrom	$1D631E,$38CC
Map_1D9BEA:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E0F5E); never displayed in the corpus
	incrom	$1D9BEA,$E
Map_1D9BF8:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E0F68); never displayed in the corpus
	incrom	$1D9BF8,$E
Map_1D9C06:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E0F72); never displayed in the corpus
	incrom	$1D9C06,$E
Map_1D9C14:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E0F7C); never displayed in the corpus
	incrom	$1D9C14,$E
Map_1D9C22:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E0F86); never displayed in the corpus
	incrom	$1D9C22,$E
Map_1D9C30:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E0F90); never displayed in the corpus
	incrom	$1D9C30,$E
Map_1D9C3E:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E0F9A); never displayed in the corpus
	incrom	$1D9C3E,$E
Map_1D9C4C:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E0FA4); never displayed in the corpus
	incrom	$1D9C4C,$E
Map_1D9C5A:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E0FAE); never displayed in the corpus
	incrom	$1D9C5A,$E
Map_1D9C68:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E0FB8); never displayed in the corpus
	incrom	$1D9C68,$E
Map_1D9C76:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E0FC2); never displayed in the corpus
	incrom	$1D9C76,$E
Map_1D9C84:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E0FCC); never displayed in the corpus
	incrom	$1D9C84,$E
Map_1D9C92:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E0FD6); never displayed in the corpus
	incrom	$1D9C92,$E
Map_1D9CA0:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E0FE0); never displayed in the corpus
	incrom	$1D9CA0,$E
Map_1D9CAE:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E0FEA); never displayed in the corpus
	incrom	$1D9CAE,$E
Map_1D9CBC:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E0FF4); never displayed in the corpus
	incrom	$1D9CBC,$E
Map_1D9CCA:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E0FFE); never displayed in the corpus
	incrom	$1D9CCA,$E
Map_1D9CD8:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E1008); never displayed in the corpus
	incrom	$1D9CD8,$E
Map_1D9CE6:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E1012); never displayed in the corpus
	incrom	$1D9CE6,$E
Map_1D9CF4:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E101C); never displayed in the corpus
	incrom	$1D9CF4,$E
Map_1D9D02:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E1026); never displayed in the corpus
	incrom	$1D9D02,$E
Map_1D9D10:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E1030); never displayed in the corpus
	incrom	$1D9D10,$E
Map_1D9D1E:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E103A); never displayed in the corpus
	incrom	$1D9D1E,$E
Map_1D9D2C:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E1044); never displayed in the corpus
	incrom	$1D9D2C,$E
Map_1D9D3A:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E104E); never displayed in the corpus
	incrom	$1D9D3A,$E
Map_1D9D48:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E1058); never displayed in the corpus
	incrom	$1D9D48,$E
Map_1D9D56:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E1062); never displayed in the corpus
	incrom	$1D9D56,$E
Map_1D9D64:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E106C); never displayed in the corpus
	incrom	$1D9D64,$E
Map_1D9D72:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E1076); never displayed in the corpus
	incrom	$1D9D72,$E
Map_1D9D80:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E1080); never displayed in the corpus
	incrom	$1D9D80,$E
Map_1D9D8E:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E108A); never displayed in the corpus
	incrom	$1D9D8E,$E
Map_1D9D9C:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E1094); never displayed in the corpus
	incrom	$1D9D9C,$E
Map_1D9DAA:	; sprite piece list: 14 piece(s) of 12 bytes (frame table entry $1E109E); never displayed in the corpus
	incrom	$1D9DAA,$AA
Map_1D9E54:	; sprite piece list: 1 piece(s) of 12 bytes (frame table entry $1E10A8); never displayed in the corpus
	incrom	$1D9E54,$E
Tbl_1D9E62:	; read by Spr_BuildSAT (pc $007BC8), Op4_DrawFrame (pc $006896); 96% read in the corpus
	incrom	$1D9E62,$3CFE
Tbl_1DDB60:	; read by Op4_DrawFrame (pc $006896), Spr_ChangeFrame (pc $007F0E), Spr_ChangeFrame (pc $007F56); referenced by 006776, 007EF6, 008000
	incrom	$1DDB60,$2
	incrom	$1DDB62,$10C
Tbl_1DDC6E:	; read by Spr_SwapFrameArt (pc $007F66), Spr_ChangeFrame (pc $007F0C), Spr_BuildSAT (pc $007BC6); 96% read in the corpus
	incrom	$1DDC6E,$CEE
	incrom	$1DE95C,$208
Tbl_1DEB64:	; sprite frame table entries (10 bytes each, as Tbl_1DEBE6 after them: the art blocks Art_13411C-Art_1361B8); $1DEB64-$1DEB77 read by Spr_BuildSAT ($007BC6), Spr_SwapFrameArt ($007F66), Spr_ChangeFrame ($007F0C) in scenario-two-player-exit
	incrom	$1DEB64,$82
Tbl_1DEBE6:	; read by Spr_BuildSAT (pc $007BC6), Spr_SwapFrameArt (pc $007F66), Spr_ChangeFrame (pc $007F0C); 90% read in the corpus
	incrom	$1DEBE6,$D2
	incrom	$1DECB8,$118
Tbl_1DEDD0:	; read by Spr_BuildSAT (pc $007BC6), Spr_SwapFrameArt (pc $007F66), Spr_ChangeFrame (pc $007F0C); 66% read in the corpus
	incrom	$1DEDD0,$230
	incrom	$1DF000,$3CA
Tbl_1DF3CA:	; sprite frame table entries (10 bytes each, as Tbl_1DF442 after them); read by Spr_BuildSAT ($007BC6), Spr_SwapFrameArt ($007F66) and the a4 frame draw at $006898 in scenario-two-player-exit
	incrom	$1DF3CA,$78
Tbl_1DF442:	; read by Spr_BuildSAT (pc $007BC6), Op4_DrawFrame (pc $006898), Spr_BuildSAT (pc $007BBE); 71% read in the corpus
	incrom	$1DF442,$1B18
	incrom	$1E0F5A,$158
Tbl_1E10B2:	; read by Spr_BuildSAT (pc $007BC6), Spr_ChangeFrame (pc $007F0C), Spr_SwapFrameArt (pc $007F66); 91% read in the corpus
	incrom	$1E10B2,$173E
Fill_1E27F0:	; $00/$FF fill between the tables and the sound code (5,129 $00, 5,111 $FF bytes)
	incrom	$1E27F0,$281E
	include "src/f/1E500E.asm"
	incrom	$1E5022,$22
	include "src/f/1E5044.asm"
	incrom	$1E5058,$E
	include "src/f/1E5066.asm"
	include "src/f/1E5074.asm"
	include "src/f/1E5082.asm"
	incrom	$1E5090,$46
Tbl_1E50D6:	; read by Enemy_BombBot_SpawnHall (pc $1E5FA0); 49% read in the corpus; referenced by 1E5F88
	incrom	$1E50D6,$2D0
Snd_1E53A6:	; sound table: continuation of Tbl_1E50D6's 8-byte entries (word, word, long into $1E5xxx) read by Snd_PlaySfx; this part never read in the corpus (inferred same table)
	incrom	$1E53A6,$BE2
	include "src/f/1E5F88.asm"
	incrom	$1E5FBA,$40
	include "src/f/1E5FFA.asm"
	include "src/f/1E6012.asm"
Tbl_1E6028:	; addressed as data by code (operand in 1E6012); never read in the corpus
	incrom	$1E6028,$2
Tbl_1E602A:	; addressed as data by code (operand in 1E6012); never read in the corpus
	incrom	$1E602A,$DA
Snd_1E6104:	; Zyrinx Z80 driver program, copied to $A00000 and verified by $1E7CAC (6,181 bytes); referenced by 1E7BBE
	incrom	$1E6104,$1825
	incrom	$1E7929,$71
	include "src/f/1E799A.asm"
	include "src/f/1E79A4.asm"
	incrom	$1E79D4,$124
	include "src/f/1E7AF8.asm"
	incrom	$1E7B6C,$4
	include "src/f/1E7B70.asm"
	include "src/f/1E7BBE.asm"
	incrom	$1E7D08,$4A
	include "src/f/1E7D52.asm"
	incrom	$1E7D8C,$158
Tbl_1E7EE4:	; read by Enemy_BombBot_SpawnHall (pc $1E7B90), Enemy_BombBot_SpawnHall (pc $1E7C78); referenced by 1E6012
	incrom	$1E7EE4,$18
	incrom	$1E7EFC,$104
Snd_1E8000:	; Zyrinx music module bank 1 (32 KB, read by the Z80 through the bank window; module table $1E7EE4)
	incrom	$1E8000,$8000
Snd_1F0000:	; Zyrinx music module bank 2 (32 KB, read by the Z80 through the bank window)
	incrom	$1F0000,$8000
Snd_1F8000:	; Zyrinx music module bank 3 (32 KB, read by the Z80 through the bank window)
	incrom	$1F8000,$8000
