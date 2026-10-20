#ifndef BLOODLAYER_API_H
#define BLOODLAYER_API_H

/*
 * 离心管红细胞/血浆分解面 — C 接口（供其它软件链接）
 *
 * 识别逻辑在 BloodLayerDetector 内，本头文件不暴露 Qt / OpenCV。
 * 路径一律 UTF-8。坐标一律原图像素（未旋转）。
 *
 * 编译进 exe：定义 BLOODLAYER_STATIC
 * 编译 DLL：定义 BLOODLAYER_EXPORTS
 * 使用 DLL：不要定义上述宏，链接 bloodlayer.lib
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

/* cv::ROTATE_90_COUNTERCLOCKWISE，管口朝上时对原图做此旋转 */
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
    int ok;                 /* 1=找到参照物 */
    char message[BLOODLAYER_MSG_LEN];
    int x, y, w, h;         /* 原图矩形 */
    int heightPx;           /* 用于比例的高度（像素） */
    double physicalMm;      /* 默认 20 */
    double pxPerMm;
} BloodRefResultC;

typedef struct BloodDetectResultC {
    int ok;                 /* 1=已定位红细胞/血浆分解面 */
    char message[BLOODLAYER_MSG_LEN];
    int horizontal;
    int plasmaOnRight;
    int refFound;
    int rotateCode;         /* 显示管口朝上：BLOODLAYER_ROTATE_90_CCW */
    double pxPerMm;
    double refPhysicalMm;
    int refHeightPx;
    int refX, refY, refW, refH;
    int tubeX, tubeY, tubeW, tubeH;
    int rbcPlasmaPos;       /* 原图 x（横管）；失败为 -1 */
    int plasmaAirPos;       /* 液面，没有则为 -1 */
    int nLayers;
    BloodLayerC layers[BLOODLAYER_MAX_LAYERS];
    int nInterfaces;
    BloodIfaceC interfaces[BLOODLAYER_MAX_IFACES];
} BloodDetectResultC;

BLOODLAYER_API const char *bloodlayer_version(void);

BLOODLAYER_API void bloodlayer_params_default(BloodDetectParamsC *params);

/* 参照物：蓝/青色块高度对应 physicalMm（默认 20） */
BLOODLAYER_API int bloodlayer_detect_reference(const char *utf8Path,
                                               double refHeightMm,
                                               BloodRefResultC *out);

/* 分解面：utf8AnnotatedPath 可为 NULL；非空则写出标注图（原图坐标，未旋转） */
BLOODLAYER_API int bloodlayer_detect_interface(const char *utf8Path,
                                               BloodDetectResultC *out,
                                               const char *utf8AnnotatedPath);

BLOODLAYER_API int bloodlayer_detect_interface_ex(const char *utf8Path,
                                                  const BloodDetectParamsC *params,
                                                  BloodDetectResultC *out,
                                                  const char *utf8AnnotatedPath);

/*
 * 已解码的 BGR 8 位图（上到下）。stride 为每行字节数，0 表示 width*3。
 * 要与当前软件像素级一致，优先用路径接口（内部 QImage 解码）。
 */
BLOODLAYER_API int bloodlayer_detect_bgr(const unsigned char *bgr,
                                         int width, int height, int stride,
                                         const BloodDetectParamsC *params,
                                         BloodDetectResultC *out);

#ifdef __cplusplus
}
#endif

#endif
