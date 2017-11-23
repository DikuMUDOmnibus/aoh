
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"

/* Mercenary */
void do_sharpen(CHAR_DATA *ch, char *argument)
{
	 OBJ_DATA *obj;

        if (!IS_PROF(ch, PROF_MERCENARY))
        {
                send_to_char("#0You are not a Mercenary.#n\n\r", ch);
                return;
        }

	if (argument[0] == '\0')
	  {
		 send_to_char("#0Which item do you wish to sharpen?#n\n\r", ch);
		 return;
	  }
	if ( (obj = get_obj_carry(ch, argument)) == NULL)
	  {
		 send_to_char("#0You are not carrying that item.#n\n\r", ch);
		 return;
	  }
        if (IS_SET(obj->quest,QUEST_SHARPEN))
          {
                send_to_char("#0This item has already been sharpened!#n\n\r",ch);
                return;
          }
	if (obj->item_type == ITEM_WEAPON)
	  {
		 obj->value[1] += 500;
		 obj->value[2] += 500;
		 obj->level     =  50;
		 SET_BIT(obj->quest, QUEST_SHARPEN);
 		 obj->condition = 100;
		 obj->toughness = 100;
		 obj->resistance = 1;
		 act("#0You sharpen#n $p.", ch, obj, NULL, TO_CHAR);
		 act("#0$n sharpens#n $p..", ch, obj, NULL, TO_ROOM);
		 ch->practice -= 1500;
		 WAIT_STATE(ch, 8); 
		 return;
	  }
}

/* Cleric */
void do_thorns(CHAR_DATA * ch, char *argument)
{
        AREA_AFFECT af;
        AREA_DATA *pArea;
        int       cost = 15000;

        if (IS_NPC(ch))
                return;
        if (!IS_PROF(ch, PROF_CLERIC))
        {
                send_to_char("#0You are not a Cleric.#n\n\r", ch);
                return;
        }
        if ((pArea = ch->in_room->area) == NULL)
        {
                send_to_char("#0You cannot cast this spell in this place.#n\n\r", ch);
                return;
        }
        if (has_area_affect(pArea, AREA_AFF_THORNS, 0))
        {
                send_to_char("#0This holy enchantment has already been invoked here.#n\n\r",ch);
                return;
        }
        if (ch->mana < cost)
        {
                send_to_char("#0You don't have enough mana.#n\n\r", ch);
                return;
        }
        if (has_area_affect(pArea, AREA_AFF_CHAOS, 0))
        {
                if (number_range(1, 4) != 2)
                {
                        int       dam;

                        dam = number_range(10, 20);
                        dam *= ch->pcdata->current_faith;   // wicked :)
                        ch->hit -= dam;
                        if (ch->hit < 1)
                                ch->hit = 1;
                        ch->mana -= cost;

                        send_to_char
                                ("Your enchantment backfires, the spell is spoiled.\n\r",
                                 ch);
                        WAIT_STATE(ch, 24);
                        return;
                }
        }

        af.type = AREA_AFF_THORNS;
        af.duration = ch->pcRaceLevel;
        af.level = ch->pcRaceLevel;
        affect_to_area(pArea, &af);

        act("$n invokes the name of $s god, as $e lays the Thorns and Brambles enchantment.", ch, NULL, NULL, TO_ROOM);
        send_to_char("You lay the Thorns and Brambles enchantment on this area.\n\r",ch);
        ch->mana -= cost;
        WAIT_STATE(ch, 36);
        return;
}

/* Cleric */
void do_landscape(CHAR_DATA * ch, char *argument)
{
        AREA_AFFECT af;
        AREA_DATA *pArea;
        int       cost = 20000;
        int       fps = 1;

        if (IS_NPC(ch))
                return;

        if (!IS_PROF(ch, PROF_CLERIC))
        {
                send_to_char("#0You are not a Cleic.#n\n\r", ch);
                return;
        }
        if ((pArea = ch->in_room->area) == NULL)
        {
                send_to_char("You cannot cast this spell in this place.\n\r",
                             ch);
                return;
        }
        if (has_area_affect(pArea, AREA_AFF_CHAOS, 0))
        {
                send_to_char
                        ("This unholy enchantment has already been invoked here.\n\r",
                         ch);
                return;
        }
        if (ch->mana < cost)
        {
                send_to_char("You don't have enough mana.\n\r", ch);
                return;
        }

        af.type = AREA_AFF_CHAOS;
        af.duration = ch->pcRaceLevel;
        af.level = ch->pcRaceLevel;
        affect_to_area(pArea, &af);

        act("$n invokes the name of $s god, as $e lays the Chaotic Landscape enchantment.", ch, NULL, NULL, TO_ROOM);
        send_to_char("You lay the Chaotic Landscape enchantment on this area.\n\r", ch);

        ch->mana -= cost;
        WAIT_STATE(ch, 12);
        return;
}

/* Solider */
void do_blink(CHAR_DATA * ch, char *argument)
{
        CHAR_DATA *victim;

        if (IS_NPC(ch))
                return;
	  if (!IS_PROF(ch, PROF_SOLDIER))	
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }
        if (ch->mana < 2500)
        {
                send_to_char("This costs 2500 mana.\n\r", ch);
                return;
        }
        if ((victim = ch->fighting) == NULL)
        {
                send_to_char("You aren't fighting anyone.\n\r", ch);
                return;
        }
        WAIT_STATE(ch, 18);
        ch->mana -= 2500;
        act("You enter limbo, steps back into reality behind $N and attack.",
            ch, NULL, victim, TO_CHAR);
        act("$n vanishes and suddenly someone strikes you from behind.", ch,
            NULL, victim, TO_VICT);
        act("$n vanishes and appear behind $N striking $S in the back.", ch,
            NULL, victim, TO_NOTVICT);
        multi_hit(ch, victim, TYPE_UNDEFINED);
        return;
}


/* Monk */
void do_darkblaze(CHAR_DATA * ch, char *argument)
{
        CHAR_DATA *victim;
        AFFECT_DATA af;


        if (IS_NPC(ch))
                return;
        if (!IS_PROF(ch, PROF_MONK))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }
        if (ch->fighting != NULL)
                victim = ch->fighting;
        else
                return;
        if (IS_SET(victim->act, PLR_HOLYLIGHT))
        {
                REMOVE_BIT(victim->act, PLR_HOLYLIGHT);
        }
        if (IS_SET(victim->affected_by, AFF_DETECT_HIDDEN))
                REMOVE_BIT(victim->affected_by, AFF_DETECT_HIDDEN);
        if (IS_SET(victim->affected_by, AFF_DETECT_INVIS))
                REMOVE_BIT(victim->affected_by, AFF_DETECT_INVIS);
        af.type = skill_lookup("blindness");
        af.location = APPLY_HITROLL;
        af.modifier = -4;
        af.duration = 60;
        af.bitvector = AFF_BLIND;
        affect_to_char(victim, &af);
        send_to_char("Your hands blaze up, blinding your opponent.\n\r", ch);
        send_to_char
                ("Your opponents hands flashes with a blinding light.\n\r",
                 victim);
        WAIT_STATE(ch, 18);
        return;
}

/* Barbarian */
void do_eyegouge( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  char buf[MAX_STRING_LENGTH];
  int dam;
  int chance;

  if (IS_NPC(ch)) return;
 
  if (!IS_PROF(ch, PROF_BARBARIAN))
  {
    send_to_char("Huh?\n\r",ch);
    return;
  }
  if ( ( victim = get_char_room( ch, arg ) ) == NULL )
  {
    if (ch->fighting == NULL) 
    {
      send_to_char( "They aren't here.\n\r", ch );
      return;
    }
    else victim = ch->fighting;
     
  }
  chance = number_range(1,4);

  if (!IS_NPC(victim)) dam = number_range(4000,8000);
  else dam = number_range(15000,25000);

  if (chance == 1)
  {
  if (IS_SET(victim->loc_hp[0], LOST_EYE_L))
  {
   send_to_char("They already dont have a left eye!.\n\r",ch);
   return;
  }
  SET_BIT(victim->loc_hp[0], LOST_EYE_L);
  send_to_char("You gouge out their left eye.\n\r",ch);
  send_to_char("Your left eye has been gouged out!.\n\r",ch);
  sprintf(buf,"Your Eye Gouge strikes $N blinding them in their left eye! [%d]\n\r",dam);
  act(buf,ch,NULL,victim,TO_CHAR);
  sprintf(buf,"$n's Eye Gouge strikes you destroying your left eye [%d]\n\r",dam);
  act(buf,ch,NULL,victim,TO_VICT);
  sprintf(buf,"$n's Eye Gouge $N destroying their left eye! [%d]\n\r",dam);
  act(buf,ch,NULL,victim,TO_NOTVICT); 
  hurt_person(ch,victim, dam);
  WAIT_STATE( ch, 6 );
  }
  if (chance == 2)
  {
  if (IS_SET(victim->loc_hp[0], LOST_EYE_R))
  {
   send_to_char("They already dont have a right eye!.\n\r",ch);
   return;
  }
  SET_BIT(victim->loc_hp[0], LOST_EYE_R);
  send_to_char("You gouge out their right eye.\n\r",ch);
  send_to_char("Your right eye has been gouged out!.\n\r",ch);
  sprintf(buf,"Your Eye Gouge strikes $N blinding them in their right eye! [%d]\n\r",dam);
  act(buf,ch,NULL,victim,TO_CHAR);
  sprintf(buf,"$n's Eye Gouge strikes you destroying your right eye [%d]\n\r",dam);
  act(buf,ch,NULL,victim,TO_VICT);
  sprintf(buf,"$n's Eye Gouge $N destroying their right eye! [%d]\n\r",dam);
  act(buf,ch,NULL,victim,TO_NOTVICT); 
  hurt_person(ch,victim, dam);
  WAIT_STATE( ch, 6 );
  }
  if (chance == 3 || chance == 4)
  {
  send_to_char("You fail to gouge out an eye.\n\r",ch);
  WAIT_STATE(ch, 12);
  return;
  }
  return;
}

/* Barbarian */
void do_rage(CHAR_DATA *ch, char *argument)
{
  char arg [MAX_INPUT_LENGTH];
  CHAR_DATA *vch;
  CHAR_DATA *vch_next;
  CHAR_DATA *mount;
  int number_hit = 0;

  argument = one_argument( argument, arg );
  if (IS_NPC(ch)) return;

  if (ch->pcdata->powers[RAGE_TIMER] > 0 )
  {
  send_to_char("#0You may not rage yet.#n\n\r",ch);
  return;
  }

  WAIT_STATE( ch, 6);


  act("#0You Roar with #RBARBARIAN #YR#yA#7G#RE#0!#n",ch,NULL,NULL,TO_CHAR);
  act("#0$n Roars with #RBARBARIAN #YR#yA#7G#RE#0!#n#n",ch,NULL,NULL,TO_ROOM);
        for (vch = char_list; vch != NULL; vch = vch_next)
        {
                vch_next = vch->next;
                if (vch->in_room == NULL)
                        continue;
                if (ch == vch)
                {
                        act("You throw back your head and howl with rage!",
                            ch, NULL, NULL, TO_CHAR);
                        continue;
                }
                if (!IS_NPC(vch) && vch->pcdata->chobj != NULL)
                        continue;
                if (!IS_NPC(vch))
                {
//                        if (vch->in_room == ch->in_room)
                        if (vch->in_room->area == ch->in_room->area)
                                send_to_char
                                        ("You hear a Warcry close by!\n\r",
                                         vch);
                        else if (!CAN_PK(vch))
                                continue;
                }
                if (vch->in_room == ch->in_room && can_see(ch, vch))
                {
                        multi_hit(ch, vch, TYPE_UNDEFINED);
                        if (vch == NULL || vch->position <= POS_STUNNED)
                                continue;
                        multi_hit(ch, vch, TYPE_UNDEFINED);
                }
        }
	ch->pcdata->powers[RAGE_TIMER] = 5;
return;
}

/* Knight */
void do_layonhands( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  char buf[MAX_STRING_LENGTH];
  int dam;

  if (IS_NPC(ch)) return;
 
  if (IS_PROF(ch, PROF_KNIGHT))
  {
    send_to_char("#0This is a Knight Ability#n\n\r",ch);
    return;
  }
  if ( ( victim = ch->fighting ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }
  WAIT_STATE( ch, 12 );

  if (!IS_NPC(victim)) dam = number_range(4000,6000);
  else dam = number_range(4500,7000);

  sprintf(buf,"#0You lay your hands on $N #R[#0%d#R]#n\n\r",dam);
  act(buf,ch,NULL,victim,TO_CHAR);
  sprintf(buf,"$n's #0lays their hands on you!#n [%d]\n\r",dam);
  act(buf,ch,NULL,victim,TO_VICT);
  sprintf(buf,"$n's #0lays their hands on $N [%d]\n\r",dam);
  act(buf,ch,NULL,victim,TO_NOTVICT); 
  hurt_person(ch,victim, dam);
  ch->hit += dam;
  if (ch->hit > ch->max_hit) ch->hit = ch->max_hit;
  return;
}

/* Knight */
void do_bash(CHAR_DATA *ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  
  argument = one_argument( argument, arg );
  argument = one_argument( argument, arg2 );
  if (IS_NPC(ch)) return;


  if (!IS_PROF(ch, PROF_KNIGHT))
  {
    send_to_char("#0You are not a knight#n.\n\r",ch);
    return;
  }
    if (!(arg2[0] == '\0') || ch->fighting != NULL)
    {
      if (arg2[0] == '\0') victim = ch->fighting;
      else if ((victim = get_char_room(ch, arg2)) == NULL)
      {
        send_to_char("They are not here.\n\r", ch );
        return;
      }
      if (is_safe(ch, victim)) return;
      WAIT_STATE(ch, 18);

  act("You take a step back, then launch yourself straight at $N...",ch,NULL,victim,TO_CHAR);
  act("$n takes a step back, then launches $mself straight at you...",ch,NULL,victim,TO_VICT);
  act("$n takes a step back, then launches $mself straight at $N...",ch,NULL,victim,TO_NOTVICT);

  if ( number_range(1,3) == 3 )
  {
    act("$N sidesteps, leaving you wide open!",ch,NULL,victim,TO_CHAR);
    act("You sidestep, leaving $n wide open!",ch,NULL,victim,TO_VICT);
    act("$N sidesteps, leaving $n wide open!",ch,NULL,victim,TO_NOTVICT);
      WAIT_STATE(victim, 24);
    if ( number_range(1,2) == 1 )
    {
      act("$N takes the opportunity to strike back!",ch,NULL,victim,TO_CHAR);
      act("You take the opportunity to strike back!",ch,NULL,victim,TO_VICT);
      act("$N takes the opportunity to strike back!",ch,NULL,victim,TO_NOTVICT);
      multi_hit(victim,ch,TYPE_UNDEFINED);
    }
    else
    {
      act("You recover yourself before $N can strike back.",ch,NULL,victim,TO_CHAR);
      act("$n recovers $mself before you can strike back.",ch,NULL,victim,TO_VICT);
      act("$n recovers $mself before $N can strike back.",ch,NULL,victim,TO_NOTVICT);
    }
    return;
  }

      stop_fighting(ch, TRUE);
      return;
    }
  }


/* Thief */
void do_dirtkick( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  char buf[MAX_STRING_LENGTH];
  int dam;
  AFFECT_DATA af;

  if (IS_NPC(ch)) return;
 
  if (!IS_PROF(ch, PROF_THIEF))
  {
    send_to_char("#0This is a Thief Ability#n\n\r",ch);
    return;
  }
  if ( ( victim = ch->fighting ) == NULL )
  {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
  }

      if (is_safe(ch, victim)) return;
      act("#0$n Kicks Dirt in $N's Eyes#n",ch,NULL,victim,TO_NOTVICT);
      act("#0You kick dirt in $N's Eyes#n'.",ch,NULL,victim,TO_CHAR);
      act("#0$n Kicks Dirt in your Eyes!#n'.",ch,NULL,victim,TO_VICT);
      if (IS_SET(victim->act,PLR_HOLYLIGHT)) REMOVE_BIT(victim->act,PLR_HOLYLIGHT);
      if (IS_SET(victim->affected_by, AFF_DETECT_HIDDEN))
        REMOVE_BIT(victim->affected_by, AFF_DETECT_HIDDEN);
      if (IS_SET(victim->affected_by, AFF_DETECT_INVIS))
        REMOVE_BIT(victim->affected_by, AFF_DETECT_INVIS);
      af.type      = skill_lookup("blindness");
      af.location  = APPLY_HITROLL;
      af.modifier  = -4;
      af.duration  = 60;
      af.bitvector = AFF_BLIND;
      affect_to_char( victim, &af );
      WAIT_STATE(ch,12);
      return;
  }

/* Assassin */
void do_sandman( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg1 [MAX_INPUT_LENGTH];
    argument = one_argument( argument, arg1 );
        
    if (IS_NPC(ch)) return;
	
 if (IS_PROF(ch, PROF_ASSASSIN))
  {
    send_to_char("#0This is an Assassin Ability#n\n\r",ch);
    return;
  }

  if (ch->in_room != NULL)
  {
    if ( IS_SET(ch->in_room->room_flags,ROOM_SAFE) )
    {
      send_to_char( "You cannot use this power in a safe room.\n\r", ch );
      return;
    }
  }

	
    if ( arg1[0] == '\0' )
    {
	send_to_char( "Put who to sleep?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( ch == victim )
    {
	send_to_char( "Not on yourself!\n\r", ch );
	return;
    }

    if (is_safe( ch, victim )) 
    {
       stc("Not in a safe room or while ethereal.\n\r", ch);
       return;
    }
	
    if ( ch->position == POS_FIGHTING )
    {
	send_to_char( "Not while fighting!\n\r", ch );
	return;
    }
    
	if (ch->move < 500)
	{
	send_to_char("You don't have enough movement points.\n\r", ch);
	return;
	}     
	  
    if ( victim->in_room == ch->in_room )
	{
	act("You invoke the power of Takhisis and put $N to sleep.",ch,NULL,victim,TO_CHAR);
	act("$n invokes the power of Takhisis and you drift into dreamland.",ch,NULL,victim,TO_VICT);
	victim->position = POS_SLEEPING;
	ch->move -= 500;
	WAIT_STATE(ch, 1);
	return;
	}
	
	return;
}

/* Assassin */
void do_impale (CHAR_DATA *ch, char *argument)
{   
  CHAR_DATA *victim;
  OBJ_DATA *obj;
  char arg[MAX_INPUT_LENGTH];
  argument = one_argument( argument, arg );

   if (IS_PROF(ch, PROF_ASSASSIN))
   {
   send_to_char("Huh?\n\r",ch);
   return;
   }
    
   else if (arg[0] == '\0')
   {       
   send_to_char("Please choose your victim.\n\r", ch );
   return;
   }
  
   else if ((victim = get_char_room(ch, arg)) == NULL)
   {       
        send_to_char("They are not here.\n\r", ch );
        return;
   }
  if(((obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL) || obj->item_type != ITEM_WEAPON)
  {
  if(((obj = get_eq_char( ch, WEAR_HOLD ) ) == NULL) || obj->item_type != ITEM_WEAPON)
     {
        send_to_char("You dont have a weapon to throw.\n\r", ch );
        return;
     }
  }
obj_from_char( obj );
obj_to_char(obj, victim);
do_say(ch, "Fool, perish at my feet.");
act( "#0$n has thrown his weapon, impaling his victim.#n",ch,NULL,NULL,TO_ROOM);
SET_BIT(victim->extra, PINNEDWALL);
send_to_char("You have impaled your victim.\n\r",ch);
send_to_char("Has thrown his weapon and impaled you, flex to remove it.\n\r", victim );    
WAIT_STATE(victim, 6);
WAIT_STATE(ch, 12);
}  
