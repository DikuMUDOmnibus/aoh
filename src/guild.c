#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"


/* Druid */
void do_vineforge(CHAR_DATA *ch, char *argument)
{
	 OBJ_DATA *obj;

	if (argument[0] == '\0')
	  {
		 send_to_char("#0Which item do you wish to vineforge?#n\n\r", ch);
		 return;
	  }
	if ( (obj = get_obj_carry(ch, argument)) == NULL)
	  {
		 send_to_char("#0You are not carrying that item.#n\n\r", ch);
		 return;
	  }
        if (IS_SET(obj->quest,QUEST_VINEFORGE))
          {
                send_to_char("#0This item has already been Vine forged#n\n\r",ch);
                return;
          }
	if (obj->item_type == ITEM_WEAPON)
	  {
		 SET_BIT(obj->quest,QUEST_VINEFORGE);
		 obj->condition = 100;
		 obj->toughness = 100;
		 obj->resistance = 1;
		 act(" $p is surrounded with thin, barbed vines.", ch, obj, NULL, TO_CHAR);
		 act("$n stares into  $p and it becomes surrounded with thin, barbed vines.", ch, obj, NULL, TO_ROOM);
		 ch->practice -= 1500;
		 WAIT_STATE(ch, 8); 
		 return;
	  }
}

/* Beast */
void do_purification( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  int hps = ch->max_hit / 4;

  argument = one_argument( argument, arg );

  if (IS_NPC(ch)) return;

  if (!IS_GUILD(ch, GUILD_BEAST))
{
stc("Only the pure in heart can use purification!\n\r",ch);
      return;
}
if (ch->fight_timer > 0)
{
stc("You have to be totally calm to purify yourself!\n\r",ch);
      return;
}
if (ch->move < 5000)
{
stc("You are too exhausted to purify your mind\n\r",ch);
      return;
}

    act( "A bright halo glows above $n's head.", ch, NULL,NULL, TO_ROOM );
    send_to_char("You purify your mind.\n\r",ch);

WAIT_STATE(ch,12);
ch->move -= 5000;
ch->hit += hps;

if (ch->hit > ch->max_hit)
ch->hit = ch->max_hit;
return;
}

/* Beast */
void do_beastform( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH];
    char buf [MAX_STRING_LENGTH]; 
    argument = one_argument( argument, arg ); 

    if (IS_NPC(ch)) return;

    if (ch->guild != 3)
    {
	send_to_char("#0Huh?#n\n\r",ch);
	return;
    }
    
    if (IS_POLYAFF(ch, POLY_ZULOFORM))
    {   
        REMOVE_BIT(ch->polyaff, POLY_ZULOFORM);
        REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
        act( "#0You transform into human form.#n", ch, NULL, NULL, TO_CHAR );
        act( "#7$n's #0shrinks back into human form.#n", ch, NULL, NULL, TO_ROOM);
        ch->damroll -= 200;
        ch->hitroll -= 200;
        ch->armor   += 300;
	free_string( ch->morph );
        ch->morph = str_dup( "" );
        return;
    }
    else if (IS_AFFECTED(ch,AFF_POLYMORPH))
    {
	send_to_char("#0You cant wolf form when changed.#n\n\r",ch);
	return;
	}
  
  
        if (ch->stance[0] != -1) do_stance(ch,"");
	if (ch->mounted == IS_RIDING) do_dismount(ch,"");
        act( "#0You transform into a huge wolf.#n", ch, NULL, NULL, TO_CHAR);
        act( "#7$n's #0body grows and distorts into a huge wolf.#n", ch, NULL, NULL, TO_ROOM );
        SET_BIT(ch->polyaff, POLY_ZULOFORM);
        SET_BIT(ch->affected_by, AFF_POLYMORPH);
        sprintf(buf, "%s the huge white wolf", ch->name);
        free_string( ch->morph );
        ch->morph = str_dup( buf );
	ch->damroll += 200;
	ch->hitroll += 200;
        ch->armor   -= 300;
        return;
    }

/* Guardian */
void do_guardianbless(CHAR_DATA * ch, char *argument)
{
        CHAR_DATA *victim;
        char      arg[MAX_INPUT_LENGTH];
        int       level, sn, spelltype;

        argument = one_argument(argument, arg);

        if (IS_NPC(ch))
                return;
        if (!IS_GUILD(ch, GUILD_GUARDIAN))
        {
                send_to_char("#0Huh?#n\n\r", ch);
                return;
        }
        if ((victim = get_char_room(ch, arg)) == NULL)
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }
        if (ch->mana < 1000 || ch->move < 500)
        {
                send_to_char("You are to tired.\n\r", ch);
                return;
        }
        act("You Guardian Bless $N.", ch, NULL, victim, TO_CHAR);
        act("$n Guardian Blesses you.", ch, NULL, victim, TO_VICT);
        act("$n protects $N  $S.", ch, NULL, victim, TO_NOTVICT);


        if ((sn = skill_lookup("bless")) > 0)
	  { 
        spelltype = skill_table[sn].target;
        level = 200;
        (*skill_table[sn].spell_fun) (sn, level, ch, victim);
	  }

        if ((sn = skill_lookup("frenzy")) > 0)
	  {
        spelltype = skill_table[sn].target;
        level = 200;
        (*skill_table[sn].spell_fun) (sn, level, ch, victim);
	  }

        if ((sn = skill_lookup("armor")) > 0)
	  {
        spelltype = skill_table[sn].target;
        level = 200;
        (*skill_table[sn].spell_fun) (sn, level, ch, victim);
        }

        if ((sn = skill_lookup("cure poison")) > 0)
	  {
        spelltype = skill_table[sn].target;
        level = 200;
        (*skill_table[sn].spell_fun) (sn, level, ch, victim);
        }

        if ((sn = skill_lookup("shield")) > 0)
	  {
        spelltype = skill_table[sn].target;
        level = 200;
        (*skill_table[sn].spell_fun) (sn, level, ch, victim);
        }

        if ((sn = skill_lookup("stone skin")) > 0) 
        {
        spelltype = skill_table[sn].target;
        level = 200;
        (*skill_table[sn].spell_fun) (sn, level, ch, victim);
        }

        if ((sn = skill_lookup("darkblessing")) > 0) 
        {
        spelltype = skill_table[sn].target;
        level = 200;
        (*skill_table[sn].spell_fun) (sn, level, ch, victim);
        }


        if ((sn = skill_lookup("fly")) > 0) 
        {
        spelltype = skill_table[sn].target;
        level = 200;
        (*skill_table[sn].spell_fun) (sn, level, ch, victim);
        }

        if ((sn = skill_lookup("detect invis")) > 0) 
        {
        spelltype = skill_table[sn].target;
        level = 200;
        (*skill_table[sn].spell_fun) (sn, level, ch, victim);
        }

        if ((sn = skill_lookup("detect hidden")) > 0) 
        {
        spelltype = skill_table[sn].target;
        level = 200;
        (*skill_table[sn].spell_fun) (sn, level, ch, victim);
        }

        if ((sn = skill_lookup("detect magic")) > 0) 
        {
        spelltype = skill_table[sn].target;
        level = 200;
        (*skill_table[sn].spell_fun) (sn, level, ch, victim);
        }

        ch->mana -= 1000;
        ch->move -= 500;
        WAIT_STATE(ch, 12);
        return;
}

/* Damned */
void do_faeriecurse(CHAR_DATA * ch, char *argument)
{
        CHAR_DATA *victim;
        char      arg[MAX_INPUT_LENGTH];
        int       level, sn, spelltype;

        argument = one_argument(argument, arg);

        if (IS_NPC(ch))
                return;
        if (!IS_GUILD(ch, GUILD_DAMNED))
        {
                send_to_char("#0Huh?#n\n\r", ch);
                return;
        }
        if ((victim = get_char_room(ch, arg)) == NULL)
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }
        if (ch == victim)
        {
                send_to_char("Not on yourself.\n\r", ch);
                return;
        }
        if (ch->mana < 1000 || ch->move < 500)
        {
                send_to_char("You are to tired.\n\r", ch);
                return;
        }
        act("You let the forces of limbo envelope $N.", ch, NULL, victim,
            TO_CHAR);
        act("$n points at you and flickering rays of energy strikes your body.", ch, NULL, victim, TO_VICT);
        act("$n points at $N and flickering rays of energy strikes $S.", ch,
            NULL, victim, TO_NOTVICT);
        if (number_range(1, 3) != 1)
        {
                if ((sn = skill_lookup("web")) < 0)
                        return;
                spelltype = skill_table[sn].target;
                level = 50;
                (*skill_table[sn].spell_fun) (sn, level, ch, victim);
        }
        if (number_range(1, 3) != 1)
        {
                if ((sn = skill_lookup("curse")) < 0)
                        return;
                spelltype = skill_table[sn].target;
                level = 50;
                (*skill_table[sn].spell_fun) (sn, level, ch, victim);
        }
        ch->mana -= 1000;
        ch->move -= 500;
        WAIT_STATE(ch, 12);
        return;
}

/* Dark Hand */
void do_raise( CHAR_DATA *ch, char *argument )
{
  char buf [MAX_STRING_LENGTH];
  OBJ_DATA *corpse;
  CHAR_DATA *victim;
  char arg[MAX_STRING_LENGTH];
  AFFECT_DATA af;

  one_argument (argument, arg);


  if (IS_NPC(ch)) return;

  if (!IS_GUILD(ch, GUILD_DARK_HAND))
  {  
    send_to_char("#0This is a Dark Hand Guild Ability.#n\n\r",ch);
    return;
  }

  if ((corpse = get_obj_room (ch, arg)) == NULL)
    {
      stc ("That corpse isnt here.\n\r", ch);
      return;
    }
  if (corpse->item_type != ITEM_CORPSE_NPC)
    {
      stc ("Only on mobile corpses.\n\r", ch);
      return;
    }

    victim=create_mobile( get_mob_index( MOB_VNUM_MOUNT ));
    free_string(victim->short_descr);
    free_string(victim->name);
    free_string(victim->long_descr);
    victim->name = str_dup( corpse->name);
    victim->short_descr = str_dup(corpse->short_descr);
    victim->long_descr = str_dup(corpse->description);
    victim->level = ch->spl[RED_MAGIC];
    victim->hit = 200 * ch->spl[RED_MAGIC]; 
    victim->max_hit = 200 * ch->spl[RED_MAGIC];
    victim->hitroll =  ch->spl[RED_MAGIC]; 
    victim->damroll =  ch->spl[RED_MAGIC];  
    char_to_room( victim, ch->in_room );
    add_follower( victim, ch );
    af.type      = skill_lookup ("charm person");
    af.duration  = 666;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( victim, &af );
    SET_BIT(victim->act,ACT_NOEXP);
    SET_BIT(victim->act,PLR_HOLYLIGHT);
    do_group(ch,victim->name);
    if (corpse != NULL)
    extract_obj (corpse);
    return;

}

/* Dark Hand */
void do_perfectsight( CHAR_DATA *ch, char *argument )
{
  if (IS_NPC(ch)) return;

  if (!IS_GUILD(ch, GUILD_DARK_HAND))
  {
    send_to_char("Huh?\n\r",ch);
    return;
  }
  if (IS_SET(ch->act, PLR_HOLYLIGHT))
  {
    REMOVE_BIT(ch->act, PLR_HOLYLIGHT);
    send_to_char("Your vision is back to normal.\n\r", ch);
  }
  else
  {
    SET_BIT(ch->act, PLR_HOLYLIGHT);
    send_to_char( "You eyes now penetrate the dimension of the mortals.\n\r", ch );
  }
return;
}


void do_knightshield( CHAR_DATA *ch, char *argument )
{
  OBJ_INDEX_DATA *pObjIndex;
  OBJ_DATA *obj;
  char arg[MAX_INPUT_LENGTH];
  int vnum = 0;

  argument = one_argument( argument, arg );

  if (IS_NPC(ch)) return;

  if (!IS_GUILD(ch, GUILD_CRUSADERS))
  {
    send_to_char("#0You are not a crusader.#n\n\r",ch);
    return;
  }


  if ( ch->pcgold < 100000 )
  {
    send_to_char("#0You don't have the 100,000 gold needed.#n\n\r",ch);
    return;
  }

  vnum = 2909;

  if ( vnum == 0 || (pObjIndex = get_obj_index( vnum )) == NULL)
  {
    send_to_char("#7Missing object, please inform Xrakisis.#n\n\r",ch);
    return;
  }
  if(!IS_IMMORTAL(ch)) ch->pcdata->quest -= 100000;
  obj = create_object(pObjIndex, 50);
  obj->questowner = str_dup(ch->pcdata->switchname);
  obj_to_char(obj, ch);
  act("You transform the 100,000 gold into $p.",ch,obj,NULL,TO_CHAR);
  act("$n forms $p out of 100,000 gold.",ch,obj,NULL,TO_ROOM);
  return;
}


void load_fortress()
{

  int i;   
  FILE *fp;

  if ((fp = fopen("../txt/fortress.txt", "r")) == NULL)
  {
    log_string("Error: fortress.txt not found!");
    exit(1);
  }
  fortress[0].guard = 0;
  fortress[0].soldier = 0;

  for (i = 0; i < 6; i++)
  {
    fortress[i].guard = fread_number(fp);
    fortress[i].soldier = fread_number(fp);
  }
  fclose(fp);

}
void save_fortress()
{

  FILE *fp;
  int i;

  if ((fp = fopen("../txt/fortress.txt","w")) == NULL)
  {
    log_string("Error writing to fortress.txt");
    return;
  }
  for (i=0;i<6;i++)
  {
    fprintf(fp, "%d\n", fortress[i].guard); 
    fprintf(fp, "%d\n", fortress[i].soldier);
  }
  fclose (fp);

}

void do_garrison(CHAR_DATA *ch, char *argument)
{
char arg1[MAX_INPUT_LENGTH];
char guild[MAX_STRING_LENGTH];
char buf[MAX_STRING_LENGTH];
int x;


argument = one_argument( argument, arg1 );


sprintf(buf,"Dark Hand Guards: %d\n\r",fortress[0].guard);
send_to_char( buf, ch );
sprintf(buf,"Dark Hand Soldiers: %d\n\r",fortress[0].soldier);
send_to_char( buf, ch );
sprintf(buf,"Guardian Guards: %d\n\r",fortress[1].guard);
send_to_char( buf, ch );
sprintf(buf,"Guardian Soldiers: %d\n\r",fortress[1].soldier);
send_to_char( buf, ch );
sprintf(buf,"Beast Guards: %d\n\r",fortress[2].guard);
send_to_char( buf, ch );
sprintf(buf,"Beast Soldiers: %d\n\r",fortress[2].soldier);
send_to_char( buf, ch );
sprintf(buf,"Damned Guards: %d\n\r",fortress[3].guard);
send_to_char( buf, ch );
sprintf(buf,"Damned Soldiers: %d\n\r",fortress[3].soldier);
send_to_char( buf, ch );
sprintf(buf,"Crusaders Guards: %d\n\r",fortress[4].guard);
send_to_char( buf, ch );
sprintf(buf,"Crusaders Soldiers: %d\n\r",fortress[4].soldier);
send_to_char( buf, ch );
sprintf(buf,"Druid Guards: %d\n\r",fortress[5].guard);
send_to_char( buf, ch );
sprintf(buf,"Druid Soldiers: %d\n\r",fortress[5].soldier);
send_to_char( buf, ch );
return;
}


void do_hire(CHAR_DATA *ch, char *argument)
{
char arg1[MAX_INPUT_LENGTH];
char guild[MAX_STRING_LENGTH];
char buf[MAX_STRING_LENGTH];
int x;

  x = ch->guild - 1;

  argument = one_argument( argument, arg1 );

  if (arg1[0] == '\0')
  {
send_to_char("Soldier 100,000\n\r",ch);
send_to_char("Guard 100,000\n\r",ch);
  }

if (!str_cmp(arg1,"soldier"))
{
  if (ch->pcgold < 100000 )
  {
    send_to_char("You don't have the 100,000 gold needed.\n\r",ch);
    return;
  }

  fortress[x].soldier++;
  send_to_char("You hire a soldier.\n\r",ch);
  ch->pcgold -= 100000;
  save_fortress();
  return;
}
if (!str_cmp(arg1,"guard"))
{
  if (ch->pcgold < 100000 )
  {
    send_to_char("You don't have the 100,000 gold needed.\n\r",ch);
    return;
  }

  fortress[x].guard++;
  send_to_char("You hire a guard.\n\r",ch);
  ch->pcgold -= 100000;
  save_fortress();
  return;
}
return;
}


void do_grecall(CHAR_DATA * ch, char *argument)
{
        char      buf[MAX_STRING_LENGTH];
        CHAR_DATA *victim;
        CHAR_DATA *mount;
        ROOM_INDEX_DATA *location;

        if (IS_NPC(ch))
                return;

        if (ch->guild == 0)
        {
        stc("#0your not in a Guild#n\n\r", ch);
        return;
        }

        act("$n's body flickers with red energy.", ch, NULL, NULL, TO_ROOM);
        act("Your body flickers with red energy.", ch, NULL, NULL, TO_CHAR);

        if (IS_GUILD(ch, GUILD_DARK_HAND))
                location = get_room_index(5200);
        else if (IS_GUILD(ch, GUILD_GUARDIAN))
                location = get_room_index(5101);

        else
        {
                send_to_char("#0Your grecall is broken, tell Xrakisis#n\n\r", ch);
                return;
        }


        if (ch->in_room == location)
                return;

        if (IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
            || IS_AFFECTED(ch, AFF_CURSE))
        {
                send_to_char("You are unable to crecall.\n\r", ch);
                return;
        }

        if ((victim = ch->fighting) != NULL)
        {
                if (number_bits(1) == 0)
                {
                        WAIT_STATE(ch, 4);
                        sprintf(buf, "You failed!\n\r");
                        send_to_char(buf, ch);
                        return;
                }
                sprintf(buf, "You grecall from combat!\n\r");
                send_to_char(buf, ch);
                stop_fighting(ch, TRUE);
        }

        act("$n disappears.", ch, NULL, NULL, TO_ROOM);
        char_from_room(ch);
        char_to_room(ch, location);
        act("$n appears in the room.", ch, NULL, NULL, TO_ROOM);
        do_look(ch, "auto");
        if ((mount = ch->mount) == NULL)
                return;
        char_from_room(mount);
        char_to_room(mount, ch->in_room);
        return;
}

