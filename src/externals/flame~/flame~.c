// flame~ morphing oscillator with integer timing based on samplerate
// by Rob Bothof

#include <stdbool.h>
#include "m_pd.h"

#define VERSION "version 0.1"
#define CLIP(a, lo, hi) ( (a)>(lo)?( (a)<(hi)?(a):(hi) ):(lo) )     // as in MaxMsp's ext_common.h
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
void *flame_class;

typedef struct _flame {
    t_object l_obj;
//    t_word *l_buf;              // pointer to external array
//    t_symbol *l_sym;            // pointer to struct holding the name of external array
    t_float freq;
    t_float samplerate;
    float remainder;
    t_float size;
    t_float smpCount;
    t_float sizeSub;
    t_float smpCountSub;
    t_float lastSmp;
	// bool l_interp;
	// bool l_overdub;
	// long l_index_precedent;
	// long l_nb_val;
    // long l_offset;
    // long l_newoffset;
	// t_float l_valeur;
    t_float l_f;                // for MAINSIGNALIN
    // long l_size;
} t_flame;


static void *flame_new(t_symbol *s) {
	t_flame *x = (t_flame*)pd_new(flame_class);
    // first inlet inlet for float indices || frequency
    inlet_new(&x->l_obj, &x->l_obj.ob_pd, &s_signal, &s_signal);   // inlet for float indices || morphing
    outlet_new(&x->l_obj, &s_signal); // outlet float || sawtooth
    outlet_new(&x->l_obj, &s_signal); // outlet float || morphed shape
    x->samplerate=44100;
    x->size=1;
    x->sizeSub=1;
    x->smpCount=0;
    x->smpCountSub=0;
    x->remainder=0;
    x->lastSmp=0;
	// x->l_sym = s;
	// x->l_interp = 0;
	// x->l_overdub = 0;
    // x->l_offset = 0;
    // x->l_newoffset = 0;
    // x->l_size = 1000;
	// x->l_index_precedent = -1;
    // x->f_fade=0;

	return (x);
}




static t_int *flame_perform(t_int *w) {
    t_flame *x          = (t_flame *)(w[1]);
    t_float *infreq     = (t_float *)(w[2]);
    t_float *inmorph    = (t_float *)(w[3]);
    t_float *outsaw     = (t_float *)(w[4]);
    t_float *outmorph   = (t_float *)(w[5]);

	int n = (int)(w[6]);

    t_float newfreq;
    t_float newmorph;
    t_float out;
    t_float outSub;
    t_float saw2;
    t_float morph1;
    t_float morph2;
    t_float m1;
    t_float m2;
    t_float subtri;
    t_float newSmp;
    while (n--)	{
        newfreq = *infreq++;
        newmorph = *inmorph++ * 2;
        if ((newfreq > 0.001) && (newfreq < x->samplerate/4)) {
            if (x->smpCount >= x->size) {
                //t_float realsize = x->samplerate / newfreq;
                x->smpCount-=x->size;
                x->size = x->samplerate / newfreq;
                //x->size = (int) (realsize + 0.5);
                //x->remainder = realsize - x->size;
            } 
            if (x->smpCountSub >= x->sizeSub) {
                x->smpCountSub-=x->sizeSub;
                x->sizeSub = x->size;
            } 
            out = x->smpCount / (t_float) x->size;
            outSub = x->smpCountSub / (t_float) x->sizeSub;

            m1 = MAX(MIN(newmorph,2),1);
            m2 = MIN(MAX(newmorph,0),1);
            morph1=0;
            morph2=0;
            saw2 = out*2*m1;
            if (saw2 > 2) {
                morph1= 1-(saw2-2)*(1/(m1-0.9));
            } else {
                morph1= saw2-1;
            }
            if (morph1 >= m2) {
                morph2=1;
            } else {
                if (morph1 > - m2) {
                    morph2= morph1 / (m2+0.01);
                } else {
                    morph2 = -1;
                }
            }
            if (outSub * 2 - 1 > 0) {
                 *outsaw++ = (outSub*2-1) * -2 + 1;
            } else {
                 *outsaw++ = (outSub*2-1) * 2 + 1;
            }
//            *outsaw++ = outSub * 2 -1;
            if ((morph2-x->lastSmp > 0.75) || (morph2-x->lastSmp < -0.75)){
                morph2=morph2*0.35+x->lastSmp*0.65;
            }
            x->lastSmp=morph2;
            *outmorph++ = morph2 * (MAX(0,newmorph-1)*0.4+0.3);
  //         *outmorph++ = morph2 ;
            x->smpCount++;
            x->smpCountSub++;
        } else {
            *outsaw++ = 0;
            *outmorph++ = 0;
        }
        
    }
	return (w + 7);
}


void flame_dsp(t_flame *x, t_signal **sp) {
    x->samplerate=sp[0]->s_sr;
    dsp_add(flame_perform, 6, x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec,sp[3]->s_vec, sp[0]->s_n);
}


static void flame_samplerate(t_flame *x, t_floatarg samplerate) {
    x->samplerate = (long)samplerate;
    
}

void flame_tilde_setup(void) {
    flame_class = class_new(gensym("flame~"), (t_newmethod)flame_new, 0, sizeof(t_flame), 0, A_DEFSYM, 0);
    CLASS_MAINSIGNALIN(flame_class, t_flame, l_f);
    class_addmethod(flame_class, (t_method)flame_dsp, gensym("dsp"), 0);
    class_addmethod(flame_class, (t_method)flame_samplerate, gensym("samplerate"), A_FLOAT, 0);

    post("[flame~] %s: External Flame, Morphing Oscillator by Robber", VERSION);
}
