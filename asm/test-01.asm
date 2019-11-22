	.org $0800
	ldx #$ff
	txs
	lda #$00
	sta $00
	sta $01

	ldx #$00
	ldy #$00
firstloop:
	txa
	sta $0200,y
	pha
	inx
	iny
	cpy #$5
	bne firstloop ;loop one
secondloop:
	pla
	sta $0200,y
	;iny ;uncomment to create dead-lock
	cpy #$10
  	bne secondloop ; loop two
	kil

	.org $2000 ; NMI handler
	inc $00
	rti

	.org $3000 ; IRQ handler
	inc $01
	rti

	.org $fffa ; vectors
	.word $2000 ; NMI vector
	.word $0800 ; RESET vector
	.word $3000 ; IRQ vector