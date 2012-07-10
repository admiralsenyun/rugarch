/*################################################################################
##
##   R package rugarch by Alexios Ghalanos Copyright (C) 2009, 2010, 2011, 2012
##   This file is part of the R package rugarch.
##
##   The R package rugarch is free software: you can redistribute it and/or modify
##   it under the terms of the GNU General Public License as published by
##   the Free Software Foundation, either version 3 of the License, or
##   (at your option) any later version.
##
##   The R package rugarch is distributed in the hope that it will be useful,
##   but WITHOUT ANY WARRANTY; without even the implied warranty of
##   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##   GNU General Public License for more details.
##
#################################################################################*/
# include <R.h>
# include <math.h>
# include "rugarch.h"
# include "filters.h"
# include "distributions.h"

/* GARCH (p,q) Filter with ARMA, ARFIMA, Exogenous Regressors, and GARCH-IN-MEAN
 * Option for Mean Equation norm, skew-norm, t, skew-t, ged, skew-ged, nig & jsu
 * distributions for innovations*/
void sgarchfilterC(int *model, double *pars, int *idx, double *hEst, double *x, double *res,
		double *e, double *mexdata, double *vexdata, double *zrf, double *constm, double *condm,
		int *m, int *T, double *h, double *z, double *llh, double *LHT)
{
	int i;
	double lk=0;
	double hm = 0;
	for(i=0; i<*m; i++)
	{
		h[i] = *hEst;
		arfimaxfilter(model, pars, idx, x, res, mexdata, zrf, constm, condm, sqrt(fabs(*hEst)), *m, i, *T);
		e[i] = res[i] * res[i];
		//z[i] = res[i]/sqrt(fabs(h[i]));
		LHT[i] = log(garchdistribution(z[i], sqrt(fabs(h[i])), pars[idx[15]], pars[idx[16]], pars[idx[17]], model[20]));
		lk = lk - LHT[i];
	}
	for (i=*m; i<*T; i++)
	{
		sgarchfilter(model, pars, idx, vexdata, e, *T, i, h);
		hm = sqrt(fabs(h[i]));
		arfimaxfilter(model, pars, idx, x, res, mexdata, zrf, constm, condm, hm, *m, i, *T);
		e[i] = res[i] * res[i];
		z[i] = res[i]/sqrt(fabs(h[i]));
		LHT[i] = log(garchdistribution(z[i], sqrt(fabs(h[i])), pars[idx[15]], pars[idx[16]], pars[idx[17]], model[20]));
		lk = lk - LHT[i];
	}
	*llh=lk;
}

void sgarchsimC(int *model, double *pars, int *idx, double *h, double *z, double *res, double *e,
		double *vexdata, int *T, int *m)
{
	int i;
	for (i=*m; i<*T; i++)
	{
		sgarchfilter(model, pars, idx, vexdata, e, *T, i, h);
		res[i]=pow(h[i], 0.5)*z[i];
		e[i] = res[i]*res[i];
	}
}

void egarchfilterC(int *model, double *pars, int *idx, double *hEst, double *meanz,
		double *x, double *res, double *e, double *mexdata, double *vexdata, double *zrf,
		double *constm, double *condm, int *m, int *T, double *h, double *z, double *llh, double *LHT)
{
	int i;
	double lk=0;
	double hm = 0;

	for( i=0; i<*m; i++ )
	{
		h[i] = *hEst;
		arfimaxfilter(model, pars, idx, x, res, mexdata, zrf, constm, condm, sqrt(fabs(*hEst)), *m, i, *T);
		e[i] = res[i] * res[i];
		//z[i] = res[i]/sqrt(h[i]);
		LHT[i] = log(garchdistribution(z[i], sqrt(fabs(h[i])), pars[idx[15]], pars[idx[16]], pars[idx[17]], model[20]));
		lk = lk - LHT[i];
	}
	for ( i=*m; i<*T; i++ )
	{
		egarchfilter(model, pars, idx, *meanz, z, vexdata, *T, i, h);
		hm = sqrt(fabs(h[i]));
		arfimaxfilter(model, pars, idx, x, res, mexdata, zrf, constm, condm, hm, *m, i, *T);
		e[i] = res[i] * res[i];
		z[i] = res[i]/sqrt(h[i]);
		LHT[i] = log(garchdistribution(z[i], sqrt(fabs(h[i])), pars[idx[15]], pars[idx[16]], pars[idx[17]], model[20]));
		lk = lk - LHT[i];
	}
	*llh = lk;
}

void egarchsimC(int *model, double *pars, int *idx, double *meanz, double *h, double *z, double *res,
		double *vexdata, int *T, int *m)
{
	int i;
	for ( i=*m; i<*T; i++ )
	{
		egarchfilter(model, pars, idx, *meanz, z, vexdata, *T, i, h);
		res[i] = pow(h[i], 0.5)*z[i];
	}
}

void gjrgarchfilterC(int *model, double *pars, int *idx, double *hEst, double *x,
		double *res, double *nres, double *e, double *mexdata, double *vexdata, double *zrf,
		double *constm, double *condm, int *m, int *T, double *h, double *z, double *llh, double *LHT)
{
	int i;
	double lk=0;
	double hm=0;
	for( i=0; i<*m; i++ )
	{
		h[i] = *hEst;
		arfimaxfilter(model, pars, idx, x, res, mexdata, zrf, constm, condm, sqrt(fabs(*hEst)), *m, i, *T);
		e[i] = res[i] * res[i];
		nres[i] = res[i] < 0.0 ? e[i] : 0.0;
		LHT[i] = log(garchdistribution(z[i], sqrt(fabs(h[i])), pars[idx[15]], pars[idx[16]], pars[idx[17]], model[20]));
		lk = lk - LHT[i];
	}
	// Main Loop
	for ( i=*m; i<*T; i++ )
	{
		gjrgarchfilter(model, pars, idx, vexdata, nres, e, *T, i, h);
		hm = sqrt(fabs(h[i]));
		arfimaxfilter(model, pars, idx, x, res, mexdata, zrf, constm, condm, hm, *m, i, *T);
		e[i] = res[i] * res[i];
		nres[i] = res[i] < 0.0 ? e[i] : 0.0;
		z[i] = res[i]/sqrt(fabs(h[i]));
		LHT[i] = log(garchdistribution(z[i], sqrt(fabs(h[i])), pars[idx[15]], pars[idx[16]], pars[idx[17]], model[20]));
		lk = lk - LHT[i];
	}
	*llh = lk;
}

void gjrgarchsimC(int *model, double *pars, int *idx, double *h, double *z, double *res, double *e, double *nres,
		double *vexdata, int *T, int *m)
{
	int i;
	// first the variance equation
	for ( i=*m; i<*T; i++ )
	{
		gjrgarchfilter(model, pars, idx, vexdata, nres, e, *T, i, h);
		res[i] = pow( h[i],0.5 ) * z[i];
		e[i] = res[i] * res[i];
		nres[i] = res[i] < 0.0 ? e[i] : 0.0;
	}
}

void aparchfilterC(int *model, double *pars, int *idx, double *hEst, double *x, double *res, double *e,
		double *mexdata, double *vexdata, double *zrf, double *constm, double *condm,
		int *m, int *T, double *h, double *z, double *llh, double *LHT)
{
	int i;
	double lk=0;
	double hm = 0;
	// Mean Equation Initialization
	for(i=0; i<*m; i++)
	{
		h[i] = *hEst;
		arfimaxfilter(model, pars, idx, x, res, mexdata, zrf, constm, condm, fabs(*hEst), *m, i, *T);
		e[i] = res[i] * res[i];
		//z[i] = res[i]/fabs(h[i]);
		LHT[i] = log(garchdistribution(z[i], fabs(h[i]), pars[idx[15]], pars[idx[16]], pars[idx[17]], model[20]));
		lk=lk-LHT[i];
	}
	// Main Loop
	for (i=*m; i<*T; i++)
	{
		aparchfilter(model, pars, idx, vexdata, res, *T, i, h);
		hm = fabs(h[i]);
		arfimaxfilter(model, pars, idx, x, res, mexdata, zrf, constm, condm, hm, *m, i, *T);
		e[i] = z[i] * h[i];
		z[i] = res[i]/fabs(h[i]);
		LHT[i] = log(garchdistribution(z[i], fabs(h[i]), pars[idx[15]], pars[idx[16]], pars[idx[17]], model[20]));
		lk=lk-LHT[i];
	}
	*llh=lk;
}

void aparchsimC(int *model, double *pars, int *idx, double *h, double *z, double *res, double *vexdata,
		int *T, int *m)
{
	int i;
	for ( i=*m; i<*T; i++ )
	{
		aparchfilter(model, pars, idx, vexdata, res, *T, i, h);
		res[i] = z[i] * h[i];
	}
}

void fgarchfilterC(int *model, double *pars, int *idx, double *hEst, double *kdelta, double *x,
		double *res,double *e, double *mexdata, double *vexdata, double *zrf, double *constm,
		double *condm, int *m, int *T, double *h, double *z, double *llh, double *LHT)
{
	int i;
	double lk=0;
	double hm = 0;
	for(i=0;i<*m;i++)
	{
		h[i]=*hEst;
		arfimaxfilter(model, pars, idx, x, res, mexdata, zrf, constm, condm, fabs(*hEst), *m, i, *T);
		e[i] = res[i] * res[i];
		z[i] = res[i]/fabs(h[i]);
		LHT[i] = log(garchdistribution(z[i], fabs(h[i]), pars[idx[15]], pars[idx[16]], pars[idx[17]], model[20]));
		lk=lk-LHT[i];
	}
	for (i=*m; i<*T; i++)
	{
		fgarchfilter(model, pars, idx, *kdelta, z, vexdata, *T, i, h);
		hm = fabs(h[i]);
		arfimaxfilter(model, pars, idx, x, res, mexdata, zrf, constm, condm, hm, *m, i, *T);
		e[i] = res[i] * res[i];
		z[i] = res[i]/fabs(h[i]);
		LHT[i] = log(garchdistribution(z[i], fabs(h[i]), pars[idx[15]], pars[idx[16]], pars[idx[17]], model[20]));
		lk=lk-LHT[i];
	}
	*llh=lk;
}

void fgarchsimC(int *model, double *pars, int *idx, double *kdelta, double *h, double *z, double *res,
		double *vexdata, int *T, int *m)
{
	int i;
	for (i=*m; i<*T; i++)
	{
		fgarchfilter(model, pars, idx, *kdelta, z, vexdata, *T, i, h);
		res[i] = z[i] * h[i];
	}
}

void arfimafitC(int *model, double *pars, int *idx, double *x, double *res, double *mexdata, double *zrf,
		double *constm, double *condm, int *m, int *T, double *z, double *llh, double *LHT)
{
	int i;
	double lk=0;
	double hm = 0;
	for(i=0; i<*m; i++)
	{
		arfimaxfilter(model, pars, idx, x, res, mexdata, zrf, constm, condm, hm, *m, i, *T);
		z[i] = res[i]/fabs(pars[idx[6]]);
		LHT[i] = log(garchdistribution(z[i], fabs(pars[idx[6]]), pars[idx[15]], pars[idx[16]], pars[idx[17]], model[20]));
		lk = lk - LHT[i];
	}
	for (i=*m; i<*T; i++)
	{
		arfimaxfilter(model, pars, idx, x, res, mexdata, zrf, constm, condm, hm, *m, i, *T);
		z[i] = res[i]/fabs(pars[idx[6]]);
		LHT[i] = log(garchdistribution(z[i], fabs(pars[idx[6]]), pars[idx[15]], pars[idx[16]], pars[idx[17]], model[20]));
		lk = lk - LHT[i];
	}
	*llh=lk;
}

void arfimaxfilterC(int *model, double *pars, int *idx, double *x, double *res, double *mexdata,
		double *zrf, double *constm, double *condm, double *h, int *m, int *T)
{
	int i;
	double hm = 0;
	for(i=0; i<*T; i++)
	{
		hm = h[i];
		arfimaxfilter(model, pars, idx, x, res, mexdata, zrf, constm, condm, hm, *m, i, *T);
	}
}

void csgarchfilterC(int *model, double *pars, int *idx, double *hEst, double *x, double *res,
		double *e, double *mexdata, double *vexdata, double *zrf, double *constm, double *condm,
		int *m, int *T, double *h, double *q, double *z, double *llh, double *LHT)
{
	int i;
	double lk=0;
	double hm = 0;
	for(i=0; i<*m; i++)
	{
		h[i] = *hEst;
		// Set to the long run intercept \omega/(1-\rho)
		q[i] = pars[idx[6]]/(1.0-pars[idx[10]]);
		h[i] = h[i] + q[i];
		arfimaxfilter(model, pars, idx, x, res, mexdata, zrf, constm, condm, sqrt(fabs(*hEst)), *m, i, *T);
		e[i] = res[i] * res[i];
		//z[i] = res[i]/sqrt(fabs(h[i]));
		LHT[i] = log(garchdistribution(z[i], sqrt(fabs(h[i])), pars[idx[15]], pars[idx[16]], pars[idx[17]], model[20]));
		lk = lk - LHT[i];
	}
	for (i=*m; i<*T; i++)
	{
		csgarchfilter(model, pars, idx, e, vexdata, *T, i, h, q);
		hm = sqrt(fabs(h[i]));
		arfimaxfilter(model, pars, idx, x, res, mexdata, zrf, constm, condm, hm, *m, i, *T);
		e[i] = res[i] * res[i];
		z[i] = res[i]/sqrt(fabs(h[i]));
		LHT[i] = log(garchdistribution(z[i], hm, pars[idx[15]], pars[idx[16]], pars[idx[17]], model[20]));
		lk = lk - LHT[i];
	}
	*llh=lk;
}

void csgarchsimC(int *model, double *pars, int *idx, double *h, double *q, double *z, double *res, double *e,
		double *vexdata, int *T, int *m)
{
	int i;
	for (i=*m; i<*T; i++)
	{
		csgarchfilter(model, pars, idx, e, vexdata, *T, i, h, q);
		res[i]=pow(h[i], 0.5)*z[i];
		e[i] = res[i]*res[i];
	}
}
