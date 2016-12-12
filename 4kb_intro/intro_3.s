                                            ;-------------------------------------------------------------------------
; ROUTINE: 4KB INTRO
; CODE:    MORPH/FREEZERS
;
;
;-------------------------------------------------------------------------

	include	"Morph:Asm/Includes/CUSTOM.i"
	include	"Morph:Asm/Includes/MAIN.m"

;*************************************************************************

	SECTION	MAIN,CODE_P

	lea	gfx,a1
	move.l	$4,a6
	moveq	#0,d0
	jsr	(-552,a6)
	move.l	d0,a6

	move.l	(38,a6),oCOP

	lea	tattr,a0
	jsr	(-72,a6)

	move.l	d0,a0

	move.w	(20,a0),ys
	move.w	(38,a0),modulo
	move.l	(34,a0),font

	move.l	d0,a1
	jsr	(-414,a6)

	lea     CUSTOM,a6

	move.w	(DMACONR,a6),d0
	ori.w	#$8000,d0
	move.w	d0,oDMACON

	move.w	(INTENAR,a6),d0
	ori.w	#$c000,d0
	move.w	d0,oINTENA

	bsr	wait
	move.l	#$7fff7fff,(INTENA,a6)
	move.w	#$7fff,(DMACON,a6)

	move.w	#0,(BPLCON3,a6)
	move.w	#$210,(COLOR00,a6)
	move.w	#$200,(BPLCON3,a6)
	move.w	#$210,(COLOR00,a6)

	bsr	make_chunky
	bsr	make_copper
	bsr	make_offs
	bsr	make_sinus
	bsr	planartochunky
	bsr	correct
	bsr	make_color

	lea	cop+2,a0
	move.l	#work,d0
	move.w	d0,(4,a0)
	swap	d0
	move.w	d0,(a0)

        move.w	#StartOkno,(DIWSTRT,a6)
        move.w	#EndOkno,(DIWSTOP,a6)
        move.w	#StartDane,(DDFSTRT,a6)
        move.w	#EndDane,(DDFSTOP,a6)
        move.w	#$1200,(BPLCON0,a6)
        move.w	#0,(BPLCON1,a6)
        move.w	#0,(BPLCON2,a6)
        move.w	#0,(BPLCON4,a6)
        move.w	#Bytes-40-8,(BPL1MOD,a6)
        move.w	#Bytes-40-8,(BPL2MOD,a6)
        move.w	#%0011,(FMODE,a6)

	move.l	#cop,(COP1LCH,a6)
	move.w	#0,(COPJMP1,a6)

	bsr	wait
	move.w	#%1000001111000000,(DMACON,a6)

;*************************************************************************

	lea	work+11+[Bytes*118],a0
	lea	text1,a3
	bsr	print

	moveq	#108-1,d7
	bsr	delay

	lea	work+11+[Bytes*118],a0
	lea	text2,a3
	bsr	print

	moveq	#108-1,d7
	bsr	delay

	bsr	wait
        move.w	#$210,(BPLCON0,a6)
	move.l	#cop1+[360*128],(COP1LCH,a6)
	move.w	#0,(COPJMP1,a6)

;*************************************************************************

	move.w	#586,count
	move.l	#0,count+2

main1:	bsr	wait

	bsr	switch_cop
	bsr	rotate
	bsr	sort

	move.l	proc,daddr
	bsr	show
	move.l	proc+4,daddr
	bsr	show
	move.l	proc+8,daddr
	bsr	show
	move.l	proc+12,daddr
	bsr	show

	bsr	help

	lea	CUSTOM,a6

	tst.b	count+2
	bne.s	noch

	cmp.w	#43,cx
	beq.s	noadd

noch:	add.w	#1,cx

noadd:	subq.w	#1,count
	bne.w	main1

	tst.b	count+4
	bne.s	next

	st	count+2
	st	count+4

	move.w	#101,count
	bra.w	main1

next:	move.l	#points2,paddr
	move.w	#282,zoom
	move.w	#9800,addz

	move.w	#628,count

main2:	bsr	wait

	bsr	kopara

	cmp.w	#43,cx
	beq.s	nosub

	subq.w	#1,cx

nosub:	subq.w	#1,count
	bne.w	main2

main3:	bsr	wait

	bsr	kopara

	addq.w	#1,cx

	cmp.w	#148,cx
	bne.s	main3

	bsr	wait
        move.w	#$1200,(BPLCON0,a6)
	move.l	#cop,(COP1LCH,a6)
	move.w	#0,(COPJMP1,a6)

	lea	work+8+[Bytes*118],a0
	lea	text3,a3
	bsr	print

mous:	btst	#6,CIAA+PRA
	bne.s	mous

	bsr	wait
	move.l	#$7fff7fff,(INTENA,a6)
	move.w	#$7fff,(DMACON,a6)

	move.w	oDMACON,(DMACON,a6)
	move.w	oINTENA,(INTENA,a6)

	move.l	oCOP,(COP1LCH,a6)
	move.w	#0,(COPJMP1,a6)

	moveq	#0,d0
	rts

;*************************************************************************

kopara:	bsr	switch_cop
	bsr	rotate
	bsr	calc_light
	bsr	sort2

	move.l	proc,daddr
	move.l	proc+4,daddr+4
	move.l	proc+8,daddr+8
	bsr	show2
	move.l	proc+12,daddr
	move.l	proc+16,daddr+4
	move.l	proc+20,daddr+8
	bsr	show2
	move.l	proc+24,daddr
	move.l	proc+28,daddr+4
	move.l	proc+32,daddr+8
	bsr	show2
	move.l	proc+36,daddr
	move.l	proc+40,daddr+4
	move.l	proc+44,daddr+8
	bsr	show2

	bsr	help

	lea	CUSTOM,a6
	rts

;*************************************************************************

wait:	WaitRaster	#$137
	rts

;*************************************************************************

delay:	cmp.b	#$80,(VHPOSR,a6)
	bne.s	delay
	bsr	wait
	dbf	d7,delay
	rts

;*************************************************************************
stack:	dc.l	0
count:	dc.w	0,0,0,0

make_chunky:
	move.l	a7,stack

	lea	chunky,a0
	lea	(scrSize,a0),a1
	lea	(scrSize,a1),a2
	lea	(scrSize,a2),a3
	lea	(scrSize,a3),a4
	lea	(scrSize,a4),a5
	lea	(scrSize,a5),a6
	lea	(scrSize,a6),a7

	moveq	#32,d0
	moveq	#33,d1
	moveq	#1,d2
	moveq	#97,d3
	
	move.w	#88,count
chunky1:
	move.w	#3,count+2
chunky2:
	move.w	d2,d4
	move.l	d0,d5
	move.w	#85,count+4
chunky3:
	move.w	#3,count+6
chunky4:
	move.w	d4,d6

	bfins	d6,(a0){d5:1}
	lsr.b	d6
	bfins	d6,(a1){d5:1}
	lsr.b	d6
	bfins	d6,(a2){d5:1}
	lsr.b	d6
	bfins	d6,(a3){d5:1}
	lsr.b	d6
	bfins	d6,(a4){d5:1}
	lsr.b	d6
	bfins	d6,(a5){d5:1}
	lsr.b	d6
	bfins	d6,(a6){d5:1}
	lsr.b	d6
	bfins	d6,(a7){d5:1}

	addq.w	#1,d5
	subq.w	#1,count+6
	bne.s	chunky4

	addq.w	#1,d4
	subq.w	#1,count+4
	bne.s	chunky3

	lea	(Bytes,a0),a0
	lea	(Bytes,a1),a1
	lea	(Bytes,a2),a2
	lea	(Bytes,a3),a3
	lea	(Bytes,a4),a4
	lea	(Bytes,a5),a5
	lea	(Bytes,a6),a6
	lea	(Bytes,a7),a7

	exg	d0,d1
	subq.w	#1,count+2
	bne.w	chunky2

	exg	d2,d3
	subq.w	#1,count
	bne.w	chunky1

	move.l	stack(pc),a7
	rts

;*************************************************************************
bc	=	$210

make_copper:
        lea     cop1+[360*128],a0  
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
	move.w	#bc,(a0)+
	addq.b	#2,d2
	dbf	d3,make3

	move.l	#$1062000,(a0)+
	move.b	#$80,d2
	moveq	#32-1,d3
make4:	move.w	d2,(a0)+
	move.w	#bc,(a0)+
	addq.b	#2,d2
	dbf	d3,make4

	move.l	#$1064000,(a0)+
	move.b	#$80,d2
	moveq	#22-1,d3
make5:	move.w	d2,(a0)+
	move.w	#bc,(a0)+
	addq.b	#2,d2
	dbf	d3,make5

	move.l	#$106c000,(a0)+

	add.l	#$3000000,d0
	lea	(2,a1),a1

	move.l	d0,(a0)+

	move.l	#$1066000,(a0)+
	move.b	#$82,d2
	moveq	#31-1,d3
make6:	move.w	d2,(a0)+
	move.w	#bc,(a0)+
	addq.b	#2,d2
	dbf	d3,make6

	move.l	#$1068000,(a0)+
	move.b	#$80,d2
	moveq	#32-1,d3
make7:	move.w	d2,(a0)+
	move.w	#bc,(a0)+
	addq.b	#2,d2
	dbf	d3,make7

	move.l	#$106a000,(a0)+
	move.b	#$80,d2
	moveq	#22-1,d3
make8:	move.w	d2,(a0)+
	move.w	#bc,(a0)+
	addq.b	#2,d2
	dbf	d3,make8

	move.l	#$106c000,(a0)+

	add.l	#$3000000,d0
	lea	(2,a1),a1

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

make_offs:
	lea	offs,a0
	moveq	#10,d0

	move.w	#320-1,d1
offs1:	move.w	#358,(a0)+
	dbf	d1,offs1

	moveq	#31-1,d1
offs2:	move.w	d0,(a0)+
	addq.w	#4,d0
	dbf	d1,offs2

	addq.w	#4,d0

	moveq	#32-1,d1
offs3:	move.w	d0,(a0)+
	addq.w	#4,d0
	dbf	d1,offs3

	addq.w	#4,d0

	moveq	#22-1,d1
offs4:	move.w	d0,(a0)+
	addq.w	#4,d0
	dbf	d1,offs4

	move.w	#320-1,d1
offs5:	move.w	#358,(a0)+
	dbf	d1,offs5
	rts

;*************************************************************************

make_sinus:
	lea	sin,a0
	lea	sinus,a1

	moveq	#90-1,d0
sin1:	move.w	(a0)+,(a1)+
	dbf	d0,sin1

	lea	sin+[90*2],a0

	moveq	#90-1,d0
sin2:	move.w	-(a0),(a1)+
	dbf	d0,sin2

	lea	sinus,a0

	move.w	#180-1,d0
sin3:	move.w	(a0)+,d1
	neg.w	d1
	move.w	d1,(a1)+
	dbf	d0,sin3

	lea	sin,a0

	moveq	#90-1,d0
sin4:	move.w	(a0)+,(a1)+
	dbf	d0,sin4
	rts

;*************************************************************************
ps	=	[16/8]*16

planartochunky:
	lea	txtr,a0
	lea	rgb,a1
	lea	pal,a2
	move.w	#16-1,d7
conv1:
	moveq	#0,d0
	move.w	#16-1,d6
conv2:	
	moveq	#0,d1
	bftst	(0*ps,a0){d0:1}
	beq.s	not1
	or.w	#1,d1
not1:	bftst	(1*ps,a0){d0:1}
	beq.s	not2
	or.w	#2,d1
not2:	bftst	(2*ps,a0){d0:1}
	beq.s	not3
	or.w	#4,d1
not3:	
	move.w	(a2,d1.w*2),(a1)+
	addq.w	#1,d0
	dbf	d6,conv2

	lea	(16/8,a0),a0
	lea	((256-16)*2,a1),a1
	dbf	d7,conv1

	lea	CUSTOM,a6
	rts

;*************************************************************************

correct:
	lea	rgb+[16*2],a0
	lea	rgb+[16*2],a1

	moveq	#16-1,d0

colo1:	move.l	a0,a2
	move.l	a1,a3
	moveq	#16-1,d1

colo2:	move.w	-(a2),(a3)+
	dbf	d1,colo2

	lea	(512,a0),a0
	lea	(512,a1),a1
	dbf	d0,colo1

	lea	rgb+[512*15],a0
	lea	rgb+[512*16],a1

	moveq	#16-1,d0

colo3:	move.l	a0,a2
	move.l	a1,a3

	moveq	#32-1,d1
colo4:	move.w	(a2)+,(a3)+
	dbf	d1,colo4

	lea	(-512,a0),a0
	lea	(512,a1),a1
	dbf	d0,colo3
	rts

;*************************************************************************

make_color:
	lea	color,a0

	move.w	#$000,d0
	move.w	#$101,d1
	moveq	#16-1,d2
	bsr	save

	move.w	#$f0f,d0
	move.w	#$010,d1
	moveq	#10-1,d2
	bsr	save

	move.w	#$000,d0
	move.w	#$011,d1
	moveq	#16-1,d2
	bsr	save

	move.w	#$0ff,d0
	move.w	#$100,d1
	moveq	#10-1,d2
	bsr	save

	move.w	#$000,d0
	move.w	#$010,d1
	moveq	#16-1,d2
	bsr	save

	move.w	#$0f0,d0
	move.w	#$101,d1
	moveq	#10-1,d2
	bsr	save
	rts

save:	move.w	#256-1,d3
slop:	move.w	d0,(a0)+
	dbf	d3,slop
	add.w	d1,d0
	dbf	d2,save
	rts

;*************************************************************************

print:	move.w	modulo,d7
	moveq	#0,d0

lop1:	move.b	(a3)+,d0
	beq.s	tend

	move.l	font,a1
	lea	(-32,a1,d0.w),a1
	move.l	a0,a2

	move.w	ys,d1
	subq.w	#1,d1
copy:	move.b	(a1),(a2)
	lea	(Bytes,a2),a2
	move.b	(a1),(a2)	
	lea	(a1,d7.w),a1
	lea	(Bytes,a2),a2
	dbf	d1,copy

	lea	(1,a0),a0
	bra.s	lop1
tend:	rts

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
paddr:	dc.l	points
zoom:	dc.w	114
addz:	dc.w	8200
cx:	dc.w	-58

rotate:	lea	axis(pc),a0

	add.w	#3,(a0)
	cmp.w	#360,(a0)
	blt.s	ox
	move.w	#0,(a0)
	
ox:	add.w	#2,(2,a0)
	cmp.w	#360,(2,a0)
	blt.s	oy
	move.w	#0,(2,a0)

oy:	add.w	#1,(4,a0)
	cmp.w	#360,(4,a0)
	blt.s	oz
	move.w	#0,(4,a0)

oz:	movem.w	axis(pc),d0-d2
	lea     sinus,a6
	lea     (a6,d0.w*2),a0
	lea     (a6,d1.w*2),a1
	lea     (a6,d2.w*2),a2
	lea     (180,a6),a6
	lea     (a6,d0.w*2),a3
	move.w	(a6,d1.w*2),d7
	move.w  (a6,d2.w*2),d6
	lea	zet,a4
	lea     rcalc,a5
	move.l  paddr(pc),a6
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
	move.w	d3,(a4)+
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
	muls	zoom(pc),d0
	muls	zoom(pc),d5
	add.w	addz(pc),d3
	divs	d3,d0
	divs	d3,d5
	add.w	cx(pc),d0
	add.w	#45,d5
	move.w  d0,(a5)+
	move.w  d5,(a5)+
	cmp.w	#-1,(a6)
	bne.s	rotate_loop
	rts

;*************************************************************************

sort:	lea	zet,a0

	move.w	(0*2,a0),d0
	add.w	(10*2,a0),d0
	asr.w	d0

	move.w	(1*2,a0),d1
	add.w	(14*2,a0),d1
	asr.w	d1

	move.w	(2*2,a0),d2
	add.w	(19*2,a0),d2
	asr.w	d2

	move.w	(3*2,a0),d3
	add.w	(23*2,a0),d3
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
so6:	
	lea	atab,a0
	lea	proc,a1

	move.l	(a0,d4.w*4),(a1)+
	move.l	(a0,d5.w*4),(a1)+
	move.l	(a0,d6.w*4),(a1)+
	move.l	(a0,d7.w*4),(a1)
	rts

;*************************************************************************
daddr:	dc.l	0,0,0

show:	move.l	daddr(pc),a5
	lea	rcalc,a6

	moveq	#6-1,d7
show_loop:
	moveq	#0,d0
	moveq	#0,d1
	moveq	#0,d2
	
	move.b	(a5),d0
	move.b	(1,a5),d1
	move.b	(2,a5),d2
	
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

	movem.l	d7/a4-a6,-(a7)
	bsr	doface
	movem.l	(a7)+,d7/a4-a6

skip:	lea	(5,a5),a5
	dbf	d7,show_loop
	rts

;*************************************************************************
mem:	dc.w	0,0,0

doface:	move.l	a7,stack

	lea	co,a0
	lea	tp,a4

	move.l	#$ff000100,mem
	moveq	#4-1,d7

loop1:	move.w	d7,mem+4

	moveq	#0,d6

	move.b	(a5)+,d6
	movem.w	(a6,d6.w*4),d0/d1
	move.b	(a5),d6
	movem.w	(a6,d6.w*4),d2/d3

	move.b	(a0)+,d6
	movem.w	(a4,d6.w*4),d4/d7
	move.b	(a0),d6
	movem.w	(a4,d6.w*4),d5/d6

	cmp.w	mem+2(pc),d0
	bge.w	no1
	move.w	d0,mem+2

no1:	cmp.w	mem(pc),d0
	ble.s	no2
	move.w	d0,mem

no2:	lea	int2+[256*3],a2

	cmp.w	d0,d2
	beq.s	out
	bgt.s	no3

	exg	d0,d2
	exg	d1,d3
	exg	d4,d5
	exg	d7,d6

	lea	int1+[256*3],a2

no3:	move.w	d0,a7
	add.w	a7,a7
	add.w	d0,a7

	lea	(a2,a7.w*2),a1

	sub.l	d0,d2
	sub.w	d1,d3
	sub.w	d4,d5
	sub.l	d7,d6

	swap	d3
	swap	d5
	divs.l	d2,d3
	divs.l	d2,d5
	divs.l	d2,d6
	swap	d3
	swap	d5

	moveq	#0,d0
	subq.w	#1,d2

loop2:	movem.w	d1/d4/d7,(a1)
	addq.w	#6,a1

	add.l	d3,d1
	addx.w	d0,d1
	add.l	d5,d4
	addx.w	d0,d4
	add.w	d6,d7
	dbf	d2,loop2

out:	move.w	mem+4(pc),d7
	dbf	d7,loop1

	movem.w	mem(pc),d0/d1
	sub.w	d1,d0
	ble.w	exit
	subq.w	#1,d0

	move.w	d1,d2
	add.w	d2,d2
	add.w	d1,d2

	move.l	caddr+4(pc),a0
	lea	(64,a0),a0

	lea	offs+[320*2],a1
	lea	(a1,d1.w*2),a1
	
	lea	int1+[256*3],a2
	lea	(a2,d2.w*2),a2

	lea	int2+[256*3],a3
	lea	(a3,d2.w*2),a3

	lea	rgb,a4

	moveq	#0,d7

loop3:	movem.w	(a2)+,d1/d3/d5
	movem.w	(a3)+,d2/d4/d6

	sub.l	d1,d2
	ble.s	not

	muls	#360,d1
	lea	(a0,d1.w),a5
	add.w	(a1),a5

	sub.w	d3,d4
	sub.l	d5,d6

	swap	d4
	divs.l	d2,d4
	divs.l	d2,d6
	swap	d4

	subq.w	#1,d2
	
loop4:	move.w	d5,d1
	move.b	d3,d1

	move.w	(a4,d1.w*2),(a5)

	add.l	d4,d3
	addx.b	d7,d3
	add.w	d6,d5

	lea	(360,a5),a5
	dbf	d2,loop4

not:	lea	(2,a1),a1
	dbf	d0,loop3

exit:	move.l	stack(pc),a7
	rts

;*************************************************************************

help:	lea	htab,a0
	move.l	caddr+4(pc),a1
	moveq	#16-1,d0
hlop:	move.l	(a0)+,(a1)+
	dbf	d0,hlop
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

sort2:	lea	zet,a0

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
	ble.s	_so1
	exg	d0,d1
	exg	d4,d5

_so1:	cmp.w	d0,d2
	ble.s	_so2
	exg	d0,d2
	exg	d4,d6

_so2:	cmp.w	d0,d3
	ble.s	_so3
	exg	d0,d3
	exg	d4,d7

_so3:	cmp.w	d1,d2
	ble.s	_so4
	exg	d1,d2
	exg	d5,d6

_so4:	cmp.w	d1,d3
	ble.s	_so5
	exg	d1,d3
	exg	d5,d7

_so5:	cmp.w	d2,d3
	ble.s	_so6
	exg	d2,d3
	exg	d6,d7

_so6:	lea	atab2,a0
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
raddr:	dc.l	0

show2:	move.l	daddr+8(pc),a4
	move.l	daddr(pc),a5
	lea	rcalc,a6
	move.l	daddr+4(pc),d7
show_loop2:
	moveq	#0,d0
	moveq	#0,d1
	moveq	#0,d2
	
	move.b	(1,a5),d0
	move.b	(2,a5),d1
	move.b	(3,a5),d2
	
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
	bmi.s	skip2

	move.l	(a4),raddr
	movem.l	d7/a4-a6,-(a7)
	bsr	doface2
	movem.l	(a7)+,d7/a4-a6

skip2:	lea	(4,a4),a4

	moveq	#0,d0
	move.b	(a5)+,d0
	lea	(1,a5,d0.w),a5

	dbf	d7,show_loop2
	rts

;*************************************************************************

doface2:
	lea	light,a4
	move.l	#$ff000100,mem

	moveq	#0,d6

	moveq	#0,d7
	move.b	(a5)+,d7
	subq.w	#1,d7

_loop1:	moveq	#0,d5

	move.b	(a5)+,d5
	movem.w	(a6,d5.w*4),d0/d1
	movem.w	(a4,d5.w*2),d4
	move.b	(a5),d5
	movem.w	(a6,d5.w*4),d2/d3
	movem.w	(a4,d5.w*2),d5

	cmp.w	mem+2(pc),d0
	bge.s	_no1
	move.w	d0,mem+2

_no1:	cmp.w	mem(pc),d0
	ble.s	_no2
	move.w	d0,mem

_no2:	lea	int2+[256*2],a2

	cmp.w	d0,d2
	beq.s	_out
	bgt.s	_no3

	exg	d0,d2
	exg	d1,d3
	exg	d4,d5

	lea	int1+[256*2],a2

_no3:	lea	(a2,d0.w*4),a1

	sub.l	d0,d2
	sub.w	d1,d3
	sub.l	d4,d5

	swap	d3
	divs.l	d2,d3
	divs.l	d2,d5
	swap	d3

	subq.w	#1,d2

_loop2:	move.w	d1,(a1)+
	move.w	d4,(a1)+

	add.l	d3,d1
	addx.w	d6,d1
	add.w	d5,d4
	dbf	d2,_loop2

_out:	dbf	d7,_loop1

	movem.w	mem(pc),d0/d1
	sub.w	d1,d0
	ble.w	_exit
	subq.w	#1,d0

	move.l	caddr+4(pc),a0
	lea	(64,a0),a0

	lea	offs+[320*2],a1
	lea	(a1,d1.w*2),a1

	lea	int1+[256*2],a2
	lea	(a2,d1.w*4),a2

	lea	int2+[256*2],a3
	lea	(a3,d1.w*4),a3

	move.l	raddr(pc),a4
	moveq	#0,d7

_loop3:	movem.w	(a2)+,d1/d3
	movem.w	(a3)+,d2/d4

	sub.l	d1,d2
	ble.s	_not

	muls	#360,d1
	lea	(a0,d1.w),a5
	add.w	(a1),a5

	sub.l	d3,d4
	divs.l	d2,d4

	subq.w	#1,d2
	
_loop4:	move.w	(a4,d3.w*2),(a5)
	add.w	d4,d3

	lea	(360,a5),a5
	dbf	d2,_loop4

_not:	lea	(2,a1),a1
	dbf	d0,_loop3

_exit:	rts

;*************************************************************************
StartOkno  	= $29a1
EndOkno    	= $31a1
StartDane  	= $38
EndDane    	= $d0
Bytes	   	= 40
scrSize		= Bytes*264
Depth	   	= 8
;*************************************************************************

	SECTION	CHIP,DATA_C

cop:	dc.w	BPL1PTH,0
	dc.w	BPL1PTL,0
	dc.w	BPLCON3,0
	dc.w	COLOR01,$fff
	dc.w	BPLCON3,$200
	dc.w	COLOR01,$fff
	dc.l	-2

	SECTION	OTHER,DATA_P

txtr:	incbin	"Morph:Trashcan/pic/leopard4.raw"

sin:	dc.w	$011f,$035b,$0596,$07d1,$0a0c,$0c46,$0e7e,$10b6,$12ec,$1521
	dc.w	$1754,$1986,$1bb5,$1de2,$200d,$2236,$245b,$267e,$289e,$2abb
	dc.w	$2cd4,$2eea,$30fc,$330b,$3515,$371c,$391e,$3b1b,$3d14,$3f08
	dc.w	$40f8,$42e2,$44c7,$46a6,$4880,$4a55,$4c24,$4dec,$4faf,$516b
	dc.w	$5321,$54d1,$567a,$581c,$59b8,$5b4c,$5cd9,$5e5f,$5fde,$6155
	dc.w	$62c5,$642d,$658d,$66e5,$6835,$697d,$6abd,$6bf4,$6d23,$6e4a
	dc.w	$6f68,$707d,$718a,$728d,$7388,$747a,$7562,$7642,$7718,$77e5
	dc.w	$78a9,$7963,$7a13,$7abb,$7b58,$7bec,$7c77,$7cf7,$7d6e,$7ddb
	dc.w	$7e3f,$7e98,$7ee8,$7f2d,$7f69,$7f9b,$7fc3,$7fe1,$7ff5,$7fff

points:	dc.w   	-1000,1000,-1000
	dc.w   	1000,1000,-1000
	dc.w   	1000,-1000,-1000
	dc.w   	-1000,-1000,-1000

	dc.w   	-1000,1000,1000
	dc.w   	1000,1000,1000
	dc.w   	1000,-1000,1000
	dc.w   	-1000,-1000,1000

	dc.w	-1000,3000,-1000
	dc.w	1000,3000,-1000
	dc.w	1000,3000,1000
	dc.w	-1000,3000,1000

	dc.w	3000,1000,-1000
	dc.w	3000,-1000,-1000
	dc.w	3000,-1000,1000
	dc.w	3000,1000,1000

	dc.w	-1000,-3000,-1000
	dc.w	1000,-3000,-1000
	dc.w	1000,-3000,1000
	dc.w	-1000,-3000,1000

	dc.w	-3000,1000,-1000
	dc.w	-3000,-1000,-1000
	dc.w	-3000,-1000,1000
	dc.w	-3000,1000,1000

	dc.w	-1

con:	dc.b	8,9,1,0,8
	dc.b	10,11,4,5,10
	dc.b	11,10,9,8,11
	dc.b	0,1,5,4,0
	dc.b	9,10,5,1,9
	dc.b	11,8,0,4,11

	dc.b	1,12,13,2,1
	dc.b	15,5,6,14,15
	dc.b	5,15,12,1,5
	dc.b	2,13,14,6,2
	dc.b	12,15,14,13,12
	dc.b	5,1,2,6,5

	dc.b	3,2,17,16,3
	dc.b	6,7,19,18,6
	dc.b	7,6,2,3,7
	dc.b	16,17,18,19,16
	dc.b	2,6,18,17,2
	dc.b	7,3,16,19,7

	dc.b	20,0,3,21,20
	dc.b	4,23,22,7,4
	dc.b	23,4,0,20,23
	dc.b	21,3,7,22,21
	dc.b	0,4,7,3,0
	dc.b	23,20,21,22,23

co:	dc.b	0,1,2,3,0
	even

tp:	dc.w	0,0<<8, 31,0<<8, 31,31<<8, 0,31<<8

atab:	dc.l	con+[5*6*0]
	dc.l	con+[5*6*1]
	dc.l	con+[5*6*2]
	dc.l	con+[5*6*3]

pal:	dc.w	$0100,$0630,$0840,$0a50,$0b61,$0c82,$0d95,$0ec9

points2:
	dc.w	-2800,300,-300
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

con2:	dc.b	4, 0,1,2,3,0
	dc.b	4, 5,4,7,6,5
	dc.b	4, 5,1,0,4,5
	dc.b	4, 2,6,7,3,2
	dc.b	4, 4,0,3,7,4

	dc.b	4, 8,9,10,11,8
	dc.b	4, 13,12,15,14,13
	dc.b	4, 9,13,14,10,9
	dc.b	4, 12,8,11,15,12
	dc.b	4, 12,13,9,8,12
	dc.b	4, 11,10,14,15,11

	dc.b	4, 16,17,18,19,16
	dc.b	4, 21,20,23,22,21
	dc.b	4, 19,18,22,23,19
	dc.b	4, 20,21,17,16,20
	dc.b	4, 17,21,22,18,17

	dc.b	4, 24,25,29,28,24
	dc.b	4, 28,29,33,32,28
	dc.b	4, 26,27,31,30,26
	dc.b	4, 30,31,32,33,30
	dc.b	4, 27,24,28,31,27
	dc.b	4, 25,26,30,29,25
	dc.b	3, 31,28,32,31
	dc.b	3, 29,30,33,29

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

atab2:	dc.l	con2,5-1,fcol
	dc.l	con2+[6*5],6-1,fcol+[4*5]
	dc.l	con2+[6*11],5-1,fcol+[4*11]
	dc.l	con2+[6*16],8-1,fcol+[4*16]

font:	dc.l	0
ys:	dc.w	0
modulo:	dc.w	0

oDMACON:dc.w	0
oINTENA:dc.w	0
oCOP:	dc.l	0

gfx:	dc.b	"graphics.library",0

text1:	dc.b	"Freezers presents...",0
text2:	dc.b	"    a 4kb intro     ",0
text3:	dc.b	"the end...  code by morph",0
	even

tattr:	dc.l	name
	dc.w	8
	dc.b	0
	dc.b	1

name:	dc.b	"topaz.font",0

;*************************************************************************

	SECTION	COPPER,BSS_C

chunky:	ds.b	scrSize*8

cop1:	ds.w	34+[180*88]+[180*256]
cop2:	ds.w	34+[180*88]+[180*128]
cop3:	ds.w	34+[180*88]+[180*128]
cop4:	ds.w	180*88

	SECTION	SCREEN,BSS_C

work:	ds.b	scrSize

	SECTION	TABLE,BSS_P

offs:	ds.w	625
sinus:	ds.w	450

htab:	ds.w	32

rcalc:	ds.w	34*2
zet:	ds.w	34
light:	ds.w	34
proc:	ds.l	4*3

int1:	ds.w	340*3
int2:	ds.w	340*3

rgb:	ds.w	256*64

color:	ds.w	256*26*3

	END
