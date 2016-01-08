/*  Original Author: Peter Meyer
    Conversion to C of Matthew Watkins' Maple code for
    his formula for the 384 TWZ data points.
    Last mod.: 1996-05-27
 */


// Ported to Linux
// 18 Jan 2016
// John A Phelps
// kl4yfd@gmail.com


/*

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>

09 Dec 2012
*/


#include <stdio.h>

int h[64], w[400];

void main(void);
void initialize_h(void);
int mod_64(int i);
int exp_minus_one(int i);
void print_data_points(void);

/*-------------------*/

// This is the KING WEN sequence without the half-twist
void initialize_h(void)
{
/*  The "first order of differences"
    i.e. the number of lines which change
    from one hexagram to the next. */

h[1] = 6;  h[2] = 2;  h[3] = 4;  h[4] = 4;  h[5] = 4;  h[6] = 3;
h[7] = 2;  h[8] = 4;  h[9] = 2; h[10] = 4; h[11] = 6; h[12] = 2;
h[13] = 2; h[14] = 4; h[15] = 2; h[16] = 2; h[17] = 6; h[18] = 3;
h[19] = 4; h[20] = 3; h[21] = 2; h[22] = 2; h[23] = 2; h[24] = 3;
h[25] = 4; h[26] = 2; h[27] = 6; h[28] = 2; h[29] = 6; h[30] = 3;
h[31] = 2; h[32] = 3; h[33] = 4; h[34] = 4; h[35] = 4; h[36] = 2;
h[37] = 4; h[38] = 6; h[39] = 4; h[40] = 3; h[41] = 2; h[42] = 4;
h[43] = 2; h[44] = 3; h[45] = 4; h[46] = 3; h[47] = 2; h[48] = 3;
h[49] = 4; h[50] = 4; h[51] = 4; h[52] = 1; h[53] = 6; h[54] = 2;
h[55] = 2; h[56] = 3; h[57] = 4; h[58] = 3; h[59] = 2; h[60] = 1;
h[61] = 6; h[62] = 3; h[63] = 6; h[64] = h[0] = 3;
}

/*-----------*/
void main(void)
{
int k, a, b;

initialize_h();

for ( k=0; k<=383; k++ )
{
a =    (exp_minus_one((k-1)/32))
       * ( h[mod_64(k-1)] - h[mod_64(k-2)]
       + h[mod_64(-k)] - h[mod_64(1-k)] )
       +  3 * ( (exp_minus_one((k-3)/96))
	      * ( h[mod_64((k/3)-1)]
		- h[mod_64((k/3)-2)]
		+ h[mod_64(-1*(k/3))]
		- h[mod_64(1-(k/3))] ) )
       +  6 * ( (exp_minus_one((k-6)/192))
	      * ( h[mod_64((k/6)-1)]
		- h[mod_64((k/6)-2)]
		+ h[mod_64(-1*(k/6))]
		- h[mod_64(1-(k/6))] ) );

b =    ( 9 - h[mod_64(-k)] - h[mod_64(k-1)] )
       +  3 * ( 9 - h[mod_64(-1*(k/3))] - h[mod_64((k/3)-1)] ),
       +  6 * ( 9 - h[mod_64(-1*(k/6))] - h[mod_64((k/6)-1)] );

w[k] =	abs(a) + abs(b);
}


print_data_points();
}

/*-------------*/
int mod_64(int i)
{
while ( i < 0 ) 
i += 64;


return ( i%64 );
}

/*--------------------*/ 
int exp_minus_one(int i)
{
if ( i < 0 )
i *= -1;

return ( i%2 ? -1 : 1 );

}

/*------------------------*/
void print_data_points(void)
{
int j, k=18; 

printf("\n%*s",k,"");

for ( j=0; j<384; j++ )
{
printf("%3d",w[j]);
if ( j < 383 )
printf(",");
if ( !((j+1)%10) )
printf("\n%*s",k,"");
}

printf("\n");
}
