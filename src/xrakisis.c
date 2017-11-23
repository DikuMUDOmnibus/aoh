#include <sys/types.h>
#include <sys/time.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>   // we need these to call wait()

#include <execinfo.h>   // internal backtraces.

#include "merc.h"

     void add_affect ( OBJ_DATA * obj, int apply, long value )
        {
        AFFECT_DATA * paf;

        if ( affect_free == NULL )
        paf = alloc_perm( sizeof( * paf ) );
        else
        {
        paf         = affect_free;
        affect_free = affect_free->next;
        }
        paf->type      = 0;
        paf->duration  = -1;
        paf->location  = apply;
        paf->modifier  = value;
        paf->bitvector = 0;
        paf->next      = obj->affected;
        obj->affected  = paf;
        }


void do_calc(CHAR_DATA *ch, char *argument)
{
  char      buf  [MAX_STRING_LENGTH];
  char      arg1 [MAX_INPUT_LENGTH];
  char      arg2 [MAX_INPUT_LENGTH];
  char      arg3 [MAX_INPUT_LENGTH];

  double value1=0,value2=0,result;

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  argument = one_argument( argument, arg3 );

    value1 = is_number( arg1 ) ? atoi( arg1 ) : -1;
    value2 = is_number( arg3 ) ? atoi( arg3 ) : -1;

sprintf(buf,"value1: %2.0f value2: %2.0f. \n\r", value1,value2);
send_to_char(buf,ch);


if (!str_cmp(arg2,"x")) // mult
{
result = value1 * value2;
sprintf(buf,"%2.0f X %2.0f = %2.0f\n\r", value1,value2,result);
send_to_char(buf,ch);
return;
}

else if (!str_cmp(arg2,"/")) // division
{
result = value1 / value2;
sprintf(buf,"%2.0f / %2.0f = %2.0f.\n\r", value1,value2,result);
send_to_char(buf,ch);
return;
}

else if (!str_cmp(arg2,"+")) // addition
{
result = value1 + value2;
sprintf(buf,"%2.0f + %2.0f = %2.0f.\n\r", value1,value2,result);
send_to_char(buf,ch);
return;
}

else if (!str_cmp(arg2,"-")) // subtraction
{
result = value1 - value2;
sprintf(buf,"%2.0f - %2.0f = %2.0f.\n\r", value1,value2,result);
send_to_char(buf,ch);
return;
}

else
{
send_to_char("Danger Will Robinson!",ch);
return;
}
return;
}


void do_toggle( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];             

  argument = one_argument( argument, arg );
  argument = one_argument(argument, arg2);

    if (!strcmp(argument, "exp"))
  {
            send_to_char("Syntax: toggle <exp> <on|off>.\n\r",ch);
    		return;
  }      
  if (!str_cmp(arg, "exp"))
  {
      if (!str_cmp(arg2, "on"))  
	{
	if (double_exp)
    {
            send_to_char("Double exp is already in affect!\n\r",ch);
            return;
        }
        double_exp = TRUE;
	do_info( ch, "#gDouble Experience is #WON#n!!!!" );

      return;
    }
    if (!str_cmp(arg2, "off"))
    {
        if (!double_exp)
    {
            send_to_char("The global exp flag isn't on!.\n\r",ch);
      return;
    }
        double_exp = FALSE; 
    do_info( ch, "#gDouble exp is now #WOFF#n" );

        return;
    }}
  if (!str_cmp(arg, "stance"))
  {
      if (!str_cmp(arg2, "on"))
        {
        if (double_fightstance)
    {
            send_to_char("Double FightStance is already in affect!\n\r",ch);
            return;
        }
        double_fightstance = TRUE;
        do_info( ch, "#gDouble FightStance is #WON#n!!!!" );

      return;
    }
    if (!str_cmp(arg2, "off"))
    {
        if (!double_fightstance)
    {
            send_to_char("The global stance flag isn't on!.\n\r",ch);
      return;
    }
        double_fightstance = FALSE;
    do_info( ch, "#gDouble FightStance is now #WOFF#n" );

        return;
    }}
  if (!str_cmp(arg, "coins"))
  {
      if (!str_cmp(arg2, "on"))  
	{
	if (double_coins)
    {
            send_to_char("Double coins is already in affect!\n\r",ch);
            return;
        }
        double_coins = TRUE;
	do_info( ch, "#gDouble Coins is #WON#n!!!!" );

      return;
    }
    if (!str_cmp(arg2, "off"))
    {
        if (!double_coins)
    {
            send_to_char("The global coins flag isn't on!.\n\r",ch);
      return;
    }
        double_coins = FALSE; 
    do_info( ch, "#gDouble coins is now #WOFF#n" );

        return;
    }}
  

return;
}

void do_resetsphere (CHAR_DATA * ch, char *argument)
{
    send_to_char ("You must type the full command to reset your Spheres.\n\r",
                  ch);
}


void do_resetspheres(CHAR_DATA * ch, char *argument)
{

        if (IS_NPC(ch))
        return;


	ch->pcsphere = 0;
	ch->pcsphereammount = 0;
	send_to_char("your spheres have been reset\n\r", ch);

	return;
}

void do_wizlist( CHAR_DATA *ch, char *argument )
{
    do_help( ch, "wizlist" );
    return;
}






const sh_int  revdir [6] =
{
    2, 3, 0, 1, 5, 4
};

void put_wall(ROOM_INDEX_DATA *room, int dir, int wall)
{
    int timer;

    timer = dice(2, 2);

	if(wall == EX_SHIN_WALL) timer = 30;

    switch (dir)
    {
	case DIR_NORTH :
	    SET_RTIMER(room, RTIMER_WALL_NORTH, timer);
	    SET_RTIMER(room->exit[dir]->to_room, RTIMER_WALL_SOUTH, timer);
	break;
	case DIR_EAST :
	    SET_RTIMER(room, RTIMER_WALL_EAST, timer);
	    SET_RTIMER(room->exit[dir]->to_room, RTIMER_WALL_WEST, timer);
	break;
	case DIR_SOUTH :
	    SET_RTIMER(room, RTIMER_WALL_SOUTH, timer);
	    SET_RTIMER(room->exit[dir]->to_room, RTIMER_WALL_NORTH, timer);
	break;
	case DIR_WEST :
	    SET_RTIMER(room, RTIMER_WALL_WEST, timer);
	    SET_RTIMER(room->exit[dir]->to_room, RTIMER_WALL_EAST, timer);
	break;
	case DIR_UP :
	    SET_RTIMER(room, RTIMER_WALL_UP, timer);
	    SET_RTIMER(room->exit[dir]->to_room, RTIMER_WALL_DOWN, timer);
	break;
	case DIR_DOWN :
	    SET_RTIMER(room, RTIMER_WALL_DOWN, timer);
	    SET_RTIMER(room->exit[dir]->to_room, RTIMER_WALL_UP, timer);
	break;
    }   
    room->exit[dir]->exit_info = wall;

}



void make_wall(ROOM_INDEX_DATA *room, int dir, int wall)
{
    int was;
    bool makewall = TRUE;

    if (dir < 0 || dir > 5)
    {
	bug("Invalid direction (create wall)", 0);
	return;
    }

    if (room->exit[dir] == NULL)
    {
	bug ("Cant make wall!!!!", 0);
	return;
    }

    was = room->exit[dir]->exit_info;

    if (was == EX_ICE_WALL && wall == EX_FIRE_WALL)
    {
	room_message(room, "The wall of ice bursts into flames.");
	room_message(room->exit[dir]->to_room, "The wall of ice bursts into flames.");
    }

    if (was == EX_FIRE_WALL && wall == EX_ICE_WALL)
    {
	room_message(room, "The wall of flames flickers blue for a second.");
	room_message(room->exit[dir]->to_room, "The wall of flames flickers blue for a second.");
	makewall = FALSE;
    }

    if (makewall) put_wall(room, dir, wall);

	room->next_update = update_room_list;
	update_room_list = room;

    if (room->exit[dir]->to_room == NULL)
    {
	bug("Exit doesnt lead anywhere (create wall).", 0);
	return;
    }

    if (room->exit[dir]->to_room->exit[revdir[dir]] == NULL)
    {
	bug("Reverse Exit doesnt lead anywhere (create wall).", 0);
	return;
    }
	room->exit[dir]->to_room->next_update = update_room_list;
	update_room_list = room->exit[dir]->to_room;
	
    if (makewall) put_wall(room->exit[dir]->to_room, revdir[dir], wall);
}

ROOM_INDEX_DATA * locate_obj (OBJ_DATA *obj)
{
    if (obj->in_obj != NULL)
	return locate_obj(obj->in_obj);

    if (obj->carried_by != NULL)
	return obj->carried_by->in_room;

    return obj->in_room;
}






void do_powers(CHAR_DATA * ch, char *argument)
{

	if (IS_NPC(ch))
	return;

	send_to_char("[--------------------[Sphere Powers]--------------------]\n\r", ch);
        if (IS_SET(ch->pcsphere, SPHERE_PYROMANCER))
	{
	send_to_char("#R[#0Pyromancer#R]#n Flameblade, Firebreath, Magma, Enflame, Jutsus\n\r", ch);
	}
        if (IS_SET(ch->pcsphere, SPHERE_HYDROMANCER))
        {
        send_to_char("#R[#0Hydromancer#R]#n Hydro, Cbreath, Frosthands, Jutsus\n\r", ch);
        }
        if (IS_SET(ch->pcsphere, SPHERE_ABJURER))
        {
        send_to_char("#R[#0Abjurer#R]#n DimensionDodge, holyaura, Shield\n\r", ch);
	}
        if (IS_SET(ch->pcsphere, SPHERE_TRANSMUTER))
        {
        send_to_char("#R[#0Transmuter#R]#n Zuloform, Change, Dragonform, Mask, Camouflage, Bonemod\n\r", ch);
	}
        if (IS_SET(ch->pcsphere, SPHERE_ENCHANTMENT))
        {
        send_to_char("#R[#0Enchantment#R]#n Weaponenhance, Armorenhance, Caust, Immolate, Freezeweapon, Poisonblade\n\r", ch);
	}
        if (IS_SET(ch->pcsphere, SPHERE_DIVINER))
        {
        send_to_char("#R[#0Diviner#R]#n Scry, Readaura, Gleam, Rreveal\n\r", ch);
        }
        if (IS_SET(ch->pcsphere, SPHERE_ILLUSIONIST))
        {
        send_to_char("#R[#0Illusionist#R]#n Clones, Formillusion, Shadowplane, Shadowsight, Grab\n\r", ch);
        }
        if (IS_SET(ch->pcsphere, SPHERE_NECROMANCER))
        {
        send_to_char("#R[#0Necromancer#R]#n Creepingdoom, Fleshportal, Powerword, Darksummon\n\r", ch);
        }
        if (IS_SET(ch->pcsphere, SPHERE_GEOMANCER))
        {
        send_to_char("#R[#0Geomancer#R]#n Solidift, Rockslide, Earthshatter, Earthswallow, Jutsus\n\r", ch);
	}
        if (IS_SET(ch->pcsphere, SPHERE_AEROMANCER))
        {
        send_to_char("#R[#0Aeromancer#R]#n Disperse, Enfold, Evaporate, Jutsus\n\r", ch);
        }
        if (IS_SET(ch->pcsphere, SPHERE_WOKANIST))
        {
        send_to_char("#R[#0Wokanist#R]#n Elemshift, Gasbreath, Naturesfury\n\r", ch);
        }
        if (IS_SET(ch->pcsphere, SPHERE_ANIMAL))
        {
        send_to_char("#R[#0Animal#R]#n Claws, Sclaws, Fangs, Rend, Talons, Burrow\n\r", ch);
	}
        if (IS_SET(ch->pcsphere, SPHERE_HEALING))
        {
        send_to_char("#R[#0Healing#R]#n Heal, Regenerate, Regrow\n\r", ch);
        }
        if (IS_SET(ch->pcsphere, SPHERE_ENHANCEMENT))
        {
        send_to_char("#R[#0Enhancement#R]#n Nightsight, Graft, Darktendrils\n\r", ch);
        }
        if (IS_SET(ch->pcsphere, SPHERE_CHAOS))
        {
        send_to_char("#R[#0Chaos#R]#n Chaossurge, Chaosgate, Chaos, Chaosmagic, Chaosshield\n\r", ch);
        }        
	if (IS_SET(ch->pcsphere, SPHERE_COMBAT1))
        {
        send_to_char("#R[#0Combat1#R]#n Leech, Bladejump, Ironclaw, Kakusu, Fightdance\n\r", ch);
        }
	if (IS_SET(ch->pcsphere, SPHERE_COMBAT2))
        {
        send_to_char("#R[#0Combat2#R]#n Bomuzite, Superberserk, Shouldercharge\n\r", ch);
        }
	if (IS_SET(ch->pcsphere, SPHERE_COMBAT3))
        {
        send_to_char("#R[#0Combat3#R]#n Entomb, Spikes, Unnerve, Dtrick\n\r", ch);
        }
	if (IS_SET(ch->pcsphere, SPHERE_COMBAT4))
        {
        send_to_char("#R[#0Combat4#R]#n Desanc, Shatter, Enrage\n\r", ch);
        }
	if (IS_SET(ch->pcsphere, SPHERE_ANIMAL2))
        {
        send_to_char("#R[#0Animal2#R]#n Quills, Roar, Flameclaws, Skin\n\r", ch);
        }
	if (IS_SET(ch->pcsphere, SPHERE_SHADOW))
        {
        send_to_char("#R[#0Shadow#R]#n Jutsus\n\r", ch);
        }
	if (IS_SET(ch->pcsphere, SPHERE_HOLY))
        {
        send_to_char("#R[#0Holy#R]#n Jutsus\n\r", ch);
        }
return;
}




void do_sell(CHAR_DATA * ch, char *argument)
{
        ROOM_INDEX_DATA *room;
        OBJ_DATA *obj;
        OBJ_DATA *obj_next;
        bool      found;
        char      arg[MAX_INPUT_LENGTH];
        char      buf[MAX_STRING_LENGTH];
        int       value = 0, total = 0, counter = 0;
        CHAR_DATA *keeper;


        argument = one_argument(argument, arg);
        if (IS_NPC(ch))
                return;
        room = ch->in_room;
        if (room == NULL)
                return;

  for (keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room)
  {
  {
    if (keeper == ch) continue;
    if (!keeper->shop_fun) continue;
    (*keeper->shop_fun)(keeper, ch, buf);
    return;
  }
                stc("You aren't at a shop.\n\r", ch);
                return;
        }

        if (arg[0] == '\0')
        {
                stc("Sell what?", ch);
                return;
        }
        if (str_cmp(arg, "all") && str_prefix("all.", arg))
        {
                if ((obj = get_obj_carry(ch, arg)) == NULL)
                {
                        stc("You don't seem to have that item!\n\r", ch);
                        return;
                }

                if (obj->questowner != NULL && strlen(obj->questowner) > 1
                    && str_cmp(ch->pcdata->switchname, obj->questowner))
                {
                        stc("You can't sell that! It belongs to someone else!\n\r", ch);
                        return;
                }
/*                if (IS_SET(obj->extra_flags2, ITEM_NO_SELL))
                {
                        stc("You are not allowed to sell that item!\n\r", ch);
                        return;
                }
*/
                if (obj->cost > 0)
                {

                        if (obj->item_type == ITEM_TREASURE)
                        {
                                value = obj->cost;
                                //value += number_range(obj->cost * 0.1, obj->cost * 0.3);
                        }

                        else
                        {
                                value = obj->cost / 2;

                                if (number_range(1, 2) == 2)
                                        value += number_range(value * 0.1,
                                                              value * 0.3);
                                else
                                        value -= number_range(value * 0.1,
                                                              value * 0.3);

                        }

                        sprintf(buf, "You sell $p for %d gold pieces!",
                                value);
                        act(buf, ch, obj, NULL, TO_CHAR);
                        act("$n sells $p to the shopkeeper!", ch, obj, NULL,
                            TO_ROOM);
                        extract_obj(obj);
                        ch->pcgold += value;
                        return;
                }
                else
                {
                        stc("That object is worthless!\n\r", ch);
                        return;
                }
                return;
        }
        else
        {
                found = FALSE;
                for (obj = ch->carrying; obj != NULL; obj = obj_next)
                {
                        obj_next = obj->next_content;
                        if (arg[3] != '\0' && !is_name(&arg[4], obj->name))
                                continue;
                        if (!can_see_obj(ch, obj)
                            || obj->wear_loc != WEAR_NONE)
                                continue;
                        if (obj->questowner != NULL
                            && strlen(obj->questowner) > 1
                            && str_cmp(ch->pcdata->switchname,
                                       obj->questowner))
                                continue;
//                        if (IS_SET(obj->extra_flags2, ITEM_NO_SELL))
  //                              continue;
                        if (obj->cost < 1)
                                continue;
                        if (obj->item_type == ITEM_TREASURE)
                                value = obj->cost;
                        else
                        {
                                value = obj->cost / 2;
                                if (number_range(1, 2) == 2)
                                        value += number_range(value * 0.1,
                                                              value * 0.3);
                                else
                                        value -= number_range(value * 0.1,
                                                              value * 0.3);
                        }
                        if (value < 1)
                                value = 1;
                        total += value;
                        extract_obj(obj);
                        found = TRUE;
                        counter += 1;
                }
                if (found)
                {
                        if (counter > 1)
                        {
                                act("$n sells several items to the shopkeeper!", ch, NULL, NULL, TO_ROOM);
                                ch->pcgold += total;
                                act("You sell several items to the shopkeeper!", ch, NULL, NULL, TO_CHAR);
                                sprintf(buf,
                                        "You make %d gold from your sells!\n\r",
                                        total);
                                stc(buf, ch);
                        }
                        else
                        {
                                act("$n sells an item to the shopkeeper!", ch,
                                    NULL, NULL, TO_ROOM);
                                ch->pcgold += total;
                                sprintf(buf,
                                        "You sell an item to the shopkeeper for %d gold\n\r!",
                                        total);
                                act(buf, ch, NULL, NULL, TO_CHAR);
                        }
                }
                else
                {
                        stc("Either you don't have them or the shopkeeper doesn't want them.\n\r", ch);
                        return;
                }
        }
        return;
}



void do_convert ( CHAR_DATA *ch, char *argument) // Recoded by NebuSoft
{
  char arg1[MAX_INPUT_LENGTH];

  argument = one_argument( argument, arg1);

  if (IS_NPC(ch)) return;

  if (arg1[0] == '\0' || (str_cmp(arg1,"silver") && str_cmp(arg1,"copper")))
  {
    send_to_char("convert <silver|copper>\n\r",ch);
    return;
  }
  if (!str_cmp(arg1,"silver"))
  { 
    if ( ch->pcsilver < 20000 )
    {
      send_to_char("You need at least 20000 silver to convert.\n\r",ch);
      return;
    }
    ch->pcgold += 2000;
    ch->pcsilver -= 20000;
    send_to_char("You convert 20000 silver into 2000 gold.\n\r",ch);
  }
  else if (!str_cmp(arg1,"copper"))
  { 
    if ( ch->pccopper < 100000 )
    {
      send_to_char("You need at least 100,000 copper to convert.\n\r",ch);
      return;
    }
    
    ch->pccopper -= 100000;
    ch->pcgold += 1000;
    send_to_char("You convert 100,000 Copper into 1000 gold.\n\r",ch);
  }
  return;
}





void do_proficiencies(CHAR_DATA * ch, char *argument)
{
        char      buf[MAX_STRING_LENGTH];

	if (IS_NPC(ch))
	return;

        send_to_char("[--------------------[Proficiencies]--------------------]\n\r", ch);
        sprintf( buf, "#0Unarmed #R[#0%d#R]		#0Aikido: #R[#0%d#R]#n\n\r#n", ch->wpn[0], ch->pcstyle[1]);
        send_to_char( buf, ch );
        sprintf( buf, "#0Knife: #R[#0%d#R]		#0Jiujitsu: #R[#0%d#R]#n\n\r#n", ch->wpn[1], ch->pcstyle[2]);
        send_to_char( buf, ch );
        sprintf( buf, "#0Spear: #R[#0%d#R]		#0Kendo: #R[#0%d#R]#n\n\r#n", ch->wpn[2], ch->pcstyle[3]);
        send_to_char( buf, ch );
        sprintf( buf, "#0Shortsword: #R[#0%d#R]\n\r#n", ch->wpn[3]);
        send_to_char( buf, ch );
        sprintf( buf, "#0Staff: #R[#0%d#R]\n\r#n", ch->wpn[4]);
        send_to_char( buf, ch );
        sprintf( buf, "#0Broadsword: #R[#0%d#R]\n\r#n", ch->wpn[5]);
        send_to_char( buf, ch );
        sprintf( buf, "#0Longsword: #R[#0%d#R]\n\r#n", ch->wpn[6]);
        send_to_char( buf, ch );
        sprintf( buf, "#0Bastard Sword: #R[#0%d#R]\n\r#n", ch->wpn[7]);
        send_to_char( buf, ch );
        sprintf( buf, "#0Two-Handed Longsword: #R[#0%d#R]\n\r#n", ch->wpn[8]);
        send_to_char( buf, ch );
        sprintf( buf, "#0Axe: #R[#0%d#R]\n\r#n", ch->wpn[9]);
        send_to_char( buf, ch );
        sprintf( buf, "#0Mace: #R[#0%d#R]\n\r#n", ch->wpn[10]);
        send_to_char( buf, ch );
        sprintf( buf, "#0Whip: #R[#0%d#R]\n\r#n", ch->wpn[11]);
        send_to_char( buf, ch );
        sprintf( buf, "#0Warhammer: #R[#0%d#R]\n\r#n", ch->wpn[12]);
        send_to_char( buf, ch );
        sprintf( buf, "#0Flail: #R[#0%d#R]\n\r#n", ch->wpn[13]);
        send_to_char( buf, ch );
        sprintf( buf, "#0Morningstar: #R[#0%d#R]\n\r#n", ch->wpn[14]);
        send_to_char( buf, ch );
        sprintf( buf, "#0Polearm: #R[#0%d#R]\n\r#n", ch->wpn[15]);
        send_to_char( buf, ch );
        sprintf( buf, "#0Club: #R[#0%d#R]\n\r#n", ch->wpn[16]);
        send_to_char( buf, ch );
        sprintf( buf, "#0Sickle: #R[#0%d#R]\n\r#n", ch->wpn[17]);
        send_to_char( buf, ch );
        sprintf( buf, "#0Dagger: #R[#0%d#R]\n\r#n", ch->wpn[18]);
        send_to_char( buf, ch );
        send_to_char("[--------------------[Proficiencies]--------------------]\n\r", ch);
	return;
}




void improve_str(CHAR_DATA * ch)
{
        int       dice1;
        int       dice2;

        dice1 = number_percent();
        dice2 = number_percent();

        if (IS_NPC(ch))
                return;

  if (ch->pcdata->perm_str >=25) return;
  if (dice1 >= 98 || dice2 >= 99)
   {
   send_to_char("#0Your Strength increases by #R[#0ONE#R]#0 point.#n \n\r",ch);
   ch->pcdata->perm_str++;
   }
return;
}


void improve_dex(CHAR_DATA * ch)
{
        int       dice1;
        int       dice2;

        dice1 = number_percent();
        dice2 = number_percent();

        if (IS_NPC(ch))
                return;

  if (ch->pcdata->perm_dex >=25) return;
  if (dice1 >= 98 || dice2 >= 99)
   {
   send_to_char("#0Your Dexterity increases by #R[#0ONE#R]#0 point.#n \n\r",ch);
   ch->pcdata->perm_dex++;
   }
return;
}

void improve_con(CHAR_DATA * ch)
{
        int       dice1;
        int       dice2;

        dice1 = number_percent();
        dice2 = number_percent();

        if (IS_NPC(ch))
                return;

  if (ch->pcdata->perm_con >=25) return;
  if (dice1 >= 98 || dice2 >= 99)
   {
   send_to_char("#0Your Constitution increases by #R[#0ONE#R]#0 point.#n \n\r",ch);
   ch->pcdata->perm_con++;
   }
return;
}


void improve_int(CHAR_DATA * ch)
{
        int       dice1;
        int       dice2;

        dice1 = number_percent();
        dice2 = number_percent();

        if (IS_NPC(ch))
                return;

  if (ch->pcdata->perm_int >=25) return;
  if (dice1 >= 98 || dice2 >= 99)
   {
   send_to_char("#0Your Intelligence increases by #R[#0ONE#R]#0 point.#n \n\r",ch);
   ch->pcdata->perm_int++;
   }
return;
}

void improve_wis(CHAR_DATA * ch)
{
        int       dice1;
        int       dice2;

        dice1 = number_percent();
        dice2 = number_percent();

        if (IS_NPC(ch))
                return;

  if (ch->pcdata->perm_wis >=25) return;
  if (dice1 >= 98 || dice2 >= 99)
   {
   send_to_char("#0Your Wisdom increases by #R[#0ONE#R]#0 point.#n \n\r",ch);
   ch->pcdata->perm_wis++;
   }
return;
}


void do_selfsphere(CHAR_DATA *ch, char *argument)
{
  char arg1[MAX_STRING_LENGTH];
  argument = one_argument( argument, arg1 );

  if (IS_NPC(ch))return;

  if (ch->level < 3)
  {
    send_to_char("#0You must be avatar to selfsphere.#n\n\r",ch);
    return;
  }
if ( arg1[0] == '\0' )
{
  send_to_char("Spheres: Type selfsphere spherename to select a sphere.\n\r\n\r",ch);
  send_to_char("Spheres: You only get 5 + 1 per remort, so choose wisely.\n\r\n\r",ch);
  send_to_char("Spheres: You may have any assortment of sphers, including 2 level 200 sphers.\n\r\n\r",ch);


send_to_char("#0[------------------------------------------------------]\n\r",ch);
send_to_char("#0Race Level One Spheres \n\r",ch);
send_to_char("#0Combat1 - Animal1 - Wokanist - Necromancer \n\r",ch);
send_to_char("#0[------------------------------------------------------]\n\r",ch);
send_to_char("#0Race Level 50 Spheres \n\r",ch);
send_to_char("#0Pyromancer - Healing - Diviner - Transmuter\n\r",ch);
send_to_char("#0Chaos - Enchantment\n\r",ch);
send_to_char("#0[------------------------------------------------------]\n\r",ch);
send_to_char("#0Race Level 100 Spheres \n\r",ch);
send_to_char("#0Combat2 - Illusionist - Geomancer - Holy\n\r",ch);
send_to_char("#0Shadow - Moon\n\r",ch);
send_to_char("#0[------------------------------------------------------]\n\r",ch);
send_to_char("#0Race Level 150 Spheres \n\r",ch);
send_to_char("#0Combat3 -  Abjurer - Hydromancer - Enhancement\n\r",ch);
send_to_char("#0Aeromancer  \n\r",ch);
send_to_char("#0[------------------------------------------------------]\n\r",ch);
send_to_char("#0Race Level 200 Spheres \n\r",ch);
send_to_char("#0Combat4 - Animal2  \n\r",ch);
send_to_char("#0[------------------------------------------------------]\n\r",ch);
  return;
}
        if (ch->pcsphereammount > 4 && ch->pcremort == 0)
        {
        send_to_char( "#0You are only allowed 5 spheres.#n\n\r", ch);
        return;
        }
	else if (ch->pcremort == 1 && ch->pcsphereammount > 5)
        {
        send_to_char( "#0You are only allowed 6 spheres.#n\n\r", ch);
        return;
        }
        else if (ch->pcremort == 2 && ch->pcsphereammount > 6)
        {
        send_to_char( "#0You are only allowed 7 spheres.#n\n\r", ch);
        return;
        }
        else if (ch->pcremort == 3 && ch->pcsphereammount > 7)
        {
        send_to_char( "#0You are only allowed 8 spheres.#n\n\r", ch);
        return;
        }
       else if (ch->pcremort == 4 && ch->pcsphereammount > 8)
        {
        send_to_char( "#0You are only allowed 8 spheres.#n\n\r", ch);
        return;
        }
       else if (ch->pcremort >= 5 && ch->pcsphereammount > 9)
        {
        send_to_char( "#0You are only allowed 8 spheres.#n\n\r", ch);
        return;
        }



if (!str_cmp(arg1,"combat1"))
{
	if (IS_SET(ch->pcsphere, SPHERE_COMBAT1))
	{
	send_to_char( "#0You Already have this sphere.#n\n\r", ch);
	return;
	}
	SET_BIT(ch->pcsphere, SPHERE_COMBAT1);
	send_to_char( "#0You have begun your studies in the Combat One Sphere.#n\n\r", ch);
	ch->pcsphereammount++;
	return;
}
else if (!str_cmp(arg1,"animal1"))
{
        if (IS_SET(ch->pcsphere, SPHERE_ANIMAL))
        {
        send_to_char( "#0You Already have this sphere.#n\n\r", ch);
        return;
        }
        SET_BIT(ch->pcsphere, SPHERE_ANIMAL);
        send_to_char( "#0You have begun your studies in the Animal One Sphere.#n\n\r", ch);
        ch->pcsphereammount++;
        return;
}
else if (!str_cmp(arg1,"wokanist"))
{
        if (IS_SET(ch->pcsphere, SPHERE_WOKANIST))
        {
        send_to_char( "#0You Already have this sphere.#n\n\r", ch);
        return;
        }

        SET_BIT(ch->pcsphere, SPHERE_WOKANIST);
        send_to_char( "#0You have begun your studies in the Wokanist Sphere.#n\n\r", ch);
        ch->pcsphereammount++;
        return;
}
else if (!str_cmp(arg1,"necromancer"))
{
        if (IS_SET(ch->pcsphere, SPHERE_NECROMANCER))
        {
        send_to_char( "#0You Already have this sphere.#n\n\r", ch);
        return;
        }

        SET_BIT(ch->pcsphere, SPHERE_NECROMANCER);
        send_to_char( "#0You have become a Necromancer.#n\n\r", ch);
        ch->pcsphereammount++;
        return;
}
else if (!str_cmp(arg1,"pyromancer"))
{
        if (ch->pcRaceLevel < 50)
        {
        send_to_char( "#0This Sphere requires Race level 50.#n\n\r", ch);
        return;
        }
        if (IS_SET(ch->pcsphere, SPHERE_PYROMANCER))
        {
        send_to_char( "#0You Already have this sphere.#n\n\r", ch);
        return;
        }
        if (IS_SET(ch->pcsphere, SPHERE_HYDROMANCER))
        {
        send_to_char( "#0You cant be a Pyromancer if your already a Hydromancer.#n\n\r", ch);
        return;
        }
	else
	{
        SET_BIT(ch->pcsphere, SPHERE_PYROMANCER);
	ch->pcdata->powers[S_HANDFIRE] = 6;
        send_to_char( "#0You have become a Pyromancer.#n\n\r", ch);
        ch->pcsphereammount++;
	}
        return;
}
else if (!str_cmp(arg1,"chaos"))
{
        if (ch->pcRaceLevel < 50)
        {
        send_to_char( "#0This Sphere requires Race level 50.#n\n\r", ch);
        return;
        }
        if (IS_SET(ch->pcsphere, SPHERE_CHAOS))
        {
        send_to_char( "#0You Already have this sphere.#n\n\r", ch);
        return;
        }
        else
        {
        SET_BIT(ch->pcsphere, SPHERE_CHAOS);
        send_to_char( "#0You have become a Chaos Mage.#n\n\r", ch);
        ch->pcsphereammount++;
        }
        return;
}
else if (!str_cmp(arg1,"enchantment"))
{
        if (ch->pcRaceLevel < 50)
        {
        send_to_char( "#0This Sphere requires Race level 50.#n\n\r", ch);
        return;
        }
        if (IS_SET(ch->pcsphere, SPHERE_ENCHANTMENT))
        {
        send_to_char( "#0You Already have this sphere.#n\n\r", ch);
        return;
        }
        else
        {
        SET_BIT(ch->pcsphere, SPHERE_ENCHANTMENT);
        send_to_char( "#0You have become an Enchanter.#n\n\r", ch);
        ch->pcsphereammount++;
        }
        return;
}

else if (!str_cmp(arg1,"healing"))
{
        if (ch->pcRaceLevel < 50)
        {
        send_to_char( "#0This Sphere requires Race level 50.#n\n\r", ch);
        return;
        }
        if (IS_SET(ch->pcsphere, SPHERE_HEALING))
        {
        send_to_char( "#0You Already have this sphere.#n\n\r", ch);
        return;
        }
        else
        {
        SET_BIT(ch->pcsphere, SPHERE_HEALING);
        send_to_char( "#0You have become a Healer.#n\n\r", ch);
        ch->pcsphereammount++;
        }
        return;
}
else if (!str_cmp(arg1,"diviner"))
{
        if (ch->pcRaceLevel < 50)
        {
        send_to_char( "#0This Sphere requires Race level 50.#n\n\r", ch);
        return;
        }
        if (IS_SET(ch->pcsphere, SPHERE_DIVINER))
        {
        send_to_char( "#0You Already have this sphere.#n\n\r", ch);
        return;
        }
        else
        {
        SET_BIT(ch->pcsphere, SPHERE_DIVINER);
        send_to_char( "#0You have become a Diviner.#n\n\r", ch);
        ch->pcsphereammount++;
        }
        return;
}
else if (!str_cmp(arg1,"transmuter"))
{
        if (ch->pcRaceLevel < 50)
        {
        send_to_char( "#0This Sphere requires Race level 50.#n\n\r", ch);
        return;
        }
        if (IS_SET(ch->pcsphere, SPHERE_TRANSMUTER))
        {
        send_to_char( "#0You Already have this sphere.#n\n\r", ch);
        return;
        }
        else
        {
        SET_BIT(ch->pcsphere, SPHERE_TRANSMUTER);
        send_to_char( "#0You have become a Transmuter.#n\n\r", ch);
        ch->pcsphereammount++;
        }
        return;
}
else if (!str_cmp(arg1,"combat2"))
{
        if (ch->pcRaceLevel < 100)
        {
        send_to_char( "#0This Sphere requires Race level 100.#n\n\r", ch);
        return;
        }
        if (IS_SET(ch->pcsphere, SPHERE_COMBAT2))
        {
        send_to_char( "#0You Already have this sphere.#n\n\r", ch);
        return;
        }
        else
        {
        SET_BIT(ch->pcsphere, SPHERE_COMBAT2);
        send_to_char( "#0You have advanced to Combat 2.#n\n\r", ch);
        ch->pcsphereammount++;
        }
        return;
}
else if (!str_cmp(arg1,"illusionist"))
{
        if (ch->pcRaceLevel < 100)
        {
        send_to_char( "#0This Sphere requires Race level 100.#n\n\r", ch);
        return;
        }
        if (IS_SET(ch->pcsphere, SPHERE_ILLUSIONIST))
        {
        send_to_char( "#0You Already have this sphere.#n\n\r", ch);
        return;
        }
        else
        {
        SET_BIT(ch->pcsphere, SPHERE_ILLUSIONIST);
        send_to_char( "#0You have become an Illusionist.#n\n\r", ch);
        ch->pcsphereammount++;
        }
        return;
}
else if (!str_cmp(arg1,"moon"))
{
        if (ch->pcRaceLevel < 100)
        {
        send_to_char( "#0This Sphere requires Race level 100.#n\n\r", ch);
        return;
        }
        if (IS_SET(ch->pcsphere, SPHERE_MOON))
        {
        send_to_char( "#0You Already have this sphere.#n\n\r", ch);
        return;
        }
        else
        {
        SET_BIT(ch->pcsphere, SPHERE_MOON);
        send_to_char( "#0You have gained the moon sphere.#n\n\r", ch);
        ch->pcsphereammount++;
        }
        return;
}

else if (!str_cmp(arg1,"geomancer"))
{
        if (ch->pcRaceLevel < 100)
        {
        send_to_char( "#0This Sphere requires Race level 100.#n\n\r", ch);
        return;
        }
        if (IS_SET(ch->pcsphere, SPHERE_GEOMANCER))
        {
        send_to_char( "#0You Already have this sphere.#n\n\r", ch);
        return;
        }
        if (IS_SET(ch->pcsphere, SPHERE_AEROMANCER))
        {
        send_to_char( "#0You cant be a Geomancer if your already an Aeromancer.#n\n\r", ch);
        return;
        }
        else
        {
        SET_BIT(ch->pcsphere, SPHERE_GEOMANCER);
	ch->pcdata->powers[S_HANDEARTH] = 6;
        send_to_char( "#0You have become a Geomancer.#n\n\r", ch);
        ch->pcsphereammount++;
        }
        return;
}
else if (!str_cmp(arg1,"shadow"))
{
        if (ch->pcRaceLevel < 100)
        {
        send_to_char( "#0This Sphere requires Race level 100.#n\n\r", ch);
        return;
        }
        if (IS_SET(ch->pcsphere, SPHERE_SHADOW))
        {
        send_to_char( "#0You Already have this sphere.#n\n\r", ch);
        return;
        }
        if (IS_SET(ch->pcsphere, SPHERE_HOLY))
        {
        send_to_char( "#0Priests arnt allowed to be Shadowmancers.#n\n\r", ch);
        return;
        }
        else
        {
        SET_BIT(ch->pcsphere, SPHERE_SHADOW);
	ch->pcdata->powers[S_HANDSHADOW] = 6;
        send_to_char( "#0You have become a Shadowmancer.#n\n\r", ch);
        ch->pcsphereammount++;
        }
        return;
}
else if (!str_cmp(arg1,"holy"))
{
        if (ch->pcRaceLevel < 100)
        {
        send_to_char( "#0This Sphere requires Race level 100.#n\n\r", ch);
        return;
        }
        if (IS_SET(ch->pcsphere, SPHERE_HOLY))
        {
        send_to_char( "#0You Already have this sphere.#n\n\r", ch);
        return;
        }
        if (IS_SET(ch->pcsphere, SPHERE_SHADOW))
        {
        send_to_char( "#0Shadowmancers arnt allowed to be Priests.#n\n\r", ch);
        return;
        }
        else
        {
        SET_BIT(ch->pcsphere, SPHERE_HOLY);
	ch->pcdata->powers[S_HANDHOLY] = 6;
        send_to_char( "#0You have become a Priest.#n\n\r", ch);
        ch->pcsphereammount++;
        }
        return;
}


else if (!str_cmp(arg1,"combat3"))
{
        if (ch->pcRaceLevel < 150)
        {
        send_to_char( "#0This Sphere requires Race level 150.#n\n\r", ch);
        return;
        }
        if (IS_SET(ch->pcsphere, SPHERE_COMBAT3))
        {
        send_to_char( "#0You Already have this sphere.#n\n\r", ch);
        return;
        }
        else
        {
        SET_BIT(ch->pcsphere, SPHERE_COMBAT3);
        send_to_char( "#0You have become a Fighter with level 3 Combat.#n\n\r", ch);
        ch->pcsphereammount++;
        }
        return;
}
else if (!str_cmp(arg1,"abjurer"))
{
        if (ch->pcRaceLevel < 150)
        {
        send_to_char( "#0This Sphere requires Race level 150.#n\n\r", ch);
        return;
        }
        if (IS_SET(ch->pcsphere, SPHERE_ABJURER))
        {
        send_to_char( "#0You Already have this sphere.#n\n\r", ch);
        return;
        }
        else
        {
        SET_BIT(ch->pcsphere, SPHERE_ABJURER);
        send_to_char( "#0You have become an Abjurer.#n\n\r", ch);
        ch->pcsphereammount++;
        }
        return;
}
else if (!str_cmp(arg1,"hydromancer"))
{
        if (ch->pcRaceLevel < 150)
        {
        send_to_char( "#0This Sphere requires Race level 150.#n\n\r", ch);
        return;
        }
        if (IS_SET(ch->pcsphere, SPHERE_HYDROMANCER))
        {
        send_to_char( "#0You Already have this sphere.#n\n\r", ch);
        return;
        }
        if (IS_SET(ch->pcsphere, SPHERE_PYROMANCER))
        {
        send_to_char( "#0You cant be a Hydromancer if your already a Pyromancer.#n\n\r", ch);
        return;
        }
        else
        {
        SET_BIT(ch->pcsphere, SPHERE_HYDROMANCER);
	ch->pcdata->powers[S_HANDWATER] = 6;
        send_to_char( "#0You have become a Hydromancer.#n\n\r", ch);
        ch->pcsphereammount++;
        }
        return;
}
else if (!str_cmp(arg1,"enhancement"))
{
        if (ch->pcRaceLevel < 150)
        {
        send_to_char( "#0This Sphere requires Race level 150.#n\n\r", ch);
        return;
        }
        if (IS_SET(ch->pcsphere, SPHERE_ENHANCEMENT))
        {
        send_to_char( "#0You Already have this sphere.#n\n\r", ch);
        return;
        }
        else
        {
        SET_BIT(ch->pcsphere, SPHERE_ENHANCEMENT);
        send_to_char( "#0You have become a student of Enhancement.#n\n\r", ch);
        ch->pcsphereammount++;
        }
        return;
}
else if (!str_cmp(arg1,"aeromancer"))
{
        if (ch->pcRaceLevel < 150)
        {
        send_to_char( "#0This Sphere requires Race level 150.#n\n\r", ch);
        return;
        }
        if (IS_SET(ch->pcsphere, SPHERE_AEROMANCER))
        {
        send_to_char( "#0You Already have this sphere.#n\n\r", ch);
        return;
        }
        else
        {
        SET_BIT(ch->pcsphere, SPHERE_AEROMANCER);
	ch->pcdata->powers[S_HANDWIND] = 6;
        send_to_char( "#0You have become an Aeromancer.#n\n\r", ch);
        ch->pcsphereammount++;
        }
        return;
}
else if (!str_cmp(arg1,"combat4"))
{
        if (ch->pcRaceLevel < 200)
        {
        send_to_char( "#0This Sphere requires Race level 200.#n\n\r", ch);
        return;
        }
        if (IS_SET(ch->pcsphere, SPHERE_COMBAT4))
        {
        send_to_char( "#0You Already have this sphere.#n\n\r", ch);
        return;
        }
        else
        {
        SET_BIT(ch->pcsphere, SPHERE_COMBAT4);
        send_to_char( "#0You have become a fighter with Combat 4.#n\n\r", ch);
        ch->pcsphereammount++;
        }
        return;
}
else if (!str_cmp(arg1,"animal2"))
{
        if (ch->pcRaceLevel < 200)
        {
        send_to_char( "#0This Sphere requires Race level 200.#n\n\r", ch);
        return;
        }
        if (IS_SET(ch->pcsphere, SPHERE_ANIMAL2))
        {
        send_to_char( "#0You Already have this sphere.#n\n\r", ch);
        return;
        }
        else
        {
        SET_BIT(ch->pcsphere, SPHERE_ANIMAL2);
        send_to_char( "#0You have become a Druid with Animal Two#n\n\r", ch);
        ch->pcsphereammount++;
        }
        return;
}
  else do_selfsphere(ch,"");   
  return;
}




/* use for processing a skill or group for addition  */
/*
void group_add( CHAR_DATA *ch, const char *name, bool deduct)
{
    int sn,gn;

    if (IS_NPC(ch)
	return;

    sn = skill_lookup(name);

    if (sn != -1)
    {
	if (ch->pcdata->learned[sn] == 0) 
	{
	    ch->pcdata->learned[sn] = 1;
	    if (deduct)
	   	ch->pcdata->points += skill_table[sn].rating[ch->class]; 
	}
	return;
    }
	*/
    /* now check groups */



void do_remort(CHAR_DATA * ch, char *argument)
{
 
        if( ch->pcRaceLevel < 200 )
        {
        send_to_char( "You have to be level 200 to remort.\n\r", ch );
        return;
        }
        if( ch->pcremort >= 5)
        {
        send_to_char( "You have reached the max remort level.\n\r", ch );
        return;
        }
	ch->pcremort += 1;
	ch->pcRaceLevel = 1;
        if (ch->pcremort == 1){
	ch->max_hit = 20000;
        ch->max_move = 20000;
        ch->max_mana = 20000;}
        if (ch->pcremort == 2){
        ch->max_hit = 30000;
        ch->max_move = 30000;
        ch->max_mana = 30000;}
        if (ch->pcremort == 3){
        ch->max_hit = 40000;
        ch->max_move = 40000;
        ch->max_mana = 40000;}
        if (ch->pcremort == 4){
        ch->max_hit = 50000;
        ch->max_move = 50000;
        ch->max_mana = 50000;}
        if (ch->pcremort == 5){
        ch->max_hit = 60000;
        ch->max_move = 60000;
        ch->max_mana = 60000;}

        send_to_char( "You have succesfuly remorted.\n\r", ch );
        return;
}

void do_advance( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int level;
    int iLevel;
        int hp = 0;  
        int mana = 0;
        int move = 0;
        
        hp = number_range(650,800);  
        mana = number_range(650,800);
        move = number_range(650,800);
        

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) )
    {
	send_to_char( "Syntax: advance <char> <level>.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	send_to_char( "That player is not here.\n\r", ch);
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( ( level = atoi( arg2 ) ) < 1 || level > 200 )
    {
	sprintf(buf,"Level must be 1 to %d.\n\r", 200);
	send_to_char(buf, ch);
	return;
    }

    if ( level <= victim->level )
    {

	send_to_char( "Lowering a player's level!\n\r", ch );
	send_to_char( "**** OOOOHHHHHHHHHH  NNNNOOOO ****\n\r", victim );
	victim->pcRaceLevel    = 1;
	victim->exp      -= 1550000 * ch->pcRaceLevel;
	victim->max_hit  -= hp;
	victim->max_mana = mana;
	victim->max_move = move;
	victim->hit      = victim->max_hit;
	victim->mana     = victim->max_mana;
	victim->move     = victim->max_move;
    }
    else
    {
	send_to_char( "Raising a player's level!\n\r", ch );
	send_to_char( "**** OOOOHHHHHHHHHH  YYYYEEEESSS ****\n\r", victim );
    }

    for ( iLevel = victim->pcRaceLevel ; iLevel < level; iLevel++ )
    {
	victim->pcRaceLevel += 1;
        victim->max_hit += hp;
        victim->max_mana += mana;
        victim->max_move += move;
    }
    sprintf(buf,"You are now level %d.\n\r",victim->pcRaceLevel);
    send_to_char(buf,victim);
    victim->exp      += 1550000 * ch->pcRaceLevel; 
    save_char_obj(victim);
    return;
}


void do_gainlevel(CHAR_DATA * ch, char *argument)
{
        char      buf[MAX_STRING_LENGTH];
	int lgainexp  = 1550000 * ch->pcRaceLevel;
 	int hp = 0;
	int mana = 0;
	int move = 0;

	hp = number_range(650,800);
	mana = number_range(650,800);
	move = number_range(650,800);


	if( ch->pcRaceLevel >= 200 )
	{
        send_to_char( "Already Maximum Race Level.\n\r", ch );
        return;
	}
	if (lgainexp > ch->exp)
        {
        send_to_char( "You dont have enough exp.\n\r", ch );
        return;
        }

	if( ch->exp < lgainexp )
	{
        sprintf( buf, "Not enough Experience points, you need %2.0f more.\n\r", ( lgainexp - ch->exp ) );
        send_to_char( buf, ch );
        return;
        }
        ch->exp -= lgainexp;
        ch->pcRaceLevel++;
	sprintf( buf, "#0%s is now #RLevel #R[#0%d#R]#n", ch->pcdata->switchname, ch->pcRaceLevel );
        do_info( ch, buf );
        sprintf( buf, "You gain %d HP, %d Move and %d Mana.\n\r", hp, move, mana );
        send_to_char( buf, ch );

	ch->max_hit += hp;
	ch->max_move += move;
	ch->max_mana += mana;

        return;
}




void do_newbiepack(CHAR_DATA * ch, char *argument)
{

        int       templevel = 0;


        if (ch->level == 1 || ch->level > 6)
        {
                templevel = ch->level;
                ch->level = 12;
                ch->trust = 12;
                do_oload(ch, "2007");
                do_oload(ch, "2008");
                do_oload(ch, "2009");
                do_oload(ch, "2010");
                do_oload(ch, "2011");
                ch->level = templevel;
                //ch->level = 1;
                ch->trust = 0;
        }
        return;
}

