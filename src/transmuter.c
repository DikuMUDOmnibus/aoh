
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"



void do_bonemod( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
        
    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
         
           
        if (IS_NPC(ch)) return;
         
        if (!IS_SPHERE(ch, SPHERE_TRANSMUTER))
        {
           send_to_char("#0This is a Transmuter Sphere Power#n\n\r",ch);
           return;
        }

    if ( arg1[0] == '\0') 
    {
        send_to_char( "Syntax: bonemod <modification>\n\r",     ch );
        send_to_char( "Horns, Wings, Head, Exoskeleton, Tail\n\r",ch);
        return;
    }
          
        
    /*
     * Set something.
     */
    if ( !str_cmp( arg1, "horns" ) )
    {
        
        if (!IS_VAMPAFF(ch, VAM_HORNS))
         {
          SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_HORNS);
          send_to_char("You pull a set of horns out of your head.\n\r",ch);
          act("$n pulls a set of horns from his head!\n\r",ch,NULL,NULL,TO_ROOM);
          return;
       }
    
        if (IS_VAMPAFF(ch, VAM_HORNS))
         {
          REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_HORNS);
          send_to_char("You push your horns back into your head.\n\r",ch);
          act("$n pushes $n's horns back into $n head.\n\r",ch,NULL,NULL,TO_ROOM);
          return;
    
         }
    }      
 
    if ( !str_cmp( arg1, "wings" ) )
    {
         
        if (!IS_VAMPAFF(ch, VAM_WINGS))
         {
          SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_WINGS);
          send_to_char("You pull a pair of leathery wings from your back.\n\r",ch);
          act("$n pulls a pair of leathery wings from $s back!\n\r",ch,NULL,NULL,TO_ROOM);
          return;
       }
         
        if (IS_VAMPAFF(ch, VAM_WINGS))
         {
          REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_WINGS);
          send_to_char("You push your wings into your back.\n\r",ch);
          act("$n pushes $s wings into $s back.\n\r",ch,NULL,NULL,TO_ROOM);
          return;
     
         }
    }   
      
     if ( !str_cmp( arg1, "exoskeleton" ) )
    {  
    
        if (!IS_VAMPAFF(ch, VAM_EXOSKELETON))
         {
          SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_EXOSKELETON);
          send_to_char("Your skin is covered by a hard exoskeleton.\n\r",ch);
          act("$n's skin is covered by a hard exoskeleton!\n\r",ch,NULL,NULL,TO_ROOM);
          return;
       }
     
        if (IS_VAMPAFF(ch, VAM_EXOSKELETON))
         {
          REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_EXOSKELETON);
          send_to_char("Your exoskeleton slowly disappears under your skin.\n\r",ch);
          act("$n's hard exoskeleton disappears under $s skin.\n\r",ch,NULL,NULL,TO_ROOM);
          return;
        
         }
    }
          
     if ( !str_cmp( arg1, "tail" ) )
    {
    
        if (!IS_VAMPAFF(ch, VAM_TAIL))
         {
          SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_TAIL);
          send_to_char("Your spine extends out into a long, pointed tail.\n\r",ch);
          act("$n's spine extends to form a long pointed tail!\n\r",ch,NULL,NULL,TO_ROOM);
          return;
       }
          
        if (IS_VAMPAFF(ch, VAM_TAIL))
         {
          REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_TAIL);
          send_to_char("Your tail slowly retracts into your spine.\n\r",ch);
          act("$n's tail shrinks and vanishes into $s spine.\n\r",ch,NULL,NULL,TO_ROOM);
          return;
          
         }
    }
          
     if ( !str_cmp( arg1, "head" ) )
    { 
     
        if (!IS_VAMPAFF(ch, VAM_HEAD))
         {
          SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_HEAD);
          send_to_char("Your head transforms into that of a fierce lion.\n\r",ch);
          act("$n's head transforms into that of a fierce lion!\n\r",ch,NULL,NULL,TO_ROOM);
          return;
       }
          
        if (IS_VAMPAFF(ch, VAM_HEAD))
         {
          REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_HEAD);
          send_to_char("Your head slowly resumes it's normal form.\n\r",ch);
          act("$n's head resumes its normal form.\n\r",ch,NULL,NULL,TO_ROOM);
          return;
          
         }
    }   
          
     
          
     
    /*
     * Generate usage message.
     */
    do_bonemod( ch, "" );
    return;
}



void do_camouflage(CHAR_DATA *ch, char *argument)
{
  char arg1[MAX_STRING_LENGTH];
  char arg2[MAX_STRING_LENGTH];
  char arg3[MAX_STRING_LENGTH];
  OBJ_DATA *obj;

  argument=one_argument(argument,arg1);
  argument=one_argument(argument,arg2);
  strcpy(arg3,argument);
     
  if (IS_NPC(ch)) return;

    if (!IS_SPHERE(ch, SPHERE_TRANSMUTER))
    {
        send_to_char("#0Change is a Transmuter power.#n\n\r",ch);
        return;
    }


  if ((obj=get_obj_carry(ch,arg1)) == NULL)
  {
    send_to_char("You dont have that item.\n\r",ch);
    return;
  }
  if (!(!str_cmp(arg2,"short") || !str_cmp(arg2,"name")))
  {
    send_to_char("syntax : camouflage <item> <short/name> <newname>.\n\r",ch);
    return;
  }
  if (strlen(arg3) > 60 || strlen(arg3) < 3)
  {
    send_to_char("From 3 to 60 characters please.\n\r",ch);
    return;
  }
/*  if (has_bad_chars(ch, arg3))
  {     
    send_to_char("Illegal chars, please retry.\n\r",ch);
    return;
  }
*/  
if (IS_SET(obj->quest, QUEST_ARTIFACT) || IS_SET(obj->quest, QUEST_PRIZE))   
  {    
    send_to_char("Not on artifacts and prizes.\n\r",ch);
    return;
  }
  if (!str_cmp(arg2,"name"))
  {
    free_string(obj->name);
    obj->name = str_dup(arg3);
    obj->questmaker = str_dup(ch->name);
  }
  if (!str_cmp(arg2,"short"))
  {
    free_string(obj->short_descr);
    obj->short_descr=str_dup(arg3);
    obj->questmaker = str_dup(ch->name);
  }
  send_to_char("Ok.\n\r",ch);
  return;
}


void do_mask( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char      arg [MAX_INPUT_LENGTH];
    char      buf [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_SPHERE(ch, SPHERE_TRANSMUTER))
    {
        send_to_char("#0Change is a Transmuter power.#n\n\r",ch);
        return;
    }

    if ( arg[0] == '\0' )
   {
	send_to_char( "Change to look like whom?\n\r", ch );
	return;
    }

    if (IS_AFFECTED(ch,AFF_POLYMORPH) && !IS_VAMPAFF(ch,VAM_DISGUISED))
    {
	send_to_char( "Not while polymorphed.\n\r", ch );
	return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here.\n\r", ch );
	return;
    }

    if ( IS_NPC(victim) )
    {
	send_to_char( "Not on NPC's.\n\r", ch );
	return;
    }

    if ( IS_IMMORTAL(victim) && victim != ch )
    {
	send_to_char( "You can only mask avatars or lower.\n\r", ch );
	return;
    }

    if ( ch->pcdata->condition[COND_THIRST] < 40)
    {
	send_to_char("You have insufficient blood.\n\r",ch);
	return;
    }
    ch->pcdata->condition[COND_THIRST] -= number_range(30,40);

    if ( ch == victim )
    {
	if (!IS_AFFECTED(ch,AFF_POLYMORPH) && !IS_VAMPAFF(ch,VAM_DISGUISED))
	{
	    send_to_char( "You already look like yourself!\n\r", ch );
	    return;
	}
	sprintf(buf,"Your form shimmers and transforms into %s.",ch->name);
	act(buf,ch,NULL,victim,TO_CHAR);
	sprintf(buf,"%s's form shimmers and transforms into %s.",ch->morph,ch->name);
	act(buf,ch,NULL,victim,TO_ROOM);
	REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
	REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_DISGUISED);
    	free_string( ch->morph );
    	ch->morph = str_dup( "" );
	return;
    }
    if (IS_VAMPAFF(ch,VAM_DISGUISED))
    {
    	sprintf(buf,"Your form shimmers and transforms into a clone of %s.",victim->name);
    	act(buf,ch,NULL,victim,TO_CHAR);
    	sprintf(buf,"%s's form shimmers and transforms into a clone of %s.",ch->morph,victim->name);
    	act(buf,ch,NULL,victim,TO_NOTVICT);
    	sprintf(buf,"%s's form shimmers and transforms into a clone of you!",ch->morph);
    	act(buf,ch,NULL,victim,TO_VICT);
    	free_string( ch->morph );
    	ch->morph = str_dup( victim->name );
	return;
    }
    sprintf(buf,"Your form shimmers and transforms into a clone of %s.",victim->name);
    act(buf,ch,NULL,victim,TO_CHAR);
    sprintf(buf,"%s's form shimmers and transforms into a clone of %s.",ch->name,victim->name);
    act(buf,ch,NULL,victim,TO_NOTVICT);
    sprintf(buf,"%s's form shimmers and transforms into a clone of you!",ch->name);
    act(buf,ch,NULL,victim,TO_VICT);
    SET_BIT(ch->affected_by, AFF_POLYMORPH);
    SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_DISGUISED);
    free_string( ch->morph );
    ch->morph = str_dup( victim->name );
    return;
}


void do_change( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH]; 
    char buf [MAX_STRING_LENGTH]; 
    argument = one_argument( argument, arg ); 

    if (IS_NPC(ch)) return;

    if (!IS_SPHERE(ch, SPHERE_TRANSMUTER)) 
    {
	send_to_char("#0Change is a Transmuter power.#n\n\r",ch);
	return;
    }

    if ( arg[0] == '\0' )
    {
	send_to_char( "You can change between 'human', 'bat', 'wolf' and 'mist' forms.\n\r", ch );
	return;
    }


    if ( !str_cmp(arg,"bat") )
    {
	if (IS_AFFECTED(ch, AFF_POLYMORPH))
	{
	    send_to_char( "You can only polymorph from human form.\n\r", ch );
	    return;
	}
    	if ( ch->pcdata->condition[COND_THIRST] < 50 )
    	{
	    send_to_char("You have insufficient blood.\n\r",ch);
	    return;
    	}
	if (ch->stance[0] != -1) do_stance(ch,"");
	if (ch->mounted == IS_RIDING) do_dismount(ch,"");
    clear_stats(ch);
	act( "You transform into bat form.", ch, NULL, NULL, TO_CHAR );
	act( "$n transforms into a bat.", ch, NULL, NULL, TO_ROOM );
	SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_FLYING);
	SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_SONIC);
/*	SET_BIT(ch->pcdata->powers[WOLF_POLYAFF], POLY_BAT);*/
	SET_BIT(ch->polyaff, POLY_BAT);
	SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_CHANGED);
	SET_BIT(ch->affected_by, AFF_POLYMORPH);
	sprintf(buf, "%s the #0v#rampire #0b#rat#n", ch->name);
    	free_string( ch->morph );
    	ch->morph = str_dup( buf );
	return;
    }
    else if ( !str_cmp(arg,"wolf") )
    {
	if (IS_AFFECTED(ch, AFF_POLYMORPH))
	{
	    send_to_char( "You can only polymorph from human form.\n\r", ch );
	    return;
	}
	if (ch->stance[0] != -1) do_stance(ch,"");
	if (ch->mounted == IS_RIDING) do_dismount(ch,"");
	act( "You transform into wolf form.", ch, NULL, NULL, TO_CHAR );
	act( "$n transforms into a dire wolf.", ch, NULL, NULL, TO_ROOM );
    clear_stats(ch);
    	if (ch->wpn[0] > 0)
    	{
	    ch->hitroll += (ch->wpn[0]);
	    ch->damroll += (ch->wpn[0]);
	    ch->armor   -= (ch->wpn[0] * 3);
    	}
    	ch->pcdata->mod_str = 10;
/*	SET_BIT(ch->pcdata->powers[WOLF_POLYAFF], POLY_WOLF);*/
	SET_BIT(ch->polyaff, POLY_WOLF);
	SET_BIT(ch->affected_by, AFF_POLYMORPH);
	SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_CHANGED);
	sprintf(buf, "%s the #0d#rire #0w#rolf#n", ch->name);
    	free_string( ch->morph );
    	ch->morph = str_dup( buf );
	return;
    }
    else if ( !str_cmp(arg,"mist") )
    {
	if (IS_AFFECTED(ch, AFF_POLYMORPH))
	{
	    send_to_char( "You can only polymorph from human form.\n\r", ch );
	    return;
	}

	if (has_timer(ch)) return;

	if (ch->stance[0] != -1) do_stance(ch,"");
	if (ch->mounted == IS_RIDING) do_dismount(ch,"");
    	ch->pcdata->condition[COND_THIRST] -= number_range(40,50);
	act( "You transform into mist form.", ch, NULL, NULL, TO_CHAR );
	act( "$n transforms into a white mist.", ch, NULL, NULL, TO_ROOM );
	if (IS_EXTRA(ch, TIED_UP))
	{
	    act("The ropes binding you fall through your ethereal form.",ch,NULL,NULL,TO_CHAR);
	    act("The ropes binding $n fall through $s ethereal form.",ch,NULL,NULL,TO_ROOM);
	    REMOVE_BIT(ch->extra, TIED_UP);
	    REMOVE_BIT(ch->extra, GAGGED);
	    REMOVE_BIT(ch->extra, BLINDFOLDED);
	}
/*	if (is_affected(ch, gsn_web))
	{
	    act("The webbing entrapping $n falls through $s ethereal form.",ch,NULL,NULL,TO_ROOM);
	    send_to_char("The webbing entrapping you falls through your ethereal form.\n\r",ch);
	    affect_strip(ch, gsn_web);
	}
*/
    clear_stats(ch);
/*	SET_BIT(ch->pcdata->powers[WOLF_POLYAFF], POLY_MIST);*/
	SET_BIT(ch->polyaff, POLY_MIST);
	SET_BIT(ch->pcdata->stats[UNI_AFF], VAM_CHANGED);
	SET_BIT(ch->affected_by, AFF_POLYMORPH);
	SET_BIT(ch->affected_by, AFF_ETHEREAL);
	sprintf(buf, "%s the #7w#nhite #7m#nist", ch->name);
    	free_string( ch->morph );
    	ch->morph = str_dup( buf );
	return;
    }
    else if ( !str_cmp(arg,"human") )
    {
       if (!IS_AFFECTED(ch, AFF_POLYMORPH))
	{
	    send_to_char( "You are already in human form.\n\r", ch );
	    return;
	}
	if (IS_VAMPAFF(ch, VAM_CHANGED) && IS_POLYAFF(ch, POLY_BAT))
	{
	    REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_FLYING);
	    REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_SONIC);
	    REMOVE_BIT(ch->polyaff, POLY_BAT);
/*	    REMOVE_BIT(ch->pcdata->powers[WOLF_POLYAFF], POLY_BAT);*/
	}
	else if (IS_VAMPAFF(ch, VAM_CHANGED) && IS_POLYAFF(ch, POLY_WOLF))
	{
/*	    REMOVE_BIT(ch->pcdata->powers[WOLF_POLYAFF], POLY_WOLF);*/
	    REMOVE_BIT(ch->polyaff, POLY_WOLF);
	    if (ch->hit < 1) ch->hit = 1;
	}
	else if (IS_VAMPAFF(ch, VAM_CHANGED) && IS_POLYAFF(ch, POLY_MIST))
	{
/*	    REMOVE_BIT(ch->pcdata->powers[WOLF_POLYAFF], POLY_MIST);*/
	    REMOVE_BIT(ch->polyaff, POLY_MIST);
	    REMOVE_BIT(ch->affected_by, AFF_ETHEREAL);
	}
	else
	{
	    /* In case they try to change to human from a non-vamp form */
	    send_to_char( "You seem to be stuck in this form.\n\r", ch );
	    return;
	}
	act( "You transform into human form.", ch, NULL, NULL, TO_CHAR );
	act( "$n transforms into human form.", ch, NULL, NULL, TO_ROOM );
	REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
	REMOVE_BIT(ch->pcdata->stats[UNI_AFF], VAM_CHANGED);
    clear_stats(ch);
  	free_string( ch->morph );
    	ch->morph = str_dup( "" );
	return;
    }
    else
	send_to_char( "You can change between 'human', 'bat', 'wolf' and 'mist' forms.\n\r", ch );
    return;
}




void do_zuloform( CHAR_DATA *ch, char *argument )
{
    char arg [MAX_INPUT_LENGTH];
    char buf [MAX_STRING_LENGTH]; 
    argument = one_argument( argument, arg ); 

    if (IS_NPC(ch)) return;
   

    if (!IS_SPHERE(ch, SPHERE_TRANSMUTER))
    {
        send_to_char("#0Zuloform is a Transmuter power.#n\n\r",ch);
        return;
    }

    if (IS_EXTRA(ch,EXTRA_DRAGON))
    {
    send_to_char("You cannot unzulo while a dragon.\n\r",ch);
    return;
    }
    if(IS_POLYAFF(ch, POLY_DRAGON))
    {
	stc( "You can't do that.\n\r",ch);
	return;
    }

    if (IS_POLYAFF(ch, POLY_ZULOFORM))
    {   
        REMOVE_BIT(ch->polyaff, POLY_ZULOFORM);
        REMOVE_BIT(ch->affected_by, AFF_POLYMORPH);
        act( "You transform into human form.", ch, NULL, NULL, TO_CHAR );
        act( "$n's shrinks back into human form.", ch, NULL, NULL, TO_ROOM );
        if (ch->hit < 1) ch->hit = 1;
        ch->damroll = ch->damroll - 150;
        ch->hitroll = ch->hitroll - 150; 
        free_string( ch->morph );
        ch->morph = str_dup( "" );
        return;
    }
    else if (IS_AFFECTED(ch,AFF_POLYMORPH))
    {
send_to_char("You cant zulo when changed.\n\r",ch);
return;
}
  
  
        if (ch->stance[0] != -1) do_stance(ch,"");
	if (ch->mounted == IS_RIDING) do_dismount(ch,"");
    	ch->pcdata->condition[COND_THIRST] -= number_range(100,200);
        act( "You transform into large beast.", ch, NULL, NULL, TO_CHAR );
        act( "$n's body grows and distorts into a large beast.", ch, NULL, NULL, TO_ROOM );
        ch->pcdata->mod_str = 15;
        ch->pcdata->mod_dex = 15;
        SET_BIT(ch->polyaff, POLY_ZULOFORM);
        SET_BIT(ch->affected_by, AFF_POLYMORPH);
        sprintf(buf, "A big black monster");
        free_string( ch->morph );
        ch->morph = str_dup( buf );
        ch->damroll = ch->damroll + 150;
        ch->hitroll = ch->hitroll + 150;
        return;
    }
  
  



/*dragon*/
void do_dragonform( CHAR_DATA *ch, char *argument )
{
char arg [MAX_INPUT_LENGTH];
char buf [MAX_STRING_LENGTH];
argument = one_argument( argument, arg );
 
if (IS_NPC(ch)) return;
 
    if (!IS_SPHERE(ch, SPHERE_TRANSMUTER))
    {
        send_to_char("#0Dragonform is a Transmuter power.#n\n\r",ch);
        return;
    }
 
 
if (!IS_POLYAFF(ch, POLY_ZULOFORM))
{
send_to_char("You can only Dragonform while in Zuloform.\n\r",ch);
return;
}
 
if (IS_EXTRA(ch, EXTRA_DRAGON))
{
REMOVE_BIT(ch->extra,EXTRA_DRAGON);
act( "You transform back into zuloform.", ch, NULL, NULL, TO_CHAR );
    act( "$n shrinks back into a big black monster.", ch, NULL, NULL,
TO_ROOM );
    
 
if (ch->hit < 1) ch->hit = 1;
    ch->damroll = ch->damroll - 100;
    ch->hitroll = ch->hitroll - 100;
    free_string( ch->morph );
    ch->morph = str_dup( "A big black monster" );
return;
}
 
 
if (ch->mounted == IS_RIDING) do_dismount(ch,"");
ch->pcdata->condition[COND_THIRST] -= number_range(20,40);
    act( "You transform into a large dragon.", ch, NULL, NULL, TO_CHAR );
    act( "$n's body grows and distorts into a large dragon.", ch, NULL,
NULL,TO_ROOM);
SET_BIT(ch->extra,EXTRA_DRAGON);
sprintf(buf, "%s, the huge rabid dragon", ch->name);
free_string( ch->morph );
ch->morph = str_dup( buf );
ch->damroll = ch->damroll + 100;
ch->hitroll = ch->hitroll + 100;
return;
}


