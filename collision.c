bool aabb_vs_aabb(v2 p1, v2 half_size1, v2 p2, v2 half_size2) {
	return p1.y + half_size1.y > p2.y - half_size2.y
		&& p1.y - half_size1.y < p2.y - half_size2.y
		&& p1.x + half_size1.x > p2.x - half_size2.x
		&& p1.x - half_size1.x < p2.x - half_size2.x;
}

// Still AABB vs AABB
inline void 
do_block_vs_ball_collision(Block *block, v2 block_half_size, v2 ball_p, v2 ball_half_size) {
    bool collide_y_down = block->p.y - block_half_size.y < ball_p.y + ball_half_size.y;
    bool collide_y_up = block->p.y + block_half_size.y > ball_p.y - ball_half_size.y;
    
    bool collide_x_left = block->p.x - block_half_size.x < ball_p.x + ball_half_size.x;
    bool collide_x_right = block->p.x + block_half_size.x > ball_p.x - ball_half_size.x;
    
    if(collide_y_down && collide_y_up && collide_x_left && collide_x_right) {
        if(collide_x_right) {
            
            ball_dp.y *= -1;
            ball_p.y = block->p.y + block_half_size.y;
        }
        else if(collide_x_left) {
            ball_dp.y *= -1;
            ball_p.y = block->p.y - block_half_size.y;
        }
        
        if(collide_y_up) {
            ball_dp.x *= -1;
            ball_p.x = block->p.x + block_half_size.x;
        }
        else if(collide_y_down) {
            ball_dp.x *= -1;
            ball_p.x = block->p.x - block_half_size.x;
        }
    }
}