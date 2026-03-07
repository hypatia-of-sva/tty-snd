#include <gsl/gsl_poly.h>
#include "common.h"

int main(int argc, char** argv) {
    assert(argc > 4);
    double dt_in = atof(argv[1]);
    int nr_poles = atoi(argv[2]);
    double preemph_freq = atof(argv[3]);
    double halfdt_window = atof(argv[4]);
    
	floating_waveform_t in_wave;
	assert(read_floating_wave(&in_wave, stdin) == 0);
    
    const double nyquist = 0.5*in_wave.sample_frequency_Hz;
    
    double safetyMargin = 50.0;
    double my_x1 = 0.5 / in_wave.sample_frequency_Hz;
    double my_dx = 1.0 / in_wave.sample_frequency_Hz;
    const double dt = ( dt_in > 0.0 ? dt_in : halfdt_window / 4.0 );
    const double physicalDuration = in_wave.data_length * (1/in_wave.sample_frequency_Hz);
    double dt_window = 2.0 * halfdt_window;
    int nr_frames = 1 + (int) floor((physicalDuration - dt_window) / dt);
    int nsamp_window = (int) floor(dt_window / my_dx);
    int halfnsamp_window = nsamp_window / 2;
    
    assert(nsamp_window >= nr_poles + 1);
    
    double t1 = my_x1 + 0.5 * (physicalDuration - my_dx - (nr_frames - 1) * dt);   // centre of first frame
    if (nr_frames < 1) {
		nr_frames = 1;
		t1 = my_x1 + 0.5 * physicalDuration;
		dt_window = physicalDuration;
		nsamp_window = in_wave.data_length;
	}
    
    /* Pre-emphasis. */
    const double emphasisFactor = exp (- M_PI * 2.0 * preemph_freq * my_dx);
    if (emphasisFactor != 0.0) {   // OPTIMIZE; will happen for cut-off frequencies above 119 times the sampling frequency
		for (int i = in_wave.data_length-1; i >= 1; i --) {
			in_wave.amplitude_data[i] -= emphasisFactor*in_wave.amplitude_data[i-1];
		}
	}
	
	/* Gaussian window. */
	double* window = calloc(nsamp_window, sizeof(double));
	for (int i = 0; i < nsamp_window; i++) {
		const double imid = 0.5 * (nsamp_window + 1), edge = exp (-12.0);
		window [i] = (exp (-48.0 * (i - imid) * (i - imid) / (nsamp_window + 1) / (nsamp_window + 1)) - edge) / (1.0 - edge);
	}
	
	int maximumFrameLength = nsamp_window;
	
	double* frameBuffer = calloc(maximumFrameLength,sizeof(double));
	double* coefficients = calloc(nr_poles,sizeof(double));   // superfluous if which==2, but nobody uses that anyway
	for(int iframe = 0; iframe < nr_frames; iframe++) {
		const double t = t1 + (iframe - 1) * dt;
		const int leftSample = ((t - my_x1) / my_dx + 1.0);
		const int rightSample = leftSample + 1;
		int startSample = rightSample - halfnsamp_window;
		int endSample = leftSample + halfnsamp_window;
		if(startSample < 1) startSample = 1;   // this should not be more than a rounding problem
		if(endSample > in_wave.data_length) endSample = in_wave.data_length;   // this should not be more than a rounding problem
		
		double maximumIntensity = 0.0;
		for(int i = startSample; i <= endSample; i++) {
			const double value = in_wave.amplitude_data[i];
			assert(isfinite(value));
			if (value * value > maximumIntensity)
					maximumIntensity = value * value;
		}
		
		printf("Frame %i: maximum intensity %f\n", iframe, maximumIntensity);
		// save maximumIntensity in per-frame formant data?
		
		if (maximumIntensity == 0.0)
				continue;   // Burg cannot stand all zeroes
				
		/* Copy a pre-emphasized window to a frame. */
		const int actualFrameLength = endSample - startSample + 1;   // should rarely be less than nsamp_window
		const int offset = startSample - 1;
		
		for (int i = 0; i < actualFrameLength; i++)
			frameBuffer[i] = in_wave.amplitude_data[offset+i] * window[i];
			
			
		
		// burg (frameBuffer, coefficients.get(), & thy frames [iframe], 0.5 / my_dx, safetyMargin);
		{
			(void) VECburg(coefficients, nr_poles, frameBuffer, actualFrameLength);
						
			/*
				Convert LP coefficients to polynomial.
			 */
			//double* polynomial = calloc(nr_poles+1, sizeof(double));
			//for (int i = 0; i < nr_poles; i ++)
			//	polynomial[i] = -coefficients[nr_poles - i - 1];
			//coefficients[nr_poles] = 1.0;
			
			
			
			/*
				Find the roots of the polynomial.
			 */
			// Instead of using Polynomial_to_Roots we do this:
			/*double* polynomial = calloc(nr_poles+1, sizeof(double));
			polynomial[0] = 1.0;
			for(int i = 0; i < nr_poles; i++) {
				polynomial[i+1] = -coeffs[nr_poles-i-1];
			}
			double* roots = calloc((nr_poles)*2, sizeof(double));
			double* mat = calloc((nr_poles)*(nr_poles), sizeof(double));
			poly_complex_solve(polynomial, nr_poles, roots, mat);
			free(mat);*/
			
			
			double* polynomial = calloc(nr_poles+1, sizeof(double));
			polynomial[nr_poles] = 1.0;
			for(int i = 0; i < nr_poles; i++) {
				polynomial[i] = -coefficients[nr_poles-i-1];
			}
			double* roots = calloc(nr_poles*2, sizeof(double));
	
			gsl_poly_complex_workspace * w = gsl_poly_complex_workspace_alloc (nr_poles+1);
			gsl_poly_complex_solve (polynomial, nr_poles+1, w, roots);
			gsl_poly_complex_workspace_free (w);
			free(polynomial);
			
			
			
			// Roots_fixIntoUnitCircle (roots.get());
			for(int i = 0; i < nr_poles; i++) {
				double real = roots[2*i];
				double imag = roots[2*i+1];
				double abs_squared = (real*real + imag*imag);
				if(abs_squared > 1.0) {
					roots[2*i] /= abs_squared;
					roots[2*i+1] /= abs_squared;
				}
			}
			
			/*
				First pass: count the formants.
				The roots come in conjugate pairs, so we need only count those above the real axis.
			 */
			int numberOfFormants = 0;
			for (int i = 0; i < nr_poles; i++) {
				double real = roots[2*i];
				double imag = roots[2*i+1];
				if (imag >= 0.0) {
					const double f = fabs(atan2(imag, real)) * nyquist / M_PI;
					if (f >= safetyMargin && f <= nyquist - safetyMargin)
						numberOfFormants ++;
				}
			}
			
			
			/*
				Second pass: fill in the formants.
				replaced with: print out the formants.
			 */
			int iformant = 0;
			for (int i = 0; i < nr_poles; i++) {
				double real = roots[2*i];
				double imag = roots[2*i+1];
				double abs_squared = (real*real + imag*imag);
				if (imag >= 0.0) {
					const double f = fabs(atan2(imag, real)) * nyquist / M_PI;
					if (f >= safetyMargin && f <= nyquist - safetyMargin) {
						//const double bw = -log (norm (roots -> roots [iroot])) * nyquistFrequency / NUMpi
						//Formant_Formant formant = & frame -> formant [++ iformant];
						//formant -> frequency = f;
						//formant -> bandwidth = bw;
						
						// i assume norm = std::norm, but idk
						
						
						const double bw = -log (abs_squared) * nyquist / M_PI;
						iformant++;
						
						printf("%f Hz +- %f\n", iformant, f, bw);
					}
				}
			}
			
			printf("\n");
		}
		
		
	}
	
	
	
	
	
	
	
    
    
	destroy_floating_wave(&in_wave);
	
	return 0;
}

