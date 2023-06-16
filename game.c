/*
TODO:
- (OK) Initial platform layer (window, rendering, input, time)
- Initial Gameplay Exploration
- Engine Improvements (audio, threading, bitmap)
- More Gameplay Exploration
- Polish pass (engine and game)
*/

bool initialized = false;

v2 ball_p;
v2 ball_dp;
v2 ball_half_size;

v2 player_p;
v2 player_dp;
v2 player_half_size;

v2 block_half_size;
typedef struct {
	v2 p;
	int life;
	u32 color;
} Block;

Block blocks[256];
int next_block;

v2 arena;
v2 arena_half_size;

bool aabb_vs_aabb(v2 p1, v2 half_size1, v2 p2, v2 half_size2) {
	return p1.y + half_size1.y > p2.y - half_size2.y
		&& p1.y - half_size1.y < p2.y - half_size2.y
		&& p1.x + half_size1.x > p2.x - half_size2.x
		&& p1.x - half_size1.x < p2.x - half_size2.x;
}

inline void do_block_vs_ball_collision(Block *block, v2 block_half_size, v2 ball_p, v2 ball_half_size) {
	bool collide_y_up = block->p.y - block_half_size.y < ball_p.y + ball_half_size.y;
	bool collide_y_down = block->p.y + block_half_size.y > ball_p.y - ball_half_size.y;
	bool collide_x_left = block->p.x - block_half_size.x < ball_p.x + ball_half_size.x;
	bool collide_x_right = block->p.x + block_half_size.x > ball_p.x - ball_half_size.y;
	
	if(collide_y_up && collide_y_down && collide_x_left && collide_x_right) {
		if(collide_x_right) {
			ball_dp.x *= -1;
			ball_p.x = block->p.x + block_half_size.x;
		}
		else if(collide_x_left) {
			ball_dp.x *= -1;
			ball_p.x = block->p.x - block_half_size.x;
		}
		
		if(collide_y_up) {
			ball_dp.y *= -1;
			ball_p.y = block->p.y + block_half_size.y;
		}
		else if(collide_y_down) {
			ball_dp.y *= -1;
			ball_p.y = block->p.y - block_half_size.y;
		}
	}
}


internal void simulate_game(Input *input, float dt) {
	if(!initialized) {
		initialized = true;
		ball_dp.y = -30;
		ball_p.y = 40;
		ball_half_size = (v2){.75, .75};
		
		player_p.y = -40;
		player_half_size = (v2){10, 2};
		
		arena = (v2){0, 0};
		arena_half_size = (v2) {85, 45};
		
		block_half_size = (v2){4, 2};
		
#define num_x 10
#define num_y 10
		f32 x_offset = block_half_size.x*num_x;
		f32 y_offset = block_half_size.y*num_y - 25;
		for(int y = 0; y < num_y; y++) {
			for(int x = 0; x < num_x; x++) {
				Block *block = blocks + next_block++;
				if(next_block >= array_count(blocks)) {
					next_block = 0;	
				}
				
				block->life = 1;
				block->p.x = x*block_half_size.x*2 - x_offset;
				block->p.y = y*block_half_size.y*2 - y_offset;
				block->color = make_color_from_grey((x+y)*2);
			}
			
		}
	}
	
	v2 player_new = pixels_to_world(input->mouse);
	player_dp.x = (player_new.x - player_p.x)/dt;
	player_p.x = player_new.x;
	
	ball_p = add_v2(ball_p, mul_v2(ball_dp, dt));
	
	// Robustness: Subtract that the ball already moved to get to the colliding position
	if(ball_dp.y < 0 && aabb_vs_aabb(player_p, player_half_size, ball_p, ball_half_size)) {
		// Player collision with ball
		ball_dp.y *= -1;
		ball_dp.x += player_dp.x * .75f;
		ball_p.y = player_p.y + player_half_size.y;
	}
	else if (ball_p.x + ball_half_size.x > arena_half_size.x) {
		// Ball collision with right border
		ball_p.x = arena_half_size.x - ball_half_size.x;
		ball_dp.x *= -1;
	}
	else if (ball_p.x - ball_half_size.x < -arena_half_size.x) {
		// Ball collision with left border
		ball_p.x = -arena_half_size.x + ball_half_size.x;
		ball_dp.x *= -1;
	}
	
	if(ball_p.y + ball_half_size.y > arena_half_size.y) {
		// Ball collision with upper border
		ball_p.y = arena_half_size.y - ball_half_size.y;
		ball_dp.y *= -1;
	}
	
	clear_screen_and_draw_rect((v2){0, 0}, arena_half_size, 0x551100, 0x220500);
	for(Block *block = blocks; block != blocks + array_count(blocks); block++) {
		if(!block->life) continue;
		if(aabb_vs_aabb(block->p, block_half_size, ball_p, ball_half_size)) {
			
		}
		draw_rect(block->p, block_half_size, block->color);
	}
	
	draw_rect(ball_p, ball_half_size, 0x00ffff);
	draw_rect(player_p, player_half_size, 0x00ff00);
}
