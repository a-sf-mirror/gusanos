#ifndef OMFG_BLITTERS_MACROS_H
#define OMFG_BLITTERS_MACROS_H

// Rectangles

#define CLIP_RECT() \
	if(x1 < where->cl)  x1 = where->cl; \
	if(y1 < where->ct)  y1 = where->ct; \
	if(x2 >= where->cr) x2 = where->cr - 1; \
	if(y2 >= where->cb) y2 = where->cb - 1; \
	if(x1 > x2 || y1 > y2) return
	
#define RECT_Y_LOOP(ops_) \
	for(int y = y1; y <= y2; ++y) { \
		ops_ }
		
#define RECT_X_LOOP_ALIGN(par_, align_, op_1, op_2) { \
	int c_ = x2 - x1; \
	pixel_t_1* p_ = (pixel_t_1 *)where->line[y] + x1; \
	while(ptrdiff_t(p_) & (align_ - 1)) { \
		pixel_t_1* p = p_; \
		op_1; \
		--c_; ++p_; } \
	for(; c_ >= par_; c_ -= par_, p_ += par_) { \
		pixel_t_2* p = (pixel_t_2 *)p_; \
		op_2; } \
	while(c_-- >= 1) { \
		pixel_t_1* p = p_; \
		op_1; ++p_; } }
	
#define RECT_X_LOOP_NOALIGN(par_, op_1, op_2) { \
	int c_ = x2 - x1; \
	pixel_t_1* p_ = (pixel_t_1 *)where->line[y] + x1; \
	for(; c_ >= par_; c_ -= par_, p_ += par_) { \
		pixel_t_2* p = (pixel_t_2 *)p_; \
		op_2; } \
	while(c_-- >= 1) { \
		pixel_t_1* p = p_; \
		op_1; ++p_; } }
		
#define RECT_X_LOOP(op_1) { \
	int c_ = x2 - x1; \
	pixel_t_1* p = (pixel_t_1 *)where->line[y] + x1; \
	for(; c_ >= 1; --c_, ++p) { \
		op_1; } }
		
// Sprites

#define CLIP_SPRITE() \
	int x1, y1, x2, y2; \
	if(x + from->w > where->cr) { x2 = where->cr - x; } else x2 = from->w; \
	if(y + from->h > where->cb) { y2 = where->cb - y; } else y2 = from->h; \
	if(x < where->cl)      { x1 = where->cl - x; x = where->cl; } else x1 = 0; \
	if(y < where->ct)      { y1 = where->ct - y; y = where->ct; } else y1 = 0; \
	if(x1 >= x2 || y1 >= y2) return
	
#define CLIP_SPRITE_REGION() \
	int x1 = cutl, y1 = cutt, x2 = from->w - cutr, y2 = from->h - cutb; \
	if(x + (x2 - x1) > where->cr) { x2 = where->cr - x + x1; } \
	if(y + (y2 - y1) > where->cb) { y2 = where->cb - y + y1; } \
	if(x < where->cl) { x1 += where->cl - x; x = where->cl; } \
	if(y < where->ct) { y1 += where->ct - y; y = where->ct; } \
	if(x1 >= x2 || y1 >= y2) return

#define SPRITE_Y_LOOP(ops_) \
	for(; y1 < y2; ++y, ++y1) { \
		ops_ }
		
#define SPRITE_X_LOOP(op_1) { \
	int c_ = x2 - x1; \
	pixel_t_1* dest = (pixel_t_1 *)where->line[y] + x; \
	pixel_t_1* src  = (pixel_t_1 *)from->line[y1] + x1; \
	for(; c_ >= 1; --c_, ++dest, ++src) { \
		op_1; } }
		
#define SPRITE_X_LOOP_T(op_1) { \
	int c_ = x2 - x1; \
	pixel_t_dest* dest = (pixel_t_dest *)where->line[y] + x; \
	pixel_t_src* src  = (pixel_t_src *)from->line[y1] + x1; \
	for(; c_ >= 1; --c_, ++dest, ++src) { \
		op_1; } }
		
		
		
#define SPRITE_X_LOOP_ALIGN(par_, align_, op_1, op_2) { \
	int c_ = x2 - x1; \
	pixel_t_1* dest_ = (pixel_t_1 *)where->line[y] + x; \
	pixel_t_1* src_  = (pixel_t_1 *)from->line[y1] + x1; \
	while(ptrdiff_t(dest_) & (align_ - 1)) { \
		pixel_t_1* dest = dest_; \
		pixel_t_1* src = src_; \
		op_1; \
		--c_; ++dest_; ++src_; } \
	for(; c_ >= par_; c_ -= par_, dest_ += par_, src_ += par_) { \
		pixel_t_2* dest = (pixel_t_2 *)dest_; \
		pixel_t_2* src = (pixel_t_2 *)src_; \
		op_2; } \
	while(c_-- >= 1) { \
		pixel_t_1* dest = dest_; \
		pixel_t_1* src = src_; \
		op_1; ++dest_; ++src_; } }
		
#define SPRITE_X_LOOP_NOALIGN(par_, op_1, op_2) { \
	int c_ = x2 - x1; \
	pixel_t_1* dest_ = (pixel_t_1 *)where->line[y] + x; \
	pixel_t_1* src_  = (pixel_t_1 *)from->line[y1] + x1; \
	for(; c_ >= par_; (c_ -= par_), (dest_ += par_), (src_ += par_)) { \
		pixel_t_2* dest = (pixel_t_2 *)dest_; \
		pixel_t_2* src = (pixel_t_2 *)src_; \
		op_2; } \
	while(c_-- >= 1) { \
		pixel_t_1* dest = dest_; \
		pixel_t_1* src = src_; \
		op_1; ++dest_; ++src_; } }

#define WULINE(a, b, OFFA, OFFB, DEPTH, SHIFT, MAX, BLEND) { \
		if(a##diff < 0)	{ \
			a##diff = -a##diff; \
			b##diff = -b##diff; \
			std::swap(a##1, a##2); \
			std::swap(b##1, b##2); } \
		if(a##diff > 0) { \
			long grad = (b##diff << prec) / a##diff; \
			long a##end1 = a##1 & ~fracmask; \
			long b##end1 = b##1 + (grad*(one - (a##1 - a##end1) + half) >> prec); \
			long a##end2 = a##2 & ~fracmask; \
			long b##end2 = b##2 - (grad*(a##2 - a##end2) >> prec); \
			long b##f = b##end1 - half; \
			long draw##a = (a##end1 + one + half) >> prec; /* TODO: Draw endpoints */  \
			long c = (a##end2 - a##end1) >> prec; \
			for(; c-- > 0; ++draw##a) { \
				int draw##b = b##f >> prec; \
				if((unsigned int)drawx < (unsigned int)where->w - (OFFA) && (unsigned int)drawy < (unsigned int)where->h - (OFFB)) { \
					int fl = ((b##f & fracmask) + (1 << (SHIFT))/2) >> (SHIFT); \
					int fu = (MAX) - fl; \
					Pixel##DEPTH* u = ((Pixel##DEPTH *)where->line[drawy]) + drawx; \
					Pixel##DEPTH* l = ((Pixel##DEPTH *)where->line[drawy + (OFFB)]) + drawx + (OFFA); \
					*u = BLEND(*u, colour, (fu * fact >> 8)); \
					*l = BLEND(*l, colour, (fl * fact >> 8)); \
				} \
				b##f += grad; \
		} } }
		
#define SIGN(x_) ((x_) < 0 ? -1 : (x_) > 0 ? 1 : 0)

#define FILTER_2X_TO_VIDEO_DO_ROW(Even_, Odd_) { \
		for(int x = 0; x < 319; ++x) { \
			Pixel ul __attribute__ ((unused)) = usrct[0]; \
			Pixel ur __attribute__ ((unused)) = usrct[1]; \
			Pixel ll __attribute__ ((unused)) = lsrct[0]; \
			Pixel lr __attribute__ ((unused)) = lsrct[1]; \
			Even_; \
			dest += sizeof(pixel_t_1); \
			Odd_; \
			dest += sizeof(pixel_t_1); \
			usrct += 1; \
			lsrct += 1; } \
		Pixel ul __attribute__ ((unused)) = usrct[0]; \
		Pixel ur __attribute__ ((unused)) = 0; \
		Pixel ll __attribute__ ((unused)) = lsrct[0]; \
		Pixel lr __attribute__ ((unused)) = 0; \
		Even_; \
		dest += sizeof(pixel_t_1); \
		Odd_; }
		
#define FILTER_2X_TO_VIDEO_DO_LAST_ROW(Even_, Odd_) { \
		Pixel ll __attribute__ ((unused)) = 0; \
		Pixel lr __attribute__ ((unused)) = 0; \
		for(int x = 0; x < 319; ++x) { \
			Pixel ul __attribute__ ((unused)) = usrct[0]; \
			Pixel ur __attribute__ ((unused)) = usrct[1]; \
			Even_; \
			dest += sizeof(pixel_t_1); \
			Odd_; \
			dest += sizeof(pixel_t_1); \
			usrct += 1; } \
		Pixel ul __attribute__ ((unused)) = usrct[0]; \
		Pixel ur __attribute__ ((unused)) = 0; \
		Even_; \
		dest += sizeof(pixel_t_1); \
		Odd_; }

#define FILTER_2X_TO_VIDEO(ULop_, URop_, LLop_, LRop_) { \
	acquire_screen(); \
	bmp_select(screen); \
	for(int y = 0; y < 239; ++y) { \
		pixel_t_1* usrc = (pixel_t_1 *)buffer->line[y]; \
		pixel_t_1* lsrc = (pixel_t_1 *)buffer->line[y + 1]; \
		unsigned long dest = bmp_write_line(screen, y*2); \
		pixel_t_1* usrct = usrc; \
		pixel_t_1* lsrct = lsrc; \
		FILTER_2X_TO_VIDEO_DO_ROW(ULop_, URop_); \
		dest = bmp_write_line(screen, y*2 + 1); \
		usrct = usrc; \
		lsrct = lsrc; \
		FILTER_2X_TO_VIDEO_DO_ROW(LLop_, LRop_); } \
	pixel_t_1* usrct = (pixel_t_1 *)buffer->line[239]; \
	unsigned long dest = bmp_write_line(screen, 478); \
	FILTER_2X_TO_VIDEO_DO_LAST_ROW(ULop_, URop_); \
	dest = bmp_write_line(screen, 479); \
	FILTER_2X_TO_VIDEO_DO_LAST_ROW(LLop_, LRop_); \
	bmp_unwrite_line(screen); \
	release_screen(); }

#endif //OMFG_BLITTERS_MACROS_H
