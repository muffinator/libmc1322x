#ifndef SLEEP2
#define SLEEP2

#define set_adc(x) \
	GPIO->FUNC_SEL.x = 1;\
	GPIO->PAD_DIR.x = 0;\
	GPIO->PAD_KEEP.x = 0;\
	GPIO->PAD_PU_EN.x = 0;\

#define set_out(x) \
	GPIO->FUNC_SEL.x = 3;\
	GPIO->PAD_DIR.x = 1;\


#endif
