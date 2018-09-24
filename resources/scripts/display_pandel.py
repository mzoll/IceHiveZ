#!/usr/bin/env python

"""
plot and display the time residual distribution of the Pandel Function for different distances
"""

import os, sys, getopt, glob, numpy, math
import matplotlib
import matplotlib.pyplot as plt
from scipy.special import gamma as Gamma
from scipy.special import gammainc as Gammainc
from scipy.special import gammaincinv as Gammaincinv

tau_ = 557.E-9 #sec
lambda_s_ = 98. #m
lambda_a_ = 33.3 #m
c_ice_ = 0.22103046286329384E-9 #m/sec

#from pandel paper
def PandelPDF (r,tres):
    a = r/lambda_s_
    b = 1./tau_ + c_ice_/lambda_a_
    p = 1./Gamma(a)
    p *= tau_**(-a)
    p *= numpy.exp(-r/lambda_a_)
    p *= (tres)**(a-1)
    p *= numpy.exp(-b * tres)
    return p

#from pandel paper: integral of P on interval t=0..inf
def tot_PandelProb (r):
    pp = numpy.exp(-r/lambda_a_) * (1 + c_ice_*tau_/lambda_a_)**(-r/lambda_s_)
    return pp

# integral of P on interval t=0..x
def intPandel(r, x):
    a = r/lambda_s_
    b = 1./tau_ + c_ice_/lambda_a_
    int_p = tau_**(-a)
    int_p *= numpy.exp(-r/lambda_a_)
    int_p *= b**(-a)
    int_p *= Gammainc(a, b*x)
    return int_p

# integral of P on interval t=0..x
def intPandelinv(r, prob_val):
    a = r/lambda_s_
    b = 1./tau_ + c_ice_/lambda_a_
    int_p = tau_**(-a)
    int_p *= numpy.exp(-r/lambda_a_)
    int_p *= b**(-a)
    
    return Gammaincinv(a, prob_val/int_p) /b

#get me the value at which this quartile is
def PandelProb(r, cont_quantile):
    tot_Prob =  tot_PandelProb (r)
    return intPandelinv(r, tot_Prob*cont_quantile)

fig = plt.figure()
t_ = numpy.linspace(1E-9, 5E-6, 100)
t_ns_ = t_*1E9

cont_quantile = 0.9

#put in at which distance you want this displayed
rdist = [50,100, 200,300]

for i in range(0, len(rdist)):
  r = rdist[i]
  ax = fig.add_subplot(len(rdist)*100+10+i+1)
  ax.plot(t_ns_, PandelPDF(r, t_))
  #ax.set_yscale('log')
  plt.grid(True)
  if i==0:
    ax.set_title("PandelFunction")
  plt.text(0, 1, "%0.1fm"%(r) , fontsize=20,
     horizontalalignment='left',
     verticalalignment='top',
     transform = ax.transAxes)
  plt.text(.9, .5, "arr-prob\n%0.2f %%"%(tot_PandelProb(r)*100) , fontsize=20,
     horizontalalignment='right',
     verticalalignment='center',
     transform = ax.transAxes)
  
  print "%fm away: total arrival prop is %f"%(r, tot_PandelProb(r))
  print "%fm away: %d percent contained at %fns"%(r, int(cont_quantile*100), PandelProb(r, cont_quantile)*1E9 )

ax.set_xlabel(r'time res [ns]', fontsize = 18)
#ax.set_ylabel(r'diff prob', fontsize = 18)
plt.show()
#plt.savefig(os.path.join(os.path.expandvars("$I3_BUILD"),'IceHiveZ/resources/pandel.png'))
