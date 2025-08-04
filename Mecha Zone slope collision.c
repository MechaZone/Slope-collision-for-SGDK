static void newCollision()
{
    // Apply gravity
    player_y_velocity += gravity;
    if (player_y_velocity > terminal_velocity)
        {
            player_y_velocity = terminal_velocity;
        } 
    
    player_y += player_y_velocity;

    u8 move_up_for_slope = 0;
    u8 next_slope_state; // used for walking down a slope
    s16 blocked_coord;
    
    //  // define collision coordinates
    s16 player_left_collision_coord = F32_toInt(player_x) + PLAYER_COLBOX_LEFT;
    s16 player_middle_collision_coord_x = F32_toInt(player_x) + PLAYER_COLBOX_MIDDLE;//test for middle colission point 
    s16 player_right_collision_coord = F32_toInt(player_x) + PLAYER_COLBOX_RIGHT;
    s16 player_top_collision_coord = F32_toInt(player_y) + PLAYER_COLBOX_TOP;
    s16 player_middle_collision_coord_y = F32_toInt(player_y) + PLAYER_COLBOX_MIDDLE_Y;//test for middle colission point 
    s16 player_bottom_collision_coord = F32_toInt(player_y) + PLAYER_COLBOX_BOTTOM;

    // for left and right movement
    s16 xtilecoord_left_collision_player = player_left_collision_coord >> 4;
    s16 xtilecoord_right_collision_player = player_right_collision_coord >> 4; 
    s16 ytilecoord_top_collision_player = player_top_collision_coord >> 4;
    s16 ytilecoord_bottom_collision_player = player_bottom_collision_coord >> 4;
 
    // // define containers for results of collision checks
    // TOP
    u8 tile_collision_type_topmiddle;
    
    // RIGHT
    u8 tile_collision_type_topright;
    u8 tile_collision_type_middleright; 
    
    //LEFT
    u8 tile_collision_type_topleft;
    u8 tile_collision_type_middleleft; 
    
    // BOTTOM - defined at top of main.c
    // u8 tile_collision_type_bottomleft;
    // u8 tile_collision_type_bottomMiddle;
    // u8 tile_collision_type_bottomright;
          
    // check all points
    tile_collision_type_topleft = CheckMapCollision(player_left_collision_coord, player_top_collision_coord);
    tile_collision_type_middleleft = CheckMapCollision(player_left_collision_coord, player_middle_collision_coord_y);
    tile_collision_type_bottomleft = CheckMapCollision(player_left_collision_coord +1, player_bottom_collision_coord);

    tile_collision_type_topmiddle = CheckMapCollision(player_middle_collision_coord_x, player_top_collision_coord);
    tile_collision_type_bottomMiddle = CheckMapCollision(player_middle_collision_coord_x, player_bottom_collision_coord);
    next_slope_state = CheckMapCollision(player_middle_collision_coord_x, player_bottom_collision_coord +4); // are you on a slope next frame? - fixes walking down slopes

    tile_collision_type_topright = CheckMapCollision(player_right_collision_coord, player_top_collision_coord);
    tile_collision_type_middleright = CheckMapCollision(player_right_collision_coord, player_middle_collision_coord_y);
    tile_collision_type_bottomright = CheckMapCollision(player_right_collision_coord -1, player_bottom_collision_coord); 


    
    // This makes jumping on slopes work!!
    bool onGround;
    onGround = (tile_collision_type_bottomleft != TileBlank || tile_collision_type_bottomMiddle != TileBlank || tile_collision_type_bottomright != TileBlank);
    if(!onGround)
        {
            // If was on ground last frame, check if we're going over a bump, and stick to the ground
            if(playerOnGround)
            {
                onGround = TRUE;
            }
        }

    // Update players ground state - must stay outside the previous IF statement 
    playerOnGround = onGround;

    // check if the player is on a slope to prevent getting stuck at the top of a slope TileSlopeLR2_1
    if ((tile_collision_type_bottomMiddle >= TileSlopeLR && tile_collision_type_bottomMiddle <= TileSlopeRL2_2)
        || (tile_collision_type_bottomleft >= TileSlopeLR && tile_collision_type_bottomleft <= TileSlopeRL2_2)
        || (tile_collision_type_bottomright >= TileSlopeLR && tile_collision_type_bottomright <= TileSlopeRL2_2))
        {
            playerOnSlope = TRUE;
        } 
    else playerOnSlope = FALSE;

    // set floor type for fall-through platforms 
    if (tile_collision_type_bottomMiddle == TileJumpthrough
        || (tile_collision_type_bottomleft == TileBlank && tile_collision_type_bottomright == TileJumpthrough)
        || (tile_collision_type_bottomleft == TileJumpthrough && tile_collision_type_bottomright == TileBlank)
        || (tile_collision_type_bottomleft == TileJumpthrough && tile_collision_type_bottomMiddle == TileJumpthrough)
        || (tile_collision_type_bottomright == TileJumpthrough && tile_collision_type_bottomMiddle == TileJumpthrough))
        {
            on_Platform_Tile = TRUE;
        }
    else on_Platform_Tile = FALSE;

    // fall through platform or stick to the ground
    if (fall_through_platform && on_Platform_Tile)
    {
        falling = TRUE;
        player_y_velocity += gravity;
    }
    else if (playerOnGround && player_y_velocity >= FIX32(0) && tile_collision_type_bottomMiddle != TileBlank)
        {
            // previous_slope_state = tile_collision_type_bottomMiddle;
            #define MAX_MOVEUP_CHECKS 16
            move_up_for_slope = 0;
            while (tile_collision_type_bottomMiddle != TileBlank && move_up_for_slope < MAX_MOVEUP_CHECKS)
                        {
                            move_up_for_slope ++;
                            tile_collision_type_bottomMiddle = CheckMapCollision(player_middle_collision_coord_x, player_bottom_collision_coord - move_up_for_slope);
                        }

            player_y -= FIX32(move_up_for_slope);
            player_y_velocity = FIX32(0);
            falling = FALSE;
            jumping = FALSE;
            jump_timer = 0;
        }
    // fixes platform corner fall-through
    else if (!playerOnSlope && player_y_velocity > FIX32(0) &&
            (next_slope_state < 3 || next_slope_state > 15) && // next step isn't on a slope
            (tile_collision_type_bottomright != TileBlank || tile_collision_type_bottomleft != TileBlank ))
        {            
            blocked_coord = (ytilecoord_bottom_collision_player << 4) - PLAYER_COLBOX_BOTTOM;
            player_y = FIX32(blocked_coord);
            player_y -= FIX32(0.1);

            player_y_velocity = FIX32(0);
            jumping = FALSE;
            jump_timer = 0;
        }

    // head collisions with solid tile
    if (tile_collision_type_topleft == TileSolid || tile_collision_type_topmiddle == TileSolid || tile_collision_type_topright == TileSolid)
        {
            blocked_coord = (ytilecoord_top_collision_player << 4) + 16 - PLAYER_COLBOX_TOP;
            player_y = FIX32(blocked_coord);
            player_y_velocity = FIX32(0); // makes the jump stop if you hit your head
            jump_timer = 0; // kills the jump
        }

    if (player_move_left)
    {    
        if(dashing_active) {player_x -= player_x_velocity + player_Dash_velocity;}
        else {player_x -= player_x_velocity;}

        player_top_collision_coord = F32_toInt(player_y) + PLAYER_COLBOX_TOP;
        player_left_collision_coord = F32_toInt(player_x) + PLAYER_COLBOX_LEFT;
        xtilecoord_left_collision_player = player_left_collision_coord >> 4;

        tile_collision_type_topleft = CheckMapCollision(player_left_collision_coord, player_top_collision_coord +1); // recalculating top coordinate and adding the +1 fixes the movement stutter when your head hits ceiling while jumping
        tile_collision_type_middleleft = CheckMapCollision(player_left_collision_coord, player_middle_collision_coord_y);

        tile_collision_type_bottomleft = CheckMapCollision(player_left_collision_coord, player_bottom_collision_coord);

        if (tile_collision_type_topleft == TileSolid
            || tile_collision_type_middleleft == TileSolid
            || (tile_collision_type_bottomleft == TileSolid && !playerOnGround && !jumping)
            )
            {
                blocked_coord = (xtilecoord_left_collision_player << 4) + 16 - PLAYER_COLBOX_LEFT;
                player_x = FIX32(blocked_coord);
                player_x += FIX32(0.1);
            }
    }
    else if (player_move_right)
    {
        if(dashing_active) {player_x += player_x_velocity + player_Dash_velocity;}
        else {player_x += player_x_velocity;}

        player_top_collision_coord = F32_toInt(player_y) + PLAYER_COLBOX_TOP;
        player_right_collision_coord = F32_toInt(player_x) + PLAYER_COLBOX_RIGHT;
        xtilecoord_right_collision_player = player_right_collision_coord >> 4;

        tile_collision_type_topright = CheckMapCollision(player_right_collision_coord, player_top_collision_coord +1); // recalculating top coordinate and adding the +1 fixes the movement stutter when your head hits ceiling while jumping
        tile_collision_type_middleright = CheckMapCollision(player_right_collision_coord, player_middle_collision_coord_y);
        tile_collision_type_bottomright = CheckMapCollision(player_right_collision_coord, player_bottom_collision_coord); 

         if (tile_collision_type_topright == TileSolid
            || tile_collision_type_middleright == TileSolid
            || (tile_collision_type_bottomright == TileSolid && !playerOnGround && !jumping)
            )
            {                
                blocked_coord = (xtilecoord_right_collision_player << 4) - PLAYER_COLBOX_RIGHT;
                player_x = FIX32(blocked_coord);
                player_x -= FIX32(0.1);
            }
    }

        // collisions with damage tile
        if (!player_flashing_trigger &&
            (tile_collision_type_topleft == TileHurt ||
            tile_collision_type_bottomleft == TileHurt ||
            tile_collision_type_topright == TileHurt ||
            tile_collision_type_bottomright == TileHurt ||
            tile_collision_type_middleleft == TileHurt ||
            tile_collision_type_middleright == TileHurt))
                {
                    player_hitpoints -= damage_tile_amount;
                
                    if ((health_bar_level - damage_tile_amount) < 0)
                        {health_bar_level = 0;}
                    else health_bar_level -= damage_tile_amount;

                    SPR_setAnim(hud_health, health_bar_level);
                    player_flashing_trigger = TRUE; // this looks redundant but needs to be here to prevent instant death

                        if(health_bar_level == 0) // you're dead!
                            {
                                GameOver = TRUE;
                                handleInput_NULL();
                                JOY_setEventHandler(joyEvent_NULL);
                                player_flashing_trigger = FALSE;
                                camera_shaking_Large = TRUE;
                                SPR_setAnim(player, ANIM_DEATH);
                            }
                        else 
                            {
                                player_flashing_trigger = TRUE;
                                camera_shaking_Small = TRUE;
                                XGM_startPlayPCM(SND_EXPLOSION, 1, SOUND_PCM_CH3);
                                defineExplosionsBasic(player_x, player_y + FIX32(8));
                            }
                }
            // death floor collision
            else if (tile_collision_type_topleft == TileDie
                || tile_collision_type_middleleft == TileDie
                || tile_collision_type_bottomleft == TileDie
                || tile_collision_type_topright == TileDie
                || tile_collision_type_middleright == TileDie
                || tile_collision_type_bottomright == TileDie
                )
                    {
                        GameOver = TRUE;
                        handleInput_NULL();
                        JOY_setEventHandler(joyEvent_NULL);

                        player_y_velocity = FIX32(0);
                        player_x = playerXLast;
                        player_y = playerYLast;
                        
                        health_bar_level = 0;
                        SPR_setAnim(hud_health, health_bar_level);
                        
                        SPR_setVisibility (player, VISIBLE);
                        player_flashing_trigger = FALSE;
                        SPR_setAnim(player, ANIM_DEATH);

                        camera_shaking_Large = TRUE;
                    }

        // conveyor left tile
        if (tile_collision_type_bottomleft == ConveyorTileLeft || tile_collision_type_bottomright == ConveyorTileLeft)
            {
                player_x -= conveyor_speed; // pushes player
            }
        // conveyor right tile
        else if (tile_collision_type_bottomleft == ConveyorTileRight || tile_collision_type_bottomright == ConveyorTileRight)
            {
                player_x += conveyor_speed; // pushes player 
            }

    SPR_setPosition(player, F32_toInt(player_x) - new_camera_x, F32_toInt(player_y) - (new_camera_y));
    playerXLast = player_x;
    playerYLast = player_y;
}