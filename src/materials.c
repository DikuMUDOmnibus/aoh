#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"


void do_materials(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  OBJ_DATA *obj = NULL;
  int x;

send_to_char( "#L[#7Material#L][#7Amount#L][#7Lb Needed#L][#7Toughness#L][#7AFF#L]\n\r",ch);
 
sprintf(buf, "#0[#oCopper#0]#n:     %-10d #L[#7%d#L] #L[#7%d#L] #7+400 Mana\n\r", 
	ch->rawmaterial[M_COPPER], LB_METAL, T_COPPER);
send_to_char(buf, ch);
sprintf(buf, "#0[#YGold#0]#n:       %-10d #L[#7%d#L] #L[#7%d#L] #7+400 Mana\n\r", 
	ch->rawmaterial[M_GOLD], LB_METAL, T_GOLD);
send_to_char(buf, ch);
sprintf(buf, "#0[#7Ivory#0]#n:      %-10d #L[#7%d#L] #L[#7%d#L] #7+125 Damroll\n\r", 
	ch->rawmaterial[M_IVORY], LB_OTHER, T_IVORY);
send_to_char(buf, ch);
sprintf(buf, "#0[#cM#Ci#ct#Ch#cr#Ci#cl#0]#n:    %-10d #L[#7%d#L] #L[#7%d#L] #7+400 Hp\n\r", 
	ch->rawmaterial[M_MITHRIL], LB_METAL, T_MITHRIL);
send_to_char(buf, ch);
sprintf(buf, "#0[#oWood#0]#n:       %-10d #L[#7%d#L] #L[#7%d#L] #7+400 Hp\n\r", 
	ch->rawmaterial[M_WOOD], LB_METAL, T_WOOD);
send_to_char(buf, ch);
sprintf(buf, "#0[#7O#gr#Gg#ga#7n#gi#Gc#0]#n:    %-10d #L[#7%d#L] #L[#7%d#L] #7Regeneration\n\r", 
	ch->rawmaterial[M_ORGANIC], LB_OTHER, T_ORGANIC);
send_to_char(buf, ch);
sprintf(buf, "#0[#7Diamond#0]#n:    %-10d #L[#7%d#L] #L[#7%d#L] #7Sanctuary\n\r", 
	ch->rawmaterial[M_DIAMOND], LB_GEM, T_DIAMOND);
send_to_char(buf, ch);
sprintf(buf, "#0[#7Platinum#0]#n:   %-10d #L[#7%d#L] #L[#7%d#L] #7+125 Hitroll\n\r", 
	ch->rawmaterial[M_PLATINUM], LB_METAL, T_PLATINUM);
send_to_char(buf, ch);
sprintf(buf, "#0[#CAdamantite#0]#n: %-10d #L[#7%d#L] #L[#7%d#L] #7+400 Hp\n\r", 
	ch->rawmaterial[M_ADAMANTITE], LB_METAL, T_ADAMANTITE);
send_to_char(buf, ch);

sprintf(buf, "#0[Tin#0]#n:        %-10d #L[#7%d#L] #L[#7%d#L] #7+400 Move\n\r", 
	ch->rawmaterial[M_TIN], LB_METAL, T_TIN);
send_to_char(buf, ch);
sprintf(buf, "#0[#nEbony#0]#n:      %-10d #L[#7%d#L] #L[#7%d#L] #7-100 Ac\n\r", ch->rawmaterial[M_EBONY], LB_OTHER, 
T_EBONY);
send_to_char(buf, ch);
sprintf(buf, "#0[#RR#ru#Rb#ry#0]#n:       %-10d #L[#7%d#L] #L[#7%d#L] #7Haste\n\r", ch->rawmaterial[M_RUBY], LB_GEM, 
T_RUBY);
send_to_char(buf, ch);
sprintf(buf, "#0[#7L#Gi#7m#Ge#7s#Gt#7o#Gn#7e#0]#n:  %-10d #L[#7%d#L] #L[#7%d#L] #7+400 Move\n\r", 
ch->rawmaterial[M_LIMESTONE], LB_OTHER, T_LIMESTONE);
send_to_char(buf, ch);
sprintf(buf, "#0[#YTopaz#0]#n:      %-10d #L[#7%d#L] #L[#7%d#L] #7Detect Hidden\n\r", ch->rawmaterial[M_TOPAZ], 
LB_GEM, T_TOPAZ);
send_to_char(buf, ch);
sprintf(buf, "#0[#pAmethyst#0]#n:   %-10d #L[#7%d#L] #L[#7%d#L] #7Fly\n\r", ch->rawmaterial[M_AMETHYST], LB_GEM, 
T_AMETHYST);
send_to_char(buf, ch);
sprintf(buf, "#0[#GJade#0]#n:       %-10d #L[#7%d#L] #L[#7%d#L] #7Infravision\n\r", ch->rawmaterial[M_JADE], 
LB_GEM, T_JADE);
send_to_char(buf, ch);
sprintf(buf, "#0[#nOnyx#0]#n:       %-10d #L[#7%d#L] #L[#7%d#L] #7Sneak\n\r", ch->rawmaterial[M_ONYX], LB_GEM, T_ONYX);
send_to_char(buf, ch);
sprintf(buf, "#0[#GE#gm#Ge#gr#Ga#gl#Gd#0]#n:    %-10d #L[#7%d#L] #L[#7%d#L] #7Slash through armor\n\r", 
ch->rawmaterial[M_EMERALD], LB_GEM, T_EMERALD);
send_to_char(buf, ch);
sprintf(buf, "#0[#7Pearl#0]#n:      %-10d #L[#7%d#L] #L[#7%d#L] #7Invis\n\r", ch->rawmaterial[M_PEARL], LB_GEM, 
T_PEARL);
send_to_char(buf, ch);
sprintf(buf, "#0[#7Opal#0]#n:       %-10d #L[#7%d#L] #L[#7%d#L] #7Pass Door\n\r", ch->rawmaterial[M_OPAL], LB_GEM, 
T_OPAL);
send_to_char(buf, ch);
sprintf(buf, "#0[#LSapphire#0]#n:   %-10d #L[#7%d#L] #L[#7%d#L] #7Chaos Shield\n\r", ch->rawmaterial[M_SAPPHIRE], 
LB_GEM, T_SAPPHIRE);
send_to_char(buf, ch);
sprintf(buf, "#0[#oLeather#0]#n:    %-10d #L[#7%d#L] #L[#7%d#L] #7-100 Ac\n\r", ch->rawmaterial[M_LEATHER], 
LB_OTHER, T_LEATHER);
send_to_char(buf, ch);
sprintf(buf, "#0[#0Iron#0]#n:       %-10d #L[#7%d#L] #L[#7%d#L] #7-100 Ac\n\r", ch->rawmaterial[M_IRON], LB_METAL, 
T_IRON);
send_to_char(buf, ch);
sprintf(buf, "#0[#7Quartz#0]#n:     %-10d #L[#7%d#L] #L[#7%d#L] #7-100Ac\n\r", ch->rawmaterial[M_QUARTZ], LB_GEM, 
T_QUARTZ);
send_to_char(buf, ch);
sprintf(buf, "#0[#CM#ca#Cr#cb#Cl#ce#0]#n:     %-10d #L[#7%d#L] #L[#7%d#L] #7+400 Hp \n\r", 
ch->rawmaterial[M_MARBLE], LB_OTHER, T_MARBLE);
send_to_char(buf, ch);
sprintf(buf, ":#0[#0Silver#0]#n     %-10d #L[#7%d#L] #L[#7%d#L] #7-100 Ac\n\r", ch->rawmaterial[M_SILVER], 
LB_METAL, T_SILVER);
send_to_char(buf, ch);
sprintf(buf, "#0[#oWool#0]#n:       %-10d #L[#7%d#L] #L[#7%d#L] #7+400 Move\n\r", ch->rawmaterial[M_WOOL], 
LB_OTHER, T_WOOL);
send_to_char(buf, ch);
sprintf(buf, "#0[#LCloth#0]#n:      %-10d #L[#7%d#L] #L[#7%d#L] #7+400 Move\n\r", ch->rawmaterial[M_CLOTH], 
LB_OTHER, T_CLOTH);
send_to_char(buf, ch);



sprintf(buf, "#0[#cAluminium#0]#n:  %-10d #L[#7%d#L] #L[#7%d#L] \n\r", ch->rawmaterial[M_ALUMINIUM], 
LB_METAL, T_ALUMINIUM);
send_to_char(buf, ch);
sprintf(buf, "#0[#PBismuth#0]#n:    %-10d #L[#7%d#L] #L[#7%d#L] \n\r", ch->rawmaterial[M_BISMUTH], 
LB_OTHER, T_BISMUTH);
send_to_char(buf, ch);

sprintf(buf, "#0[#0Cobalt#0]#n:     %-10d #L[#7%d#L] #L[#7%d#L] \n\r", ch->rawmaterial[M_COBALT], 
LB_OTHER, T_COBALT);
send_to_char(buf, ch);

sprintf(buf, "#0[#PGallium#0]#n:    %-10d #L[#7%d#L] #L[#7%d#L] \n\r", ch->rawmaterial[M_GALLIUM], 
LB_OTHER, T_GALLIUM);
send_to_char(buf, ch);

sprintf(buf, "#0[#PIndium#0]#n:     %-10d #L[#7%d#L] #L[#7%d#L] \n\r", ch->rawmaterial[M_INDIUM], 
LB_OTHER, T_INDIUM);
send_to_char(buf, ch);

sprintf(buf, "#0[#0Lead#0]#n:       %-10d #L[#7%d#L] #L[#7%d#L] \n\r", 
ch->rawmaterial[M_LEAD], 
LB_OTHER, T_LEAD);
send_to_char(buf, ch);

sprintf(buf, "#0[#7Magnesium#0]#n:  %-10d #L[#7%d#L] #L[#7%d#L] \n\r", ch->rawmaterial[M_MAGNESIUM], 
LB_OTHER, T_MAGNESIUM);
send_to_char(buf, ch);

sprintf(buf, "#0[#RMercury#0]#n:    %-10d #L[#7%d#L] #L[#7%d#L] \n\r", ch->rawmaterial[M_MERCURY], 
LB_OTHER, T_MERCURY);
send_to_char(buf, ch);

sprintf(buf, "#0[#0Nickel#0]#n:     %-10d #L[#7%d#L] #L[#7%d#L] \n\r", ch->rawmaterial[M_NICKEL], 
LB_OTHER, T_NICKEL);
send_to_char(buf, ch);

sprintf(buf, "#0[#PPotassium#0]#n:  %-10d #L[#7%d#L] #L[#7%d#L] \n\r", ch->rawmaterial[M_POTASSIUM], 
LB_OTHER, T_POTASSIUM);
send_to_char(buf, ch);

sprintf(buf, "#0[#oUranium#0]#n:    %-10d #L[#7%d#L] #L[#7%d#L] \n\r", ch->rawmaterial[M_URANIUM], 
LB_OTHER, T_URANIUM);
send_to_char(buf, ch);

sprintf(buf, "#0[#cTitanium#0]#n:   %-10d #L[#7%d#L] #L[#7%d#L] \n\r", ch->rawmaterial[M_TITANIUM], 
LB_OTHER, T_TITANIUM);
send_to_char(buf, ch);

sprintf(buf, "#0[#PZinc#0]#n:       %-10d #L[#7%d#L] #L[#7%d#L] \n\r", 
ch->rawmaterial[M_ZINC], 
LB_OTHER, T_ZINC);
send_to_char(buf, ch);

sprintf(buf, "#0[#PZirconium#0]#n:  %-10d #L[#7%d#L] #L[#7%d#L] \n\r", ch->rawmaterial[M_ZIRCONIUM], 
LB_OTHER, T_ZIRCONIUM);
send_to_char(buf, ch);
sprintf(buf, "#0[#oCarbon#0]#n:     %-10d #L[#7%d#L] #L[#7%d#L] \n\r", 
ch->rawmaterial[M_CARBON], 
LB_OTHER, T_CARBON);
send_to_char(buf, ch);
sprintf(buf, "#0[#PPalladium#0]#n:  %-10d #L[#7%d#L] #L[#7%d#L] \n\r", ch->rawmaterial[M_PALLADIUM], 
LB_OTHER, T_PALLADIUM);
send_to_char(buf, ch);
return;
}

void do_alloys(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  OBJ_DATA *obj = NULL;
  int x;

send_to_char( "#L[#7Material#L][#7Amount#L][#7Lb Needed#L][#7Toughness#L][#7AFF#L]\n\r",ch);
 sprintf(buf, "#0[#7Steel#0]#n:             %-10d #L[#7%d#L] #L[#7%d#L] #7+400 Hp\n\r", 
	ch->rawmaterial[M_STEEL], LB_METAL, T_STEEL);
send_to_char(buf, ch);

sprintf(buf, "#0[#7Brass#0]#n:              %-10d #L[#7%d#L] #L[#7%d#L] #7+400 Hp\n\r", 
	ch->rawmaterial[M_BRASS], LB_METAL, T_BRASS);
send_to_char(buf, ch);
sprintf(buf, "#0[#7Bronze#0]#n:             %-10d #L[#7%d#L] #L[#7%d#L] #7+400 Hp\n\r", 
	ch->rawmaterial[M_BRONZE], LB_METAL, T_BRONZE);
send_to_char(buf, ch);

sprintf(buf, "#0[#7Duralumin#0]#n:          %-10d #L[#7%d#L] #L[#7%d#L] \n\r", 
	ch->alloy[A_DURALUMIN], LB_METAL, T_DURALUMIN);
send_to_char(buf, ch);
sprintf(buf, "#0[#7Rose Metal#0]#n:         %-10d #L[#7%d#L] #L[#7%d#L] #7+400 Mana\n\r", 
	ch->alloy[A_ROSE_METAL], LB_METAL, T_ROSE_METAL);
send_to_char(buf, ch);
sprintf(buf, "#0[#7Galfenol#0]#n:           %-10d #L[#7%d#L] #L[#7%d#L] #7+400 Mana\n\r", 
	ch->alloy[A_GALFENOL], LB_METAL, T_GALFENOL);
send_to_char(buf, ch);
sprintf(buf, "#0[#7Galinstan#0]#n:          %-10d #L[#7%d#L] #L[#7%d#L] #7+400 Mana\n\r", 
	ch->alloy[A_GALINSTAN], LB_METAL, T_GALINSTAN);
send_to_char(buf, ch);
sprintf(buf, "#0[#7Electrum#0]#n:           %-10d #L[#7%d#L] #L[#7%d#L] #7+400 Mana\n\r", 
	ch->alloy[A_ELECTRUM], LB_METAL, T_ELECTRUM);
send_to_char(buf, ch);
sprintf(buf, "#0[#7Rose Gold#0]#n:          %-10d #L[#7%d#L] #L[#7%d#L] #7+400 Mana\n\r", 
	ch->alloy[A_ROSE_GOLD], LB_METAL, T_ROSE_GOLD);
send_to_char(buf, ch);
sprintf(buf, "#0[#7White Gold#0]#n:         %-10d #L[#7%d#L] #L[#7%d#L] #7+400 Mana\n\r", 
	ch->alloy[A_WHITE_GOLD], LB_METAL, T_WHITE_GOLD);
send_to_char(buf, ch);
sprintf(buf, "#0[#7Fields Metal#0]#n:       %-10d #L[#7%d#L] #L[#7%d#L] #7+400 Mana\n\r", 
	ch->alloy[A_FIELDS_METAL], LB_METAL, T_FIELDS_METAL);
send_to_char(buf, ch);
sprintf(buf, "#0[#7Kovar#0]#n:              %-10d #L[#7%d#L] #L[#7%d#L] #7+400 Mana\n\r", 
	ch->alloy[A_KOVAR], LB_METAL, T_KOVAR);
send_to_char(buf, ch);
sprintf(buf, "#0[#7Solder#0]#n:             %-10d #L[#7%d#L] #L[#7%d#L] #7+400 Mana\n\r", 
	ch->alloy[A_SOLDER], LB_METAL, T_SOLDER);
send_to_char(buf, ch);
sprintf(buf, "#0[#7Magnox#0]#n:             %-10d #L[#7%d#L] #L[#7%d#L] #7+400 Mana\n\r", 
	ch->alloy[A_MAGNOX], LB_METAL, T_MAGNOX);
send_to_char(buf, ch);
sprintf(buf, "#0[#7Silver Amalgam#0]#n:     %-10d #L[#7%d#L] #L[#7%d#L] #7+400 Mana\n\r", 
	ch->alloy[A_SILVER_AMALGAM], LB_METAL, T_SILVER_AMALGAM);
send_to_char(buf, ch);
sprintf(buf, "#0[#7Gold Amalgam#0]#n:       %-10d #L[#7%d#L] #L[#7%d#L] #7+400 Mana\n\r", 
	ch->alloy[A_GOLD_AMALGAM], LB_METAL, T_GOLD_AMALGAM);
send_to_char(buf, ch);
sprintf(buf, "#0[#7Alnico#0]#n:             %-10d #L[#7%d#L] #L[#7%d#L] #7+400 Mana\n\r", 
	ch->alloy[A_ALNICO], LB_METAL, T_ALNICO);
send_to_char(buf, ch);
sprintf(buf, "#0[#7Platinum Sterling#0]#n:  %-10d #L[#7%d#L] #L[#7%d#L] #7+400 Mana\n\r", 
	ch->alloy[A_PLATINUM_STERLING], LB_METAL, T_PLATINUM_STERLING);
send_to_char(buf, ch);
sprintf(buf, "#0[#7Sterling Silver#0]#n:    %-10d #L[#7%d#L] #L[#7%d#L] #7+400 Mana\n\r", 
	ch->alloy[A_STERLING_SILVER], LB_METAL, T_STERLING_SILVER);
send_to_char(buf, ch);
sprintf(buf, "#0[#7Pewter#0]#n:             %-10d #L[#7%d#L] #L[#7%d#L] #7+400 Mana\n\r", 
	ch->alloy[A_PEWTER], LB_METAL, T_PEWTER);
send_to_char(buf, ch);
sprintf(buf, "#0[#7Staballoy#0]#n:          %-10d #L[#7%d#L] #L[#7%d#L] #7+400 Mana\n\r", 
	ch->alloy[A_STABALLOY], LB_METAL, T_STABALLOY);
send_to_char(buf, ch);
sprintf(buf, "#0[#7Zircaloy#0]#n:           %-10d #L[#7%d#L] #L[#7%d#L] #7+400 Mana\n\r", 
	ch->alloy[A_ZIRCALOY], LB_METAL, A_ZIRCALOY);
send_to_char(buf, ch);

return;
}



void do_destroy(CHAR_DATA * ch, char *argument)
{
  char arg[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  OBJ_DATA *obj;

  int amount, material, x;
 
  one_argument(argument, arg);

  if (arg[0] == '\0')
  {
    send_to_char("What object do you wish to deconstruct?\n\r", ch);
    return;
  }
  if ((obj = get_obj_carry( ch, arg) ) == NULL)
  {
    send_to_char( "You are not carrying that item.\n\r", ch );
    return;
  }

  amount = obj->weight;
  material = obj->material;

if (material >= 1 && material <= 100) ch->rawmaterial[material] += amount;


if (IS_SET(obj->c_material, C_COPPER)) ch->rawmaterial[M_COPPER] += LB_METAL;
if (IS_SET(obj->c_material, C_GOLD)) ch->rawmaterial[M_GOLD] += LB_METAL;
if (IS_SET(obj->c_material, C_IVORY)) ch->rawmaterial[M_IVORY] += LB_OTHER;
if (IS_SET(obj->c_material, C_MITHRIL)) ch->rawmaterial[M_MITHRIL] += LB_METAL;
if (IS_SET(obj->c_material, C_WOOD)) ch->rawmaterial[M_WOOD] += LB_METAL;
if (IS_SET(obj->c_material, C_ORGANIC)) ch->rawmaterial[M_ORGANIC] += LB_OTHER;
if (IS_SET(obj->c_material, C_DIAMOND)) ch->rawmaterial[M_DIAMOND] += LB_GEM;
if (IS_SET(obj->c_material, C_PLATINUM)) ch->rawmaterial[M_PLATINUM] += LB_METAL;
if (IS_SET(obj->c_material, C_ADAMANTITE)) ch->rawmaterial[M_ADAMANTITE] += LB_METAL;
if (IS_SET(obj->c_material, C_TIN)) ch->rawmaterial[M_TIN] += LB_METAL;
if (IS_SET(obj->c_material, C_EBONY)) ch->rawmaterial[M_EBONY] += LB_OTHER;
if (IS_SET(obj->c_material, C_RUBY)) ch->rawmaterial[M_RUBY] += LB_GEM;
if (IS_SET(obj->c_material, C_LIMESTONE)) ch->rawmaterial[M_LIMESTONE] += LB_OTHER;
if (IS_SET(obj->c_material, C_TOPAZ)) ch->rawmaterial[M_TOPAZ] += LB_GEM;
if (IS_SET(obj->c_material, C_AMETHYST)) ch->rawmaterial[M_AMETHYST] += LB_GEM;
if (IS_SET(obj->c_material, C_JADE)) ch->rawmaterial[M_JADE] += LB_GEM;
if (IS_SET(obj->c_material, C_ONYX)) ch->rawmaterial[M_ONYX] += LB_GEM;
if (IS_SET(obj->c_material, C_EMERALD)) ch->rawmaterial[M_EMERALD] += LB_GEM;
if (IS_SET(obj->c_material, C_PEARL)) ch->rawmaterial[M_PEARL] += LB_GEM;
if (IS_SET(obj->c_material, C_OPAL)) ch->rawmaterial[M_OPAL] += LB_GEM;
if (IS_SET(obj->c_material, C_SAPPHIRE)) ch->rawmaterial[M_SAPPHIRE] += LB_GEM;
if (IS_SET(obj->c_material, C_LEATHER)) ch->rawmaterial[M_LEATHER] += LB_OTHER;
if (IS_SET(obj->c_material, C_IRON)) ch->rawmaterial[M_IRON] += LB_METAL;
if (IS_SET(obj->c_material, C_QUARTZ)) ch->rawmaterial[M_QUARTZ] += LB_GEM;
if (IS_SET(obj->c_material, C_MARBLE)) ch->rawmaterial[M_MARBLE] += LB_OTHER;
if (IS_SET(obj->c_material, C_SILVER)) ch->rawmaterial[M_SILVER] += LB_METAL;
if (IS_SET(obj->c_material, C_WOOL)) ch->rawmaterial[M_WOOL] += LB_OTHER;
if (IS_SET(obj->c_material, C_CLOTH)) ch->rawmaterial[M_CLOTH] += LB_OTHER;

// old alloys
if (IS_SET(obj->c_material, C_STEEL)) ch->rawmaterial[M_STEEL] += LB_METAL;
if (IS_SET(obj->c_material, C_BRASS)) ch->rawmaterial[M_BRASS] += LB_METAL;
if (IS_SET(obj->c_material, C_BRONZE)) ch->rawmaterial[M_BRONZE] += LB_METAL;

// new alloys
if (IS_SET(obj->alloy, A_DURALUMIN)) ch->rawmaterial[A_DURALUMIN] += LB_OTHER;
if (IS_SET(obj->alloy, A_ROSE_METAL)) ch->rawmaterial[A_ROSE_METAL] += LB_OTHER;
if (IS_SET(obj->alloy, A_GALFENOL)) ch->rawmaterial[A_GALFENOL] += LB_OTHER;
if (IS_SET(obj->alloy, A_GALINSTAN)) ch->rawmaterial[A_GALINSTAN] += LB_OTHER;
if (IS_SET(obj->alloy, A_ELECTRUM)) ch->rawmaterial[A_ELECTRUM] += LB_OTHER;
if (IS_SET(obj->alloy, A_ROSE_GOLD)) ch->rawmaterial[A_ROSE_GOLD] += LB_OTHER;
if (IS_SET(obj->alloy, A_WHITE_GOLD)) ch->rawmaterial[A_WHITE_GOLD] += LB_OTHER;
if (IS_SET(obj->alloy, A_FIELDS_METAL)) ch->rawmaterial[A_FIELDS_METAL] += LB_OTHER;
if (IS_SET(obj->alloy, A_KOVAR)) ch->rawmaterial[A_KOVAR] += LB_OTHER;
if (IS_SET(obj->alloy, A_SOLDER)) ch->rawmaterial[A_SOLDER] += LB_OTHER;
if (IS_SET(obj->alloy, A_MAGNOX)) ch->rawmaterial[A_MAGNOX] += LB_OTHER;
if (IS_SET(obj->alloy, A_SILVER_AMALGAM)) ch->rawmaterial[A_SILVER_AMALGAM] += LB_OTHER;
if (IS_SET(obj->alloy, A_GOLD_AMALGAM)) ch->rawmaterial[A_GOLD_AMALGAM] += LB_OTHER;
if (IS_SET(obj->alloy, A_ALNICO)) ch->rawmaterial[A_ALNICO] += LB_OTHER;
if (IS_SET(obj->alloy, A_PLATINUM_STERLING)) ch->rawmaterial[A_PLATINUM_STERLING] += LB_OTHER;
if (IS_SET(obj->alloy, A_STERLING_SILVER)) ch->rawmaterial[A_STERLING_SILVER] += LB_OTHER;
if (IS_SET(obj->alloy, A_PEWTER)) ch->rawmaterial[A_PEWTER] += LB_OTHER;
if (IS_SET(obj->alloy, A_STABALLOY)) ch->rawmaterial[A_STABALLOY] += LB_OTHER;
if (IS_SET(obj->alloy, A_ZIRCALOY)) ch->rawmaterial[A_ZIRCALOY] += LB_OTHER;




  extract_obj( obj );

  send_to_char("#0You #7harvest#0 the #7object#0 for raw materials.#n\n\r", ch);
  return;
}

void do_mix(CHAR_DATA * ch, char *argument)
{
  OBJ_DATA *obj;
  OBJ_INDEX_DATA *pObjIndex;
  char name[MSL];
  char buf[MSL];
  int hitroll = 0;
  int damroll = 0;
  int hp      = 0;
  int mana    = 0;
  int move    = 0;
  int wear    = 0;
  int armor   = 0;
  int rnum    = 53;

  char arg1[MAX_STRING_LENGTH];

  argument = one_argument( argument, arg1 );

  if (arg1[0] == '\0')
  {
    send_to_char("What Alloy do want to create?\n\r", ch);
    send_to_char("Duralumin: Aluminium, Copper\n\r", ch);
    send_to_char("Rose Metal: Bismuth, Lead, Tin\n\r", ch);
    send_to_char("Galfenol: Iron, Gallium\n\r", ch);
    send_to_char("Brass: Zinc Copper\n\r", ch);
    send_to_char("Bronze: Tin, Aluminium, Copper\n\r", ch);
    send_to_char("Galistan: Indium, Tin, Gallium\n\r", ch);
    send_to_char("Electrum: Gold, Silver, Copper\n\r", ch);
    send_to_char("Rose Gold: Copper Gold\n\r", ch);
    send_to_char("White Gold: Nickel, Gold, Palladium\n\r", ch);
    send_to_char("Fields Metal: Indium, Bismuth, Tin\n\r", ch);
    send_to_char("Steel: Iron, Carbon\n\r", ch);
    send_to_char("Kovar: Nickel, Cobalt, Iron\n\r", ch);
    send_to_char("Solder: Lead, Tin\n\r", ch);
    send_to_char("Magnox: Aluminium, Magnesium\n\r", ch);
    send_to_char("Silver: Amalgam: Mercury, Silver\n\r", ch);
    send_to_char("Gold: Amalgam: Mercury, Gold\n\r", ch);
    send_to_char("Alnico: Aluminium, Cobalt, Nickel\n\r", ch);
    send_to_char("Platinum Sterling: Silver, Platinum\n\r", ch);
    send_to_char("Sterling Silver: Copper, Silver\n\r", ch);
    send_to_char("Pewter: Lead, Tin, Copper\n\r", ch);
    send_to_char("Staballoy: Uranium, Titanium\n\r", ch);
    send_to_char("Zircalloy: Zirconium, Tin\n\r", ch);
    return;
}

if (!str_prefix(arg1, "zircalloy"))// Zircalloy: Zirconium, Tin
{
if (ch->rawmaterial[M_ZIRCONIUM] < LB_OTHER)
{
sprintf( buf, "#0You dont have enough Zirconium.#n");
send_to_char(buf, ch);
return;
}
if (ch->rawmaterial[M_TIN] < LB_METAL) 
{
sprintf( buf, "#0You dont have enough Tin.#n");
send_to_char(buf, ch);
return;
}
ch->alloy[A_ZIRCALOY] += 8;
sprintf( buf, "#0You mix Zirconium and Tin to make Zircalloy.#n");
send_to_char(buf, ch);
return;
}

if (!str_prefix(arg1, "staballoy"))// Staballoy: Uranium, Titanium
{
if (ch->rawmaterial[M_URANIUM] < LB_OTHER)
{
sprintf( buf, "#0You dont have enough Uranium.#n");
send_to_char(buf, ch);
return;
}
if (ch->rawmaterial[M_TITANIUM] < LB_METAL) 
{
sprintf( buf, "#0You dont have enough Titanium.#n");
send_to_char(buf, ch);
return;
}
ch->alloy[A_STABALLOY] += 8;
sprintf( buf, "#0You mix Uranium and Titanium to make Staballoy.#n");
send_to_char(buf, ch);
return;
}


if (!str_prefix(arg1, "pewter"))// Pewter: Lead, Tin, Copper
{
if (ch->rawmaterial[M_LEAD] < LB_METAL)
{
sprintf( buf, "#0You dont have enough Lead.#n");
send_to_char(buf, ch);
return;
}
if (ch->rawmaterial[M_TIN] < LB_METAL) 
{
sprintf( buf, "#0You dont have enough Tin.#n");
send_to_char(buf, ch);
return;
}
if (ch->rawmaterial[M_SILVER] < LB_METAL) 
{
sprintf( buf, "#0You dont have enough Silver.#n");
send_to_char(buf, ch);
return;
}
ch->alloy[A_STERLING_SILVER] += 15;
sprintf( buf, "#0You mix Sterling Silver.#n");
send_to_char(buf, ch);
return;
}


if (!str_prefix(arg1, "sterlingsilver"))// Sterling Silver: Copper, Silver
{
if (ch->rawmaterial[M_SILVER] < LB_METAL)
{
sprintf( buf, "#0You dont have enough Silver.#n");
send_to_char(buf, ch);
return;
}
if (ch->rawmaterial[M_COPPER] < LB_METAL) 
{
sprintf( buf, "#0You dont have enough Copper.#n");
send_to_char(buf, ch);
return;
}
ch->alloy[A_STERLING_SILVER] += 10;
sprintf( buf, "#0You mix silver and copper to make Sterling Silver.#n");
send_to_char(buf, ch);
return;
}


if (!str_prefix(arg1, "platinumsterling"))// Platinum Sterling: Silver, Platinum
{
if (ch->rawmaterial[M_SILVER] < LB_METAL)
{
sprintf( buf, "#0You dont have enough Silver.#n");
send_to_char(buf, ch);
return;
}
if (ch->rawmaterial[M_PLATINUM] < LB_METAL) 
{
sprintf( buf, "#0You dont have enough Platinum.#n");
send_to_char(buf, ch);
return;
}
ch->alloy[A_PLATINUM_STERLING] += 10;
sprintf( buf, "#0You mix platinum and Silver to make Platinum Sterling.#n");
send_to_char(buf, ch);
return;
}

if (!str_prefix(arg1, "Alnico"))// Alnico: Aluminium, Cobalt, Nickel
{
if (ch->rawmaterial[M_COBALT] < LB_METAL)
{
sprintf( buf, "#0You dont have enough Cobalt.#n");
send_to_char(buf, ch);
return;
}
if (ch->rawmaterial[M_ALUMINIUM] < LB_METAL) 
{
sprintf( buf, "#0You dont have enough Aluminium.#n");
send_to_char(buf, ch);
return;
}
if (ch->rawmaterial[M_NICKEL] < LB_METAL) 
{
sprintf( buf, "#0You dont have enough Nickel.#n");
send_to_char(buf, ch);
return;
}
ch->alloy[A_ALNICO] += 15;
sprintf( buf, "#0You mix Aluminium, Cobalt and Nickel to make Alnico.#n");
send_to_char(buf, ch);
return;
}


if (!str_prefix(arg1, "goldamalgam"))// Silver Amalgam
{
if (ch->rawmaterial[M_GOLD] < LB_METAL)
{
sprintf( buf, "#0You dont have enough Gold.#n");
send_to_char(buf, ch);
return;
}
if (ch->rawmaterial[M_MERCURY] < LB_OTHER) 
{
sprintf( buf, "#0You dont have enough Mercury.#n");
send_to_char(buf, ch);
return;
}
ch->alloy[A_GOLD_AMALGAM] += 8;
sprintf( buf, "#0You mix Gold and Mercury to make Gold Amalgam#n");
send_to_char(buf, ch);
return;
}


if (!str_prefix(arg1, "silveramalgam"))// Silver Amalgam
{
if (ch->rawmaterial[M_SILVER] < LB_METAL)
{
sprintf( buf, "#0You dont have enough Silver.#n");
send_to_char(buf, ch);
return;
}
if (ch->rawmaterial[M_MERCURY] < LB_OTHER) 
{
sprintf( buf, "#0You dont have enough Mercury.#n");
send_to_char(buf, ch);
return;
}
ch->alloy[A_SILVER_AMALGAM] += 8;
sprintf( buf, "#0You mix Silver and Mercury to make Silver Amalgam#n");
send_to_char(buf, ch);
return;
}


if (!str_prefix(arg1, "magnox"))// Aluminium Magnesium
{
if (ch->rawmaterial[M_ALUMINIUM] < LB_METAL)
{
sprintf( buf, "#0You dont have enough Aluminium.#n");
send_to_char(buf, ch);
return;
}
if (ch->rawmaterial[M_MAGNESIUM] < LB_OTHER) 
{
sprintf( buf, "#0You dont have enough Magnesium.#n");
send_to_char(buf, ch);
return;
}

ch->alloy[A_MAGNOX] += 8;
sprintf( buf, "#0You mix Aluminium and Magnesium to make Magnox.#n");
send_to_char(buf, ch);
return;
}


if (!str_prefix(arg1, "solder"))// Solder: Lead, Tin
{
if (ch->rawmaterial[M_LEAD] < LB_OTHER)
{
sprintf( buf, "#0You dont have enough Lead.#n");
send_to_char(buf, ch);
return;
}
if (ch->rawmaterial[M_TIN] < LB_METAL) 
{
sprintf( buf, "#0You dont have enough Tin.#n");
send_to_char(buf, ch);
return;
}
ch->alloy[A_SOLDER] += 8;
sprintf( buf, "#0You mix Lead and Tin to make Solder.#n");
send_to_char(buf, ch);
return;
}

if (!str_prefix(arg1, "kovar"))// Kovar: Nickel, Cobalt, Iron
{
if (ch->rawmaterial[M_NICKEL] < LB_METAL)
{
sprintf( buf, "#0You dont have enough Nickel.#n");
send_to_char(buf, ch);
return;
}
if (ch->rawmaterial[M_COBALT] < LB_METAL) 
{
sprintf( buf, "#0You dont have enough Cobalt.#n");
send_to_char(buf, ch);
return;
}
if (ch->rawmaterial[M_IRON] < LB_METAL) 
{
sprintf( buf, "#0You dont have enough Iron.#n");
send_to_char(buf, ch);
return;
}
ch->alloy[A_KOVAR] += 15;
sprintf( buf, "#0You mix Nickel, Cobalt and Iron to make Kovar.#n");
send_to_char(buf, ch);
return;
}

if (!str_prefix(arg1, "steel"))// Steel: Iron, Carbon
{
if (ch->rawmaterial[M_IRON] < LB_METAL)
{
sprintf( buf, "#0You dont have enough Iron.#n");
send_to_char(buf, ch);
return;
}
if (ch->rawmaterial[M_CARBON] < LB_OTHER) 
{
sprintf( buf, "#0You dont have enough Carbon.#n");
send_to_char(buf, ch);
return;
}
ch->rawmaterial[M_STEEL] += 8;
sprintf( buf, "#0You mix Iron and Carbon to make Steel.#n");
send_to_char(buf, ch);
return;
}


if (!str_prefix(arg1, "fieldsmetal"))// Fields Metal: Indium, Bismuth, Tin
{
if (ch->rawmaterial[M_TIN] < LB_METAL)
{
sprintf( buf, "#0You dont have enough Tin.#n");
send_to_char(buf, ch);
return;
}
if (ch->rawmaterial[M_BISMUTH] < LB_OTHER) 
{
sprintf( buf, "#0You dont have enough Bismuth.#n");
send_to_char(buf, ch);
return;
}
if (ch->rawmaterial[M_INDIUM] < LB_OTHER) 
{
sprintf( buf, "#0You dont have enough Indium.#n");
send_to_char(buf, ch);
return;
}
ch->alloy[A_FIELDS_METAL] += 11;
sprintf( buf, "#0You mix Tin, Bismuth and Indium to make Fields Metal.#n");
send_to_char(buf, ch);
return;
}


if (!str_prefix(arg1, "whitegold"))// White Gold: Nickel, Gold, Palladium
{
if (ch->rawmaterial[M_GOLD] < LB_METAL)
{
sprintf( buf, "#0You dont have enough Gold.#n");
send_to_char(buf, ch);
return;
}
if (ch->rawmaterial[M_PALLADIUM] < LB_OTHER) 
{
sprintf( buf, "#0You dont have enough Palladium.#n");
send_to_char(buf, ch);
return;
}
if (ch->rawmaterial[M_NICKEL] < LB_METAL) 
{
sprintf( buf, "#0You dont have enough Nickel.#n");
send_to_char(buf, ch);
return;
}
ch->alloy[A_WHITE_GOLD] += 13;
sprintf( buf, "#0You mix Gold, Nickel and Palladium to make White Gold.#n");
send_to_char(buf, ch);
return;
}

if (!str_prefix(arg1, "rosegold"))// Rose Gold: Copper Gold
{
if (ch->rawmaterial[M_GOLD] < LB_METAL)
{
sprintf( buf, "#0You dont have enough Gold.#n");
send_to_char(buf, ch);
return;
}
if (ch->rawmaterial[M_COPPER] < LB_METAL) 
{
sprintf( buf, "#0You dont have enough Copper.#n");
send_to_char(buf, ch);
return;
}
if (ch->rawmaterial[M_GALLIUM] < LB_OTHER) 
{
sprintf( buf, "#0You dont have enough Gallium.#n");
send_to_char(buf, ch);
return;
}
ch->alloy[A_ROSE_GOLD] += 13;
sprintf( buf, "#0You mix Gold and Copper to make Rose Gold.#n");
send_to_char(buf, ch);
return;
}


if (!str_prefix(arg1, "Electrum"))// Electrum: Gold, Silver, Copper
{
	if (ch->rawmaterial[M_GOLD] < LB_METAL)
	{
	sprintf( buf, "#0You dont have enough Gold.#n");
	send_to_char(buf, ch);
	return;
	}
	if (ch->rawmaterial[M_SILVER] < LB_METAL) 
	{
	sprintf( buf, "#0You dont have enough Silver.#n");
	send_to_char(buf, ch);
	return;
	}
	if (ch->rawmaterial[M_COPPER] < LB_METAL) 
	{
	sprintf( buf, "#0You dont have enough Copper.#n");
	send_to_char(buf, ch);
	return;
	}
	ch->alloy[A_ELECTRUM] += 15;
	sprintf( buf, "#0You mix Gold, Silver, and Copper to make Electrum.#n");
	send_to_char(buf, ch);
	return;
}


if (!str_prefix(arg1, "Galistan"))// Galistan: Indium, Tin, Gallium
{
if (ch->rawmaterial[M_TIN] < LB_METAL)
{
sprintf( buf, "#0You dont have enough Tin.#n");
send_to_char(buf, ch);
return;
}
if (ch->rawmaterial[M_INDIUM] < LB_OTHER) 
{
sprintf( buf, "#0You dont have enough Indium.#n");
send_to_char(buf, ch);
return;
}
if (ch->rawmaterial[M_GALLIUM] < LB_OTHER) 
{
sprintf( buf, "#0You dont have enough Gallium.#n");
send_to_char(buf, ch);
return;
}
ch->alloy[A_GALINSTAN] += 11;
sprintf( buf, "#0You mix Tin, Indium, Gallium to make Galistan.#n");
send_to_char(buf, ch);
return;
}


 
if (!str_prefix(arg1, "Bronze"))// Bronze: Tin, Aluminium, Copper
{
if (ch->rawmaterial[M_TIN] < LB_METAL)
{
sprintf( buf, "#0You dont have enough Tin.#n");
send_to_char(buf, ch);
return;
}
if (ch->rawmaterial[M_ALUMINIUM] < LB_METAL) 
{
sprintf( buf, "#0You dont have enough aluminium.#n");
send_to_char(buf, ch);
return;
}
if (ch->rawmaterial[M_COPPER] < LB_METAL) 
{
sprintf( buf, "#0You dont have enough aluminium.#n");
send_to_char(buf, ch);
return;
}
ch->rawmaterial[C_BRONZE] += 15;
sprintf( buf, "#0You mix Tin, Aluminium and Copper to make Bronze.#n");
send_to_char(buf, ch);
return;
}

if (!str_prefix(arg1, "Brass"))// Brass: Zinc Copper
{
if (ch->rawmaterial[M_ZINC] < LB_OTHER)
{
sprintf( buf, "#0You dont have enough copper.#n");
send_to_char(buf, ch);
return;
}
if (ch->rawmaterial[M_COPPER] < LB_METAL) 
{
sprintf( buf, "#0You dont have enough aluminium.#n");
send_to_char(buf, ch);
return;
}
ch->rawmaterial[C_BRASS] += (LB_OTHER + LB_METAL);
sprintf( buf, "#0You mix Zinc and Copper to make Brass.#n");
send_to_char(buf, ch);
return;
}


if (!str_prefix(arg1, "Duralumin"))// Duralumin: Aluminium, Copper
{
if (ch->rawmaterial[M_COPPER] < LB_METAL)
{
sprintf( buf, "#0You dont have enough copper.#n");
send_to_char(buf, ch);
return;
}
if (ch->rawmaterial[M_ALUMINIUM] < LB_METAL) 
{
sprintf( buf, "#0You dont have enough aluminium.#n");
send_to_char(buf, ch);
return;
}
ch->alloy[A_DURALUMIN] += 10;
sprintf( buf, "#0You mix Copper and Aluminium to make Duralumin.#n");
send_to_char(buf, ch);
return;
}

if (!str_prefix(arg1, "rosemetal")) // Rose Metal: Bismuth, Lead, Tin
{
if (ch->rawmaterial[M_TIN] < 5)
{
sprintf( buf, "#0You dont have enough Tin.#n");
send_to_char(buf, ch);
return;
}
if (ch->rawmaterial[M_LEAD] < 5)
{
sprintf( buf, "#0You dont have enough Lead.#n");
send_to_char(buf, ch);
return;
}
if (ch->rawmaterial[M_BISMUTH] < 5)
{
sprintf( buf, "#0You dont have enough Bismuth.#n");
send_to_char(buf, ch);
return;
}
ch->alloy[A_ROSE_METAL] += 15;
sprintf( buf, "#0You mix Tin, Lead and Bismuth to make Rose Metal.#n");
send_to_char(buf, ch);
return;
}
if (!str_prefix(arg1, "galfenol")) // Galfenol: Iron, Gallium
{
if (ch->rawmaterial[M_IRON] < LB_METAL)
{
sprintf( buf, "#0You dont have enough iron.#n");
send_to_char(buf, ch);
return;
}
if (ch->rawmaterial[M_GALLIUM] < LB_METAL) 
{
sprintf( buf, "#0You dont have enough Gallium.#n");
send_to_char(buf, ch);
return;
}
ch->alloy[A_GALFENOL] += 10;
sprintf( buf, "#0You mix Iron and Gallium to make Galfenol.#n");
send_to_char(buf, ch);
return;
}


/*
if (obj->alloy A_DURALUMIN)) ch->rawmaterial[A_DURALUMIN] += LB_OTHER;
if (IS_SET(obj->alloy, A_ROSE_METAL)) ch->rawmaterial[A_ROSE_METAL] += LB_OTHER;
if (IS_SET(obj->alloy, A_GALFENOL)) ch->rawmaterial[A_GALFENOL] += LB_OTHER;
if (IS_SET(obj->alloy, A_GALINSTAN)) ch->rawmaterial[A_GALINSTAN] += LB_OTHER;
if (IS_SET(obj->alloy, A_ELECTRUM)) ch->rawmaterial[A_ELECTRUM] += LB_OTHER;
if (IS_SET(obj->alloy, A_ROSE_GOLD)) ch->rawmaterial[A_ROSE_GOLD] += LB_OTHER;
if (IS_SET(obj->alloy, A_WHITE_GOLD)) ch->rawmaterial[A_WHITE_GOLD] += LB_OTHER;
if (IS_SET(obj->alloy, A_FIELDS_METAL)) ch->rawmaterial[A_FIELDS_METAL] += LB_OTHER;
if (IS_SET(obj->alloy, A_KOVAR)) ch->rawmaterial[A_KOVAR] += LB_OTHER;
if (IS_SET(obj->alloy, A_SOLDER)) ch->rawmaterial[A_SOLDER] += LB_OTHER;
if (IS_SET(obj->alloy, A_MAGNOX)) ch->rawmaterial[A_MAGNOX] += LB_OTHER;
if (IS_SET(obj->alloy, A_SILVER_AMALGAM)) ch->rawmaterial[A_SILVER_AMALGAM] += LB_OTHER;
if (IS_SET(obj->alloy, A_GOLD_AMALGAM)) ch->rawmaterial[A_GOLD_AMALGAM] += LB_OTHER;
if (IS_SET(obj->alloy, A_ALNICO)) ch->rawmaterial[A_ALNICO] += LB_OTHER;
if (IS_SET(obj->alloy, A_PLATINUM_STERLING)) ch->rawmaterial[A_PLATINUM_STERLING] += LB_OTHER;
if (IS_SET(obj->alloy, A_STERLING_SILVER)) ch->rawmaterial[A_STERLING_SILVER] += LB_OTHER;
if (IS_SET(obj->alloy, A_PEWTER)) ch->rawmaterial[A_PEWTER] += LB_OTHER;
if (IS_SET(obj->alloy, A_STABALLOY)) ch->rawmaterial[A_STABALLOY] += LB_OTHER;
if (IS_SET(obj->alloy, A_ZIRCALOY)) ch->rawmaterial[A_ZIRCALOY] += LB_OTHER;
*/
return;
}

void do_smith( CHAR_DATA *ch, char *argument )
{
  OBJ_DATA *obj;
  OBJ_INDEX_DATA *pObjIndex;
  char name[MSL];
  char buf[MSL];
  int hitroll = 0;
  int damroll = 0;
  int hp      = 0;
  int mana    = 0;
  int move    = 0;
  int wear    = 0;
  int armor   = 0;
  int rnum    = 53;

  char arg1[MAX_STRING_LENGTH];
  char arg2[MAX_STRING_LENGTH];
  char arg3[MAX_STRING_LENGTH];

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  argument = one_argument( argument, arg3 );

  if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0')
  {

    send_to_char( "Blade Material: steel, adamantite, mithril \n\r", ch );
    send_to_char( "Hilt Material: Iron, Gold, Bronze, Ebony Platinum, Silver \n\r", ch );
    send_to_char( "		    Ivory, Marble, Limestone, Quartz \n\r", ch );

// * ivory = curse       ebony  = blind   crystal(quartz)   = dispel evil    marble    = drain
// * gold  = lightning   bronze = acid    sandstone(Iron) = fire    limestone = poison
    send_to_char( "Wpn Type :see prof  \n\r", ch );
//    send_to_char( "       \n\r", ch );
    send_to_char( "Syntax: smith <wpntype> <hilt> <blade>\n\r", ch );
    return;
  }
        if ( rnum == 0 || ( pObjIndex = get_obj_index( rnum ) ) == NULL )
        {
        stc( "Missing object, please notify a member of the staff.\n\r", ch );
        return;
        }


         obj = create_object( pObjIndex, 53);
         obj->item_type = ITEM_WEAPON;
  

	
  if (!str_prefix(arg1, "dagger")){ obj->value[3] = 0; sprintf( name, "Dagger");}
  if (!str_prefix(arg1, "knife")){  obj->value[3] = 1; sprintf( name, "Knife");}
  if (!str_prefix(arg1, "spear")){  obj->value[3] = 2; sprintf( name, "Spear");}
  if (!str_prefix(arg1, "shortsword")){  obj->value[3] = 3; sprintf( name, "Shortsword");}
  if (!str_prefix(arg1, "staff")){  obj->value[3] = 4; sprintf( name, "Staff");}
  if (!str_prefix(arg1, "broadsword")){  obj->value[3] = 5; sprintf( name, "Broadsword");}
  if (!str_prefix(arg1, "longsword")){  obj->value[3] = 6; sprintf( name, "Longsword");}
  if (!str_prefix(arg1, "bastardsword")){  obj->value[3] = 7; sprintf( name, "Bastard Sword");}
  if (!str_prefix(arg1, "two-handed longsword")){  obj->value[3] = 8; sprintf( name, "Two-Handed Longsword");}
  if (!str_prefix(arg1, "axe")){  obj->value[3] = 9; sprintf( name, "Axe");}
  if (!str_prefix(arg1, "mace")){  obj->value[3] = 10; sprintf( name, "Mace");}
  if (!str_prefix(arg1, "whip")){  obj->value[3] = 11; sprintf( name, "Whip");}
  if (!str_prefix(arg1, "warhammer")){  obj->value[3] = 12; sprintf( name, "Warhammer");}
  if (!str_prefix(arg1, "flail")){  obj->value[3] = 13; sprintf( name, "Flail");}
  if (!str_prefix(arg1, "morningstar")){  obj->value[3] = 14;sprintf( name, "Morningstar");}
  if (!str_prefix(arg1, "polearm")){  obj->value[3] = 15; sprintf( name, "Polearm");}
  if (!str_prefix(arg1, "club")){  obj->value[3] = 16; sprintf( name, "Club");}
  if (!str_prefix(arg1, "sickle")){  obj->value[3] = 17; sprintf( name, "Sickle");}

  if (!str_prefix(arg2, "copper"))
  {

	if (ch->rawmaterial[M_COPPER] < 5) 
      {
      stc( "#0You do not have enough copper.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_COPPER);
      mana    = 400;
      add_affect( obj, APPLY_MANA, mana );
  }
  if (!str_prefix(arg2, "gold"))
  {

	if (ch->rawmaterial[M_GOLD] < 5) 
      {
      stc( "#0You do not have enough gold.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_GOLD);
    mana    = 400;
    add_affect( obj, APPLY_MANA, mana );  }
  if (!str_prefix(arg2, "ivory"))
  {

	if (ch->rawmaterial[M_IVORY] < 2) 
      {
      stc( "#0You do not have enough Ivory.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_IVORY);
    damroll = 125;
    add_affect( obj, APPLY_DAMROLL, damroll );
  }
  if (!str_prefix(arg2, "steel"))
  {

	if (ch->rawmaterial[M_STEEL] < 5) 
      {
      stc( "#0You do not have enough steel.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_STEEL);
    hp      = 400;
    add_affect( obj, APPLY_HIT, hp );
  }
  if (!str_prefix(arg2, "mithril"))
  {

	if (ch->rawmaterial[M_MITHRIL] < 5) 
      {
      stc( "#0You do not have enough mithril.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_MITHRIL);
    hp      = 400;
    add_affect( obj, APPLY_HIT, hp );

  }
  if (!str_prefix(arg2, "wood"))
  {

	if (ch->rawmaterial[M_WOOD] < 5) 
      {
      stc( "#0You do not have enough Wood.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_WOOD);
    hp      = 400;
    add_affect( obj, APPLY_HIT, hp );
  }

  if (!str_prefix(arg2, "platinum"))
  {

	if (ch->rawmaterial[M_PLATINUM] < 2) 
      {
      stc( "#0You do not have enough platinum.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_PLATINUM);
    hitroll = 125;
    add_affect( obj, APPLY_HITROLL, hitroll );
  }
  if (!str_prefix(arg2, "adamantite"))
  {

	if (ch->rawmaterial[M_ADAMANTITE] < 5) 
      {
      stc( "#0You do not have enough adamantite.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_ADAMANTITE);
    hp      = 400;
    add_affect( obj, APPLY_HIT, hp );
  }
  if (!str_prefix(arg2, "brass"))
  {

	if (ch->rawmaterial[M_BRASS] < 5) 
      {
      stc( "#0You do not have enough Brass.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_BRASS);
    hp      = 400;
    add_affect( obj, APPLY_HIT, hp );
  }
  if (!str_prefix(arg2, "bronze"))
  {

	if (ch->rawmaterial[M_BRONZE] < 5) 
      {
      stc( "#0You do not have enough bronze.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_BRONZE);
    hp      = 400;
    add_affect( obj, APPLY_HIT, hp );
  }

  if (!str_prefix(arg2, "ebony"))
  {

	if (ch->rawmaterial[M_EBONY] < 2) 
      {
      stc( "#0You do not have enough ebony.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_EBONY);

    armor   = -100;
    add_affect( obj, APPLY_AC, armor );
  }

  if (!str_prefix(arg2, "limestone"))
  {

	if (ch->rawmaterial[M_LIMESTONE] < 2) 
      {
      stc( "#0You do not have enough Limestone.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_LIMESTONE);
    move    = 400;
    add_affect( obj, APPLY_MOVE, move );
  }
  if (!str_prefix(arg2, "iron"))
  {

	if (ch->rawmaterial[M_IRON] < 5) 
      {
      stc( "#0You do not have enough iron.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_IRON);
    armor   = -100;
    add_affect( obj, APPLY_AC, armor );
  }
  if (!str_prefix(arg2, "quartz"))
  {

	if (ch->rawmaterial[M_QUARTZ] < 2) 
      {
      stc( "#0You do not have enough Quartz.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_QUARTZ);
    armor   = -100;
    add_affect( obj, APPLY_AC, armor );
  }
  if (!str_prefix(arg2, "marble"))
  {

	if (ch->rawmaterial[M_MARBLE] < 2) 
      {
      stc( "#0You do not have enough Marble.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_MARBLE);
    hp      = 400;
    add_affect( obj, APPLY_HIT, hp );
  }
  if (!str_prefix(arg2, "silver"))
  {

	if (ch->rawmaterial[M_SILVER] < 5) 
      {
      stc( "#0You do not have enough Silver.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_SILVER);
    armor   = -100;
    add_affect( obj, APPLY_AC, armor );

  }

// ------------------ ARG3 -------------------
  if (!str_prefix(arg3, "copper"))
  {
	if (ch->rawmaterial[M_COPPER] < 5) 
      {
      stc( "#0You do not have enough copper.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_COPPER);
      mana    = 400;
      add_affect( obj, APPLY_MANA, mana );
  }
  if (!str_prefix(arg3, "gold"))
  {

	if (ch->rawmaterial[M_GOLD] < 5) 
      {
      stc( "#0You do not have enough gold.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_GOLD);
    mana    = 400;
    add_affect( obj, APPLY_MANA, mana );  }
  if (!str_prefix(arg3, "ivory"))
  {

	if (ch->rawmaterial[M_IVORY] < 2) 
      {
      stc( "#0You do not have enough Ivory.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_IVORY);
    damroll = 125;
    add_affect( obj, APPLY_DAMROLL, damroll );
  }
  if (!str_prefix(arg3, "steel"))
  {

	if (ch->rawmaterial[M_STEEL] < 5) 
      {
      stc( "#0You do not have enough steel.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_STEEL);
    hp      = 400;
    add_affect( obj, APPLY_HIT, hp );
  }
  if (!str_prefix(arg3, "mithril"))
  {

	if (ch->rawmaterial[M_MITHRIL] < 5) 
      {
      stc( "#0You do not have enough mithril.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_MITHRIL);
    hp      = 400;
    add_affect( obj, APPLY_HIT, hp );

  }
  if (!str_prefix(arg3, "wood"))
  {

	if (ch->rawmaterial[M_WOOD] < 5) 
      {
      stc( "#0You do not have enough Wood.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_WOOD);
    hp      = 400;
    add_affect( obj, APPLY_HIT, hp );
  }

  if (!str_prefix(arg3, "platinum"))
  {

	if (ch->rawmaterial[M_PLATINUM] < 2) 
      {
      stc( "#0You do not have enough platinum.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_PLATINUM);
    hitroll = 125;
    add_affect( obj, APPLY_HITROLL, hitroll );
  }
  if (!str_prefix(arg3, "adamantite"))
  {

	if (ch->rawmaterial[M_ADAMANTITE] < 5) 
      {
      stc( "#0You do not have enough adamantite.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_ADAMANTITE);
    hp      = 400;
    add_affect( obj, APPLY_HIT, hp );
  }
  if (!str_prefix(arg3, "brass"))
  {

	if (ch->rawmaterial[M_BRASS] < 5) 
      {
      stc( "#0You do not have enough Brass.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_BRASS);
    hp      = 400;
    add_affect( obj, APPLY_HIT, hp );
  }
  if (!str_prefix(arg3, "bronze"))
  {

	if (ch->rawmaterial[M_BRONZE] < 5) 
      {
      stc( "#0You do not have enough bronze.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_BRONZE);
    hp      = 400;
    add_affect( obj, APPLY_HIT, hp );
  }

  if (!str_prefix(arg3, "ebony"))
  {

	if (ch->rawmaterial[M_EBONY] < 2) 
      {
      stc( "#0You do not have enough ebony.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_EBONY);

    armor   = -100;
    add_affect( obj, APPLY_AC, armor );
  }

  if (!str_prefix(arg3, "limestone"))
  {

	if (ch->rawmaterial[M_LIMESTONE] < 2) 
      {
      stc( "#0You do not have enough Limestone.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_LIMESTONE);
    move    = 400;
    add_affect( obj, APPLY_MOVE, move );
  }
  if (!str_prefix(arg3, "iron"))
  {

	if (ch->rawmaterial[M_IRON] < 5) 
      {
      stc( "#0You do not have enough iron.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_IRON);
    armor   = -100;
    add_affect( obj, APPLY_AC, armor );
  }
  if (!str_prefix(arg3, "quartz"))
  {

	if (ch->rawmaterial[M_QUARTZ] < 2) 
      {
      stc( "#0You do not have enough Quartz.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_QUARTZ);
    armor   = -100;
    add_affect( obj, APPLY_AC, armor );
  }
  if (!str_prefix(arg3, "marble"))
  {

	if (ch->rawmaterial[M_MARBLE] < 2) 
      {
      stc( "#0You do not have enough Marble.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_MARBLE);
    hp      = 400;
    add_affect( obj, APPLY_HIT, hp );
  }
  if (!str_prefix(arg3, "silver"))
  {

	if (ch->rawmaterial[M_SILVER] < 5) 
      {
      stc( "#0You do not have enough Silver.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_SILVER);
    armor   = -100;
    add_affect( obj, APPLY_AC, armor );

  }

if (IS_SET(obj->c_material, C_COPPER)) 
{
ch->rawmaterial[M_COPPER] -= LB_METAL;
obj->toughness += T_COPPER;
obj->weight += LB_METAL;
}
if (IS_SET(obj->c_material, C_GOLD)) 
{
ch->rawmaterial[M_GOLD] -= LB_METAL;
obj->toughness += T_GOLD;
obj->weight += LB_METAL;
}
if (IS_SET(obj->c_material, C_IVORY))
{
ch->rawmaterial[M_IVORY] -= LB_OTHER;
obj->toughness += T_IVORY;
obj->weight += LB_OTHER;
}
if (IS_SET(obj->c_material, C_STEEL))
{
ch->rawmaterial[M_STEEL] -= LB_METAL;
obj->toughness += T_STEEL;
obj->weight += LB_METAL;
}
if (IS_SET(obj->c_material, C_MITHRIL))
{
ch->rawmaterial[M_MITHRIL] -= LB_METAL;
obj->toughness += T_MITHRIL;
obj->weight += LB_METAL;
}
if (IS_SET(obj->c_material, C_WOOD))
{
ch->rawmaterial[M_WOOD] -= LB_METAL;
obj->toughness += T_WOOD;
obj->weight += LB_METAL;
}
if (IS_SET(obj->c_material, C_PLATINUM))
{
ch->rawmaterial[M_PLATINUM] -= LB_METAL;
obj->toughness += T_PLATINUM;
obj->weight += LB_METAL;
}
if (IS_SET(obj->c_material, C_ADAMANTITE))
{
ch->rawmaterial[M_ADAMANTITE] -= LB_METAL;
obj->toughness += T_ADAMANTITE;
obj->weight += LB_METAL;
}
if (IS_SET(obj->c_material, C_BRASS))
{
ch->rawmaterial[M_BRASS] -= LB_METAL;
obj->toughness += T_BRASS;
obj->weight += LB_METAL;
}
if (IS_SET(obj->c_material, C_BRONZE))
{
ch->rawmaterial[M_BRONZE] -= LB_METAL;
obj->toughness += T_BRONZE;
obj->weight += LB_METAL;
}
if (IS_SET(obj->c_material, C_EBONY)) 
{
ch->rawmaterial[M_EBONY] -= LB_OTHER;
obj->toughness += T_EBONY;
obj->weight += LB_OTHER;
}
if (IS_SET(obj->c_material, C_LIMESTONE))
{
ch->rawmaterial[M_LIMESTONE] -= LB_OTHER;
obj->toughness += T_LIMESTONE;
obj->weight += LB_OTHER;
}
if (IS_SET(obj->c_material, C_IRON)) 
{
ch->rawmaterial[M_IRON] -= LB_METAL;
obj->toughness += T_IRON;
obj->weight += LB_METAL;
}
if (IS_SET(obj->c_material, C_QUARTZ)) 
{
ch->rawmaterial[M_QUARTZ] -= LB_OTHER;
obj->toughness += T_QUARTZ;
obj->weight += LB_OTHER;
}
if (IS_SET(obj->c_material, C_MARBLE))
{
ch->rawmaterial[M_MARBLE] -= LB_OTHER;
obj->toughness += T_MARBLE;
obj->weight += LB_OTHER;
}
if (IS_SET(obj->c_material, C_SILVER))
{
ch->rawmaterial[M_SILVER] -= LB_METAL;
obj->toughness += T_SILVER;
obj->weight += LB_METAL;
}


   if (obj->toughness > 100) obj->toughness = 100;



	  obj->value[0] = 14000;
        obj->value[1] = number_range (100,300);
        obj->value[2] = number_range (300,500);
	  obj->level = 55;
        sprintf( buf,"#7The %s#n", name );
        free_string( obj->short_descr );
        obj->short_descr = str_dup( buf );
        sprintf( buf ,"#7A %s lies on the ground.#n", name );
        free_string( obj->description );
        obj->description = str_dup( buf );
        sprintf( buf, "%s %s ", ch->name, name );
        free_string( obj->name );
        obj->name = str_dup( buf );
        obj_to_char( obj, ch ); obj->questowner = str_dup(ch->pcdata->switchname);
        act( "$p #7appears in your hands.#n", ch, obj, NULL, TO_CHAR );
        act( "$p #7appears in $n's hands.#n", ch, obj, NULL, TO_ROOM );

return;
}

void do_forge( CHAR_DATA *ch, char *argument )
{

  OBJ_DATA *obj;
  OBJ_INDEX_DATA *pObjIndex;
  char name[MSL];
  char buf[MSL];
  int hitroll = 0;
  int damroll = 0;
  int hp      = 0;
  int mana    = 0;
  int move    = 0;
  int wear    = 0;
  int armor   = 0;
  int rnum    = 52;


  char arg1[MAX_STRING_LENGTH];
  char arg2[MAX_STRING_LENGTH];
  char arg3[MAX_STRING_LENGTH];

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );
  argument = one_argument( argument, arg3 );


  if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0')
  {

    send_to_char( "material: copper, gold, iron, steel, adamantite, mithril \n\r", ch );
    send_to_char( "          wood, wool, cloth, platinum, organic, leather, silver \n\r", ch );
    send_to_char( "	     tin, brass, bronze \n\r", ch );
    send_to_char( "Gem: ruby, topaz, amethyst, jade, topaz, onyx, pearl\n\r", ch );
    send_to_char( "     sapphire, opal, emerald, diamond \n\r", ch );  
    send_to_char( "Other: marble, quartz, ebony, ivory  \n\r", ch );
    send_to_char( "item: bracer, greaves, collar, plate, ring, helmet, shield \n\r", ch );
    send_to_char( "      boots, sleeves, gauntlets, cloak, belt, mask \n\r", ch );
    send_to_char( "Syntax: forge <item> <mat1> <mat2>\n\r", ch );
    return;
  }

  if (str_prefix(arg1, "bracer") && str_prefix(arg1, "greaves")
      && str_prefix(arg1, "collar") && str_prefix(arg1, "plate")
      && str_prefix(arg1, "ring") && str_prefix(arg1, "helmet")
      && str_prefix(arg1, "boots") && str_prefix(arg1, "sleeves")
      && str_prefix(arg1, "gauntlets") && str_prefix(arg1, "cloak")
      && str_prefix(arg1, "belt") && str_prefix(arg1, "mask")
      && str_prefix(arg1, "shield"))
  {
  send_to_char( "That is not the name of a valid piece of Armor. \n\r", ch);
  return;
  } 

        if ( rnum == 0 || ( pObjIndex = get_obj_index( rnum ) ) == NULL )
        {
        stc( "Missing object, please notify a member of the staff.\n\r", ch );
        return;
        }

    obj     = create_object(pObjIndex, 52);

//    send_to_char( "item: bracer, greaves, collar, plate, ring, helmet \n\r", ch );


  if (!str_prefix(arg1, "bracer"))
  {
    wear = ITEM_WEAR_WRIST;
    sprintf( name, "Bracer" );
    SET_BIT( obj->wear_flags, wear );
  }
  if (!str_prefix(arg1, "greaves"))
  {
    wear = ITEM_WEAR_LEGS;
    sprintf( name, "Greaves" );
    SET_BIT( obj->wear_flags, wear );
  }
  if (!str_prefix(arg1, "collar"))
  {
    wear = ITEM_WEAR_NECK;
    sprintf( name, "Collar" );
    SET_BIT( obj->wear_flags, wear );
  }
  if (!str_prefix(arg1, "plate"))
  {
    wear = ITEM_WEAR_BODY;
    sprintf( name, "Plate" );
    SET_BIT( obj->wear_flags, wear );
  }
  if (!str_prefix(arg1, "ring"))
  {
    wear = ITEM_WEAR_FINGER;
    sprintf( name, "Ring" );
    SET_BIT( obj->wear_flags, wear );
  }
  if (!str_prefix(arg1, "helmet"))
  {
    wear = ITEM_WEAR_HEAD;
    sprintf( name, "Helmet" );
    SET_BIT( obj->wear_flags, wear );
  }

//    send_to_char( "      boots, sleeves, gauntlets, cloak, belt, mask \n\r", ch );

  if (!str_prefix(arg1, "boots"))
  {
    wear = ITEM_WEAR_FEET;
    sprintf( name, "Boots" );
    SET_BIT( obj->wear_flags, wear );
  }
  if (!str_prefix(arg1, "sleeves"))
  {
    wear = ITEM_WEAR_ARMS;
    sprintf( name, "Sleeves" );
    SET_BIT( obj->wear_flags, wear );
  }
  if (!str_prefix(arg1, "gauntlets"))
  {
    wear = ITEM_WEAR_HANDS;
    sprintf( name, "Gauntlets" );
    SET_BIT( obj->wear_flags, wear );
  }
  if (!str_prefix(arg1, "cloak"))
  {
    wear = ITEM_WEAR_ABOUT;
    sprintf( name, "Cloak" );
    SET_BIT( obj->wear_flags, wear );
  }
  if (!str_prefix(arg1, "belt"))
  {
    wear = ITEM_WEAR_WAIST;
    sprintf( name, "Belt" );
    SET_BIT( obj->wear_flags, wear );
  }
  if (!str_prefix(arg1, "mask"))
  {
    wear = ITEM_WEAR_FACE;
    sprintf( name, "Faceplate" );
    SET_BIT( obj->wear_flags, wear );
  }
  if (!str_prefix(arg1, "shield"))
  {
    wear = ITEM_WEAR_SHIELD;
    sprintf( name, "Shield" );
    SET_BIT( obj->wear_flags, wear );
  }


  if (!str_prefix(arg2, "copper"))
  {

	if (ch->rawmaterial[M_COPPER] < 5) 
      {
      stc( "#0You do not have enough copper.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_COPPER);
      mana    = 400;
      add_affect( obj, APPLY_MANA, mana );
  }
  if (!str_prefix(arg2, "gold"))
  {

	if (ch->rawmaterial[M_GOLD] < 5) 
      {
      stc( "#0You do not have enough gold.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_GOLD);
    mana    = 400;
    add_affect( obj, APPLY_MANA, mana );  }
  if (!str_prefix(arg2, "ivory"))
  {

	if (ch->rawmaterial[M_IVORY] < 2) 
      {
      stc( "#0You do not have enough Ivory.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_IVORY);
    damroll = 125;
    add_affect( obj, APPLY_DAMROLL, damroll );
  }
  if (!str_prefix(arg2, "steel"))
  {

	if (ch->rawmaterial[M_STEEL] < 5) 
      {
      stc( "#0You do not have enough steel.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_STEEL);
    hp      = 400;
    add_affect( obj, APPLY_HIT, hp );
  }
  if (!str_prefix(arg2, "mithril"))
  {

	if (ch->rawmaterial[M_MITHRIL] < 5) 
      {
      stc( "#0You do not have enough mithril.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_MITHRIL);
    hp      = 400;
    add_affect( obj, APPLY_HIT, hp );

  }
  if (!str_prefix(arg2, "wood"))
  {

	if (ch->rawmaterial[M_WOOD] < 5) 
      {
      stc( "#0You do not have enough Wood.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_WOOD);
    hp      = 400;
    add_affect( obj, APPLY_HIT, hp );
  }
  if (!str_prefix(arg2, "organic"))
  {

	if (ch->rawmaterial[M_ORGANIC] < 5) 
      {
      stc( "#0You do not have enough organic material.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_ORGANIC);
      obj->value[3] = 14; // Regen
  }
  if (!str_prefix(arg2, "diamond"))
  {

	if (ch->rawmaterial[M_DIAMOND] < 1) 
      {
      stc( "#0You do not have enough diamonds.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_DIAMOND);
      obj->value[3] = 9; // Sanc
  }
  if (!str_prefix(arg2, "platinum"))
  {

	if (ch->rawmaterial[M_PLATINUM] < 2) 
      {
      stc( "#0You do not have enough platinum.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_PLATINUM);
    hitroll = 125;
    add_affect( obj, APPLY_HITROLL, hitroll );
  }
  if (!str_prefix(arg2, "adamantite"))
  {

	if (ch->rawmaterial[M_ADAMANTITE] < 5) 
      {
      stc( "#0You do not have enough adamantite.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_ADAMANTITE);
    hp      = 400;
    add_affect( obj, APPLY_HIT, hp );
  }
  if (!str_prefix(arg2, "brass"))
  {

	if (ch->rawmaterial[M_BRASS] < 5) 
      {
      stc( "#0You do not have enough Brass.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_BRASS);
    hp      = 400;
    add_affect( obj, APPLY_HIT, hp );
  }
  if (!str_prefix(arg2, "bronze"))
  {

	if (ch->rawmaterial[M_BRONZE] < 5) 
      {
      stc( "#0You do not have enough bronze.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_BRONZE);
    hp      = 400;
    add_affect( obj, APPLY_HIT, hp );
  }
  if (!str_prefix(arg2, "tin"))
  {

	if (ch->rawmaterial[M_TIN] < 5) 
      {
      stc( "#0You do not have enough TIN.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_TIN);
    move    = 400;
    add_affect( obj, APPLY_MOVE, move );
  }
  if (!str_prefix(arg2, "ebony"))
  {

	if (ch->rawmaterial[M_EBONY] < 2) 
      {
      stc( "#0You do not have enough ebony.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_EBONY);

    armor   = -100;
    add_affect( obj, APPLY_AC, armor );
  }
  if (!str_prefix(arg2, "ruby"))
  {

	if (ch->rawmaterial[M_RUBY] < 1) 
      {
      stc( "#0You do not have a ruby.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_RUBY);
      obj->value[3] = 14; // Haste
  }
  if (!str_prefix(arg2, "limestone"))
  {

	if (ch->rawmaterial[M_LIMESTONE] < 2) 
      {
      stc( "#0You do not have enough Limestone.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_LIMESTONE);
    move    = 400;
    add_affect( obj, APPLY_MOVE, move );
  }
  if (!str_prefix(arg2, "topaz"))
  {

	if (ch->rawmaterial[M_TOPAZ] < 1) 
      {
      stc( "#0You do not have enough Topaz.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_TOPAZ);
      obj->value[3] = 10; // Detect hidden
  }
  if (!str_prefix(arg2, "amethyst"))
  {

	if (ch->rawmaterial[M_AMETHYST] < 1) 
      {
      stc( "#0You do not have an Amethyst.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_AMETHYST);
      obj->value[3] = 3; // Fly
  }
  if (!str_prefix(arg2, "jade"))
  {

	if (ch->rawmaterial[M_JADE] < 1) 
      {
      stc( "#0You do not have enough Jade.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_JADE);
      obj->value[3] = 4; // Infra
  }
  if (!str_prefix(arg2, "onyx"))
  {

	if (ch->rawmaterial[M_ONYX] < 1) 
      {
      stc( "#0You do not have enough Onyx.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_ONYX);
      obj->value[3] = 18; // Sneak
  }
  if (!str_prefix(arg2, "emerald"))
  {

	if (ch->rawmaterial[M_EMERALD] < 1) 
      {
      stc( "#0You do not have an Emerald.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_EMERALD);
      obj->value[3] = 15; // sup cut
  }
  if (!str_prefix(arg2, "pearl"))
  {

	if (ch->rawmaterial[M_PEARL] < 1) 
      {
      stc( "#0You do not have a Pearl.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_PEARL);
      obj->value[3] = 5; // invis
  }
  if (!str_prefix(arg2, "opal"))
  {
	if (ch->rawmaterial[M_OPAL] < 1) 
      {
      stc( "#0You do not have an opal.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_OPAL);
      obj->value[3] = 6; // pass door
  }
  if (!str_prefix(arg2, "sapphire"))
  {

	if (ch->rawmaterial[M_SAPPHIRE] < 1) 
      {
      stc( "#0You do not have a Sapphire.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_SAPPHIRE);
      obj->value[3] = 12; // chaos shield
  }
  if (!str_prefix(arg2, "leather"))
  {

	if (ch->rawmaterial[M_LEATHER] < 5) 
      {
      stc( "#0You do not have enough leather.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_LEATHER);
    armor   = -100;
    add_affect( obj, APPLY_AC, armor );
  }
  if (!str_prefix(arg2, "iron"))
  {

	if (ch->rawmaterial[M_IRON] < 5) 
      {
      stc( "#0You do not have enough iron.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_IRON);
    armor   = -100;
    add_affect( obj, APPLY_AC, armor );
  }
  if (!str_prefix(arg2, "quartz"))
  {

	if (ch->rawmaterial[M_QUARTZ] < 2) 
      {
      stc( "#0You do not have enough Quartz.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_QUARTZ);
    armor   = -100;
    add_affect( obj, APPLY_AC, armor );
  }
  if (!str_prefix(arg2, "marble"))
  {

	if (ch->rawmaterial[M_MARBLE] < 2) 
      {
      stc( "#0You do not have enough Marble.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_MARBLE);
    hp      = 400;
    add_affect( obj, APPLY_HIT, hp );
  }
  if (!str_prefix(arg2, "silver"))
  {

	if (ch->rawmaterial[M_SILVER] < 5) 
      {
      stc( "#0You do not have enough Silver.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_SILVER);
    armor   = -100;
    add_affect( obj, APPLY_AC, armor );

  }
  if (!str_prefix(arg2, "wool"))
  {

	if (ch->rawmaterial[M_WOOL] < 5) 
      {
      stc( "#0You do not have enough wool.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_WOOL);
    move    = 400;
    add_affect( obj, APPLY_MOVE, move );
  }
  if (!str_prefix(arg2, "cloth"))
  {

	if (ch->rawmaterial[M_CLOTH] < 5) 
      {
      stc( "#0You do not have enough Cloth.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_CLOTH);
    move    = 400;
    add_affect( obj, APPLY_MOVE, move );
  }






// ARG 3!!!!!
  if (!str_prefix(arg3, "copper"))
  {

	if (ch->rawmaterial[M_COPPER] < 5) 
      {
      stc( "#0You do not have enough copper.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_COPPER);
      mana    = 400;
      add_affect( obj, APPLY_MANA, mana );
  }
  if (!str_prefix(arg3, "gold"))
  {

	if (ch->rawmaterial[M_GOLD] < 5) 
      {
      stc( "#0You do not have enough gold.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_GOLD);
    mana    = 400;
    add_affect( obj, APPLY_MANA, mana );  }
  if (!str_prefix(arg3, "ivory"))
  {

	if (ch->rawmaterial[M_IVORY] < 2) 
      {
      stc( "#0You do not have enough Ivory.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_IVORY);
    damroll = 125;
    add_affect( obj, APPLY_DAMROLL, damroll );
  }
  if (!str_prefix(arg3, "steel"))
  {

	if (ch->rawmaterial[M_STEEL] < 5) 
      {
      stc( "#0You do not have enough steel.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_STEEL);
    hp      = 400;
    add_affect( obj, APPLY_HIT, hp );
  }
  if (!str_prefix(arg3, "mithril"))
  {

	if (ch->rawmaterial[M_MITHRIL] < 5) 
      {
      stc( "#0You do not have enough mithril.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_MITHRIL);
    hp      = 400;
    add_affect( obj, APPLY_HIT, hp );

  }
  if (!str_prefix(arg3, "wood"))
  {

	if (ch->rawmaterial[M_WOOD] < 5) 
      {
      stc( "#0You do not have enough Wood.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_WOOD);
    hp      = 400;
    add_affect( obj, APPLY_HIT, hp );
  }
  if (!str_prefix(arg3, "organic"))
  {

	if (ch->rawmaterial[M_ORGANIC] < 5) 
      {
      stc( "#0You do not have enough organic material.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_ORGANIC);
      obj->value[3] = 14; // Regen
  }
  if (!str_prefix(arg3, "diamond"))
  {

	if (ch->rawmaterial[M_DIAMOND] < 1) 
      {
      stc( "#0You do not have enough diamonds.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_DIAMOND);
      obj->value[3] = 9; // Sanc
  }
  if (!str_prefix(arg3, "platinum"))
  {

	if (ch->rawmaterial[M_PLATINUM] < 2) 
      {
      stc( "#0You do not have enough platinum.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_PLATINUM);
    hitroll = 125;
    add_affect( obj, APPLY_HITROLL, hitroll );
  }
  if (!str_prefix(arg3, "adamantite"))
  {

	if (ch->rawmaterial[M_ADAMANTITE] < 5) 
      {
      stc( "#0You do not have enough adamantite.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_ADAMANTITE);
    hp      = 400;
    add_affect( obj, APPLY_HIT, hp );
  }
  if (!str_prefix(arg3, "brass"))
  {

	if (ch->rawmaterial[M_BRASS] < 5) 
      {
      stc( "#0You do not have enough Brass.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_BRASS);
    hp      = 400;
    add_affect( obj, APPLY_HIT, hp );
  }
  if (!str_prefix(arg3, "bronze"))
  {

	if (ch->rawmaterial[M_BRONZE] < 5) 
      {
      stc( "#0You do not have enough bronze.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_BRONZE);
    hp      = 400;
    add_affect( obj, APPLY_HIT, hp );
  }
  if (!str_prefix(arg3, "tin"))
  {

	if (ch->rawmaterial[M_TIN] < 5) 
      {
      stc( "#0You do not have enough TIN.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_TIN);
    move    = 400;
    add_affect( obj, APPLY_MOVE, move );
  }
  if (!str_prefix(arg3, "ebony"))
  {

	if (ch->rawmaterial[M_EBONY] < 2) 
      {
      stc( "#0You do not have enough ebony.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_EBONY);

    armor   = -100;
    add_affect( obj, APPLY_AC, armor );
  }
  if (!str_prefix(arg3, "ruby"))
  {

	if (ch->rawmaterial[M_RUBY] < 1) 
      {
      stc( "#0You do not have a ruby.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_RUBY);
      obj->value[3] = 14; // Haste
  }
  if (!str_prefix(arg3, "limestone"))
  {

	if (ch->rawmaterial[M_LIMESTONE] < 2) 
      {
      stc( "#0You do not have enough Limestone.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_LIMESTONE);
    move    = 400;
    add_affect( obj, APPLY_MOVE, move );
  }
  if (!str_prefix(arg3, "topaz"))
  {

	if (ch->rawmaterial[M_TOPAZ] < 1) 
      {
      stc( "#0You do not have enough Topaz.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_TOPAZ);
      obj->value[3] = 10; // Detect hidden
  }
  if (!str_prefix(arg3, "amethyst"))
  {

	if (ch->rawmaterial[M_AMETHYST] < 1) 
      {
      stc( "#0You do not have an Amethyst.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_AMETHYST);
      obj->value[3] = 3; // Fly
  }
  if (!str_prefix(arg3, "jade"))
  {

	if (ch->rawmaterial[M_JADE] < 1) 
      {
      stc( "#0You do not have enough Jade.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_JADE);
      obj->value[3] = 4; // Infra
  }
  if (!str_prefix(arg3, "onyx"))
  {

	if (ch->rawmaterial[M_ONYX] < 1) 
      {
      stc( "#0You do not have enough Onyx.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_ONYX);
      obj->value[3] = 18; // Sneak
  }
  if (!str_prefix(arg3, "emerald"))
  {

	if (ch->rawmaterial[M_EMERALD] < 1) 
      {
      stc( "#0You do not have an Emerald.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_EMERALD);
      obj->value[3] = 15; // sup cut
  }
  if (!str_prefix(arg3, "pearl"))
  {

	if (ch->rawmaterial[M_PEARL] < 1) 
      {
      stc( "#0You do not have a Pearl.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_PEARL);
      obj->value[3] = 5; // invis
  }
  if (!str_prefix(arg3, "opal"))
  {
	if (ch->rawmaterial[M_OPAL] < 1) 
      {
      stc( "#0You do not have an opal.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_OPAL);
      obj->value[3] = 6; // pass door
  }
  if (!str_prefix(arg3, "sapphire"))
  {

	if (ch->rawmaterial[M_SAPPHIRE] < 1) 
      {
      stc( "#0You do not have a Sapphire.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_SAPPHIRE);
      obj->value[3] = 12; // chaos shield
  }
  if (!str_prefix(arg3, "leather"))
  {

	if (ch->rawmaterial[M_LEATHER] < 5) 
      {
      stc( "#0You do not have enough leather.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_LEATHER);
    armor   = -100;
    add_affect( obj, APPLY_AC, armor );
  }
  if (!str_prefix(arg3, "iron"))
  {

	if (ch->rawmaterial[M_IRON] < 5) 
      {
      stc( "#0You do not have enough iron.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_IRON);
    armor   = -100;
    add_affect( obj, APPLY_AC, armor );
  }
  if (!str_prefix(arg3, "quartz"))
  {

	if (ch->rawmaterial[M_QUARTZ] < 2) 
      {
      stc( "#0You do not have enough Quartz.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_QUARTZ);
    armor   = -100;
    add_affect( obj, APPLY_AC, armor );
  }
  if (!str_prefix(arg3, "marble"))
  {

	if (ch->rawmaterial[M_MARBLE] < 2) 
      {
      stc( "#0You do not have enough Marble.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_MARBLE);
    hp      = 400;
    add_affect( obj, APPLY_HIT, hp );
  }
  if (!str_prefix(arg3, "silver"))
  {

	if (ch->rawmaterial[M_SILVER] < 5) 
      {
      stc( "#0You do not have enough Silver.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_SILVER);
    armor   = -100;
    add_affect( obj, APPLY_AC, armor );

  }
  if (!str_prefix(arg3, "wool"))
  {

	if (ch->rawmaterial[M_WOOL] < 5) 
      {
      stc( "#0You do not have enough wool.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_WOOL);
    move    = 400;
    add_affect( obj, APPLY_MOVE, move );
  }
  if (!str_prefix(arg3, "cloth"))
  {

	if (ch->rawmaterial[M_CLOTH] < 5) 
      {
      stc( "#0You do not have enough Cloth.\n\r", ch );
      return;
      }
      SET_BIT(obj->c_material, C_CLOTH);
    move    = 400;
    add_affect( obj, APPLY_MOVE, move );
  }



if (IS_SET(obj->c_material, C_COPPER)) 
{
ch->rawmaterial[M_COPPER] -= LB_METAL;
obj->toughness += T_COPPER;
obj->weight += LB_METAL;
}
if (IS_SET(obj->c_material, C_GOLD)) 
{
ch->rawmaterial[M_GOLD] -= LB_METAL;
obj->toughness += T_GOLD;
obj->weight += LB_METAL;
}
if (IS_SET(obj->c_material, C_IVORY))
{
ch->rawmaterial[M_IVORY] -= LB_OTHER;
obj->toughness += T_IVORY;
obj->weight += LB_OTHER;
}
if (IS_SET(obj->c_material, C_STEEL))
{
ch->rawmaterial[M_STEEL] -= LB_METAL;
obj->toughness += T_STEEL;
obj->weight += LB_METAL;
}
if (IS_SET(obj->c_material, C_MITHRIL))
{
ch->rawmaterial[M_MITHRIL] -= LB_METAL;
obj->toughness += T_MITHRIL;
obj->weight += LB_METAL;
}
if (IS_SET(obj->c_material, C_WOOD))
{
ch->rawmaterial[M_WOOD] -= LB_METAL;
obj->toughness += T_WOOD;
obj->weight += LB_METAL;
}
if (IS_SET(obj->c_material, C_ORGANIC))
{
ch->rawmaterial[M_ORGANIC] -= LB_METAL;
obj->toughness += T_ORGANIC;
obj->weight += LB_METAL;
}
if (IS_SET(obj->c_material, C_DIAMOND)) 
{
ch->rawmaterial[M_DIAMOND] -= LB_OTHER;
obj->toughness += T_DIAMOND;
obj->weight += LB_OTHER;
}
if (IS_SET(obj->c_material, C_PLATINUM))
{
ch->rawmaterial[M_PLATINUM] -= LB_METAL;
obj->toughness += T_PLATINUM;
obj->weight += LB_METAL;
}
if (IS_SET(obj->c_material, C_ADAMANTITE))
{
ch->rawmaterial[M_ADAMANTITE] -= LB_METAL;
obj->toughness += T_ADAMANTITE;
obj->weight += LB_METAL;
}
if (IS_SET(obj->c_material, C_BRASS))
{
ch->rawmaterial[M_BRASS] -= LB_METAL;
obj->toughness += T_BRASS;
obj->weight += LB_METAL;
}
if (IS_SET(obj->c_material, C_BRONZE))
{
ch->rawmaterial[M_BRONZE] -= LB_METAL;
obj->toughness += T_BRONZE;
obj->weight += LB_METAL;
}
if (IS_SET(obj->c_material, C_TIN))
{
ch->rawmaterial[M_TIN] -= LB_METAL;
obj->toughness += T_TIN;
obj->weight += LB_METAL;
}
if (IS_SET(obj->c_material, C_EBONY)) 
{
ch->rawmaterial[M_EBONY] -= LB_OTHER;
obj->toughness += T_EBONY;
obj->weight += LB_OTHER;
}
if (IS_SET(obj->c_material, C_RUBY))
{
ch->rawmaterial[M_RUBY] -= LB_GEM;
obj->toughness += T_RUBY;
obj->weight += LB_GEM;
}
if (IS_SET(obj->c_material, C_LIMESTONE))
{
ch->rawmaterial[M_LIMESTONE] -= LB_OTHER;
obj->toughness += T_LIMESTONE;
obj->weight += LB_OTHER;
}
if (IS_SET(obj->c_material, C_TOPAZ))
{
ch->rawmaterial[M_TOPAZ] -= LB_GEM;
obj->toughness += T_TOPAZ;
obj->weight += LB_GEM;
}
if (IS_SET(obj->c_material, C_AMETHYST))
{
ch->rawmaterial[M_AMETHYST] -= LB_GEM;
obj->toughness += T_AMETHYST;
obj->weight += LB_GEM;
}
if (IS_SET(obj->c_material, C_JADE))
{
ch->rawmaterial[M_JADE] -= LB_GEM;
obj->toughness += T_JADE;
obj->weight += LB_GEM;
}

if (IS_SET(obj->c_material, C_ONYX)) 
{
ch->rawmaterial[M_ONYX] -= LB_GEM;
obj->toughness += T_ONYX;
obj->weight += LB_GEM;
}
if (IS_SET(obj->c_material, C_EMERALD)) 
{
ch->rawmaterial[M_EMERALD] -= LB_GEM;
obj->toughness += T_EMERALD;
obj->weight += LB_GEM;
}
if (IS_SET(obj->c_material, C_PEARL)) 
{
ch->rawmaterial[M_PEARL] -= LB_GEM;
obj->toughness += T_PEARL;
obj->weight += LB_GEM;
}
if (IS_SET(obj->c_material, C_OPAL)) 
{
ch->rawmaterial[M_OPAL] -= LB_GEM;
obj->toughness += T_OPAL;
obj->weight += LB_GEM;
}
if (IS_SET(obj->c_material, C_SAPPHIRE)) 
{
ch->rawmaterial[M_SAPPHIRE] -= LB_GEM;
obj->toughness += T_SAPPHIRE;
obj->weight += LB_GEM;
}
if (IS_SET(obj->c_material, C_LEATHER)) 
{
ch->rawmaterial[M_LEATHER] -= LB_OTHER;
obj->toughness += T_LEATHER;
obj->weight += LB_OTHER;
}
if (IS_SET(obj->c_material, C_IRON)) 
{
ch->rawmaterial[M_IRON] -= LB_METAL;
obj->toughness += T_IRON;
obj->weight += LB_METAL;
}
if (IS_SET(obj->c_material, C_QUARTZ)) 
{
ch->rawmaterial[M_QUARTZ] -= LB_OTHER;
obj->toughness += T_QUARTZ;
obj->weight += LB_OTHER;
}
if (IS_SET(obj->c_material, C_MARBLE))
{
ch->rawmaterial[M_MARBLE] -= LB_OTHER;
obj->toughness += T_MARBLE;
obj->weight += LB_OTHER;
}
if (IS_SET(obj->c_material, C_SILVER))
{
ch->rawmaterial[M_SILVER] -= LB_METAL;
obj->toughness += T_SILVER;
obj->weight += LB_METAL;
}
if (IS_SET(obj->c_material, C_WOOL)) 
{
ch->rawmaterial[M_WOOL] -= LB_OTHER;
obj->toughness += T_WOOL;
obj->weight += LB_OTHER;
}
if (IS_SET(obj->c_material, C_CLOTH)) 
{
ch->rawmaterial[M_CLOTH] -= LB_OTHER;
obj->toughness += T_CLOTH;
obj->weight += LB_OTHER;
}

   if (obj->toughness > 100) obj->toughness = 100;

/*
    hitroll = 1LB_OTHER0;
    damroll = 120;
    armor   = -200;
    add_affect( obj, APPLY_HITROLL, hitroll );
    add_affect( obj, APPLY_DAMROLL, damroll );
    add_affect( obj, APPLY_AC, armor );
*/
    obj->material = 0;
    obj->item_type = ITEM_ARMOR;
    obj->questowner = str_dup( ch->name );

    sprintf( buf, "%s#n", name );
    free_string( obj->short_descr );
    obj->short_descr = str_dup( buf );

    sprintf( buf, "%s#n lies on the ground.", name );
    free_string( obj->description );
    obj->description = str_dup( buf );

    sprintf( buf, "%s ", name );
    free_string( obj->name );
    obj->name = str_dup( buf );
    obj_to_char( obj, ch );


return;
}

