// code for the "notes" pd class. 
// Encodes information created in Pd into a Lilypond score (lilypond.org)
// developed by Jaime Oliver La Rosa (la.rosa@nyu.edu)
// @ the NYU Waverly Labs in the Music Department - FAS. (nyu-waverlylabs.org)
// Released under the GNU General Public License. 

#include "m_pd.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/utsname.h> // get OS type
#include "g_canvas.h" 
#define MXS 16384 // Maximum size of arrays  ( or input messages...)
struct utsname unameData;
////	____________________________________________________ FUNCTIONS
float reduce_meter(int a) 					{
	float b, c;
	
	if 		(a == 2) { a = 1; 	b = .16;	}
	else if (a == 6) { a = 3; 	b = .16;	}
	else if (a == 10){ a = 5; 	b = .16;	}
	else if (a == 14){ a = 7; 	b = .16;	} 
	else if (a == 18){ a = 9; 	b = .16;	} 
	else if (a == 22){ a = 11; 	b = .16;	}
	else if (a == 26){ a = 13; 	b = .16;	}
	else if (a == 30){ a = 15; 	b = .16;	}

	else if (a == 4) { a = 1; 	b = .08;	} 
	else if (a == 12){ a = 3; 	b = .08;	}
	else if (a == 20){ a = 5; 	b = .08;	}
	else if (a == 28){ a = 7; 	b = .08;	}

	else if (a == 8) { a = 1; 	b = .04;	} 
	else if (a == 16){ a = 2; 	b = .04;	} 
	else if (a == 24){ a = 3; 	b = .04;	}
	else if (a == 32){ a = 4; 	b = .04;	}
	
	else {a = 0; b = 0;}

	c = a + b;
	return (c);
}
int get_beatsize(int a, int b, int c) 		{
	// a = num, b = den, c = refdur
	int m, beatsize;
	
	if 		(b == 4) {
		beatsize = exp2f( (float) (log2f((float) c) - log2f((float) b)));
	}
	else if 		(b == 8 || b == 16 || b == 32) {
		m = b / 2;
		if 		(a == 4) {
			beatsize = exp2f( (float) (log2f((float) c) - log2f((float) m)));
			beatsize = beatsize / 2;
		}
		else if (a == 7 || a == 8 || a == 10 || a == 13) {
			beatsize = exp2f( (float) (log2f((float) c) - log2f((float) m)));
		}
		else if (a == 3 || a == 5 || a == 6 || a == 9 || a == 11 || a == 12) {
			beatsize = exp2f( (float) (log2f((float) c) - log2f((float) m)));
		//	beatsize = (beatsize / 2);
		}
		else {
			beatsize = exp2f( (float) (log2f((float) c) - log2f((float) m)));
			beatsize = (beatsize / 2);
		}
	}
	else beatsize = exp2f( (float) (log2f((float) c) - log2f((float) a)));
	
	if (beatsize < 2) beatsize = 2;
	
//	if (beatsize == 16) beatsize = 8;
	
	return (beatsize);
}
int pow2test(int a) 						{
	if ((a & (a - 1)) == 0) {return 1;}
	else 					{return 0;}
}
int pow2dottest(int a) 						{
	int b;
	b = (int) ((a / 3) * 2);
	if (((b/2)*3) != a)	{return 0;}
	else {
		if ((b & (b - 1)) == 0) {return 1;}
		else 					{return 0;}
	}
}
int pow2ordot(int a) 						{
	int t1, t2;
	if ((a & (a - 1)) == 0) {t1 = 1;}
	else 					{t1 = 0;}
	int b;
	b = (int) ((a / 3) * 2);
	if (((b/2)*3) != a)		{t2 = 0;}
	else {
		if ((b & (b - 1)) == 0) {t2 = 1;}
		else 					{t2 = 0;}
	}
	t1 = t1+t2;
	if (t1>0) 	{return 1;}
	else 		{return 0;}
}
void find_relative(int a, FILE *f) 			{
	int temp;
	temp = (int) a - 48;
//	fprintf(f, " ");
//	post("find_relative function, float = %d", temp);
	if		(temp >=  55 && temp <=  66)	{	fprintf(f, "\\relative c''''' \n\{\n\n");	} 
	else if	(temp >=  43 && temp <=  54)	{	fprintf(f, "\\relative c'''' \n\{\n\n");	} 
	else if	(temp >=  31 && temp <=  42)	{	fprintf(f, "\\relative c''' \n\{\n\n");		} 
	else if	(temp >=  19 && temp <=  30)	{	fprintf(f, "\\relative c'' \n\{\n\n");		}
	else if	(temp >=   7 && temp <=  18)	{	fprintf(f, "\\relative c' \n\{\n\n");		}
	else if	(temp >=  -6 && temp <=   6)	{	fprintf(f, "\\relative c \n\{\n\n");		}
	else if (temp >= -18 && temp <=  -7)	{	fprintf(f, "\\relative c, \n\{\n\n");		}
	else if (temp >= -30 && temp <= -18)	{	fprintf(f, "\\relative c,, \n\{\n\n");		}
	else if (temp >= -42 && temp <= -31)	{	fprintf(f, "\\relative c,,, \n\{\n\n");		}
	else if (temp >= -54 && temp <= -43)	{	fprintf(f, "\\relative c,,,, \n\{\n\n");	}
	else if (temp >= -66 && temp <= -55)	{	fprintf(f, "\\relative c,,,,, \n\{\n\n");	}
}
void find_pitch(float a, int b, char g[]) 	{
///////////////////////////// a = current note, b = accidentals 0 = is or  1= es 
///////////////////////////// g=note[] (char)
	int p;
	float f;
	f = (float) a;
	p = a;
//	post ("f1=%f", f);
//	post ("p =%d", p);
	f -= ((float) p);
//	post ("f2=%f", f);
	
	if (p < 0) {
		g[0] = 'r'; 
		g[1] = '\0';
	}
	else {
		p = p % 12; // see if it is a natural or altered note (# or b) // assign note name, c, d, etc.
		if		(p ==  0)	{	g[0] = 'c'; g[1] = '\0';	
				if	(b==0 && f==0.5) { g[0] = 'c'; g[1] = 'i'; g[2] = 'h'; g[3] = '\0';} 
				if	(b==1 && f==0.5) { g[0] = 'd'; g[1] = 'e'; g[2] = 's'; g[3] = 'e';	g[4] = 'h'; g[5] = '\0';}
		}
		else if (p ==  1)	{
			if (b == 0)			{ g[0] = 'c'; g[1] = 'i'; g[2] = 's'; g[3] = '\0';}
			if (b==1)			{ g[0] = 'd'; g[1] = 'e'; g[2] = 's'; g[3] = '\0';}
			if (b==0 && f==0.5) { g[0] = 'c'; g[1] = 'i'; g[2] = 's'; g[3] = 'i';	g[4] = 'h'; g[5] = '\0';} 
			if (b==1 && f==0.5) { g[0] = 'd'; g[1] = 'e'; g[2] = 'h'; g[3] = '\0';}
		}
		else if (p ==  2)	{	g[0] = 'd'; g[1] = '\0';						
				if	(b==0 && f==0.5) { g[0] = 'd'; g[1] = 'i'; g[2] = 'h'; g[3] = '\0';} 
				if	(b==1 && f==0.5) { g[0] = 'e'; g[1] = 'e'; g[2] = 's'; g[3] = 'e';	g[4] = 'h'; g[5] = '\0';}
		}
		else if (p ==  3)	{
			if (b==0)			{ g[0] = 'd'; g[1] = 'i'; g[2] = 's'; g[3] = '\0';}
			if (b==1)			{ g[0] = 'e'; g[1] = 'e'; g[2] = 's'; g[3] = '\0';}
			if (b==0 && f==0.5) { g[0] = 'd'; g[1] = 'i'; g[2] = 's'; g[3] = 'i';	g[4] = 'h'; g[5] = '\0';} 
			if (b==1 && f==0.5) { g[0] = 'e'; g[1] = 'e'; g[2] = 'h'; g[3] = '\0';}
		}
		else if (p ==  4)	{	g[0] = 'e'; g[1] = '\0';						
				if	(b==0 && f==0.5) { g[0] = 'e'; g[1] = 'i'; g[2] = 'h'; g[3] = '\0';} 
				if	(b==1 && f==0.5) { g[0] = 'f'; g[1] = 'e'; g[2] = 'h'; g[3] = '\0';} 
		}
		else if (p ==  5)	{	g[0] = 'f'; g[1] = '\0';						
				if	(b==0 && f==0.5) { g[0] = 'f'; g[1] = 'i'; g[2] = 'h'; g[3] = '\0';} 
				if	(b==1 && f==0.5) { g[0] = 'g'; g[1] = 'e'; g[2] = 's'; g[3] = 'e';	g[4] = 'h'; g[5] = '\0';}
		}
		else if (p ==  6)	{
			if (b==0)			{ g[0] = 'f'; g[1] = 'i'; g[2] = 's'; g[3] = '\0';}
			if (b==1)			{ g[0] = 'g'; g[1] = 'e'; g[2] = 's'; g[3] = '\0';}
			if (b==0 && f==0.5) { g[0] = 'f'; g[1] = 'i'; g[2] = 's'; g[3] = 'i';	g[4] = 'h'; g[5] = '\0';} 
			if (b==1 && f==0.5) { g[0] = 'g'; g[1] = 'e'; g[2] = 'h'; g[3] = '\0';}
		}
		else if (p ==  7)	{	g[0] = 'g'; g[1] = '\0';						
				if	(b==0 && f==0.5) { g[0] = 'g'; g[1] = 'i'; g[2] = 'h'; g[3] = '\0';} 
				if	(b==1 && f==0.5) { g[0] = 'a'; g[1] = 'e'; g[2] = 's'; g[3] = 'e';	g[4] = 'h'; g[5] = '\0';}
		}
		else if (p ==  8)	{
			if (b==0)			{ g[0] = 'g'; g[1] = 'i'; g[2] = 's'; g[3] = '\0';}
			if (b==1)			{ g[0] = 'a'; g[1] = 'e'; g[2] = 's'; g[3] = '\0';}
			if (b==0 && f==0.5) { g[0] = 'g'; g[1] = 'i'; g[2] = 's'; g[3] = 'i';	g[4] = 'h'; g[5] = '\0';} 
			if (b==1 && f==0.5) { g[0] = 'a'; g[1] = 'e'; g[2] = 'h'; g[3] = '\0';}
		}
		else if (p ==  9)	{	g[0] = 'a'; g[1] = '\0';						
				if	(b==0 && f==0.5) { g[0] = 'a'; g[1] = 'i'; g[2] = 'h'; g[3] = '\0';} 
				if	(b==1 && f==0.5) { g[0] = 'b'; g[1] = 'e'; g[2] = 's'; g[3] = 'e';	g[4] = 'h'; g[5] = '\0';}
		}
		else if (p == 10)	{
			if (b==0)			{ g[0] = 'a'; g[1] = 'i'; g[2] = 's'; g[3] = '\0';}
			if (b==1)			{ g[0] = 'b'; g[1] = 'e'; g[2] = 's'; g[3] = '\0';}
			if (b==0 && f==0.5) { g[0] = 'a'; g[1] = 'i'; g[2] = 's'; g[3] = 'i';	g[4] = 'h'; g[5] = '\0';} 
			if (b==1 && f==0.5) { g[0] = 'b'; g[1] = 'e'; g[2] = 'h'; g[3] = '\0';}
		}
		else				{	g[0] = 'b'; g[1] = '\0';						
				if	(b==0 && f==0.5) { g[0] = 'b'; g[1] = 'i'; g[2] = 'h'; g[3] = '\0';} 
				if	(b==1 && f==0.5) { g[0] = 'c'; g[1] = 'e'; g[2] = 'h'; g[3] = '\0';} 
		}
	}
}
int  find_jump(int a, int b, char g[]) 		{   
	int jmp, direction, i;
	int natural_pitches[250] = { 1, 1, 2, 2, 3, 4, 4, 5, 5, 6, 6, 7, 8, 8, 9, 9, 10, 11, 11, 12, 12, 13, 13, 14, 15, 15, 16, 16, 17, 18, 18, 19, 19, 20, 20, 21, 22, 22, 23, 23, 24, 25, 25, 26, 26, 27, 27, 28, 29, 29, 30, 30, 31, 32, 32, 33, 33, 34, 34, 35, 36, 36, 37, 37, 38, 39, 39, 40, 40, 41, 41, 42, 43, 43, 44, 44, 45, 46, 46, 47, 47, 48, 48, 49, 50, 50, 51, 51, 52, 53, 53, 54, 54, 55, 55, 56, 57, 57, 58, 58, 59, 60, 60, 61, 61, 62, 62, 63, 64, 64, 65, 65, 66, 67, 67, 68, 68, 69, 69, 70, 71, 71, 72, 72, 73, 74, 74, 75, 75, 76, 76, 77, 78, 78, 79, 79, 80, 81, 81, 82, 82, 83, 83, 84, 85, 85, 86, 86, 87, 88, 88, 89, 89, 90, 90, 91, 92, 92, 93, 93, 94, 95, 95, 96, 96, 97, 97, 98, 99, 99, 100, 100, 101, 102, 102, 103, 103, 104, 104, 105, 106, 106, 107, 107, 108, 109, 109, 110, 110, 111, 111, 112, 113, 113, 114, 114, 115, 116, 116 };
	int	pitch_current, pitch_previous, interval;
	i = 1;
	if (b < 0) b = abs(b);
	pitch_current 	= natural_pitches[abs(a)];
	pitch_previous 	= natural_pitches[abs(b)];
	jmp = a - b;
	if (jmp >= 0) direction = 1;
	else direction = 0;
//	post("JUMP:\traw\tp =%d\tp-1 =%d\tscale\tp =%d\tp-1 =%d\t", a, b, pitch_current, pitch_previous);
	if (a < 0) interval = 1;
	else {
		if (direction == 1) {
			interval = pitch_current - pitch_previous + 1;
			if (interval < 0) {
				interval = pitch_previous - pitch_current;
				interval = interval * -1 + 8;
			}
		}
		else {
			interval = pitch_previous - pitch_current + 1;
			if (interval < 0) {
				interval = pitch_current - pitch_previous;
				interval = interval * -1 + 8;
			}
			interval = interval * -1;
		}
	}
	if		(interval 					 < -18) 	{g[0] = ','; g[1] = ','; g[2] = ','; g[3] = '\0';}
	else if	(interval >= -18 && interval < -11) 	{g[0] = ','; g[1] = ','; g[2] = '\0';}
	else if	(interval >= -11 && interval <  -4) 	{g[0] = ','; g[1] = '\0';}
	else if (interval >=  -4 && interval <=  4) 		{i = 0; g[0] = '\0';}
	else if (interval >=   4 && interval <= 11) 	{g[0] = '\''; g[1] = '\0';}
	else if	(interval >   11 && interval <= 18) 	{g[0] = '\''; g[1] = '\''; g[2] = '\0';}	
	else if	(interval >   18				  ) 	{g[0] = '\''; g[1] = '\''; g[2] = '\''; g[3] = '\0';}
	
	return(i);
} 
void find_clef(int a, FILE *f) 				{
	if		(a==0)	fprintf(f, "\\clef treble "); // 
	else if	(a==1)	fprintf(f, "\\clef alto "); //  
	else if	(a==2)	fprintf(f, "\\clef tenor "); //, 
	else if	(a==3)	fprintf(f, "\\clef bass "); //, 
	else if	(a==4)	fprintf(f, "\\clef french "); //
	else if	(a==5)	fprintf(f, "\\clef soprano "); // , 
	else if	(a==6)	fprintf(f, "\\clef mezzosoprano "); // 
	else if	(a==7)	fprintf(f, "\\clef baritone "); // 
	else if	(a==8)	fprintf(f, "\\clef varbaritone "); // 
	else if	(a==9)	fprintf(f, "\\clef percussion "); //
}
void find_articulation(int a, FILE *f) 		{
	if		(a==0)	fprintf(f, "\\accent"); // 
	else if	(a==1)	fprintf(f, "\\espressivo"); //  
	else if	(a==2)	fprintf(f, "\\marcato"); //, 
	else if	(a==3)	fprintf(f, "\\portato"); //, 
	else if	(a==4)	fprintf(f, "\\staccatissimo"); //
	else if	(a==5)	fprintf(f, "\\staccato"); // 
	else if	(a==6)	fprintf(f, "\\tenuto"); //
	else if	(a==7)	fprintf(f, "\\shortfermata"); // 
	else if	(a==8)	fprintf(f, "\\fermata"); // 
	else if	(a==9)	fprintf(f, "\\longfermata"); //
	else if	(a==10)	fprintf(f, "\\verylongfermata"); //
	else if	(a==11)	fprintf(f, "\\upbow"); //
	else if	(a==12)	fprintf(f, "\\downbow"); //
	else if	(a==13)	fprintf(f, "\\flageolet"); //
	else if	(a==14)	fprintf(f, "\\thumb"); //
	else if	(a==15)	fprintf(f, "\\snappizzicato"); //
	else if	(a==16)	fprintf(f, "\\open"); //
	else if	(a==17)	fprintf(f, "\\halfopen"); //
	else if	(a==18)	fprintf(f, "\\stopped"); //
	else if	(a==19)	fprintf(f, "\\lheel"); //
	else if	(a==20)	fprintf(f, "\\rheel"); //
	else if	(a==21)	fprintf(f, "\\ltoe"); //
	else if	(a==22)	fprintf(f, "\\rtoe"); //
	else if	(a==23)	fprintf(f, "\\laissezVibrer"); //,
	else if	(a==24)	fprintf(f, " \\xNote "); //,
	else if	(a==25)	fprintf(f, "\\harmonic"); //,		
	else if	(a==26)	fprintf(f, "\\trill"); //
	else if	(a==27)	fprintf(f, "\\prall"); //
	else if	(a==28)	fprintf(f, "\\mordent"); //
	else if	(a==29)	fprintf(f, "\\prallmordent"); //
	else if	(a==30)	fprintf(f, "\\upprall"); //, 
	else if	(a==31)	fprintf(f, "\\downprall"); //, 
	else if	(a==32)	fprintf(f, "\\upmordent"); //, 
	else if	(a==33)	fprintf(f, "\\downmordent"); //, 
	else if	(a==34)	fprintf(f, "\\lineprall"); //, 
	else if	(a==35)	fprintf(f, "\\prallprall"); //, 
	else if	(a==36)	fprintf(f, "\\pralldown"); //, 
	else if	(a==37)	fprintf(f, "\\prallup"); //, 
	else if	(a==38)	fprintf(f, "\\turn"); //,
	else if	(a==39)	fprintf(f, "\\reverseturn"); //,  
	else if	(a==40)	fprintf(f, "\\segno"); //		
	else if	(a==41)	fprintf(f, "\\coda"); //		
	else if	(a==42)	fprintf(f, "\\varcoda"); //
	else if	(a==43)	fprintf(f, "\\breathe"); //
	else if	(a==44)	fprintf(f, "\\bendAfter #+4"); //
	else if	(a==45)	fprintf(f, "\\bendAfter #-4"); //
	else if	(a==46)	fprintf(f, "-\\markup{pizz.}"); //
	else if	(a==47)	fprintf(f, "-\\markup{arco}"); //
	else if	(a==48)	fprintf(f, "-\\markup{vib.}"); //
	else if	(a==49)	fprintf(f, "-\\markup{senza vib.}"); //	
//	else if	(a==50)	fprintf(f, "-\\markup{senza vib.}"); //			
}
void find_dynamics(int a, FILE *f) 			{
	if		(a==0)	fprintf(f, "\\<"); // 
	else if	(a==1)	fprintf(f, "\\>"); // 
	else if	(a==2)	fprintf(f, "\\!"); // 
	else if	(a==3)	fprintf(f, "\\ppppp"); // 
	else if	(a==4)	fprintf(f, "\\pppp"); // 
	else if	(a==5)	fprintf(f, "\\ppp"); // 
	else if	(a==6)	fprintf(f, "\\pp"); // 
	else if	(a==7)	fprintf(f, "\\p"); // 
	else if	(a==8)	fprintf(f, "\\mp"); // 
	else if	(a==9)	fprintf(f, "\\mf"); // 
	else if	(a==10)	fprintf(f, "\\f"); // 
	else if	(a==11)	fprintf(f, "\\ff"); // 
	else if	(a==12)	fprintf(f, "\\fff"); // 
	else if	(a==13)	fprintf(f, "\\ffff"); // 
	else if	(a==14)	fprintf(f, "\\fffff"); //
	else if	(a==15)	fprintf(f, "\\fp");
	else if	(a==16)	fprintf(f, "\\sf");
	else if	(a==17)	fprintf(f, "\\sff");
	else if	(a==18)	fprintf(f, "\\sp");
	else if	(a==19)	fprintf(f, "\\spp");
	else if	(a==20)	fprintf(f, "\\sfz");
	else if	(a==21)	fprintf(f, "\\rfz");
	else if (a>=22) post("notes: Warning: Only dynamics 0 through 21 are predefined");
}
void find_small_numbers(int a, FILE *f) 	{
	fprintf(f, "-%i", a);
}
void find_span(int a, FILE *f) 				{
	if		(a==0)	fprintf(f, "\\glissando"); // GLISSANDO
	else if	(a==1)	fprintf(f, "\\sustainOn"); //  SUSTAIN
	else if	(a==2)	fprintf(f, "\\sustainOff"); //  STOP SUSTAIN
	else if	(a==3)	fprintf(f, "("); //, SLUR 
	else if	(a==4)	fprintf(f, ")"); //, STOP SLUR
	else if	(a==5)	fprintf(f, "\\startTrillSpan"); // TRILL SPAN
	else if	(a==6)	fprintf(f, "\\stopTrillSpan"); //STOP TRILL SPAN
/*	else if	(a==7)	fprintf(f, "\\shortfermata"); // 
	else if	(a==8)	fprintf(f, "\\fermata"); // 
	else if	(a==9)	fprintf(f, "\\longfermata"); //
	else if	(a==10)	fprintf(f, "\\verylongfermata"); //
	else if	(a==11)	fprintf(f, "\\upbow"); //
	else if	(a==12)	fprintf(f, "\\downbow"); //
	else if	(a==13)	fprintf(f, "\\flageolet"); //
	else if	(a==14)	fprintf(f, "\\thumb"); //
	else if	(a==15)	fprintf(f, "\\snappizzicato"); //
	else if	(a==16)	fprintf(f, "\\open"); //		*/		
}
void find_stafftext(int a, char g[]) 		{
	int n, total;
	char fst[6];
	total = a;
	if (total < 10) {
		n = total;
		if		(n==0) strcpy( fst, "zero");	
		else if	(n==1) strcpy( fst, "one");
		else if (n==2) strcpy( fst, "two");
		else if (n==3) strcpy( fst, "three");
		else if (n==4) strcpy( fst, "four");
		else if (n==5) strcpy( fst, "five");
		else if (n==6) strcpy( fst, "six");
		else if (n==7) strcpy( fst, "seven");
		else if (n==8) strcpy( fst, "eight");
		else if (n==9) strcpy( fst, "nine");
	}
	else {
		n = (int) ((float) total / 10.);
		if		(n==0) strcpy( fst, "zero");	
		else if	(n==1) strcpy( fst, "one");
		else if (n==2) strcpy( fst, "two");
		else if (n==3) strcpy( fst, "three");
		else if (n==4) strcpy( fst, "four");
		else if (n==5) strcpy( fst, "five");
		else if (n==6) strcpy( fst, "six");
		else if (n==7) strcpy( fst, "seven");
		else if (n==8) strcpy( fst, "eight");
		else if (n==9) strcpy( fst, "nine");
		n = total - (n*10);
		if		(n==0) strcat( fst, "zero");	
		else if	(n==1) strcat( fst, "one");
		else if (n==2) strcat( fst, "two");
		else if (n==3) strcat( fst, "three");
		else if (n==4) strcat( fst, "four");
		else if (n==5) strcat( fst, "five");
		else if (n==6) strcat( fst, "six");
		else if (n==7) strcat( fst, "seven");
		else if (n==8) strcat( fst, "eight");
		else if (n==9) strcat( fst, "nine");
	}	/*	if		(n==0) { g[0]='z';g[1]='e';g[2]='r';g[3]='o';g[4]='\0';} 	
	else if	(n==1) { g[0]='o';g[1]='n';g[2]='e';g[3]='\0';} 
	else if (n==2) { g[0]='t';g[1]='w';g[2]='o';g[3]='\0';} 
	else if (n==3) { g[0]='t';g[1]='h';g[2]='r';g[3]='e';g[4]='e';g[5]='\0';}
	else if (n==4) { g[0]='f';g[1]='o';g[2]='u';g[3]='r';g[4]='\0';}
	else if (n==5) { g[0]='f';g[1]='i';g[2]='v';g[3]='e';g[4]='\0';}
	else if (n==6) { g[0]='s';g[1]='i';g[2]='x';g[3]='\0';} 
	else if (n==7) { g[0]='s';g[1]='e';g[2]='v';g[3]='e';g[4]='n';g[5]='\0';}
	else if (n==8) { g[0]='e';g[1]='i';g[2]='g';g[3]='h';g[4]='t';g[5]='\0';}
	else if (n==9) { g[0]='n';g[1]='i';g[2]='n';g[3]='e';g[4]='\0';}*/
	strcpy (g, fst);
}
int readbarfile(int a[][8], FILE *f)		{
	int i, ii, j, jj, strsize, temp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	char ss[10];

	ii=0;
	while ((read = getline(&line, &len, f)) != -1) {
		jj 		= 4;
		strsize = (int) read;
		for (i=strsize-1; i>=0; i--){
			if 		( line[i] == (int) 32 || line[i] == (int) 10) {
				if(i != (strsize-1)) {	
					a[ii][jj] = temp;
					jj--;
				}
				j=0;
				temp=0;
			}
			else	 {
				ss[0] = line[i];
				temp += atoi(ss)*( (int) pow((double)10, (double)j) );
				j++;
			}	
		}
		ii++;
	}
	return (ii);	
}
void copyfiles(FILE *f, FILE *g)			{
	char a;
	post("copyfiles");
	a = fgetc(g);
	if (a != EOF) fputc(a,  f);
	while(a != EOF)	{
		post("copyfiles");
		a = fgetc(g);
		if (a != EOF) fputc(a,  f);
	}
} // f1 into f2 

////	____________________________________________________ MAIN STRUCT
typedef struct notes 														{
  t_object x_ob;
  t_outlet *x_outlet0, *x_outlet1, *x_outlet2;
  t_canvas *x_canvas;
  FILE *fp1;
  
////////////////// ******* RAW INPUT DATA:
// pitch, duration, articulation, tie, dynamics, articulation, tempo, meter, 
// add rehearsal marks, tremolo

  int ri_index;
  int ri_cho[MXS], ri_dur[MXS], ri_tie[MXS], ri_clef[MXS], ri_acc[MXS], ri_trm[MXS];
  int ri_dyn_n[MXS], ri_art_n[MXS], ri_smn_n[MXS], ri_spa_n[MXS], ri_txt_n[MXS];//, ri_txt_cmd_n[MXS];
  float ri_pit[MXS][128];
  int ri_mtr[MXS][2], ri_tmp[MXS][2], ri_tup[MXS][3], ri_dyn[MXS][16], ri_art[MXS][16], ri_smn[MXS][8], ri_spa[MXS][8];
  char ri_txt[MXS][32][64];
  
////////////////// ******* CLEAN INPUT DATA:
  int i_index, i_rii[MXS];
  int i_cho[MXS], i_dur[MXS], i_mtr[MXS][2], i_tup[MXS], i_tie[MXS];
  
////////////////// ******* TUPLET DATA:
  int tp_info[MXS][5], tp_n;
  int tp_index, tp_dur[MXS], tp_i[MXS], tp_ri[MXS], tp_tie[MXS], tp_num[MXS];
  int b_tp_index, b_tp_dur[MXS], b_tp_i[MXS], b_tp_ri[MXS], b_tp_tie[MXS], b_tp_num[MXS];
  int sb_tp_index, sb_tp_dur[MXS], sb_tp_i[MXS], sb_tp_ri[MXS], sb_tp_tie[MXS], sb_tp_num[MXS];
  int sb_tp_start[MXS], sb_tp_end[MXS];
  
////////////////// ******* BAR DATA:
  int b_index, bar_n[MXS], b_tie[MXS], b_dur[MXS], b_i[MXS], b_mtr[MXS][2], b_tmp[MXS][2], b_rii[MXS], b_tup[MXS];
  int bar_info[MXS][8]; // 0 = initial position, 1 = num, 2 = den
////////////////// ******* BEAT DATA:
  int be_index, be_bar_n[MXS], be_beat_n[MXS], be_tie[MXS], be_dur[MXS], be_i[MXS], be_rii[MXS], be_tup[MXS];
////////////////// ******* SUB-BEAT DATA:
  int sb_index, sb_bar_n[MXS], sb_beat_n[MXS], sb_tie[MXS], sb_dur[MXS], sb_i[MXS], sb_rii[MXS], sb_tup[MXS];
  
  char title[64][130], sub_title[64][130], author[64][130], osname[130], lily_dir[130], *dummysym, barfile[150], inst[150];
  int ii, refdur, debug, auth_n, titl_n, sub_title_n, tempo, OS, lastpit_ch, lastpit;
  int SLAVE, inst_n, papersize, paperorientation, render;
} 
t_notes;
////	____________________________________________________ INPUT
void notes_input(t_notes *x, t_symbol *s, int argcount, t_atom *argvec) {
	int a, i, j, input_check[20];	
	float temp_f, temp_f_array[128];
	x->dummysym = s->s_name;
	
	t_symbol *nt_pitch 			= gensym("pit"); // float = single note & list = chord
	t_symbol *nt_rhythm 		= gensym("dur"); // float = duration
	t_symbol *nt_dynamics 		= gensym("dyn"); // float = dynamics list (0 = nothing, 1 = pppp, etc..)
	t_symbol *nt_tuplet 		= gensym("tup"); // 1 3 4 (3 1's in the place of 4)?
	t_symbol *nt_meter	 		= gensym("mtr"); // meter num, denom
	t_symbol *nt_articulation	= gensym("art"); // articulation, n
	t_symbol *nt_tempo			= gensym("tmp"); // tempo 32 = msec
	t_symbol *nt_small_numbers	= gensym("smn"); // small numbers, mostly for fingering.
	t_symbol *nt_clef			= gensym("clf"); // Clef changes.
	t_symbol *nt_accidentals	= gensym("acc"); // accidentals.
	t_symbol *nt_tremolo		= gensym("trm"); // tremolo.
	t_symbol *nt_span			= gensym("spa"); // tremolo.
	t_symbol *nt_text			= gensym("txt"); // just text...
//	t_symbol *nt_command		= gensym("cmd"); // just text..
	
	if (x->debug >= 1) post("INDEX ========= %d", x->ri_index);
	for (a = 0; a < 20; a++) { input_check[a] = 0;	};
    for (a = 0; a < argcount; a++) {
    	if (x->debug >= 1) post("notes: Input index %d received", x->ri_index);
    	if (argvec[a].a_type == A_SYMBOL) {
//_____________________________________________________________________________
	    	if 		( argvec[a].a_w.w_symbol == nt_pitch)			{
	    		i = 0;
	    		while (argvec[a+1].a_type == A_FLOAT && a < argcount-1) {
	    			a++; 
	    			x->ri_pit[x->ri_index][i] 		= argvec[a].a_w.w_float;
	    		//	post("pitch =\t%f", x->ri_pit[x->ri_index][i]);
	    			i++;
	    		}
	    		x->ri_cho[x->ri_index] = i;
	    		if (x->debug >= 1) post("chord size = %d", x->ri_cho[x->ri_index]);
	    		input_check[0] = 1;
	    		for (j=0; j<x->ri_cho[x->ri_index]; j++) {
	    			temp_f = 1000;
					for (i=0; i<x->ri_cho[x->ri_index]; i++) {
						if (j==0) {
							if (x->ri_pit[x->ri_index][i] <= temp_f) {
								temp_f = x->ri_pit[x->ri_index][i];
							}
						}
						else {
							if (x->ri_pit[x->ri_index][i] < temp_f && 
								x->ri_pit[x->ri_index][i] > temp_f_array[j-1]) {
								temp_f = x->ri_pit[x->ri_index][i];
							}
						}
					}
					temp_f_array[j] = temp_f;
					if (x->debug >= 1) post("o.pitch =\t%f", temp_f_array[j]); 
				}
				for (j=0; j<x->ri_cho[x->ri_index]; j++) {
					x->ri_pit[x->ri_index][j]	= temp_f_array[j];	
				}
	    	}
//_____________________________________________________________________________
	    	else if ( argvec[a].a_w.w_symbol == nt_rhythm)			{ 
	    		while (argvec[a+1].a_type == A_FLOAT && a < argcount-1) {
	    			a++;
	    			x->ri_dur[x->ri_index]	 	= argvec[a].a_w.w_float;
	    			if ( x->ri_dur[x->ri_index] == 0) {
	    				post("notes: ERROR: durations of 0 are not allowed, changing it to last duration");
	    				x->ri_dur[x->ri_index] = 0;
	    				input_check[1] = 0;
	    			}
	    			else {
	    			//	post("duration = %d", x->ri_dur[x->ri_index]);
	    				input_check[1] = 1;
	    			}
	    		}
	    		
	    	}
//_____________________________________________________________________________
	    	else if ( argvec[a].a_w.w_symbol == nt_dynamics)		{ 
	    		i = 0;
	    		while (argvec[a+1].a_type == A_FLOAT && a < argcount-1) {
	    			a++;
	    			x->ri_dyn[x->ri_index][i]	 	= argvec[a].a_w.w_float;
	    			i++;
	    		}
	    		x->ri_dyn_n[x->ri_index] = i;
	    		if (x->debug >= 1) post("dynamics = %d", x->ri_dyn[x->ri_index]);
	    		input_check[2] = 1;
	    	}
//_____________________________________________________________________________
	    	else if ( argvec[a].a_w.w_symbol == nt_tuplet)			{
	    		i = 0;
	    		while (argvec[a+1].a_type == A_FLOAT && a < argcount-1) {
	    			a++; 
	    			x->ri_tup[x->ri_index][i] 		= argvec[a].a_w.w_float;
	    			i++;
	    		}
	    		if (i != 3) post("notes: ERROR: not enough arguments for tuplet (need 3... see help file)");
	    		if (x->debug >= 1) post("tuplet = %d %d %d", x->ri_tup[x->ri_index][0], x->ri_tup[x->ri_index][1], x->ri_tup[x->ri_index][2]);
	    		input_check[3] = 1;
	    	}
//_____________________________________________________________________________
	    	else if ( argvec[a].a_w.w_symbol == nt_meter)			{
	    		i = 0;
	    		while (argvec[a+1].a_type == A_FLOAT && a < argcount-1) {
	    			a++; 
	    			x->ri_mtr[x->ri_index][i] 		= argvec[a].a_w.w_float;
	    			i++;	
	    		}
	    		if (i != 2) {
	    			post("notes: ERROR: not enough arguments for meter (need 2... see help file)");
	    			input_check[4] = 0;
	    		}
	    		else {
	    			if (x->debug >= 1) post("meter = %d / %d", x->ri_mtr[x->ri_index][0], x->ri_mtr[x->ri_index][1]);
	    			input_check[4] = 1;
	    		}
	    	}
//_____________________________________________________________________________
	    	else if ( argvec[a].a_w.w_symbol == nt_articulation)	{ 
	    		i = 0;
	    		while (argvec[a+1].a_type == A_FLOAT && a < argcount-1) {
	    			a++;
	    			x->ri_art[x->ri_index][i] 		= argvec[a].a_w.w_float;
	    			i++;
	    		}
	    		x->ri_art_n[x->ri_index] = i;
	    		if (x->debug >= 1) post("articulation = %d", x->ri_art_n[x->ri_index]);
	    		input_check[5] = 1;
	    	}
//_____________________________________________________________________________
	    	else if ( argvec[a].a_w.w_symbol == nt_tempo)			{
	    		i = 0;
	    		while (argvec[a+1].a_type == A_FLOAT && a < argcount-1) {
	    			a++; 
	    			x->ri_tmp[x->ri_index][i] 		= argvec[a].a_w.w_float;
	    			i++;	
	    		}
	    		if (i != 2) {
	    			post("notes: ERROR: not enough arguments for tempo (need 2... see help file)");
	    			input_check[6] = 0;
	    		}
	    		else {
					if (x->debug >= 1) post("tempo: %d = %d", x->ri_tmp[x->ri_index][0], x->ri_tmp[x->ri_index][1]);
					input_check[6] = 1;
					x->tempo = 1;
				}
	    	}
//_____________________________________________________________________________
	    	else if ( argvec[a].a_w.w_symbol == nt_small_numbers)	{
	    		 
	    		i = 0;
	    		while (argvec[a+1].a_type == A_FLOAT && a < argcount-1) {
	    			a++;
	    			x->ri_smn[x->ri_index][i] 		= argvec[a].a_w.w_float;
	    			if (x->debug >= 1) post("small number = %d", x->ri_smn[x->ri_index][i]);
	    			i++;
	    		}
	    		x->ri_smn_n[x->ri_index] = i;
	    		
	    		input_check[7] = 1;
	    	}
//_____________________________________________________________________________
	    	else if ( argvec[a].a_w.w_symbol == nt_clef)			{ 
	    		i = 0;
	    		while (argvec[a+1].a_type == A_FLOAT && a < argcount-1) {
	    			a++;
	    			x->ri_clef[x->ri_index] 		= argvec[a].a_w.w_float;
	    		}
	    		if (x->debug >= 1) post("clef = %d", x->ri_clef[x->ri_index]);
	    		input_check[8] = 1;
	    	}
//_____________________________________________________________________________
	    	else if ( argvec[a].a_w.w_symbol == nt_accidentals)		{ 
	    		i = 0;
	    		while (argvec[a+1].a_type == A_FLOAT && a < argcount-1) {
	    			a++;
	    			x->ri_acc[x->ri_index] 		= argvec[a].a_w.w_float;
	    		}
	    		if (x->ri_acc[x->ri_index] < 0) x->ri_acc[x->ri_index] = 0;
	    		else if (x->ri_acc[x->ri_index] > 1) x->ri_acc[x->ri_index] = 1;
	    		if (x->debug >= 1) post("accidentals = %d", x->ri_acc[x->ri_index]);
	    		input_check[9] = 1;
	    	}
//_____________________________________________________________________________
	    	else if ( argvec[a].a_w.w_symbol == nt_tremolo)			{ 
	    		i = 0;
	    		while (argvec[a+1].a_type == A_FLOAT && a < argcount-1) {
	    			a++;
	    			x->ri_trm[x->ri_index] 		= argvec[a].a_w.w_float;
	    		}
	    		i=x->ri_trm[x->ri_index];
	    		if(i==128||i==64||i==32||i==16||i==8) {
	    			if (x->debug >= 1) post("tremolo = %d", x->ri_acc[x->ri_index]);
	    			input_check[10] = 1;
	    		}
	    		else {
	    			post("notes: ERROR: tremolo is %d, but it cannot be a different number than 16, 32, 64, etc...", x->ri_acc[x->ri_index]);
	    			post("notes: changing tremolo to default of 32");
	    			x->ri_acc[x->ri_index] = 32;
	    			input_check[10] = 1;
	    		}
	    	}
//_____________________________________________________________________________
	    	else if ( argvec[a].a_w.w_symbol == nt_span)			{ 
	    		i = 0;
	    		while (argvec[a+1].a_type == A_FLOAT && a < argcount-1) {
	    			a++;
	    			x->ri_spa[x->ri_index][i] 		= argvec[a].a_w.w_float;
	    			if (x->debug >= 1) post("span=%d", (int) x->ri_spa[x->ri_index][i]);
	    			i++;
	    		}
	    		x->ri_spa_n[x->ri_index] = i;
				input_check[11] = 1;
	    	}
//_____________________________________________________________________________
	    	else if ( argvec[a].a_w.w_symbol == nt_text)			{
	    		i = 0;
	    		while (argvec[a+1].a_type == A_SYMBOL && a < argcount-1 && i < 32) {
					a++;
					atom_string(argvec+a, x->ri_txt[x->ri_index][i], 1000);
					if (x->debug >= 1) post(x->ri_txt[x->ri_index][i]);
					i++;
				}
				x->ri_txt_n[x->ri_index] = i; // 
				if (x->debug >= 1) post("text n = %d", x->ri_txt_n[x->ri_index]);
	    	}
//_____________________________________________________________________________
/*	    	else if ( argvec[a].a_w.w_symbol == nt_command)			{
	    		i = 0;
	    		while (argvec[a+1].a_type == A_SYMBOL && a < argcount-1 && i < 32) {
					a++;
					atom_string(argvec+a, x->ri_txt_cmd[x->ri_index][i], 1000);
					post(x->ri_txt_cmd[x->ri_index][i]);
					i++;
				}
				x->ri_txt_cmd_n[x->ri_index] = i; // 
				post("text_cmd n = %d", x->ri_txt_cmd_n[x->ri_index]);
	    	}*/
	    }
	    else if (argvec[a].a_type == A_FLOAT) post("this float escaped: %f", argvec[a].a_w.w_float); 
	}
	if (x->debug >= 1) post("INPUT CHECKs");
    ////CHECK INPUT
	if (input_check[0] == 0) 	{ // if no pitch value is provided then the previous one is kept.
		if (x->ri_index == 0) {
			x->ri_pit[x->ri_index][0] = 60;
			x->ri_cho[x->ri_index] 	= 1;
		}
		else {
			for (i = 0; i < x->ri_cho[x->ri_index - 1]; i++) {
				x->ri_pit[x->ri_index][i] = x->ri_pit[x->ri_index - 1][i];
			}
			x->ri_cho[x->ri_index] 	= x->ri_cho[x->ri_index - 1];
		}
	} // Pitch
	if (input_check[1] == 0) 	{ // if no rhythmic value is provided then the previous one is kept.
		if 	(x->ri_index == 0)	x->ri_dur[x->ri_index]	= 8;
		else					x->ri_dur[x->ri_index] 	= x->ri_dur[x->ri_index - 1];
	} // Duration	
	if (input_check[2] == 0) 	{
		x->ri_dyn_n[x->ri_index]	= 0;
	} // Dynamics
	if (input_check[3] == 0) 	{ 
		x->ri_tup[x->ri_index][0] = x->ri_tup[x->ri_index][1] = x->ri_tup[x->ri_index][2] = 0;
	} // Tuplet
	if (input_check[4] == 0) 	{ 
		if (x->ri_index == 0) {
			x->ri_mtr[x->ri_index][0] = 4;
			x->ri_mtr[x->ri_index][1] = 4;
		}
		else {
			x->ri_mtr[x->ri_index][0] = x->ri_mtr[x->ri_index-1][0];
			x->ri_mtr[x->ri_index][1] = x->ri_mtr[x->ri_index-1][1];
		}	
	} // Meter
	if (input_check[5] == 0) 	{ 
		x->ri_art_n[x->ri_index] = 0;
	} // Articulation
	if (input_check[6] == 0) 	{ 
		if (x->ri_index == 0) {
			x->ri_tmp[x->ri_index][0] = 8;
			x->ri_tmp[x->ri_index][1] = 60;
		}
		else {
			x->ri_tmp[x->ri_index][0] = x->ri_tmp[x->ri_index-1][0];
			x->ri_tmp[x->ri_index][1] = x->ri_tmp[x->ri_index-1][1];
		}
	//	post("wrote tempo");
	} // Tempo
	if (input_check[7] == 0) 	{ 
		x->ri_smn_n[x->ri_index] = 0;
	//	post("no small numbers");
	} // small numbers
	if (input_check[8] == 0) 	{ 
		if (x->ri_index == 0) {
			x->ri_clef[x->ri_index] = 0;
		}
		else {
			x->ri_clef[x->ri_index] = x->ri_clef[x->ri_index-1];
		}
	//	post("wrote clef");
	} // Clef
	if (input_check[9] == 0) 	{ 
		if (x->ri_index == 0) {
			x->ri_acc[x->ri_index] = 0;
		}
		else {
			x->ri_acc[x->ri_index] = x->ri_acc[x->ri_index-1];
		}
	//	post("wrote clef");
	} // accidentals
	if (input_check[10] == 0) 	{ 
			x->ri_trm[x->ri_index] = 0;
	//	post("wrote tremolo");
	} // tremolo
	if (input_check[11] == 0) 	{ 
		x->ri_spa_n[x->ri_index] = 0;
	} // Spans
	x->ri_index++;
}
////	____________________________________________________ WRITE
void notes_write(t_notes *x, t_symbol *s)								{
	int i, j, k, barcount, barsize, beatsize, dur_remainder, bar_number, beatcount, beat_number, sub_beatsize, subdiv;
	int pow2dot_array[18] = {0, 1, 2, 3, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96, 128, 192, 256, 384};
	float temp_f;
	bar_number = beatcount = beat_number = j = 0;
	x->sb_tp_index = x->i_index = x->tp_index = x->tp_n = x->b_index = x->be_index = x->sb_index = 0;
	
	if (x->ri_index > 0) 					{	
		post("notes: Generating Score ...");
	//// ____________________________________________________________ INPUT SORTING
		if (x->debug >= 1) 				{
			post("\nRAW INPUT LIST :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
			post("i\tdur\ttup\ttup\ttup\tmtr\tmtr");
		 }
		for(i=0; i<x->ri_index; i++) 	{
			if (x->ri_cho[i] == 1 && x->ri_pit[i][0] < 0){
				temp_f = -1000;
				for (j=i; j>0; j--) {
					if (x->ri_pit[j][0] > 0) temp_f = x->ri_pit[j][0];
				}
				if (temp_f == -1000) {
					for (j=i; j<x->ri_index; j++) {
						if (x->ri_pit[j][0] > 0) temp_f = x->ri_pit[j][0];
					}
				}
				if (temp_f == -1000) post("notes: all pitches are negative you should know...");
				else x->ri_pit[x->ri_index][0] = temp_f;
			}
			if (x->debug >= 1) post("%d\t%d\t%d\t%d\t%d\t%d\t%d\t rawrawraw1", i, (int) x->ri_dur[i], x->ri_tup[i][0], x->ri_tup[i][1], x->ri_tup[i][2], x->ri_mtr[i][0], x->ri_mtr[i][1]);
		}
		post("notes: Sorting Input ...");
		if (x->debug >= 1) 				{
			post("\nSORTING INPUT :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
		}
		x->tp_index = x->tp_n = 0;
		for(i=0; i<x->ri_index; i++) 	{
			if (x->ri_tup[i][0] != 0) {
				x->tp_info[x->tp_n][0] = x->ri_tup[i][0]; // reference beat duration
				x->tp_info[x->tp_n][1] = x->ri_tup[i][1]; // fit
				x->tp_info[x->tp_n][2] = x->ri_tup[i][2]; // into
				x->tp_info[x->tp_n][3] = x->tp_index; // starting tp index			
		
				x->i_mtr[x->i_index][0] = x->ri_mtr[i][0];
				x->i_mtr[x->i_index][1] = x->ri_mtr[i][1];
				x->i_tup[x->i_index]	= x->tp_n;
				x->i_dur[x->i_index]	= x->ri_tup[i][0] * x->ri_tup[i][2];
				x->i_tie[x->i_index]	= 0;
				dur_remainder 			= x->ri_tup[i][0] * x->ri_tup[i][1];
				x->i_rii[x->i_index]	= i; 
			
				x->tp_i[x->tp_index]	= x->i_index; // reference to clean/precessed input or just "input"
				x->tp_ri[x->tp_index]	= i; // reference to raw input indexes
				x->tp_dur[x->tp_index]	= x->ri_dur[i]; 
				x->tp_num[x->tp_index]	= x->tp_n;
				beatcount 				= x->tp_dur[x->tp_index];
				//post("a");
				x->tp_index++; 
			
				while (  (beatcount + x->ri_dur[i+1]) <= dur_remainder && i < x->ri_index && beatcount-dur_remainder !=0) {
					i++;
					x->tp_i[x->tp_index]	= x->i_index; // reference to clean/precessed input or just "input"
					x->tp_ri[x->tp_index]	= i; // reference to raw input indexes
					x->tp_dur[x->tp_index]	= x->ri_dur[i]; 
					x->tp_tie[x->tp_index]	= 0;
					x->tp_num[x->tp_index]	= x->tp_n;
					beatcount 				+= x->tp_dur[x->tp_index];
					x->tp_index++;
				//	post("b");
				}
				x->i_index++;
				if (x->debug >= 1) post("beatcount=%d\tdur_remainder=%d", beatcount, dur_remainder);
				if (beatcount < dur_remainder) {
					post("¡¡¡¡¡¡¡WARNING!!!!!! Tuplet no. %d has a longer duration than declared length, tied to regular beat", x->tp_n);
				
					i++;
					x->tp_i[x->tp_index]	= x->i_index; // reference to clean/precessed input or just "input"
					x->tp_ri[x->tp_index]	= i; // reference to raw input indexes
					x->tp_dur[x->tp_index]	= dur_remainder - beatcount;
					x->tp_tie[x->tp_index]	= 1;
					x->tp_num[x->tp_index]	= x->tp_n;
				
					x->i_mtr[x->i_index][0] = x->i_mtr[x->i_index-1][0];
					x->i_mtr[x->i_index][1] = x->i_mtr[x->i_index-1][1];
					x->i_dur[x->i_index]	= x->ri_dur[i] - x->tp_dur[x->tp_index];
					x->i_rii[x->i_index]	= i;
					x->i_tie[x->i_index]	= 1;
					x->i_tup[x->i_index]	= -1;
					//post("c");
					x->i_index++;
					x->tp_index++;	
				}
			
				x->tp_info[x->tp_n][4] = x->tp_index; // ending tp index	
				x->tp_n++;	
			}
			else {
				x->i_mtr[x->i_index][0] = x->ri_mtr[i][0];
				x->i_mtr[x->i_index][1] = x->ri_mtr[i][1];
				x->i_dur[x->i_index]	= x->ri_dur[i];
				x->i_rii[x->i_index]	= i;
				x->i_tup[x->i_index]	= -1;
				x->i_index++;
			}
		}
	//// ____________________________________________________________  TUPLET PROGRAM 
		if (x->debug >= 1) 				{
			post("\nRAW TUPLET INFO :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
			post("i\trefdur\tfit\tinto\tstart\tend");
		}
		for(i=0; i<x->tp_n; i++) 		{
			if (x->debug >= 1) post("%d\t%d\t%d\t%d\t%d\t%d", i, x->tp_info[i][0], x->tp_info[i][1], x->tp_info[i][2], x->tp_info[i][3], x->tp_info[i][4]);
		}
		if (x->debug >= 1) 				{
			post("\nRAW TUPLET LIST :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
			post("i\ttp_n\traw_i\tclean_i\tdur\ttie");
		}
		x->b_tp_index = x->tp_index;
		for(i=0; i<x->tp_index; i++) 	{
			if (x->tp_num[i] != x->tp_num[i-1]) post("-   -   -   -   -   -   -   -   -   -   -   -   ");
			if (x->debug >= 1) post("%d\t%d\t%d\t%d\t%d\t%d", i, x->tp_num[i], x->tp_ri[i], x->tp_i[i], (int) x->tp_dur[i], x->tp_tie[i]);
			x->b_tp_dur[i] 		= x->tp_dur[i];
			x->b_tp_ri[i]		= x->tp_ri[i];
			x->b_tp_i[i]		= x->tp_i[i];
			x->b_tp_tie[i]		= x->tp_tie[i];
			x->b_tp_num[i]		= x->tp_num[i];
		}
		if (x->debug >= 1) 				{
			post("\nSORTING TUPLET RHYTHMS :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
		}
		for (k=0; k<4; k++) 			{
			x->sb_tp_index = 0;
			beatcount = 0;
			subdiv = (int) exp2( (float) k );
			beatsize = x->tp_info[0][0];
			sub_beatsize = beatsize / subdiv;
			if (x->debug >= 3) post("beatsize=%d\tsubdiv=%d\tsub_beatsize=%d", beatsize, subdiv, sub_beatsize);
			i = 0;
			if (x->debug >= 3) post("%d ***************   TUPLET %d   *************** %d", i, i, i);
			for(i=0; i<x->b_tp_index; i++) { 
				if (i>0 && x->b_tp_num[i] != x->b_tp_num[i-1]) {
					if (x->debug >= 3) post("%d ***************   TUPLET %d   *************** %d", x->b_tp_num[i], x->b_tp_num[i], x->b_tp_num[i]);
					beatsize = x->tp_info[x->b_tp_num[i]][0];
					sub_beatsize = beatsize / subdiv;
					if (x->debug >= 3) post("beatsize=%d\tsubdiv=%d\tsub_beatsize=%d", beatsize, subdiv, sub_beatsize);
					beatcount = 0;
				}
				if (sub_beatsize >= 4) {
					if 		(((beatcount + x->b_tp_dur[i]) <= sub_beatsize)) {
						x->sb_tp_dur[x->sb_tp_index] 	= x->b_tp_dur[i];
						x->sb_tp_ri[x->sb_tp_index]		= x->b_tp_ri[i];
						x->sb_tp_i[x->sb_tp_index]		= x->b_tp_i[i];
						x->sb_tp_tie[x->sb_tp_index]	= x->b_tp_tie[i];
						x->sb_tp_num[x->sb_tp_index] 	= x->b_tp_num[i];
						if (x->debug >= 3) post("%d\t%d\t%d\t%d\t%d\t%d", 	x->sb_tp_index, x->sb_tp_num[x->sb_tp_index], x->sb_tp_ri[x->sb_tp_index], x->sb_tp_i[x->sb_tp_index], x->sb_tp_dur[x->sb_tp_index], x->sb_tp_tie[x->sb_tp_index]);
						beatcount 			+= x->b_tp_dur[i];
						while (beatcount >= beatsize) {
							beatcount -= beatsize;
						}
						if (x->debug >= 3 && beatcount % beatsize == 0) post("-   -   -   -   -   -   -   -   -   -   -   -   ");
						x->sb_tp_index++;
					}
					else if	((beatcount == 0) && ((x->b_tp_dur[i] % sub_beatsize) == 0)) {
						for(j=0; j<32; j++){
							if (x->tp_dur[i] == sub_beatsize * j) {
								x->sb_tp_dur[x->sb_tp_index] 	= x->b_tp_dur[i];
								x->sb_tp_ri[x->sb_tp_index]		= x->b_tp_ri[i];
								x->sb_tp_i[x->sb_tp_index]		= x->b_tp_i[i];
								x->sb_tp_tie[x->sb_tp_index]	= x->b_tp_tie[i];
								x->sb_tp_num[x->sb_tp_index] 	= x->b_tp_num[i];
								if (x->debug >= 3) post("%d\t%d\t%d\t%d\t%d\t%d", 	x->sb_tp_index, x->sb_tp_num[x->sb_tp_index], x->sb_tp_ri[x->sb_tp_index], x->sb_tp_i[x->sb_tp_index], x->sb_tp_dur[x->sb_tp_index], x->sb_tp_tie[x->sb_tp_index]);
								beatcount 			+= x->b_tp_dur[i];
								while (beatcount >= beatsize) {
									beatcount -= beatsize;
								}
								if (x->debug >= 3 && beatcount % beatsize == 0) post("-   -   -   -   -   -   -   -   -   -   -   -   ");
								x->sb_tp_index++;
								break;
							}
						}
					}
					else {
						x->sb_tp_dur[x->sb_tp_index] 	= sub_beatsize - beatcount;
						x->sb_tp_ri[x->sb_tp_index]		= x->b_tp_ri[i];
						x->sb_tp_i[x->sb_tp_index]		= x->b_tp_i[i];
						x->sb_tp_tie[x->sb_tp_index]	= 1;
						x->sb_tp_num[x->sb_tp_index] 	= x->b_tp_num[i];
					
						if (x->debug >= 3) {post("%d\t%d\t%d\t%d\t%d\t%d", 	x->sb_tp_index, x->sb_tp_num[x->sb_tp_index], x->sb_tp_ri[x->sb_tp_index], x->sb_tp_i[x->sb_tp_index], x->sb_tp_dur[x->sb_tp_index], x->sb_tp_tie[x->sb_tp_index]); post("-   -   -   -   -   -   -   -   -   -   -   -   ");}					
						x->sb_tp_index++;
					
						dur_remainder = x->b_tp_dur[i] - x->sb_tp_dur[x->sb_tp_index-1];
					
						while (dur_remainder > sub_beatsize) {
							x->sb_tp_dur[x->sb_tp_index] 	= sub_beatsize;
							x->sb_tp_ri[x->sb_tp_index]		= x->b_tp_ri[i];
							x->sb_tp_i[x->sb_tp_index]		= x->b_tp_i[i];
							x->sb_tp_tie[x->sb_tp_index]	= 1;
							x->sb_tp_num[x->sb_tp_index] 	= x->b_tp_num[i];

							if (x->debug >= 3){ post("%d\t%d\t%d\t%d\t%d\t%d", 	x->sb_tp_index, x->sb_tp_num[x->sb_tp_index], x->sb_tp_ri[x->sb_tp_index], x->sb_tp_i[x->sb_tp_index], x->sb_tp_dur[x->sb_tp_index], x->sb_tp_tie[x->sb_tp_index]); post("-   -   -   -   -   -   -   -   -   -   -   -   ");}
							x->sb_tp_index++;
						
							dur_remainder 					-= sub_beatsize;
						}
			
						x->sb_tp_dur[x->sb_tp_index] 	= dur_remainder;
						x->sb_tp_ri[x->sb_tp_index]		= x->b_tp_ri[i];
						x->sb_tp_i[x->sb_tp_index]		= x->b_tp_i[i];
						x->sb_tp_tie[x->sb_tp_index]	= x->b_tp_tie[i];
						x->sb_tp_num[x->sb_tp_index] 	= x->b_tp_num[i];
					
						beatcount 			+= x->b_tp_dur[i];
						while (beatcount >= beatsize) {
							beatcount -= beatsize;
						}
						if (x->debug >= 3)post("%d\t%d\t%d\t%d\t%d\t%d", 	x->sb_tp_index, x->sb_tp_num[x->sb_tp_index], x->sb_tp_ri[x->sb_tp_index], x->sb_tp_i[x->sb_tp_index], x->sb_tp_dur[x->sb_tp_index], x->sb_tp_tie[x->sb_tp_index]);
						if (x->debug >= 3 && beatcount % beatsize == 0) post("-   -   -   -   -   -   -   -   -   -   -   -   ");
						x->sb_tp_index++;
					}
				}
				else {
					x->sb_tp_dur[x->sb_tp_index] 	= x->b_tp_dur[i];
					x->sb_tp_ri[x->sb_tp_index]		= x->b_tp_ri[i];
					x->sb_tp_i[x->sb_tp_index]		= x->b_tp_i[i];
					x->sb_tp_tie[x->sb_tp_index]	= x->b_tp_tie[i];
					x->sb_tp_num[x->sb_tp_index] 	= x->b_tp_num[i];
					if (x->debug >= 3) post("%d\t%d\t%d\t%d\t%d\t%d", 	x->sb_tp_index, x->sb_tp_num[x->sb_tp_index], x->sb_tp_ri[x->sb_tp_index], x->sb_tp_i[x->sb_tp_index], x->sb_tp_dur[x->sb_tp_index], x->sb_tp_tie[x->sb_tp_index]);
					beatcount 			+= x->b_tp_dur[i];
					if (x->debug >= 3 && beatcount % beatsize == 0) post("-   -   -   -   -   -   -   -   -   -   -   -   ");
					x->sb_tp_index++;
				}
			
			}
			for(i=0; i<x->sb_tp_index; i++){
				x->b_tp_dur[i] 		= x->sb_tp_dur[i];
				x->b_tp_ri[i]		= x->sb_tp_ri[i];
				x->b_tp_i[i]		= x->sb_tp_i[i];
				x->b_tp_tie[i]		= x->sb_tp_tie[i];
				x->b_tp_num[i]		= x->sb_tp_num[i];
			}
			x->b_tp_index 	= x->sb_tp_index;
		}
		if (x->debug >= 1) 				{
			post("\nFINAL TUPLET LIST :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
			post("i\ttp_n\traw_i\tclean_i\tdur\ttie");
		}
		for(i=0; i<x->sb_tp_index; i++) {
			if (x->sb_tp_num[i] != x->sb_tp_num[i-1]) if (x->debug >= 1) post("-   -   -   -   -   -   -   -   -   -   -   -   ");
			if (x->debug >= 1) post("%d\t%d\t%d\t%d\t%d\t%d", i, x->sb_tp_num[i], x->sb_tp_ri[i], x->sb_tp_i[i], (int) x->sb_tp_dur[i], x->sb_tp_tie[i]);
		}
	//// ____________________________________________________________ BAR and BEAT Programs 
		if (x->debug >= 1) 				{
			post("\nCLEAN INPUT LIST :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
			post("i\traw_i\tdur\ttie\ttup");
		}
		for(i=0; i<x->i_index; i++) 	{
			if (x->debug >= 1) post("%d\t%d\t%d\t%d\t%d", i, x->i_rii[i], (int) x->i_dur[i], x->i_tie[i], x->i_tup[i]);
			if ( x->i_tup[i] >= 0) {
				if (x->debug >= 1) post("Tuplet will replace last line");
			}
		}
	//// ____________________________________________________________  BAR SUBDIVISION 
		if (x->debug >= 1) post(".\nBAR PROGRAMs ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n.");
/////////////////////// BREAK INPUT INTO BARS
		int ii;
		if (x->SLAVE == 0) {
			beatsize = exp2f( (float) (log2f((float) x->refdur) - log2f((float) x->i_mtr[0][1])));
			barsize = (int) x->i_mtr[0][0] * beatsize;
			x->bar_info[0][0] = 0;
			x->bar_info[0][1] = x->i_mtr[0][0];
			x->bar_info[0][2] = x->i_mtr[0][1];
			x->bar_info[0][3] = beatsize;
			x->bar_info[0][4] = barsize;
			if (x->debug >= 1) 				{
				post("num=%d", (int) x->i_mtr[0][0]);
				post("refdur=%d denom=%d beatsize=%d", x->refdur, x->i_mtr[0][1], beatsize);
				post("barsize=%d\n", barsize);
				post("i\tbar no.\tclean i\traw i\tdur\ttie\ttuplet");
				post("BAR %d - - - - - - - - - - - - - - - - - - - - - - - - - - - - ", bar_number);	
			}
			barcount = 0; 
			for(i=0; i<x->i_index; i++) 	{
				if		(i > 1 && (x->i_mtr[i][0] != x->i_mtr[i-1][0] || x->i_mtr[i][1] != x->i_mtr[i-1][1])) {	
					if 	(barcount != 0) {
						x->bar_info[bar_number][1] = barcount;
						x->bar_info[bar_number][2] = 32;
						temp_f = reduce_meter(x->bar_info[bar_number][1]);
						if (temp_f != 0) {
							x->bar_info[bar_number][1] = (int) temp_f;
							temp_f = ((temp_f - (int) temp_f) * 100.) + 0.02;
							x->bar_info[bar_number][2] = (int) temp_f;
						}
						beatsize = exp2f( (float) (log2f((float) x->refdur) - log2f((float) x->bar_info[bar_number][2])));
						barsize = (int) x->bar_info[bar_number][1] * beatsize;
						beatsize = get_beatsize(x->bar_info[bar_number][1], x->bar_info[bar_number][2], x->refdur);
						x->bar_info[bar_number][3] = beatsize;
						x->bar_info[bar_number][4] = barsize;
						if (x->debug >= 1) post("Meter change::: %d / %d", x->bar_info[bar_number][1], x->bar_info[bar_number][2]);
						bar_number++;
						x->bar_info[bar_number][0] = x->b_index;
						if (x->debug >= 1) post("BAR %d - - - - - - - - - - - - - - - - - - - - - - - - - - - - ", bar_number);
					}
					beatsize = exp2f( (float) (log2f((float) x->refdur) - log2f((float) x->i_mtr[0][1])));
					barsize = (int) x->i_mtr[i][0] * beatsize;
					beatsize = get_beatsize(x->i_mtr[i][0], x->i_mtr[i][1], x->refdur);
					x->bar_info[bar_number][1] = x->i_mtr[i][0];
					x->bar_info[bar_number][2] = x->i_mtr[i][1];
					x->bar_info[bar_number][3] = beatsize;
					x->bar_info[bar_number][4] = barsize;
					barcount = 0;
					if (x->debug >= 1) post("Meter change::: %d / %d", x->bar_info[bar_number][1], x->bar_info[bar_number][2]);
				}
				if		(barcount + x->i_dur[i] <= barsize){
					x->b_dur[x->b_index] 	= x->i_dur[i]; //duration
					x->b_tie[x->b_index] 	= 0; // tie
					x->b_i[x->b_index] 		= i; // clean input number
					x->bar_n[x->b_index] 	= bar_number; // bar number
					x->b_rii[x->b_index]	= x->i_rii[i]; //raw input number
					x->b_tup[x->b_index] 	= x->i_tup[i]; //tuplet
					barcount += x->i_dur[i];
					if (x->debug >= 1) post("%d\t%d\t%d\t%d\t%d\t%d\t%d", x->b_index, x->bar_n[x->b_index], x->b_i[x->b_index], x->b_rii[x->b_index], x->b_dur[x->b_index], x->b_tie[x->b_index], x->b_tup[x->b_index]);
					x->b_index++;
					if (barcount == barsize) {
						bar_number++;
						barcount 					= 0;
						x->bar_info[bar_number][0] 	= x->b_index;
						x->bar_info[bar_number][1] 	= x->i_mtr[i][0];
						x->bar_info[bar_number][2] 	= x->i_mtr[i][1];
						x->bar_info[bar_number][3] 	= beatsize;
						x->bar_info[bar_number][4] 	= barsize;
						if (x->debug >= 1) post("BAR %d - - - - - - - - - - - - - - - - - - - - - - - - - - - - ", bar_number);	
					}
				}
				else 	{
					x->b_dur[x->b_index] 	= barsize - barcount;
					barcount 				= 0;
					dur_remainder 			= x->i_dur[i] - x->b_dur[x->b_index];
					x->b_tie[x->b_index] 	= 1;
					x->b_i[x->b_index] 		= i;
					x->bar_n[x->b_index] 	= bar_number;
					x->b_rii[x->b_index]	= x->i_rii[i]; //raw input number
					x->b_tup[x->b_index] 	= x->i_tup[i]; //tuplet
					if (x->debug >= 1) post("%d\t%d\t%d\t%d\t%d\t%d\t%d", x->b_index, x->bar_n[x->b_index], x->b_i[x->b_index], x->b_rii[x->b_index], x->b_dur[x->b_index], x->b_tie[x->b_index], x->b_tup[x->b_index]);
					x->b_index++;
					bar_number++;
					x->bar_info[bar_number][0] = x->b_index;
					x->bar_info[bar_number][1] = x->i_mtr[i][0];
					x->bar_info[bar_number][2] = x->i_mtr[i][1];
					x->bar_info[bar_number][3] = beatsize;
					x->bar_info[bar_number][4] = barsize;
					if (x->debug >= 1) post("BAR %d - - - - - - - - - - - - - - - - - - - - - - - - - - - - ", bar_number);	
						
					while (dur_remainder > 0) {
						if		(barcount + dur_remainder <= barsize) {
							x->b_dur[x->b_index] 	= dur_remainder;
							x->b_tie[x->b_index]	= 0;
							x->b_i[x->b_index] 		= i;
							x->b_rii[x->b_index]	= x->i_rii[i]; ////raw input number
							x->b_tup[x->b_index] 	= x->i_tup[i]; //tuplet
							x->bar_n[x->b_index] 	= bar_number;
							barcount += dur_remainder;
							if (x->debug >= 1) post("%d\t%d\t%d\t%d\t%d\t%d\t%d", x->b_index, x->bar_n[x->b_index], x->b_i[x->b_index], x->b_rii[x->b_index], x->b_dur[x->b_index], x->b_tie[x->b_index], x->b_tup[x->b_index]);
							x->b_index++;
							if (barcount == barsize) {
								barcount 					= 0;
								bar_number++;
								x->bar_info[bar_number][0] 	= x->b_index;
								x->bar_info[bar_number][1] 	= x->i_mtr[i][0];
								x->bar_info[bar_number][2] 	= x->i_mtr[i][1];
								x->bar_info[bar_number][3] 	= beatsize;
								x->bar_info[bar_number][4] 	= barsize;
								if (x->debug >= 1) post("BAR %d - - - - - - - - - - - - - - - - - - - - - - - - - - - - ", bar_number);	
							}
							dur_remainder = 0;
						}
						else {
							// copy pitches
							x->b_dur[x->b_index] 	= barsize;
							barcount 				= 0;
							dur_remainder 			-= barsize;
							x->b_tie[x->b_index] 	= 1;
							x->b_i[x->b_index] 		= i;
							x->b_rii[x->b_index]	= x->i_rii[i]; //raw input number
							x->b_tup[x->b_index] 	= x->i_tup[i]; //tuplet
							x->bar_n[x->b_index] 	= bar_number;
							if (x->debug >= 1) post("%d\t%d\t%d\t%d\t%d\t%d\t%d", x->b_index, x->bar_n[x->b_index], x->b_i[x->b_index], x->b_rii[x->b_index], x->b_dur[x->b_index], x->b_tie[x->b_index], x->b_tup[x->b_index]);
							x->b_index++;
							bar_number++;
							x->bar_info[bar_number][0] = x->b_index;
							x->bar_info[bar_number][1] = x->i_mtr[i][0];
							x->bar_info[bar_number][2] = x->i_mtr[i][1];
							x->bar_info[bar_number][3] = beatsize;
							x->bar_info[bar_number][4] = barsize;
							if (x->debug >= 1) post("BAR %d - - - - - - - - - - - - - - - - - - - - - - - - - - - - ", bar_number);	
						} 
					}
				}
			}
		}
	////////// BREAK INPUT INTO THE (imported) MASTER BARS
		else {
		//////// READ BARFILE
			FILE *fp3;
			fp3 = fopen( x->barfile, "r");
			if(!fp3) {
				error("%s: couldn't read barfile, send one to the second inlet", x->barfile);
				return;
			}
			else 	ii = readbarfile(x->bar_info, (FILE *) fp3); //////sdsdfsdfsdfsdf
//////sdsdfsdfsdfsdf//////sdsdfsdfsdfsdf//////sdsdfsdfsdfsdf//////sdsdfsdfsdfsdf
//////sdsdfsdfsdfsdf//////sdsdfsdfsdfsdf//////sdsdfsdfsdfsdf//////sdsdfsdfsdfsdf
//////sdsdfsdfsdfsdf//////sdsdfsdfsdfsdf//////sdsdfsdfsdfsdf//////sdsdfsdfsdfsdf
//////sdsdfsdfsdfsdf//////sdsdfsdfsdfsdf//////sdsdfsdfsdfsdf//////sdsdfsdfsdfsdf
			if (x->debug >= 1){
				post("Reading Master Bar File");
				post("i\tnum\tdenom\tbeatsize\tbarsize\t");
				for (i=0;i<ii;i++){
					post("%d\t%d\t%d\t%d\t%d\t", i, x->bar_info[i][1], x->bar_info[i][2], x->bar_info[i][3], x->bar_info[i][4]);
				}
			}
		//////// BREAK INPUT BY BARFILE
			beatsize 	= x->bar_info[0][3];
			barsize 	= x->bar_info[0][4];
			x->bar_info[0][0] = 0;
			if (x->debug >= 1) 				{
				post("num=%d", (int) x->bar_info[0][1]);
				post("refdur=%d denom=%d beatsize=%d", x->refdur, x->bar_info[0][2], beatsize);
				post("barsize=%d\n", barsize);
				post("i\tbar no.\tclean i\traw i\tdur\ttie\ttuplet");
				post("BAR %d - - - - - - - - - - - - - - - - - - - - - - - - - - - - ", bar_number);	
			}
			barcount = 0; 
			x->bar_info[bar_number][0] = x->b_index;
			for(i=0; i<x->i_index; i++) 	{
				if		(barcount + x->i_dur[i] <= barsize){
					x->b_dur[x->b_index] 	= x->i_dur[i]; //duration
					x->b_tie[x->b_index] 	= x->i_tie[i]; // tie
					x->b_i[x->b_index] 		= i; // clean input number
					x->bar_n[x->b_index] 	= bar_number; // bar number
					x->b_rii[x->b_index]	= x->i_rii[i]; //raw input number
					x->b_tup[x->b_index] 	= x->i_tup[i]; //tuplet
					barcount += x->i_dur[i];
					if (x->debug >= 1) post("%d\t%d\t%d\t%d\t%d\t%d\t%d", x->b_index, x->bar_n[x->b_index], x->b_i[x->b_index], x->b_rii[x->b_index], x->b_dur[x->b_index], x->b_tie[x->b_index], x->b_tup[x->b_index]);
					x->b_index++;
					if (barcount == barsize) {
						bar_number++;
						barcount 					= 0;
						x->bar_info[bar_number][0] 	= x->b_index;
						beatsize 	= x->bar_info[bar_number][3];
						barsize 	= x->bar_info[bar_number][4];
						if (bar_number >= ii) {
							x->bar_info[bar_number][1] 	= x->bar_info[bar_number-1][1];
							x->bar_info[bar_number][2] 	= x->bar_info[bar_number-1][2];
							x->bar_info[bar_number][3] 	= x->bar_info[bar_number-1][3];
							x->bar_info[bar_number][4] 	= x->bar_info[bar_number-1][4];
							beatsize 					= x->bar_info[bar_number][3];
							barsize 					= x->bar_info[bar_number][4];
						}
						if (x->debug >= 1) post("BAR %d - - - - - - - - - - - - - - - - - - - - - - - - - - - - ", bar_number);	
					}
				}
				else 	{
					x->b_dur[x->b_index] 	= barsize - barcount;
					barcount 				= 0;
					dur_remainder 			= x->i_dur[i] - x->b_dur[x->b_index];
					x->b_tie[x->b_index] 	= 1;
					x->b_i[x->b_index] 		= i;
					x->bar_n[x->b_index] 	= bar_number;
					x->b_rii[x->b_index]	= x->i_rii[i]; //raw input number
					x->b_tup[x->b_index] 	= x->i_tup[i]; //tuplet
					if (x->debug >= 1) post("%d\t%d\t%d\t%d\t%d\t%d\t%d", x->b_index, x->bar_n[x->b_index], x->b_i[x->b_index], x->b_rii[x->b_index], x->b_dur[x->b_index], x->b_tie[x->b_index], x->b_tup[x->b_index]);
					x->b_index++;
					bar_number++;
					x->bar_info[bar_number][0] = x->b_index;
					beatsize 	= x->bar_info[bar_number][3];
					barsize 	= x->bar_info[bar_number][4];
					if (bar_number >= ii) {
						x->bar_info[bar_number][1] 	= x->bar_info[bar_number-1][1];
						x->bar_info[bar_number][2] 	= x->bar_info[bar_number-1][2];
						x->bar_info[bar_number][3] 	= x->bar_info[bar_number-1][3];
						x->bar_info[bar_number][4] 	= x->bar_info[bar_number-1][4];
						beatsize 					= x->bar_info[bar_number][3];
						barsize 					= x->bar_info[bar_number][4];
					}
					if (x->debug >= 1) post("BAR %d - - - - - - - - - - - - - - - - - - - - - - - - - - - - ", bar_number);	
						
					while (dur_remainder > 0) {
						if		(barcount + dur_remainder <= barsize) {
							x->b_dur[x->b_index] 	= dur_remainder;
							x->b_tie[x->b_index]	= 0;
							x->b_i[x->b_index] 		= i;
							x->b_rii[x->b_index]	= x->i_rii[i]; ////raw input number
							x->b_tup[x->b_index] 	= x->i_tup[i]; //tuplet
							x->bar_n[x->b_index] 	= bar_number;
							barcount += dur_remainder;
							if (x->debug >= 1) post("%d\t%d\t%d\t%d\t%d\t%d\t%d", x->b_index, x->bar_n[x->b_index], x->b_i[x->b_index], x->b_rii[x->b_index], x->b_dur[x->b_index], x->b_tie[x->b_index], x->b_tup[x->b_index]);
							x->b_index++;
							if (barcount == barsize) {
								barcount 					= 0;
								bar_number++;
								x->bar_info[bar_number][0] 	= x->b_index;
								beatsize 	= x->bar_info[bar_number][3];
								barsize 	= x->bar_info[bar_number][4];
								if (bar_number >= ii) {
									x->bar_info[bar_number][1] 	= x->bar_info[bar_number-1][1];
									x->bar_info[bar_number][2] 	= x->bar_info[bar_number-1][2];
									x->bar_info[bar_number][3] 	= x->bar_info[bar_number-1][3];
									x->bar_info[bar_number][4] 	= x->bar_info[bar_number-1][4];
									beatsize 					= x->bar_info[bar_number][3];
									barsize 					= x->bar_info[bar_number][4];
								}
								if (x->debug >= 1) post("BAR %d - - - - - - - - - - - - - - - - - - - - - - - - - - - - ", bar_number);	
							}
							dur_remainder = 0;
						}
						else {
							// copy pitches
							x->b_dur[x->b_index] 	= barsize;
							barcount 				= 0;
							dur_remainder 			-= barsize;
							x->b_tie[x->b_index] 	= 1;
							x->b_i[x->b_index] 		= i;
							x->b_rii[x->b_index]	= x->i_rii[i]; //raw input number
							x->b_tup[x->b_index] 	= x->i_tup[i]; //tuplet
							x->bar_n[x->b_index] 	= bar_number;
							if (x->debug >= 1) post("%d\t%d\t%d\t%d\t%d\t%d\t%d", x->b_index, x->bar_n[x->b_index], x->b_i[x->b_index], x->b_rii[x->b_index], x->b_dur[x->b_index], x->b_tie[x->b_index], x->b_tup[x->b_index]);
							x->b_index++;
							bar_number++;
							x->bar_info[bar_number][0] = x->b_index;
							beatsize 	= x->bar_info[bar_number][3];
							barsize 	= x->bar_info[bar_number][4];
							if (bar_number >= ii) {
								x->bar_info[bar_number][1] 	= x->bar_info[bar_number-1][1];
								x->bar_info[bar_number][2] 	= x->bar_info[bar_number-1][2];
								x->bar_info[bar_number][3] 	= x->bar_info[bar_number-1][3];
								x->bar_info[bar_number][4] 	= x->bar_info[bar_number-1][4];
								beatsize 					= x->bar_info[bar_number][3];
								barsize 					= x->bar_info[bar_number][4];
							}
							if (x->debug >= 1) post("BAR %d - - - - - - - - - - - - - - - - - - - - - - - - - - - - ", bar_number);	
						} 
					}
				}
			}	
		}
		if (x->debug >= 1) 				{
			post("\nChecking for tuplet errors\n");
		} //// Check for tuplet errors
		for(i=0; i<x->i_index; i++) 	{
			if (x->bar_n[i] != x->bar_n[i-1] && x->b_tup[i] == x->b_tup[i-1] && x->b_tup[i] > 0) post("¡¡¡¡¡¡¡ERROR!!!!!! Tuplet No. %d crosses the bar line between bars %d and %d", x->b_tup[i], x->bar_n[i-1], x->bar_n[i]);
		} //// Print Tuplet Errors
	//	x->debug = 1;
		if (x->debug >= 1) 				{
			post("\nBAR INFORMATION\n");
			post("i\tinit\telems\tnum\tden\tbeatsize\tbarsize");
		} //// Print bar info headers
		if (x->SLAVE == 0) 				{
			char bar_buf[MAXPDSTRING];
			FILE *fp;

			canvas_makefilename(x->x_canvas, s->s_name, bar_buf, MAXPDSTRING); /// This is a Pd function to get the path relative to the canvas
			sys_bashfilename(bar_buf, bar_buf);
			strcat( bar_buf, "_barinfo.txt");
			t_symbol *masterbar = gensym(bar_buf);
			outlet_symbol(x->x_outlet1, masterbar);
			fp = fopen(bar_buf, "w");
			if(!fp)	{
				error("%s: couldn't create", bar_buf);
				return;
			}
			else {
				for(i=0; i<=bar_number; i++) {
					j = x->bar_info[i+1][0] - x->bar_info[i][0];
					if (i == bar_number) j = x->b_index - x->bar_info[i][0];
					if (x->debug >= 1) post("%d\t%d\t%d\t%d\t%d\t%d\t%d\t", i, x->bar_info[i][0], j, x->bar_info[i][1], x->bar_info[i][2], x->bar_info[i][3], x->bar_info[i][4]);
					if (x->SLAVE == 0) fprintf(fp, "%d %d %d %d %d\n", i, x->bar_info[i][1], x->bar_info[i][2], x->bar_info[i][3], x->bar_info[i][4]);
				}
			}
			fclose(fp);
		} //// GENERATE A BAR INFO FILE
		else 							{
			for(i=0; i<=bar_number; i++) {
				j = x->bar_info[i+1][0] - x->bar_info[i][0];
				if (i == bar_number) j = x->b_index - x->bar_info[i][0];
				if (x->debug >= 1) post("%d\t%d\t%d\t%d\t%d\t%d\t%d", i, x->bar_info[i][0], j, x->bar_info[i][1], x->bar_info[i][2], x->bar_info[i][3], x->bar_info[i][4]);
			}
		} //// Print BAR INFOR
	//	x->debug = 0;
	//// _______________________________________________ BEAT SUBDIVISION 
		if (x->debug >= 1) 				{
			post(".\nBEAT PROGRAM :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n.");
			post("beatsize=%d\n", beatsize);
			post("i\tbar_n\tbeat_n\tclean_i\traw_i\tdur\ttie\ttuplet");
			post("METER = %d/%d\t beatsize = %d", x->bar_info[x->be_bar_n[0]][1], x->bar_info[x->be_bar_n[0]][2], beatsize);			
			post("BAR %d ===============================================", 0);
		}
		beatcount = beat_number = 0;
		beatsize 	= x->bar_info[0][3];
		barsize		= x->bar_info[0][4];
		for(i=0; i<x->b_index; i++) 	{
		///// CHECK METER
			if 		(i > 0	&&	x->bar_n[i] != x->bar_n[i-1]) 	{
				if (x->debug >= 1) post("BAR %d ===============================================", x->bar_n[i]);
				beatcount = 0;
				beat_number = 0;
				if (x->debug >= 1) post("beatsize = %d", beatsize);
				if (x->bar_info[x->bar_n[i]][1] != x->bar_info[x->bar_n[i-1]][1] ||
					x->bar_info[x->bar_n[i]][2] != x->bar_info[x->bar_n[i-1]][2] ) {
					beatsize 	= x->bar_info[x->bar_n[i]][3];
					barsize		= x->bar_info[x->bar_n[i]][4];
					if (x->debug >= 1) post("METER = %d/%d\t beatsize = %d", x->bar_info[x->bar_n[i]][1], x->bar_info[x->bar_n[i]][2], beatsize);			
				}
			}
		///// ASSIGN DURATIONS
			/// less than beat?
			if		((beatcount + x->b_dur[i]) <= beatsize) {
				x->be_dur[x->be_index] 		= x->b_dur[i];
				x->be_tie[x->be_index] 		= x->b_tie[i];
				x->be_i[x->be_index] 		= x->b_i[i];   // clean input number (original input index)
				x->be_rii[x->be_index]		= x->b_rii[i]; // raw input number
				x->be_tup[x->be_index] 		= x->b_tup[i]; // tuplet
				x->be_bar_n[x->be_index]	= x->bar_n[i]; // bar number
				beatcount 					+= x->b_dur[i];
				x->be_beat_n[x->be_index]	= beat_number / x->bar_info[x->bar_n[i]][3];
				beat_number					+= x->b_dur[i];
				if (x->debug >= 1) post("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d", x->be_index, x->be_bar_n[x->be_index], x->be_beat_n[x->be_index], x->be_i[x->be_index], x->be_rii[x->be_index], x->be_dur[x->be_index], x->be_tie[x->be_index], x->be_tup[x->be_index]);
				if (x->debug >= 3) post("<<<<<<<<< beatcount = %d", beatcount);
				if (beatcount == beatsize) {
					beatcount = 0;
					if (x->debug >= 1) post("-      -      -      -      -      -      -      -      -      -      -      -      -      -");	
				}
				x->be_index++;
			}
			else if	(beatcount == 0 && x->b_dur[i]%beatsize == 0)	{
				dur_remainder = x->b_dur[i];
				if (x->debug >= 3) post("durem=%d\tbarsize=%d", dur_remainder, barsize);
				////// EXACTLY EQUAL to A MULTIPLE
				for(j=0; j<32; j++){
					if (dur_remainder == beatsize * j) {
						if (pow2ordot(dur_remainder) == 1) {
							x->be_dur[x->be_index] 		= dur_remainder;
							dur_remainder 				= 0;
							beatcount 					= 0;
							x->be_tie[x->be_index] 		= x->b_tie[i];
							x->be_i[x->be_index] 		= x->b_i[i]; // input number (original input index)
							x->be_rii[x->be_index]		= x->b_rii[i]; // raw input number
							x->be_tup[x->be_index] 		= x->b_tup[i]; // tuplet
							x->be_bar_n[x->be_index]	= x->bar_n[i]; // bar number
							x->be_beat_n[x->be_index]	= beat_number / x->bar_info[x->bar_n[i]][3];
							beat_number					+= x->be_dur[x->be_index];
							if (x->debug >= 1) post("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d", x->be_index, x->be_bar_n[x->be_index], x->be_beat_n[x->be_index], x->be_i[x->be_index], x->be_rii[x->be_index], x->be_dur[x->be_index], x->be_tie[x->be_index], x->be_tup[x->be_index]);
							if (x->debug >= 3) post("BEAT:::: EXACT-2orDOT beatcount = %d,\tdur:%d", beatcount, x->be_dur[x->be_index]);
							if (x->debug >= 1) post("-      -      -      -      -      -      -      -      -      -      -      -      -      -");	
							x->be_index++;
						} // If it is exactly power or dot
						else {
							beatcount += x->b_dur[i] % beatsize;							
							while (dur_remainder > 0) {
								if (dur_remainder >= beatsize) {
									for(j=0; j<18; j++){
										if 	(dur_remainder < pow2dot_array[j])	{
											if (x->debug >= 3) post("%d, temp=%d", j, (int) temp_f);
											break;
										}
									} 
									if (x->debug >= 3) post("rem=%d\tj=%d\tdur=%d", dur_remainder, j-1, pow2dot_array[j-1]);
									x->be_dur[x->be_index] 		= pow2dot_array[j-1];
									dur_remainder 				-= x->be_dur[x->be_index];
								}
								else {
									x->be_dur[x->be_index] 		= dur_remainder;
									dur_remainder 				= 0;
								}
								if (dur_remainder == 0) {	x->be_tie[x->be_index] 	= x->b_tie[i];	}
								else 					{	x->be_tie[x->be_index] 	= 1; 			}
								x->be_i[x->be_index] 		= x->b_i[i]; // input number (original input index)
								x->be_rii[x->be_index]		= x->b_rii[i]; // raw input number
								x->be_tup[x->be_index] 		= x->b_tup[i]; // tuplet
								x->be_bar_n[x->be_index]	= x->bar_n[i]; // bar number
								x->be_beat_n[x->be_index]	= beat_number / x->bar_info[x->bar_n[i]][3];
								beat_number					+= x->be_dur[x->be_index];
								if (x->debug >= 1) post("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d", x->be_index, x->be_bar_n[x->be_index], x->be_beat_n[x->be_index], x->be_i[x->be_index], x->be_rii[x->be_index], x->be_dur[x->be_index], x->be_tie[x->be_index], x->be_tup[x->be_index]);
								if (x->debug >= 3) post("BEAT:::: ==0 Closest 2ordot beatcount = %d\tdur=%d", beatcount, x->be_dur[x->be_index]);
								if (x->debug >= 1) post("-      -      -      -      -      -      -      -      -      -      -      -      -      -");	
								x->be_index++;
							}						
						} 
						break;
					}
				}
			} 
			else if	(beatcount == 0 && x->b_dur[i]%beatsize != 0)	{
				dur_remainder = x->b_dur[i];
				while (dur_remainder > 0) {
					if (dur_remainder >= beatsize) {
						for(j=0; j<18; j++){
							if 	(dur_remainder < pow2dot_array[j])	{
								if (x->debug >= 3) post("%d, temp=%d", j, (int) temp_f);
								break;
							}
						} 
						if (x->debug >= 3) post("rem=%d\tj=%d\tdur=%d", dur_remainder, j-1, pow2dot_array[j-1]);
						x->be_dur[x->be_index] 		= pow2dot_array[j-1];
						dur_remainder 				-= x->be_dur[x->be_index];
					}
					else{
						x->be_dur[x->be_index] 		= dur_remainder;
						dur_remainder 				= 0;
					}
					if (dur_remainder == 0) {	x->be_tie[x->be_index] 	= x->b_tie[i];	}
					else 					{	x->be_tie[x->be_index] 	= 1; 			}
					x->be_i[x->be_index] 		= x->b_i[i]; // input number (original input index)
					x->be_rii[x->be_index]		= x->b_rii[i]; // raw input number
					x->be_tup[x->be_index] 		= x->b_tup[i]; // tuplet
					x->be_bar_n[x->be_index]	= x->bar_n[i]; // bar number
					x->be_beat_n[x->be_index]	= beat_number / x->bar_info[x->bar_n[i]][3];
					beat_number					+= x->be_dur[x->be_index];
					if (x->debug >= 1) post("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d", x->be_index, x->be_bar_n[x->be_index], x->be_beat_n[x->be_index], x->be_i[x->be_index], x->be_rii[x->be_index], x->be_dur[x->be_index], x->be_tie[x->be_index], x->be_tup[x->be_index]);
					if (x->debug >= 3) post("BEAT:::: !=0 Closest 2ordot beatcount = %d\tdur=%d", beatcount, x->be_dur[x->be_index]);
					beatcount += x->be_dur[x->be_index] % beatsize;
					if (beatcount == 0) if (x->debug >= 1) post("-      -      -      -      -      -      -      -      -      -      -      -      -      -");	
					x->be_index++;
				}
			}
			////// NOT ZERO 
			else 											{
				x->be_dur[x->be_index] 		= beatsize - beatcount;
				dur_remainder 				= x->b_dur[i] - x->be_dur[x->be_index];
				x->be_tie[x->be_index] 		= 1;
				x->be_i[x->be_index] 		= x->b_i[i];
				x->be_rii[x->be_index]		= x->b_rii[i]; // raw input number
				x->be_tup[x->be_index] 		= x->b_tup[i]; // tuplet
				x->be_bar_n[x->be_index]	= x->bar_n[i]; // bar number
				beatcount 					= 0;
				x->be_beat_n[x->be_index]	= beat_number / x->bar_info[x->bar_n[i]][3];
				beat_number					+= x->be_dur[x->be_index];
				if (x->debug >= 1) post("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d", x->be_index, x->be_bar_n[x->be_index], x->be_beat_n[x->be_index], x->be_i[x->be_index], x->be_rii[x->be_index], x->be_dur[x->be_index], x->be_tie[x->be_index], x->be_tup[x->be_index]);
				if (x->debug >= 3) post("BEAT::::not zero beatcount = %d", beatcount);
				if (x->debug >= 1) post("-      -      -      -      -      -      -      -      -      -      -      -      -      -");					
				x->be_index++;
				while (dur_remainder > 0) {
					if	(dur_remainder <= beatsize) {
						x->be_dur[x->be_index] 		= dur_remainder;
						x->be_tie[x->be_index] 		= x->b_tie[i];
						x->be_i[x->be_index] 		= x->b_i[i]; // input number (original input index)
						x->be_rii[x->be_index]		= x->b_rii[i]; // raw input number
						x->be_tup[x->be_index] 		= x->b_tup[i]; // tuplet
						x->be_bar_n[x->be_index]	= x->bar_n[i]; // bar number
						beatcount 					+= dur_remainder;
						x->be_beat_n[x->be_index]	= beat_number / x->bar_info[x->bar_n[i]][3];
						beat_number					+= x->be_dur[x->be_index];
						if (x->debug >= 1) post("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d", x->be_index, x->be_bar_n[x->be_index], x->be_beat_n[x->be_index], x->be_i[x->be_index], x->be_rii[x->be_index], x->be_dur[x->be_index], x->be_tie[x->be_index], x->be_tup[x->be_index]);
						if (x->debug >= 3) post("BEAT::::ELSE while <= beatcount = %d", beatcount);
						if (beatcount == beatsize) {
							beatcount = 0;
							if (x->debug >= 1) post("-      -      -      -      -      -      -      -      -      -      -      -      -      -");	
						}
						dur_remainder = 0;
						x->be_index++;
					}
					else {
						for(j=0; j<32; j++){
							if (dur_remainder < (beatsize * j)) { //&& pow2ordot(beatsize * j) == 1) {
								x->be_dur[x->be_index] 		= beatsize * (j - 1);
								break;
							}
						}
						dur_remainder 				-= x->be_dur[x->be_index];
						x->be_tie[x->be_index] 		= 1;
						x->be_i[x->be_index] 		= x->b_i[i]; // input number (original input index)
						x->be_rii[x->be_index]		= x->b_rii[i]; // raw input number
						x->be_tup[x->be_index] 		= x->b_tup[i]; // tuplet
						x->be_bar_n[x->be_index]	= x->bar_n[i]; // bar number
						x->be_beat_n[x->be_index]	= beat_number / x->bar_info[x->bar_n[i]][3];
						beat_number					+= x->be_dur[x->be_index];
						beatcount 					= 0;
						if (x->debug >= 1) post("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d", x->be_index, x->be_bar_n[x->be_index], x->be_beat_n[x->be_index], x->be_i[x->be_index], x->be_rii[x->be_index], x->be_dur[x->be_index], x->be_tie[x->be_index], x->be_tup[x->be_index]);
						if (x->debug >= 3) post("BEAT::::while else beatcount 	= %d", beatcount);
						if (x->debug >= 1) post("-      -      -      -      -      -      -      -      -      -      -      -      -      -");	
						x->be_index++;
					} 
				}
			}	
		}
	//// _______________________________________________ SUB-BEAT SUBDIVISION 
		for (j=1; j<3; j++) 			{
			if (x->debug >= 1) post("SUB-BEAT PROGRAM %d :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::", j);
			beatsize = x->bar_info[x->be_bar_n[0]][3];
			if (x->debug >= 1) post("....\nbeatsize=%d\n", beatsize);
			if (x->debug >= 1) post("i\tbar_n\tbeat_n\tclean_i\traw_i\tdur\ttie\ttuplet");
			if (x->debug >= 1) post("METER = %d/%d\t beatsize = %d", x->bar_info[x->be_bar_n[0]][1], x->bar_info[x->be_bar_n[0]][2], beatsize);			
			if (x->debug >= 1) post("BAR %d ===============================================", 0);
			x->sb_index = beatcount = 0;
			subdiv = j*2; if (x->debug >= 1) post("j=%d",j);
			sub_beatsize = beatsize / subdiv;
			for(i=0; i<x->be_index; i++){
				if 		(i > 0	&&	x->be_bar_n[i] 		!= x->be_bar_n[i-1]) {
					if (x->debug >= 1) post("BAR %d ============================================ BAR %d", x->be_bar_n[i], x->be_bar_n[i]);
					if (x->debug >= 1) post("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - %d", x->be_beat_n[i]);
					if (x->bar_info[x->be_bar_n[i]][1] != x->bar_info[x->be_bar_n[i-1]][1] ||
						x->bar_info[x->be_bar_n[i]][2] != x->bar_info[x->be_bar_n[i-1]][2] ) {
						beatsize = x->bar_info[x->be_bar_n[i]][3];
						sub_beatsize = beatsize / subdiv;
						if (sub_beatsize < 2) sub_beatsize = 2;
						if (x->debug >= 1) post("METER = %d/%d\t beatsize = %d\t sub_beatsize = %d", x->bar_info[x->be_bar_n[i]][1], x->bar_info[x->be_bar_n[i]][2], beatsize, sub_beatsize);			
						beatcount = 0;
					}
					while (beatcount >= sub_beatsize) {
						beatcount -= sub_beatsize;
					//	post("beatcount =\t%d", beatcount);
					}
				}
				else if (i > 0	&&	x->be_beat_n[i] 	!= x->be_beat_n[i-1] ) {
					if (x->debug >= 1) post("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - %d", x->be_beat_n[i]);
					while (beatcount >= sub_beatsize) {
						beatcount -= sub_beatsize;
					}
				}
				if (sub_beatsize < 4) {
					x->sb_dur[x->sb_index] 		=	x->be_dur[i];
					x->sb_tie[x->sb_index] 		= 	x->be_tie[i];
					x->sb_i[x->sb_index] 		= 	x->be_i[i]; // input number (original input index)
					x->sb_rii[x->sb_index]		= 	x->be_rii[i]; // raw input number
					x->sb_tup[x->sb_index] 		= 	x->be_tup[i]; // tuplet
					x->sb_bar_n[x->sb_index]	= 	x->be_bar_n[i]; // bar number
					x->sb_beat_n[x->sb_index]	= 	x->be_beat_n[i];
					if (x->debug >= 1) post("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d", x->sb_index, x->sb_bar_n[x->sb_index], x->sb_beat_n[x->sb_index], x->sb_i[x->sb_index], x->sb_rii[x->sb_index], x->sb_dur[x->sb_index], x->sb_tie[x->sb_index], x->sb_tup[x->sb_index]);
					x->sb_index++;
					beatcount += x->be_dur[i];	
					if( (((beatcount + x->be_dur[i]) == sub_beatsize) || ((beatcount + x->be_dur[i]) % sub_beatsize == 0)) && (sub_beatsize > 2) ) {
						if (x->debug >= 1) post(".   .    .    .    .    .    .    .    .    .    .    .    .    .    .   .    .    .    .    .    .    .    .    .    .    .    .    .    .%d", sub_beatsize);
					}
					while (beatcount >= sub_beatsize) {
						beatcount -= sub_beatsize;
					}
				}
				else {
					if	( ((beatcount + x->be_dur[i]) <= sub_beatsize)  ) {
						x->sb_dur[x->sb_index] 		=	x->be_dur[i];
						x->sb_tie[x->sb_index] 		= 	x->be_tie[i];
						x->sb_i[x->sb_index] 		= 	x->be_i[i]; // input number (original input index)
						x->sb_rii[x->sb_index]		= 	x->be_rii[i]; // raw input number
						x->sb_tup[x->sb_index] 		= 	x->be_tup[i]; // tuplet
						x->sb_bar_n[x->sb_index]	= 	x->be_bar_n[i]; // bar number
						x->sb_beat_n[x->sb_index]	= 	x->be_beat_n[i];
						if (x->debug >= 1) post("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d", x->sb_index, x->sb_bar_n[x->sb_index], x->sb_beat_n[x->sb_index], x->sb_i[x->sb_index], x->sb_rii[x->sb_index], x->sb_dur[x->sb_index], x->sb_tie[x->sb_index], x->sb_tup[x->sb_index]);
						x->sb_index++;
						beatcount += x->be_dur[i];	
						if( (((beatcount + x->be_dur[i]) == sub_beatsize) || ((beatcount + x->be_dur[i]) % sub_beatsize == 0)) && (sub_beatsize > 2) ) {
							if (x->debug >= 1) post(".   .    .    .    .    .    .    .    .    .    .    .    .    .    .   .    .    .    .    .    .    .    .    .    .    .    .    .    .%d", sub_beatsize);
						}
						while (beatcount >= sub_beatsize) {
							beatcount -= sub_beatsize;
						}	
					} /// less than sub_beatsize
					else if ( (((beatcount % sub_beatsize) == 0) && (((beatcount + x->be_dur[i]) % sub_beatsize) == 0)) 		&& (sub_beatsize > 2) ) {
						x->sb_dur[x->sb_index] 		=	x->be_dur[i];
						x->sb_tie[x->sb_index] 		= 	x->be_tie[i];
						x->sb_i[x->sb_index] 		= 	x->be_i[i]; // input number (original input index)
						x->sb_rii[x->sb_index]		= 	x->be_rii[i]; // raw input number
						x->sb_tup[x->sb_index] 		= 	x->be_tup[i]; // tuplet
						x->sb_bar_n[x->sb_index]	= 	x->be_bar_n[i]; // bar number
						x->sb_beat_n[x->sb_index]	= 	x->be_beat_n[i];
						if (x->debug >= 1) post("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d", x->sb_index, x->sb_bar_n[x->sb_index], x->sb_beat_n[x->sb_index], x->sb_i[x->sb_index], x->sb_rii[x->sb_index], x->sb_dur[x->sb_index], x->sb_tie[x->sb_index], x->sb_tup[x->sb_index]);
						x->sb_index++;
						beatcount += x->be_dur[i];
						if (beatcount % sub_beatsize == 0) if (x->debug >= 1) post(".   .    .    .    .    .    .    .    .    .    .    .    .    .    .   .    .    .    .    .    .    .    .    .    .    .    .    .    .%d", sub_beatsize);
						while (beatcount >= sub_beatsize) {
							beatcount -= sub_beatsize;
						//	post("beatcount =\t%d", beatcount);
						}
					} /// for cases like 4 12 or 8 24 in a subbeat of 8 or 16 OR if beatcount = 0 and duration == 48, 64, 16, etc...
					else if ( (((beatcount % (sub_beatsize/2)) == 0) && (((beatcount + x->be_dur[i]) % (sub_beatsize/2)) == 0)) && (sub_beatsize > 2) ) {
						x->sb_dur[x->sb_index] 		=	x->be_dur[i];
						x->sb_tie[x->sb_index] 		= 	x->be_tie[i];
						x->sb_i[x->sb_index] 		= 	x->be_i[i]; // input number (original input index)
						x->sb_rii[x->sb_index]		= 	x->be_rii[i]; // raw input number
						x->sb_tup[x->sb_index] 		= 	x->be_tup[i]; // tuplet
						x->sb_bar_n[x->sb_index]	= 	x->be_bar_n[i]; // bar number
						x->sb_beat_n[x->sb_index]	= 	x->be_beat_n[i];
						if (x->debug >= 1) post("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d", x->sb_index, x->sb_bar_n[x->sb_index], x->sb_beat_n[x->sb_index], x->sb_i[x->sb_index], x->sb_rii[x->sb_index], x->sb_dur[x->sb_index], x->sb_tie[x->sb_index], x->sb_tup[x->sb_index]);
			
						x->sb_index++;
						beatcount += x->be_dur[i];
						if (beatcount % sub_beatsize == 0) 	if (x->debug >= 1) post(".   .    .    .    .    .    .    .    .    .    .    .    .    .    .   .    .    .    .    .    .    .    .    .    .    .    .    .    .%d", sub_beatsize);
						while (beatcount >= sub_beatsize) {
							beatcount -= sub_beatsize;
						//	post("beatcount =\t%d", beatcount);
						}
					} /// for cases like 4 12 or 8 24 in a subbeat of 8 or 16 OR if beatcount = 0 and duration == 48, 64, 16, etc...
					else {
						if (sub_beatsize > 2) {
							x->sb_dur[x->sb_index] 		= sub_beatsize - beatcount;
							dur_remainder				= x->be_dur[i] - x->sb_dur[x->sb_index];
							x->sb_tie[x->sb_index] 		= 1;
							x->sb_i[x->sb_index] 		= x->be_i[i]; // input number (original input index)
							x->sb_rii[x->sb_index]		= 	x->be_rii[i]; // raw input number
							x->sb_tup[x->sb_index] 		= 	x->be_tup[i]; // tuplet
							x->sb_bar_n[x->sb_index]	= x->be_bar_n[i]; // bar number
							x->sb_beat_n[x->sb_index]	= x->be_beat_n[i];
							if (x->debug >= 1) post("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d", x->sb_index, x->sb_bar_n[x->sb_index], x->sb_beat_n[x->sb_index], x->sb_i[x->sb_index], x->sb_rii[x->sb_index], x->sb_dur[x->sb_index], x->sb_tie[x->sb_index], x->sb_tup[x->sb_index]);
							if ((beatcount + x->sb_dur[x->sb_index]) % sub_beatsize == 0) if (x->debug >= 1)  post(".   .    .    .    .    .    .    .    .    .    .    .    .    .    .   .    .    .    .    .    .    .    .    .    .    .    .    .    .%d", sub_beatsize);
							x->sb_index++;
				
							x->sb_dur[x->sb_index] 		= dur_remainder;
							x->sb_tie[x->sb_index] 		= x->be_tie[i];
							x->sb_i[x->sb_index] 		= x->be_i[i]; // input number (original input index)
							x->sb_rii[x->sb_index]		= 	x->be_rii[i]; // raw input number
							x->sb_tup[x->sb_index] 		= 	x->be_tup[i]; // tuplet
							x->sb_bar_n[x->sb_index]	= x->be_bar_n[i]; // bar number
							x->sb_beat_n[x->sb_index]	= x->be_beat_n[i];
							if (x->debug >= 1) post("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d", x->sb_index, x->sb_bar_n[x->sb_index], x->sb_beat_n[x->sb_index], x->sb_i[x->sb_index], x->sb_rii[x->sb_index], x->sb_dur[x->sb_index], x->sb_tie[x->sb_index], x->sb_tup[x->sb_index]);
							x->sb_index++;
				
							beatcount 					+= x->be_dur[i];
							if (beatcount % sub_beatsize == 0) 	if (x->debug >= 1) post(".   .    .    .    .    .    .    .    .    .    .    .    .    .    .   .    .    .    .    .    .    .    .    .    .    .    .    .    .%d", sub_beatsize);
							while (beatcount >= sub_beatsize) {
								beatcount -= sub_beatsize;
							//	post("beatcount =\t%d", beatcount);
							}
						}
					}
				}
			}
			for( i=0; i<x->sb_index; i++ ){
				x->be_dur[i] 	= x->sb_dur[i];
				x->be_tie[i] 	= x->sb_tie[i];
				x->be_i[i]		= x->sb_i[i]; // input number (original input index)
				x->be_rii[i]	= x->sb_rii[i]; // raw input number
				x->be_tup[i] 	= x->sb_tup[i]; // tuplet
				x->be_bar_n[i]	= x->sb_bar_n[i]; // bar number
				x->be_beat_n[i]	= x->sb_beat_n[i];
				x->be_index		= x->sb_index;
			}		
		}
		post("notes: ... Rhythmic computation done");
	//// ____________________________________   produce a single final list 
		if (x->debug >= 1) post("\n......... beginning final list!!!!! ");
		ii = 0;
		x->sb_index = 0;
		if (x->debug >= 1) post("i\tbar_n\tbeat_n\tclean_i\traw_i\tdur\ttie\ttuplet");
		for(i=0; i<x->be_index; i++)	{
		//	post("i=%d", i);
			if (x->be_bar_n[i] != x->be_bar_n[i-1]) if (x->debug >= 1) post("_______________________________________________________________________________________");
			if (x->be_beat_n[i] != x->be_beat_n[i-1]) if (x->debug >= 1) post("-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -");
			if (x->be_tup[i] >= 0) {
				while( x->sb_tp_num[ii] == x->be_tup[i] && ii<x->sb_tp_index) {
		//			post("ii=%d", ii);
					x->sb_dur[x->sb_index]		= x->sb_tp_dur[ii];
					x->sb_tie[x->sb_index]		= x->sb_tp_tie[ii];
					x->sb_i[x->sb_index]		= x->sb_tp_i[ii];
					x->sb_rii[x->sb_index]		= x->sb_tp_ri[ii];
					x->sb_tup[x->sb_index]		= x->sb_tp_num[ii];
					x->sb_bar_n[x->sb_index]	= x->be_bar_n[i];
					x->sb_beat_n[x->sb_index]	= x->be_beat_n[i];
					if (x->debug >= 1) post("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\ttuplet_loop", x->sb_index, x->sb_bar_n[x->sb_index], x->sb_beat_n[x->sb_index], x->sb_i[x->sb_index], x->sb_rii[x->sb_index], x->sb_dur[x->sb_index], x->sb_tie[x->sb_index], x->sb_tup[x->sb_index]);
					x->sb_index++;
					ii++;
				}
			}
			else {
				x->sb_dur[x->sb_index] 		= x->be_dur[i];
				x->sb_tie[x->sb_index] 		= x->be_tie[i];
				x->sb_i[x->sb_index]		= x->be_i[i]; // input number (original input index)
				x->sb_rii[x->sb_index]		= x->be_rii[i]; // raw input number
				x->sb_tup[x->sb_index] 		= x->be_tup[i]; // tuplet
				x->sb_bar_n[x->sb_index]	= x->be_bar_n[i]; // bar number
				x->sb_beat_n[x->sb_index]	= x->be_beat_n[i];
				if (x->debug >= 1) post("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\tregular_loop", x->sb_index, x->sb_bar_n[x->sb_index], x->sb_beat_n[x->sb_index], x->sb_i[x->sb_index], x->sb_rii[x->sb_index], x->sb_dur[x->sb_index], x->sb_tie[x->sb_index], x->sb_tup[x->sb_index]);			
				x->sb_index++;
			}			
		}
		post("notes: ... Formatting Score... ");
	//// _______________________________________________ ENGRAVING Programs
		char o_pitch[10], o_jump[10], o_text[64], osname[20], os_osx[20], os_linux[20], command[150];
		int o_dur, t, f, tp;	
		char buf[MAXPDSTRING], partname[MAXPDSTRING], scorename[MAXPDSTRING], linename[MAXPDSTRING];
		FILE *fp1, *fp2;
	//// _______________________________________________ OPEN FILE		
		canvas_makefilename(x->x_canvas, s->s_name, buf, MAXPDSTRING); /// This is a Pd function to get the path relative to the canvas
		sys_bashfilename(buf, buf);
		strcpy( scorename, buf);
		strcat( scorename, ".ly");
		fp1 = fopen(scorename, "w");
		if(!fp1)						{
			error("%s: couldn't create", buf);
			return;
		}
	//// _______________________________________________ WRITE SCORE	
		else 							{  
			post("notes: writing into %s", scorename);
			fprintf(fp1, "%% [notes] external for Pure Data\n%% development-version July 14, 2014 \n%% by Jaime E. Oliver La Rosa\n%% la.rosa@nyu.edu\n%% @ the Waverly Labs in NYU MUSIC FAS\n%% Open this file with Lilypond\n%% more information is available at lilypond.org\n%% Released under the GNU General Public License.\n\n");
			if (x->auth_n > 0 || x->titl_n > 0 || x->sub_title_n > 0) {
				fprintf(fp1, "\n\\header {");
				if (x->auth_n 		> 0) {
					fprintf(fp1, "\n\tcomposer = \"");
					for(i=0; i<x->auth_n; i++){
						fprintf(fp1, "%1.32s ", x->author[i]);
					}
					fprintf(fp1, "\"");
				}
				if (x->titl_n 		> 0) {
					fprintf(fp1, "\n\ttitle = \"");
					for(i=0; i<x->titl_n; i++){
						fprintf(fp1, "%1.32s ", x->title[i]);
					}
					fprintf(fp1, "\"");
				}
				if (x->sub_title_n 	> 0) {
					fprintf(fp1, "\n\tsubtitle = \"");
					for(i=0; i<x->sub_title_n; i++){
						fprintf(fp1, "%1.32s ", x->sub_title[i]);
					}
					fprintf(fp1, "\"");
				}
				fprintf(fp1, "\n}\n\n");
			} //	HEADER Title, subtitle, and Author
		//// ___________________________________________________ OPEN PART FILE
			strcpy( partname, buf);
			strcat( partname, "_part.ly");
			fp2 = fopen(partname, "w");
			if(!fp2)						{
				error("%s: couldn't create", buf);
				return;
			}
			else {	
				strcpy(linename, x->inst);
				strcat(linename, "_part = ");
				fprintf(fp2, "%s", linename);
				for (i=0; i<x->sb_index; i++)	{
					j = x->sb_rii[i];
				//	post("%d, pittttt=%f", j, x->ri_pit[j][0]);
					if (x->ri_pit[j][0] > 0) {
				//		post("found first pitch");
						temp_f = x->ri_pit[j][0];
						find_relative((int) temp_f, (FILE *) fp2);
						x->lastpit = (int) temp_f;
						break;
					}
				} 				// find relative
				fprintf(fp2, "\\time %i/%i\n\n", x->bar_info[0][1], x->bar_info[0][2]); // METER
				if(x->tempo == 1) 				{
					for(k=0; k<64; k++){
						t = (int) exp2( (float) k );
						if 		( x->ri_tmp[0][0] == t ) {
						k=t;
						t=0;
						k = x->refdur / k;
						fprintf(fp2, "\\tempo %i = %i\n\n", k, x->ri_tmp[0][1]);
						break;
					}
						else if ( x->ri_tmp[0][0] == t / 2 * 3) {
						k=t;
						t=1;
						k = x->refdur / k;
						fprintf(fp2, "\\tempo %i. = %i\n\n", k, x->ri_tmp[0][1]);
						break;
					}
					}
				} 								// TEMPO
				find_clef((int) x->ri_clef[0], (FILE *) fp2);		//	CLEF
			//// __________________________________________ WRITE SOME NOTES
				fprintf(fp2, "\n%% ________________________________________bar 1 :\n");	
				f = 1;	tp = 0;
				for (i=0; i<x->sb_index; i++)	{
					j = x->sb_rii[i];		
			///////////////////////////// SEPARATION
				// close tupletttt: ________________________________________________________	
					if (	i > 0 &&((x->sb_tup[i] > x->sb_tup[i-1]	&& x->sb_tup[i-1] > -1	) || (x->sb_tup[i] == -1	&& x->sb_tup[i-1] > -1	))	) {
						fprintf(fp2, "} ");	
						tp = 0;	
					}
				// BAR BREAK ________________________________________________________
					if (i>0 && x->sb_bar_n[i] != x->sb_bar_n[i-1]) {
						if(f==2) fprintf(fp2, "]");
						fprintf(fp2, " |\n%% ________________________________________bar %i :\n", x->sb_bar_n[i]+1);
						f = 1;
					// write meter
						if (	(i>0) && (x->bar_info[x->sb_bar_n[i]][1] != x->bar_info[x->sb_bar_n[i-1]][1] || x->bar_info[x->sb_bar_n[i]][2] != x->bar_info[x->sb_bar_n[i-1]][2] )	)  {
							fprintf(fp2, "\\time %i/%i\n\n", x->bar_info[x->sb_bar_n[i]][1], x->bar_info[x->sb_bar_n[i]][2]);	
						}
					}
				// BEAT Tab ________________________________________________________
					else if (i>0 && x->sb_beat_n[i] != x->sb_beat_n[i-1]) {
						if(f==2) fprintf(fp2, "]");
						fprintf(fp2, "\n");
						for (k=0; k<x->sb_beat_n[i]; k++) {
							fprintf(fp2, "\t");
						}
						f = 1;
					}
				// SPACE ________________________________________________________
					else {
						fprintf(fp2, " ");
					}
				// write tempo
					if(x->tempo == 1) {
						if (	(x->ri_tmp[j][0] != x->ri_tmp[j-1][0] || x->ri_tmp[j][1] != x->ri_tmp[j-1][1]) && j != x->sb_rii[i-1] )	{
							for(k=0; k<64; k++){
								t = (int) exp2( (float) k );
								if 		( x->ri_tmp[j][0] == t ) {
									k=t;
									t=0;
									k = x->refdur / k;
									fprintf(fp2, " \\tempo %i = %i ", k, x->ri_tmp[j][1]);
									break;
								}
								else if ( x->ri_tmp[j][0] == t / 2 * 3) {
									k=t;
									t=1;
									k = x->refdur / k;
									fprintf(fp2, " \\tempo %i. = %i ", k, x->ri_tmp[j][1]);
									break;
								}
							}
						}
					}	
			//////////////////////////// NOTE EXPRESSION
				// Clef
					if (i>0 && x->ri_clef[j] != x->ri_clef[j-1]) {
						find_clef((int) x->ri_clef[j], (FILE *) fp2);				
					}
				// articulation ________________________________________________________
					if (x->ri_art_n[j] > 0 && j != x->sb_rii[i-1]) {
						for (k=0; k<x->ri_art_n[j]; k++){
							if (x->ri_art[j][k]==24)find_articulation((int) x->ri_art[j][k], (FILE *) fp2);
						}
					}
					else if (i == 0 && x->ri_art_n[j] > 0) {
						for (k=0; k<x->ri_art_n[j]; k++){
							if (x->ri_art[j][k]==24)find_articulation((int) x->ri_art[j][k], (FILE *) fp2);
						}
					}
				// tuplet start ________________________________________________________
					if (x->sb_tup[i] > -1 && x->sb_tup[i] != x->sb_tup[i-1] ) {
						fprintf(fp2, " \\tuplet %i/%i { ", x->tp_info[x->sb_tup[i]][1], x->tp_info[x->sb_tup[i]][2]);
						tp = 1;
					}
				// PITCH ________________________________________________________
				// chord run ________________________________________________________	
					if (x->ri_cho[j]>1) fprintf(fp2, "<");
					for (k=0; k<x->ri_cho[j]; k++) {
						// pitch
						find_pitch(x->ri_pit[j][k], x->ri_acc[j], o_pitch); 
						fprintf(fp2, "%1.8s", o_pitch);
						// jumps
						if (i>0) { // interval jumps
							if (k==0) {
								if (find_jump((int) x->ri_pit[j][k], (int) x->lastpit, o_jump)==1)
								fprintf(fp2, "%1.8s", o_jump);
							}
							else	{
								if (find_jump((int) x->ri_pit[j][k], (int) x->lastpit_ch, o_jump)==1)
								fprintf(fp2, "%1.8s", o_jump);
							}
						}
						else {
							if (k==0) {
								if (find_jump((int) x->ri_pit[j][k], (int) x->lastpit, o_jump)==1)
								fprintf(fp2, "%1.8s", o_jump);
							}
							else	{
								if (find_jump((int) x->ri_pit[j][k], (int) x->lastpit_ch, o_jump)==1)
								fprintf(fp2, "%1.8s", o_jump);
							}
						}
						if (x->ri_cho[j]>1) fprintf(fp2, " ");
						if (x->ri_pit[j][0] > 0)	x->lastpit 		= x->ri_pit[j][0];
						if (x->ri_pit[j][k] > 0)	x->lastpit_ch 	= x->ri_pit[j][k];
					}
					if (x->ri_cho[j]>1) fprintf(fp2, ">");
				// DURATION ________________________________________________________
					for(k=0; k<64; k++){
						t = (int) exp2( (float) k );
						if 		( x->sb_dur[i] == t ) {
							k=t;
							t=0;
							break;
						}
						else if ( x->sb_dur[i] == t / 2 * 3) {
							k=t;
							t=1;
							break;
						}
					}
					o_dur = x->refdur / k;
					fprintf(fp2, "%i", o_dur);
				// dot ________________________________________________________
					if (t==1) fprintf(fp2, ".");
				// tremolo ________________________________________________________
					if (x->ri_trm[j] > 0) fprintf(fp2, ":%i", x->ri_trm[j]);
				// tie ________________________________________________________
					if (x->sb_tie[i] == 1) fprintf(fp2, "~");
				// smallnumbers ________________________________________________________
					if (x->ri_smn_n[j] > 0 && j != x->sb_rii[i-1]) {
						for (k=0; k<x->ri_smn_n[j]; k++){
							find_small_numbers((int) x->ri_smn[j][k], (FILE *) fp2);
						}
					}
					else if (i == 0 && x->ri_smn_n[j] > 0) {
						for (k=0; k<x->ri_smn_n[j]; k++){
							find_small_numbers((int) x->ri_smn[j][k], (FILE *) fp2);
						}
					}
				// articulation ________________________________________________________
					if (x->ri_art_n[j] > 0 && j != x->sb_rii[i-1]) {
						for (k=0; k<x->ri_art_n[j]; k++){
							if (x->ri_art[j][k]!=24)find_articulation((int) x->ri_art[j][k], (FILE *) fp2);
						}
					}
					else if (i == 0 && x->ri_art_n[j] > 0) {
						for (k=0; k<x->ri_art_n[j]; k++){
							if (x->ri_art[j][k]!=24)find_articulation((int) x->ri_art[j][k], (FILE *) fp2);
						}
					}
				// dynamics ________________________________________________________
					if (x->ri_dyn_n[j] > 0 && j != x->sb_rii[i-1]) {
						for (k=0; k<x->ri_dyn_n[j]; k++){
							find_dynamics((int) x->ri_dyn[j][k], (FILE *) fp2);
						}
					}
					else if (i == 0 && x->ri_dyn_n[j] > 0) {
						for (k=0; k<x->ri_dyn_n[j]; k++){
							find_dynamics((int) x->ri_dyn[j][k], (FILE *) fp2);
						}
					}
				// spans ________________________________________________________
					if (x->ri_spa_n[j] > 0 && j != x->sb_rii[i-1]) {
						for (k=0; k<x->ri_spa_n[j]; k++){
							//post("BANG");
							find_span((int) x->ri_spa[j][k], (FILE *) fp2);
						}
					}
					else if (i == 0 && x->ri_spa_n[j] > 0) {
						for (k=0; k<x->ri_spa_n[j]; k++){
							//post("BONG");
							find_span((int) x->ri_spa[j][k], (FILE *) fp2);
						}
					}
				// texts ________________________________________________________
					if (x->ri_txt_n[j] > 0 && j != x->sb_rii[i-1]) {
						fprintf(fp2, "-\\markup {");
						for (k=0; k<x->ri_txt_n[j]; k++){
							strcpy( o_text, x->ri_txt[j][k]);
							fprintf(fp2, "%1.32s ", o_text);
						}
						fprintf(fp2, "}");
					}
					else if (i == 0 && x->ri_txt_n[j] > 0) {
						fprintf(fp2, "_\\markup {");
						for (k=0; k<x->ri_txt_n[j]; k++){
							strcpy( o_text, x->ri_txt[j][k]);
							fprintf(fp2, "%1.32s ", o_text);
						}
						fprintf(fp2, "}");
					}
				// beat grouping ________________________________________________________	
					if (f == 1 && tp != 1) {
						if ( x->sb_beat_n[i] == x->sb_beat_n[i+1] && x->sb_bar_n[i] == x->sb_bar_n[i+1] ) {
							fprintf(fp2, "[");
							f = 2;
						}
						else f = 0;
					}
				}
				if(tp == 1) fprintf(fp2, "}");
				if(f  == 2) fprintf(fp2, "]");
				fprintf(fp2, "\n}\n");
				fclose( fp2);
			//// _______________________________________ COPY PART TO MAIN SCORE
				fp2 = fopen(partname, "r");
				char a;
				a = fgetc(fp2);
				if (a != EOF) fputc(a,  fp1);
				while(a != EOF)	{
					a = fgetc(fp2);
					if (a != EOF) fputc(a,  fp1);
				}
				fclose(fp2);
			//// __________________________________________ SCORE EXPRESSION
				strcpy(linename, x->inst);
				strcat(linename, "_part");
				fprintf(fp1, "\n\\score {\n\t\\new Staff ");
				if (x->inst_n == 1) fprintf(fp1, "\\with { instrumentName = \"%s\" } ", x->inst);
				fprintf(fp1, "{\n\t\t\\new Voice {\n\t\t\t\\%s\n\t\t}\n\t}\n\t\\layout {", linename);	
				fprintf(fp1, "\n\t\t\\mergeDifferentlyHeadedOn");
				fprintf(fp1, "\n\t\t\\mergeDifferentlyDottedOn");
				fprintf(fp1, "\n\t\t\\set Staff.pedalSustainStyle = #'mixed");
				fprintf(fp1, "\n\t\t#(set-default-paper-size \"a%i", x->papersize);
				if(x->paperorientation==1) fprintf(fp1, "landscape\")");
				else fprintf(fp1, "\")");
				fprintf(fp1, "\n\t}\n\t\\midi { }\n}");
			//// ____________________________________________________________ CLOSE SCORE 
				fprintf(fp1, "\n\n\\version \"2.18.2\"\n%% notes Pd External version testing \n");
				fclose(fp1);
				post("notes: .ly score finished");
				
				t_atom paths[MAXPDSTRING];
				SETSYMBOL(&paths[0], gensym(x->inst));
				SETSYMBOL(&paths[1], gensym(partname));
   				outlet_list(x->x_outlet0, 0, 2, paths);
			}	
		//// ____________________________________________________________ COMPILE AND OPEN
	/*		char dirresult[MAXPDSTRING], *nameresult;
    		int fd;
			post("scorename = %s", scorename);
			fd = canvas_open(x->x_canvas, scorename, "", dirresult, &nameresult, MAXPDSTRING, 1);
			post("dirresult = %s", dirresult);
			post("nameresult = %s", nameresult);*/		
			if (x->SLAVE == 0 && x->render == 1) 				{
				strcpy( os_osx, "Darwin");
				strcpy( os_linux, "Linux");
				uname(&unameData);
				strcpy( osname, unameData.sysname);
				if (strncmp(osname, os_osx, 5) == 0) {
					post("notes: Adjusting directories to OS X");
					x->OS = 0;
				} // 0 is os x
				if (strncmp(osname, os_linux, 5) == 0) {
					post("notes: Adjusting directories to Linux");
					x->OS = 1;
				} // 1 is linux
				post("notes: compiling score ");
			////	RENDER lilypond SCORE ________________________________________________
				if(x->OS == 0)	{
					strcpy( command, "exec ");
					strcat( command, x->lily_dir);
					strcat( command, "/");
					strcat( command, "lilypond -o ");
					strcat( command, x->lily_dir);
				}
				if(x->OS == 1)	{
					strcpy( command, "lilypond -o ");
					strcat( command, "~");
				}
				strcat( command, "/temp ");
				strcat( command, scorename);
				system( command);
				post("notes: Opening PDF score ");
			////	OPEN PDF SCORE ________________________________________________________
				if(x->OS == 1)	{
					strcpy( command, "xdg-open ");
					strcat( command, "~");
				}
				if(x->OS == 0)	{
					strcpy( command, "open ");
					strcat( command, x->lily_dir);
				}				
				strcat( command, "/temp.pdf");
				system( command);
			}
		} // if a file is correctly provided.
	} 
	else 									{
		post("notes: ERROR: Can't flush because no input has been provided... you have to enter some notes!"); 
	} //exit(1);
	outlet_bang(x->x_outlet2);
}
////	____________________________________________________ CLEAR
void notes_clear(t_notes *x)											{	
	x->auth_n = x->titl_n = x->sb_tp_index = x->i_index = x->ri_index = x->tp_index = x->tp_n = x->b_index = x->be_index = x->sb_index = 0;
	x->refdur = 32;
 //   x->debug = 0;
    x->tempo = 0;
    x->auth_n = x->titl_n = x->sub_title_n = 0; 
    x->ri_mtr[x->ri_index][0] = x->ri_mtr[x->ri_index][1] = 4;
	post("notes: Cleared"); 
}
////	____________________________________________________ PRINT
void notes_print(t_notes *x)											{	
	post("notes: x->sb_tp_index = %d\nx->i_index = %d\nx->ri_index = %d\nx->tp_index = %d\nx->tp_n = %d\nx->b_index = %d\nx->be_index = %d\nx->sb_index = %d\n", x->sb_tp_index, x->i_index, x->ri_index, x->tp_index, x->tp_n, x->b_index, x->be_index, x->sb_index);
} 
////	____________________________________________________ REFERENCE DURATION
void notes_refdur(t_notes *x, t_floatarg f) 							{	
	x->refdur = f;
	post("notes: Reference duration set to a 1/%i note", x->refdur);
}
////	____________________________________________________ RENDERING SWITCH
void notes_render(t_notes *x, t_floatarg f) 							{	
	int i;
	if (f == 0) {
		i = 0;
		post("notes: score rendering off");
	}
	else {
		i = 1;
		post("notes: score rendering on");
	}
	x->render = i;
}
////	____________________________________________________ DEBUG  
void notes_debug(t_notes *x, t_floatarg f) 								{	
	x->debug = (int) f;
	post("debugging level = %d", x->debug);
} 
////	____________________________________________________ TITLE
void notes_title(t_notes *x, t_symbol *s, int argc, t_atom *argv) 		{
	x->dummysym = s->s_name;
	int i;
	for(i=0;i<argc;i++){
		atom_string(argv+i, x->title[i], 1000);
	//	post(x->title[i]);
	}	
	x->titl_n = argc;
}
////	____________________________________________________ SUB_TITLE  
void notes_sub_title(t_notes *x, t_symbol *s, int argc, t_atom *argv) 	{
	x->dummysym = s->s_name;
	int i;
	for(i=0;i<argc;i++){
		atom_string(argv+i, x->sub_title[i], 1000);
	//	post(x->sub_title[i]);
	}	
	x->sub_title_n = argc;
}
////	____________________________________________________ AUTHOR
void notes_author(t_notes *x, t_symbol *s, int argc, t_atom *argv) 		{
	x->dummysym = s->s_name;
	int i;
	for(i=0;i<argc;i++){
		atom_string(argv+i, x->author[i], 1000);
	//	post(x->author[i]);
	}
	x->auth_n = argc;	
}
////	____________________________________________________ AUTHOR
void notes_inst(t_notes *x, t_symbol *s) 		{
	strcpy (x->inst, s->s_name);
	post("notes: instrument name = %s", x->inst);
	x->inst_n = 1;	
}
////	____________________________________________________ Lilypond Location
void notes_lily_dir(t_notes *x, t_symbol *s, int argc, t_atom *argv) 	{
	int i; i = argc;
	x->dummysym = s->s_name;
	atom_string(argv, x->lily_dir, 1000);
	post("notes: Lilypond directory set to: %s", x->lily_dir);
}
////	____________________________________________________ READFILE
void notes_readfile(t_notes *x, t_symbol *s)							{
	char filename[100];
	strcpy( filename, s->s_name);
	strcpy( x->barfile, filename);
	post("notes: Master Bar File RECEIVED at %s", x->barfile);
}
////	____________________________________________________ PAPER SIZE
void notes_paper(t_notes *x, t_floatarg f, t_floatarg g) 								{	
	x->papersize 		= (int) f;
	x->paperorientation = (int) g;
} 

t_class *notes_class;
void *notes_new(t_floatarg ff)												{
    t_notes *x = (t_notes *)pd_new(notes_class);
	inlet_new(&x->x_ob, &x->x_ob.ob_pd, gensym("symbol"), gensym("readfile"));
  	x->x_outlet0 = outlet_new(&x->x_ob, &s_symbol);
  	x->x_outlet1 = outlet_new(&x->x_ob, &s_symbol);
  	x->x_outlet2 = outlet_new(&x->x_ob, &s_symbol);
    x->x_canvas = canvas_getcurrent();
    x->SLAVE 			= (int) ff;
    x->sb_tp_index = x->i_index = x->ri_index = x->tp_index = x->tp_n = x->b_index = x->be_index = 0;
    x->debug = x->inst_n = x->tempo = x->auth_n = x->titl_n = 0; 
    x->refdur 			= 32;
    x->render 			= 1;
    x->papersize 		= 4;
	x->paperorientation = 0;
	strcpy( x->lily_dir, "~/bin");
    strcpy (x->inst, "inst");
    return (void *)x;
}
void notes_setup(void)														{
    notes_class = 	class_new(gensym("notes"), (t_newmethod)notes_new, 0, sizeof(t_notes), 0, A_DEFFLOAT, 0);

	class_addmethod(notes_class, (t_method)notes_input, 	gensym("input"), 	A_GIMME, 	0);
	class_addmethod(notes_class, (t_method)notes_write, 	gensym("write"), 	A_SYMBOL,	0);	
	class_addmethod(notes_class, (t_method)notes_title, 	gensym("title"), 	A_GIMME, 	0);
	class_addmethod(notes_class, (t_method)notes_sub_title, gensym("sub_title"),A_GIMME, 	0);
	class_addmethod(notes_class, (t_method)notes_author, 	gensym("author"), 	A_GIMME, 	0);
	class_addmethod(notes_class, (t_method)notes_inst, 		gensym("inst"), 	A_SYMBOL, 	0);
	class_addmethod(notes_class, (t_method)notes_lily_dir, 	gensym("lily_dir"), A_GIMME,	0);
	class_addmethod(notes_class, (t_method)notes_clear, 	gensym("clear"), 				0);
	class_addmethod(notes_class, (t_method)notes_readfile, 	gensym("readfile"),	A_SYMBOL, 	0);
	class_addmethod(notes_class, (t_method)notes_print, 	gensym("print"), 				0);
	class_addmethod(notes_class, (t_method)notes_refdur, 	gensym("refdur"), 	A_DEFFLOAT, 0);
	class_addmethod(notes_class, (t_method)notes_render, 	gensym("render"), 	A_DEFFLOAT, 0);
	class_addmethod(notes_class, (t_method)notes_debug, 	gensym("debug"), 	A_DEFFLOAT, 0);
	class_addmethod(notes_class, (t_method)notes_paper, 	gensym("paper"), 	A_DEFFLOAT, A_DEFFLOAT, 0);
	post("notes:\t\ttesting version: 2014-07-21");
}