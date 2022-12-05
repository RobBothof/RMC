// flame~ morphing oscillator with integer timing based on samplerate
// by Rob Bothof

#include <stdbool.h>
#include "m_pd.h"

#define VERSION "version 0.1"
#define CLIP(a, lo, hi) ( (a)>(lo)?( (a)<(hi)?(a):(hi) ):(lo) )     // as in MaxMsp's ext_common.h
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
void *flame2_class;

typedef struct _flame2 {
    t_object l_obj;
    t_float samplerate;
    t_float size1;
    t_float size2;
    t_float sizeSub;
    t_float smpCount1;
    t_float smpCount2;
    t_float smpCountSub;
    t_float lastSmp1;
    t_float lastSmp2;
    bool hardsync;
    t_float l_f;                // for MAINSIGNALIN
} t_flame2;


static void *flame2_new(t_symbol *s) {
	t_flame2 *x = (t_flame2*)pd_new(flame2_class);                 // inlet inlet for float indices || frequency1
    inlet_new(&x->l_obj, &x->l_obj.ob_pd, &s_signal, &s_signal);   // inlet for float indices || morphing2
    inlet_new(&x->l_obj, &x->l_obj.ob_pd, &s_signal, &s_signal);   // inlet for float indices || morphing1
    inlet_new(&x->l_obj, &x->l_obj.ob_pd, &s_signal, &s_signal);   // inlet for float indices || morphing2
    outlet_new(&x->l_obj, &s_signal); // outlet float || sub sawtooth
    outlet_new(&x->l_obj, &s_signal); // outlet float || morphed shape1
    outlet_new(&x->l_obj, &s_signal); // outlet float || morphed shape2
    x->samplerate=44100;
    x->size1=1;
    x->size2=1;
    x->sizeSub=1;
    x->smpCount1=0;
    x->smpCount2=0;
    x->smpCountSub=0;
    x->lastSmp1=0;
    x->lastSmp2=0;
    x->hardsync=false;

	return (x);
}




static t_int *flame2_perform(t_int *w) {
    t_flame2 *x         = (t_flame2 *)(w[1]);
    t_float *infreq1    = (t_float *)(w[2]);
    t_float *infreq2    = (t_float *)(w[3]);
    t_float *inshape1   = (t_float *)(w[4]);
    t_float *inshape2   = (t_float *)(w[5]);
    t_float *out1  = (t_float *)(w[6]);
    t_float *out2  = (t_float *)(w[7]);
    t_float *out3  = (t_float *)(w[8]);

	int n = (int)(w[9]);

    t_float freq1;
    t_float freq2;
    t_float shape1;
    t_float shape2;

    t_float phase1;
    t_float phase2;
    t_float outSub;

    t_float morph1A;
    t_float morph1B;
    t_float morph1C;
    t_float morph2A;
    t_float morph2B;
    t_float morph2C;
    t_float mA1;
    t_float mB1;
    t_float mA2;
    t_float mB2;

    while (n--)	{
        freq1 = *infreq1++;
        freq2 = *infreq2++;
        shape1 = *inshape1++ * 3 - 1;
        shape2 = *inshape2++ * 3 - 1;
        //subosc
        if ((freq1 > 0.001) && (freq1 < x->samplerate/4)) {
            if (x->smpCountSub >= x->sizeSub) {
                x->smpCountSub-=x->sizeSub;
                x->sizeSub = x->samplerate / (freq1*0.5);
            } 
            outSub = (x->smpCountSub / (t_float) x->sizeSub)*(x->smpCountSub / (t_float) x->sizeSub);
            if (outSub *2 - 1 > 0) {
                 *out1++ = (outSub*2-1) * -2 + 1;
            } else {
                 *out1++ = (outSub*2-1) * 2 + 1;
            }
            x->smpCountSub++;
        } else {
            *out1++ = 0;
        }
        //oscillator 1
        if ((freq1 > 0.001) && (freq1 < x->samplerate/4)) {
            if (x->smpCount1 >= x->size1) {
                x->smpCount1-=x->size1;
                x->size1 = x->samplerate / freq1;
                if (x->hardsync) {
                    x->smpCount2 = x->smpCount1;
                    x->size2 = x->samplerate / freq2;
                }
            } 
            phase1 = x->smpCount1 / (t_float) x->size1;

            mA1 = MAX(MIN(shape1,2),1);
            mB1 = MIN(MAX(shape1,-1),1);

            morph1A=0;
            morph1B=0;
            morph1C=0;
           
            morph1A = phase1*phase1*2*mA1;
            if (morph1A > 2) {
                morph1B = 1-(morph1A-2)*(1/(mA1-0.9));
            } else {
                morph1B= morph1A-1;
            }
            if (morph1B >= mB1) {
                morph1C=1;
            } else {
                if (morph1B > - mB1) {
                    morph1C= morph1B / (mB1+0.01);
                } else {
                    morph1C = -1;
                }
            }
            if ((morph1C-x->lastSmp1 > 0.75) || (morph1C-x->lastSmp1 < -0.75)){
                //morph1C=morph1C*0.35+x->lastSmp1*0.65;
                morph1C=morph1C*0.5+x->lastSmp1*0.5;

            }
            x->lastSmp1=morph1C;
            *out2++ = morph1C * (MAX(0,mA1-1)*0.5+0.3);
            x->smpCount1++;
        } else {
            *out2++ = 0;
        }
        //oscillator2
        if ((freq2 > 0.001) && (freq2 < x->samplerate/4)) {
            if (x->smpCount2 >= x->size2) {
                x->smpCount2-=x->size2;
                x->size2 = x->samplerate / freq2;
            } 
            phase2 = x->smpCount2 / (t_float) x->size2;

            mA2 = MAX(MIN(shape2,2),1);
            mB2 = MIN(MAX(shape2,-1),1);

            morph2A=0;
            morph2B=0;
            morph2C=0;
           
            morph2A = phase2*phase2*2*mA2;
            if (morph2A > 2) {
                morph2B = 1-(morph2A-2)*(1/(mA2-0.9));
            } else {
                morph2B= morph2A-1;
            }
            if (morph2B >= mB2) {
                morph2C=1;
            } else {
                if (morph2B > - mB2) {
                    morph2C= morph2B / (mB2+0.01);
                } else {
                    morph2C = -1;
                }
            }
            if ((morph2C-x->lastSmp2 > 0.75) || (morph2C-x->lastSmp2 < -0.75)){
//                morph2C=morph2C*0.35+x->lastSmp2*0.65;
                morph2C=morph2C*0.5+x->lastSmp2*0.5;
            }
            x->lastSmp2=morph2C;
            *out3++ = morph2C * (MAX(0,mA2-1)*0.5+0.3);
            x->smpCount2++;
        } else {
            *out3++ = 0;
        }
    }
	return (w + 10);
}


void flame2_dsp(t_flame2 *x, t_signal **sp) {
    x->samplerate=sp[0]->s_sr;
    dsp_add(flame2_perform, 9, x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec,sp[3]->s_vec,sp[4]->s_vec,sp[5]->s_vec,sp[6]->s_vec, sp[0]->s_n);
}


static void flame2_samplerate(t_flame2 *x, t_floatarg samplerate) {
    x->samplerate = (long)samplerate;
}

static void flame2_hardsync(t_flame2 *x, t_floatarg hardsync) {
    if (hardsync > 0.5) {
        x->hardsync = true;
    } else {
        x->hardsync = false;
    }
}

void flame2_tilde_setup(void) {
    flame2_class = class_new(gensym("flame2~"), (t_newmethod)flame2_new, 0, sizeof(t_flame2), 0, A_DEFSYM, 0);
    CLASS_MAINSIGNALIN(flame2_class, t_flame2, l_f);
    class_addmethod(flame2_class, (t_method)flame2_dsp, gensym("dsp"), 0);
    class_addmethod(flame2_class, (t_method)flame2_samplerate, gensym("samplerate"), A_FLOAT, 0);
    class_addmethod(flame2_class, (t_method)flame2_hardsync, gensym("hardsync"), A_FLOAT, 0);
    post("[flame2~] %s: External flame2, Morphing Oscillator by Robber", VERSION);
}
