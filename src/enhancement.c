
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"





void do_darktendrils(CHAR_DATA *ch, char *argument)
{
	if (IS_NPC(ch)) return;


  if (!IS_SPHERE(ch, SPHERE_ENHANCEMENT))
    {
        stc("#0Darktendrils is an Enhancement Sphere Ability.#n\n\r",ch);
        return;
    }

	if (IS_SET(ch->newbits, NEW_DARKTENDRILS)) {
	send_to_char("Your tendrils of darkness disappear.\n\r", ch );
	REMOVE_BIT(ch->newbits, NEW_DARKTENDRILS);
	}
	else {
	send_to_char("You call forth dark tendrils to attack your foes.\n\r", ch );
	SET_BIT(ch->newbits, NEW_DARKTENDRILS);
	}

	return;
}



void do_nightsight( CHAR_DATA *ch, char *argument )
{
    char      arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

  if (!IS_SPHERE(ch, SPHERE_ENHANCEMENT))
    {
        stc("#0Nightsite is an Enhancement Sphere Ability.#n\n\r",ch);
        return;
    }

    if (IS_VAMPAFF(ch,VAM_NIGHTSIGHT) )
    {
	send_to_char("The red glow in your eyes fades.\n\r",ch);
	act("The red glow in $n's eyes fades.", ch, NULL, NULL, TO_ROOM);
	REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_NIGHTSIGHT);
	return;
    }
    send_to_char("Your eyes start glowing red.\n\r",ch);
    act("$n's eyes start glowing red.", ch, NULL, NULL, TO_ROOM);
    SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_NIGHTSIGHT);
    return;
}





void do_graft(CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *obj;
    char arg[MAX_INPUT_LENGTH];
    argument = one_argument(argument, arg);

    if (IS_NPC(ch)) return; 

  if (!IS_SPHERE(ch, SPHERE_ENHANCEMENT))
    {
	stc("#0Graft is an Enhancement Sphere Ability.#n\n\r",ch);
	return;
    }
    if ( arg[0] == '\0')
    {
	stc("Which limb do you wish to graft on to yourself?\n\r",ch);
	return;
    }

    if ( ( obj = get_obj_carry(ch,arg) ) == NULL )
    {
	stc("You do not have that limb.\n\r",ch);
	return;
    }

    if (!IS_SET(obj->extra_flags2, ITEM_ARM))
    {
	stc("That's not even an arm!\n\r",ch);
	return;
    }

    if (IS_SET(ch->newbits, THIRD_HAND) && IS_SET(ch->newbits, FOURTH_HAND))
    {
	stc("You already have four arms!\n\r",ch);
	return;
    }

    if (!IS_SET(ch->newbits, FOURTH_HAND))
    {
	act("You graft an arm onto your body.",ch,NULL,obj,TO_CHAR);
	act("$n grafts an arm onto $m body.",ch,NULL,obj,TO_ROOM);
	SET_BIT(ch->newbits, FOURTH_HAND);
	SET_TIMER(ch, TIMER_FOURTH_ARM_GOING, dice(ch->generation, 6) );
	WAIT_STATE(ch, 18);
	extract_obj(obj);
	return;    
    }

    if (!IS_SET(ch->newbits, THIRD_HAND))
    {
	act("You graft an arm onto your body.",ch,NULL,obj,TO_CHAR);
	act("$n grafts an arm onto $m body.",ch,NULL,obj,TO_ROOM);
	SET_BIT(ch->newbits, THIRD_HAND);
	SET_TIMER(ch, TIMER_THIRD_ARM_GOING, dice(ch->generation, 6) );
	WAIT_STATE(ch, 18);
	extract_obj(obj);
	return;    
    }    

    return;
}


