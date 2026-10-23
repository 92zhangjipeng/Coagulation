#ifndef BLOODLAYER_API_H
#define BLOODLAYER_API_H

/*
 * Centrifuge-tube RBC/plasma interface. C API for other programs.
 *
 * Detection stays inside BloodLayerDetector. This header does not expose Qt or OpenCV.
 * Paths are UTF-8. Coordinates are original-image pixels, not rotated.
 *
 * Build into the exe: define BLOODLAYER_STATIC
 * Build the DLL: define BLOODLAYER_EXPORTS
 * Use the DLL: define neither macro, and link bloodlayer.lib
 */

#if defined(_WIN32)
#  if defined(BLOODLAYER_STATIC)
#    define BLOODLAYER_API
#  elif defined(BLOODLAYER_EXPORTS)
#    define BLOODLAYER_API __declspec(dllexport)
#  else
#    define BLOODLAYER_API __declspec(dllimport)
#  endif
#else
#  define BLOODLAYER_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define BLOODLAYER_API_VERSION "1.0.0"
#define BLOODLAYER_MAX_LAYERS 8
#define BLOODLAYER_MAX_IFACES 8
#define BLOODLAYER_MSG_LEN 512
#define BLOODLAYER_NAME_LEN 64

/* cv::ROTATE_90_COUNTERCLOCKWISE. Rotate the original image so the tube mouth faces up. */
#define BLOODLAYER_ROTATE_90_CCW 2

typedef struct BloodDetectParamsC {
    double centerBand;
    int smoothKsize;
    double peakRatio;
    double minLayerRatio;
    double refHeightMm;
} BloodDetectParamsC;

typedef struct BloodLayerC {
    char name[BLOODLAYER_NAME_LEN];
    int a0;
    int a1;
    double percent;
} BloodLayerC;

typedef struct BloodIfaceC {
    char name[BLOODLAYER_NAME_LEN];
    int pos;
    double fromClosedEndPercent;
} BloodIfaceC;

typedef struct BloodRefResultC {
    int ok;                 /* 1 = reference mark found */
    char message[BLOODLAYER_MSG_LEN];
    int x, y, w, h;         /* rectangle on the original image */
    int heightPx;           /* height used for scale, in pixels */
    double physicalMm;      /* default 20 */
    double pxPerMm;
} BloodRefResultC;

typedef struct BloodDetectResultC {
    int ok;                 /* 1 = RBC/plasma interface located */
    char message[BLOODLAYER_MSG_LEN];
    int horizontal;
    int plasmaOnRight;
    int refFound;
    int rotateCode;         /* mouth-up display: BLOODLAYER_ROTATE_90_CCW */
    double pxPerMm;
    double refPhysicalMm;
    int refHeightPx;
    int refX, refY, refW, refH;
    int tubeX, tubeY, tubeW, tubeH;
    int rbcPlasmaPos;       /* original-image x for a horizontal tube; -1 on failure */
    int plasmaAirPos;       /* liquid surface, or -1 when absent */
    int nLayers;
    BloodLayerC layers[BLOODLAYER_MAX_LAYERS];
    int nInterfaces;
    BloodIfaceC interfaces[BLOODLAYER_MAX_IFACES];
} BloodDetectResultC;

BLOODLAYER_API const char *bloodlayer_version(void);

BLOODLAYER_API void bloodlayer_params_default(BloodDetectParamsC *params);

/* Reference mark: blue/cyan block height maps to physicalMm (default 20). */
BLOODLAYER_API int bloodlayer_detect_reference(const char *utf8Path,
                                               double refHeightMm,
                                               BloodRefResultC *out);

/* Interface: utf8AnnotatedPath may be NULL. Otherwise write an annotated image in original coordinates. */
BLOODLAYER_API int bloodlayer_detect_interface(const char *utf8Path,
                                               BloodDetectResultC *out,
                                               const char *utf8AnnotatedPath);

BLOODLAYER_API int bloodlayer_detect_interface_ex(const char *utf8Path,
                                                  const BloodDetectParamsC *params,
                                                  BloodDetectResultC *out,
                                                  const char *utf8AnnotatedPath);

/*
 * Decoded 8-bit BGR image, top to bottom. stride is bytes per row; 0 means width*3.
 * For the same pixels this software uses, prefer the path API. It decodes with QImage.
 */
BLOODLAYER_API int bloodlayer_detect_bgr(const unsigned char *bgr,
                                         int width, int height, int stride,
                                         const BloodDetectParamsC *params,
                                         BloodDetectResultC *out);

#ifdef __cplusplus
}
#endif

#endif
