/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Dystopia Mud improvements copyright (C) 2000, 2001 by Brian Graversen  *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"

#if !defined(macintosh)
extern  int     _filbuf         args( (FILE *) );
#endif



/*
 * Array of containers read for proper re-nesting of objects.
 */
#define MAX_NEST        100
static  OBJ_DATA *      rgObjNest       [MAX_NEST];
char *top_string;


/*
 * Local functions.
 */
void    fwrite_char     args( ( CHAR_DATA *ch,  FILE *fp ) );
void    fwrite_obj      args( ( CHAR_DATA *ch,  OBJ_DATA  *obj, FILE *fp, int iNest ) );
void    fread_char      args( ( CHAR_DATA *ch,  FILE *fp ) );
void    fread_obj       args( ( CHAR_DATA *ch,  FILE *fp ) );
void    save_char_obj_finger    args( ( CHAR_DATA *ch ) );


/*
 * Save a character and inventory.
 * Would be cool to save NPC's too for quest purposes,
 *   some of the infrastructure is provided.
 */
void save_char_obj( CHAR_DATA *ch )
{
    char strsave[MAX_STRING_LENGTH];
    FILE *fp;

    if ( IS_NPC(ch) || ch->level < 2 )
	return;

    /* A new player saved for the first time */
    if (ch->pcdata->playerid == 0)
      ch->pcdata->playerid = get_next_playerid();

    if ( ch->desc != NULL && ch->desc->connected != CON_PLAYING )
	return;

    check_leaderboard(ch);

    ch->save_time = current_time;
    fclose( fpReserve );
    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( ch->pcdata->switchname ) );
    if ( ( fp = fopen( strsave, "w" ) ) == NULL )
    {
	bug( "Save_char_obj: fopen", 0 );
	perror( strsave );
    }
    else
    {
		fwrite_char( ch, fp );
		
	if ( ch->carrying != NULL )
	    fwrite_obj( ch, ch->carrying, fp, 0 );
	fprintf( fp, "#END\n" );
    }
    fflush( fp );
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    save_char_obj_finger( ch );
    return;
}

void save_char_obj_finger( CHAR_DATA *ch )
{
    char strsave[MAX_STRING_LENGTH];
    FILE *fp;

    if ( IS_NPC(ch) || ch->level < 2 )
	return;

    ch->save_time = current_time;
    fclose( fpReserve );
    sprintf( strsave, "%sbackup/%s", PLAYER_DIR, capitalize(ch->pcdata->switchname) );
    if ( ( fp = fopen( strsave, "w" ) ) == NULL )
    {
	bug( "Save_char_obj: fopen", 0 );
	perror( strsave );
    }
    else
    {
	fprintf( fp, "%s~\n",   ch->name                );
	fprintf( fp, "%s~\n",   ch->pcdata->title       );
	fprintf( fp, "%s~\n",   ch->lasthost            );
	fprintf( fp, "%s~\n",   ch->lasttime            );
	fprintf( fp, "%d\n",    ch->extra               );
	fprintf( fp, "%d\n",    ch->sex                 );
	fprintf( fp, "%s~\n",   ch->pcdata->conception  );
	fprintf( fp, "%s~\n",   ch->createtime          );
	fprintf( fp, "%d\n",    ch->level               );
	fprintf( fp, "%d\n",
	    ch->played + (int) (current_time - ch->logon));
	fprintf( fp, "%s~\n",   ch->pcdata->marriage    );
	fprintf( fp, "%d\n",    ch->pkill );
	fprintf( fp, "%d\n",    ch->pdeath );
	fprintf( fp, "%d\n",    ch->pcdata->awins       );
	fprintf( fp, "%d\n",    ch->pcdata->alosses     );
	fprintf( fp, "%d\n",    ch->mkill );
	fprintf( fp, "%d\n",    ch->mdeath );
    }
    fflush( fp );
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}


/*
 * Write the char.
 */
void fwrite_char( CHAR_DATA *ch, FILE *fp )
{
    AFFECT_DATA *paf;
    ALIAS_DATA *ali;
    QUEST_DATA *quest;
    int sn;
    int i;

    fprintf( fp, "#%s\n", IS_NPC(ch) ? "MOB" : "PLAYERS"                );

    fprintf( fp, "Name         %s~\n",  ch->name                );
    fprintf( fp, "Switchname   %s~\n",  ch->pcdata->switchname  );
    fprintf( fp, "PlayerID     %d\n",   ch->pcdata->playerid    );
    fprintf( fp, "ShortDescr   %s~\n",  ch->short_descr         );
    fprintf( fp, "LongDescr    %s~\n",  ch->long_descr          );
    fprintf( fp, "Description  %s~\n",  ch->description         );
    fprintf( fp, "Religion     %d\n", ch->pcdata->religion      );
    fprintf( fp, "Class        %d\n",   ch->class               );
    fprintf( fp, "Morph        %s~\n",  ch->morph               );
    fprintf( fp, "Createtime   %s~\n",  ch->createtime          );
    fprintf( fp, "Lasttime     %s~\n",  ch->lasttime            );
    fprintf( fp, "Lasthost     %s~\n",  ch->lasthost            );
    fprintf( fp, "Prompt       %s~\n",  ch->prompt              );
    fprintf( fp, "Cprompt      %s~\n",  ch->cprompt             );
    fprintf( fp, "Decapmessage  %s~\n",  ch->pcdata->decapmessage  );
    fprintf( fp, "Loginmessage  %s~\n",  ch->pcdata->loginmessage  );
    fprintf( fp, "Logoutmessage %s~\n",  ch->pcdata->logoutmessage );
    fprintf( fp, "Avatarmessage %s~\n",  ch->pcdata->avatarmessage );
    fprintf( fp, "Tiemessage    %s~\n",  ch->pcdata->tiemessage );
    fprintf( fp, "Jflags       %d\n",   ch->pcdata->jflags	);
    fprintf( fp, "Relrank      %d\n",   ch->pcdata->relrank     );
    fprintf( fp, "Sex          %d\n",   ch->sex                 );
    fprintf( fp, "Meanparadox  %d\n",   ch->pcdata->mean_paradox_counter   );
    fprintf( fp, "Timetick     %d\n",   ch->pcdata->time_tick   );
    fprintf( fp, "Revision     %d\n",   ch->pcdata->revision   );

    fprintf( fp, "Faith        %d\n",   ch->pcdata->faith     );
    fprintf( fp, "Currentfaith %d\n",   ch->pcdata->current_faith  );
    fprintf( fp, "Relpowers    %d\n",   ch->pcdata->rel_powers     );

    fprintf( fp, "Immune       %d\n",   ch->immune              );
    fprintf( fp, "Polyaff      %d\n",   ch->polyaff             );
        fprintf(fp, "PlayerRank   %d\n", ch->pRank);
        fprintf(fp, "PlayerRace   %d\n", ch->pcRace);
	fprintf(fp, "PlayerRaceLevel %d\n", ch->pcRaceLevel);
	fprintf(fp, "PcPractice   %d\n", ch->pcpractice);
        fprintf(fp, "Pcwpnprof    %d\n", ch->pcwpnprof);
        fprintf(fp, "Pcmaxwpnprof %d\n", ch->pcmaxwpnprof);
	fprintf(fp, "Pcsphereammount %d\n", ch->pcsphereammount);
	fprintf(fp, "Pcsphere %d\n", ch->pcsphere);
	fprintf(fp, "Pcgold %d\n", ch->pcgold);
	fprintf(fp, "Pcsilver %d\n", ch->pcsilver);
	fprintf(fp, "Pccopper %d\n", ch->pccopper);
        fprintf(fp, "Pc_build %d\n", ch->pcdata->pc_build);
        fprintf(fp, "Pc_eyes %d\n", ch->pcdata->pc_eyes);
        fprintf(fp, "Pc_hair %d\n", ch->pcdata->pc_hair);
        fprintf(fp, "Pc_looks %d\n", ch->pcdata->pc_looks);
    fprintf( fp, "Pcstyle       %d %d %d %d \n",
        ch->pcstyle[0], ch->pcstyle[1], ch->pcstyle[2], ch->pcstyle[3]);
        fprintf(fp, "Pcremort %d\n", ch->pcremort);
        fprintf(fp, "PcLegacy %d\n", ch->pcLegacy);
	fprintf(fp, "Prof %d\n", ch->profession);
	fprintf(fp, "Guild %d\n", ch->guild);
    fprintf( fp, "Generation   %d\n",   ch->generation          );
    fprintf( fp, "Flag2       %d\n",   ch->flag2              );
    fprintf( fp, "Itemaffect   %d\n",   ch->itemaffect          );
    fprintf( fp, "Form         %d\n",   ch->form                );
    fprintf( fp, "Home         %d\n",   ch->home                );
    fprintf( fp, "Level        %d\n",   ch->level               );
    fprintf( fp, "Trust        %d\n",   ch->trust               );
    fprintf( fp, "Security     %d\n",    ch->pcdata->security    );  /* OLC */

    fprintf( fp, "Played       %d\n",
	ch->played + (int) (current_time - ch->logon)           );
    fprintf( fp, "Room         %d\n",
	(  ch->in_room == get_room_index( ROOM_VNUM_LIMBO )
	&& ch->was_in_room != NULL )
	    ? ch->was_in_room->vnum
	    : ch->in_room->vnum );

    fprintf( fp, "PkPdMkMd     %d %d %d %d\n",
	ch->pkill, ch->pdeath, ch->mkill, ch->mdeath );
    fprintf( fp, "Alos         %d\n", ch->pcdata->alosses);
    fprintf( fp, "Awin         %d\n", ch->pcdata->awins); 
    fprintf( fp, "Weapons      %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
	ch->wpn[0], ch->wpn[1], ch->wpn[2], ch->wpn[3], ch->wpn[4], 
	ch->wpn[5], ch->wpn[6], ch->wpn[7], ch->wpn[8], ch->wpn[9], 
	ch->wpn[10], ch->wpn[11], ch->wpn[12], ch->wpn[13],ch->wpn[14],
	ch->wpn[15], ch->wpn[16], ch->wpn[17], ch->wpn[18] );
    fprintf( fp, "Spells       %d %d %d %d %d\n",
	ch->spl[0], ch->spl[1], ch->spl[2], ch->spl[3], ch->spl[4] );
    fprintf( fp, "Combat       %d %d %d %d %d %d %d %d\n",
	ch->cmbt[0], ch->cmbt[1], ch->cmbt[2], ch->cmbt[3],
	ch->cmbt[4], ch->cmbt[5], ch->cmbt[6], ch->cmbt[7] );
    fprintf( fp, "Stance       %d %d %d %d %d %d %d %d %d %d %d %d\n",
	ch->stance[0], ch->stance[1], ch->stance[2], ch->stance[3],
	ch->stance[4], ch->stance[5], ch->stance[6], ch->stance[7],
	ch->stance[8], ch->stance[9], ch->stance[10], ch->stance[11] );
    fprintf( fp, "Stance2      %d %d %d %d %d %d %d %d %d %d %d %d\n",
        ch->stance[12], ch->stance[13], ch->stance[14], ch->stance[15],
        ch->stance[16], ch->stance[17], ch->stance[18], ch->stance[19],
        ch->stance[20], ch->stance[21], ch->stance[22], ch->stance[23] );
    fprintf( fp, "Locationhp   %d %d %d %d %d %d %d\n",
	ch->loc_hp[0], ch->loc_hp[1], ch->loc_hp[2], ch->loc_hp[3],
	ch->loc_hp[4], ch->loc_hp[5], ch->loc_hp[6] );
    fprintf( fp, "HpManaMove   %d %d %d %d %d %d\n",
	ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move, ch->max_move );
    fprintf( fp, "Exp          %2.0f\n",   ch->exp                 );
    fprintf( fp, "Act          %d\n",   ch->act                 );
    fprintf( fp, "Special      %d\n",   ch->special             );
    fprintf( fp, "Newbits      %d\n",   ch->newbits             );

    fprintf( fp, "Extra        %d\n",   ch->extra               );
    fprintf( fp, "AffectedBy   %d\n",   ch->affected_by         );

    fprintf( fp, "Position     %d\n",
	ch->position == POS_FIGHTING ? POS_STANDING : ch->position );

    fprintf( fp, "Practice     %d\n",   ch->practice            );
    fprintf( fp, "SavingThrow  %d\n",   ch->saving_throw        );
    fprintf( fp, "Alignment    %d\n",   ch->alignment           );
    fprintf( fp, "Hitroll      %d\n",   ch->hitroll             );
    fprintf( fp, "Damroll      %d\n",   ch->damroll             );
    fprintf( fp, "Armor        %d\n",   ch->armor               );
    fprintf( fp, "Deaf         %d\n",   ch->deaf                );

    if (ch->pcdata->damreduct > 100 || ch->pcdata->damreduct < 50)
    {
      ch->pcdata->damreduct = 100;
      ch->pcdata->damreductdec = 0;
    }
    if (ch->pcdata->damreductdec != 0)
    {
      if (ch->pcdata->damreductdec > 9 || ch->pcdata->damreductdec < 1)
        ch->pcdata->damreductdec = 0;
    }
    fprintf(fp, "DamRed %d %d\n", ch->pcdata->damreduct, ch->pcdata->damreductdec);

    fprintf( fp, "RawMaterial       %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d \n",
        ch->rawmaterial[0], ch->rawmaterial[1], ch->rawmaterial[2], ch->rawmaterial[3],
	ch->rawmaterial[4], ch->rawmaterial[5], ch->rawmaterial[6], ch->rawmaterial[7],
        ch->rawmaterial[8], ch->rawmaterial[9], ch->rawmaterial[10], ch->rawmaterial[11],
        ch->rawmaterial[12], ch->rawmaterial[13], ch->rawmaterial[14], ch->rawmaterial[15],
        ch->rawmaterial[16], ch->rawmaterial[17], ch->rawmaterial[18], ch->rawmaterial[19],
        ch->rawmaterial[20], ch->rawmaterial[21], ch->rawmaterial[22], ch->rawmaterial[23],
        ch->rawmaterial[24], ch->rawmaterial[25], ch->rawmaterial[26], ch->rawmaterial[27],
        ch->rawmaterial[28], ch->rawmaterial[29], ch->rawmaterial[30], ch->rawmaterial[31],
        ch->rawmaterial[32], ch->rawmaterial[33], ch->rawmaterial[34], ch->rawmaterial[35],
        ch->rawmaterial[36], ch->rawmaterial[37], ch->rawmaterial[38], ch->rawmaterial[39],
        ch->rawmaterial[40], ch->rawmaterial[41], ch->rawmaterial[42], ch->rawmaterial[43],
        ch->rawmaterial[44], ch->rawmaterial[45], ch->rawmaterial[46], ch->rawmaterial[47],
        ch->rawmaterial[48], ch->rawmaterial[49] );

    fprintf( fp, "Alloy   %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", 
	ch->alloy[0], ch->alloy[1], ch->alloy[2], ch->alloy[3], ch->alloy[4], ch->alloy[5],
	ch->alloy[6], ch->alloy[7], ch->alloy[8], ch->alloy[9], ch->alloy[10], ch->alloy[11],
	ch->alloy[12], ch->alloy[13], ch->alloy[14], ch->alloy[15], ch->alloy[16], ch->alloy[17],
	ch->alloy[18], ch->alloy[19]);


    if ( IS_NPC(ch) )
    {
	fprintf( fp, "Vnum         %d\n",       ch->pIndexData->vnum    );
    }
    else
    {
	fprintf( fp, "Password     %s~\n",      ch->pcdata->pwd         );
	fprintf( fp, "Bamfin       %s~\n",      ch->pcdata->bamfin      );
	fprintf( fp, "Bamfout      %s~\n",      ch->pcdata->bamfout     );
        fprintf( fp, "Lastdecap1      %s~\n",      ch->pcdata->last_decap[0]  );
        fprintf( fp, "Lastdecap2      %s~\n",      ch->pcdata->last_decap[1]  );
        fprintf( fp, "Retaliation     %s~\n",      ch->pcdata->retaliation    );
	fprintf( fp, "Questsrun    %d\n",	ch->pcdata->questsrun	);
	fprintf( fp, "Queststotal  %d\n",	ch->pcdata->questtotal	);
	fprintf( fp, "Title        %s~\n",      ch->pcdata->title       );
	fprintf( fp, "Bounty	    %d\n",      ch->pcdata->bounty	);
	fprintf( fp, "Conception   %s~\n",      ch->pcdata->conception  );
	fprintf( fp, "Parents      %s~\n",      ch->pcdata->parents     );
	fprintf( fp, "Cparents     %s~\n",      ch->pcdata->cparents    );
	fprintf( fp, "Marriage     %s~\n",      ch->pcdata->marriage    );
	fprintf( fp, "AttrPerm     %d %d %d %d %d\n",
	    ch->pcdata->perm_str,
	    ch->pcdata->perm_int,
	    ch->pcdata->perm_wis,
	    ch->pcdata->perm_dex,
	    ch->pcdata->perm_con );

	fprintf( fp, "AttrMod      %d %d %d %d %d\n",
	    ch->pcdata->mod_str, 
	    ch->pcdata->mod_int, 
	    ch->pcdata->mod_wis,
	    ch->pcdata->mod_dex, 
	    ch->pcdata->mod_con );

	fprintf( fp, "Quest        %d\n", ch->pcdata->quest );
        fprintf( fp, "Rank         %d\n", ch->pcdata->rank );
	fprintf( fp, "Stage        %d %d %d\n",
	    ch->pcdata->stage[0],
	    ch->pcdata->stage[1],
	    ch->pcdata->stage[2] );
	fprintf( fp, "Score        %d %d %d %d %d %d\n",
	    ch->pcdata->score[0],
	    ch->pcdata->score[1],
	    ch->pcdata->score[2],
	    ch->pcdata->score[3],
	    ch->pcdata->score[4],
	    ch->pcdata->score[5]);
	fprintf( fp, "Genes        %d %d %d %d %d %d %d %d %d %d\n",
	    ch->pcdata->genes[0],
	    ch->pcdata->genes[1],
	    ch->pcdata->genes[2],
	    ch->pcdata->genes[3],
	    ch->pcdata->genes[4],
	    ch->pcdata->genes[5],
	    ch->pcdata->genes[6],
	    ch->pcdata->genes[7],
	    ch->pcdata->genes[8],
	    ch->pcdata->genes[9] );
	fprintf( fp, "Power        ");
	    for (sn=0; sn < 20; sn++)
	    {
		fprintf(fp, "%d ", ch->pcdata->powers[sn]);
	    }
	    fprintf(fp, "\n");


        fprintf( fp, "Stats        ");
            for (sn=0; sn < 12; sn++)
            {
                fprintf(fp, "%d ", ch->pcdata->stats[sn]);
            }
            fprintf(fp, "\n");

	if (ch->pcdata->obj_vnum != 0)
	    fprintf( fp, "Objvnum      %d\n", ch->pcdata->obj_vnum );

	fprintf( fp, "Condition    %d %d %d\n",
	    ch->pcdata->condition[0],
	    ch->pcdata->condition[1],
	    ch->pcdata->condition[2] );

        fprintf( fp, "Denied       %ld\n", ch->pcdata->denied);

	/* Save note board status */
	/* Save number of boards in case that number changes */
	fprintf (fp, "Boards       %d ", MAX_BOARD);
	for (i = 0; i < MAX_BOARD; i++)
	fprintf (fp, "%s %ld ", boards[i].short_name, ch->pcdata->last_note[i]);
	fprintf (fp, "\n");
	

	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( skill_table[sn].name != NULL && ch->pcdata->learned[sn] > 0 )
	    {
		fprintf( fp, "Skill        %d '%s'\n",
		    ch->pcdata->learned[sn], skill_table[sn].name );
	    }
	}

        for (ali = ch->pcdata->alias; ali; ali = ali->next)
        {
          fprintf(fp, "Alias      %s~ %s~\n", ali->short_n, ali->long_n);
        }

        for (quest = ch->pcdata->quests; quest; quest = quest->next)
        {
          fprintf(fp, "QuestData %d %d %d %d %d %d %d\n",
            quest->time, quest->type, quest->giver, quest->vnums[0],
            quest->vnums[1], quest->vnums[2], quest->vnums[3]);
        }
    }

    for ( paf = ch->affected; paf != NULL; paf = paf->next )
    {
	/* Thx Alander */
	if ( paf->type < 0 || paf->type >= MAX_SKILL )
	    continue;

	fprintf( fp, "AffectData   '%s' %3d %3d %3d %10d\n",
	    skill_table[paf->type].name,
	    paf->duration,
	    paf->modifier,
	    paf->location,
	    paf->bitvector
	    );
    }

    fprintf( fp, "End\n\n" );
    return;
}



/*
 * Write an object and its contents.
 */
void fwrite_obj( CHAR_DATA *ch, OBJ_DATA *obj, FILE *fp, int iNest )
{
    EXTRA_DESCR_DATA *ed;
    AFFECT_DATA *paf;

    /*
     *
     * Slick recursion to write lists backwards,
     *   so loading them will load in forwards order.
     */
    if ( obj->next_content != NULL )
	fwrite_obj( ch, obj->next_content, fp, iNest );

    if ( (obj->chobj != NULL && (!IS_NPC(obj->chobj) &&
	obj->chobj->pcdata->obj_vnum != 0)) || obj->item_type == ITEM_KEY)
	return;

    fprintf( fp, "#OBJECT\n" );
    fprintf( fp, "Nest         %d\n",   iNest                        );
    fprintf( fp, "Name         %s~\n",  obj->name                    );
    fprintf( fp, "OwnerID      %d\n",   obj->ownerid		     );
    fprintf( fp, "ShortDescr   %s~\n",  obj->short_descr             );
    fprintf( fp, "Description  %s~\n",  obj->description             );

    if (obj->questmaker != NULL && strlen(obj->questmaker) > 1)
	fprintf( fp, "Questmaker   %s~\n",      obj->questmaker      );
    if (obj->questowner != NULL && strlen(obj->questowner) > 1)
	fprintf( fp, "Questowner   %s~\n",      obj->questowner      );

    fprintf( fp, "Vnum         %d\n",   obj->pIndexData->vnum        );

    fprintf( fp, "ExtraFlags   %d\n",   obj->extra_flags             );
    fprintf( fp, "ExtraFlags2  %d\n",   obj->extra_flags2            );
    fprintf( fp, "WeapFlags    %d\n",   obj->weapflags               );
    fprintf( fp, "WearFlags    %d\n",   obj->wear_flags              );
    fprintf( fp, "WearLoc      %d\n",   obj->wear_loc                );
    fprintf( fp, "ItemType     %d\n",   obj->item_type               );
    fprintf( fp, "Weight       %d\n",   obj->weight                  );
    fprintf( fp, "Condition    %d\n",   obj->condition               );
    fprintf( fp, "Toughness    %d\n",   obj->toughness               );
    fprintf( fp, "Resistance   %d\n",   obj->resistance              );
    if (obj->quest != 0)
	fprintf( fp, "Quest        %d\n",       obj->quest           );
    if (obj->points != 0)
	fprintf( fp, "Points       %d\n",       obj->points          );
    fprintf( fp, "Level        %d\n",   obj->level                   );
    fprintf( fp, "Timer        %d\n",   obj->timer                   );
    fprintf( fp, "Cost         %d\n",   obj->cost                    );
    fprintf( fp, "Material     %d\n",   obj->material		     );
    fprintf( fp, "CMaterial     %d\n",   obj->c_material                );
    fprintf( fp, "Values       %d %d %d %d\n",
	obj->value[0], obj->value[1], obj->value[2], obj->value[3]           );

    switch ( obj->item_type )
    {
    case ITEM_POTION:
	if ( obj->value[1] > 0 )
	{
	    fprintf( fp, "Spell 1      '%s'\n", 
		skill_table[obj->value[1]].name );
	}

	if ( obj->value[2] > 0 )
	{
	    fprintf( fp, "Spell 2      '%s'\n", 
		skill_table[obj->value[2]].name );
	}

	if ( obj->value[3] > 0 )
	{
	    fprintf( fp, "Spell 3      '%s'\n", 
		skill_table[obj->value[3]].name );
	}

	break;

    case ITEM_SCROLL:
	if ( obj->value[1] > 0 )
	{
	    fprintf( fp, "Spell 1      '%s'\n", 
		skill_table[obj->value[1]].name );
	}

	if ( obj->value[2] > 0 )
	{
	    fprintf( fp, "Spell 2      '%s'\n", 
		skill_table[obj->value[2]].name );
	}

	if ( obj->value[3] > 0 )
	{
	    fprintf( fp, "Spell 3      '%s'\n", 
		skill_table[obj->value[3]].name );
	}

	break;

    case ITEM_PILL:
    case ITEM_STAFF:
    case ITEM_WAND:
	if ( obj->value[3] > 0 )
	{
	    fprintf( fp, "Spell 3      '%s'\n", 
		skill_table[obj->value[3]].name );
	}

	break;
    }

    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
	fprintf( fp, "AffectData   %d %d %d\n",
	    paf->duration, paf->modifier, paf->location );
    }

    for ( ed = obj->extra_descr; ed != NULL; ed = ed->next )
    {
	fprintf( fp, "ExtraDescr   %s~ %s~\n",
	    ed->keyword, ed->description );
    }

    fprintf( fp, "End\n\n" );

    if ( obj->contains != NULL )
	fwrite_obj( ch, obj->contains, fp, iNest + 1 );

    return;
}

bool load_char_short( DESCRIPTOR_DATA *d, char *name)
{
    static PC_DATA pcdata_zero;
    char strsave[MAX_STRING_LENGTH];
    char *strtime;
    CHAR_DATA *ch;
    FILE *fp;
    bool found;
    int sn;

    if ( char_free == NULL )
    {
        ch                              = alloc_perm( sizeof(*ch) );
    }
    else
    {
        ch                              = char_free;
        char_free                       = char_free->next;
    }
    clear_char( ch );

    if ( pcdata_free == NULL )
    {
        ch->pcdata                      = alloc_perm( sizeof(*ch->pcdata) );
    }
    else
    {
        ch->pcdata                      = pcdata_free;
        pcdata_free                     = pcdata_free->next;
    }
    *ch->pcdata                         = pcdata_zero;

    d->character                        = ch;
    ch->desc                            = d;
    ch->name                            = str_dup( name );
    ch->pcdata->switchname              = str_dup( name );
    ch->act                             = PLR_BRIEF3
                                        | PLR_AUTOEXIT
                                        | PLR_PROMPT
                                        | PLR_MAP;
    ch->pcdata->board                   = &boards[DEFAULT_BOARD];
    ch->pcdata->history                 = generate_history();
    ch->extra                           = 0;
    ch->deaf                            = CHANNEL_DKTALK; /*no danish by default */
    ch->special                         = 0;
    ch->newbits                         = 0;
    ch->class                           = 0;
    ch->amount_attacks_dealt		= 0;
    ch->amount_attacks_recieved         = 0;
    ch->amount_damage_dealt             = 0;
    ch->amount_damage_recieved          = 0;
    ch->pcdata->familiar                = NULL;   
    ch->pcdata->pfile                   = NULL;
    ch->pcdata->partner                 = NULL;
    ch->pcdata->propose                 = NULL;
    ch->pcdata->chobj                   = NULL;
    ch->pcdata->memorised               = NULL;
    ch->pcdata->safe_counter            = 0;
    ch->pcdata->mean_paradox_counter    = 0;
    ch->pcdata->relrank                 = 0;
    ch->pcdata->time_tick               = 0;
    ch->pcdata->betting_amount          = 0;
    ch->pcdata->betting_char            = 0;
    ch->pcdata->tempflag                = 0; // always 0 at login, never saved
    ch->pcdata->denied                  = current_time;
    ch->pcdata->playerid		= 0;
    ch->pcdata->revision                = 0;
    ch->pcdata->faith                   = 0;
    ch->pcdata->current_faith           = 0;
    ch->pcdata->damreduct               = 100;
    ch->pcdata->damreductdec            = 0;
    ch->pcdata->rel_powers              = 0;
    ch->pcdata->jflags                  = 0;
    ch->pcdata->decapmessage            = str_dup( "" );
    ch->pcdata->logoutmessage           = str_dup( "" );
    ch->pcdata->avatarmessage           = str_dup( "" );
    ch->pcdata->loginmessage            = str_dup( "" );
    ch->pcdata->tiemessage              = str_dup( "" );
    ch->pcdata->last_global             = str_dup( "" );
    ch->pcdata->pwd                     = str_dup( "" );
    ch->pcdata->bamfin                  = str_dup( "" );
    ch->pcdata->last_decap[0]           = str_dup( "" );
    ch->pcdata->last_decap[1]           = str_dup( "" );
    ch->pcdata->retaliation             = str_dup( "" );
    ch->pcdata->bamfout                 = str_dup( "" );
    ch->pcdata->title                   = str_dup( "" );
    ch->pcdata->bounty                  = 0;
    ch->pcdata->conception              = str_dup( "" );
    ch->pcdata->parents                 = str_dup( "" );
    ch->pcdata->cparents                = str_dup( "" );
    ch->pcdata->marriage                = str_dup( "" );
    ch->morph                           = str_dup( "" );
    ch->pload                           = str_dup( "" );
    ch->prompt                          = str_dup( "" );
    ch->cprompt                         = str_dup( "" );
    strtime = ctime( &current_time );
    strtime[strlen(strtime)-1] = '\0';
    free_string(ch->lasttime);
    ch->createtime                      = str_dup( strtime );
    ch->lasttime                        = str_dup( "" );  
    ch->lasthost                        = str_dup( "" );
    ch->hunting                         = str_dup( "" );
    ch->pcdata->followers               = 0;
    ch->mounted                         = 0;
    ch->home                            = 2002;
    ch->damcap[0]                       = 1000;
    ch->damcap[1]                       = 0; 
    ch->itemaffect                      = 0;
    ch->polyaff                         = 0;
    ch->immune                          = IMM_TRAVEL;
	ch->pRank = 1;
	ch->pcRace = 0;
	ch->pcRaceLevel = 1;
        ch->pcpractice = 0;
        ch->pcwpnprof = 0;
        ch->pcmaxwpnprof = 0;
        ch->pcsphereammount = 0;
        ch->pcsphere = 0;
	ch->pcgold = 0;
	ch->pcsilver = 0;
	ch->pccopper = 0;
	ch->pcremort = 0;
	ch->pcLegacy = 0;
	ch->profession = 0;
	ch->guild = 0;
	ch->pcdata->pc_build = 0;
        ch->pcdata->pc_eyes = 0;
        ch->pcdata->pc_hair = 0;
        ch->pcdata->pc_looks = 0;
    ch->form                            = 1048575;
    for( sn=0 ; sn < 7 ; sn ++ )
        ch->loc_hp[sn]                  = 0;
    for( sn=0 ; sn < 19 ; sn++ )
         ch->wpn[sn]                    = 0;
    for( sn=0 ; sn < 5 ; sn++ )
         ch->spl[sn]                    = 0;
    for( sn=0 ; sn < 8 ; sn++ )
         ch->cmbt[sn]                   = 0;
    for( sn=0 ; sn < 11 ; sn++ )
         ch->cmbt[sn]                   = 0;
    for (sn=0; sn < MAX_IGNORE; sn++)
         ch->pcdata->ignore[sn]         = -1;
    ch->pkill                           = 0;      
    ch->pdeath                          = 0;   
    ch->mkill                           = 0;   
    ch->mdeath                          = 0;   
    ch->class                           = 0;   
    ch->amount_attacks_dealt            = 0;
    ch->amount_attacks_recieved         = 0;
    ch->amount_damage_dealt             = 0;
    ch->amount_damage_recieved          = 0;
    ch->pcdata->followers               = 0;
    ch->pcdata->perm_str                = 13;
    ch->pcdata->perm_int                = 13;
    ch->pcdata->perm_wis                = 13;
    ch->pcdata->perm_dex                = 13;
    ch->pcdata->perm_con                = 13;
    ch->pcdata->quest                   = 0;
    ch->pcdata->religion                = 0;
    ch->pcdata->rank                    = 0;
    ch->pcdata->stage[0]                = 0;
    ch->pcdata->stage[1]                = 0;
    ch->pcdata->stage[2]                = 0;
    ch->pcdata->score[0]                = 0;
    ch->pcdata->score[1]                = 0; 
    ch->pcdata->score[2]                = 0;
    ch->pcdata->score[3]                = 0;
    ch->pcdata->score[4]                = 0;
    ch->pcdata->score[5]                = 0;

    for( sn=0 ; sn < 10 ; sn++ )
         ch->pcdata->genes[sn]          = 0;

    for ( sn=0 ; sn < 20 ; sn++)
        ch->pcdata->powers[sn]          = 0;

    for ( sn=0 ; sn < 12 ; sn++)
        ch->pcdata->stats[sn]           = 0;

    ch->pcdata->security                = 0;    /* OLC */

    ch->pcdata->obj_vnum                = 0;
    ch->pcdata->condition[COND_THIRST]  = 48;
    ch->pcdata->condition[COND_FULL]    = 48;

    found = FALSE;
    fclose( fpReserve );
    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( name ) );
    if ( ( fp = fopen( strsave, "r" ) ) != NULL )
    {
        int iNest;

        for ( iNest = 0; iNest < MAX_NEST; iNest++ )
            rgObjNest[iNest] = NULL;

        found = TRUE;
        for ( ; ; )
        {
            char letter;
            char *word;
    
            letter = fread_letter( fp );
            if ( letter == '*' )
            {
                fread_to_eol( fp );
                continue;
            }

            if ( letter != '#' )
            {
                bug( "Load_char_obj: # not found.", 0 );
                break;
            }

            word = fread_word( fp );
            if ( !str_cmp( word, "PLAYERS" ) ) fread_char    ( ch, fp );
            else if ( !str_cmp( word, "OBJECT" ) ) break;
            else if ( !str_cmp( word, "END"    ) ) break;
            else
            {
                bug( "Load_char_obj: bad section.", 0 );
                break;
            }
        }
        fclose( fp );
    }
    fpReserve = fopen( NULL_FILE, "r" );
    return found;
}


/*
 * Load a char and inventory into a new ch structure.
 */
bool load_char_obj( DESCRIPTOR_DATA *d, char *name)
{
    static PC_DATA pcdata_zero;
    char strsave[MAX_STRING_LENGTH];
    char *strtime;
    CHAR_DATA *ch;
    FILE *fp;
    bool found;
    int sn;

    if ( char_free == NULL )
    {
	ch                              = alloc_perm( sizeof(*ch) );
    }
    else
    {
	ch                              = char_free;
	char_free                       = char_free->next;
    }
    clear_char( ch );

    if ( pcdata_free == NULL )
    {
	ch->pcdata                      = alloc_perm( sizeof(*ch->pcdata) );
    }
    else
    {
	ch->pcdata                      = pcdata_free;
	pcdata_free                     = pcdata_free->next;
    }
    *ch->pcdata                         = pcdata_zero;

    d->character            = ch;
    ch->desc                            = d;
    ch->name                            = str_dup( name );
    ch->pcdata->switchname              = str_dup( name );
    ch->act                             = PLR_BRIEF3
                                        | PLR_AUTOEXIT
                                        | PLR_MAP
					| PLR_PROMPT;
    ch->pcdata->board                   = &boards[DEFAULT_BOARD];
    ch->pcdata->history                 = generate_history();
    ch->extra                           = 0;
    ch->deaf				= CHANNEL_DKTALK; /*no danish by default */
    ch->special                         = 0;
    ch->newbits                         = 0;
    ch->class                           = 0;
    ch->pcdata->familiar                = NULL;
    ch->pcdata->pfile                   = NULL;
    ch->pcdata->partner                 = NULL;
    ch->pcdata->propose                 = NULL;
    ch->pcdata->chobj                   = NULL;
    ch->pcdata->memorised               = NULL;
    ch->pcdata->safe_counter		= 0;
    ch->pcdata->relrank                 = 0;
    ch->pcdata->mean_paradox_counter	= 0;
    ch->pcdata->betting_amount          = 0;
    ch->pcdata->betting_char            = 0;
    ch->pcdata->tempflag                = 0; // always 0 at login, never saved 
    ch->pcdata->denied                  = current_time;
    ch->pcdata->playerid                = 0;
    ch->pcdata->time_tick               = 0;
    ch->pcdata->revision                = 0;
    ch->pcdata->faith                   = 0;
    ch->pcdata->current_faith           = 0;
    ch->pcdata->damreduct               = 100;
    ch->pcdata->damreductdec            = 0;
    ch->pcdata->rel_powers              = 0;
    ch->pcdata->jflags                  = 0;
    ch->pcdata->decapmessage            = str_dup( "" );
    ch->pcdata->avatarmessage           = str_dup( "" );
    ch->pcdata->logoutmessage           = str_dup( "" );
    ch->pcdata->loginmessage            = str_dup( "" );
    ch->pcdata->tiemessage              = str_dup( "" );
    ch->pcdata->last_global             = str_dup( "" );
    ch->pcdata->pwd                     = str_dup( "" );
    ch->pcdata->bamfin                  = str_dup( "" );
    ch->pcdata->bamfout                 = str_dup( "" );
    ch->pcdata->last_decap[0]           = str_dup( "" );
    ch->pcdata->last_decap[1]           = str_dup( "" );
    ch->pcdata->retaliation             = str_dup( "" );
    ch->pcdata->title                   = str_dup( "" );
    ch->pcdata->bounty			= 0;
    ch->pcdata->conception              = str_dup( "" );
    ch->pcdata->parents                 = str_dup( "" );
    ch->pcdata->cparents                = str_dup( "" );
    ch->pcdata->marriage                = str_dup( "" );
    ch->morph                           = str_dup( "" );
    ch->pload                           = str_dup( "" );
    ch->prompt                          = str_dup( "" );
    ch->cprompt                         = str_dup( "" );
    strtime = ctime( &current_time );
    strtime[strlen(strtime)-1] = '\0';
    free_string(ch->lasttime);
    ch->createtime                      = str_dup( strtime );
    ch->lasttime                        = str_dup( "" );
    ch->lasthost                        = str_dup( "" );
    ch->hunting                         = str_dup( "" );
    ch->pcdata->followers               = 0;
    ch->mounted                         = 0;
    ch->home                            = 2002;
    ch->damcap[0]                       = 1000;
    ch->damcap[1]                       = 0;
    ch->itemaffect                      = 0;
    ch->polyaff                         = 0;
    ch->immune                          = IMM_TRAVEL;
	ch->pRank   = 1;
	ch->pcRace = 0;
	ch->pcRaceLevel = 0;
        ch->pcpractice = 0;
        ch->pcwpnprof = 0;
        ch->pcmaxwpnprof = 0;
        ch->pcsphereammount = 0;
        ch->pcsphere = 0;
        ch->pcgold = 0;
        ch->pcsilver = 0;
        ch->pccopper = 0;
	ch->pcremort = 0;
	ch->pcLegacy = 0;
	ch->profession = 0;
	ch->guild = 0;
        ch->pcdata->pc_build = 0;
        ch->pcdata->pc_eyes = 0;
        ch->pcdata->pc_hair = 0;
        ch->pcdata->pc_looks = 0;

    ch->form                            = 1048575;
    for( sn=0 ; sn < 7 ; sn ++ )
	ch->loc_hp[sn]                  = 0;
    for( sn=0 ; sn < 19 ; sn++ )
	 ch->wpn[sn]                    = 0;
    for( sn=0 ; sn < 5 ; sn++ )
	 ch->spl[sn]                    = 0;
    for( sn=0 ; sn < 8 ; sn++ )
	 ch->cmbt[sn]                   = 0;
    for( sn=0 ; sn < 11 ; sn++ )
	 ch->cmbt[sn]                   = 0;
    for (sn=0; sn < MAX_IGNORE; sn++)
         ch->pcdata->ignore[sn]         = -1;

    ch->pkill                           = 0;
    ch->pdeath                          = 0;
    ch->mkill                           = 0;
    ch->mdeath                          = 0;
    ch->class                           = 0;
    ch->pcdata->followers               = 0;
    ch->pcdata->perm_str                = 13;
    ch->pcdata->perm_int                = 13; 
    ch->pcdata->perm_wis                = 13;
    ch->pcdata->perm_dex                = 13;
    ch->pcdata->perm_con                = 13;
    ch->pcdata->quest                   = 0;
    ch->pcdata->religion                = 0;
    ch->pcdata->rank                    = 0;
    ch->pcdata->stage[0]                = 0;
    ch->pcdata->stage[1]                = 0;
    ch->pcdata->stage[2]                = 0;
    ch->pcdata->score[0]                = 0;
    ch->pcdata->score[1]                = 0;
    ch->pcdata->score[2]                = 0;
    ch->pcdata->score[3]                = 0;
    ch->pcdata->score[4]                = 0;
    ch->pcdata->score[5]                = 0;

    for( sn=0 ; sn < 10 ; sn++ )
	 ch->pcdata->genes[sn]          = 0;

    for ( sn=0 ; sn < 20 ; sn++)
	ch->pcdata->powers[sn]          = 0;

    ch->pcdata->security                = 0;    /* OLC */

    ch->pcdata->obj_vnum                = 0;
    ch->pcdata->condition[COND_THIRST]  = 48;
    ch->pcdata->condition[COND_FULL]    = 48;

    found = FALSE;
    fclose( fpReserve );
    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( name ) );
    if ( ( fp = fopen( strsave, "r" ) ) != NULL )
    {
	int iNest;

	for ( iNest = 0; iNest < MAX_NEST; iNest++ )
	    rgObjNest[iNest] = NULL;

	found = TRUE;
	for ( ; ; )
	{
	    char letter;
	    char *word;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    if ( letter != '#' )
	    {
		bug( "Load_char_obj: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
	    if ( !str_cmp( word, "PLAYERS" ) ) fread_char    ( ch, fp );
	    else if ( !str_cmp( word, "OBJECT" ) ) fread_obj      ( ch, fp );
	    else if ( !str_cmp( word, "END"    ) ) break;
	    else
	    {
		bug( "Load_char_obj: bad section.", 0 );
		break;
	    }
	}
	fclose( fp );
    }
    fpReserve = fopen( NULL_FILE, "r" );
    return found;
}



/*
 * Read in a char.
 */

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )    if ( !str_cmp( word, literal ) ){field  = value;fMatch = TRUE;break;}


void fread_obj( CHAR_DATA *ch, FILE *fp )
{
    static OBJ_DATA obj_zero;
    OBJ_DATA *obj;
    char *word;
    int iNest;
    bool fMatch;
    bool fNest;
    bool fVnum;

    if ( obj_free == NULL )
    {
	obj             = alloc_perm( sizeof(*obj) );
    }
    else
    {
	obj             = obj_free;
	obj_free        = obj_free->next;
    }

    *obj                = obj_zero;
    obj->name           = str_dup( "" );
    obj->short_descr    = str_dup( "" );
    obj->description    = str_dup( "" );
    obj->questmaker     = str_dup( "" );
    obj->questowner     = str_dup( "" );
    obj->condition      = 100;
    obj->toughness      = 0;
    obj->resistance     = 100;
    obj->quest          = 0;
    obj->points         = 0;
    obj->ownerid        = 0;

    fNest               = FALSE;
    fVnum               = TRUE;
    iNest               = 0;

    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'A':
	    if ( !str_cmp( word, "Affect" ) || !str_cmp( word, "AffectData" ) )
	    {
		AFFECT_DATA *paf;

		if ( affect_free == NULL )
		{
		    paf         = alloc_perm( sizeof(*paf) );
		}
		else
		{
		    paf         = affect_free;
		    affect_free = affect_free->next;
		}

		paf->type       = 0;
		paf->duration   = fread_number( fp );
		paf->modifier   = fread_number( fp );
		paf->location   = fread_number( fp );
		paf->bitvector  = 0;
		paf->next       = obj->affected;
		obj->affected   = paf;
		fMatch          = TRUE;
		break;
	    }
	    break;

	case 'C':
            KEY( "CMaterial",   obj->c_material,             fread_number( fp ) );
	    KEY( "Condition",   obj->condition,         fread_number( fp ) );
	    KEY( "Cost",        obj->cost,              fread_number( fp ) );
	    break;

	case 'D':
	    KEY( "Description", obj->description,       fread_string( fp ) );
	    break;

	case 'E':
	    KEY( "ExtraFlags",  obj->extra_flags,       fread_number( fp ) );
	    KEY( "ExtraFlags2", obj->extra_flags2,      fread_number( fp ) );

	    if ( !str_cmp( word, "ExtraDescr" ) )
	    {
		EXTRA_DESCR_DATA *ed;

		if ( extra_descr_free == NULL )
		{
		    ed                  = alloc_perm( sizeof(*ed) );
		}
		else
		{
		    ed                  = extra_descr_free;
		    extra_descr_free    = extra_descr_free->next;
		}

		ed->keyword             = fread_string( fp );
		ed->description         = fread_string( fp );
		ed->next                = obj->extra_descr;
		obj->extra_descr        = ed;
		fMatch = TRUE;
	    }

	    if ( !str_cmp( word, "End" ) )
	    {
		if ( !fNest || !fVnum )
		{
		    bug( "Fread_obj: incomplete object.", 0 );
		    free_string( obj->name        );
		    free_string( obj->description );
		    free_string( obj->short_descr );
		    obj->next = obj_free;
		    obj_free  = obj;
		    return;
		}
		else
		{
		    obj->next   = object_list;
		    object_list = obj;
		    obj->pIndexData->count++;
		    if ( iNest == 0 || rgObjNest[iNest] == NULL )
			obj_to_char( obj, ch );
		    else
			obj_to_obj( obj, rgObjNest[iNest-1] );
		    return;
		}
	    }
	    break;

	case 'I':
	    KEY( "ItemType",    obj->item_type,         fread_number( fp ) );
	    break;

	case 'L':
	    KEY( "Level",       obj->level,             fread_number( fp ) );
	    break;

        case 'M':
            KEY( "Material",    obj->material,             fread_number( fp ) );
            break;

	case 'N':

	    KEY( "Name",        obj->name,              fread_string( fp ) );

	    if ( !str_cmp( word, "Nest" ) )
	    {
		iNest = fread_number( fp );
		if ( iNest < 0 || iNest >= MAX_NEST )
		{
		    bug( "Fread_obj: bad nest %d.", iNest );
		}
		else
		{
		    rgObjNest[iNest] = obj;
		    fNest = TRUE;
		}
		fMatch = TRUE;
	    }
	    break;

        case 'O':
            KEY( "OwnerID",     obj->ownerid,           fread_number( fp ) );
            break;

	case 'P':
	    KEY( "Points",      obj->points,            fread_number( fp ) );
	    break;

	case 'Q':
	    KEY( "Quest",       obj->quest,             fread_number( fp ) );
	    KEY( "Questmaker",  obj->questmaker,        fread_string( fp ) );
	    KEY( "Questowner",  obj->questowner,        fread_string( fp ) );
	    break;

	case 'R':
	    KEY( "Resistance",  obj->resistance,        fread_number( fp ) );
	    break;

	case 'S':
	    KEY( "ShortDescr",  obj->short_descr,       fread_string( fp ) );

	    if ( !str_cmp( word, "Spell" ) )
	    {
		int iValue;
		int sn;

		iValue = fread_number( fp );
		sn     = skill_lookup(fread_word( fp ));
		if ( iValue < 0 || iValue > 3 )
		{
		    bug( "Fread_obj: bad iValue %d.", iValue );
		}
		else if ( sn < 0 )
		{
		    bug( "Fread_obj: unknown skill.", 0 );
		}
		else
		{
		    obj->value[iValue] = sn;
		}
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'T':
	    KEY( "Timer",       obj->timer,             fread_number( fp ) );
	    KEY( "Toughness",   obj->toughness,         fread_number( fp ) );
	    break;

	case 'V':
	    if ( !str_cmp( word, "Values" ) )
	    {
		obj->value[0]   = fread_number( fp );
		obj->value[1]   = fread_number( fp );
		obj->value[2]   = fread_number( fp );
		obj->value[3]   = fread_number( fp );
		fMatch          = TRUE;
		break;
	    }

	    if ( !str_cmp( word, "Vnum" ) )     /* OLC */
	    {
		int vnum;

		vnum = fread_number( fp );
		if ( !( obj->pIndexData = get_obj_index( vnum ) ) )
			obj->pIndexData = get_obj_index( OBJ_VNUM_DUMMY );
    
		fVnum = TRUE;
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'W':
	    KEY( "WeapFlags",   obj->weapflags,         fread_number( fp ) );
	    KEY( "WearFlags",   obj->wear_flags,        fread_number( fp ) );
	    KEY( "WearLoc",     obj->wear_loc,          fread_number( fp ) );
	    KEY( "Weight",      obj->weight,            fread_number( fp ) );
	    break;
	}
	if ( !fMatch )
	{
	    bug( "Fread_obj: no match.", 0 );
	    fread_to_eol( fp );
	}
    }

}



void fread_char( CHAR_DATA *ch, FILE *fp )
{
    char buf[MAX_STRING_LENGTH];
    char *word;
    bool fMatch;
    int sn;
    int x;

    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

	case 'A':
	    KEY( "Act",         ch->act,                fread_number( fp ) );
	    KEY( "AffectedBy",  ch->affected_by,        fread_number( fp ) );
	    KEY( "Alignment",   ch->alignment,          fread_number( fp ) );
            if ( !str_cmp( word, "Alloy" ) )
            {
                for (x=0;x<20;x++)
                {
                ch->alloy[x] = fread_number( fp );
                }
                fMatch = TRUE;
                break;
            }

	    KEY( "Armor",       ch->armor,              fread_number( fp ) );
            KEY( "Avatarmessage", ch->pcdata->avatarmessage,  fread_string( fp ) );
	    KEY( "Awin",        ch->pcdata->awins,      fread_number( fp ) );
	    KEY( "Alos",        ch->pcdata->alosses,    fread_number( fp ) );
	    if ( !str_cmp( word, "Affect" ) || !str_cmp( word, "AffectData" ) )
	    {
		AFFECT_DATA *paf;

		if ( affect_free == NULL )
		{
		    paf         = alloc_perm( sizeof(*paf) );
		}
		else
		{
		    paf         = affect_free;
		    affect_free = affect_free->next;
		}

		if ( !str_cmp( word, "Affect" ) )
		{
		    /* Obsolete 2.0 form. */
		    paf->type   = fread_number( fp );
		}
		else
		{
		    int sn;
		    sn = skill_lookup(fread_word( fp ));
		    if ( sn < 0 )
			bug( "Fread_char: unknown skill.", 0 );
		    else
			paf->type = sn;
		}

		paf->duration   = fread_number( fp );
		paf->modifier   = fread_number( fp );
		paf->location   = fread_number( fp );
		paf->bitvector  = fread_number( fp );
		paf->next       = ch->affected;
		ch->affected    = paf;
		fMatch = TRUE;
		break;
	    }

	    if ( !str_cmp( word, "AttrMod"  ) )
	    {
		ch->pcdata->mod_str  = fread_number( fp );
		ch->pcdata->mod_int  = fread_number( fp );
		ch->pcdata->mod_wis  = fread_number( fp );
		ch->pcdata->mod_dex  = fread_number( fp );
		ch->pcdata->mod_con  = fread_number( fp );
		fMatch = TRUE;
		break;
	    }

	    if ( !str_cmp( word, "AttrPerm" ) )
	    {
		ch->pcdata->perm_str = fread_number( fp );
		ch->pcdata->perm_int = fread_number( fp );
		ch->pcdata->perm_wis = fread_number( fp );
		ch->pcdata->perm_dex = fread_number( fp );
		ch->pcdata->perm_con = fread_number( fp );
		fMatch = TRUE;
		break;
	    }

            if (!str_cmp(word, "Alias"))
            {
              ALIAS_DATA *ali;

              if (alias_free == NULL )
              {
                ali = alloc_perm(sizeof(*ali));
              }
              else
              {
                ali = alias_free;
                alias_free = alias_free->next;
              }
              ali->short_n = fread_string(fp);
              ali->long_n = fread_string(fp);
              ali->next = ch->pcdata->alias;
              ch->pcdata->alias = ali;
              ch->pcdata->alias_count++;
              fMatch = TRUE;
            }
	    break;

	case 'B':
	    KEY( "Bamfin",      ch->pcdata->bamfin,     fread_string( fp ) );
	    KEY( "Bamfout",     ch->pcdata->bamfout,    fread_string( fp ) );
       	    KEY( "Bounty",  ch->pcdata->bounty, fread_number( fp ));

          if (!str_cmp(word, "Breath1" )) {fMatch = TRUE; fread_string(fp); break; }
          if (!str_cmp(word, "Breath2" )) {fMatch = TRUE; fread_string(fp); break; }  
          if (!str_cmp(word, "Breath3" )) {fMatch = TRUE; fread_string(fp); break; }  
          if (!str_cmp(word, "Breath4" )) {fMatch = TRUE; fread_string(fp); break; }  
          if (!str_cmp(word, "Breath5" )) {fMatch = TRUE; fread_string(fp); break; }  

		/* Read in board status */          
	    if (!str_cmp(word, "Boards" ))
	    {
			int i,num = fread_number (fp); /* number of boards saved */
			char *boardname;
			
			for (; num ; num-- ) /* for each of the board saved */
			{
				boardname = fread_word (fp);
				i = board_lookup (boardname); /* find board number */
				
				if (i == BOARD_NOTFOUND) /* Does board still exist ? */
				{
					sprintf (buf, "fread_char: %s had unknown board name: %s. Skipped.", ch->name, boardname);                                      
					log_string (buf);
					fread_number (fp); /* read last_note and skip info */
				}
				else /* Save it */
					ch->pcdata->last_note[i] = fread_number(fp);
			}        /* for */
			
			fMatch = TRUE;
	    } /* Boards */
	    



	    break;

	case 'C':
        KEY( "Class",       ch->class,              fread_number( fp ) );
        KEY( "Currentfaith",     ch->pcdata->current_faith,   fread_number( fp ) );

	    if ( !str_cmp( word, "Combat" ) )
	    {
		ch->cmbt[0]     = fread_number( fp );
		ch->cmbt[1]     = fread_number( fp );
		ch->cmbt[2]     = fread_number( fp );
		ch->cmbt[3]     = fread_number( fp );
		ch->cmbt[4]     = fread_number( fp );
		ch->cmbt[5]     = fread_number( fp );
		ch->cmbt[6]     = fread_number( fp );
		ch->cmbt[7]     = fread_number( fp );
		fMatch = TRUE;
		break;
	    }

	    KEY( "Conception",  ch->pcdata->conception, fread_string( fp ) );

	    if ( !str_cmp( word, "Condition" ) )
	    {
		ch->pcdata->condition[0] = fread_number( fp );
		ch->pcdata->condition[1] = fread_number( fp );
		ch->pcdata->condition[2] = fread_number( fp );
		fMatch = TRUE;
		break;
	    }
	    KEY( "Cparents",    ch->pcdata->cparents,   fread_string( fp ) );
	    KEY( "Cprompt",     ch->cprompt,            fread_string( fp ) );
	    KEY( "Createtime",  ch->createtime,         fread_string( fp ) );
	    break;

	case 'D':
	    KEY( "Damroll",     ch->damroll,            fread_number( fp ) );
	    KEY( "Deaf",        ch->deaf,               fread_number( fp ) );
	    if ( !str_cmp( word, "DamRed" ) )
	    {
	      ch->pcdata->damreduct = fread_number(fp);
	      ch->pcdata->damreductdec = fread_number(fp);
		fMatch = TRUE;
		break;
	    }
            KEY( "Decapmessage", ch->pcdata->decapmessage,  fread_string( fp ));
            KEY( "Denied",      ch->pcdata->denied,     fread_number( fp ) );
	    KEY( "Description", ch->description,        fread_string( fp ) );
	    break;

	case 'E':
            if (!str_cmp(word, "Email")) {fMatch = TRUE; fread_string(fp); break; }
	    if ( !str_cmp( word, "End" ) )
		return;
	    KEY( "Exp",         ch->exp,                fread_double( fp ) );
	    KEY( "Extra",       ch->extra,              fread_number( fp ) );
	    break;

	case 'F':
	    KEY( "Faith",        ch->pcdata->faith,       fread_number( fp ) );
            KEY( "Flag2",  ch->flag2, fread_number( fp ) );
            KEY( "Form",        ch->form,               fread_number( fp ) );
	    break;

	case 'G':
	KEY( "Generation", ch->generation, fread_number( fp ) );
	    if ( !str_cmp( word, "Genes" ) )
	    {
		ch->pcdata->genes[0] = fread_number( fp );
		ch->pcdata->genes[1] = fread_number( fp );
		ch->pcdata->genes[2] = fread_number( fp );
		ch->pcdata->genes[3] = fread_number( fp );
		ch->pcdata->genes[4] = fread_number( fp );
		ch->pcdata->genes[5] = fread_number( fp );
		ch->pcdata->genes[6] = fread_number( fp );
		ch->pcdata->genes[7] = fread_number( fp );
		ch->pcdata->genes[8] = fread_number( fp );
		ch->pcdata->genes[9] = fread_number( fp );
		fMatch = TRUE;
		break;
	    }
	  KEY( "Guild", ch->guild, fread_number( fp ) );

	    break;

	case 'H':
	    KEY( "Hitroll",     ch->hitroll,            fread_number( fp ) );
	    KEY( "Home",        ch->home,               fread_number( fp ) );
	    if ( !str_cmp( word, "HpManaMove" ) )
	    {
		ch->hit         = fread_number( fp );
		ch->max_hit     = fread_number( fp );
		ch->mana        = fread_number( fp );
		ch->max_mana    = fread_number( fp );
		ch->move        = fread_number( fp );
		ch->max_move    = fread_number( fp );
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'I':
	    KEY( "Immune",        ch->immune,           fread_number( fp ) );
	    KEY( "Itemaffect",    ch->itemaffect,       fread_number( fp ) );
	    break;

        case 'J':
            KEY( "Jflags",        ch->pcdata->jflags,   fread_number( fp ) );
            break;

	case 'L':
	    KEY( "Lasthost",    ch->lasthost,           fread_string( fp ) );
            KEY( "Lastdecap1",  ch->pcdata->last_decap[0],     fread_string( fp ) );
            KEY( "Lastdecap2",  ch->pcdata->last_decap[1],     fread_string( fp ) );
	    KEY( "Lasttime",    ch->lasttime,           fread_string( fp ) );
	    KEY( "Level",       ch->level,              fread_number( fp ) );
	    if ( !str_cmp( word, "Locationhp" ) )
	    {
		ch->loc_hp[0]   = fread_number( fp );
		ch->loc_hp[1]   = fread_number( fp );
		ch->loc_hp[2]   = fread_number( fp );
		ch->loc_hp[3]   = fread_number( fp );
		ch->loc_hp[4]   = fread_number( fp );
		ch->loc_hp[5]   = fread_number( fp );
		ch->loc_hp[6]   = fread_number( fp );
		fMatch = TRUE;
		break;
	    }
            KEY( "Loginmessage", ch->pcdata->loginmessage,  fread_string( fp ) );
            KEY( "Logoutmessage", ch->pcdata->logoutmessage,  fread_string( fp ) );
	    KEY( "LongDescr",   ch->long_descr,         fread_string( fp ) );
            if (!str_cmp(word, "Lord"))
            {
              fread_string(fp);
              fMatch = TRUE;
              break;
            }
	    break;

	case 'M':
        KEY( "Meanparadox",       ch->pcdata->mean_paradox_counter, fread_number( fp ) );
	    KEY( "Marriage",    ch->pcdata->marriage,   fread_string( fp ) );
	    KEY( "Morph",       ch->morph,              fread_string( fp ) );
	    break;
	case 'N':
	    KEY( "Newbits",     ch->newbits,    fread_number( fp ) );

	    if ( !str_cmp( word, "Name" ) )
	    {
		/*
		 * Name already set externally.
		 */
		fread_to_eol( fp );
		fMatch = TRUE;
		break;
	    }

	    break;

	case 'O':
	    KEY( "Objvnum",     ch->pcdata->obj_vnum,   fread_number( fp ) );
	    break;

	case 'P':
                        KEY("PlayerRank", ch->pRank, fread_number( fp ) );
			KEY("PlayerRace", ch->pcRace, fread_number( fp ) );
			KEY("PlayerRaceLevel", ch->pcRaceLevel, fread_number( fp ) );
			KEY("PcPractice", ch->pcpractice, fread_number( fp ) );
                        KEY("Pcwpnprof", ch->pcwpnprof, fread_number( fp ) );
                        KEY("Pcmaxwpnprof", ch->pcmaxwpnprof, fread_number( fp ) );
                        KEY("Pcsphereammount", ch->pcsphereammount, fread_number( fp ) );
                        KEY("Pcsphere", ch->pcsphere, fread_number( fp ) );
			KEY("Pcgold", ch->pcgold, fread_number( fp ) );
                        KEY("Pcsilver", ch->pcsilver, fread_number( fp ) );
                        KEY("Pccopper", ch->pccopper, fread_number( fp ) );
                        KEY("Pc_build", ch->pcdata->pc_build, fread_number( fp ) );
                        KEY("Pc_eyes", ch->pcdata->pc_eyes, fread_number( fp ) );
                        KEY("Pc_hair", ch->pcdata->pc_hair, fread_number( fp ) );
                        KEY("Pc_looks", ch->pcdata->pc_looks, fread_number( fp ) );

            if ( !str_cmp( word, "Pcstyle" ) )
            {
                ch->pcstyle[0]   = fread_number( fp );
                ch->pcstyle[1]   = fread_number( fp );
                ch->pcstyle[2]   = fread_number( fp );
                ch->pcstyle[3]   = fread_number( fp );
                fMatch = TRUE;
                break;
            }
                        KEY("Pcremort", ch->pcremort, fread_number( fp ) );
                        KEY("PcLegacy", ch->pcLegacy, fread_number( fp ) );

	    KEY( "Parents",     ch->pcdata->parents,    fread_string( fp ) );
	    KEY( "Password",    ch->pcdata->pwd,        fread_string( fp ) );
	    KEY( "Played",      ch->played,             fread_number( fp ) );
            KEY( "PlayerID",    ch->pcdata->playerid,   fread_number( fp ) );
	    KEY( "Polyaff",     ch->polyaff,            fread_number( fp ) );

	    if ( !str_cmp( word, "Power" ) )
	    {
		for ( sn = 0; sn < 20 ; sn++ )
			ch->pcdata->powers[sn] = fread_number( fp );

		fMatch = TRUE;
		break;
	    }

	    KEY( "Position",    ch->position,           fread_number( fp ) );
	    KEY( "Practice",    ch->practice,           fread_number( fp ) );
	    if ( !str_cmp( word, "PkPdMkMd" ) )
	    {
		ch->pkill       = fread_number( fp );
		ch->pdeath      = fread_number( fp );
		ch->mkill       = fread_number( fp );
		ch->mdeath      = fread_number( fp );
		fMatch = TRUE;
		break;
	    }
	    KEY( "Prof", ch->profession, fread_number( fp) );
	    KEY( "Prompt",      ch->prompt,             fread_string( fp ) );
	    break;

	case 'Q':
	    KEY( "Quest",        ch->pcdata->quest,     fread_number( fp ) );
	    KEY( "Questsrun",	 ch->pcdata->questsrun, fread_number( fp ) );
	    KEY( "Queststotal",	 ch->pcdata->questtotal, fread_number( fp ) );
            if (!str_cmp(word, "QuestData"))
            {
              QUEST_DATA *quest;
            
              if (quest_free == NULL)
              {
                quest = alloc_perm(sizeof(*quest));
              }
              else
              {
                quest = quest_free;
                quest_free = quest_free->next;
              }
              quest->time     = fread_number(fp);
              quest->type     = fread_number(fp);
              quest->giver    = fread_number(fp);
              quest->vnums[0] = fread_number(fp);
              quest->vnums[1] = fread_number(fp);
              quest->vnums[2] = fread_number(fp);
              quest->vnums[3] = fread_number(fp);
              quest->next = ch->pcdata->quests;
              ch->pcdata->quests = quest;
              fMatch = TRUE;
              break;
            }
            break;

	case 'R':
            KEY( "Rank",        ch->pcdata->rank,       fread_number( fp ) );

	    if ( !str_cmp( word, "RawMaterial" ) )
            {
		for (x=0;x<50;x++)
		{
                ch->rawmaterial[x] = fread_number( fp );
		}
                fMatch = TRUE;
                break;
            }

            KEY( "Religion",       ch->pcdata->religion,    fread_number(fp));
            KEY( "Relrank",        ch->pcdata->relrank,       fread_number( fp ) );
            KEY( "Relpowers",      ch->pcdata->rel_powers,    fread_number( fp ) );
            KEY( "Retaliation",    ch->pcdata->retaliation,   fread_string( fp ) );
            KEY( "Revision",       ch->pcdata->revision,      fread_number( fp ) );
	    if ( !str_cmp( word, "Room" ) )
	    {
		ch->in_room = get_room_index( fread_number( fp ) );
		if ( ch->in_room == NULL )
		    ch->in_room = get_room_index( ROOM_VNUM_LIMBO );
		fMatch = TRUE;
		break;
	    }

	    break;

	case 'S':
          if (!str_cmp(word, "Smite" )) {fMatch = TRUE; fread_string(fp); break; }
	    KEY( "SavingThrow", ch->saving_throw,       fread_number( fp ) );
	    KEY( "Switchname",  ch->pcdata->switchname, fread_string( fp ) );
	    if ( !str_cmp( word, "Score" ) )
	    {
		ch->pcdata->score[0] = fread_number( fp );
		ch->pcdata->score[1] = fread_number( fp );
		ch->pcdata->score[2] = fread_number( fp );
		ch->pcdata->score[3] = fread_number( fp );
		ch->pcdata->score[4] = fread_number( fp );
		ch->pcdata->score[5] = fread_number( fp );
		fMatch = TRUE;
		break;
	    }
	    KEY( "Sex",         ch->sex,                fread_number( fp ) );
	    KEY( "ShortDescr",  ch->short_descr,        fread_string( fp ) );
		KEY( "Security", ch->pcdata->security,  fread_number(fp));
	    if ( !str_cmp( word, "Skill" ) )
	    {
		int sn;
		int value;

		value = fread_number( fp );
		sn    = skill_lookup(fread_word( fp ));

		if ( sn >= 0 )
		    ch->pcdata->learned[sn] = value;
		fMatch = TRUE;
	    }
	    KEY( "Special",     ch->special,    fread_number( fp ) );


	    if ( !str_cmp( word, "Spells" ) )
	    {
		ch->spl[0]      = fread_number( fp );
		ch->spl[1]      = fread_number( fp );
		ch->spl[2]      = fread_number( fp );
		ch->spl[3]      = fread_number( fp );
		ch->spl[4]      = fread_number( fp );
		fMatch = TRUE;
		break;
	    }

	    if ( !str_cmp( word, "Stage" ) )
	    {
		ch->pcdata->stage[0] = fread_number( fp );
		ch->pcdata->stage[1] = fread_number( fp );
		ch->pcdata->stage[2] = fread_number( fp );
		fMatch = TRUE;
		break;
	    }

	    if ( !str_cmp( word, "Stance" ) )
	    {
		ch->stance[0]   = fread_number( fp );
		ch->stance[1]   = fread_number( fp );
		ch->stance[2]   = fread_number( fp );
		ch->stance[3]   = fread_number( fp );
		ch->stance[4]   = fread_number( fp );
		ch->stance[5]   = fread_number( fp );
		ch->stance[6]   = fread_number( fp );
		ch->stance[7]   = fread_number( fp );
		ch->stance[8]   = fread_number( fp );
		ch->stance[9]   = fread_number( fp );
		ch->stance[10]  = fread_number( fp );
		ch->stance[11]  = fread_number( fp );
		fMatch = TRUE;
		break;
	    }

            if( !str_cmp( word, "Stance2" ) )
            {
               ch->stance[12] = fread_number(fp);
               ch->stance[13] = fread_number(fp);
               ch->stance[14] = fread_number(fp);
               ch->stance[15] = fread_number(fp);
               ch->stance[16] = fread_number(fp);
               ch->stance[17] = fread_number(fp);
               ch->stance[18] = fread_number(fp);
               ch->stance[19] = fread_number(fp);
               ch->stance[20] = fread_number(fp);
               ch->stance[21] = fread_number(fp);
               ch->stance[22] = fread_number(fp);
               ch->stance[23] = fread_number(fp);
               fMatch = TRUE;
               break;
            }

     
                 if ( !str_cmp( word, "Stats" ) )
            {
                for ( sn = 0; sn < 12 ; sn++ )
                        ch->pcdata->stats[sn] = fread_number( fp );
                fMatch = TRUE;
                break;
            }


	    break;

	case 'T':
            KEY( "Tiemessage",  ch->pcdata->tiemessage,  fread_string( fp ));
	    KEY( "Trust",       ch->trust,              fread_number( fp ) );
            KEY( "Timetick",    ch->pcdata->time_tick,    fread_number( fp ) );

	    if ( !str_cmp( word, "Title" ) )
	    {
		ch->pcdata->title = fread_string( fp );
		if ( isalpha(ch->pcdata->title[0])
		||   isdigit(ch->pcdata->title[0]) )
		{
		    sprintf( buf, " %s", ch->pcdata->title );
		    free_string( ch->pcdata->title );
		    ch->pcdata->title = str_dup( buf );
		}
		fMatch = TRUE;
		break;
	    }

	    break;

	case 'V':
	    if ( !str_cmp( word, "Vnum" ) )
	    {
		ch->pIndexData = get_mob_index( fread_number( fp ) );
		fMatch = TRUE;
		break;
	    }
	    break;

	case 'W':
	    if ( !str_cmp( word, "Weapons" ) )
	    {
		ch->wpn[0]      = fread_number( fp );
		ch->wpn[1]      = fread_number( fp );
		ch->wpn[2]      = fread_number( fp );
		ch->wpn[3]      = fread_number( fp );
		ch->wpn[4]      = fread_number( fp );
		ch->wpn[5]      = fread_number( fp );
		ch->wpn[6]      = fread_number( fp );
		ch->wpn[7]      = fread_number( fp );
		ch->wpn[8]      = fread_number( fp );
		ch->wpn[9]      = fread_number( fp );
		ch->wpn[10]     = fread_number( fp );
		ch->wpn[11]     = fread_number( fp );
		ch->wpn[12]     = fread_number( fp );
                ch->wpn[13]     = fread_number( fp );
                ch->wpn[14]     = fread_number( fp );
                ch->wpn[15]     = fread_number( fp );
                ch->wpn[16]     = fread_number( fp );
                ch->wpn[17]     = fread_number( fp );
                ch->wpn[18]     = fread_number( fp );

		fMatch = TRUE;
		break;
	    }
	    break;
	}

	if ( !fMatch )
	{
	    sprintf(buf, "Fread_char: no match. WORD: %s", word); 
	    bug( buf, 0 );
	    fread_to_eol( fp );
	}
    }
}

double fread_double (FILE * fp)
{
  double number;
  bool sign;
  char *ptr;
  int c;

  ptr = top_string + sizeof (char *);

  do
    {
      c = getc (fp);
      *ptr = c;
    }
  while (isspace (c));

  number = 0;

  sign = FALSE;
  if (c == '+')
    {
      c = getc (fp);
      *ptr = c;
    }
  else if (c == '-')
    {
      sign = TRUE;
      c = getc (fp);
      *ptr = c;
    }

  if (!isdigit (c))
    {
      bug ("Fread_number: bad format.  save.c", 0);
      return 1;
    }

  while (isdigit (c))
    {
      number = number * 10 + c - '0';
      c = getc (fp);
      *ptr = c;
    }
  if (sign)
    number = 0 - number;
  if (c == '|')
    number += fread_number (fp);
  else if (c != ' ')
    ungetc (c, fp);

  return number;
}

