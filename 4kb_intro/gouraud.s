����   �   �   �   �   �   �   �   �   �   �;-------------------------------------------------------------------------
; ROUTINE: GOURAUD OBJECT
; CODE:    MORPH/FREEZERS
;
;
;-------------------------------------------------------------------------

	include	"Morph:Asm/Includes/CUSTOM.i"
	include	"Morph:Asm/Includes/MAIN.m"

;*************************************************************************

	SECTION	MAIN,CODE_P

	lea     CUSTOM,a6

	WaitRaster	#$137
	move.l	#$7fff7fff,(INTENA,a6)
	move.w	#$7fff,(DMACON,a6)

	bsr	make_copper
	bsr	make_color

        move.w	#StartOkno,(DIWSTRT,a6)
        move.w	#EndOkno,(DIWSTOP,a6)
        move.w	#StartDane,(DDFSTRT,a6)
        move.w	#EndDane,(DDFSTOP,a6)
        move.w	#$210,(BPLCON0,a6)
        move.w	#0,(BPLCON1,a6)
        move.w	#0,(BPLCON2,a6)
        move.w	#0,(BPLCON4,a6)
        move.w	#Bytes-40-8,(BPL1MOD,a6)
        move.w	#Bytes-40-8,(BPL2MOD,a6)
        move.w	#%0011,(FMODE,a6)

	move.w	#0,(BPLCON3,a6)
	move.w	#$000,(COLOR00,a6)
	move.w	#$200,(BPLCON3,a6)
	move.w	#$000,(COLOR00,a6)

	move.l	#cop1+[360*128],(COP1LCH,a6)
	move.w	#0,(COPJMP1,a6)

	WaitRaster	#$137
	move.w	#%1000001111000000,(DMACON,a6)

;*************************************************************************

main:	WaitRaster	#$137

	bsr	switch_cop
	bsr	rotate
	bsr	calc_light
	bsr	sort

	move.l	proc,paddr
	move.l	proc+4,paddr+4
	move.l	proc+8,paddr+8
	bsr	show

	move.l	proc+12,paddr
	move.l	proc+16,paddr+4
	move.l	proc+20,paddr+8
	bsr	show

	move.l	proc+24,paddr
	move.l	proc+28,paddr+4
	move.l	proc+32,paddr+8
	bsr	show

	move.l	proc+36,paddr
	move.l	proc+40,paddr+4
	move.l	proc+44,paddr+8
	bsr	show

	bsr	help

	lea	CUSTOM,a6

	btst	#6,CIAA+PRA
	bne.w	main
	rts	

;*************************************************************************

make_copper:
        lea     cop1+[360*128],a0  
	lea	back,a1
	lea	htab,a2
	move.w	#BPL1PTH,d0
	move.w	#BPL1PTL,d1
	move.l	#chunky,d2
	moveq	#Depth-1,d3

make1:	move.w	d0,(a0)+
	move.w	d0,(a2)+
	swap	d2
	move.w	d2,(a0)+
	move.w	d2,(a2)+
	move.w	d1,(a0)+
	move.w	d1,(a2)+
	swap	d2
	move.w	d2,(a0)+
	move.w	d2,(a2)+
	add.l	#scrSize,d2	
	addq.w	#4,d0
	addq.w	#4,d1
	dbf	d3,make1

	move.l	#$2741fffe,d0
	move.w	#$100,d2
	moveq	#[88/2]-1,d1

make2:	move.l	d0,(a0)+
	move.l	#$1060000,(a0)+
	move.b	#$82,d2
	moveq	#31-1,d3
make3:	move.w	d2,(a0)+
	move.w	(a1)+,(a0)+
	addq.b	#2,d2
	dbf	d3,make3

	move.l	#$1062000,(a0)+
	move.b	#$80,d2
	moveq	#32-1,d3
make4:	move.w	d2,(a0)+
	move.w	(a1)+,(a0)+
	addq.b	#2,d2
	dbf	d3,make4

	move.l	#$1064000,(a0)+
	move.b	#$80,d2
	moveq	#22-1,d3
make5:	move.w	d2,(a0)+
	move.w	(a1)+,(a0)+
	addq.b	#2,d2
	dbf	d3,make5

	move.l	#$106c000,(a0)+

	add.l	#$3000000,d0

	move.l	d0,(a0)+

	move.l	#$1066000,(a0)+
	move.b	#$82,d2
	moveq	#31-1,d3
make6:	move.w	d2,(a0)+
	move.w	(a1)+,(a0)+
	addq.b	#2,d2
	dbf	d3,make6

	move.l	#$1068000,(a0)+
	move.b	#$80,d2
	moveq	#32-1,d3
make7:	move.w	d2,(a0)+
	move.w	(a1)+,(a0)+
	addq.b	#2,d2
	dbf	d3,make7

	move.l	#$106a000,(a0)+
	move.b	#$80,d2
	moveq	#22-1,d3
make8:	move.w	d2,(a0)+
	move.w	(a1)+,(a0)+
	addq.b	#2,d2
	dbf	d3,make8

	move.l	#$106c000,(a0)+

	add.l	#$3000000,d0

	dbf	d1,make2

	move.l	#-2,(a0)

        lea     cop1+[360*128],a0  
        lea     cop2+[360*64],a1
        lea	cop3+[360*64],a2
	move.w	#[36+[180*88]]-1,d0
copy1:	move.w	(a0),(a1)+
	move.w	(a0)+,(a2)+
	dbf	d0,copy1

        lea     cop1+[360*128]+64,a0  
        lea     cop4,a1  
	move.w	#[180*88]-1,d0
copy2:	move.w	(a0)+,(a1)+
	dbf	d0,copy2
	rts
	
;*************************************************************************

make_color:
	lea	col,a0
	lea	color,a1
	moveq	#[26*3]-1,d7

co2:	move.w	#256-1,d6

co3:	move.w	(a0),(a1)+
	dbf	d6,co3

	tst.w	(a0)+
	dbf	d7,co2
	rts

;*************************************************************************
caddr: 	dc.l	cop1+[360*128],cop2+[360*64],cop3+[360*64]

switch_cop:
	movem.l	caddr(pc),a0-a3
	movem.l	a1/a2,caddr
	move.l	a0,caddr+8
	move.l	a1,(COP1LCH,a6)

	lea	(64,a0),a0

	WaitBlitter
	move.l	#$9f00000,(BLTCON0,a6)
	move.l	#-1,(BLTAFWM,a6)
	move.l	#0,(BLTAMOD,a6)
	move.l	#cop4,(BLTAPTH,a6)
	move.l	a0,(BLTDPTH,a6)
	move.w	#[360<<6]!44,(BLTSIZE,a6)
	rts
		
;*************************************************************************
axis:	dc.w	0,0,0

rotate:	lea	axis(pc),a0
	add.w	#-10,(a0)
	and.w	#$3ff,(a0)+
	add.w	#6,(a0)
	and.w	#$3ff,(a0)+
	add.w	#2,(a0)
	and.w	#$3ff,(a0)
	movem.w	(-4,a0),d0-d2
	lea     sinus,a6
	lea     (a6,d0.w*2),a0
	lea     (a6,d1.w*2),a1
	lea     (a6,d2.w*2),a2
	lea     (512,a6),a6
	lea     (a6,d0.w*2),a3
	move.w	(a6,d1.w*2),d7
	move.w  (a6,d2.w*2),d6
	lea	zet,a4
	lea     rcalc,a5
	lea   	points,a6
rotate_loop:
	movem.w (a6),d0-d2
	movem.w (a6)+,d3-d5
	muls    (a3),d1		
	muls    (a0),d2		
	sub.l   d2,d1
	swap    d1
	asl.l	d1
	muls    (a0),d4
	muls    (a3),d5
	add.l   d4,d5
	swap    d5
	asl.l	d5
	move.w  d0,d2
	move.w  d5,d3
	muls    d7,d0
	muls    (a1),d5
	sub.l   d5,d0
	swap    d0
	asl.l	d0
	muls    (a1),d2
	muls    d7,d3
	add.l   d2,d3
	swap    d3
	asl.l	d3
	move.w  d0,d4
	move.w  d1,d5
	muls    d6,d0
	muls    (a2),d1
	sub.l   d1,d0
	swap    d0
	asl.l	d0
	muls    (a2),d4
	muls    d6,d5
	add.l   d4,d5
	swap    d5
	asl.l	d5
	move.w	d3,(a4)+
	muls	#268,d0
	muls	#268,d5
	add.w	#9800,d3
	divs	d3,d0
	divs	d3,d5
	add.w	#43,d0
	add.w	#45,d5
	move.w  d0,(a5)+
	move.w  d5,(a5)+
	cmp.w	#-1,(a6)
	bne.s	rotate_loop
	rts

;*************************************************************************

calc_light:
	lea	zet,a0
	lea	light,a1
	moveq	#34-1,d0
light1:	
	move.w	(a0)+,d1
	muls	#15,d1
	divs	#2348,d1
	bmi.s	ok
	moveq	#0,d1
ok:	neg.w	d1
	lsl.w	#8,d1
	move.w	d1,(a1)+

	dbf	d0,light1
	rts

;*************************************************************************

sort:	lea	zet,a0

	move.w	(0*2,a0),d0
	add.w	(6*2,a0),d0
	asr.w	d0

	move.w	(8*2,a0),d1
	add.w	(14*2,a0),d1
	asr.w	d1

	move.w	(16*2,a0),d2
	add.w	(22*2,a0),d2
	asr.w	d2

	move.w	(26*2,a0),d3
	add.w	(32*2,a0),d3
	asr.w	d3

	moveq	#0,d4
	moveq	#1,d5
	moveq	#2,d6
	moveq	#3,d7

	cmp.w	d0,d1
	ble.s	so1
	exg	d0,d1
	exg	d4,d5

so1:	cmp.w	d0,d2
	ble.s	so2
	exg	d0,d2
	exg	d4,d6

so2:	cmp.w	d0,d3
	ble.s	so3
	exg	d0,d3
	exg	d4,d7

so3:	cmp.w	d1,d2
	ble.s	so4
	exg	d1,d2
	exg	d5,d6

so4:	cmp.w	d1,d3
	ble.s	so5
	exg	d1,d3
	exg	d5,d7

so5:	cmp.w	d2,d3
	ble.s	so6
	exg	d2,d3
	exg	d6,d7

so6:	lea	proctab,a0
	lea	proc,a1

	mulu	#12,d4
	mulu	#12,d5
	mulu	#12,d6
	mulu	#12,d7

	move.l	(a0,d4.w),(a1)+
	move.l	(4,a0,d4.w),(a1)+
	move.l	(8,a0,d4.w),(a1)+

	move.l	(a0,d5.w),(a1)+
	move.l	(4,a0,d5.w),(a1)+
	move.l	(8,a0,d5.w),(a1)+

	move.l	(a0,d6.w),(a1)+
	move.l	(4,a0,d6.w),(a1)+
	move.l	(8,a0,d6.w),(a1)+

	move.l	(a0,d7.w),(a1)+
	move.l	(4,a0,d7.w),(a1)+
	move.l	(8,a0,d7.w),(a1)
	rts

;*************************************************************************
paddr:	dc.l	0,0,0
raddr:	dc.l	0

show:	move.l	paddr+8(pc),a4
	move.l	paddr(pc),a5
	lea	rcalc,a6
	move.l	paddr+4(pc),d7
show_loop:
	movem.w	(2,a5),d0-d2
	
	movem.w	(a6,d2.w*4),d4/d5
	movem.w	(a6,d1.w*4),d2/d3
	movem.w	(a6,d0.w*4),d0/d1
	sub.w	d2,d4
	sub.w	d3,d5
	sub.w	d0,d2
	sub.w	d1,d3
	muls	d2,d5
	muls	d3,d4
	sub.l	d5,d4
	bmi.s	skip

	move.l	(a4),raddr
	movem.l	d7/a4-a6,-(a7)
	bsr	doface
	movem.l	(a7)+,d7/a4-a6

skip:	tst.l	(a4)+
	move.w	(a5)+,d0
	lea	(2,a5,d0.w*2),a5
	dbf	d7,show_loop
	rts

;*************************************************************************
mem:	dc.w	0,0

doface:	lea	light,a4
	move.l	#$ff000100,mem

	moveq	#0,d6
	move.w	(a5)+,d7
	subq.w	#1,d7

loop1:	move.l	(a5),d5
	movem.w	(a6,d5.w*4),d0/d1
	movem.w	(a4,d5.w*2),d4
	swap	d5
	movem.w	(a6,d5.w*4),d2/d3
	movem.w	(a4,d5.w*2),d5

	cmp.w	mem+2(pc),d0
	bge.s	no1
	move.w	d0,mem+2

no1:	cmp.w	mem(pc),d0
	ble.s	no2
	move.w	d0,mem

no2:	lea	int2+[256*2],a2

	cmp.w	d0,d2
	beq.s	out
	bgt.s	no3

	exg	d0,d2
	exg	d1,d3
	exg	d4,d5

	lea	int1+[256*2],a2

no3:	lea	(a2,d0.w*4),a1

	sub.l	d0,d2
	sub.w	d1,d3
	sub.l	d4,d5

	swap	d3
	divs.l	d2,d3
	divs.l	d2,d5
	swap	d3

	subq.w	#1,d2

loop2:	move.w	d1,(a1)+
	move.w	d4,(a1)+

	add.l	d3,d1
	addx.w	d6,d1
	add.w	d5,d4
	dbf	d2,loop2

out:	tst.w	(a5)+
	dbf	d7,loop1

	movem.w	mem(pc),d0/d1
	sub.w	d1,d0
	ble.w	exit
	subq.w	#1,d0

	move.l	caddr+4(pc),a0
	lea	(64,a0),a0

	lea	offs+[64*2],a1
	lea	(a1,d1.w*2),a1

	lea	int2+[256*2],a2
	lea	(a2,d1.w*4),a2

	lea	int1+[256*2],a3
	lea	(a3,d1.w*4),a3

	move.l	raddr(pc),a4
	moveq	#0,d7

loop3:	movem.w	(a2)+,d1/d3
	movem.w	(a3)+,d2/d4

	sub.l	d1,d2
	ble.s	not

	muls	#360,d1
	lea	(a0,d1.w),a5
	add.w	(a1),a5

	sub.l	d3,d4

	divs.l	d2,d4

	subq.w	#1,d2
	
loop4:	move.w	(a4,d3.w*2),(a5)

	add.w	d4,d3

	lea	(360,a5),a5
	dbf	d2,loop4

not:	tst.w	(a1)+
	dbf	d0,loop3

exit:	rts

;*************************************************************************

help:	lea	htab,a0
	move.l	caddr+4(pc),a1

	rept	16
	move.l	(a0)+,(a1)+
	endr
	rts	

;*************************************************************************
StartOkno  	= $29a1
EndOkno    	= $31a1
StartDane  	= $38
EndDane    	= $d0
Bytes	   	= 40
scrSize		= Bytes*264
Depth	   	= 8
;*************************************************************************

	SECTION	SCREEN,DATA_C

chunky:	incbin	"Morph:Trashcan/chunky2.320x264x256.raw"

	SECTION	OTHER,DATA_P

back:	incbin	"Morph:Trashcan/trash/back_brick.chunky"

sinus:	dc.w	$0000,$00c8,$0191,$025a,$0322,$03eb,$04b3,$057c
	dc.w	$0644,$070d,$07d5,$089d,$0966,$0a2e,$0af6,$0bbe
	dc.w	$0c85,$0d4d,$0e15,$0edc,$0fa3,$106a,$1131,$11f8
	dc.w	$12bf,$1385,$144b,$1512,$15d7,$169d,$1762,$1828
	dc.w	$18ed,$19b1,$1a76,$1b3a,$1bfe,$1cc2,$1d85,$1e48
	dc.w	$1f0b,$1fce,$2090,$2152,$2214,$22d5,$2396,$2456
	dc.w	$2517,$25d7,$2696,$2755,$2814,$28d2,$2990,$2a4e
	dc.w	$2b0b,$2bc8,$2c84,$2d40,$2dfc,$2eb7,$2f72,$302c
	dc.w	$30e5,$319f,$3257,$3310,$33c7,$347f,$3536,$35ec
	dc.w	$36a2,$3757,$380c,$38c0,$3973,$3a26,$3ad9,$3b8b
	dc.w	$3c3c,$3ced,$3d9d,$3e4d,$3efc,$3faa,$4058,$4105
	dc.w	$41b2,$425e,$4309,$43b4,$445e,$4507,$45b0,$4658
	dc.w	$46ff,$47a6,$484c,$48f1,$4995,$4a39,$4adc,$4b7f
	dc.w	$4c20,$4cc1,$4d62,$4e01,$4ea0,$4f3e,$4fdb,$5078
	dc.w	$5113,$51ae,$5248,$52e2,$537a,$5412,$54a9,$553f
	dc.w	$55d4,$5669,$56fc,$578f,$5821,$58b2,$5942,$59d2
	dc.w	$5a60,$5aee,$5b7b,$5c07,$5c92,$5d1c,$5da5,$5e2e
	dc.w	$5eb5,$5f3c,$5fc1,$6046,$60ca,$614d,$61cf,$6250
	dc.w	$62d0,$634f,$63cd,$644a,$64c6,$6541,$65bc,$6635
	dc.w	$66ad,$6725,$679b,$6810,$6885,$68f8,$696a,$69dc
	dc.w	$6a4c,$6abb,$6b2a,$6b97,$6c03,$6c6e,$6cd8,$6d41
	dc.w	$6da9,$6e10,$6e76,$6edb,$6f3f,$6fa2,$7003,$7064
	dc.w	$70c3,$7122,$717f,$71db,$7236,$7290,$72e9,$7341
	dc.w	$7398,$73ee,$7442,$7495,$74e8,$7539,$7589,$75d8
	dc.w	$7626,$7672,$76be,$7708,$7751,$7799,$77e0,$7826
	dc.w	$786b,$78ae,$78f0,$7931,$7971,$79b0,$79ee,$7a2a
	dc.w	$7a66,$7aa0,$7ad9,$7b10,$7b47,$7b7c,$7bb1,$7be4
	dc.w	$7c15,$7c46,$7c75,$7ca4,$7cd1,$7cfd,$7d27,$7d51
	dc.w	$7d79,$7da0,$7dc6,$7dea,$7e0e,$7e30,$7e51,$7e71
	dc.w	$7e8f,$7ead,$7ec9,$7ee4,$7efd,$7f16,$7f2d,$7f43
	dc.w	$7f58,$7f6c,$7f7e,$7f8f,$7f9f,$7fae,$7fbb,$7fc7
	dc.w	$7fd2,$7fdc,$7fe5,$7fec,$7ff2,$7ff7,$7ffb,$7ffd
	dc.w	$7ffe,$7ffe,$7ffd,$7ffb,$7ff7,$7ff2,$7fec,$7fe4
	dc.w	$7fdc,$7fd2,$7fc7,$7fba,$7fad,$7f9e,$7f8e,$7f7d
	dc.w	$7f6a,$7f57,$7f42,$7f2c,$7f14,$7efc,$7ee2,$7ec7
	dc.w	$7eab,$7e8d,$7e6e,$7e4f,$7e2e,$7e0b,$7de8,$7dc3
	dc.w	$7d9d,$7d76,$7d4e,$7d24,$7cf9,$7ccd,$7ca0,$7c72
	dc.w	$7c42,$7c12,$7be0,$7bad,$7b78,$7b43,$7b0c,$7ad4
	dc.w	$7a9b,$7a61,$7a26,$79e9,$79ac,$796d,$792d,$78eb
	dc.w	$78a9,$7865,$7821,$77db,$7794,$774c,$7702,$76b8
	dc.w	$766c,$7620,$75d2,$7583,$7533,$74e2,$748f,$743c
	dc.w	$73e7,$7391,$733b,$72e3,$728a,$7230,$71d4,$7178
	dc.w	$711b,$70bc,$705d,$6ffc,$6f9a,$6f37,$6ed4,$6e6f
	dc.w	$6e09,$6da2,$6d39,$6cd0,$6c66,$6bfb,$6b8f,$6b21
	dc.w	$6ab3,$6a44,$69d3,$6962,$68ef,$687c,$6808,$6792
	dc.w	$671c,$66a4,$662c,$65b3,$6538,$64bd,$6441,$63c3
	dc.w	$6345,$62c6,$6246,$61c5,$6143,$60c0,$603c,$5fb7
	dc.w	$5f31,$5eab,$5e23,$5d9b,$5d12,$5c87,$5bfc,$5b70
	dc.w	$5ae3,$5a56,$59c7,$5938,$58a7,$5816,$5784,$56f1
	dc.w	$565d,$55c9,$5534,$549d,$5406,$536f,$52d6,$523d
	dc.w	$51a2,$5107,$506c,$4fcf,$4f32,$4e94,$4df5,$4d55
	dc.w	$4cb5,$4c14,$4b72,$4ad0,$4a2d,$4989,$48e4,$483f
	dc.w	$4799,$46f2,$464b,$45a3,$44fa,$4451,$43a7,$42fc
	dc.w	$4250,$41a5,$40f8,$404b,$3f9d,$3eee,$3e3f,$3d90
	dc.w	$3cdf,$3c2f,$3b7d,$3acb,$3a19,$3966,$38b2,$37fe
	dc.w	$3749,$3694,$35de,$3528,$3471,$33b9,$3302,$3249
	dc.w	$3190,$30d7,$301d,$2f63,$2ea9,$2ded,$2d32,$2c76
	dc.w	$2bb9,$2afd,$2a3f,$2982,$28c4,$2805,$2746,$2687
	dc.w	$25c8,$2508,$2447,$2387,$22c6,$2205,$2143,$2081
	dc.w	$1fbf,$1efc,$1e39,$1d76,$1cb3,$1bef,$1b2b,$1a67
	dc.w	$19a2,$18dd,$1818,$1753,$168e,$15c8,$1502,$143c
	dc.w	$1376,$12af,$11e9,$1122,$105b,$0f94,$0ecd,$0e05
	dc.w	$0d3e,$0c76,$0bae,$0ae6,$0a1e,$0956,$088e,$07c6
	dc.w	$06fd,$0635,$056c,$04a4,$03db,$0312,$024a,$0181
	dc.w	$00b8,$fff1,$ff28,$fe5f,$fd97,$fcce,$fc05,$fb3d
	dc.w	$fa74,$f9ac,$f8e3,$f81b,$f753,$f68b,$f5c3,$f4fb
	dc.w	$f433,$f36b,$f2a3,$f1dc,$f114,$f04d,$ef86,$eebf
	dc.w	$edf8,$ed32,$ec6b,$eba5,$eadf,$ea19,$e953,$e88e
	dc.w	$e7c9,$e704,$e63f,$e57b,$e4b6,$e3f2,$e32f,$e26b
	dc.w	$e1a8,$e0e5,$e023,$df61,$de9f,$dddd,$dd1c,$dc5b
	dc.w	$db9a,$dada,$da1a,$d95b,$d89c,$d7dd,$d71e,$d661
	dc.w	$d5a3,$d4e6,$d429,$d36d,$d2b1,$d1f5,$d13a,$d080
	dc.w	$cfc5,$cf0c,$ce53,$cd9a,$cce2,$cc2a,$cb73,$cabc
	dc.w	$ca06,$c950,$c89b,$c7e6,$c732,$c67e,$c5cb,$c519
	dc.w	$c467,$c3b6,$c305,$c255,$c1a5,$c0f6,$c048,$bf9a
	dc.w	$beed,$be40,$bd95,$bce9,$bc3f,$bb95,$baec,$ba43
	dc.w	$b99b,$b8f4,$b84d,$b7a7,$b702,$b65d,$b5ba,$b517
	dc.w	$b474,$b3d3,$b332,$b291,$b1f2,$b153,$b0b5,$b018
	dc.w	$af7c,$aee0,$ae45,$adab,$ad12,$ac7a,$abe2,$ab4b
	dc.w	$aab5,$aa20,$a98b,$a8f8,$a865,$a7d3,$a742,$a6b2
	dc.w	$a623,$a594,$a506,$a47a,$a3ee,$a363,$a2d9,$a250
	dc.w	$a1c7,$a140,$a0ba,$a034,$9faf,$9f2c,$9ea9,$9e27
	dc.w	$9da6,$9d26,$9ca7,$9c29,$9bac,$9b30,$9ab5,$9a3a
	dc.w	$99c1,$9949,$98d2,$985b,$97e6,$9772,$96ff,$968c
	dc.w	$961b,$95ab,$953c,$94ce,$9460,$93f4,$9389,$931f
	dc.w	$92b6,$924e,$91e7,$9182,$911d,$90b9,$9056,$8ff5
	dc.w	$8f94,$8f35,$8ed7,$8e79,$8e1d,$8dc2,$8d68,$8d0f
	dc.w	$8cb8,$8c61,$8c0b,$8bb7,$8b64,$8b12,$8ac1,$8a71
	dc.w	$8a22,$89d4,$8988,$893c,$88f2,$88a9,$8861,$881a
	dc.w	$87d4,$8790,$874c,$870a,$86c9,$8689,$864b,$860d
	dc.w	$85d1,$8596,$855c,$8523,$84eb,$84b5,$847f,$844b
	dc.w	$8418,$83e7,$83b6,$8387,$8359,$832c,$8300,$82d5
	dc.w	$82ac,$8284,$825d,$8237,$8213,$81ef,$81cd,$81ac
	dc.w	$818d,$816e,$8151,$8135,$811a,$8100,$80e8,$80d1
	dc.w	$80bb,$80a6,$8093,$8081,$806f,$8060,$8051,$8044
	dc.w	$8038,$802d,$8023,$801b,$8013,$800d,$8009,$8005
	dc.w	$8003,$8002,$8002,$8003,$8006,$800a,$800f,$8015
	dc.w	$801c,$8025,$802f,$803a,$8047,$8054,$8063,$8073
	dc.w	$8085,$8097,$80ab,$80c0,$80d6,$80ee,$8106,$8120
	dc.w	$813b,$8158,$8175,$8194,$81b4,$81d5,$81f8,$821b
	dc.w	$8240,$8266,$828d,$82b6,$82df,$830a,$8336,$8364
	dc.w	$8392,$83c2,$83f2,$8424,$8458,$848c,$84c2,$84f8
	dc.w	$8530,$8569,$85a4,$85df,$861c,$865a,$8699,$86d9
	dc.w	$871a,$875d,$87a0,$87e5,$882b,$8872,$88ba,$8904
	dc.w	$894e,$899a,$89e7,$8a35,$8a84,$8ad4,$8b25,$8b78
	dc.w	$8bcb,$8c20,$8c76,$8ccd,$8d25,$8d7e,$8dd8,$8e33
	dc.w	$8e90,$8eed,$8f4c,$8fac,$900c,$906e,$90d1,$9135
	dc.w	$919a,$9200,$9267,$92cf,$9339,$93a3,$940e,$947b
	dc.w	$94e8,$9556,$95c6,$9636,$96a8,$971a,$978e,$9802
	dc.w	$9878,$98ee,$9966,$99de,$9a58,$9ad2,$9b4e,$9bca
	dc.w	$9c47,$9cc6,$9d45,$9dc5,$9e46,$9ec8,$9f4b,$9fcf
	dc.w	$a054,$a0da,$a161,$a1e8,$a271,$a2fa,$a384,$a410
	dc.w	$a49c,$a529,$a5b6,$a645,$a6d5,$a765,$a7f6,$a888
	dc.w	$a91b,$a9af,$aa44,$aad9,$ab6f,$ac06,$ac9e,$ad37
	dc.w	$add0,$ae6b,$af06,$afa2,$b03e,$b0db,$b17a,$b218
	dc.w	$b2b8,$b358,$b3fa,$b49b,$b53e,$b5e1,$b685,$b72a
	dc.w	$b7cf,$b875,$b91c,$b9c3,$ba6c,$bb14,$bbbe,$bc68
	dc.w	$bd13,$bdbe,$be6a,$bf17,$bfc4,$c072,$c120,$c1d0
	dc.w	$c27f,$c330,$c3e0,$c492,$c544,$c5f7,$c6aa,$c75d
	dc.w	$c812,$c8c6,$c97c,$ca32,$cae8,$cb9f,$cc56,$cd0e
	dc.w	$cdc6,$ce7f,$cf39,$cff2,$d0ad,$d167,$d222,$d2de
	dc.w	$d39a,$d456,$d513,$d5d1,$d68e,$d74c,$d80b,$d8ca
	dc.w	$d989,$da49,$db09,$dbc9,$dc8a,$dd4b,$de0c,$dece
	dc.w	$df90,$e052,$e114,$e1d7,$e29b,$e35e,$e422,$e4e6
	dc.w	$e5aa,$e66f,$e733,$e7f8,$e8be,$e983,$ea49,$eb0f
	dc.w	$ebd5,$ec9b,$ed61,$ee28,$eeef,$efb6,$f07d,$f144
	dc.w	$f20c,$f2d3,$f39b,$f463,$f52b,$f5f3,$f6bb,$f783
	dc.w	$f84b,$f914,$f9dc,$faa5,$fb6d,$fc36,$fcfe,$fdc7
	dc.w	$0000,$00c8,$0191,$025a,$0322,$03eb,$04b3,$057c
	dc.w	$0644,$070d,$07d5,$089d,$0966,$0a2e,$0af6,$0bbe
	dc.w	$0c85,$0d4d,$0e15,$0edc,$0fa3,$106a,$1131,$11f8
	dc.w	$12bf,$1385,$144b,$1512,$15d7,$169d,$1762,$1828
	dc.w	$18ed,$19b1,$1a76,$1b3a,$1bfe,$1cc2,$1d85,$1e48
	dc.w	$1f0b,$1fce,$2090,$2152,$2214,$22d5,$2396,$2456
	dc.w	$2517,$25d7,$2696,$2755,$2814,$28d2,$2990,$2a4e
	dc.w	$2b0b,$2bc8,$2c84,$2d40,$2dfc,$2eb7,$2f72,$302c
	dc.w	$30e5,$319f,$3257,$3310,$33c7,$347f,$3536,$35ec
	dc.w	$36a2,$3757,$380c,$38c0,$3973,$3a26,$3ad9,$3b8b
	dc.w	$3c3c,$3ced,$3d9d,$3e4d,$3efc,$3faa,$4058,$4105
	dc.w	$41b2,$425e,$4309,$43b4,$445e,$4507,$45b0,$4658
	dc.w	$46ff,$47a6,$484c,$48f1,$4995,$4a39,$4adc,$4b7f
	dc.w	$4c20,$4cc1,$4d62,$4e01,$4ea0,$4f3e,$4fdb,$5078
	dc.w	$5113,$51ae,$5248,$52e2,$537a,$5412,$54a9,$553f
	dc.w	$55d4,$5669,$56fc,$578f,$5821,$58b2,$5942,$59d2
	dc.w	$5a60,$5aee,$5b7b,$5c07,$5c92,$5d1c,$5da5,$5e2e
	dc.w	$5eb5,$5f3c,$5fc1,$6046,$60ca,$614d,$61cf,$6250
	dc.w	$62d0,$634f,$63cd,$644a,$64c6,$6541,$65bc,$6635
	dc.w	$66ad,$6725,$679b,$6810,$6885,$68f8,$696a,$69dc
	dc.w	$6a4c,$6abb,$6b2a,$6b97,$6c03,$6c6e,$6cd8,$6d41
	dc.w	$6da9,$6e10,$6e76,$6edb,$6f3f,$6fa2,$7003,$7064
	dc.w	$70c3,$7122,$717f,$71db,$7236,$7290,$72e9,$7341
	dc.w	$7398,$73ee,$7442,$7495,$74e8,$7539,$7589,$75d8
	dc.w	$7626,$7672,$76be,$7708,$7751,$7799,$77e0,$7826
	dc.w	$786b,$78ae,$78f0,$7931,$7971,$79b0,$79ee,$7a2a
	dc.w	$7a66,$7aa0,$7ad9,$7b10,$7b47,$7b7c,$7bb1,$7be4
	dc.w	$7c15,$7c46,$7c75,$7ca4,$7cd1,$7cfd,$7d27,$7d51
	dc.w	$7d79,$7da0,$7dc6,$7dea,$7e0e,$7e30,$7e51,$7e71
	dc.w	$7e8f,$7ead,$7ec9,$7ee4,$7efd,$7f16,$7f2d,$7f43
	dc.w	$7f58,$7f6c,$7f7e,$7f8f,$7f9f,$7fae,$7fbb,$7fc7
	dc.w	$7fd2,$7fdc,$7fe5,$7fec,$7ff2,$7ff7,$7ffb,$7ffd

points:	dc.w	-2800,300,-300
	dc.w	1000,300,-300
	dc.w	1000,-300,-300
	dc.w	-2800,-300,-300
	dc.w	-2800,300,300
	dc.w	1000,300,300
	dc.w	1000,-300,300
	dc.w	-2800,-300,300

	dc.w	1000,400,-400
	dc.w	2800,400,-400
	dc.w	2800,-400,-400
	dc.w	1000,-400,-400
	dc.w	1000,400,400
	dc.w	2800,400,400
	dc.w	2800,-400,400
	dc.w	1000,-400,400

	dc.w	2800,300,-300
	dc.w	3050,300,-300
	dc.w	3050,-300,-300
	dc.w	2800,-300,-300
	dc.w	2800,300,300
	dc.w	3050,300,300
	dc.w	3050,-300,300
	dc.w	2800,-300,300

	dc.w	1300,-400,-220
	dc.w	2500,-400,-220
	dc.w	2500,-400,220
	dc.w	1300,-400,220
	dc.w	600,-1420,-180
	dc.w	3200,-1420,-180
	dc.w	3200,-1420,180
	dc.w	600,-1420,180
	dc.w	980,-2180,0
	dc.w	2820,-2180,0

	dc.w	-1

con:	dc.w	4, 0,1,2,3,0
	dc.w	4, 5,4,7,6,5
	dc.w	4, 5,1,0,4,5
	dc.w	4, 2,6,7,3,2
	dc.w	4, 4,0,3,7,4

	dc.w	4, 8,9,10,11,8
	dc.w	4, 13,12,15,14,13
	dc.w	4, 9,13,14,10,9
	dc.w	4, 12,8,11,15,12
	dc.w	4, 12,13,9,8,12
	dc.w	4, 11,10,14,15,11

	dc.w	4, 16,17,18,19,16
	dc.w	4, 21,20,23,22,21
	dc.w	4, 19,18,22,23,19
	dc.w	4, 20,21,17,16,20
	dc.w	4, 17,21,22,18,17

	dc.w	4, 24,25,29,28,24
	dc.w	4, 28,29,33,32,28
	dc.w	4, 26,27,31,30,26
	dc.w	4, 30,31,32,33,30
	dc.w	4, 27,24,28,31,27
	dc.w	4, 25,26,30,29,25
	dc.w	3, 31,28,32,31
	dc.w	3, 29,30,33,29

fcol:	dc.l	color+[0*256*26*2]
	dc.l	color+[0*256*26*2]
	dc.l	color+[0*256*26*2]
	dc.l	color+[0*256*26*2]
	dc.l	color+[0*256*26*2]

	dc.l	color+[1*256*26*2]
	dc.l	color+[1*256*26*2]
	dc.l	color+[1*256*26*2]
	dc.l	color+[1*256*26*2]
	dc.l	color+[1*256*26*2]
	dc.l	color+[1*256*26*2]

	dc.l	color+[0*256*26*2]
	dc.l	color+[0*256*26*2]
	dc.l	color+[0*256*26*2]
	dc.l	color+[0*256*26*2]
	dc.l	color+[0*256*26*2]

	dc.l	color+[2*256*26*2]
	dc.l	color+[2*256*26*2]
	dc.l	color+[2*256*26*2]
	dc.l	color+[2*256*26*2]
	dc.l	color+[2*256*26*2]
	dc.l	color+[2*256*26*2]
	dc.l	color+[2*256*26*2]
	dc.l	color+[2*256*26*2]

proctab:
	dc.l	con,5-1,fcol
	dc.l	con+[12*5],6-1,fcol+[4*5]
	dc.l	con+[12*11],5-1,fcol+[4*11]
	dc.l	con+[12*16],8-1,fcol+[4*16]

col:	dc.w	$000,$101,$202,$303,$404,$505,$606,$707,$808
	dc.w	$909,$a0a,$b0b,$c0c,$d0d,$e0e,$f0f,$f1f,$f2f
	dc.w	$f3f,$f4f,$f5f,$f6f,$f7f,$f8f,$f9f,$faf

	dc.w	$000,$011,$022,$033,$044,$055,$066,$077,$088
	dc.w	$099,$0aa,$0bb,$0cc,$0dd,$0ee,$0ff,$1ff,$2ff
	dc.w	$3ff,$4ff,$5ff,$6ff,$7ff,$8ff,$9ff,$aff

	dc.w	$000,$010,$020,$030,$040,$050,$060,$070,$080
	dc.w	$090,$0a0,$0b0,$0c0,$0d0,$0e0,$0f0,$0f1,$0f2
	dc.w	$0f3,$0f4,$0f5,$0f6,$0f7,$0f8,$0f9,$0fa

ofs	set	10

offs:	rept	64
	dc.w	358
	endr

	rept	31
	dc.w	ofs
ofs	set	ofs+4
	endr

ofs	set	ofs+4
	
	rept	32
	dc.w	ofs
ofs	set	ofs+4
	endr

ofs	set	ofs+4

	rept	22
	dc.w	ofs
ofs	set	ofs+4
	endr

	rept	64
	dc.w	358
	endr

;*************************************************************************

	SECTION	COPPER,BSS_C

cop1:	ds.w	34+[180*88]+[180*256]
cop2:	ds.w	34+[180*88]+[180*128]
cop3:	ds.w	34+[180*88]+[180*128]
cop4:	ds.w	180*88

	SECTION	TABLE,BSS_P

htab:	ds.w	32

rcalc:	ds.w	34*2
zet:	ds.w	34

light:	ds.w	34

proc:	ds.l	4*3

color:	ds.w	[256*26]*3

int1:	ds.w	340*2
int2:	ds.w	340*2

	END
