
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"

void do_weaponenhance(CHAR_DATA *ch, char *argument)
{
	 OBJ_DATA *obj;

        if (!IS_SPHERE(ch, SPHERE_ENCHANTMENT))
          {
             send_to_char("You need the Enhancement Sphere for this.\n\r",ch);
             return;
          }
	if ( ch->practice < 1500)
	  {
		 send_to_char("It costs 1500 primal to enhance a weapon.\n\r",ch);
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
        if (IS_SET(obj->quest,QUEST_RITUAL))
          {
                send_to_char("This item has already been through the Ritual!\n\r",ch);
                return;
          }
	if (obj->item_type == ITEM_WEAPON)
	  {
		 obj->value[1] += 100;
		 obj->value[2] += 100;
		 obj->level     =  50;
		 oset_affect(ch,obj,50,APPLY_HITROLL,TRUE);
		 oset_affect(ch,obj,50,APPLY_DAMROLL,TRUE);
		 SET_BIT(obj->quest,QUEST_RITUAL);
		 obj->condition = 100;
		 obj->toughness = 100;
		 obj->resistance = 1;
		 act("You bestow power into $p through the Dark Ritual.", ch, obj, NULL, TO_CHAR);
		 act("$n bestows power into $p through the Dark Ritual.", ch, obj, NULL, TO_ROOM);
		 if(!IS_SET(obj->weapflags, WEAPON_FLAMING)) SET_BIT(obj->weapflags, WEAPON_FLAMING);
		 ch->practice -= 1500;
		 WAIT_STATE(ch, 8); 
		 return;
	  }
}


void do_armorenhance(CHAR_DATA *ch, char *argument)
{
	 OBJ_DATA *obj;

        if (!IS_SPHERE(ch, SPHERE_ENCHANTMENT))
          {
             send_to_char("You need the Enhancement Sphere for this.\n\r",ch);
             return;
          }
	if ( ch->practice < 1500)
	  {
		 send_to_char("It costs 1500 primal to enhance a weapon.\n\r",ch);
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
        if (IS_SET(obj->quest,QUEST_RITUAL))
          {
                send_to_char("This item has already been through the Ritual!\n\r",ch);
                return;       
	  }
	if (obj->item_type == ITEM_ARMOR)
          {
                obj->value[0] += 50;
                oset_affect(ch,obj,30,APPLY_HITROLL,TRUE);
                oset_affect(ch,obj,30,APPLY_DAMROLL,TRUE);
                oset_affect(ch,obj,-50,APPLY_AC,TRUE);
                SET_BIT(obj->quest,QUEST_RITUAL);
                obj->condition = 100;
                obj->toughness = 100;
                obj->resistance = 1;
		    act("You bestow power into $p through the Dark Ritual.", ch, obj, NULL, TO_CHAR);
		    act("$n bestows power into $p through the Dark Ritual.", ch, obj, NULL, TO_ROOM);
                ch->practice -= 1500;
                WAIT_STATE(ch, 8);
                return;
          }
          

}

void do_caust(CHAR_DATA * ch, char *argument)
{
        OBJ_DATA *obj;

        if (!IS_SPHERE(ch, SPHERE_ENCHANTMENT))
          {
             send_to_char("You need the Enhancement Sphere for this.\n\r",ch);
             return;
          }

        if (argument[0] == '\0')
        {
                send_to_char("Which item do you wish to caust?\n\r", ch);
                return;
        }

        if ((obj = get_obj_carry(ch, argument)) == NULL)
        {
                send_to_char("You are not carrying that item.\n\r", ch);
                return;
        }

        if (obj->item_type != ITEM_WEAPON)
        {
                send_to_char("That is not a weapon!\n\r", ch);
                return;
        }

        if (dice(1, 100) == 1)
        {
                act("$p is disintegrated by $n's poison.", ch, obj, NULL,
                    TO_ROOM);
                act("Your poison eats through $p.", ch, obj, NULL, TO_CHAR);
                extract_obj(obj);
                return;
        }

        if (IS_WEAP(obj, WEAPON_POISON))
        {
                act("$p is already coated with deadly poison.", ch, obj, NULL,
                    TO_CHAR);
                return;
        }

        act("You run your tongue along $p, coating it with a sickly venom.",
            ch, obj, NULL, TO_CHAR);
        act("$n runs $m tongue along $p, coating it with a sickly venom.", ch,
            obj, NULL, TO_ROOM);

        WAIT_STATE(ch, 8);
        SET_BIT(obj->weapflags, WEAPON_POISON);
        obj->value[1] += 10 - ch->generation;
        obj->value[2] += 10 - ch->generation;
}

void do_immolate(CHAR_DATA * ch, char *argument)
{
        OBJ_DATA *obj;

        if (!IS_SPHERE(ch, SPHERE_ENCHANTMENT))
          {
             send_to_char("You need the Enhancement Sphere for this.\n\r",ch);
             return;
          }

        if (argument[0] == '\0')
        {
                send_to_char("Which item do you wish to immolate?\n\r", ch);
                return;
        }

        if ((obj = get_obj_carry(ch, argument)) == NULL)
        {
                send_to_char("You are not carrying that item.\n\r", ch);
                return;
        }

        if (obj->item_type != ITEM_WEAPON)
        {
                send_to_char("That is not a weapon!\n\r", ch);
                return;
        }

/*        if (dice(1, 100) == 1)
        {
                act("$p explodes in a burst of flames.", ch, obj, NULL,
                    TO_ROOM);
                act("$p explodes in a burst of flames.", ch, obj, NULL,
                    TO_CHAR);
                damage(ch, ch, 500, gsn_inferno);
                extract_obj(obj);
                return;
        }
*/
        if (IS_WEAP(obj, WEAPON_FLAMING))
        {
                act("$p is already flaming.", ch, obj, NULL, TO_CHAR);
                return;
        }

        act("$p bursts into flames.", ch, obj, NULL, TO_CHAR);
        act("$p, carried by $n bursts into flames.", ch, obj, NULL, TO_ROOM);

        WAIT_STATE(ch, 8);
        SET_BIT(obj->weapflags, WEAPON_FLAMING);
}

void do_wfreeze(CHAR_DATA * ch, char *argument)
{
        OBJ_DATA *obj;



        if (!IS_SPHERE(ch, SPHERE_ENCHANTMENT))
          {
             send_to_char("You need the Enhancement Sphere for this.\n\r",ch);
             return;
          }

        if (argument[0] == '\0')
        {
                send_to_char("Which item do you wish to freeze?\n\r", ch);
                return;
        }

        if ((obj = get_obj_carry(ch, argument)) == NULL)
        {
                send_to_char("You are not carrying that item.\n\r", ch);
                return;
        }

        if (obj->item_type != ITEM_WEAPON)
        {
                send_to_char("That is not a weapon!\n\r", ch);
                return;
        }

        if (dice(1, 100) == 1)
        {
                act("$p freezes and shatters.", ch, obj, NULL, TO_ROOM);
                act("$p freezes and shatters.", ch, obj, NULL, TO_CHAR);
                extract_obj(obj);
                return;
        }

        if (IS_WEAP(obj, WEAPON_FROST))
        {
                act("$p is already frozen.", ch, obj, NULL, TO_CHAR);
                return;
        }

        act("$p is surrounded by ice crystals.", ch, obj, NULL, TO_CHAR);
        act("$p, carried by $n is surrounded by ice crystals.", ch, obj, NULL,
            TO_ROOM);

        WAIT_STATE(ch, 8);
        SET_BIT(obj->weapflags, WEAPON_FROST);
}


void do_poisonblade(CHAR_DATA * ch, char *argument)
{

        OBJ_DATA *obj;
        char      arg[MAX_INPUT_LENGTH];

        argument = one_argument(argument, arg);

        if (IS_NPC(ch))
                return;

        if (!IS_SPHERE(ch, SPHERE_ENCHANTMENT))
          {
             send_to_char("You need the Enhancement Sphere for this.\n\r",ch);
             return;
          }


        if (arg[0] == '\0')
        {
                stc("Poison What?\n\r", ch);
                return;
        }

        if ((obj = get_obj_carry(ch, arg)) == NULL)
        {
                send_to_char("You dont have that item.\n\r", ch);
                return;
        }

        if (obj->item_type != ITEM_WEAPON)
        {
                send_to_char("You can only poison weapons!\n\r", ch);
                return;
        }

        if (IS_SET(obj->quest, QUEST_WWPOISON))
        {
                send_to_char
                        ("#0This item has already been #gP#Goisone#gd!#n.\n\r",
                         ch);
                return;
        }


        SET_BIT(obj->quest, QUEST_WWPOISON);
        stc("#0You coat your weapon in foul smelling #gp#Goiso#gn!#n\n\r",
            ch);
        obj->timer = 35;
        return;
}

