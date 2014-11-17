// creation of scoreboard
#include "game.h"
#include "colors.h"
#include "comed.h"

namespace game
{
    VARP(scoreboard2d, 0, 1, 1);
    VARP(showservinfo, 0, 1, 1);
    VARP(showclientnum, 0, 0, 1);
    VARP(showpj, 0, 0, 1);
    VARP(showping, 0, 1, 1);
    VARP(showspectators, 0, 1, 1);
    VARP(highlightscore, 0, 1, 1);
    VARP(showconnecting, 0, 0, 1);
    XIDENT(IDF_SWLACC, VARP, showfrags, 0, 1, 1);
    XIDENT(IDF_SWLACC, VARP, showflags, 0, 1, 1);
    XIDENT(IDF_SWLACC, VARP, showdamagedealt, 0, 0, 2);
    XIDENT(IDF_SWLACC, VARP, showdamagereceived, 0, 0, 2);
    XIDENT(IDF_SWLACC, VARP, showaccuracy, 0, 0, 1);
    XIDENT(IDF_SWLACC, VARP, showdeaths, 0, 0, 1);
    XIDENT(IDF_SWLACC, VARP, showsuicides, 0, 0, 1);
    XIDENT(IDF_SWLACC, VARP, showkpd, 0, 0, 1);

    scoreboardgui scoreboard;
    
    hashset<teaminfo> teaminfos;

    void clearteaminfo()
    {
        teaminfos.clear();
    }

    void setteaminfo(const char *team, int frags)
    {
        teaminfo *t = teaminfos.access(team);
        if(!t) { t = &teaminfos[team]; copystring(t->team, team, sizeof(t->team)); }
        t->frags = frags;
    }
            
    static inline bool playersort(const fpsent *a, const fpsent *b)
    {
        if(a->state==CS_SPECTATOR)
        {
            if(b->state==CS_SPECTATOR) return strcmp(a->name, b->name) < 0;
            else return false;
        }
        else if(b->state==CS_SPECTATOR) return true;
        if(m_ctf || m_collect)
        {
            if(a->flags > b->flags) return true;
            if(a->flags < b->flags) return false;
        }
        if(a->frags > b->frags) return true;
        if(a->frags < b->frags) return false;
        return strcmp(a->name, b->name) < 0;
    }

    void getbestplayers(vector<fpsent *> &best, bool fulllist)
    {
        loopv(players)
        {
            fpsent *o = players[i];
            if(o->state!=CS_SPECTATOR) best.add(o);
        }
        best.sort(playersort);
        if(!fulllist)
            while(best.length() > 1 && best.last()->frags < best[0]->frags) 
                best.drop();
    }

    void getbestteams(vector<const char *> &best)
    {
        if(cmode && cmode->hidefrags()) 
        {
            vector<teamscore> teamscores;
            cmode->getteamscores(teamscores);
            teamscores.sort(teamscore::compare);
            while(teamscores.length() > 1 && teamscores.last().score < teamscores[0].score) teamscores.drop();
            loopv(teamscores) best.add(teamscores[i].team);
        }
        else 
        {
            int bestfrags = INT_MIN;
            enumerates(teaminfos, teaminfo, t, bestfrags = max(bestfrags, t.frags));
            if(bestfrags <= 0) loopv(players)
            {
                fpsent *o = players[i];
                if(o->state!=CS_SPECTATOR && !teaminfos.access(o->team) && best.htfind(o->team) < 0) { bestfrags = 0; best.add(o->team); } 
            }
            enumerates(teaminfos, teaminfo, t, if(t.frags >= bestfrags) best.add(t.team));
        }
    }

    static vector<scoregroup *> groups;
    static vector<fpsent *> spectators;
    
    vector<scoregroup *> getscoregroups() {
        return groups; 
    }

    static inline bool scoregroupcmp(const scoregroup *x, const scoregroup *y)
    {
        if(!x->team)
        {
            if(y->team) return false;
        }
        else if(!y->team) return true;
        if(x->score > y->score) return true;
        if(x->score < y->score) return false;
        if(x->players.length() > y->players.length()) return true;
        if(x->players.length() < y->players.length()) return false;
        return x->team && y->team && strcmp(x->team, y->team) < 0;
    }

    int groupplayers()
    {
        int numgroups = 0;
        spectators.setsize(0);
        loopv(players)
        {
            fpsent *o = players[i];
            if(!showconnecting && !o->name[0]) continue;
            if(o->state==CS_SPECTATOR) { spectators.add(o); continue; }
            const char *team = m_teammode && o->team[0] ? o->team : NULL;
            bool found = false;
            loopj(numgroups)
            {
                scoregroup &g = *groups[j];
                if(team!=g.team && (!team || !g.team || strcmp(team, g.team))) continue;
                g.players.add(o);
                found = true;
            }
            if(found) continue;
            if(numgroups>=groups.length()) groups.add(new scoregroup);
            scoregroup &g = *groups[numgroups++];
            g.team = team;
            if(!team) g.score = 0;
            else if(cmode && cmode->hidefrags()) g.score = cmode->getteamscore(o->team);
            else { teaminfo *ti = teaminfos.access(team); g.score = ti ? ti->frags : 0; }
            g.players.setsize(0);
            g.players.add(o);
        }
        loopi(numgroups) groups[i]->players.sort(playersort);
        spectators.sort(playersort);
        groups.sort(scoregroupcmp, 0, numgroups);
        return numgroups;
    }

    void renderscoreboard(g3d_gui &g, bool firstpass)
    {
        const ENetAddress *address = connectedpeer();
        if(showservinfo && address)
        {
            string hostname;
            if(enet_address_get_host_ip(address, hostname, sizeof(hostname)) >= 0)
            {
                /*if(servinfo[0]) g.titlef("%.25s", 0xFFFF80, NULL, servinfo);
                else g.titlef("%s:%d", 0xFFFF80, NULL, hostname, address->port);*/
            	if(servinfo[0]) g.titlef("%.25s", COL_WHITE, NULL, servinfo); // flat gui
            	else g.titlef("%s:%d", COL_WHITE, NULL, hostname, address->port); // flat gui
            }
        }
     
        g.pushlist();
        g.spring();
        //g.text(server::modename(gamemode), 0xFFFF80);
        g.text(server::modename(gamemode), COL_WHITE); // flat gui
        g.separator();
        const char *mname = getclientmap();
        //g.text(mname[0] ? mname : "[new map]", 0xFFFF80);
        g.text(mname[0] ? mname : "[new map]", COL_WHITE); //flat gui
        extern int gamespeed;
        //if(gamespeed != 100) { g.separator(); g.textf("%d.%02dx", 0xFFFF80, NULL, gamespeed/100, gamespeed%100); }
        if(gamespeed != 100) { g.separator(); g.textf("%d.%02dx", COL_WHITE, NULL, gamespeed/100, gamespeed%100); } // flat gui
        if(m_timed && mname[0] && (maplimit >= 0 || intermission))
        {
            g.separator();
            //if(intermission) g.text("intermission", 0xFFFF80);
            if(intermission) g.text("intermission", COL_WHITE); // flat gui
            else 
            {
                int secs = max(maplimit-lastmillis, 0)/1000, mins = secs/60;
                secs %= 60;
                g.pushlist();
                g.strut(mins >= 10 ? 4.5f : 3.5f);
                //g.textf("%d:%02d", 0xFFFF80, NULL, mins, secs);
                g.textf("%d:%02d", COL_WHITE, NULL, mins, secs); // flat gui
                g.poplist();
            }
        }
        //if(ispaused()) { g.separator(); g.text("paused", 0xFFFF80); }
        if(ispaused()) { g.separator(); g.text("paused", COL_WHITE); } // flat gui
        g.spring();
        g.poplist();

        g.separator();
 
        int numgroups = groupplayers();
        loopk(numgroups)
        {
            if((k%2)==0) g.pushlist(); // horizontal
            
            scoregroup &sg = *groups[k];
            //int bgcolor = sg.team && m_teammode ? (isteam(player1->team, sg.team) ? 0x3030C0 : 0xC03030) : 0,
            //    fgcolor = 0xFFFF80;
            int teamcolor = sg.team && m_teammode ? (isteam(autohudplayer()->team, sg.team) ? COL_BLUE : COL_RED) : COL_WHITE; // flat gui

            g.pushlist(); // vertical
            g.pushlist(); // horizontal

            #define loopscoregroup(o, b) \
                loopv(sg.players) \
                { \
                    fpsent *o = sg.players[i]; \
                    b; \
                }    

            g.pushlist();
            if(sg.team && m_teammode)
            {
                g.pushlist();
                //g.background(bgcolor, numgroups>1 ? 3 : 5);
                g.strut(1);
                g.poplist();
            }
            //g.text("", 0, " ");
            g.pushlist(); // flat gui
			g.strut(1); // flat gui
			g.poplist(); // flat gui
            loopscoregroup(o,
            {
                if(o==player1 && highlightscore && (multiplayer(false) || demoplayback || players.length() > 1))
                {
                    g.pushlist();
                    //g.background(0x808080, numgroups>1 ? 3 : 5);
                    g.background(0x808080, numgroups>1 ? 2 : 4); // flat gui
                }
                /*const playermodelinfo &mdl = getplayermodelinfo(o);
                const char *icon = sg.team && m_teammode ? (isteam(player1->team, sg.team) ? mdl.blueicon : mdl.redicon) : mdl.ffaicon;
                g.text("", 0, icon); */
                g.text("", 0); // flat gui
                if(o==player1 && highlightscore && (multiplayer(false) || demoplayback || players.length() > 1)) g.poplist();
            });
            g.poplist();

            if(sg.team && m_teammode)
            {
                g.pushlist(); // vertical

                /*if(sg.score>=10000) g.textf("%s: WIN", fgcolor, NULL, sg.team);
                else g.textf("%s: %d", fgcolor, NULL, sg.team, sg.score);*/
				if(sg.score>=10000) g.textf("%s: WIN", teamcolor, NULL, sg.team); // flat gui
				else g.textf("%s: %d", teamcolor, NULL, sg.team, sg.score); // flat gui

                g.pushlist(); // horizontal
            }

            if((m_ctf || m_collect) && showflags)
            {
               g.pushlist();
               g.strut(m_ctf?4:5);
               //g.text(m_ctf?"flags":"skulls", fgcolor);
               g.text(m_ctf?"flags":"skulls", COL_GREY); // flat gui
               //loopscoregroup(o, g.textf("%d", 0xFFFFDD, NULL, o->flags));
               loopscoregroup(o, g.textf("%d", COL_WHITE, NULL, o->flags)); // flat gui
               g.poplist();
            }

            if(!cmode || !cmode->hidefrags() || showfrags)
            { 
                g.pushlist();
                g.strut(5);
                //g.text("frags", fgcolor);
                //loopscoregroup(o, g.textf("%d", 0xFFFFDD, NULL, o->frags));
                g.text("frags", COL_GREY); // flat gui
                loopscoregroup(o, g.textf("%d", COL_WHITE, NULL, o->frags)); // flat gui
                g.poplist();
            }
			// show death
			if(showdeaths)
			{
				g.pushlist();
				g.strut(6);
				//g.text("deaths", fgcolor);
				g.text("deaths", COL_GREY); // flat gui
				//loopscoregroup(o, g.textf("%d", 0xFFFFDD, NULL, o->deaths));
				loopscoregroup(o, g.textf("%d", COL_WHITE, NULL, o->deaths)); // flat gui
				g.poplist();
			}
			// show suicides
			if(showsuicides)
			{
				g.pushlist();
				g.strut(5);
				//g.text("sui", fgcolor);
				g.text("sui", COL_GREY); // flat gui
				//loopscoregroup(o, g.textf("%d", 0xFFFFDD, NULL, o->suicides));
				loopscoregroup(o, g.textf("%d", COL_WHITE, NULL, o->suicides)); // flat gui
				g.poplist();
			}
			// show kpd
			if(showkpd)
			{
				g.pushlist();
				g.strut(5);
				//g.text("K/D", fgcolor);
				g.text("K/D", COL_GREY); // flat gui
				//loopscoregroup(o, g.textf("%d", 0xFFFFDD, NULL,  o->deaths ? g.textf("%.2f", COL_WHITE, NULL, (float)o->frags/o->deaths) : g.textf("%d.00", COL_WHITE, NULL, o->frags) ));
				loopscoregroup(o, o->deaths ? g.textf("%.2f", COL_WHITE, NULL, (float)o->frags/o->deaths) : g.textf("%d.00", COL_WHITE, NULL, o->frags) ); // flat gui
				g.poplist();
			}
            // show totaldamage per player
			if(showdamagedealt)
			{
				g.pushlist();
				g.strut(6);
				//g.text("dd", fgcolor);
				g.text("dd", COL_GREY); // flat gui
				if (showdamagedealt == 1)
				{
					//loopscoregroup(o, o->totaldamage > 1000 ? g.textf("%.2fk", 0xFFFFDD, NULL, (float)o->totaldamage/1000 ) : g.textf("%d", 0xFFFFDD, NULL,  o->totaldamage));
					loopscoregroup(o, o->totaldamage > 1000 ? g.textf("%.2fk", COL_WHITE, NULL,(float)o->totaldamage/1000 ) : g.textf("%d", COL_WHITE, NULL,  o->totaldamage)); // flat gui
				}
				else {
					//loopscoregroup(o, g.textf("%d", 0xFFFFDD, NULL,  o->totaldamage));
					loopscoregroup(o, g.textf("%d", COL_WHITE, NULL,  o->totaldamage)); // flat gui
				}
				g.poplist();
			}
			// show damagereceived per player
			if(showdamagereceived)
			{
				g.pushlist();
				g.strut(6);
				//g.text("dr", fgcolor);
				g.text("dr", COL_GREY); // flat gui
				if (showdamagereceived == 1)
				{
					//loopscoregroup(o, o->damagereceived > 1000 ? g.textf("%.2fk", 0xFFFFDD, NULL, (float)o->damagereceived/1000 ) : g.textf("%d", 0xFFFFDD, NULL,  o->damagereceived));
					loopscoregroup(o, o->damagereceived > 1000 ? g.textf("%.2fk", COL_WHITE, NULL, (float)o->damagereceived/1000 ) : g.textf("%d", COL_WHITE, NULL,  o->damagereceived)); // flat gui
				}
				else {
					//loopscoregroup(o, g.textf("%d", 0xFFFFDD, NULL,  o->damagereceived));
					loopscoregroup(o, g.textf("%d", COL_WHITE, NULL,  o->damagereceived)); // flat gui
				}
				g.poplist();
			}
			// show accuracy per player
			if(showaccuracy)
			{
				g.pushlist();
				g.strut(5);
				//g.text("acc", fgcolor);
				g.text("acc", COL_GREY); // flat gui
				//loopscoregroup(o, g.textf("%d%%", 0xFFFFDD, NULL, (o->totaldamage*100)/max(o->totalshots, 1)));
				loopscoregroup(o, g.textf("%d%%", COL_WHITE, NULL, (o->totaldamage*100)/max(o->totalshots, 1))); // flat gui
				g.poplist();
			}

            g.pushlist();
            //g.text("name", fgcolor);
            g.text("name", COL_GREY); // flat gui
            g.strut(13);
            loopscoregroup(o, 
            {
                //int status = o->state!=CS_DEAD ? 0xFFFFDD : 0x606060;
            	int status = o->state!=CS_DEAD ? COL_WHITE : COL_DEAD; // flat gui
                if(o->privilege)
                {
                    //status = o->privilege>=PRIV_ADMIN ? 0xFF8000 : 0x40FF80;
                    status = o->privilege>=PRIV_ADMIN ? COL_ADMIN : COL_MASTER; // flat gui
                    if(o->state==CS_DEAD) status = (status>>1)&0x7F7F7F;
                }
                g.textf("%s ", status, NULL, colorname(o));
            });
            g.poplist();

            if(multiplayer(false) || demoplayback)
            {
                if(showpj)
                {
                    g.pushlist();
                    g.strut(5);
                    //g.text("pj", fgcolor);
                    g.text("pj", COL_GREY); // flat gui
                    loopscoregroup(o,
                    {
                        /*if(o->state==CS_LAGGED) g.text("LAG", 0xFFFFDD);
                        else g.textf("%d", 0xFFFFDD, NULL, o->plag);*/
						if(o->state==CS_LAGGED) g.text("LAG", COL_WHITE); // flat gui
						else g.textf("%d", COL_WHITE, NULL, o->plag); // flat gui
                    });
                    g.poplist();
                }

                if(showping)
                {
                    g.pushlist();
                    //g.text("ping", fgcolor);
                    g.text("ping", COL_GREY); // flat gui
                    g.strut(6);
                    loopscoregroup(o,
                    {
                        fpsent *p = o->ownernum >= 0 ? getclient(o->ownernum) : o;
                        if(!p) p = o;
                        /*if(!showpj && p->state==CS_LAGGED) g.text("LAG", 0xFFFFDD);
                        else g.textf("%d", 0xFFFFDD, NULL, p->ping);*/
                        if(!showpj && p->state==CS_LAGGED) g.text("LAG", COL_WHITE); // flat gui
                        else g.textf("%d", COL_WHITE, NULL, p->ping); // flat gui
                    });
                    g.poplist();
                }
            }

            if(showclientnum || player1->privilege>=PRIV_MASTER)
            {
                g.space(1);
                g.pushlist();
                //g.text("cn", fgcolor);
                g.text("cn", COL_GREY); // flat gui
                //loopscoregroup(o, g.textf("%d", 0xFFFFDD, NULL, o->clientnum));
                loopscoregroup(o, g.textf("%d", COL_WHITE, NULL, o->clientnum)); // flat gui
                g.poplist();
            }
            
            if(sg.team && m_teammode)
            {
                g.poplist(); // horizontal
                g.poplist(); // vertical
            }

            g.poplist(); // horizontal
            g.poplist(); // vertical

            //if(k+1<numgroups && (k+1)%2) g.space(2);
            if(k+1<numgroups && (k+1)%2) g.space(4); // flat gui
            else g.poplist(); // horizontal
        }
        
        if(showspectators && spectators.length())
        {
        	g.separator(); // flat gui
        	g.textf("%d spectator%s", COL_YELLOW, NULL, spectators.length(), spectators.length()!=1 ? "s" : ""); // flat gui
            if(showclientnum || player1->privilege>=PRIV_MASTER)
            {
                g.pushlist();
                
                g.pushlist();
                //g.text("spectator", 0xFFFF80, " ");
				g.text("name", COL_GREY); // flat gui
				g.strut(13); // flat gui
                loopv(spectators) 
                {
                    fpsent *o = spectators[i];
                    //int status = 0xFFFFDD;
                    //if(o->privilege) status = o->privilege>=PRIV_ADMIN ? 0xFF8000 : 0x40FF80;
                    int status = COL_WHITE; // flat gui
					if(o->privilege) status = o->privilege>=PRIV_ADMIN ? COL_ADMIN : COL_MASTER; // flat gui

                    if(o==player1 && highlightscore)
                    {
                        g.pushlist();
                        g.background(0x808080, 3);
                    }
                    //g.text(colorname(o), status, "spectator");
                    g.text(colorname(o), status); // flat gui
                    if(o==player1 && highlightscore) g.poplist();
                }
                g.poplist();

                g.space(1);
                g.pushlist();
                //g.text("cn", 0xFFFF80);
                g.text("cn", COL_GREY); // flat gui
                //loopv(spectators) g.textf("%d", 0xFFFFDD, NULL, spectators[i]->clientnum);
                loopv(spectators) g.textf("%d", COL_WHITE, NULL, spectators[i]->clientnum); // flat gui
                g.poplist();

                if(showping){
                       g.space(1);
                       g.pushlist();
                       //g.text("ping", 0xFFFF80);
                       g.text("ping", COL_GREY); // flat gui
                       loopv(spectators){
                               fpsent *p = spectators[i]->ownernum >= 0 ? getclient(spectators[i]->ownernum) : spectators[i];
                               if(!p) p = spectators[i];
                               /*if(!showpj && p->state==CS_LAGGED) g.text("LAG", 0xFFFFDD);
                               else g.textf("%d", 0xFFFFDD, NULL, p->ping);*/
                               if(!showpj && p->state==CS_LAGGED) g.text("LAG", COL_WHITE); // flat gui
							   else g.textf("%d", COL_WHITE, NULL, p->ping); // flat gui
                       }
                       g.poplist();
                }

                g.poplist();
            }
            else
            {
                // g.textf("%d spectator%s", 0xFFFF80, " ", spectators.length(), spectators.length()!=1 ? "s" : ""); // flat gui
                loopv(spectators)
                {
                    if((i%3)==0) 
                    {
                        g.pushlist();
                        // g.text("", 0xFFFFDD, "spectator"); // flat gui
                        //g.text("", COL_WHITE, "spectator"); // flat gui
                    }
                    fpsent *o = spectators[i];
                    /*int status = 0xFFFFDD;
                    if(o->privilege) status = o->privilege>=PRIV_ADMIN ? 0xFF8000 : 0x40FF80;*/
                    int status = COL_WHITE; // flat gui
                    if(o->privilege) status = o->privilege>=PRIV_ADMIN ? COL_ADMIN : COL_MASTER; // flat gui
                    if(o==player1 && highlightscore)
                    {
                        g.pushlist();
                        g.background(0x808080);
                    }
                    g.text(colorname(o), status);
                    if(o==player1 && highlightscore) g.poplist();
                    if(i+1<spectators.length() && (i+1)%3) g.space(1);
                    else g.poplist();
                }
            }
        }
    }

    void g3d_gamemenus()
    {
        scoreboard.render();
    }

    VARFN(scoreboard, showscoreboard, 0, 0, 1, scoreboard.show(showscoreboard!=0));

    void showscores(bool on)
    {
        showscoreboard = on ? 1 : 0;
        scoreboard.show(on);
    }
    ICOMMAND(showscores, "D", (int *down), showscores(*down!=0));
}

