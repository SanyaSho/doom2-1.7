// dither.h

void MakeDither (void);
void SetDitherPalette (unsigned char *palette);
void DitherUpdate4 (unsigned char *source, unsigned char *dest
	, int width, int height);
void DitherUpdate2 (unsigned char *source, unsigned char *dest
	, int width, int height);
void DitherUpdate1 (unsigned char *source, unsigned char *dest
	, int width, int height);
