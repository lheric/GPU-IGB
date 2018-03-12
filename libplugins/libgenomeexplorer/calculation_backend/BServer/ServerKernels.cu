//
// ServerKernels.cu
//
//

#include "Profile.h"
#include <cuda_runtime.h>
#include <curand_kernel.h> 
#include <curand.h>
#include <iostream>
#include <assert.h>
#include <vector>

#include <thrust/reduce.h>
#include <thrust/functional.h>
#include <thrust/device_vector.h>
#include <thrust/execution_policy.h>
#include <thrust/iterator/constant_iterator.h>

// ==================================================================
//  CudaEventTimer
//
// Example usage:
//
//	T.Begin();
//	generate << < blocks, threads >> > (gData, DataPerBlock);
//	T.End();
//	cudaError_t err = cudaGetLastError();
//	printf("\nError = %s", cudaGetErrorString(err));
//	printf("\nDuration of generate kernel = %.3f ms for %d floats\n\n", T.GetTime(), N);
// ==================================================================
class CudaEventTimer
{
public:
	CudaEventTimer()
	{
		cudaEventCreate(&start);
		cudaEventCreate(&stop);
	}
	~CudaEventTimer() { cudaEventDestroy(start); cudaEventDestroy(stop); }

	void Begin() { cudaEventRecord(start); }
	void End() {
		cudaEventRecord(stop);
		cudaEventSynchronize(stop);
	}
	float GetTime(void) { cudaEventElapsedTime(&ms, start, stop); return ms; }

private:
	cudaEvent_t start, stop;
	float ms;
};

// ==================================================================
// ==================================================================
using namespace std;

/* BIG = 1/MACHEPF */

#define BIG   16777216.0f
#define MACHEPF 5.9604644775390625E-8f

/* MAXNUMF = 2^128 * (1 - 2^-24) */
#define  MAXNUMF 3.4028234663852885981170418348451692544e38f
/* log(2^-149) */
#define  MINLOGF -103.278929903431851103f		 
#define  PIF 3.141592653589793238f
#define  PIINV  0.318309886183790671538f

/* log( sqrt( 2*pi ) ) */
#define LS2PI  0.91893853320467274178f
#define MAXLGM 2.035093e36
#define MAXLOGF 88.72283905206835f

// sqrt(2pi)
#define s2pi 2.50662827463100050242f

/* log gamma(x+2), -.5 < x < .5 */
__host__ __device__ float polevlfB(float xx)
{
	float t = 6.055172732649237E-004f;
	t = t * xx - 1.311620815545743E-003f;
	t = t * xx + 2.863437556468661E-003f;
	t = t * xx - 7.366775108654962E-003f;
	t = t * xx + 2.058355474821512E-002f;
	t = t * xx - 6.735323259371034E-002f;
	t = t * xx + 3.224669577325661E-001f;
	t = t * xx + 4.227843421859038E-001f;
	return t;
}

/* log gamma(x+1), -.25 < x < .25 */
__host__ __device__ float polevlfC(float xx)
{
	float t = 1.369488127325832E-001f;
	t = t * xx - 1.590086327657347E-001f;
	t = t * xx + 1.692415923504637E-001f;
	t = t * xx - 2.067882815621965E-001f;
	t = t * xx + 2.705806208275915E-001f;
	t = t * xx - 4.006931650563372E-001f;
	t = t * xx + 8.224670749082976E-001f;
	t = t * xx - 5.772156501719101E-001f;
	return t;
}

/* approximation for 0 <= |y - 0.5| <= 3/8 */

__host__ __device__ float polevlfP0(float xx)
{
	float t = -5.99633501014107895267E1f;
	t = t * xx + 9.80010754185999661536E1f;
	t = t * xx - 5.66762857469070293439E1f;
	t = t * xx + 1.39312609387279679503E1f;
	t = t * xx - 1.23916583867381258016E0f;
	return t;
}

__host__ __device__ float p1evlfQ0(float xx)
{
	float t = xx + 1.95448858338141759834E0f;
	t = t * xx + 4.67627912898881538453E0f;
	t = t * xx + 8.63602421390890590575E1f;
	t = t * xx - 2.25462687854119370527E2f;
	t = t * xx + 2.00260212380060660359E2f;
	t = t * xx - 8.20372256168333339912E1f;
	t = t * xx + 1.59056225126211695515E1f;
	t = t * xx - 1.18331621121330003142E0f;
	return t;
}

/* Approximation for interval z = sqrt(-2 log y ) between 2 and 8
* i.e., y between exp(-2) = .135 and exp(-32) = 1.27e-14.
*/

__host__ __device__ float polevlfP1(float xx)
{
	float t = 4.05544892305962419923E0f;
	t = t * xx + 3.15251094599893866154E1f;
	t = t * xx + 5.71628192246421288162E1f;
	t = t * xx + 4.40805073893200834700E1f;
	t = t * xx + 1.46849561928858024014E1f;
	t = t * xx + 2.18663306850790267539E0f;
	t = t * xx - 1.40256079171354495875E-1f;
	t = t * xx - 3.50424626827848203418E-2f;
	t = t * xx - 8.57456785154685413611E-4f;
	return t;
}

__host__ __device__ float p1evlfQ1(float xx)
{
	float t = xx + 1.57799883256466749731E1f;
	t = t * xx + 4.53907635128879210584E1f;
	t = t * xx + 4.13172038254672030440E1f;
	t = t * xx + 1.50425385692907503408E1f;
	t = t * xx + 2.50464946208309415979E0f;
	t = t * xx - 1.42182922854787788574E-1f;
	t = t * xx - 3.80806407691578277194E-2f;
	t = t * xx - 9.33259480895457427372E-4f;
	return t;
}

/* Approximation for interval z = sqrt(-2 log y ) between 8 and 64
* i.e., y between exp(-32) = 1.27e-14 and exp(-2048) = 3.67e-890.
*/

__host__ __device__ float polevlfP2(float xx)
{
	float t = 3.23774891776946035970E0f;
	t = t * xx + 6.91522889068984211695E0f;
	t = t * xx + 3.93881025292474443415E0f;
	t = t * xx + 1.33303460815807542389E0f;
	t = t * xx + 2.01485389549179081538E-1f;
	t = t * xx + 1.23716634817820021358E-2f;
	t = t * xx + 3.01581553508235416007E-4f;
	t = t * xx + 2.65806974686737550832E-6f;
	t = t * xx + 6.23974539184983293730E-9f;
	return t;
}

__host__ __device__ float p1evlfQ2(float xx)
{
	float t = xx + 6.02427039364742014255E0f;
	t = t * xx + 3.67983563856160859403E0f;
	t = t * xx + 1.37702099489081330271E0f;
	t = t * xx + 2.16236993594496635890E-1f;
	t = t * xx + 1.34204006088543189037E-2f;
	t = t * xx + 3.28014464682127739104E-4f;
	t = t * xx + 2.89247864745380683936E-6f;
	t = t * xx + 6.79019408009981274425E-9f;
	return t;
}


__host__ __device__ float lgamf(float xx)
{
	float p, q, w, z, x;
	float nx, tx;
	int i, direction;

	int sgngamf = 1;

	x = xx;
	if (x < 0.0f)
	{
		q = -x;
		w = lgamf(q); /* note this modifies sgngam! */
		p = floorf(q);
		if (p == q)
			goto loverf;
		i = (int)p;
		if ((i & 1) == 0)
			sgngamf = -1;
		else
			sgngamf = 1;
		z = q - p;
		if (z > 0.5f)
		{
			p += 1.0f;
			z = p - q;
		}
		z = q * sinf(PIF * z);
		if (z == 0.0)
			goto loverf;
		z = -logf(PIINV*z) - w;
		return(z);
	}

	if (x < 6.5f)
	{
		direction = 0;
		z = 1.0;
		tx = x;
		nx = 0.0;
		if (x >= 1.5)
		{
			while (tx > 2.5f)
			{
				nx -= 1.0f;
				tx = x + nx;
				z *= tx;
			}
			x += nx - 2.0f;
		iv1r5:
			p = x * polevlfB(x);
			goto cont;
		}
		if (x >= 1.25f)
		{
			z *= x;
			x -= 1.0f; /* x + 1 - 2 */
			direction = 1;
			goto iv1r5;
		}
		if (x >= 0.75f)
		{
			x -= 1.0f;
			p = x * polevlfC(x);
			q = 0.0f;
			goto contz;
		}
		while (tx < 1.5f)
		{
			if (tx == 0.0f)
				goto loverf;
			z *= tx;
			nx += 1.0f;
			tx = x + nx;
		}
		direction = 1;
		x += nx - 2.0f;
		p = x * polevlfB(x);

	cont:
		if (z < 0.0f)
		{
			sgngamf = -1;
			z = -z;
		}
		else
		{
			sgngamf = 1;
		}
		q = logf(z);
		if (direction)
			q = -q;
	contz:
		return(p + q);
	}

	if (x > MAXLGM)
	{
	loverf:
		return(sgngamf * MAXNUMF);	// overflow
	}

	// Note, though an asymptotic formula could be used for x >= 3,
	// there is cancellation error in the following if x < 6.5. 
	q = LS2PI - x;
	q += (x - 0.5f) * logf(x);

	if (x <= 1.0e4)
	{
		z = 1.0f / x;
		p = z * z;
		q += ((6.789774945028216E-004f * p
			- 2.769887652139868E-003f) * p
			+ 8.333316229807355E-002f) * z;
	}
	return(q);
}

//
// Continued fraction expansion #1 for incomplete beta integral.
//
__host__ __device__ float incbcff(float aa, float bb, float xx)
{
	float a, b, x, xk, pk, pkm1, pkm2, qk, qkm1, qkm2;
	float k1, k2, k3, k4, k5, k6, k7, k8;
	float r, t, ans;
	int n;

	a = aa;
	b = bb;
	x = xx;
	k1 = a;
	k2 = a + b;
	k3 = a;
	k4 = a + 1.0f;
	k5 = 1.0f;
	k6 = b - 1.0f;
	k7 = k4;
	k8 = a + 2.0f;

	pkm2 = 0.0f;
	qkm2 = 1.0f;
	pkm1 = 1.0f;
	qkm1 = 1.0f;
	ans = 1.0f;
	r = 0.0f;
	n = 0;
	do
	{

		xk = -(x * k1 * k2) / (k3 * k4);
		pk = pkm1 + pkm2 * xk;
		qk = qkm1 + qkm2 * xk;
		pkm2 = pkm1;
		pkm1 = pk;
		qkm2 = qkm1;
		qkm1 = qk;

		xk = (x * k5 * k6) / (k7 * k8);
		pk = pkm1 + pkm2 * xk;
		qk = qkm1 + qkm2 * xk;
		pkm2 = pkm1;
		pkm1 = pk;
		qkm2 = qkm1;
		qkm1 = qk;

		if (qk != 0)
			r = pk / qk;

		if (r != 0)
		{
			t = fabsf((ans - r) / r);
			ans = r;
		}
		else
			t = 1.0f;

		if (t < MACHEPF) return (ans);

		k1 += 1.0f;
		k2 += 1.0f;
		k3 += 2.0f;
		k4 += 2.0f;
		k5 += 1.0f;
		k6 -= 1.0f;
		k7 += 2.0f;
		k8 += 2.0f;

		if ((fabsf(qk) + fabsf(pk)) > BIG)
		{
			pkm2 *= MACHEPF;
			pkm1 *= MACHEPF;
			qkm2 *= MACHEPF;
			qkm1 *= MACHEPF;
		}
		if ((fabsf(qk) < MACHEPF) || (fabsf(pk) < MACHEPF))
		{
			pkm2 *= BIG;
			pkm1 *= BIG;
			qkm2 *= BIG;
			qkm1 *= BIG;
		}
	} while (++n < 100);

	return(ans);
}

//
// Continued fraction expansion #2 for incomplete beta integral.
//

__host__ __device__ float incbdf(float aa, float bb, float xx)
{
	float a, b, x, xk, pk, pkm1, pkm2, qk, qkm1, qkm2;
	float k1, k2, k3, k4, k5, k6, k7, k8;
	float r, t, ans, z;
	int n;

	a = aa;
	b = bb;
	x = xx;
	k1 = a;
	k2 = b - 1.0f;
	k3 = a;
	k4 = a + 1.0f;
	k5 = 1.0f;
	k6 = a + b;
	k7 = a + 1.0f;
	k8 = a + 2.0f;

	pkm2 = 0.0f;
	qkm2 = 1.0f;
	pkm1 = 1.0f;
	qkm1 = 1.0f;
	z = x / (1.0f - x);
	ans = 1.0f;
	r = 0.0f;
	n = 0;
	do
	{

		xk = -(z * k1 * k2) / (k3 * k4);
		pk = pkm1 + pkm2 * xk;
		qk = qkm1 + qkm2 * xk;
		pkm2 = pkm1;
		pkm1 = pk;
		qkm2 = qkm1;
		qkm1 = qk;

		xk = (z * k5 * k6) / (k7 * k8);
		pk = pkm1 + pkm2 * xk;
		qk = qkm1 + qkm2 * xk;
		pkm2 = pkm1;
		pkm1 = pk;
		qkm2 = qkm1;
		qkm1 = qk;

		if (qk != 0)
			r = pk / qk;
		if (r != 0)
		{
			t = fabsf((ans - r) / r);
			ans = r;
		}
		else
			t = 1.0f;

		if (t < MACHEPF) return ans;	// underflow

		k1 += 1.0f;
		k2 -= 1.0f;
		k3 += 2.0f;
		k4 += 2.0f;
		k5 += 1.0f;
		k6 += 1.0f;
		k7 += 2.0f;
		k8 += 2.0f;

		if ((fabsf(qk) + fabsf(pk)) > BIG)
		{
			pkm2 *= MACHEPF;
			pkm1 *= MACHEPF;
			qkm2 *= MACHEPF;
			qkm1 *= MACHEPF;
		}
		if ((fabsf(qk) < MACHEPF) || (fabsf(pk) < MACHEPF))
		{
			pkm2 *= BIG;
			pkm1 *= BIG;
			qkm2 *= BIG;
			qkm1 *= BIG;
		}
	} while (++n < 100);

	return(ans);
}

__host__ __device__ float incbpsf(float aa, float bb, float xx)
{
	float a, b, x, t, u, y, s;

	a = aa;
	b = bb;
	x = xx;

	y = a * logf(x) + (b - 1.0f)*logf(1.0f - x) - logf(a);
	y -= lgamf(a) + lgamf(b);
	y += lgamf(a + b);


	t = x / (1.0f - x);
	s = 0.0f;
	u = 1.0f;
	do
	{
		b -= 1.0f;
		if (b == 0.0f)
			break;

		a += 1.0f;
		u *= t*b / a;
		s += u;
	} while (fabsf(u) > MACHEPF);

	if (y < MINLOGF)
	{
		s = 0.0f;	// underflow
	}
	else
		s = expf(y) * (1.0f + s);

	return(s);
}


__host__ __device__  float incbetf(float aa, float bb, float xx)
{
	float ans, a, b, t, x, onemx;
	int flag;

	if ((xx <= 0.0f) || (xx >= 1.0f))
	{
		if (xx == 0.0f)
			return(0.0f);
		if (xx == 1.0f)
			return(1.0f);
		return(0.0f);
	}

	onemx = 1.0f - xx;


	// Transformation for small aa.

	if (aa <= 1.0f)
	{
		ans = incbetf(aa + 1.0f, bb, xx);
		t = aa*logf(xx) + bb*logf(1.0f - xx)
			+ lgamf(aa + bb) - lgamf(aa + 1.0f) - lgamf(bb);

		if (t > MINLOGF)
			ans += expf(t);

		return(ans);
	}


	// see if x is greater than the mean.

	if (xx > (aa / (aa + bb)))
	{
		flag = 1;
		a = bb;
		b = aa;
		t = xx;
		x = onemx;
	}
	else
	{
		flag = 0;
		a = aa;
		b = bb;
		t = onemx;
		x = xx;
	}


	// Choose expansion for optimal convergence.

	if (b > 10.0f)
	{
		if (fabsf(b*x / a) < 0.3f)
		{
			t = incbpsf(a, b, x);
			goto bdone;
		}
	}

	ans = x * (a + b - 2.0f) / (a - 1.0f);
	if (ans < 1.0f)
	{
		ans = incbcff(a, b, x);
		t = b * logf(t);
	}
	else
	{
		ans = incbdf(a, b, x);
		t = (b - 1.0f) * logf(t);
	}

	t += a*logf(x) + lgamf(a + b) - lgamf(a) - lgamf(b);
	t += logf(ans / a);

	if (t < MINLOGF)
	{
		t = 0.0f;	// underflow
	}
	else
	{
		t = expf(t);
	}

bdone:
	if (flag)
		t = 1.0f - t;

	return(t);
}

__host__ __device__ float fdtrcf(float a, float b, float x)
{
	float w;

	if ((a < 1.0f) || (b < 1.0f) || (x < 0.0f))
	{
		return(0.0f);
	}

	w = b / (b + a * x);

	return incbetf(0.5f*b, 0.5f*a, w);
}


__host__ __device__ float ndtrif(float yy0)
{
	float y0, x, y, z, y2, x0, x1;
	int code;

	y0 = yy0;
	if (y0 <= 0.0f)
	{
//		mtherr("ndtrif", DOMAIN);
		return(-MAXNUMF);
	}
	if (y0 >= 1.0f)
	{
//		mtherr("ndtrif", DOMAIN);
		return(MAXNUMF);
	}
	code = 1;
	y = y0;
	if (y > (1.0 - 0.13533528323661269189f)) /* 0.135... = exp(-2) */
	{
		y = 1.0f - y;
		code = 0;
	}

	if (y > 0.13533528323661269189f)
	{
		y = y - 0.5f;
		y2 = y * y;
//		x = y + y * (y2 * polevlf(y2, P0, 4) / p1evlf(y2, Q0, 8));
		x = y + y * (y2 * polevlfP0(y2) / p1evlfQ0(y2));
		x = x * s2pi;
		return(x);
	}

	x = sqrtf(-2.0f * logf(y));
	x0 = x - logf(x) / x;

	z = 1.0f / x;
	if (x < 8.0f) /* y > exp(-32) = 1.2664165549e-14 */
//		x1 = z * polevlf(z, P1, 8) / p1evlf(z, Q1, 8);
		x1 = z * polevlfP1(z) / p1evlfQ1(z);
	else
//		x1 = z * polevlf(z, P2, 8) / p1evlf(z, Q2, 8);
		x1 = z * polevlfP2(z) / p1evlfQ2(z);

	x = x0 - x1;
	if (code != 0)
		x = -x;
	return(x);
}

__host__ __device__ float igamcf(float aa, float xx);

__host__ __device__ float igamf(float aa, float xx)
{
	float a, x, ans, ax, c, r;

	a = aa;
	x = xx;
	if ((x <= 0) || (a <= 0))
		return(0.0f);

	if ((x > 1.0f) && (x > a))
		return(1.0f - igamcf(a, x));

	/* Compute  x**a * exp(-x) / gamma(a)  */
	ax = a * logf(x) - x - lgamf(a);
	if (ax < -MAXLOGF)
	{
		//		mtherr("igamf", UNDERFLOW);
		return(0.0f);
	}
	ax = expf(ax);

	/* power series */
	r = a;
	c = 1.0f;
	ans = 1.0f;

	do
	{
		r += 1.0f;
		c *= x / r;
		ans += c;
	} while (c / ans > MACHEPF);

	return(ans * ax / a);
}

__host__ __device__ float igamcf(float aa, float xx)
{
	float a, x, ans, c, yc, ax, y, z;
	float pk, pkm1, pkm2, qk, qkm1, qkm2;
	float r, t;
//	static float big = BIG;

	a = aa;
	x = xx;
	if ((x <= 0) || (a <= 0))
		return(1.0f);

	if ((x < 1.0f) || (x < a))
		return(1.0f - igamf(a, x));

	ax = a * logf(x) - x - lgamf(a);
	if (ax < -MAXLOGF)
	{
//		mtherr("igamcf", UNDERFLOW);
		return(0.0f);
	}
	ax = expf(ax);

	/* continued fraction */
	y = 1.0f - a;
	z = x + y + 1.0f;
	c = 0.0;
	pkm2 = 1.0f;
	qkm2 = x;
	pkm1 = x + 1.0f;
	qkm1 = z * x;
	ans = pkm1 / qkm1;

	do
	{
		c += 1.0f;
		y += 1.0f;
		z += 2.0f;
		yc = y * c;
		pk = pkm1 * z - pkm2 * yc;
		qk = qkm1 * z - qkm2 * yc;
		if (qk != 0)
		{
			r = pk / qk;
			t = fabsf((ans - r) / r);
			ans = r;
		}
		else
			t = 1.0f;
		pkm2 = pkm1;
		pkm1 = pk;
		qkm2 = qkm1;
		qkm1 = qk;
		if (fabsf(pk) > BIG)
		{
			pkm2 *= MACHEPF;
			pkm1 *= MACHEPF;
			qkm2 *= MACHEPF;
			qkm1 *= MACHEPF;
		}
	} while (t > MACHEPF);

	return(ans * ax);
}

__host__ __device__ float igamif(float aa, float yy0)
{
	float a, y0, d, y, x0, lgm;
	int i;

	if (yy0 > 0.5f)
	{
		//mtherr("igamif", PLOSS);
		return 0.0f;
	}

	a = aa;
	y0 = yy0;

	// approximation to inverse function 
	d = 1.0f / (9.0f*a);
	y = (1.0f - d - ndtrif(y0) * sqrtf(d));
	x0 = a * y * y * y;

	lgm = lgamf(a);

	for (i = 0; i<10; i++)
	{
		if (x0 <= 0.0f)
		{
			// mtherr("igamif", UNDERFLOW);
			return(0.0f);
		}
		y = igamcf(a, x0);
		/* compute the derivative of the function at this point */

		d = (a - 1.0f) * logf(x0) - x0 - lgm;
		if (d < -MAXLOGF)
		{
			// mtherr("igamif", UNDERFLOW);
			goto done;
		}
		d = -expf(d);
		/* compute the step to the next approximation of x */
		if (d == 0.0)
			goto done;

		d = (y - y0) / d;
		x0 = x0 - d;
		if (i < 3)
			continue;

		if (fabsf(d / x0) < (2.0f * MACHEPF))
			goto done;
	}

done:
	return(x0);
}


__host__ __device__ float chdtrcf(float dff, float xx)
{
	float df, x;

	df = dff;
	x = xx;

	if ((x < 0.0f) || (df < 1.0f))
	{
//		mtherr("chdtrcf", DOMAIN);
		return(0.0f);
	}
	return(igamcf(0.5f*df, 0.5f*x));
}


__host__ __device__ float chdtrf(float dff, float xx)
{
	float df, x;

	df = dff;
	x = xx;
	if ((x < 0.0f) || (df < 1.0f))
	{
//		mtherr("chdtrf", DOMAIN);
		return(0.0);
	}
	return(igamf(0.5f*df, 0.5f*x));
}


__host__ __device__ float chdtrif(float dff, float yy)
{
	float y, df, x;

	y = yy;
	df = dff;
	if ((y < 0.0f) || (y > 1.0f) || (df < 1.0f))
	{
		return(0.0f);
	}

	x = igamif(0.5f * df, y);
	return(2.0f * x);
}

//
// Return the area under the F-Distribution from x to +infinity.
// This represents the p-value.
//
__host__ __device__ float CumulativeFDistributionComplimentary(float dof1, float dof2, float x)
{
	return fdtrcf(dof1, dof2, x);
}

// ==================================================================
//  Anova kernel
// ==================================================================


template <int BLOCK_SIZEW, int BLOCK_SIZEH, int BLOCK_MASKW> __global__ void
AnovaKernel(
	float * __restrict__ pVoxelSubject,
	int	  * __restrict__ pSNPSubject,
	float * __restrict__ pVoxelSNP,
	int    NumberOfSNPS,
	int    NumberOfSubjects,
	int    NumberOfVoxels)
{
	// Block index
	const int ddy = gridDim.y;

	const int bx = blockIdx.x;
	      int by = blockIdx.y;

	// Thread index
	const int tx = threadIdx.x;
	const int ty = threadIdx.y;

	//
	// First phase .. compute the between group variance.
	//
	while (by < (NumberOfVoxels+blockDim.y-1)/blockDim.y)
	{

		// Index of the first sub-matrix of VoxelSubject processed by the block
		const int aBegin = NumberOfSubjects * BLOCK_SIZEW * by;

		// Index of the last sub-matrix of VoxelSubject processed by the block
		const int aEnd = aBegin + NumberOfSubjects - 1;

		// Step size used to iterate through the sub-matrices of VoxelSubject
		const int aStep = BLOCK_SIZEW;

		// Index of the first sub-matrix of SNPSubject processed by the block
		const int bBegin = NumberOfSubjects * BLOCK_SIZEH * bx;

		// Step size used to iterate through the sub-matrices of SNPSubject
		const int bStep = BLOCK_SIZEH;

		__shared__ float s_VoxelSubject[BLOCK_SIZEH][BLOCK_SIZEW];
		__shared__ int s_SNPSubject[BLOCK_SIZEH][BLOCK_SIZEW];


		float n0 = 0.0f;
		float n1 = 0.0f;
		float n2 = 0.0f;

		float sum0 = 0.0f;
		float sum1 = 0.0f;
		float sum2 = 0.0f;


		for (int a = aBegin, b = bBegin; a <= aEnd; a += aStep, b += bStep)
		{
			const int condVox = (a - aBegin + tx < NumberOfSubjects) && (by * blockDim.y + ty < NumberOfVoxels);
			const int condSNP = (b - bBegin + tx < NumberOfSubjects) && (bx * blockDim.x + ty < NumberOfSNPS);

			s_VoxelSubject[ty][tx] = (condVox == 1) ? pVoxelSubject[a + NumberOfSubjects * ty + tx] : 0.0f;
			s_SNPSubject[ty][tx] = (condSNP == 1) ? pSNPSubject[b + NumberOfSubjects * ty + tx] : -1;

			__syncthreads();		// Synchronize to make sure the matrices are loaded

#pragma unroll
			for (int k = 0; k < BLOCK_SIZEW; k++)
			{
				//
				// This access pattern guarantees no shared memory conflicts.
				//
				int SS = s_SNPSubject[tx][(k + tx) & BLOCK_MASKW];
				float VS = s_VoxelSubject[ty][(k + tx) & BLOCK_MASKW];

				const float C0 = (SS == 0);	// Save the predicate result to avoid IF stmts
				const float C1 = (SS == 1);
				const float C2 = (SS == 2);

				n0 += C0;
				n1 += C1;
				n2 += C2;

				sum0 += C0*VS;
				sum1 += C1*VS;
				sum2 += C2*VS;
			}


			// Synchronize to make sure that the preceding
			// computation is done before loading two new
			// sub-matrices of VoxelSubject and SNPSubject in the next iteration.
			__syncthreads();
		}


		const float n = n0 + n1 + n2;
		const float sum = sum0 + sum1 + sum2;

		const float	mean = sum / n;
		const float	mean0 = sum0 / fmaxf(n0, 1.0f);
		const float	mean1 = sum1 / fmaxf(n1, 1.0f);
		const float	mean2 = sum2 / fmaxf(n2, 1.0f);

		const float T0 = mean0 - mean;
		const float T1 = mean1 - mean;
		const float T2 = mean2 - mean;

		const float bg_var = (n0*T0*T0 + n1*T1*T1 + n2*T2*T2) / 2.0f;


		//
		// Second phase, compute the withing group variance.
		//

		float sumsq0 = 0.0f;
		float sumsq1 = 0.0f;
		float sumsq2 = 0.0f;


		for (int a = aBegin, b = bBegin; a <= aEnd; a += aStep, b += bStep)
		{
			const int condVox = (a - aBegin + tx < NumberOfSubjects) && (by * blockDim.y + ty < NumberOfVoxels);
			const int condSNP = (b - bBegin + tx < NumberOfSubjects) && (bx * blockDim.x + ty < NumberOfSNPS);

			s_VoxelSubject[ty][tx] = (condVox == 1) ? pVoxelSubject[a + NumberOfSubjects * ty + tx] : 0.0f;
			s_SNPSubject[ty][tx] = (condSNP == 1) ? pSNPSubject[b + NumberOfSubjects * ty + tx] : -1;

			__syncthreads();		// Synchronize to make sure the matrices are loaded

#pragma unroll
			for (int k = 0; k < BLOCK_SIZEW; k++)
			{
				//
				// This access pattern guarantees no shared memory conflicts.
				//
				int SS = s_SNPSubject[tx][(k + tx) & BLOCK_MASKW];
				float VS = s_VoxelSubject[ty][(k + tx) & BLOCK_MASKW];

				float C0 = (SS == 0);	// Save the predicate result to avoid IF stmts
				float C1 = (SS == 1);
				float C2 = (SS == 2);

				sumsq0 += C0*(VS - mean0)*(VS - mean0);
				sumsq1 += C1*(VS - mean1)*(VS - mean1);
				sumsq2 += C2*(VS - mean2)*(VS - mean2);
			}

			// Synchronize to make sure that the preceding
			// computation is done before loading two new
			// sub-matrices of VoxelSubject and SNPSubject in the next iteration.
			__syncthreads();
		}

		const int tidx = blockIdx.x * blockDim.x + tx;
		const int tidy = by * blockDim.y + ty;

		const float wg_var = (sumsq0 + sumsq1 + sumsq2) / (n - 3.0f);
		const int c        = NumberOfSNPS * BLOCK_SIZEH * by + BLOCK_SIZEW * bx;

		if (tidx < NumberOfSNPS && tidy < NumberOfVoxels)
			pVoxelSNP[c + NumberOfSNPS * ty + tx] = CumulativeFDistributionComplimentary(2.0f, n - 3.0f, bg_var / wg_var);

		by += ddy;
	}
}

// ==================================================================
//  CopySubset kernel
// ==================================================================

template <typename T> __global__ void
CopySubsetKernel(
	T * Src,
	int *  SrcList,
	T * Dst,
	int SrcH,
	int SrcW,
	int SrcListLen)
{
	const int y	= blockIdx.y * blockDim.y + threadIdx.y;
	const int x = blockIdx.x * blockDim.x + threadIdx.x;	

	if (y >= SrcH) return;

	int srow = -1;

	// Zero'th thread in each warp reads the row number

	if ((threadIdx.x & 0x1F) == 0)
		srow = SrcList[y];

	srow = __shfl(srow, 0);	// all threads in the warp read from laneid 0

	// If you put this IF statment before the __shfl instruction, then the right most warps
	// with less than 32 threads will hang up indefinitely.

	if (x >= SrcW) return;

	const int sidx = srow * SrcW + x;
	const int didx = y * SrcW + x;

	T val	  = Src[sidx];
	Dst[didx] = val;
}

// ==================================================================
//  DumpRam
// ==================================================================

void DumpRam(float *dS1, float *dS2, int off, int cnt)
{
	float *hS1 = new float[cnt];
	float *hS2 = new float[cnt];

	cudaMemcpy(hS1, dS1+off, cnt * 4, cudaMemcpyDeviceToHost);
	cudaMemcpy(hS2, dS2+off, cnt * 4, cudaMemcpyDeviceToHost);

	delete[] hS1;
	delete[] hS2;
}

// ==================================================================
//  DoKernelAnova
// ==================================================================

#define BLK_SIZEW 16
#define BLK_SIZEH 16
#define BLK_MASKW 0xF

cudaError_t DoKernelAnova(
	float *VoxelSubject,
	int *SNPSubject,
	float *VoxelSNP,

	int NumberOfSNPs,
	int NumberOfSubjects,
	int NumberOfVoxels,

	int *VoxelList,
	int VoxelListCount,
	int *SNPList,
	int SNPListCount)
{

	//
	//  1. GPUMalloc VoxelSubjectTemp of size (VoxelListCount, NumberOfSubjects)
	//  2. Use VoxelList to copy the subset of VoxelSubject into VoxelSubjectTemp
	//
	//  3. GPUMalloc SNPSubjectTemp of size (SNPListCount, NumberOfSubjects)
	//  4. Use SNPList to copy the subset of SNPSubject into SNPSubjectTemp
	//
	//  5. call Anova with VoxelSubjectTemp, SNPSubjectTemp, VoxelListCount, SNPListCount, NumberOfSubjects
	//
	//  6. GPUFree VoxelSubjectTemp
	//  7. GPUFree SNPSubjectTemp

	cudaError_t error;

	// Special case ... the host wants to process all of the voxels (could be as much as 8000000) but the
	//                  SNPListCount=1. This result is a VERY tall matrix that is 1 element wide. The results 
	//                  are used to show how this 1 SNP affects every voxel in the 3D brain view.
	//
	if (VoxelListCount == NumberOfVoxels && SNPListCount == 1)
	{
		int *SNPSubjectTemp;

		error = cudaMalloc(&SNPSubjectTemp, SNPListCount*NumberOfSubjects * sizeof(int));
		if (error != cudaSuccess)
		{
			fprintf(stderr, "cudaMalloc failed on SNPSubjectTemp (error code %s)!\n", cudaGetErrorString(error));
			return error;
		}

		{
			dim3 block = dim3(512, 1, 1);
			dim3 grid = dim3((NumberOfSubjects + block.x - 1) / block.x, SNPListCount, 1);
			CopySubsetKernel<int> << <grid, block >> > (SNPSubject, SNPList, SNPSubjectTemp, NumberOfSNPs, NumberOfSubjects, SNPListCount);
			cudaDeviceSynchronize();
			error = cudaGetLastError();
			if (error != cudaSuccess)
			{
				fprintf(stderr, "CopySubsetKernel failed on SNPSubject (error code %s)!\n", cudaGetErrorString(error));
				exit(EXIT_FAILURE);
			}
		}

		{
			dim3 block = dim3(BLK_SIZEW, BLK_SIZEH, 1);
			dim3 grid = dim3((SNPListCount + BLK_SIZEW - 1) / BLK_SIZEW, 16383, 1);

			AnovaKernel<BLK_SIZEW, BLK_SIZEH, BLK_MASKW> << <grid, block >> > (VoxelSubject,
				SNPSubjectTemp,
				VoxelSNP,
				SNPListCount,
				NumberOfSubjects,
				VoxelListCount
				);
		}

		cudaDeviceSynchronize();

		error = cudaGetLastError();
		if (error != cudaSuccess)
		{
			fprintf(stderr, "Failed to launch (VoxelListCount == NumberOfVoxels && SNPListCount == 1) (error code %s)!\n", cudaGetErrorString(error));
		}
		cudaFree(SNPSubjectTemp);
	}
	//
	// This code will be called by the host when the data inside the window needs processed.
	//
	else
	{
		float *VoxelSubjectTemp;
		int *SNPSubjectTemp;

		error = cudaMalloc(&VoxelSubjectTemp, VoxelListCount*NumberOfSubjects * sizeof(float));
		if (error != cudaSuccess)
		{
			fprintf(stderr, "cudaMalloc failed on VoxelSubjectTemp (error code %s)!\n", cudaGetErrorString(error));
			return error;
		}

		error = cudaMalloc(&SNPSubjectTemp, SNPListCount*NumberOfSubjects * sizeof(int));
		if (error != cudaSuccess)
		{
			fprintf(stderr, "cudaMalloc failed on SNPSubjectTemp (error code %s)!\n", cudaGetErrorString(error));
			cudaFree(VoxelSubjectTemp);
			return error;
		}

		{
			dim3 block = dim3(512, 1, 1);
			dim3 grid = dim3((NumberOfSubjects + block.x - 1) / block.x, VoxelListCount, 1);
			CopySubsetKernel<float> << <grid, block >> > (VoxelSubject, VoxelList, VoxelSubjectTemp, NumberOfVoxels, NumberOfSubjects, VoxelListCount);
			cudaDeviceSynchronize();
			error = cudaGetLastError();
			if (error != cudaSuccess)
			{
				fprintf(stderr, "CopySubsetKernel failed on VoxelSubject (error code %s)!\n", cudaGetErrorString(error));
				exit(EXIT_FAILURE);
			}
		}

		{
			dim3 block = dim3(512, 1, 1);
			dim3 grid = dim3((NumberOfSubjects + block.x - 1) / block.x, SNPListCount, 1);
			CopySubsetKernel<int> << <grid, block >> > (SNPSubject, SNPList, SNPSubjectTemp, NumberOfSNPs, NumberOfSubjects, SNPListCount);
			cudaDeviceSynchronize();
			error = cudaGetLastError();
			if (error != cudaSuccess)
			{
				fprintf(stderr, "CopySubsetKernel failed on SNPSubject (error code %s)!\n", cudaGetErrorString(error));
				exit(EXIT_FAILURE);
			}
		}


		{
			dim3 block = dim3(BLK_SIZEW, BLK_SIZEH, 1);
			dim3 grid = dim3((SNPListCount + BLK_SIZEW - 1) / BLK_SIZEW, (VoxelListCount + BLK_SIZEH - 1) / BLK_SIZEH, 1);

			AnovaKernel<BLK_SIZEW, BLK_SIZEH, BLK_MASKW> << <grid, block >> > (VoxelSubjectTemp,
				SNPSubjectTemp,
				VoxelSNP,
				SNPListCount,
				NumberOfSubjects,
				VoxelListCount
				);

		}

		cudaDeviceSynchronize();

		error = cudaGetLastError();
		if (error != cudaSuccess)
		{
			fprintf(stderr, "Failed to launch (error code %s)!\n", cudaGetErrorString(error));
		}

		cudaFree(VoxelSubjectTemp);
		cudaFree(SNPSubjectTemp);
	}

	return error;
}

// ==================================================================
//  DoKernelVegasTest
// ==================================================================
#define NumThreadsPerBlock 256

__global__ void GenerateRandomSequenceKernel(float *Dest, int NumberOfValues, curandState_t* States)
{
	const int gtid = blockIdx.x * blockDim.x + threadIdx.x;

	__shared__ curandState_t RT[NumThreadsPerBlock];
	
	RT[threadIdx.x] = States[gtid];

	for (int i = gtid; i < NumberOfValues; i += blockDim.x*gridDim.x)
	{
		float T = curand_normal(&RT[threadIdx.x]);
		Dest[i] = T;
	}
	States[gtid] = RT[threadIdx.x];
}

__global__ void  InitRandomKernel(unsigned int seed, curandState_t* States)
{
	const int gtid = blockIdx.x * blockDim.x + threadIdx.x;

	curandState_t S;
	curand_init(gtid << 8, 0, 0, &S);
	States[gtid] = S;
}

__global__ void GenerateRandomVariatesKernel(float *LDMatrix, int LDMatrixSize, float *N01VariatesBuffer, int N01Offset, int Length, float *ResultBuffer)
{
	const int gtid = blockIdx.x * blockDim.x + threadIdx.x;

	for (int i = gtid; i < LDMatrixSize*Length; i += blockDim.x*gridDim.x)
	{
		float T = N01VariatesBuffer[N01Offset*LDMatrixSize*Length + i];
		ResultBuffer[i] = T;
	}
}


class KGene
{
public:
	KGene() :
		m_SnpPos(0),
		m_SnpLen(0),
		m_pLDMatrix(0),
		m_LDMatrixSize(0)
	{};

	~KGene()
	{
	}

	void AddSnpPos(int pos, int len);
	void AddLDMatrix(uint64_t N, uint64_t MatrixPtr);

	int GetSnpPos(void) { return m_SnpPos; }
	int GetSnpLen(void) { return m_SnpLen; }
	float *GetLDMatrixPtr(void) { return m_pLDMatrix;  }
	int GetLDMatrixSize(void) { return m_LDMatrixSize;  }

private:
	int		m_SnpPos;
	int		m_SnpLen;
	float*	m_pLDMatrix;
	int		m_LDMatrixSize;
};

void KGene::AddSnpPos(int pos, int len)
{
	m_SnpPos = pos;
	m_SnpLen = len;
}

void KGene::AddLDMatrix(uint64_t N, uint64_t MatrixPtr)
{
	m_pLDMatrix = (float *)MatrixPtr;
	m_LDMatrixSize = (int)N;

	// Just a sanity check to insure the values
	// are equal. Unfortunately this implies that
	// AddSnpPos() is called before AddLDMatrix().
	//
	assert(m_LDMatrixSize == m_SnpLen);
}

#if 0
// =========================================================================================================================
void ComputeObservedPvalues(CGene* G, int GeneNumber, int VoxelNumber, CArray2D<float> &VoxelSNP, CArray2D<float> &VoxelGeneObs)
{
	double sum = 0;

	for (int i = G->GetSnpPos(); i < G->GetSnpPos() + G->GetSnpLen(); i++)
	{
		const float PValue = VoxelSNP(VoxelNumber, i);
		assert((1.0f - PValue) >= 0 && (1.0f - PValue) < 1);

		// I hate to cast the chi2inv as a float, but we are trying to make this app
		// as fast as possible.

		const float ChiSquare = (float)chi2inv(1.0f - PValue, 1);
		sum += ChiSquare;
	}

	VoxelGeneObs(VoxelNumber, GeneNumber) = (float)sum;
	cout << "\tVoxel " << setw(3) << VoxelNumber << " Gene " << setw(3) << GeneNumber;
	cout << " Snp-Pos " << setw(4) << G->GetSnpPos() << " Snp-Len " << setw(4) << G->GetSnpLen() << " Test Statistic " << sum << endl;
}
#endif

struct InvChiSq_functor : public thrust::unary_function<float, float>
{
	__host__ __device__
		float operator()(float x) const
	{
//		return (float)chi2inv(1.0f - PValue, 1);
		return 1.0f / (x*x + 1.0f);
	}
};

struct sumsq_functor
{
	int R;
	int C;
	float *arr;

	sumsq_functor(int _R, int _C, float *_arr) : R(_R), C(_C), arr(_arr) {};

	__host__ __device__
		float operator()(int myC) {
		float sum = 0;
		for (int i = 0; i < R; i++)
		{
			float T = arr[i*C + myC];
			sum += T * T;
		}
		return sum;
	}
};

struct compare_functor
{
	float Thresh;

	compare_functor(float Threshold) : Thresh(Threshold) {};

	__host__ __device__
		float operator()(float X) 
		{
		return X > Thresh ? 1.0f : 0.0f;
		}
};


cudaError_t DoKernelVegasTest(float *dVoxelGeneObserved,
	float *dVoxelGeneSim,
	float *dVoxelSNP,
	int *dSNPPosLenPairs,
	uint64_t *dLDMatrixList,
	int NumberOfVoxels,
	int NumberOfGenes,
	int NumberOfSNPs,
	int NumberOfIterations,
	int NumberOfSNPPositionLength
)
{
	// 1. Create the GeneList
	//
	// 2. Compute the VoxelSNPkey vector
	//
	// 3. Convert each SNP p-value to it's chi square value, then reduce
	//	  them according to the VoxelSNPkey vector, and write the result
	//	  to the VoxelGeneObserved matrix.
	//
	// 4. PerformSimulation
	//		for each G in Genes
	//			RandNumberCount = NumberOfVoxels* SNPS in G * Iteration
	//			Launch Kernel to generate random  N(0,1)'s
	//			for each V in Voxels
	//				P = pointer to next set of random N(0,1)'s
	//				Launch MatMul Kernel( LDMatrix_G and P )
	//				Plus reduce SNP p-values to GeneValue, Compare to Observed, Count
	//				Write Count/Iterations to VoxelGeneSim
	//			next V
	//		next G
	//
	// 5. Free GeneList
	//
	int numSMs;
	const int devId = 0;
	cudaDeviceGetAttribute(&numSMs, cudaDevAttrMultiProcessorCount, devId);


	//
	// STEP 1 -- Create the Gene List.
	//
	vector<KGene*> GeneList;

	int *hSNPPosLenPairs = new int[NumberOfGenes * 2];
	uint64_t *hLDMatrixList = new uint64_t[NumberOfGenes * 2];

	cudaError_t err = cudaMemcpy(hSNPPosLenPairs, dSNPPosLenPairs, NumberOfGenes * 2 * sizeof(int), cudaMemcpyDeviceToHost);
	assert(err == cudaSuccess);
	err = cudaMemcpy(hLDMatrixList, dLDMatrixList, NumberOfGenes * 2 * sizeof(uint64_t), cudaMemcpyDeviceToHost);
	assert(err == cudaSuccess);

	int MaxSnpLen = 0;
	for (int i = 0; i < NumberOfGenes; i++)
	{
		KGene* G = new KGene;

		G->AddSnpPos(hSNPPosLenPairs[2 * i], hSNPPosLenPairs[2 * i + 1]);
		G->AddLDMatrix(hLDMatrixList[2 * i], hLDMatrixList[2 * i + 1]);

		if (G->GetSnpLen() > MaxSnpLen)
			MaxSnpLen = G->GetSnpLen();

		GeneList.push_back(G);
	}
	delete[] hSNPPosLenPairs;
	delete[] hLDMatrixList;

	// 
	// STEP 2
	//
	// First build a "mask" that will be used on the GPU to perform a
	//		"reduce by key" with "transformation". The Thrust library
	//		calls this mask a "key".
	//
	unsigned char *dVoxelSNPKey;
	err = cudaMalloc((void**)&dVoxelSNPKey, NumberOfVoxels*NumberOfSNPs);
	assert(err == cudaSuccess);

	{
		unsigned char *hVoxelSNPKey = new unsigned char[NumberOfVoxels*NumberOfSNPs];
		int SnpSum = 0;
		for (int i = 0; i < GeneList.size(); i++)
		{
			memset(hVoxelSNPKey + SnpSum, i, GeneList[i]->GetSnpLen());
			SnpSum += GeneList[i]->GetSnpLen();
		}
		for (int i = 1; i < NumberOfVoxels; i++)
			memcpy(hVoxelSNPKey + SnpSum*i, hVoxelSNPKey, SnpSum);
		err = cudaMemcpy(dVoxelSNPKey, hVoxelSNPKey, SnpSum*NumberOfVoxels, cudaMemcpyHostToDevice);
		delete[] hVoxelSNPKey;
	}

	//
	// STEP 3
	//
	// Convert each SNP p-value to it's chi square value, then reduce
	//	  them according to the VoxelSNPkey vector, and write the result
	//	  to the VoxelGeneObserved matrix.

	thrust::device_vector<unsigned char> d_OutputKeys(NumberOfVoxels*NumberOfGenes);

	thrust::pair<thrust::device_vector<unsigned char>::iterator, float * > new_end;
	new_end = thrust::reduce_by_key(thrust::device,
		dVoxelSNPKey,
		dVoxelSNPKey+ NumberOfVoxels*NumberOfSNPs,
		thrust::make_transform_iterator(dVoxelSNP, InvChiSq_functor()),
		d_OutputKeys.begin(),
		dVoxelGeneObserved);

	assert(new_end.first - d_OutputKeys.begin() == NumberOfVoxels*NumberOfGenes);

	//
	// This is kind of gross. Unfortunately I have to copy the dVoxelGeneObserved matrix
	// back to the host because I have to pass each value into the loops below to act
	// as a threshold against the computed Gene p-value.
	//
	float *hVoxelGeneObserved = new float[NumberOfVoxels*NumberOfGenes];
	err = cudaMemcpy(hVoxelGeneObserved, dVoxelGeneObserved, NumberOfVoxels*NumberOfGenes * sizeof(float), cudaMemcpyDeviceToHost);
	assert(err == cudaSuccess);

	//
	// STEP 4
	//
	const int NumBlock = 512;
	const int NumThreads = NumThreadsPerBlock;

	curandState_t* States;
	err = cudaMalloc((void**)&States, NumBlock * NumThreads * sizeof(curandState_t));
	assert(err == cudaSuccess);
	InitRandomKernel << <NumBlock, NumThreads >> > (time(0), States);

	float *dRandomNumberBuffer;
	err = cudaMalloc(&dRandomNumberBuffer, MaxSnpLen * NumberOfVoxels * NumberOfIterations * sizeof(float));
	assert(err == cudaSuccess);

	float *dIterationResultBuffer;
	err = cudaMalloc(&dIterationResultBuffer, MaxSnpLen *  NumberOfIterations * sizeof(float));
	assert(err == cudaSuccess);

	thrust::device_vector<int> keys(NumberOfIterations, 0);

	//
	// Now loop over every gene ...
	//     and loop over every voxel ...
	//     and compute the VoxelGenSim value.
	//
	for (int g = 0; g < GeneList.size(); g++)
	{
		const int RandomNumberCount = GeneList[g]->GetSnpLen() * NumberOfVoxels * NumberOfIterations;

		GenerateRandomSequenceKernel << <numSMs * 32, NumThreadsPerBlock >> > (dRandomNumberBuffer, RandomNumberCount, States);

		for (int v = 0; v < NumberOfVoxels; v++)
		{
			const int StartingIndex = (v * GeneList[g]->GetSnpLen() *  NumberOfIterations) & (~0x01F);
			GenerateRandomVariatesKernel << <NumBlock, NumThreads >> > (GeneList[g]->GetLDMatrixPtr(), GeneList[g]->GetLDMatrixSize(), dRandomNumberBuffer, v, NumberOfIterations, dIterationResultBuffer);

			//
			// Square every element in  dIterationResultBuffer, Plus-reduce every column, threshold against dVoxelGeneObserved(g,v), Count results.
			// Write Count / Iterations to dVoxelGeneSim(G,Vox)

			thrust::device_vector<float> col_sums(NumberOfIterations);
			thrust::sequence(col_sums.begin(), col_sums.end()); 
			thrust::transform(col_sums.begin(), col_sums.end(), col_sums.begin(), sumsq_functor(GeneList[g]->GetLDMatrixSize(), NumberOfIterations, thrust::raw_pointer_cast(dIterationResultBuffer)));

			thrust::transform(col_sums.begin(), col_sums.end(), col_sums.begin(), compare_functor(hVoxelGeneObserved[v*NumberOfGenes + g]));

			thrust::reduce_by_key(thrust::device, 
				keys.begin(), 
				keys.end(), 
				col_sums.begin(), 
				thrust::make_discard_iterator(), 
				dVoxelGeneSim + v*NumberOfGenes + g );
		}
	}

	thrust::transform(thrust::device, 
		dVoxelGeneSim, 
		dVoxelGeneSim+NumberOfVoxels*NumberOfGenes,
		thrust::make_constant_iterator<float>(NumberOfIterations), 
		dVoxelGeneSim,
		thrust::divides<float>());


	//
	// STEP 5
	//
	delete[] hVoxelGeneObserved;
	cudaFree(dIterationResultBuffer);
	cudaFree(dRandomNumberBuffer);
	cudaFree(States);
	cudaFree(dVoxelSNPKey);

	while (!GeneList.empty())
	{
		KGene *G = GeneList.back();
		GeneList.pop_back();
		delete G;
	}

	return cudaSuccess;
}





