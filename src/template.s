	.section ".rodata"

	.globl html_top
	.globl html_bot

html_top:
	.incbin "src/top.html"
	.byte 0

html_bot:
	.incbin "src/bottom.html"
	.byte 0
