// ----------------------------------------------------------------------------
/**
   File: thinfilm.h

   Status:   Version 1.0 Release 3
   Language: C++

   License: GNU Public License

   (c) Copyright LESO-PB 2011

   Address:
    EPFL ENAC IIC LESO-PB
    CH-1015 Lausanne

   Author: Mario Geiger
   E-Mail: geiger.mario@gmail.com

   Description: Header file to calculate the
                reflectance, transmittance,
                psi and delta of a
                multilayer coating.

   Limitations: Tested with Film Wizard and
                Tfcalc. absorbing incident
                medium, psi and delta are
                not verified.

   Function: 1) asin and acos for complex numbers (source Wikipedia)
             2) simulate a multlayer coating (source Iris Mack)

   Thread Safe: Yes


   Change History:
   Date         Author        Description
   27.06.2011   Mario Geiger  Initial release
   31.03.2012   Mario Geiger  Release 3, performance Matrix22
*/
// ----------------------------------------------------------------------------

#ifndef THINFILM_H
#define THINFILM_H

#include <complex>          // for complex numbers
#include <cstdio>           // for printf
#include <vector>           // for a vector of layers


// ----------------------------------------------------------------------------
namespace thinfilm {

// complex typedef
typedef std::complex<double> complex;

// complex i value
const complex onei(0, 1);


// asin and acos for complex numbers
// source: http://fr.wikipedia.org/wiki/Trigonom%C3%A9trie_complexe
inline complex asin(const complex &z)
{
    return -onei * log(onei * z + sqrt(1.0 - z * z));
}


inline complex acos(const complex &z)
{
    return -onei * log(       z + sqrt(z * z - 1.0));
}


// ----------------------------------------------------------------------------
/**
  \
   \             /  Reflectance
    \           /
     \         /
      \       /
       \     /
        v   /

        Incident medium
            n - i*k
 -------------------------------
        Layer 0 medium
         d0 (n0 - i*k0)
 -------------------------------
             ...

 -------------------------------
        Layer i medium
         d0 (ni - i*ki)
 -------------------------------
             ...

 -------------------------------
        Layer n medium
         d0 (n0 - i*k0)
 -------------------------------
         Exit medium
            n - i*k
                    \
                     \
                      \
                       v  Transmittance

**/
// ----------------------------------------------------------------------------


struct Layer {
    // use the same unit of wavelength
    double thickness;

    // with k negative
    // example (1.5, -0.001)
    complex refractiveIndex;
};

/*
  Simple class for matrix 2x2 to replace blitz++ library
    - good improvement on mac
    - very small improvement on linux
 */
class Matrix22 {
public:
    Matrix22() {}
    Matrix22(complex m11, complex m12, complex m21, complex m22) :
        m11(m11), m12(m12), m21(m21), m22(m22)
    {
    }

    // matrix 2x2 product
    Matrix22 operator *(const Matrix22 &b) const {
        return Matrix22(m11 * b.m11 + m12 * b.m21,
                        m11 * b.m12 + m12 * b.m22,
                        m21 * b.m11 + m22 * b.m21,
                        m21 * b.m12 + m22 * b.m22);
    }

    /*
      ( m11   m12 )
      (           )
      ( m21   m22 )
     */
    complex m11;
    complex m12;
    complex m21;
    complex m22;
};


// ----------------------------------------------------------------------------
inline void simulate(
        // cosine of insident angle
        complex incidentCosTheta,
        // wavelength of light (same unit as layers thickness)
        double lamda,
        // angle of polarization 0 mean P and pi/2 mean S polarization
        double polarization,
        // complex index of refraxion of insident medium,
        // k value must be nagative or null
        complex nIncident,
        // complex index of refraxion of exit medium,
        // k value must be nagative or null
        complex nExit,

        // array of layers
        // in the array the layers are presented from
        //           the insident one to the exit one
        std::vector<struct Layer> layers,

        // pointer for reflectance
        double *reflectance = 0,
        // pointer for transmittance need ptr of reflectance != 0
        double *transmittance = 0,
        // pointer for absorptance need ptr of transmittance != 0
        double *absorptance = 0,
        // pointers of psi and delta, they are optional
        double *psi = 0, double *delta = 0
        )
{
    // variables for optimization
    int layersAmount = layers.size();

    /**
      always for each P and S polarization

      1.1 : we calculate the admittance of the incident and exit medium

      1.2 : the big unique loop
            calculate :
            admittance of the layer
            shift phase of the layer
            caracterictic matrix
            product of the layer matrix and the main matrix

      2   : we do strange equations with the main matrix to extract
            reflectance transmittance and absorptance
      **/

    // ------------------------------------------------------------------------
    // ---------- 1.1 ----------


    // calculate admittance of the incident medium for P and S polarization
    const complex admittanceIncidentP = nIncident / incidentCosTheta;
    const complex admittanceIncidentS = nIncident * incidentCosTheta;


    // calculate cosine of exit medium with snell law optimized
    //  sin(theta0)    n0  =   sin(theta1)    n1
    //       s0        n0  =        s0        n1
    // sqrt( 1 - c0² ) n0  =  sqrt( 1 - c1² ) n1
    // and solve for c1 :
    // c1 = sqrt( c0² n0² - n0² + n1² ) / n1
    const complex squareIncidentCosTheta =
            incidentCosTheta * incidentCosTheta;
    const complex squareN0 = nIncident * nIncident;
    const complex squareN1 = nExit * nExit;
    const complex exitCosTheta =
            sqrt(squareIncidentCosTheta * squareN0 - squareN0 + squareN1) / nExit;

    // calculate the exit medium admitance P and S
    const complex admittanceExitP = nExit / exitCosTheta;
    const complex admittanceExitS = nExit * exitCosTheta;








    // ------------------------------------------------------------------------
    // ---------- 1.2 ----------

    // mains matrix, the product of each layer matrix
    Matrix22 productMatrixP(1.0, 0.0, 0.0, 1.0);
    Matrix22 productMatrixS(1.0, 0.0 ,0.0, 1.0);
    // initialized to Identity Matrix
    /*
      ( 1    0 )
      (        )
      ( 0    1 )
     */



    // the matrix of the layer
    Matrix22 layerMatrixP;
    Matrix22 layerMatrixS;



    // i : for each layer
    for (int i = 0; i < layersAmount; ++i) {

        // create complex number : n - ik
        //const complex layerRefractiveIndex(layers[i][1], -layers[i][2]);

        // snell law
        const complex squareN1 =
                layers[i].refractiveIndex * layers[i].refractiveIndex;
        const complex layerCosTheta =
                sqrt(squareIncidentCosTheta * squareN0 - squareN0 + squareN1)
                / layers[i].refractiveIndex;



        // then calculate the admittance of the layer P and S
        const complex admittanceLayerP =
                layers[i].refractiveIndex / layerCosTheta;
        const complex admittanceLayerS =
                layers[i].refractiveIndex * layerCosTheta;

        // now the delta dephasing of the layer
        const complex deltaLayer =
                2.0 * M_PI * layers[i].refractiveIndex * layers[i].thickness * layerCosTheta / lamda;
        // layers[i][0] is the thickness layer, is need to be the same unit of lamda


        // create the matrix layer
        const complex c = cos(deltaLayer);
        const complex s = sin(deltaLayer) * onei;

        layerMatrixP.m11 = c;
        layerMatrixP.m12 = s / admittanceLayerP;
        layerMatrixP.m21 = s * admittanceLayerP;
        layerMatrixP.m22 = c;


        layerMatrixS.m11 = c;
        layerMatrixS.m12 = s / admittanceLayerS;
        layerMatrixS.m21 = s * admittanceLayerS;
        layerMatrixS.m22 = c;


        // now we need to make the product of the main matrix with the layer matrix

        // do the matrix product
        productMatrixP = productMatrixP * layerMatrixP;
        productMatrixS = productMatrixS * layerMatrixS;
    }







    // ------------------------------------------------------------------------
    // ----------  2  ----------

    const complex bP = productMatrixP.m11 + productMatrixP.m12 * admittanceExitP;
    const complex cP = productMatrixP.m21 + productMatrixP.m22 * admittanceExitP;



    const complex bS = productMatrixS.m11 + productMatrixS.m12 * admittanceExitS;
    const complex cS = productMatrixS.m21 + productMatrixS.m22 * admittanceExitS;



    // calculate the reflectance
    const complex reflectionCoefficientP =
            (admittanceIncidentP - cP / bP) / (admittanceIncidentP + cP / bP);

    const complex reflectionCoefficientS =
            (admittanceIncidentS - cS / bS) / (admittanceIncidentS + cS / bS);



    if (reflectance != 0) {

        // norm returns the norm value of the complex number : norm(3+4i) = 25
        const double reflectanceP = norm(reflectionCoefficientP);
        const double reflectanceS = norm(reflectionCoefficientS);

        // sin^2 + cos^2 = 1
        double polP = cos(polarization);
        double polS = sin(polarization);
        polP *= polP;
        polS *= polS;

        // calculation of the reflectance weighted on the polarization
        *reflectance = polP * reflectanceP + polS * reflectanceS;

        if (transmittance != 0) {

            // warning ! : the transmittance is correct only if kIncident == 0
            if (imag(nIncident) != 0.0) {
                fprintf(stderr, "%s:%d : warning ! the transmittance"
                        " is maybe false (incident k != 0)", __FILE__, __LINE__);
            }

            // and the transmittance
            const double transmittanceP =
                    real(admittanceExitP) * (1.0 - reflectanceP) / real(bP * conj(cP));

            const double transmittanceS =
                    real(admittanceExitS) * (1.0 - reflectanceS) / real(bS * conj(cS));


            *transmittance = polP * transmittanceP + polS * transmittanceS;

            if (absorptance != 0)
                *absorptance = 1.0 - *reflectance - *transmittance;
        }
    }

    if (psi != 0 && delta != 0) {
        const complex reflectionCoefficientPS =
                reflectionCoefficientP / reflectionCoefficientS;

        // abs returns the absolute value of the complex
        *psi = atan(abs(reflectionCoefficientPS));
        // arg return phase angle of complex
        *delta = arg(reflectionCoefficientPS);
    }
}


} // end namespace

#endif // THINFILM_H

