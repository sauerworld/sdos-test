#ifndef sauerbraten_sdosscripts_h
#define sauerbraten_sdosscripts_h

/* automatically generated file -- do not edit */

const char *script_demo =
"newgui keepdemogui [\n"
"	guititle \"Keep demo?\"\n"
"	guialign 0 [\n"
"        guilist [\n"
"            guibar\n"
"            guitext \"^f7Automatic client demo recording is ^f0active.\"\n"
"            guitext \"^f7Would you like to ^f0keep ^f7the last recorded demo?\"\n"
"            guistrut\n"
"            guibar\n"
"            guilist [\n"
"                guistrut\n"
"                guitext \"         \"\n"
"                guibutton \"^f0Yes ^f7(default) \" [keepdemo 1]\n"
"                guistrut\n"
"                guibutton \"^f3No \" [keepdemo 0]\n"
"                guistrut\n"
"            ]\n"
"            guibar\n"
"        ]\n"
"	]\n"
"] \"Client Demo\"\n"
;

const char *script_clock =
"newgui gameclock_settings [\n"
"    guititle \"Game Clock Settings\"\n"
"    guibar\n"
"    guicheckbox \"Show\" gameclock\n"
"    //guicheckbox \"Count up\" gameclockcountup\n"
"    guistrut 1\n"
"    guicheckbox \"Turn red on low time\" gameclockturnredonlowtime\n"
"    guistrut 1\n"
"    guitext \"Size:\" 0\n"
"    guislider gameclocksize\n"
"    guitab \"Color\"\n"
"    guitext \"Color (^f3R^f~/^f0G^f~/^f1B^f~/^f4A^f~):\" 0\n"
"    guislider gameclockcolor_r\n"
"    guislider gameclockcolor_g\n"
"    guislider gameclockcolor_b\n"
"    guislider gameclockcolor_a\n"
"    guitab \"Position\"\n"
"    guitext \"Offset (X/Y) ^f4(radar absent)^f~:\" 0\n"
"    guislider gameclockoffset_x\n"
"    guislider gameclockoffset_y\n"
"    guistrut 1\n"
"    guitext \"Offset (X/Y) ^f4(radar present)^f~:\" 0\n"
"    guislider gameclockoffset_x_withradar\n"
"    guislider gameclockoffset_y_withradar\n"
"] \"General\"\n"
;

const char *sdos_scripts[] = { script_demo, script_clock, 0 };

#endif /* sauerbraten_sdosscripts_h */

