  .ORG $0810

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
  INY
  CPY #$ff
  BNE secondloop ; loop two
  KIL

	.org $1000
	
	lda #$ff
	ldx #$ff
	ldy #$a0
	kil