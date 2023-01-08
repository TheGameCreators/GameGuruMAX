//----------------------------------------------------
//--- GAMEGURU - M-Debug
//----------------------------------------------------

// Includes 
#include "stdafx.h"
#include "gameguru.h"

// 
//  Debug Routines
// 
bool bTimestampactivityReady = false;

void backuptimestampactivity(void)
{
	// 091215 - to ensure old LOG files are preserved, make a copy of any existing LOG before write a new one
	cstr file_s = "";
	if ( g.gproducelogfiles > 0 )
	{
		cstr logfile_s;
		if ( g.gproducelogfilesdir_s.Len() > 0 )
			logfile_s = g.gproducelogfilesdir_s+"\\"+g.trueappname_s+".log";
		else
			logfile_s = g.fpscrootdir_s+"\\"+g.trueappname_s+".log";
		file_s = logfile_s; 
		if (FileExist(file_s.Get()) == 1)
		{
			cstr filedest_s = g.fpscrootdir_s + "\\" + g.trueappname_s + "-last.log";
			if (FileExist(filedest_s.Get()) == 1)  DeleteAFile(filedest_s.Get());
			CopyFileA(file_s.Get(), filedest_s.Get(), TRUE);
		}
	}
	bTimestampactivityReady = true;
}

#define USEAPPENDLOG
#ifdef USEAPPENDLOG
	//PE: Way faster! old way added 2+ sec to the startup logging times, did not give the correct results.
	#include "stdio.h"
	int debugfilenetries = 0;

#endif

std::vector<std::string> early_debug_list;

void timestampactivity ( int i, char* desc_s )
{
	cstr videomemsofardesc_s =  "";
	cstr videomemdesc_s =  "";
	cstr memdesc_s =  "";
	cstr tpart1_s =  "";
	cstr tpart2_s =  "";
	cstr tpart3_s =  "";
	cstr file_s =  "";
	int smem = 0;
	int mem = 0;

#ifdef USEAPPENDLOG
	if (g.gproducelogfiles > 0 || !bTimestampactivityReady)
	{
		Dim(t.timestampactivity_s, 1);

		file_s = g.fpscrootdir_s + "\\" + g.trueappname_s + ".log";

		if (bTimestampactivityReady)
		{
			if (debugfilenetries == 0) {
				if (FileExist(file_s.Get()) == 1)  DeleteAFile(file_s.Get());
			}
		}
		else
		{
			extern char* m_szTokenString;
			if (!m_szTokenString)
			{
				InitCTextC(); //PE: Init new CTextC.
				void ValidateWorkStringBySize(unsigned int dwSize);
				ValidateWorkStringBySize(2048);
				SetLocalTimerReset();
			}
		}
		smem = SMEMAvailable(1);
		memdesc_s = Str((smem - g.timestampactivitymemthen) / 1024);
		memdesc_s = memdesc_s + "MB";
		g.timestampactivitymemthen = smem;
		if (g.gproducetruevidmemreading == 1)
		{
			Sync();
		}
		mem = DMEMAvailable();
		videomemdesc_s = Str((mem - g.timestampactivityvideomemthen)); videomemdesc_s = videomemdesc_s + "MB";
		g.timestampactivityvideomemthen = mem;
		videomemsofardesc_s = " ("; videomemsofardesc_s = videomemsofardesc_s + Str(smem / 1024) + "," + Str(DMEMAvailable()) + ")";
		tpart1_s = Str(Timer()); tpart1_s = tpart1_s + " : " + desc_s + " ";
		tpart2_s = "S:"; tpart2_s = tpart2_s + memdesc_s;
		tpart3_s = "V:"; tpart3_s = tpart3_s + videomemsofardesc_s;
		if (Len(tpart1_s.Get())<64)  tpart1_s = tpart1_s + Spaces(64 - Len(tpart1_s.Get()));
		if (Len(tpart2_s.Get())<8)  tpart2_s = tpart2_s + Spaces(8 - Len(tpart2_s.Get()));
		if (Len(tpart3_s.Get())<16)  tpart3_s = tpart3_s + Spaces(16 - Len(tpart3_s.Get()));
		t.timestampactivity_s[0] = tpart1_s;
		t.timestampactivity_s[0] += tpart2_s;
		t.timestampactivity_s[0] += tpart3_s;

		if (bTimestampactivityReady)
		{
			FILE * pFile;
			pFile = GG_fopen(file_s.Get(), "a+");
			if (pFile != NULL)
			{
				if (early_debug_list.size() > 0)
				{
					for (int l = 0; l < early_debug_list.size(); l++)
					{
						fputs(early_debug_list[l].c_str(), pFile);
						fputs("\n", pFile);
					}
					early_debug_list.clear();
				}
				fputs(t.timestampactivity_s[0].Get(), pFile);
				fputs("\n", pFile);
				fclose(pFile);
			}
		}
		else
		{
			early_debug_list.push_back(t.timestampactivity_s[0].Get());
		}
		if(bTimestampactivityReady)
			debugfilenetries++;
	}
#else
	if ( g.gproducelogfiles > 0 ) 
	{

		if ( i == 0 ) 
		{
			++g.timestampactivityindex;
			if (  g.timestampactivityindex>7999 ) //PE: Increase to 8000 lines.
			{
				g.timestampactivityindex=7999;
			}
			i=g.timestampactivityindex;
		}
		else
		{
			g.timestampactivityindex=i;
		}
		if (  i>g.timestampactivitymax ) 
		{
			g.timestampactivitymax=i;
		}
		Dim (  t.timestampactivity_s,g.timestampactivitymax  );
		smem=SMEMAvailable(1);
		memdesc_s=Str((smem-g.timestampactivitymemthen)/1024);
		memdesc_s = memdesc_s + "MB";
		g.timestampactivitymemthen=smem;
		if (  g.gproducetruevidmemreading == 1 ) 
		{
			Sync (  );
		}
		mem=DMEMAvailable();
		videomemdesc_s=Str((mem-g.timestampactivityvideomemthen)); videomemdesc_s = videomemdesc_s +"MB";
		g.timestampactivityvideomemthen=mem;
		videomemsofardesc_s=" ("; videomemsofardesc_s = videomemsofardesc_s+Str(smem/1024)+","+Str(DMEMAvailable())+")";
		tpart1_s=Str(Timer()); tpart1_s = tpart1_s + " : "+desc_s+" "; 
		tpart2_s="S:"; tpart2_s = tpart2_s+memdesc_s;
		tpart3_s="V:"; tpart3_s = tpart3_s+videomemsofardesc_s;
		if (  Len(tpart1_s.Get())<64  )  tpart1_s = tpart1_s+Spaces(64-Len(tpart1_s.Get()));
		if (  Len(tpart2_s.Get())<8  )  tpart2_s = tpart2_s+Spaces(8-Len(tpart2_s.Get()));
		if (  Len(tpart3_s.Get())<16  )  tpart3_s = tpart3_s+Spaces(16-Len(tpart3_s.Get()));
		t.timestampactivity_s[i]=tpart1_s;
		t.timestampactivity_s[i]+=tpart2_s;
		t.timestampactivity_s[i]+=tpart3_s;
		cstr logfile_s;
		if ( g.gproducelogfilesdir_s.Len() > 0 )
			logfile_s = g.gproducelogfilesdir_s+"\\"+g.trueappname_s+".log";
		else
			logfile_s = g.fpscrootdir_s+"\\"+g.trueappname_s+".log";
		file_s = logfile_s; if (  FileExist(file_s.Get()) == 1  )  DeleteAFile (  file_s.Get() );
		if (  Len(g.trueappname_s.Get())>0 ) 
		{
			OpenToWrite (  13,file_s.Get() );
			//PE: using t.t here can cause tons of problems as t.t is used in so many places.
			//PE: So adding debug to functions can break everything. just use a local.
			//PE: Sorry just spent 4 hours to find a bug , just to find out that the debug logging caused the bugs.
			for ( int localt = 1 ; localt <=  g.timestampactivitymax; localt++ )
			{
				WriteString (  13,t.timestampactivity_s[localt].Get() );
			}
			CloseFile (  13 );
		}
	}
#endif
}

#ifdef VRTECH
#else
void timestampactivityConsole(int i, char* desc_s)
{
	cstr videomemsofardesc_s = "";
	cstr videomemdesc_s = "";
	cstr memdesc_s = "";
	cstr tpart1_s = "";
	cstr tpart2_s = "";
	cstr tpart3_s = "";
	cstr file_s = "";
	int smem = 0;
	int mem = 0;

	smem = SMEMAvailable(1);
	memdesc_s = Str((smem - g.timestampactivitymemthen) / 1024);
	memdesc_s = memdesc_s + "MB";
	g.timestampactivitymemthen = smem;
	if (g.gproducetruevidmemreading == 1)
	{
		Sync();
	}
	mem = DMEMAvailable();
	videomemdesc_s = Str((mem - g.timestampactivityvideomemthen)); videomemdesc_s = videomemdesc_s + "MB";
	g.timestampactivityvideomemthen = mem;
	videomemsofardesc_s = " ("; videomemsofardesc_s = videomemsofardesc_s + Str(smem / 1024) + "," + Str(DMEMAvailable()) + ")";
	tpart1_s = Str(Timer()); tpart1_s = tpart1_s + " : " + desc_s + " ";
	tpart2_s = "S:"; tpart2_s = tpart2_s + memdesc_s;
	tpart3_s = "V:"; tpart3_s = tpart3_s + videomemsofardesc_s;
	if (Len(tpart1_s.Get())<64)  tpart1_s = tpart1_s + Spaces(64 - Len(tpart1_s.Get()));
	if (Len(tpart2_s.Get())<8)  tpart2_s = tpart2_s + Spaces(8 - Len(tpart2_s.Get()));
	if (Len(tpart3_s.Get())<16)  tpart3_s = tpart3_s + Spaces(16 - Len(tpart3_s.Get()));
	t.timestampactivity_s[0] = tpart1_s;
	t.timestampactivity_s[0] += tpart2_s;
	t.timestampactivity_s[0] += tpart3_s;
	OutputDebugString(t.timestampactivity_s[0].Get());
	OutputDebugString("\n");
}
#endif
