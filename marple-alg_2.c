

#define MAX_NUMBER_POSSIBLE_FOR_MMAX 100

typedef enum ar_status_t {
    SUCCESS = 1,
    DEN_ILL_CONDITIONED = 2,
    A_ILL_CONDITIONED = 3,
    TOL1_ERROR = 4,
    TOL2_ERROR = 5,
} ar_status_t;

ar_status_t ar_params (complex *x /* data */,
     int n /* num_values */,
     int mmax /* nr_formants */,
     float tol1, float tol2 /* tolerances */,
     int* out_m /* out_calculated */,
     complex *a /* out_ar_params, has to have nr_formants space */,
     float *out_e /* out_pred_error_energy_order_m */,
     float *out_e0 /* out_twice_total_energy */
     )
{
int m; float e, e0;
int status;

complex q2, q3, f, b, h, s, u, v, c[MAX_NUMBER_POSSIBLE_FOR_MMAX+1], d[MAX_NUMBER_POSSIBLE_FOR_MMAX+1], r[MAX_NUMBER_POSSIBLE_FOR_MMAX+1], save1, save2, save3, save4, delta, c1, c2, c3, c4, c5, c6;
float q1, q4, q5, q6, g, w, den, eold, q7, y1, y2, y3, y4, alpha;
int nm, m1, k1, nk, m2, mk;

/* initialization */
e0 = 0;
for(int k = 0; k < n; k++) {
    e0 += cabs(x[k]);
}
e0 *= 2.0;
q1 = 1.0/e0;
q2 = q1*conj(x[1]);
g = q1*cabs(x[0]);
w = q1*cabs(x[n-1]);
den = 1.0 - g - w;
q4 = 1.0 / den;
q5 = 1.0 - g;
q6 = 1.0 - w;
f = x[0];
b = x[n-1];
h = q2*conj(x[n-1]);
s = q2*x[n-1];
u = q1*x[n-1]*x[n-1];
h = q2*conj(x[0]);
e = e0*den;
q1 = 1.0 / e;
c[0] = q1*conj(x[0]);
d[0] = q1*x[n-1];
m = 1;
save1 = 0.0;
nm = n-1;
for(int k = 0; k < nm; k++) {
    save1 += x[k+1]*conj(x[k]);
}
r[0] = 2.0*save1;
a[0] = -q1*r[0];
e *= 1.0 - cabs(a[0]);



while(true) {

    if(m >= mmax) {
        status = 1;
        goto end;
    }

    /* prediction error filter update */

    eold = e;
    m1 = m+1;
    f = x[m1-1];
    b = x[nm-1];
    for(int k = 0; k < m; k++) {
        f += x[m1-k]*a[k];
        b += x[nm+k]*conj(a[k]);
    }

    /* auxiliary vectors order update */
    q1 = 1.0 / e;
    q2 = q1*conj(f);
    q3 = q1*b;
    for(int k = m-1; k >= 0; k--) {
        k1 = k+1;
        c[k1] = c[k]+q2*a[k];
        d[k1] = d[k]+q3*a[k];
    }
    c[0] = q2;
    d[0] = q3;

    /* scalar order update */

    q7 = cabs(s);
    y1 = cabs(f);
    y3 = cabs(b);
    y2 = cabs(v);
    y4 = cabs(u);
    g += y1*q1+q4*(y2*q6+q7*q5+2.0*real(conj(v)*h*s));
    w += y3*q1+q4*(y4*q5+q7*q6+2.0*real(conj(s)*h*v));
    h = s = u = v = 0.0;
    for(int k = -1; k < m; k++) {
        k1 = k+1;
        nk = n-k;
        h += conj(x[nm+k])*c[k1];
        s += x[nk]*c[k1];
        u += x[nk]*d[k1];
        v += conj(x[k1])*c[k1];
    }

    /* denominator update */
    q5 = 1.0 - g;
    q6 = 1.0 - w;
    den = q5*q6 - cabs(h);
    if(den <= 0.0) {
        status = 2;
        goto end;
    }


    /* time shift variables update */
    q4 = 1.0 / den;
    q1 *= q4;
    alpha = 1.0 / (1.0 + (y1*q6+y3*q5+2.0*real(h*f*b))*q1);
    e *= alpha;
    c1 = q4*(f*q6+conj(b*h));
    c2 = q4*(conj(b)*q5+h*f);
    c3 = q4*(v*q6+h*s);
    c4 = q4*(s*q5+v*conj(h));
    c5 = q4*(s*q6+h*u);
    c6 = q4*(u*q5+s*conj(h));
    for(int k = 0; k < m; k++) {
        k1 = k+1;
        a[k] = alpha*(a[k]+c1*c[k1]+c2*d[k1]);
    }
    m2 = m/2+1;
    for(int k = 0; k < m2; k++) {
        mk = m+2-k;
        save1 = conj(c[k]);
        save2 = conj(d[k]);
        save3 = conj(c[mk]);
        save4 = conj(d[mk]);
        c[k] += c3*save3+c4*save4;
        d[k] += c5*save3+c6*save4;
        if(mk != k) {
            c[mk] += c3*save1+c4*save2;
            d[mk] += c5*save1+c6*save2;
        }
    }


    /* order update */
    m++;
    nm = n-m;
    m1 = m-1;
    delta = 0.0;
    c1 = conj(x[n-m]);
    c2 = x[m-1];
    for(int k = m1-1; k >= 0; k--) {
        r[k+1] = r[k]-x[n+1-k]*c1-conj(x[k])*c2;
        delta += r[k+1]*a[k];
    }
    save1 = 0.0;
    for(int k = 0; k < nm; k++) {
        save1 += x[k+m]*conj(x[k]);
    }
    r[0] = 2.0*save1;
    delta += r[0];
    q2 = -delta/e;
    a[m-1] = q2;
    m2 = m/2;
    for(int k = 0; k < m2; k++) {
        mk = m-k;
        save1 = conj(a[k]);
        a[k] += q2*conj(a[mk]);
        if(k != mk) {
            a[mk] += q2*save1;
        }
    }
    y1 = cabs(q2);
    e *= 1.0 - y1;
    if(y1 >= 1.0) {
        status = 3;
        goto end;
    }
    if(e < e0*tol1) {
        status = 4;
        goto end;
    }
    if((eold - e) < eold*tol2) {
        status = 5;
        goto end;
    }
}

end:

out_m[0] = m;
out_e[0] = e;
out_e0[0] = e0;
return status;
}
