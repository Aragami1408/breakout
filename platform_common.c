typedef struct {
	bool is_down;
	bool changed;
} Button;

enum {
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_UP,
	BUTTON_DOWN,

	BUTTON_COUNT, // Always the last item
};

typedef struct {
	v2i mouse;
	Button buttons[BUTTON_COUNT];
} Input;

#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)
#define is_down(b) (!input->buttons[b].is_down)
