#include "engine.h"


struct engine* game;
PALETTE pal;

void echo()
{
  if (strlen(con->arg)!=0)
  con->echolist.add_echo(con->arg);
};

void save_log()
{
  if (strlen(con->arg)!=0)
  con->save_log(con->arg);
};

void connect()
{
  cli = new Client( 0, 8989 );
  adr_srv.setAddress( eZCom_AddressUDP, 0, con->arg );
  ZCom_BitStream *req = ZCom_Control::ZCom_createBitStream();
  req->addString( "letmein" ); 
  cli->ZCom_Connect( adr_srv, req );
};
 
void quit()
{
	exit(0);
};

void recharge_weapons(struct worm* player)
{
  int o;
  for(o=0;o<5;o++)
  {
    player->weap[o].shoot_time=0;
    player->weap[o].ammo=weaps->num[player->weap[o].weap]->ammo;
    player->weap[o].reloading=false;
    player->weap[o].reload_time=0;
  };
};

void respawn_player(struct worm* player)
{
	int g,o;
	player->health=*game->MAX_HEALTH;
	g=-1;
  if (map->spawnpoint_count==0)
  {
    while (!map->mat[g+1].worm_pass || map->mat[g+1].flows)
    {
      player->x=(rand()%map->material->w)*1000;
      player->y=(rand()%map->material->h)*1000;
      g=getpixel(map->material,player->x/1000,(player->y)/1000);
    };
    while (map->mat[g+1].worm_pass && !map->mat[g+1].flows)
    {
      player->y++;
      g=getpixel(map->material,player->x/1000,(player->y)/1000+5);
    };
  }else
  {
    g=rand()%map->spawnpoint_count;
    while (game->teamplay && map->spawnpoint[g].team!=player->team)
    {
      g=rand()%map->spawnpoint_count;
    };
    player->x=map->spawnpoint[g].x*1000;
    player->y=map->spawnpoint[g].y*1000;
  };
  //render_light(player->x/1000,player->y/1000, map->mapimg ,map->material); 
	player->xspd=0;
	player->yspd=0;
	player->aim=64000;
  player->aim_recoil_speed=0;
  player->aim_speed=0;
  player->curr_frame=0;
	player->dir=1;
  player->killed_by=local_player[player->local_slot];
  player->air=*game->AIR_CAPACITY;
	play_sample(game->respawn->snd,*game->VOLUME, 127, 1000, 0);
	player->active=true;
  if (*game->RESPAWN_RELOAD==1)
    recharge_weapons(player);
};

volatile int speed_counter = 0;
volatile int t = 0;

void increment_speed_counter()
{
	speed_counter++;
}
END_OF_FUNCTION(increment_speed_counter);

void tick()
{
	t++;
}
END_OF_FUNCTION(tick);


void engine::calcphysics()
{
	int c,i,o,g,x,y;
  
  if(srv)srv->ZCom_processInput(eZCom_NoBlock);
  if(cli)cli->ZCom_processInput(eZCom_NoBlock);
  if (srv) srv->ZCom_processOutput();
  if (cli) cli->ZCom_processOutput();

  //if (player_count>2 && player[player_count-1]->node->getRole()==eZCom_RoleOwner)allegro_message("yep its the owner");
  
  if(player_count!=0)
  {
    if (game->selecting)
    {
      game->selecting=false;
      for (i=0;i<local_players;i++)
      if (player[local_player[i]]->selecting_weaps) game->selecting=true;

      if (!game->selecting)
      {
        game->selecting=false;
        play_sample(gstart->snd, *VOLUME, 127, 1000, 0);
      };
    };
    if (true)
    {
      calc_particles();
      calc_exps();
      for(c=0;c<player_count;c++)
      {
        if(player[c]->node && (cli||srv)){
          player[c]->checkevents();
          if (player[c]->deleteme)
          {
            delete player[c];
            player[c]=player[player_count-1];
            player[player_count-1]=NULL;
            player_count--;
            c--;
          };
        };
        if (player[c] && player[c]->active)
        {
          /****************AIR FRICTION********************/
          if(!cli)
          {
            if(getpixel(map->material,player[c]->x/1000,(player[c]->y-4500)/1000)==3)
            {
              if (abs(player[c]->xspd)>20)
              {
                if (player[c]->xspd<0) player[c]->xspd+=20;
                if (player[c]->xspd>0) player[c]->xspd+=-20;
              } else player[c]->xspd=0;
              if (abs(player[c]->yspd)>20)
              {
                if (player[c]->yspd<0) player[c]->yspd+=20;
                if (player[c]->yspd>0) player[c]->yspd+=-20;
              } else player[c]->yspd=0;
              player[c]->yspd=player[c]->yspd-37;
              player[c]->air--;
              i=0;
              for(o=0;o<20;o++)
              {
                g=getpixel(map->material,player[c]->x/1000,(player[c]->y-4500)/1000 + o);
                if(!map->mat[g+1].worm_pass)
                {
                  break;
                }
                if(map->mat[g+1].can_breath)
                {
                  i=o;
                  break;
                };
              };
              if(i>0)player[c]->yspd=player[c]->yspd+100;
            }else
            {
              if (abs(player[c]->xspd)>abs(*AIR_FRICTION))
              {
                if (player[c]->xspd<0) player[c]->xspd+=*AIR_FRICTION;
                if (player[c]->xspd>0) player[c]->xspd+=-*AIR_FRICTION;
              } else player[c]->xspd=0;
              if (abs(player[c]->yspd)>abs(*AIR_FRICTION))
              {
                if (player[c]->yspd<0) player[c]->yspd+=*AIR_FRICTION;
                if (player[c]->yspd>0) player[c]->yspd+=-*AIR_FRICTION;
              } else player[c]->yspd=0;
              if(player[c]->air<=*AIR_CAPACITY/2 && breath->snd!=NULL) play_sample(breath->snd, *VOLUME, 127, 1000, 0);
              player[c]->air=*AIR_CAPACITY;
            };
            player[c]->yspd=player[c]->yspd+*GRAVITY;
            
            /***************************************************/
            g=getpixel(map->material,player[c]->x/1000,(player[c]->y+player[c]->yspd)/1000);
            
            if (!map->mat[g+1].worm_pass && player[c]->yspd>0)
            {
              if (abs(player[c]->xspd)<*FRICTION) player[c]->xspd=0;
              
              if (player[c]->xspd<0) player[c]->xspd+=*FRICTION;
              if (player[c]->xspd>0) player[c]->xspd+=-*FRICTION;
              if (player[c]->yspd > *DAMAGE_SPEED) player[c]->health-=((abs(player[c]->yspd) - *DAMAGE_SPEED)* *FALL_DAMAGE)/1000;
              if (player[c]->yspd< WORM_BOUNCE_LIMIT) player[c]->yspd=0;
              else
              {
                if (player[c]->health>0)
                play_sample(bump->snd, *VOLUME, 127, 1000, 0);
                player[c]->yspd= (int) (player[c]->yspd*-1* (*WORM_BOUNCINESS/1000.));
              };
            };
            g=getpixel(map->material,player[c]->x/1000,player[c]->y/1000-8);
            
            if (!map->mat[g+1].worm_pass && player[c]->yspd<0) 
            {
              if (abs(player[c]->yspd)< WORM_BOUNCE_LIMIT) player[c]->yspd=0;
              else
              {
                if (abs(player[c]->yspd) > *DAMAGE_SPEED) player[c]->health-=((abs(player[c]->yspd) - *DAMAGE_SPEED)* *FALL_DAMAGE)/1000;;
                if (player[c]->health>0)
                play_sample(bump->snd, *VOLUME, 127, 1000, 0);
                player[c]->yspd= (int) (player[c]->yspd*-1* (*WORM_BOUNCINESS/1000.));
              };
            };
            
            o=0;
            for (i=1;i<=8;i++)
            {
              g=getpixel(map->material,(player[c]->x+player[c]->xspd)/1000,player[c]->y/1000-i+1);
              if (!map->mat[g+1].worm_pass) o=i;
            };
            
            if (o==0) player[c]->x=player[c]->x+player[c]->xspd;
            player[c]->y=player[c]->y+player[c]->yspd;
            
            if (o<=4 && o>0 && !player[c]->xspd==0)
            {
              player[c]->y-=1000;
              if (player[c]->xspd<0) player[c]->xspd+=*FRICTION;
              if (player[c]->xspd>0) player[c]->xspd+=-*FRICTION;
            };
            
            if (o>1)
            {
              if (abs(player[c]->xspd) > *DAMAGE_SPEED) player[c]->health-=((abs(player[c]->xspd) - *DAMAGE_SPEED)* *FALL_DAMAGE)/1000;
              if (abs(player[c]->xspd)>WORM_BOUNCE_LIMIT)
              {
                if (player[c]->health>0)
                play_sample(bump->snd, *VOLUME, 127, 1000, 0);
                player[c]->xspd*=-1* (*WORM_BOUNCINESS/1000.);
              }
              else player[c]->xspd=0;
            };
          };
          
          if(!cli && player[c]->weap[player[c]->curr_weap].shoot_time>0)player[c]->weap[player[c]->curr_weap].shoot_time--;
          if (player[c]->firecone_time>0) player[c]->firecone_time--;
          
          if (weaps->num[player[c]->weap[player[c]->curr_weap].weap]->ammo>0 && player[c]->weap[player[c]->curr_weap].ammo<1 && !player[c]->weap[player[c]->curr_weap].reloading)
          {
            player[c]->weap[player[c]->curr_weap].reloading=true;
            player[c]->weap[player[c]->curr_weap].reload_time=weaps->num[player[c]->weap[player[c]->curr_weap].weap]->reload_time;
          };
          
          if (player[c]->weap[player[c]->curr_weap].reloading)
            player[c]->weap[player[c]->curr_weap].reload_time--;
          
          if (player[c]->weap[player[c]->curr_weap].reloading && player[c]->weap[player[c]->curr_weap].reload_time==0)
          {
            player[c]->weap[player[c]->curr_weap].reloading=false;
            player[c]->fireing=false;
            player[c]->weap[player[c]->curr_weap].ammo=weaps->num[player[c]->weap[player[c]->curr_weap].weap]->ammo;
            if (weaps->num[player[c]->weap[player[c]->curr_weap].weap]->reload_sound!=NULL)
            play_sample(weaps->num[player[c]->weap[player[c]->curr_weap].weap]->reload_sound->snd, *VOLUME, 127, 1000, 0);
          };
          
          if(srv || player[c]->islocal)
          {
            if (abs(player[c]->aim_speed)>abs(*pl_options[player[c]->local_slot].aim_friction))
            {
              if (player[c]->aim_speed<0) player[c]->aim_speed+=*pl_options[player[c]->local_slot].aim_friction;
              if (player[c]->aim_speed>0) player[c]->aim_speed+=-*pl_options[player[c]->local_slot].aim_friction;
            } else player[c]->aim_speed=0;
            
            if (abs(player[c]->aim_recoil_speed)>abs(*AIM_RECOIL_FRICTION))
            {
              if (player[c]->aim_recoil_speed<0) player[c]->aim_recoil_speed+=*AIM_RECOIL_FRICTION;
              if (player[c]->aim_recoil_speed>0) player[c]->aim_recoil_speed-=*AIM_RECOIL_FRICTION;
            } else player[c]->aim_recoil_speed=0;
            
            player[c]->aim+=player[c]->aim_speed+player[c]->aim_recoil_speed;
          };
          if (player[c]->aim>128000) 
          {
            player[c]->aim=128000;
            player[c]->aim_speed=0;
          };
          if (player[c]->aim<24000) 
          {
            player[c]->aim=24000;
            player[c]->aim_speed=0;
          };
          
          if(!cli && player[c]->air<=0)
          player[c]->health=0;
          
          if(!cli)
          if (player[c]->health<=0)
          {
            char tmpstr[1024];
            if(srv) player[c]->deatheventsend();
            for (o=1;o<14;o++)
            partlist.shoot_part(rand()%1000*255,(rand()%200)+600,1,player[c]->x,player[c]->y-4000,player[c]->xspd/2,player[c]->yspd/2,c,gore);
            play_sample(death->snd, *VOLUME, 127, 1000, 0);
            player[c]->active=false;
            player[c]->ropestate=0;
            player[c]->deaths++;
            player[c]->health=0;
            
            sprintf(tmpstr,"%s DIED",player[c]->name);
            if (player[c]->killed_by==-1)
            {
              sprintf(tmpstr,"%s WAS KILLED BY UNKNOWN FORCES",player[c]->name);
            }else if(player[player[c]->killed_by])
            {
              if(player[c]->killed_by==player[c]->local_slot)
              {
                sprintf(tmpstr,"%s KILLED HIMSELF, WHAT A N00B",player[c]->name);
              } else
              {
                sprintf(tmpstr,"%s WAS KILLED BY %s",player[c]->name,player[player[c]->killed_by]->name);
              };
            }
            con->echolist.add_echo(tmpstr);
          };
        }
        else if (player[c]->health<*MAX_HEALTH) player[c]->health+=10;
        calcrope(player[c]);
      };
      calc_water();
    };
  };
	if (con->flag!=1 && con->pos>0) con->pos-=*con->speed;
	if (con->flag==1 && con->pos<*con->height) con->pos+=*con->speed;
	if (con->flag!=1 && con->pos<0) con->pos=0;
	if (con->flag==1 && con->pos>*con->height) con->pos=*con->height;
  con->echolist.calc();
	
};

/***********************************************************************/
void logfunc(const char *_log)
{
  //sys_print(_log);
}


void engine::init_game()
{
  char tmp[1024];
	int i;
	allegro_init();

  //if(v_depth!=8)
	set_color_depth(v_depth);
  
  if (w==0) set_gfx_mode (GFX_AUTODETECT, v_width, v_height, 0, 0);
	else set_gfx_mode (GFX_AUTODETECT_WINDOWED, v_width, v_height, 0, 0);
	
  if(set_display_switch_mode(SWITCH_BACKAMNESIA) == -1)
    set_display_switch_mode(SWITCH_BACKGROUND);

	if (install_sound(DIGI_AUTODETECT, MIDI_NONE, NULL) != 0) {
		allegro_message("Error initialising sound driver\n%s\n", allegro_error);
	};
	if (digi_card == DIGI_NONE) {
		 allegro_message("Unable to find a sound driver\n%s\n", allegro_error);
	};
  //set_color_conversion(COLORCONV_NONE);
 
  game->weap_count=0;
	fonty=(struct fnt*) malloc(sizeof(struct fnt));
	sprites=new spritelist;
	sounds=new sound_list;
  weaps=new weap_list;
	types=(struct type_list*) malloc(sizeof(struct type_list));
	exp_types=new exp_type_list;
	exps=new exp_list;
  types->init();
	partlist.init();
  water=(struct s_water*) malloc(sizeof(struct s_water)*100000);
  map=new class level;

    
  zcom = new ZoidCom("netlog.txt");
  if (!zcom)
    exit(255);
  if (!zcom->Init())
  {
    //sys_draw();
    //sys_rest(2000);
    exit(255);
  }
	

  
	install_keyboard();
	srand(time(NULL));
  

  
  
  if (v_depth==8)
  {
    get_pallete(pal);
    int h;
    for(h=0;h<256;h++)
    {
      pal[h].r=h/4;
      pal[h].g=h/4;
      pal[h].b=h/4;
    };
    set_palette(pal);
  }else
  {
    if(screen->w>320)
    {
      #ifdef AA2XSAI
      Init_2xSaI(v_depth);
      #endif
      screen_buffer=create_bitmap(screen->w,screen->h);
    };
  };
  //generate_332_palette(pal);
  //set_palette(pal);
  



	
	LOCK_VARIABLE(speed_counter);
	LOCK_FUNCTION(increment_speed_counter);
	install_int_ex(increment_speed_counter, BPS_TO_TIMER(100));
	
	LOCK_VARIABLE(t);
	LOCK_FUNCTION(tick);
	install_int(tick, 10);
	
	con=new struct console;
	con->init();
	con->flag=0;
	con->pos=0;
	con->font=fonty;
	WORM_JUMP_FORCE=con->create_variable("JUMP_FORCE",1200);
	GRAVITY=con->create_variable("GRAVITY",40);
	MAX_SPEED=con->create_variable("MAX_SPEED",1000);
	ACELERATION=con->create_variable("ACCELERATION",100);
	FRICTION=con->create_variable("FRICTION",50);
	AIR_FRICTION=con->create_variable("AIR_FRICTION",3);
	MAX_HEALTH=con->create_variable("MAX_HEALTH",1000);
	WORM_HEIGHT=90;
	WORM_BOUNCINESS=con->create_variable("BOUNCINESS",200);
	WORM_BOUNCE_LIMIT=1200;
	MAP_SHOW_MODE=con->create_variable("MAP_SHOW_MODE",0);
	RENDER_LAYERS=con->create_variable("RENDER_LAYERS",1);	
	VOLUME=con->create_variable("VOLUME",128);
	DAMAGE_SPEED=con->create_variable("DAMAGE_SPEED",5000);
	FALL_DAMAGE=con->create_variable("FALL_DAMAGE",500);
  RESPAWN_RELOAD=con->create_variable("RESPAWN_RELOAD",1);
  ROPE_STRENTH=con->create_variable("ROPE_STRENTH",80);
  ROPE_LENGHT=con->create_variable("ROPE_LENGHT",35000);
  AIM_RECOIL_FRICTION=con->create_variable("AIM_RECOIL_FRICTION",250);
  AIR_CAPACITY=con->create_variable("AIR_CAPACITY",1500);
  FLASHLIGHT=con->create_variable("FLASHLIGHT",0);
  ROPE_GRAVITY=con->create_variable("ROPE_GRAVITY",20);
  HOST=con->create_variable("HOST",0);
  VIDEO_FILTER=con->create_variable("V_FILTER",0);
  SPLIT_SCREEN=con->create_variable("SPLITSCREEN",1);
  TEAMPLAY=con->create_variable("TEAMPLAY",0);

	con->add_cmd("EXIT",quit);
	con->add_cmd("QUIT",quit);
	con->add_cmd("MAP",change_level); 
	con->add_cmd("EXEC",execute_config);
	con->add_cmd("BIND",bind);
  con->add_cmd("ECHO",echo);
	con->add_cmd("UNBIND",unbind);	
	con->add_cmd("P1_RIGHT",pl1_moveright);
	con->add_cmd("P1_LEFT",pl1_moveleft);
	con->add_cmd("P1_UP",pl1_aimup);
	con->add_cmd("P1_DOWN",pl1_aimdown);
	con->add_cmd("P1_JUMP",pl1_jump);
	con->add_cmd("P1_FIRE",pl1_fire);
	con->add_cmd("P1_CHANGE",pl1_change);
  con->add_cmd("P1_RELOAD",pl1_reload);
	con->add_cmd("P0_RIGHT",pl0_moveright);
	con->add_cmd("P0_LEFT",pl0_moveleft);
	con->add_cmd("P0_UP",pl0_aimup);
	con->add_cmd("P0_DOWN",pl0_aimdown);
	con->add_cmd("P0_JUMP",pl0_jump);
	con->add_cmd("P0_FIRE",pl0_fire);
	con->add_cmd("P0_CHANGE",pl0_change);
	con->add_cmd("P0_RELOAD",pl0_reload);
  con->add_cmd("CONNECT",connect);
  con->add_cmd("SAY", send_msg);
  con->add_cmd("P0_COLOR", pl0_color);
  con->add_cmd("P1_COLOR", pl1_color);
  con->add_cmd("P0_NICK", pl0_nick);
  con->add_cmd("P1_NICK", pl1_nick);
  con->add_cmd("P0_TEAM", pl0_team);
  con->add_cmd("P1_TEAM", pl1_team);
  con->add_cmd("SAVE_LOG", save_log);
  
  if(v_depth==8)
    fonty->ld_fnt_8("default/fonts/minifont.bmp");
  else fonty->ld_fnt("default/fonts/minifont.bmp");
	

  
  
	buffer=create_bitmap(320,240);
	
	

	file=new struct al_ffblk;
	
  strcpy(tmp,game->mod);
  strcat(tmp,"/weapons/*.wpn");
	al_findfirst(tmp, file, FA_ARCH);
	do
	{
		load_weap(file->name);
	} while(al_findnext(file)==0);
	
	delete file;
  
	gore=load_part("gore.obj");
  chreact=load_part("chreact.obj");
	death=sounds->load("death2.wav");
	respawn=sounds->load("alive.wav");
	throwrope=sounds->load("throw.wav");
	bump=sounds->load("bump.wav");
  breath=sounds->load("breath.wav");
  gstart=sounds->load("start.wav");
  menu_move=sounds->load("menu_move.wav");
  menu_select=sounds->load("menu_select.wav");
	health=sprites->load_sprite("health.bmp",1,game->mod,game->v_depth);
  ammo=sprites->load_sprite("ammo.bmp",1,game->mod,game->v_depth);
	death_img=sprites->load_sprite("kills.bmp",1,game->mod,game->v_depth);
	hook=sprites->load_sprite("hook.bmp",1,game->mod,game->v_depth);
  firecone=sprites->load_sprite("firecone.bmp",7,game->mod,game->v_depth);
	worm_hole=load_exp("worm_hole.obj");
  
  init_players();
  teamplay=false;
    
	for (i=0;i<2;i++)
	{
    int o;
		player[i] = (struct worm*) malloc(sizeof(struct worm));
    player[i]->weap = (struct s_playerweap*) malloc(sizeof(struct s_playerweap)*5);
		player[i]->view=create_bitmap(160,240);
		player[i]->x=80*1000;
		player[i]->y=40*1000;
		player[i]->xspd=0;
		player[i]->yspd=0;
		player[i]->aim=64000;
		player[i]->dir=1;
		player[i]->crossr=20;
		player[i]->health=1000;
		player[i]->deaths=0;
    player[i]->air=2000;
		player[i]->curr_weap=0;
    player[i]->aim_speed=0;
    for(o=0;o<5;o++)
    {
      player[i]->weap[o].weap=0;
      player[i]->weap[o].shoot_time=0;
      player[i]->weap[o].ammo=weaps->num[player[i]->weap[o].weap]->ammo;
      player[i]->weap[o].reloading=false;
    };
		player[i]->curr_frame=2700;
		player[i]->crosshair=sprites->load_sprite("crosshair.bmp",1,game->mod,game->v_depth);

		player[i]->active=true;
		player[i]->flag=false;
    player[i]->islocal=true;
    player[i]->deleteme=false;
    player[i]->local_slot=local_players;
    player[i]->selecting_weaps=true;
		player[i]->ropestate=0;
		player[i]->ropex=1;
		player[i]->ropey=1;
    
    player[i]->curr_firecone=NULL;
    player[i]->firecone_time=0;
    
    player[i]->node=NULL;
    
    local_player[i]=i;
    strcpy(pl_options[i].name,"PLAYER");
    strcpy(player[i]->name,"PLAYER");
    local_players++;
    player_count++;

    
	};
	
  pl_options[1].aim_acceleration=con->create_variable("P1_AIM_ACCEL",100);
  pl_options[1].aim_friction=con->create_variable("P1_AIM_FRICTION",50);
  pl_options[1].aim_maxspeed=con->create_variable("P1_AIM_MAX_SPEED",1200);
	player[1]->skin=sprites->load_sprite("lskinb.bmp",21,game->mod,game->v_depth);
  player[1]->mask=sprites->load_sprite("lskinmask.bmp",21,game->mod,game->v_depth);
  player[1]->color=makecol(rand()%255,rand()%255,rand()%255);
  player[0]->color=makecol(rand()%255,rand()%255,rand()%255);
  player[0]->mask=sprites->load_sprite("lskinmask.bmp",21,game->mod,game->v_depth);

	player[1]->keys=new struct KEYS;
	player[1]->keys->up=false;
	player[1]->keys->down=false;
	player[1]->keys->right=false;
	player[1]->keys->left=false;
	player[1]->keys->jump=false;
	player[1]->keys->fire=false;
	player[1]->keys->change=false;
	pl_options[1].HEALTH_DRAW_MODE=con->create_variable("P1_HEALTH_MODE",0);	
  viewport[1].w=320/2-1;
  viewport[1].h=240;
  viewport[1].x=viewport[1].w+2;
	viewport[1].y=0;

	pl_options[0].aim_maxspeed=con->create_variable("P0_AIM_MAX_SPEED",1200);
  pl_options[0].aim_acceleration=con->create_variable("P0_AIM_ACCEL",100);
  pl_options[0].aim_friction=con->create_variable("P0_AIM_FRICTION",50);
	player[0]->skin=sprites->load_sprite("lskinb.bmp",21,game->mod,game->v_depth);
	player[0]->keys=new struct KEYS;
	player[0]->keys->up=false;
	player[0]->keys->down=false;
	player[0]->keys->right=false;
	player[0]->keys->left=false;
	player[0]->keys->jump=false;
	player[0]->keys->fire=false;
	player[0]->keys->change=false;
	pl_options[0].HEALTH_DRAW_MODE=con->create_variable("P0_HEALTH_MODE",0);
  viewport[0].w=320/2-1;
  viewport[0].h=240;
  viewport[0].x=0;
	viewport[0].y=0;
  
	
	strcpy(con->arg,game->level);
	change_level();
	
  set_color_conversion(COLORCONV_TOTAL|COLORCONV_DITHER);
	strcpy(con->arg,"autoexec.cfg");
	execute_config();
  set_color_conversion(COLORCONV_TOTAL);
  
};
