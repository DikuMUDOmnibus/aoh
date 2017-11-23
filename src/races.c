#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "merc.h"


void do_gore(CHAR_DATA * ch, char *argument)
{
        char      arg[MAX_INPUT_LENGTH];
        CHAR_DATA *victim;

        one_argument(argument, arg);

        if (IS_NPC(ch))
                return;

        if (!IS_RACE(ch, RACE_MINOTAUR))
        {
                send_to_char("#0This is a Minotaur Ability#n\n\r", ch);
                return;
        }

        if (arg[0] == '\0')
        {
                send_to_char("Gore whom?\n\r", ch);
                return;
        }

        if ((victim = get_char_room(ch, arg)) == NULL)
        {
                send_to_char("They aren't here.\n\r", ch);
                return;
        }

        if (victim == ch)
        {
                send_to_char("#0No goring yourself#n\n\r", ch);
                return;
        }


        if (IS_NPC(victim))
        {
         
                send_to_char
                        ("#cYou materialize in front of your opponent and #Rgore him with your sharp horns!#n\n\r",
                         ch);
                act("#G$n #cmaterializes out of thin air, screaming wildly, and #Rgores $s opponent with $s razor sharp horns!!#n", ch, NULL, NULL, TO_ROOM);
                ch->move -= 10000;
                WAIT_STATE(ch, 3 * PULSE_VIOLENCE);
                multi_hit(ch, victim, TYPE_UNDEFINED);
                multi_hit(ch, victim, TYPE_UNDEFINED);
                multi_hit(ch, victim, TYPE_UNDEFINED);
                multi_hit(ch, victim, TYPE_UNDEFINED);
                multi_hit(ch, victim, TYPE_UNDEFINED);
                return;
        }

        send_to_char
                ("#cYou materialize in front of your opponent and #Rgore him with your sharp horns#n\n\r",
                 ch);
        act("#G$n #cmaterializes out of thin air, screaming wildly, and #Rgores you with $s razor sharp horns!!#n", ch, NULL, NULL, TO_VICT);
        ch->move -= 10000;
        WAIT_STATE(ch, 3 * PULSE_VIOLENCE);
        multi_hit(ch, victim, TYPE_UNDEFINED);
        if (number_percent() > 85)
                multi_hit(ch, victim, TYPE_UNDEFINED);
        if (number_percent() > 95)
                multi_hit(ch, victim, TYPE_UNDEFINED);
        return;
}


void do_jawlock(CHAR_DATA *ch,char *argument)
{
  if (IS_NPC(ch)) return;
  if (!IS_RACE(ch, RACE_GNOLL))
  {
  send_to_char("Huh?\n\r",ch);
  return;
  }
  if (!IS_SET(ch->newbits, NEW_JAWLOCK))
  {
  send_to_char("Your jaw grows stronger and more ready to clamp on your enemy.\n\r",ch);
  SET_BIT(ch->newbits, NEW_JAWLOCK);
  return;
  }
  else if (IS_SET(ch->newbits, NEW_JAWLOCK))
  { 
  send_to_char("Your jaw weakens.\n\r",ch);
  REMOVE_BIT(ch->newbits,NEW_JAWLOCK);
  return;
  }
return;
}



/* Syntax : chant <type> <color> <target>
 * ex. chant bless red self 
 */
void do_chant (CHAR_DATA *ch, char *argument)
{
  char arg1[MAX_STRING_LENGTH];
  char arg2[MAX_STRING_LENGTH];
  char buf1[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  char buf3[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  AFFECT_DATA af;
  int i, dam, count, sn;
  int red_magic = ch->spl[RED_MAGIC];
  int blue_magic = ch->spl[BLUE_MAGIC];
  int green_magic = ch->spl[GREEN_MAGIC];
  int purple_magic = ch->spl[PURPLE_MAGIC];
  int yellow_magic = ch->spl[YELLOW_MAGIC];
  int magic_power = ch->spl[RED_MAGIC] + ch->spl[BLUE_MAGIC] + ch->spl[GREEN_MAGIC] + ch->spl[PURPLE_MAGIC] + ch->spl[YELLOW_MAGIC];
    
  argument=one_argument(argument,arg1);
  strcpy(arg2,argument);

  if (IS_NPC(ch)) return;

  if (!IS_RACE(ch, RACE_OGREMAGE))
  {
    send_to_char("#0Chant is an Ogre-Mage Ability#n\n\r",ch);
    return;
  }
  if (arg1[0] == '\0')
  {  
    send_to_char("Syntax : chant <bless/curse/damage/heal> <target>.\n\r", ch);
    return;
  }  
  if (!str_cmp(arg1, "heal"))
  {
    if (arg2[0] == '\0') victim = ch;
    else if ((victim = get_char_room(ch, arg2)) == NULL)
    {
      send_to_char("They are not here.\n\r", ch );
      return;
    }
    if (ch->mana < 1500)
    {
      send_to_char("You need 1500 mana.\n\r",ch);
      return;
    }    
    act("$n channels lifeforce from the five elements into $N.",ch,NULL,victim,TO_NOTVICT);
    act("You channel lifeforce from the five elements into $N.",ch,NULL,victim,TO_CHAR);
    act("$n channels lifeforce from the five elements into you.",ch,NULL,victim,TO_VICT);
    WAIT_STATE(ch,8);
    ch->mana -= 1500;
    victim->hit = UMIN(victim->hit + (magic_power * 1.5), victim->max_hit);
    return;
  }
  else if (!str_cmp(arg1, "damage"))
  {
    if (arg2[0] == '\0' && ch->fighting != NULL) victim = ch->fighting;
    else if ((victim = get_char_room(ch, arg2)) == NULL)
    {
      send_to_char("They are not here.\n\r", ch );
      return;
    }
    if (ch->mana <1000)
    {
      send_to_char("You need 1000 mana.\n\r",ch);
      return;
    }
    if (victim == ch)
    {
      send_to_char("You really don't want to hurt yourself.\n\r", ch );
      return;
    }
    if (is_safe(ch, victim)) return;
    if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
      return;
    if (IS_AFFECTED(ch, AFF_PEACE)) REMOVE_BIT(ch->affected_by, AFF_PEACE);
    if (victim->position > POS_STUNNED)
    {
      if (victim->fighting == NULL) set_fighting(victim, ch);
      if (ch->fighting == NULL) set_fighting(ch, victim);
    }
    WAIT_STATE(ch,8);
    ch->mana -= 1000;
    dam = number_range(red_magic*4, red_magic*5);
    dam += char_damroll(ch);
    dam = cap_dam(ch,victim,dam);
    if (!IS_NPC(victim))
    {
//      if (IS_CLASS(ch, CLASS_MAGE) && dam > 600) dam = number_range(580,620);
      if (dam > 1000) dam = number_range(950,1050);
    }
    sprintf(buf1,"$n calls for the forces of #Rhell#n to destroy $N [#C%d#n]" , dam);
    sprintf(buf2,"You call for the forces of #Rhell#n to destroy $N [#C%d#n]" , dam);
    sprintf(buf3,"$n calls for the forces of #Rhell#n to destroy you [#C%d#n]" , dam);
    act(buf1,ch,NULL,victim, TO_NOTVICT);
    act(buf2,ch,NULL,victim, TO_CHAR);
    act(buf3,ch,NULL,victim, TO_VICT);
    hurt_person(ch, victim, dam);
    if (victim->position == POS_DEAD || ch->in_room != victim->in_room) return;
    dam = number_range(blue_magic*4, blue_magic*5);
    dam += char_damroll(ch);
    dam = cap_dam(ch,victim,dam);
    if (!IS_NPC(victim))
    {
//      if (IS_CLASS(ch, CLASS_MAGE) && dam > 600) dam = number_range(580,620);
      if (dam > 1000) dam = number_range(950,1050);
    }
    sprintf(buf1,"$n summons a supernatural #Lstorm#n to strike $N with lightning [#C%d#n]" , dam);
    sprintf(buf2,"You summon a supernatural #Lstorm#n to strike $N with lightning [#C%d#n]" , dam);
    sprintf(buf3,"$n summons a supernatural #Lstorm#n to strike you with lightning [#C%d#n]" , dam);
    act(buf1,ch,NULL,victim, TO_NOTVICT);
    act(buf2,ch,NULL,victim, TO_CHAR);
    act(buf3,ch,NULL,victim, TO_VICT);
    hurt_person(ch, victim, dam);
    if (victim->position == POS_DEAD || ch->in_room != victim->in_room) return;
    dam = number_range(green_magic*4, green_magic*5);
    dam += char_damroll(ch);
    dam = cap_dam(ch,victim,dam);
    if (!IS_NPC(victim))
    {
//      if (IS_CLASS(ch, CLASS_MAGE) && dam > 600) dam = number_range(580,620);
      if (dam > 1000) dam = number_range(950,1050);
    }
    sprintf(buf1,"$n calls for #Gthorns#n to grow from the earth and pierce $N [#C%d#n]" , dam);
    sprintf(buf2,"You call for #Gthorns#n to grow from the earth and pierce $N [#C%d#n]" , dam);
    sprintf(buf3,"$n calls for #Gthorns#n to grow from the earth and pierce You [#C%d#n]" , dam);
    act(buf1,ch,NULL,victim, TO_NOTVICT);
    act(buf2,ch,NULL,victim, TO_CHAR);
    act(buf3,ch,NULL,victim, TO_VICT);
    hurt_person(ch, victim, dam);
    if (victim->position == POS_DEAD || ch->in_room != victim->in_room) return;
    dam = number_range(purple_magic*4, purple_magic*5);
    dam += char_damroll(ch);
    dam = cap_dam(ch,victim,dam);
    if (!IS_NPC(victim))
    {
//      if (IS_CLASS(ch, CLASS_MAGE) && dam > 600) dam = number_range(580,620);
      if (dam > 1000) dam = number_range(950,1050);
    }
    sprintf(buf1,"$n calls on the powers of the #punderdark#n to shatter $N's bones [#C%d#n]" , dam);
    sprintf(buf2,"You call on the powers of the #punderdark#n to shatter $N's bones [#C%d#n]" , dam);
    sprintf(buf3,"$n calls on the powers of the #punderdark#n to shatter your bones [#C%d#n]" , dam);
    act(buf1,ch,NULL,victim, TO_NOTVICT);
    act(buf2,ch,NULL,victim, TO_CHAR);
    act(buf3,ch,NULL,victim, TO_VICT);
    hurt_person(ch, victim, dam);
    if (victim->position == POS_DEAD || ch->in_room != victim->in_room) return;
    dam = number_range(yellow_magic*4, yellow_magic*5);
    dam += char_damroll(ch);
    dam = cap_dam(ch,victim,dam);
    if (!IS_NPC(victim))
    {
//      if (IS_CLASS(ch, CLASS_MAGE) && dam > 600) dam = number_range(580,620);
      if (dam > 1000) dam = number_range(950,1050);
    }
    sprintf(buf1,"$n spews forth a shower of #yacid#n striking $N [#C%d#n]" , dam);
    sprintf(buf2,"you spew forth a shower of #yacid#n striking $N [#C%d#n]" , dam);
    sprintf(buf3,"$n spews forth a shower of #yacid#n striking you [#C%d#n]" , dam);
    act(buf1,ch,NULL,victim, TO_NOTVICT);
    act(buf2,ch,NULL,victim, TO_CHAR);
    act(buf3,ch,NULL,victim, TO_VICT);
    hurt_person(ch, victim, dam);
    return;
  }
  else if (!str_cmp(arg1, "bless"))
  {
    if ((victim = get_char_room(ch, arg2)) == NULL)
    {
      send_to_char("They are not here.\n\r", ch );
      return;
    }
    if (ch->mana <2500)
    {
      send_to_char("You need 2500 mana.\n\r",ch);
      return;
    }
    if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
      return;
    count = 0;
    for (i = 0; i<5 ; i++)
    {
      if (i==0) {sn = skill_lookup("purple sorcery");dam=purple_magic;}
      if (i==1) {sn = skill_lookup("yellow sorcery");dam=yellow_magic;}
      if (i==2) {sn = skill_lookup("green sorcery");dam=green_magic;}
      if (i==3) {sn = skill_lookup("red sorcery");dam=red_magic;}
      if (i==4) {sn = skill_lookup("blue sorcery");dam=blue_magic;}
      if (is_affected(victim, sn))
        continue;
      
      count++;
      
      af.type      = sn;
      af.duration  = dam/4;
      af.location  = APPLY_HIT;
      af.modifier  = dam*2;
      af.bitvector = 0;
      affect_to_char(victim, &af);
      af.type      = sn;
      af.duration  = dam/4;
      af.location  = APPLY_AC;
      af.modifier  = -dam/2;
      af.bitvector = 0;
      affect_to_char(victim, &af);
      af.type      = sn;
      af.duration  = dam/4;
      af.location  = APPLY_MOVE;
      af.modifier  = dam*2;
      af.bitvector = 0;
      affect_to_char(victim, &af);
      af.type      = sn;
      af.duration  = dam/4;
      af.location  = APPLY_MANA;
      af.modifier  = dam*2;
      af.bitvector = 0;
      affect_to_char(victim, &af);
      af.type      = sn;
      af.duration  = dam/4;
      af.location  = APPLY_HITROLL;
      af.modifier  = dam/3;
      af.bitvector = 0;
      affect_to_char(victim, &af);
      af.type      = sn;
      af.duration  = dam/4;
      af.location  = APPLY_DAMROLL;
      af.modifier  = dam/3;
      af.bitvector = 0;
      affect_to_char(victim, &af);
    }
    ch->mana -= count * 500;
    WAIT_STATE(ch,6);
    sprintf(buf1, "You bless $N with the power of the elements.");
    sprintf(buf2, "The power of the five elements fills your body.");
    act(buf1, ch, NULL, victim, TO_CHAR);
    act(buf2, ch, NULL, victim, TO_VICT);
    return;
  }
  else if (!str_cmp(arg1, "curse"))
  {
    if ((victim = get_char_room(ch, arg2)) == NULL)
    {
      send_to_char("They are not here.\n\r", ch );
      return;
    }
    if (ch->mana <2500)
    {
      send_to_char("You need 2500 mana.\n\r",ch);
      return;
    }
    if (victim->level < 3)
    {
      send_to_char("Not on mortal, thier bodies cant handles the strain....poor little buggers.\n\r",ch);
      return;
    }
    if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
      return;
    count = 0;
    for (i = 0;i<5;i++)
    {
      if (i==0) {sn = skill_lookup("purple sorcery");dam=purple_magic;}
      if (i==1) {sn = skill_lookup("yellow sorcery");dam=yellow_magic;}
      if (i==2) {sn = skill_lookup("green sorcery");dam=green_magic;}
      if (i==3) {sn = skill_lookup("red sorcery");dam=red_magic;}
      if (i==4) {sn = skill_lookup("blue sorcery");dam=blue_magic;}
      if (is_affected(victim, sn))
        continue;
      
      count++;

      af.type      = sn;
      af.duration  = dam/4;
      af.location  = APPLY_HIT;
      af.modifier  = -dam*2;
      af.bitvector = 0;
      affect_to_char(victim, &af);
      af.type      = sn;
      af.duration  = dam/4;
      af.location  = APPLY_AC;
      af.modifier  = dam/2;
      af.bitvector = 0;
      affect_to_char(victim, &af);
      af.type      = sn;
      af.duration  = dam/4;
      af.location  = APPLY_MOVE;
      af.modifier  = -dam*2;
      af.bitvector = 0;
      affect_to_char(victim, &af);
      af.type      = sn;
      af.duration  = dam/4;
      af.location  = APPLY_MANA;
      af.modifier  = -dam*2;
      af.bitvector = 0;
      affect_to_char(victim, &af);
      af.type      = sn;
      af.duration  = dam/4;
      af.location  = APPLY_HITROLL;
      af.modifier  = -dam/3;
      af.bitvector = 0;
      affect_to_char(victim, &af);
      af.type      = sn;
      af.duration  = dam/4;
      af.location  = APPLY_DAMROLL;
      af.modifier  = -dam/3;
      af.bitvector = 0;
      affect_to_char(victim, &af);
    }
    ch->mana -= count * 500;
    WAIT_STATE(ch,6);
    sprintf(buf1, "You curse $N with the power of the elements.");
    sprintf(buf2, "The power of the five elements wrecks your body.");
    act(buf1, ch, NULL, victim, TO_CHAR);
    act(buf2, ch, NULL, victim, TO_VICT);
    return;      
  }
  send_to_char("Syntax : chant <bless/curse/damage/heal> <target>.\n\r", ch);
  return;
}

void do_tunnel(CHAR_DATA * ch, char *argument)
{
        char      arg[MAX_INPUT_LENGTH];
        CHAR_DATA *victim;

        argument = one_argument(argument, arg);

        if (IS_NPC(ch))
                return;

        if (!IS_RACE(ch, RACE_MTDWARF) && !IS_RACE(ch, RACE_GULLYDWARF)
	  && !IS_RACE(ch, RACE_HILLDWARF) && !IS_RACE(ch, RACE_AQUATICDWARF)
	  && !IS_RACE(ch, RACE_ARCTICDWARF) && !IS_RACE(ch, RACE_DEEPDWARF)
	  && !IS_RACE(ch, RACE_DESERTDWARF) && !IS_RACE(ch, RACE_DREAMDWARF)
	  && !IS_RACE(ch, RACE_EARTHDWARF) && !IS_RACE(ch, RACE_GOLDDWARF) 
	  && !IS_RACE(ch, RACE_GREYDWARF) && !IS_RACE(ch, RACE_JUNGLEDWARF)
	  && !IS_RACE(ch, RACE_DUERGARDWARF))
        {
                send_to_char("#0This is a Dwarf Ability.#n\n\r", ch);
                return;
        }

        if (arg[0] == '\0')
        {
                send_to_char("#0Tunnel to who?#n\n\r", ch);
                return;
        }

        if (IS_SET(ch->in_room->room_flags, ROOM_ASTRAL))
        {
                stc("#0Your room is not connected to the astral plane.#n\n\r", ch);
                return;
        }

        if ((victim = get_char_world(ch, arg)) == NULL)
        {
                send_to_char("#0They are not here.#n\n\r", ch);
                return;
        }

        if (IS_IMMUNE(victim, IMM_TRAVEL))
        {
                send_to_char("#0They are immune to travel.#n\n\r", ch);
                return;
        }

        if (IS_SET(victim->act, ACT_NOTRAVEL))
        {
                send_to_char("#0No Can Do.#n\n\r", ch);
                return;
        }

        if (victim == ch)
        {
                send_to_char("#0You cannot tunnel to yourself.#n\n\r", ch);
                return;
        }

        if (victim->in_room == NULL)
        {
                send_to_char("#0Nothing happens.#n\n\r", ch);
                return;
        }
        if (IS_SET(victim->in_room->room_flags, ROOM_ASTRAL))
        {
                stc("#0Their room is not connected to the astral plane.#n\n\r", ch);
                return;
        }

        send_to_char("#0You tunnel into the ground.#n\n\r", ch);
        act("$n #0tunnels into the ground.#n", ch, NULL, NULL, TO_ROOM);
        char_from_room(ch);
        char_to_room(ch, victim->in_room);
        do_look(ch, "");
        send_to_char("#0You tunnel out of the ground.#n\n\r", ch);
        act("$n #0tunnels out of the ground.#n", ch, NULL, NULL, TO_ROOM);
        return;
}

void do_stoneshape(CHAR_DATA * ch, char *argument)
{
        OBJ_DATA *obj;
        char      arg[MAX_INPUT_LENGTH];
        char      buf[MAX_STRING_LENGTH];
        int       cost = 500;

        one_argument(argument, arg);

        if (IS_NPC(ch))
                return;
        if (!IS_RACE(ch, RACE_FIREGIANT)  && !IS_RACE(ch, RACE_HILLGIANT)
        && !IS_RACE(ch, RACE_FROSTGIANT))
        {
                send_to_char("#0This is a Giant Ability#n\n\r", ch);
                return;
        }
        if (arg[0] == '\0')
        {
                send_to_char("Stoneshape what?\n\r", ch);
                return;
        }
        if ((obj = get_obj_carry(ch, arg)) == NULL)
        {
                send_to_char("You dont have that item.\n\r", ch);
                return;
        }
        if ((obj->item_type != ITEM_WEAPON && obj->item_type != ITEM_ARMOR)
            || obj->chobj != NULL || IS_SET(obj->quest, QUEST_GIANTSTONE))
        {
                send_to_char("You are unable to stoneshape this item.\n\r", ch);
                return;
        }
        if (ch->pcgold < cost)
        {
                sprintf(buf,
                        "It costs %d bones to use this power, and you don't have that much.\n\r", cost);
                send_to_char(buf, ch);
                return;
        }
        if (obj->item_type == ITEM_WEAPON)
        {
                obj->value[1] += number_range(1, 8);
                obj->value[2] += number_range(3, 10);
        }
        forge_affect(obj, number_range(1, 5));
        SET_BIT(obj->quest, QUEST_GIANTSTONE);
        ch->pcgold -= cost;
        send_to_char
                ("You shape the ancient stone of the world onto the item, making it more poweful.\n\r", ch);
        return;
}



void do_earthpunch(CHAR_DATA * ch, char *argument)
{
        CHAR_DATA *gch;

        if (IS_NPC(ch))
                return;
        if (!IS_RACE(ch, RACE_FIREGIANT)  && !IS_RACE(ch, RACE_HILLGIANT)
        && !IS_RACE(ch, RACE_FROSTGIANT))
        {
                send_to_char("#0This is a giant Power#n\n\r", ch);
                return;
        }
        send_to_char
                ("You slam your fist into the ground creating a minor earthquake.\n\r",
                 ch);
        act("$n slams $m fist into the ground, creating a minor earthquake.",
            ch, NULL, NULL, TO_ROOM);
        for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room)
        {
                if (IS_NPC(gch))
                        continue;
                if (ch == gch)
                        continue;
                if (is_safe(ch, gch))
                        continue;
                if (gch->fighting != NULL)
                {
                        one_hit(ch, gch, gsn_bash, 0);
			if (number_range(1, 8) == 4)
			{
                        	send_to_char("You lose your balance.\n\r", gch);
                        	do_stance(gch, "");
			}
                }
                else
                {
                        send_to_char("You drop to the ground, stunned.\n\r",
                                     gch);
                        stop_fighting(gch, TRUE);
                        gch->position = POS_STUNNED;
                }
        }
        WAIT_STATE(ch, 16);
        return;
}



void do_gfavor( CHAR_DATA *ch, char *argument )
{
  char buf [MAX_STRING_LENGTH];

  if (IS_NPC(ch)) return;
  if (!IS_RACE(ch, RACE_ARCHANGEL))
  {  
    send_to_char("#0This is an ArchAngel Ability.#n\n\r",ch);
    return;
  }
  if (ch->alignment != 1000) ch->alignment = 1000;
  if (IS_SET(ch->newbits, NEW_CUBEFORM))
  {
    REMOVE_BIT(ch->newbits, NEW_CUBEFORM);
    REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
    act( "$n turns into a human.",ch,NULL,NULL,TO_ROOM);
    send_to_char("You turn into your normal form.\n\r",ch);
    ch->damroll -=400;
    ch->hitroll -=400;
    free_string(ch->morph);
    ch->morph=str_dup("");
    return;
  }
  if (ch->move < 2000 || ch->mana < 2000)
  {
    send_to_char("You need 2000 mana and 2000 move to make yourself a true angel.\n\r",ch);
    return;
  }
  else
  {
    act( "$n transforms into an angel.",ch,NULL,NULL,TO_ROOM);
    send_to_char("God is with you, your now one of his divine.\n\r",ch);
    SET_BIT(ch->newbits, NEW_CUBEFORM);
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    sprintf(buf,"%s the angel",ch->name);
    free_string(ch->morph);
    ch->morph = str_dup(buf);
    ch->move -= 2000;
    ch->mana -= 2000;
    ch->damroll +=400;
    ch->hitroll +=400;
    return;
  }
  return;
}

void do_demonform( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH];
    char buf [MAX_STRING_LENGTH]; 
    argument = one_argument( argument, arg ); 

    if (IS_NPC(ch)) return;

    if (!IS_RACE(ch, RACE_ARCHDEMON))
    {
	send_to_char("#0This is an ArchDemon Ability.#n\n\r",ch);
	return;
    }

    
    
    if (IS_POLYAFF(ch, POLY_ZULOFORM))
    {   
        REMOVE_BIT(ch->polyaff, POLY_ZULOFORM);
        REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
        act( "You transform into human form.", ch, NULL, NULL, TO_CHAR );
        act( "$n's shrinks back into human form.", ch, NULL, NULL, TO_ROOM);
        ch->damroll -= 200;
        ch->hitroll -= 200;
        ch->armor   += 300;
	free_string( ch->morph );
        ch->morph = str_dup( "" );
        return;
    }
    else if (IS_AFFECTED(ch,AFF_POLYMORPH))
    {
send_to_char("You cant demon form when changed.\n\r",ch);
return;
}
  
  
        if (ch->stance[0] != -1) do_stance(ch,"");
	if (ch->mounted == IS_RIDING) do_dismount(ch,"");
        act( "You transform into a huge demon.", ch, NULL, NULL, TO_CHAR);
        act( "$n's body grows and distorts into a huge demon.", ch, NULL, NULL, TO_ROOM );
        ch->pcdata->mod_str = 15;
        ch->pcdata->mod_dex = 15;
        SET_BIT(ch->polyaff, POLY_ZULOFORM);
        SET_BIT(ch->affected_by, AFF_POLYMORPH);
        sprintf(buf, "%s the huge hulking demon", ch->name);
        free_string( ch->morph );
        ch->morph = str_dup( buf );
	ch->damroll += 200;
	ch->hitroll += 200;
        ch->armor   -= 300;
        return;
    }
  


void do_drowfire( CHAR_DATA *ch, char *argument ) {

    CHAR_DATA *victim;
    char arg [MAX_INPUT_LENGTH];
    int sn;
    int level;
    int spelltype;

	argument = one_argument(argument, arg);

	if (IS_NPC(ch)) return;

	if(!IS_RACE(ch, RACE_DROW)){
		send_to_char("#0Drowfire is a Dark Elf Ability#n\n\r", ch );
		return;}

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( ch == victim )
    {
        send_to_char( "Not on yourself.\n\r", ch );
        return;
    }

	if (ch->mana < 100) {
	send_to_char("You don't have enough mana.\n\r", ch );
		return;}

    if ( ( sn = skill_lookup( "drowfire" ) ) < 0 ) return;
    spelltype = skill_table[sn].target;
    level = ch->spl[spelltype] * 1.5;
    (*skill_table[sn].spell_fun) ( sn, level, ch, victim );
    WAIT_STATE( ch, 12 );
	ch->mana = ch->mana - 100;
    return;
}

void do_darkness(CHAR_DATA *ch, char *argument) 
{
	bool blah = FALSE;

	if (IS_NPC(ch)) return;

    if (!IS_RACE(ch, RACE_DROW))
         {
          send_to_char("#0Darkness is a Drow Ability.#n\n\r", ch);
          return;
         }

	if (ch->fight_timer>0) {
	  send_to_char("Not until your fighttimer expires.\n\r",ch);
	  return;
	}

	if (IS_SET(ch->newbits, NEW_DARKNESS)) {
	send_to_char("You banish your globe of darkness.\n\r", ch );
	act("The globe of darkness around $n disappears.",ch,NULL,NULL,TO_ROOM);
	REMOVE_BIT(ch->newbits, NEW_DARKNESS);
	if (ch->in_room != NULL)
	if (IS_SET(ch->in_room->room_flags, ROOM_TOTAL_DARKNESS))
	REMOVE_BIT(ch->in_room->room_flags, ROOM_TOTAL_DARKNESS);
	return;
	}

	if (ch->mana < 500) {
	send_to_char("You don't have enough mana to summon the darkness.\n\r",ch );
	return;}

	send_to_char("You summon a globe of darkness.\n\r", ch );
	act("$n summons a globe of darkness.",ch,NULL,NULL,TO_ROOM);
	ch->mana -= 500;
	if (IS_SET(ch->extra, TIED_UP)) {
		REMOVE_BIT(ch->extra, TIED_UP);
		blah = TRUE;}

/*	if (IS_AFFECTED(ch, AFF_WEBBED)) {
		REMOVE_BIT(ch->affected_by, AFF_WEBBED);
		blah = TRUE;}
*/
	if (blah) send_to_char("The darkness sets you free.\n\r", ch );		

	SET_BIT(ch->newbits, NEW_DARKNESS);
	if (!IS_SET(ch->in_room->room_flags, ROOM_TOTAL_DARKNESS))
	SET_BIT(ch->in_room->room_flags, ROOM_TOTAL_DARKNESS);
	return;
}


void do_fly( CHAR_DATA *ch, char *argument )
{
	char arg[MAX_INPUT_LENGTH];
        CHAR_DATA *victim;
		  CHAR_DATA *mount;

	one_argument( argument, arg );

	if(!IS_IMMORTAL(ch))
	{
    if (!IS_RACE(ch, RACE_DRAGONKIND))
	 {
	  send_to_char("#0Fly is a Dragon Ability.#n\n\r", ch);
	  return;
	 }

	}

	if ( arg[0] == '\0' )
	 {
	  send_to_char( "Fly to whom?\n\r", ch );
	  return;
	 }

	if ( ch->position == POS_FIGHTING )
	 {
	  send_to_char( "No way! You are fighting.\n\r", ch );
	  return;
	 }

	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	 {
	  send_to_char( "Nobody by that name.\n\r", ch );
 	  return;
	 }

	if( victim == ch )
	 {
	  stc( "Not to yourself.\n\r",ch);
	  return;
	 }

	if ( victim->fight_timer == 0 && !IS_IMMUNE(victim, IMM_SUMMON) && !IS_NPC(victim) && !IS_IMMORTAL(ch))
	{
	  send_to_char("You can't find them.\n\r", ch);
	  return;
	}

	if(IS_SET(victim->in_room->room_flags, ROOM_ASTRAL) || IS_SET(ch->in_room->room_flags, ROOM_ASTRAL))
	  {
	   send_to_char("You can't find them.\n\r",ch);
	   return;
	  }
	act( "You disappear up into the sky.", ch, NULL, NULL, TO_CHAR );
	act( "$n disappears up into the sky.", ch, NULL, NULL, TO_ROOM );
	char_from_room( ch );
	char_to_room( ch, get_room_index(victim->in_room->vnum) );
	act( "You swoop down and land infront of $N", ch, NULL, victim, TO_CHAR );
	act( "$n swoop down and land infront of $N.", ch, NULL, victim, TO_ROOM );
	do_look( ch, "auto" );
	WAIT_STATE(ch,4);
	if ( (mount = victim->mount) == NULL ) return;
	char_from_room( mount );
	char_to_room( mount, get_room_index(victim->in_room->vnum) );
	return;
}


void do_ascension(CHAR_DATA *ch, char *argument)
{
  char arg1[MAX_STRING_LENGTH];
  argument = one_argument( argument, arg1 );

  if (IS_NPC(ch))return;

  if (!IS_NPC(ch) && ch->pcRaceLevel < 200)
  {   
    send_to_char("You must be level 200 to Ascend\n\r",ch);
    return;
  }
  if (ch->level < 3)
  {
    send_to_char("You must be avatar to Ascend.\n\r",ch);
    return;
  }
if ( arg1[0] == '\0' )
{
  send_to_char("Ascension  : Type Ascend (race) to choose your Demi-God Race.\n\r\n\r",ch);
  send_to_char("Options are ArchDemon or ArchAngel.\n\r\n\r",ch);
  return;
}

do_clearstats2(ch,"");

if (!str_cmp(arg1,"archdemon"))
{
	ch->pcRace = RACE_ARCHDEMON;
	send_to_char( "You have Become An Arch-Demon Demi-God.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"archangel"))
{
        ch->pcRace = RACE_ARCHANGEL;
        send_to_char( "You have Become an ArchAngel.\n\r", ch);
        return;
}

}


void do_racepowers(CHAR_DATA * ch, char *argument)
{

	if (IS_NPC(ch))
	return;

	send_to_char("[--------------------[Race Powers]--------------------]\n\r", ch);

        if (IS_RACE(ch, RACE_MTDWARF) || IS_RACE(ch, RACE_GULLYDWARF)
	  || IS_RACE(ch, RACE_HILLDWARF) || IS_RACE(ch, RACE_AQUATICDWARF)
	  || IS_RACE(ch, RACE_ARCTICDWARF) || IS_RACE(ch, RACE_DEEPDWARF)
	  || IS_RACE(ch, RACE_DESERTDWARF) || IS_RACE(ch, RACE_DREAMDWARF)
	  || IS_RACE(ch, RACE_EARTHDWARF) || IS_RACE(ch, RACE_GOLDDWARF) 
	  || IS_RACE(ch, RACE_GREYDWARF) || IS_RACE(ch, RACE_JUNGLEDWARF)
	  || IS_RACE(ch, RACE_DUERGARDWARF))
 	{
	send_to_char("#R(#0ThunderHammer#R)#0 Charge an ethereal Hammer and send a shockwave.#n\n\r", ch);
	send_to_char("#R(#0Tunnel#R)#0 Racial Travel.#n\n\r", ch);
	}
        if (IS_RACE(ch, RACE_GOBLIN) || IS_RACE(ch, RACE_HOBGOBLIN))
        {
        send_to_char("#R(#0Goblingrenade#R)#0 Drop a Goblin Grenade.#n\n\r", ch);
        }
        if (IS_RACE(ch, RACE_MINOTAUR))
        {
        send_to_char("#R(#0Gore#R)#0 Gore your opponent!!#n\n\r", ch);
        }
        if (IS_RACE(ch, RACE_KENDER))
        {
        send_to_char("#R(#0Pirate#R)#0 Steal equipment from your opponent!!#n\n\r", ch);
        }
        if (IS_RACE(ch, RACE_GNOLL))
        {
        send_to_char("#R(#0Jawlock#R)#0 Clamp onto Enemy, preventing Flee!!#n\n\r", ch);
        }
        if (IS_RACE(ch, RACE_OGREMAGE))
        {
        send_to_char("#R(#0Chant#R)#0 Damage, Heal, Curse, Bless#n\n\r", ch);
        }
        if (IS_RACE(ch, RACE_DRAGONKIND))
        {
        send_to_char("#R(#0Fly#R)#0 Racial Travel method.#n\n\r", ch);
        }
	  if (IS_RACE(ch, RACE_GNOME))
        {
        send_to_char("#R(#0AirshipTravel#R)#0 Supreme method of travel.#n\n\r", ch);
        }
        if (IS_RACE(ch, RACE_MTDWARF) || IS_RACE(ch, RACE_GULLYDWARF)
	  || IS_RACE(ch, RACE_HILLDWARF) || IS_RACE(ch, RACE_AQUATICDWARF)
	  || IS_RACE(ch, RACE_ARCTICDWARF) || IS_RACE(ch, RACE_DEEPDWARF)
	  || IS_RACE(ch, RACE_DESERTDWARF) || IS_RACE(ch, RACE_DREAMDWARF)
	  || IS_RACE(ch, RACE_EARTHDWARF) || IS_RACE(ch, RACE_GOLDDWARF) 
	  || IS_RACE(ch, RACE_GREYDWARF) || IS_RACE(ch, RACE_JUNGLEDWARF)
	  || IS_RACE(ch, RACE_DUERGARDWARF) || IS_RACE(ch, RACE_ELF) 
	  || IS_RACE(ch, RACE_HALFELF) || IS_RACE(ch, RACE_GNOME) 
	  || IS_RACE(ch, RACE_DROW) || IS_RACE(ch, RACE_WILDELF) 
	  || IS_RACE(ch, RACE_ARCHANGEL) || IS_RACE(ch, RACE_ARCHDEMON))
        {
        send_to_char("#R(#0Infravision#R)#0 The Ability to see at night.#n\n\r", ch);
        }
	if (IS_RACE(ch, RACE_ELF) || IS_RACE(ch, RACE_WILDELF)
	|| IS_RACE(ch, RACE_HALFELF) || IS_RACE(ch, RACE_DROW))
	{
        send_to_char("#R(#0ElfSing#R)#0 Sing Songs.#n\n\r", ch);
        send_to_char("#R(#0Lessons#R)#0 Learn Songs.#n\n\r", ch);
        send_to_char("#R(#0Songs#R)#0 Listing of Songs.#n\n\r", ch);
	}
        if (IS_RACE(ch, RACE_TROLL) || IS_RACE(ch, RACE_HALFTROLL)
        || IS_RACE(ch, RACE_ARCHANGEL) || IS_RACE(ch, RACE_ARCHDEMON)
        || IS_RACE(ch, RACE_DRAGONKIND))
        {
        send_to_char("#R(#0Toughskin#R)#0 Have your Skin take a Leathery Texture.#n\n\r", ch);
        }
	if (IS_RACE(ch, RACE_ARCHDEMON))
        {
        send_to_char("#R(#0Demonform#R)#0 Change into a Monsterous Demon#n\n\r", ch);
        }
	if(IS_RACE(ch, RACE_ARCHANGEL))
	{
	send_to_char("#R(#0gfavor#R)#0 Angel Form#n\n\r", ch);
	}
        if(IS_RACE(ch, RACE_FIREGIANT) || IS_RACE(ch, RACE_FROSTGIANT) || IS_RACE(ch, RACE_HILLGIANT))
        {
        send_to_char("#R(#0Earthpunch#R)#0 Bash/Stun those in the room.#n\n\r", ch);
        send_to_char("#R(#0Stoneshape#R)#0 Increase average damage of weapon.#n\n\r", ch);
        }
        send_to_char("[--------------------[  level 25  ]-------------------]\n\r", ch);
        if (IS_RACE(ch, RACE_MINOTAUR) || IS_RACE(ch, RACE_OGRE)
        || IS_RACE(ch, RACE_HALFOGRE) || IS_RACE(ch, RACE_GIANT))
        {
        send_to_char("#R(#0Tremble#R)#0 The ability to destance your opponent.#n\n\r", ch);
        }
	send_to_char("[--------------------[  level 50  ]-------------------]\n\r", ch);
        if (ch->pcRaceLevel >= 50)
        {
        send_to_char("#R(#0Mistform#R)#0 The Ability to turn Ethereal.#n\n\r", ch);
        send_to_char("#R(#0Flameblade#R)#0 The Power to surround a blade with Flames.#n\n\r", ch);
        }
        send_to_char("[--------------------[Race Powers]--------------------]\n\r", ch);


	return;
}


void do_raceself(CHAR_DATA *ch, char *argument)
{
  char arg1[MAX_STRING_LENGTH];
  argument = one_argument( argument, arg1 );

  if (IS_NPC(ch))return;

	if (ch->pcdata->pc_eyes == 0)
	ch->pcdata->pc_eyes = number_range(1,10);
        if (ch->pcdata->pc_looks == 0)
	ch->pcdata->pc_looks = number_range(1,4);
        if (ch->pcdata->pc_hair == 0)
	ch->pcdata->pc_hair = number_range(1,11);
        if (ch->pcdata->pc_build == 0)
	ch->pcdata->pc_build = number_range(1,4);

  if (!IS_NPC(ch) && ch->pcRace != 0)
  {   
    send_to_char("You already have a race.\n\r",ch);
    return;
  }
  if (ch->level < 3)
  {
    send_to_char("You must be avatar to selfrace.\n\r",ch);
    return;
  }
if ( arg1[0] == '\0' )
{
  send_to_char("Races: Type selfrace (race) to choose your Race.\n\r\n\r",ch);
  send_to_char("#0<<--------------------------------------------------------------------------------->> \n\r",ch);
  send_to_char("#0BugBear			Giant, Fire		Kender\n\r",ch);
  send_to_char("#0Centaur			Giant, Frost		Kobold, Aquatic\n\r",ch);
  send_to_char("#0Dwarf, Aquatic		Giant, Hill		Kobold, Arctic\n\r",ch);
  send_to_char("#0Dwarf, Arctic		Gnome, Air		Kobold, Desert\n\r",ch);
  send_to_char("#0Dwarf, Deep		Gnome, Aquatic		Kobold, Earth\n\r",ch);
  send_to_char("#0Dwarf, Desert		Gnome, Arctic		Kobold, Jungle\n\r",ch);
  send_to_char("#0Dwarf, Dream		Gnome, Chaos		Minotaur\n\r",ch);
  send_to_char("#0Dwarf, Earth		Gnome, Deep		Ogre\n\r",ch);
  send_to_char("#0Dwarf, Gold		Gnome, Desert		OgreMage\n\r",ch);
  send_to_char("#0Dwarf, Grey		Gnome, Forest		Orc\n\r",ch);
  send_to_char("#0Dwarf, Hill		Gnome, Jungle		Pixie\n\r",ch);
  send_to_char("#0Dwarf, Jungle		Gnome, Rock		Troll\n\r",ch);
  send_to_char("#0Elf, Aquatic		Goblin, Air\n\r",ch);
  send_to_char("#0Elf, Arctic		Goblin, Aquatic\n\r",ch);
  send_to_char("#0Elf, Dark(drow)		Goblin, Arctic\n\r",ch);
  send_to_char("#0Elf, Desert		Goblin, Desert\n\r",ch);
  send_to_char("#0Elf, Fire		Goblin, Forestkith\n\r",ch);
  send_to_char("#0Elf, Ghost		Goblin, Jungle\n\r",ch);
  send_to_char("#0Elf, Gray		Half-Elf\n\r",ch);
  send_to_char("#0Elf, High		Half-Ogre\n\r",ch);
  send_to_char("#0Elf, Jungle		Half-Orc\n\r",ch);
  send_to_char("#0Elf, Wild		Half-Troll\n\r",ch);
  send_to_char("#0Elf, Wood		Hobbit\n\r",ch);
  send_to_char("#0Gnoll			Human\n\r",ch);
  send_to_char("#0<<--------------------------------------------------------------------------------->> \n\r",ch);

  return;
}

do_clearstats2(ch,"");


if (!str_cmp(arg1,"human"))
{
	ch->pcRace = RACE_HUMAN;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You have Become Human.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"highelf"))
{
	ch->pcRace = RACE_ELF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You have Become a High Elf.\n\r", ch);
        return;
}
/*else if (!str_cmp(arg1,"gnome"))
{
	ch->pcRace = RACE_GNOME;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You have Become a Gnome.\n\r", ch);
        return;
}
*/
else if (!str_cmp(arg1,"gullydwarf"))
{
	ch->pcRace = RACE_GULLYDWARF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You have Become a Gully Dwarf.\n\r", ch);
        return;
}
else if (!str_cmp(arg1,"hilldwarf"))
{
	ch->pcRace = RACE_HILLDWARF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You have Become a Hill Dwarf.\n\r", ch);
        return;
}
else if (!str_cmp(arg1,"mountaindwarf"))
{
	ch->pcRace = RACE_MTDWARF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You have Become a MT. Dwarf.\n\r", ch);
        return;
}
else if (!str_cmp(arg1,"dragonkind"))
{
	ch->pcRace = RACE_DRAGONKIND;
	ch->pcsize = SIZE_GIANT;
	send_to_char( "You have Become Dragonkind.\n\r", ch);
        return;
}
else if (!str_cmp(arg1,"hobbit"))
{
	ch->pcRace = RACE_HOBBIT;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You have become a Shireling.\n\r", ch);
        return;
}
else if (!str_cmp(arg1,"kender"))
{
	ch->pcRace = RACE_KENDER;
	send_to_char( "Your Feet feel the calling of Wanderlust.\n\r", ch);
        return;
}
else if (!str_cmp(arg1,"drow"))
{
	ch->pcRace = RACE_DROW;
	ch->pcdata->pc_eyes = 2;
	ch->pcdata->pc_looks = 5;
	ch->pcdata->pc_hair = number_range(2,3);
	ch->pcdata->pc_build = 1;
	send_to_char( "The Suns becomes Brilliant and painful to your eyes.\n\r", ch);
        return;
}
/*
else if (!str_cmp(arg1,"giant"))
{
	ch->pcRace = RACE_GIANT;
	send_to_char( "You choose to stay a Giant.\n\r", ch);
        return;
}
*/
else if (!str_cmp(arg1,"halfelf"))
{
	ch->pcRace = RACE_HALFELF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Half Elf.\n\r", ch);
        return;
}
else if (!str_cmp(arg1,"ogre"))
{
	ch->pcRace = RACE_OGRE;
	ch->pcsize = SIZE_LARGE;
	send_to_char( "You are an Ogre.\n\r", ch);
        return;
}
else if (!str_cmp(arg1,"halfogre"))
{
	ch->pcRace = RACE_HALFOGRE;
	ch->pcsize = SIZE_LARGE;
	send_to_char( "You are a Half Ogre.\n\r", ch);
        return;
}
else if (!str_cmp(arg1,"ogremage"))
{
	ch->pcRace = RACE_OGREMAGE;
	ch->pcsize = SIZE_LARGE;
	send_to_char( "You are an Ogre Mage.\n\r", ch);
        return;
}
else if (!str_cmp(arg1,"minotaur"))
{
	ch->pcRace = RACE_MINOTAUR;
	ch->pcsize = SIZE_LARGE;
	send_to_char( "You grow in stature, Your skin turns red and Bull Horns sprout from your head.\n\r", ch);
        return;
}
/*else if (!str_cmp(arg1,"goblin"))
{
	ch->pcRace = RACE_GOBLIN;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a stinky, thieving little goblin.\n\r", ch);
	return;
}
*/
else if (!str_cmp(arg1,"airgoblin"))
{
	ch->pcRace = RACE_AIRGOBLIN;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a stinky, thieving little goblin.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"aquaticgoblin"))
{
	ch->pcRace = RACE_AQUATICGOBLIN;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a stinky, thieving little goblin.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"arcticgoblin"))
{
	ch->pcRace = RACE_ARCTICGOBLIN;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a stinky, thieving little goblin.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"desertgoblin"))
{
	ch->pcRace = RACE_DESERTGOBLIN;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a stinky, thieving little goblin.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"forestkith"))
{
	ch->pcRace = RACE_GOBLINFORESTKITH;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a stinky, thieving little goblin.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"junglegoblin"))
{
	ch->pcRace = RACE_JUNGLEGOBLIN;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a stinky, thieving little goblin.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"hobgoblin"))
{
	ch->pcRace = RACE_HOBGOBLIN;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Hobgoblin Commander of a Goblin Squad.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"duergar"))
{
	ch->pcRace = RACE_DUERGARDWARF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are now a Duergar Dwarf.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"troll"))
{
	ch->pcRace = RACE_TROLL;
	ch->pcsize = SIZE_LARGE;
	SET_BIT(ch->vuln, VULN_FIRE);
	send_to_char( "You are now a Troll.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"orc"))
{
	ch->pcRace = RACE_ORC;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are now an Orc.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"halforc"))
{
	ch->pcRace = RACE_HALFORC;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are now a Half-Orc.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"centaur"))
{
	ch->pcRace = RACE_CENTAUR;
	ch->pcsize = SIZE_LARGE;
	send_to_char( "you are now a Centaur.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"wildelf"))
{
	ch->pcRace = RACE_WILDELF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Wild Elf.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"aquaticelf"))
{
	ch->pcRace = RACE_AQUATICELF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are an Aquatic Elf.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"arcticelf"))
{
	ch->pcRace = RACE_ARCTICELF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are an Arctic Elf.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"desertelf"))
{
	ch->pcRace = RACE_DESERTELF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Desert Elf.\n\r", ch);
	return;
} 
else if (!str_cmp(arg1,"fireelf"))
{
	ch->pcRace = RACE_FIREELF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Fire Elf.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"ghostelf"))
{
	ch->pcRace = RACE_GHOSTELF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Ghost Elf.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"grayelf"))
{
	ch->pcRace = RACE_GRAYELF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Gray Elf.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"jungleelf"))
{
	ch->pcRace = RACE_JUNGLEELF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Jungle Elf.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"woodelf"))
{
	ch->pcRace = RACE_WOODELF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Wood Elf.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"aquaticdwarf"))
{
	ch->pcRace = RACE_AQUATICDWARF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are an Aquatic Dwarf.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"arcticdwarf"))
{
	ch->pcRace = RACE_ARCTICDWARF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are an Arctic Dwarf.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"deepdwarf"))
{
	ch->pcRace = RACE_DEEPDWARF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Deep Dwarf.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"desertdwarf"))
{
	ch->pcRace = RACE_DESERTDWARF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Desert Dwarf.\n\r", ch);
	return;
}
else if (!str_cmp(arg1, "dreamdwarf"))
{
	ch->pcRace = RACE_DREAMDWARF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Dream Dwarf.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"earthdwarf"))
{
	ch->pcRace = RACE_EARTHDWARF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Earth Dwarf.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"golddwarf"))
{
	ch->pcRace = RACE_GOLDDWARF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Gold Dwarf.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"greydwarf"))
{
	ch->pcRace = RACE_GREYDWARF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Grey Dwarf.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"jungledwarf"))
{
	ch->pcRace = RACE_JUNGLEDWARF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Jungle Dwarf.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"aquatickobold"))
{
	ch->pcRace = RACE_AQUATICKOBOLD;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are an Aquatic Kobold.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"arctickobold"))
{
	ch->pcRace = RACE_ARCTICKOBOLD;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are an Arctic Kobold.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"desertkobold"))
{
	ch->pcRace = RACE_DESERTKOBOLD;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a Desert Kobold.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"earthkobold"))
{
	ch->pcRace = RACE_EARTHKOBOLD;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a Earth Kobold.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"junglekobold"))
{
	ch->pcRace = RACE_JUNGLEKOBOLD;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a Jungle Kobold.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"hillgiant"))
{
	ch->pcRace = RACE_HILLGIANT;
	ch->pcsize = SIZE_GIANT;
	send_to_char( "You are a Hill Giant.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"firegiant"))
{
	ch->pcRace = RACE_FIREGIANT;
	ch->pcsize = SIZE_GIANT;
	send_to_char( "You are a Fire Giant.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"frostgiant"))
{
	ch->pcRace = RACE_FROSTGIANT;
	ch->pcsize = SIZE_GIANT;
	send_to_char( "You are a Frost Giant.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"pixie"))
{
	ch->pcRace = RACE_PIXIE;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a Pixie.\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"aquaticgnome"))
{
	ch->pcRace = RACE_AQUATICGNOME;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are an Aquatic Gnome\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"arcticgnome"))
{
	ch->pcRace = RACE_ARCTICGNOME;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are an Arctic Gnome\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"chaosgnome"))
{
	ch->pcRace = RACE_CHAOSGNOME;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a Chaos Gnome\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"deepgnome"))
{
	ch->pcRace = RACE_DEEPGNOME;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a Deep Gnome\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"desertgnome"))
{
	ch->pcRace = RACE_DESERTGNOME;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a Desert Gnome\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"forestgnome"))
{
	ch->pcRace = RACE_FORESTGNOME;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a Forest Gnome\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"junglegnome"))
{
	ch->pcRace = RACE_JUNGLEGNOME;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a Jungle Gnome\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"rockgnome"))
{
	ch->pcRace = RACE_ROCKGNOME;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a Rock Gnome\n\r", ch);
	return;
}
else if (!str_cmp(arg1,"bugbear"))
{
	ch->pcRace = RACE_BUGBEAR;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Bugbear\n\r", ch);
	return;
}









































 else do_raceself(ch,"");   
  return;
}


void do_elveninfra(CHAR_DATA *ch, char *argument)
{

	if (!IS_RACE(ch, RACE_ELF) && !IS_RACE(ch, RACE_HALFELF)
	&& !IS_RACE(ch, RACE_GNOME) && !IS_RACE(ch, RACE_DUERGARDWARF)
	&& !IS_RACE(ch, RACE_GULLYDWARF) && !IS_RACE(ch, RACE_HILLDWARF)
	&& !IS_RACE(ch, RACE_MTDWARF) && !IS_RACE(ch, RACE_DROW)
	&& !IS_RACE(ch, RACE_WILDELF) && !IS_RACE(ch, RACE_ARCHANGEL)
	&& !IS_RACE(ch, RACE_ARCHDEMON))
          {
             send_to_char("huh?\n\r",ch);
             return;
          }
               if (IS_SET(ch->act, PLR_HOLYLIGHT))
                {
                        REMOVE_BIT(ch->act, PLR_HOLYLIGHT);
                        send_to_char("Your Infravision Fades.\n\r",
                                     ch);
                }
                else
                {
                        SET_BIT(ch->act, PLR_HOLYLIGHT);
                        send_to_char("Your eyes glow red with Infravision.\n\r",
                                     ch);
                }
	 return;
}



void do_mistform(CHAR_DATA * ch, char *argument)
{
        if (IS_NPC(ch))
                return;
        if (ch->pcRaceLevel < 50)
        {
                send_to_char("You need your race level to 50 to use this.\n\r", ch);
                return;
        }
        if (IS_AFFECTED(ch, AFF_ETHEREAL))
        {
                REMOVE_BIT(ch->affected_by, AFF_ETHEREAL);
                act("You shift back int your solid material form.", ch, NULL, NULL,
                    TO_CHAR);
                act("$n regains $s form and re-enters their solid form.", ch, NULL,
                    NULL, TO_ROOM);
                return;
        }
        if (has_timer(ch))
                return;
        SET_BIT(ch->affected_by, AFF_ETHEREAL);
        act("You shift into the ethereal realm.", ch, NULL, NULL, TO_CHAR);
        act("$n turns ethereal.", ch, NULL, NULL, TO_ROOM);
        return;
}


void do_goblingrenade(CHAR_DATA * ch, char *argument)
{
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;
        int       dam;
        int       level;


        if (IS_NPC(ch))
                return;

	if (!IS_RACE(ch, RACE_GOBLIN) && !IS_RACE(ch, RACE_HOBGOBLIN))
	{
                send_to_char("Huh?\n\r", ch);
                return;
        }

        if (ch->mana < 1150)
        {
                send_to_char("You need more mana.\n\r", ch);
                return;
        }

        level = ch->spl[PURPLE_MAGIC];
        ch->mana -= 1150;

        send_to_char
                ("You pull the pin on a Goblin Grenade.\n\r",
                 ch);
        act("$n drops a Goblin Grenade!", ch, NULL, NULL, TO_ROOM);

        for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
        {
                vch_next = vch->next_in_room;

                if (vch == ch)
                        continue;
                if (vch->trust > 6)
                        continue;
                dam = dice(level, 40);
                if (saves_spell(level, vch))
                        dam /= 2;
                damage(ch, vch, dam, skill_lookup("earthquake"));
        }
        WAIT_STATE(ch, 9);
        return;
}


void do_airshiptravel(CHAR_DATA * ch, char *argument)
{
        char      arg[MAX_INPUT_LENGTH];
        CHAR_DATA *victim;

        argument = one_argument(argument, arg);

        if (IS_NPC(ch))
                return;

        if (!IS_RACE(ch, RACE_GNOME))
        {
                send_to_char("Huh?\n\r", ch);
                return;
        }

        if (arg[0] == '\0')
        {
                send_to_char("Airship Travel to who?\n\r", ch);
                return;
        }

        if ((victim = get_char_world(ch, arg)) == NULL)
        {
                send_to_char("They are not here.\n\r", ch);
                return;
        }

        if (victim == ch)
        {
                send_to_char("You cannot travel to yourself.\n\r", ch);
                return;
        }

        if (victim->in_room == NULL)
        {
                send_to_char("Nothing happens.\n\r", ch);
                return;
        }

        send_to_char("You set off a signal flare and an airship picks you up.\n\r", ch);
        act("$n sets off a signal flare and is picked up by an airship.", ch, NULL, NULL, TO_ROOM);
        char_from_room(ch);
        char_to_room(ch, victim->in_room);
        do_look(ch, "");
        send_to_char("You are dropped off by a Gnome Airship.\n\r", ch);
        act("$n is dropped off by a Gnome Airship.", ch, NULL, NULL, TO_ROOM);
        return;
}


void do_thunderhammer(CHAR_DATA * ch, char *argument)
{

	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	CHAR_DATA *vch;
	CHAR_DATA *vch_next;
	int dam;
	dam = 6000;
	dam *= 4;
	dam /= 2;
	
	if(IS_NPC(ch)) return;
	
        if (!IS_RACE(ch, RACE_MTDWARF) && !IS_RACE(ch, RACE_GULLYDWARF)
	  && !IS_RACE(ch, RACE_HILLDWARF) && !IS_RACE(ch, RACE_AQUATICDWARF)
	  && !IS_RACE(ch, RACE_ARCTICDWARF) && !IS_RACE(ch, RACE_DEEPDWARF)
	  && !IS_RACE(ch, RACE_DESERTDWARF) && !IS_RACE(ch, RACE_DREAMDWARF)
	  && !IS_RACE(ch, RACE_EARTHDWARF) && !IS_RACE(ch, RACE_GOLDDWARF) 
	  && !IS_RACE(ch, RACE_GREYDWARF) && !IS_RACE(ch, RACE_JUNGLEDWARF)
	  && !IS_RACE(ch, RACE_DUERGARDWARF))
	{
		stc("#0This is a Dwarven Area Attack#n\n\r",ch);
		return;
	}
	for ( vch = ch->in_room->people; vch!=NULL;vch = vch_next)
	      {
	        vch_next = vch->next_in_room;
	        if (can_see(ch,vch))
	        if ( IS_NPC(ch) ? !IS_NPC(vch) : IS_NPC(vch) )
	        {
		if(is_safe(ch,vch)) continue;
	          sprintf(buf, "#0Your body is struck by lightning! [#C%d#n]\n\r", dam);
	          send_to_char( buf, vch );
	          sprintf(buf2, "#r%s's#0 body is struck by lightning! [#C%d#n]\n\r",vch->name, dam);
	          send_to_char( buf2, ch );
	          set_fighting(ch,vch);
	          hurt_person(ch,vch,dam);
	        }
	      }
	
	WAIT_STATE(ch,8);

}




void do_lessons(CHAR_DATA *ch, char *argument)
{
 char buf[MAX_INPUT_LENGTH];
 int cost;
 int cost2;
 cost = ch->pcdata->powers[BARD_LESSONS] * 100000;  
 cost2 = cost - ch->exp;


if (!IS_RACE(ch, RACE_ELF) && !IS_RACE(ch, RACE_WILDELF)
&& !IS_RACE(ch, RACE_HALFELF) && !IS_RACE(ch, RACE_DROW))
{
 send_to_char("Huh?\n\r", ch);
 return;
}

if (argument[0] == '\0')
{
 send_to_char("Options are: show, improve.\n\r", ch);
 return;
}
if (!str_cmp(argument, "show"))
{
 sprintf(buf, "Your skill is currently at %d.\n\r", ch->pcdata->powers[BARD_LESSONS]);
 send_to_char(buf, ch);
}
else if (!str_cmp(argument, "improve"))
{
 if (ch->exp < cost)
 {
  sprintf(buf, "You need %d more experience.\n\r", cost2);
  send_to_char(buf, ch);
  return;
 }
 if (ch->pcdata->powers[BARD_LESSONS] >= 10)
 {
  send_to_char("You're already as skilled as you can become.\n\r", ch);
  return;
 }
 ch->exp -= cost;
 send_to_char("You learn a little more about the arts.\n\r", ch);
 ch->pcdata->powers[BARD_LESSONS]++;
 return;
}
else 
{
 send_to_char("Try improve or show.\n\r", ch);
 return;
}
return;
}

void do_elfsing(CHAR_DATA *ch, char *argument) 
{
 char buf[MAX_INPUT_LENGTH];
 char arg1[MAX_INPUT_LENGTH];
 char arg2[MAX_INPUT_LENGTH];
  int chance;
  int dam[4];
 CHAR_DATA *victim;
 argument = one_argument(argument, arg1);
 argument = one_argument(argument, arg2);



if (!IS_RACE(ch, RACE_ELF) && !IS_RACE(ch, RACE_WILDELF)
&& !IS_RACE(ch, RACE_HALFELF) && !IS_RACE(ch, RACE_DROW))
{
 send_to_char("Huh?\n\r", ch);
 return;
}

 if (arg1[0] == '\0')
 {
  send_to_char("Syntax: sing (song) (target)\n\r", ch);
  send_to_char("Possible Songs: \n\r", ch);
  send_to_char("Minuet, Minne, Madrigal, March, Dissonance \n\r", ch);
  send_to_char("Requiem, Paeon, Ballad, Lullaby, Hymnus, Spiritual \n\r", 
ch);
  return;
 }
  if (str_cmp(arg1, "minuet") && str_cmp(arg1, "minne") && str_cmp(arg1, "madrigal") &&
str_cmp(arg1, "march") && str_cmp(arg1, "requiem") && str_cmp(arg1, "paeon") &&
str_cmp(arg1, "ballad") && str_cmp(arg1, "lullaby") && str_cmp(arg1, "hymnnus") &&
str_cmp(arg1, "spiritual") && str_cmp(arg1, "dissonance"))
  {
   send_to_char("Possible Songs: Minuet, Minne, Madrigal, March, Requiem, Paeon,\n\r", ch);
   send_to_char("Ballad, Lullaby, Hymnus, Dissonance, Spiritual\n\r", ch);
   return;
  }
  
  if (!str_cmp(arg2, "self") || !str_cmp(arg2, ch->name))
   victim = ch;
  else if ((victim = get_char_room(ch, arg2)) == NULL)
  {
   send_to_char("They aren't here.\n\r", ch);
   return;
  }
  
  if (ch->pcdata->powers[SONG_MASTERY] < 1)
   ch->pcdata->powers[SONG_MASTERY] = 1;

  chance = number_range(1, ch->pcdata->powers[BARD_LESSONS]);
  if (chance == 1)
  {
   if (ch->pcdata->powers[SONG_MASTERY] < 10)
   {
    send_to_char("Your skill at singing improves.\n\r", ch);
    ch->pcdata->powers[SONG_MASTERY]++;
   }
  }

  if (!str_cmp(arg1, "minuet"))
  {
   if (IS_NPC(victim))
   { 
    send_to_char("Not on NPCs.\n\r", ch);
    return;
   }
   if (victim->pcdata->songs[SONG_MINUET] > 0)
   {
    send_to_char("They're already affected!\n\r", ch);
    return;
   }
   sprintf(buf, "#nYou sing a wonderful #rminuet#n to %s.\n\r", victim->name);
   send_to_char(buf, ch);
   sprintf(buf, "#n%s sings a wonderful #rminuet#n to you.\n\r", ch->name);
   send_to_char(buf, victim);
   victim->pcdata->songs[SONG_MINUET] += (50 * ch->pcdata->powers[SONG_MASTERY]);
   victim->damroll += (50 * ch->pcdata->powers[SONG_MASTERY]);
   return;
  }
  else if (!str_cmp(arg1, "minne"))
  {
   if (IS_NPC(victim))
   {
    send_to_char("Not on NPCs.\n\r", ch);
    return;
   }
   if (ch->pcdata->powers[BARD_LESSONS] < 1)
   {
    send_to_char("You need to take more lessons.\n\r", ch);
    return;
   }
   if(victim->pcdata->songs[SONG_MINNE] > 0)
   {
    send_to_char("They're already affected!\n\r", ch);
    return;
   }
   sprintf(buf, "#nYou sing a wonderful #Lminne#n to %s.\n\r", victim->name);
   send_to_char(buf, ch);
   sprintf(buf, "#n%s sings a wonderful #Lminne#n to you.\n\r", ch->name);
   send_to_char(buf,victim);
   victim->pcdata->songs[SONG_MINNE] += (50 * ch->pcdata->powers[SONG_MASTERY]);
   victim->armor -= (50 * ch->pcdata->powers[SONG_MASTERY]);
   return;
  }
  else if (!str_cmp(arg1, "madrigal"))
  {
   if (IS_NPC(victim))
   {
    send_to_char("Not on NPCs.\n\r", ch);
   return;
   }
   if (ch->pcdata->powers[BARD_LESSONS] < 2)
   { 
    send_to_char("You need to take more lessons.\n\r", ch);
    return;
   }
   if (victim->pcdata->songs[SONG_MADRIGAL] > 0)
   {
    send_to_char("They're already affected!\n\r", ch);
    return;
   }
   sprintf(buf, "#nYou sing a wonderful #Gmadrigal#n to %s.\n\r", victim->name);
   send_to_char(buf, ch);
   sprintf(buf, "#n%s sings a wonderful #Gmadrigal#n to you.\n\r", ch->name);
   send_to_char(buf, victim);
   victim->pcdata->songs[SONG_MADRIGAL] += (50 * ch->pcdata->powers[SONG_MASTERY]);
   victim->hitroll += (50 * ch->pcdata->powers[SONG_MASTERY]);
   return;
  }
  else if (!str_cmp(arg1, "spiritual"))
  {
   if(ch->pcdata->powers[BARD_LESSONS] < 10)
   {
    send_to_char("You need to take more lessons.\n\r", ch);
    return;
   } 
   dam[0] = ch->pcdata->songs[0] + ch->pcdata->songs[1] + ch->pcdata->songs[2] + ch->pcdata->songs[3];
   dam[1] = ch->pcdata->songs[4] + ch->pcdata->songs[5] + ch->pcdata->songs[6] + ch->pcdata->songs[7];
   dam[2] = dam[0] + dam[1];
   dam[3] = dam[2] * 12.5;
   damage(ch, victim, dam[3], gsn_requiem);
   sprintf(buf, "#yYou channel all your #7inspiration#y into one powerful song, hitting #C%s #yincredibly hard. [#L%d#y]#n\n\r", 
   victim->name, dam[3]);
   send_to_char(buf, ch);
   sprintf(buf, "#C%s #ychannels every bit of their #7inspiration #yinto one powerful song, hitting you hard. [#L%d#y]#n\n\r", ch->name, dam[3]);
   send_to_char(buf, victim);
   WAIT_STATE(ch, 36);
   return;
  }
  else if (!str_cmp(arg1, "lullaby"))
  {
   if (IS_NPC(victim))
   {
    send_to_char("Only on PCs.\n\r", ch);
    return;
   }
   if (ch->pcdata->powers[BARD_LESSONS] < 7)
   {
    send_to_char("You need to take more lessons.\n\r", ch);
    return;
   }
   if (victim->pcdata->songs[SONG_LULLABY] > 0)
   {
    send_to_char("They're already asleep.\n\r", ch);
    return;
   }
   sprintf(buf, "#nYou sing a #0lullaby#n to %s, putting them to sleep.\n\r", victim->name);
   send_to_char(buf, ch);
   sprintf(buf, "#n%s sings a #0lullaby#n to you, putting you to sleep.\n\r", ch->name);
   send_to_char(buf, victim);
   victim->position = POS_STUNNED;
  }   
  else if (!str_cmp(arg1, "requiem"))
  {
   if (IS_NPC(victim))
   {
    send_to_char("Only on PCs.\n\r", ch);
    return;
   }
   if (ch->pcdata->powers[BARD_LESSONS] < 4)
   {
    send_to_char("You need to take more lessons.\n\r", ch);
    return;
   }
   if (victim->pcdata->songs[SONG_REQUIEM] > 0)
   {
    send_to_char("They're already suffering.\n\r", ch);
    return;
   }
   sprintf(buf, "#nYou instill within %s a horrific #rrequiem#n.\n\r", victim->name);
   send_to_char(buf, ch);
   sprintf(buf, "#n%s instills within you a horrific #rrequiem#n.\n\r", ch->name);
   send_to_char(buf, victim);
   victim->pcdata->songs[SONG_REQUIEM] += ch->pcdata->powers[SONG_MASTERY];
   return;
  }
  else if (!str_cmp(arg1, "dissonance"))
  {
   if (ch->pcdata->powers[BARD_LESSONS] < 9)
   {
    send_to_char("You need to take more lessons.\n\r", ch);
    return;
   }
   dam[0] = number_range(1, 100);
   dam[1] = number_range(5, 500);
   dam[2] = number_range(9, 900);
   dam[3] = number_range(13, 1300);
   dam[4] = number_range(17, 1700);
   damage(ch, victim, dam[0], gsn_requiem);
   damage(ch, victim, dam[1], gsn_requiem);
   damage(ch, victim, dam[2], gsn_requiem);
   damage(ch, victim, dam[3], gsn_requiem);
   damage(ch, victim, dam[4], gsn_requiem);
   sprintf(buf, "#nThe #7intrdouction#n to your dissonant rush does #y[#L%d#y]#n damage to #C%s.#n\n\r", dam[0], victim->name);
   send_to_char(buf, ch);
   sprintf(buf, "#nThe #7warmup#n to your dissonant attack does #y[#L%d#y]#n damage to #C%s.#n\n\r", dam[1], victim->name);
   send_to_char(buf, ch); 
   sprintf(buf, "#nThe #7body#n of your dissonant onslaught does #y[#L%d#y]#n damage to #C%s.#n\n\r", dam[2], victim->name);
   send_to_char(buf, ch);
   sprintf(buf, "#nThe #7outro#n to your dissonant song does #y[#L%d#y]#n damage to #C%s.#n\n\r", dam[3], victim->name);
   send_to_char(buf, ch);
   sprintf(buf, "#nThe #7aftermath#n of your attack does #y[#L%d#y]#n damage to #C%s.#n\n\r", dam[4], victim->name);
   send_to_char(buf, ch);
   sprintf(buf, "You take #y[#L%d#y]#n damage from %s's dissonant song #7introduction.#m\n\r", dam[0], ch->name);
   send_to_char(buf, victim);
   sprintf(buf, "The #7warmup#n to %s's dissonant attack does #y[#L%d#y]#n damage to you.\n\r", ch->name, dam[1]);
   send_to_char(buf, victim);
   sprintf(buf, "The #7body#n of %s's dissonant onslaught does #[#L%d#y]#n damage to you.\n\r", ch->name, dam[2]);
   send_to_char(buf, victim);
   sprintf(buf, "%s's dissonant song #7outro#n deals #y[#L%d#y]#n damage to you.\n\r", ch->name, dam[3]);
   send_to_char(buf, victim);
   sprintf(buf, "You take #y[#L%d#y]#n damage from %s's dissonant #7aftermath.#n\n\r", dam[4], ch->name);
   send_to_char(buf, victim);
   WAIT_STATE(ch, 36);
   return;
  }
  else if (!str_cmp(arg1, "paeon"))
  {
   if (IS_NPC(victim))
   {
    send_to_char("Only on PCs.\n\r", ch);
    return;
   }
   if (ch->pcdata->powers[BARD_LESSONS] < 5)
   {
    send_to_char("You need to take more lessons.\n\r", ch);
    return;
   }
   if (victim->pcdata->songs[SONG_PAEON] > 0)
   {
    send_to_char("They're regenerating quickly already.\n\r", ch);
    return;
   }
   sprintf(buf, "#nYou successfully cause %s to regenerate quickly with your #Cpaeon#n.\n\r", victim->name);
   send_to_char(buf, ch);
   sprintf(buf, "#n%s successfully causes you to regenerate quickly with their #Cpaeon#n.\n\r", ch->name);
   send_to_char(buf, victim);
   victim->pcdata->songs[SONG_PAEON] += ch->pcdata->powers[SONG_MASTERY];
   return;
  }
  else if (!str_cmp(arg1, "ballad"))
  {
   if (IS_NPC(victim))
   {
    send_to_char("Only on players.\n\r", ch);
    return;
   }
   if (ch->pcdata->powers[BARD_LESSONS] < 6)
   {
    send_to_char("You need to take more lessons.\n\r", ch);
    return;
   }
   if (victim->pcdata->songs[SONG_BALLAD] > 0)
   {
    send_to_char("They're regenerating mana fiercely already.\n\r", ch);
    return;
   }
   sprintf(buf, "#nYou cause %s to regenerate mana fiercely with your #Pballad#n.\n\r", victim->name);
   send_to_char(buf, ch);
   sprintf(buf, "#n%s causes you to regenerate mana fiercely with their #Pballad#n.\n\r", ch->name);
   send_to_char(buf, victim);
   victim->pcdata->songs[SONG_BALLAD] += ch->pcdata->powers[SONG_MASTERY];
   return;
  }
  else if (!str_cmp(arg1, "hymnus"))
  {
   if (IS_NPC(victim))
   {
    send_to_char("Only PCs.\n\r", ch);
    return;
   }
   if (ch->pcdata->powers[BARD_LESSONS] < 8)
   { 
    send_to_char("You need to take more lessons.\n\r", ch);
    return;
   }
   if (victim->pcdata->songs[SONG_HYMNUS] > 0)
   {
    send_to_char("They are already suffering from it.\n\r", ch);
    return;
   }
   sprintf(buf, "#nYou degrade %s and their ability to regenerate mana with your #7hymnus#n.\n\r", victim->name);
   send_to_char(buf, ch);
   sprintf(buf, "#n%s degrades your ability to regenerate mana with their #7hymnus#n.\n\r", victim->name);
   send_to_char(buf, victim);
   victim->pcdata->songs[SONG_HYMNUS] += ch->pcdata->powers[SONG_MASTERY];
   return;
  }
  else if (!str_cmp(arg1, "march"))
  {
   if (IS_NPC(victim))
   {
    send_to_char("Only on players.\n\r", ch);
    return;
   }
   if (ch->pcdata->powers[BARD_LESSONS] < 3)
   {
    send_to_char("You need to take more lessons.\n\r", ch);
    return;
   }
   if (victim->pcdata->songs[SONG_MARCH] > 0)
   {
    send_to_char("They're already marching!\n\r", ch);
    return;
   }
   sprintf(buf, "#nYou sing a wonderful #ymarch#n to %s\n\r", victim->name);
   send_to_char(buf, ch);
   sprintf(buf, "#n%s sings a wonderful #ymarch#n to you.\n\r", ch->name);
   send_to_char(buf, victim);
   victim->pcdata->songs[SONG_MARCH] += (ch->pcdata->powers[SONG_MASTERY]*50);
   return;
  }
 return; 
}

void do_songs(CHAR_DATA *ch, char *argument)
{
 char uta[MAX_INPUT_LENGTH];
 char buf[MAX_INPUT_LENGTH];
 int song = 0;


if (!IS_RACE(ch, RACE_ELF) && !IS_RACE(ch, RACE_WILDELF)
&& !IS_RACE(ch, RACE_HALFELF) && !IS_RACE(ch, RACE_DROW))
{
 send_to_char("Huh?\n\r", ch);
 return;
}

 for (song = 0; song < 9; song++)
 {
  switch(song)
  {
   case 0:
    sprintf(uta, "Minuet:   ");
    break;
   case 2:
    sprintf(uta, "Madrigal: ");
    break;
   case 1:
    sprintf(uta, "Minne:    ");
    break;
   case 3:
    sprintf(uta, "March:    ");
    break;
   case 4:
    sprintf(uta, "Requiem:  ");
    break;
   case 5:
    sprintf(uta, "Paeon:    ");
    break;
   case 6:
    sprintf(uta, "Ballad:   ");
    break;
   case 7:
    sprintf(uta, "Lullaby:  ");
    break;
   case 8:
    sprintf(uta, "Hymnus:   ");
    break;
  }
  sprintf(buf, "%s %d\n\r", uta, ch->pcdata->songs[song]);
  send_to_char(buf, ch);
 }
 sprintf(buf, "Mastery:   %d\n", ch->pcdata->powers[SONG_MASTERY]);
 send_to_char(buf, ch);
 return;
}

void do_toughskin (CHAR_DATA * ch, char *argument)
{

  if (IS_NPC (ch))
    return;

	if (!IS_CLASS (ch, RACE_TROLL) && !IS_RACE(ch, RACE_HALFTROLL)
	&& !IS_RACE(ch, RACE_ARCHANGEL) && !IS_RACE(ch, RACE_ARCHDEMON)
	&& !IS_RACE(ch, RACE_DRAGONKIND))
    {
      stc ("Huh?\n\r", ch);
      return;
    }

  if (!IS_SET (ch->newbits, NEW_TOUGHSKIN))
    {
      stc ("Your skin grows stronger.\n\r", ch);
      SET_BIT (ch->newbits, NEW_TOUGHSKIN);
    }
  else
    {
      stc ("Your skin looses its leathery texture.\n\r", ch);
      REMOVE_BIT (ch->newbits, NEW_TOUGHSKIN);

    }
return;
}

