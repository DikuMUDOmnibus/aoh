
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"

void do_fbreath(CHAR_DATA *ch, char *argument)
{
  CHAR_DATA *victim;
  int i=0;
  int dam = 0;
  int chance =0;
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  char arg [MAX_INPUT_LENGTH];

  one_argument( argument, arg );

  if (IS_NPC(ch)) return;

  if (!IS_SPHERE(ch, SPHERE_PYROMANCER))
  {
    send_to_char("This is a Pyromancer Sphere ability.\n\r",ch);
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
 if (ch->mana < 100 )
  {
	 send_to_char("You dont have enough mana.\n\r",ch);
	 return;
  }
  if ((victim = ch->fighting) == NULL)
  {
	 send_to_char( "You aren't fighting anyone.\n\r", ch );
	 return;
  }
  ch->mana -= 100;

  chance = number_range (1, 120);

  if (chance <= 7) i = 1;
  if (chance > 7 && chance <= 22)   i = 2;
  if (chance > 22 && chance <= 32)   i = 3;
//  if (chance > 32 && chance <= 52)   i = 4;
  if (chance > 52 && chance <= 90)   i = 5;
  if (chance > 90 && chance <= 94)   i = 6;
//  if (chance > 94 && chance <= 110)  i = 7;
  if (chance > 110 && chance <= 125) i = 8;
//  if (chance > 125 && chance <= 128) i = 9;
  if (chance > 128 ) i = 10;

  for(i=i;i>0;i--)
   {
     dam = number_range(6000 , 7500);
     if (dam < 6000) dam = 6000;
     dam = cap_dam(ch,victim,dam);
     if(!IS_NPC(ch))
     {
     if (IS_IMMUNE2(victim, IMM2_FIRE))
     dam = dam / 3;
     if (IS_RES(victim, RES_FIRE))
     dam = dam / 1.4;
     if (IS_VULN(victim, VULN_FIRE)) 
     dam += 3000;
     }
     sprintf(buf, "%s's blast of #rf#Rl#ra#Rm#re #Rimmolates#n you [#r%d#n]\n\r", ch->name, dam);
     send_to_char( buf, victim );
     sprintf(buf2, "Your blast of #rf#Rl#ra#Rm#re #Rimmolates#n %s [#r%d#n]\n\r",victim->name, dam);
     send_to_char( buf2, ch );
     act( "$n's blast of #rf#Rl#ra#Rm#re #Rimmolates#n $N",ch,NULL,victim,TO_NOTVICT);
//     fire_effect(victim,ch->explevel,dam,TARGET_CHAR);
     hurt_person(ch,victim,dam);
     if (victim->hit < 1) break;
   }
  if (!IS_AFFECTED(victim, AFF_FLAMING))
    SET_BIT(victim->affected_by, AFF_FLAMING);
  WAIT_STATE(ch,6);
  return;
}




void do_magma( CHAR_DATA *ch, char *argument )
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  char buf[MAX_STRING_LENGTH];
  int dam;


  if (IS_NPC(ch)) return;
 
  if (!IS_SPHERE(ch, SPHERE_PYROMANCER))
  {
    send_to_char("This is a Pyromancer Sphere ability.\n\r",ch);
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

  WAIT_STATE( ch, 12 );

  if (!IS_NPC(victim)) dam = number_range(4000,8000);
  else dam = number_range(15000,25000);

  sprintf(buf,"Your Magma strikes $N with DEVESTATING FORCE! [%d]\n\r",dam);
  act(buf,ch,NULL,victim,TO_CHAR);
  sprintf(buf,"$n's Magma strikes you with DEVESTATING FORCE! [%d]\n\r",dam);
  act(buf,ch,NULL,victim,TO_VICT);
  sprintf(buf,"$n's Magma strikes $N with DEVESTATING FORCE! [%d]\n\r",dam);
  act(buf,ch,NULL,victim,TO_NOTVICT); 
  hurt_person(ch,victim, dam);
  return;
}



void do_enflame( CHAR_DATA *ch, char *argument )
{
  if (IS_NPC(ch)) return;

  if (!IS_SPHERE(ch, SPHERE_PYROMANCER))
  {
    send_to_char("#0This is a Pyromancer Sphere ability.#n\n\r",ch);
    return;
  }
  {
    send_to_char("Huh?\n\r",ch);
    return;
  }
  if (IS_SET(ch->pcdata->powers[ELEMENTAL_POWERS], ELEMENTAL_ENFLAME))
  {
    REMOVE_BIT(ch->pcdata->powers[ELEMENTAL_POWERS], ELEMENTAL_ENFLAME);
    act("You Will No Longer Enflame.", ch, NULL, NULL, TO_CHAR);
    act("$n's Will No Longer Enflame.", ch, NULL, NULL, TO_ROOM);
    return;
  }
  SET_BIT(ch->pcdata->powers[ELEMENTAL_POWERS], ELEMENTAL_ENFLAME);
  act("#CYou Will Now Use Your FIRE to Seek Retribution.#n", ch, NULL, NULL, TO_CHAR);
  act("$n Will now use $s Fire to seek RETRIBUTION.", ch, NULL, NULL, TO_ROOM);
  return;
}



void do_flameblade(CHAR_DATA *ch, char *argument)
{
	 OBJ_DATA *obj;

        if (ch->pcRaceLevel < 50)
          {
             send_to_char("huh?\n\r",ch);
             return;
          }
	if ( ch->practice < 1500)
	  {
		 send_to_char("It costs 1500 primal to flameblade a weapon.\n\r",ch);
		 return;
	  }
	if (argument[0] == '\0')
	  {
		 send_to_char("Which item do you wish to forge?\n\r", ch);
		 return;
	  }
	if ( (obj = get_obj_carry(ch, argument)) == NULL)
	  {
		 send_to_char("You are not carrying that item.\n\r", ch);
		 return;
	  }
/*	if (IS_SET(obj->quest,QUEST_RELIC) || IS_SET(obj->quest,QUEST_ARTIFACT))
	  {
		send_to_char("Not on a Relic.\n\r",ch);
		return;
	  }
*/
        if (IS_SET(obj->quest,QUEST_FLAMEBLADE))
          {
                send_to_char("This item has already been through the Ritual!\n\r",ch);
                return;
          }
	if (obj->item_type == ITEM_WEAPON)
	  {
		 obj->level     =  50;
		 SET_BIT(obj->quest,QUEST_FLAMEBLADE);
		 obj->condition = 100;
		 obj->toughness = 100;
		 obj->resistance = 1;
		 act(" $p is surrounded with Black Flames.", ch, obj, NULL, TO_CHAR);
		 act("$n stares into  $p and it becomes surrounded with black flames.", ch, obj, NULL, TO_ROOM);
		 if(!IS_SET(obj->weapflags, WEAPON_FLAMING)) SET_BIT(obj->weapflags, WEAPON_FLAMING);
		 ch->practice -= 1500;
		 WAIT_STATE(ch, 8); 
		 return;
	  }
}


