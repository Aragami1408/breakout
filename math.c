// Color
u32 make_color_from_grey(u32 grey) {
	return (grey << 0) | (grey << 8) | (grey << 16);
}
// Vector 2
typedef struct {
	union {
		struct {
			f32 x;
			f32 y;
		};
		f32 e[2];
	};
} v2;

inline v2 add_v2(v2 a, v2 b) {
	return (v2){a.x + b.x, a.y + b.y};
}

inline v2 mul_v2(v2 a, f32 s) {
	return (v2){a.x * s, a.y * s};
}

// Vector 2 int
typedef struct {
	union {
		struct {
			int x;
			int y;
		};
		int e[2];
	};
} v2i;

