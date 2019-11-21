	.org $810

	lda #%10010101
	sta $00
	lda #%11100110
	sta $01
	inc $01

  LDX #$00
  LDY #$00
firstloop:
  TXA
  STA $0200,Y
  PHA
  INX
  INY
  CPY #$ff
  BNE firstloop ;loop one
secondloop:
  PLA
  STA $0200,Y
  	ldy #$fe
	cpy #$ff
  	bne secondloop ; loop two
	kil

	.org $1000
	
	lda #$ff
	ldx #$ff
	ldy #$a0
	kil