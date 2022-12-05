// roboloop~ looper with delay time
// written by Rob Bothof

// inspired by:

//	ipoke~ - a buffer writter with skipped address filling feature (interpolated or not)
//	by Pierre Alexandre Tremblay
//	v.2 optimised at the University of Huddersfield, with the help of the dev@cycling74.com comments of Mark Pauley, 
//          Alex Harker, Ben Neville and Peter Castine, addressed  to me or to others.
//	v.3 updated for Max5
//  Pd port by Katja Vetter 2012, with assistance from Matt Barber, Julian Brooks, Alexander Harker, Charles Henry 
//          and P.A. Tremblay.

#include <stdbool.h>
#include "m_pd.h"

#define VERSION "version 0.1"
#define CLIP(a, lo, hi) ( (a)>(lo)?( (a)<(hi)?(a):(hi) ):(lo) )     // as in MaxMsp's ext_common.h

void *roboloop_class;

typedef struct _roboloop
{
    t_object l_obj;
    t_word *l_buf;              // pointer to external array
    t_symbol *l_sym;            // pointer to struct holding the name of external array
    long bufsize;
	bool l_interp;
	bool l_overdub;
	long l_index_precedent;
	long l_nb_val;
    long l_offset;
    long l_newoffset;
    t_float f_fade;
	t_float l_valeur;
    t_float l_f;                // for MAINSIGNALIN
    long l_size;
} t_roboloop;

t_symbol *ps_buffer;

static void *roboloop_new(t_symbol *s)
{
	t_roboloop *x = (t_roboloop*)pd_new(roboloop_class);
    inlet_new(&x->l_obj, &x->l_obj.ob_pd, &s_signal, &s_signal);   // inlet for float indices
    inlet_new(&x->l_obj, &x->l_obj.ob_pd, &s_signal, &s_signal);   // inlet for float indices
    outlet_new(&x->l_obj, &s_signal);
	x->l_sym = s;
	x->l_interp = 0;
	x->l_overdub = 0;
    x->l_offset = 0;
    x->l_newoffset = 0;
    x->l_size = 1000;
	x->l_index_precedent = -1;
    x->f_fade=0;

	return (x);
}

static inline long wrap_index(long i, long arrayLength)
{
	while(i >= arrayLength)
        i -= arrayLength;
	while(i < 0)
        i += arrayLength;

    return i;
}

// four different subroutines of perform method:
// roboloop_perform_basic() - interpolation no, overdub no
// roboloop_perform_interp() - interpolation yes, overdub no
// roboloop_perform_overdub() - interpolation no, overdub yes
// roboloop_perform_interp_overdub() - interpolation yes, overdub yes

static void roboloop_perform_basic(t_int *w, t_roboloop *x)    // perform method without interpolation and overdub
{
    t_float *inval   = (t_float *)(w[2]);
    t_float *inind   = (t_float *)(w[3]);
    t_float *ingain  = (t_float *)(w[4]);
    t_float *out     = (t_float *)(w[5]);
	int n = (int)(w[6]);
    
    t_float newvalue;
    t_float newindex;
    t_float outvalue;

    long bufsize;
    long index = 0;
    long offsetindex=0;

    t_float gain;

    t_word *tab = x->l_buf;
    
    if (x->l_size < x->bufsize) {
        bufsize=x->l_size;
    } else {
        bufsize = x->bufsize;
    }
    
    while (n--)	// dsp loop without interpolation
    {
        newvalue = *inval++;
        newindex = *inind++;
        gain = *ingain++;


        index = wrap_index((long)(newindex),bufsize);		// round the next index and make sure he is in the buffer's boundaries
        offsetindex = wrap_index(index - x->l_offset,bufsize);

        outvalue = newvalue + gain * tab[offsetindex].w_float;                  // write the average value at the last index
        tab[index].w_float=outvalue;
        *out++ = outvalue;

    }

}
static void roboloop_perform_interp(t_int *w, t_roboloop *x) // perform method with interpolation, without overdub
{
    t_float *inval   = (t_float *)(w[2]);
    t_float *inind   = (t_float *)(w[3]);
    t_float *ingain  = (t_float *)(w[4]);
    t_float *out     = (t_float *)(w[5]);
	int n = (int)(w[6]);
    
	t_float valeur, coeff;
	long nb_val, index_precedent, pas, i, demivie;
    
    t_word *tab = x->l_buf;
	index_precedent = x->l_index_precedent;
	valeur = x->l_valeur;
	nb_val = x->l_nb_val;

    t_float newvalue;
    t_float newindex;
    t_float outvalue;

    long bufsize;
    long index = 0;
    long offsetindex=0;
    long offsetindexplus=0;
    long offsetindexminus=0;

    long iOffset = 0;

    t_float gain;
    t_float loopvalue;

    if (x->l_size < x->bufsize) {
        bufsize=x->l_size;
    } else {
        bufsize = x->bufsize;
    }

	demivie = (long)(bufsize * 0.5);

    while (n--)	// dsp loop with interpolation
    {
        newvalue = *inval++;
        newindex = *inind++;
        gain = *ingain++;


        if (x->l_offset != x->l_newoffset) {
            if (x->f_fade <= 0) {
                x->f_fade=0;
                x->l_offset=x->l_newoffset;
            } else {
                x->f_fade-=0.05;
            }
        }

        if (newindex < 0.0)	{ // if writing
            if (index_precedent >= 0) {
                loopvalue = tab[wrap_index(index_precedent - x->l_offset,bufsize)].w_float * x->f_fade + tab[wrap_index(index_precedent - x->l_newoffset,bufsize)].w_float * (1 - x->f_fade);
                tab[index_precedent].w_float = valeur/nb_val + gain * loopvalue;           // write the average value at the last given index
                valeur = 0.0;
                index_precedent = -1;
            }
        }			
        else {
            index = wrap_index((long)(newindex+0.5),bufsize);		// round the next index and make sure he is in the buffer's boundaries
            if (index_precedent < 0) {									// if it is the first index to write, resets the averaging and the values
                index_precedent = index;
                nb_val = 0;
            }
            if (index == index_precedent) {								// if the index has not moved, accumulate the value to average later.
                 valeur += newvalue;
                 nb_val += 1;
            } else {														// if it moves

                if (nb_val != 1) {										// is there more than one values to average
                     valeur = valeur/nb_val;								// if yes, calculate the average
                     nb_val = 1;
                }
                loopvalue = tab[wrap_index(index_precedent - x->l_offset,bufsize)].w_float * x->f_fade + tab[wrap_index(index_precedent - x->l_newoffset,bufsize)].w_float * (1 - x->f_fade);
                tab[index_precedent].w_float = valeur + gain * loopvalue;                  // write the average value at the last index
                pas = index - index_precedent;							// calculate the step to do
                
                if (pas > 0) {											// are we going up
                    if (pas > demivie) {									// is it faster to go the other way round?
                        pas -= bufsize;									// calculate the new number of steps
                        coeff = (newvalue - valeur) / pas;			// calculate the interpolation coefficient
                        
                        for(i=(index_precedent-1);i>=0;i--) {				// fill the gap to zero
                            valeur -= coeff;
                            loopvalue = tab[wrap_index(i - x->l_offset,bufsize)].w_float * x->f_fade + tab[wrap_index(i - x->l_newoffset,bufsize)].w_float * (1 - x->f_fade);
                            tab[i].w_float = valeur + gain * loopvalue;                  // write the average value at the last index
                        }
                        for(i=(bufsize-1);i>index;i--) {					// fill the gap from the top
                            valeur -= coeff;
                            loopvalue = tab[wrap_index(i - x->l_offset,bufsize)].w_float * x->f_fade + tab[wrap_index(i - x->l_newoffset,bufsize)].w_float * (1 - x->f_fade);
                            tab[i].w_float = valeur + gain * loopvalue;                  // write the average value at the last index
                        }
                    } else {												// if not, just fill the gaps
                        coeff = (newvalue - valeur) / pas;			// calculate the interpolation coefficient
                        for (i=(index_precedent+1); i<index; i++) {
                            valeur += coeff;
                            loopvalue = tab[wrap_index(i - x->l_offset,bufsize)].w_float * x->f_fade + tab[wrap_index(i - x->l_newoffset,bufsize)].w_float * (1 - x->f_fade);
                            tab[i].w_float = valeur + gain * loopvalue;                  // write the average value at the last index
                        }
                    }
                } else {													// if we are going down
                    if ((-pas) > demivie)								// is it faster to go the other way round?
                    {
                        pas += bufsize;									// calculate the new number of steps
                        coeff = (newvalue - valeur) / pas;			// calculate the interpolation coefficient
                        
                        for (i=(index_precedent+1);i<bufsize;i++) {		// fill the gap to the top
                            valeur += coeff;
                            loopvalue = tab[wrap_index(i - x->l_offset,bufsize)].w_float * x->f_fade + tab[wrap_index(i - x->l_newoffset,bufsize)].w_float * (1 - x->f_fade);
                            tab[i].w_float = valeur + gain * loopvalue;                  // write the average value at the last index
                        }
                        for(i=0;i<index;i++) {							// fill the gap from zero
                            valeur += coeff;
                            loopvalue = tab[wrap_index(i - x->l_offset,bufsize)].w_float * x->f_fade + tab[wrap_index(i - x->l_newoffset,bufsize)].w_float * (1 - x->f_fade);
                            tab[i].w_float = valeur + gain * loopvalue;                  // write the average value at the last index
                        }
                    }
                    else {												// if not, just fill the gaps
                        coeff = (newvalue - valeur) / pas;			// calculate the interpolation coefficient
                        for (i=(index_precedent-1); i>index; i--) {
                            valeur -= coeff;
                            loopvalue = tab[wrap_index(i - x->l_offset,bufsize)].w_float * x->f_fade + tab[wrap_index(i - x->l_newoffset,bufsize)].w_float * (1 - x->f_fade);
                            tab[i].w_float = valeur + gain * loopvalue;                  // write the average value at the last index
                        }							
                    }
                }
                valeur = newvalue;									// transfer the new previous value
            }// end of else (if it moves)
        }// end of else (if writing)	

        index_precedent = index;	
        									// transfer the new previous address

        offsetindexminus = wrap_index(index - x->l_offset -1,bufsize);
        offsetindex = wrap_index(index - x->l_offset,bufsize);
        offsetindexplus = wrap_index(index - x->l_offset + 1,bufsize);

        //output loop, not added value;
        *out++ = gain * (tab[offsetindex].w_float + tab[offsetindexminus].w_float + tab[offsetindexplus].w_float) * 0.333333;                  
    }// end of while(n--)
    
    x->l_index_precedent = index_precedent;
	x->l_valeur = valeur;
	x->l_nb_val = nb_val;
}


static t_int *roboloop_perform(t_int *w)
{
    t_roboloop *x = (t_roboloop *)(w[1]);
    
    if(!x->l_buf) goto out;                           // skip dsp loop if buffer does not exist
    
	//int interp = x->l_interp;
	//int overdub = x->l_overdub;
	
    //if(interp)
    //{
    //    if(!overdub) 
    //roboloop_perform_interp(w, x);      // interpolation yes, overdub no (default)
    //    else roboloop_perform_interp_overdub(w, x);      // interpolation yes, overdub yes
    //}
    //else if(overdub) roboloop_perform_overdub(w, x);     // interpolation no, overdub yes
    //else 
    //roboloop_perform_basic(w, x);                   // interpolation no, overdub no
    roboloop_perform_interp(w, x);                   // interpolation no, overdub no

out:
	return (w + 7);
}

void roboloop_set(t_roboloop *x, t_symbol *s)
{
	t_garray *b;
    int bufsize;
    x->l_sym = s;
    
    if(!(b = (t_garray*)pd_findbyclass(x->l_sym, garray_class)))
    {
        if(*x->l_sym->s_name) pd_error(x, "roboloop~: %s, no such array", x->l_sym->s_name);
        x->l_buf = 0;
    }
    
    else if (!garray_getfloatwords(b, &bufsize, &x->l_buf))
    {
        pd_error(x, "%s: bad template for roboloop~", x->l_sym->s_name);
        x->l_buf = 0;
    }
    
    else 
    {
        x->bufsize = bufsize;
        garray_usedindsp(b);
    }
}

void roboloop_dsp(t_roboloop *x, t_signal **sp)
{
	x->l_index_precedent = -1;
    roboloop_set(x, x->l_sym);
    dsp_add(roboloop_perform, 6, x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec,sp[3]->s_vec, sp[0]->s_n);
    
}




static void roboloop_offset(t_roboloop *x, t_floatarg offset)
{
//	x->l_offset = (long)offset;
    x->l_newoffset = (long)offset;
    x->f_fade=1.0;

}
static void roboloop_size(t_roboloop *x, t_floatarg size)
{
	x->l_size = (long)size;
}
static void roboloop_bang(t_roboloop *x)
{
    t_garray *buf = (t_garray *)pd_findbyclass(x->l_sym, garray_class);
    if(!buf) pd_error(x, "roboloop~: %s, no such array", x->l_sym->s_name);
    else garray_redraw(buf); 
}

void roboloop_tilde_setup(void)
{
    roboloop_class = class_new(gensym("roboloop~"), (t_newmethod)roboloop_new, 0, sizeof(t_roboloop), 0, A_DEFSYM, 0);
    CLASS_MAINSIGNALIN(roboloop_class, t_roboloop, l_f);
    class_addmethod(roboloop_class, (t_method)roboloop_dsp, gensym("dsp"), 0);
    class_addmethod(roboloop_class, (t_method)roboloop_offset, gensym("offset"), A_FLOAT, 0);
    class_addmethod(roboloop_class, (t_method)roboloop_size, gensym("size"), A_FLOAT, 0);
    class_addmethod(roboloop_class, (t_method)roboloop_set, gensym("set"), A_SYMBOL,0);
    class_addbang(roboloop_class, roboloop_bang);
    post("[roboloop~] %s: Looper by Robber", VERSION);
}
