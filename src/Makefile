CC      = gcc
C_FLAGS = -Wall -g
L_FLAGS = -lz -lcrypt -lpthread -rdynamic

O_FILES = act_comm.o act_info.o act_move.o act_obj.o act_wiz.o kav_wiz.o     \
	kav_soundex.o kav_fight.o kav_info.o board.o build.o social-edit.o   \
	fight.o arena.o mccp.o db.o x-socials.o interp.o handler.o           \
	mem.o olc.o olc_act.o olc_save.o desc_map.o wizutil.o magic.o bit.o  \
	update.o const.o save.o special.o string.o jope.o msp.o comm.o       \
        jobo_quest.o jobo_update.o jobo_save.o jobo_comm.o jobo_fight.o      \
	jobo_act.o jobo_wiz.o jobo_util.o jobo_shop.o xrakisis.o wokanist.o  \
	combat.o illusionist.o aeromancer.o enchanter.o hydromancer.o	     \
	necromancer.o pyromancer.o healing.o animal.o abjurer.o diviner.o    \
        geomancer.o transmuter.o chaos.o enhancement.o races.o ashur.o	     \
	changes.o moon.o shinobi.o cprintf.o religion.o guild.o profession.o \
	materials.o \


merc: $(O_FILES)
	rm -f aoh
	$(CC)  -o aoh $(O_FILES) $(L_FLAGS)
	chmod g+w aoh
	chmod g+w $(O_FILES)

.c.o: merc.h
	$(CC) -c $(C_FLAGS) $<

clean :
	rm -f *.o
	make
