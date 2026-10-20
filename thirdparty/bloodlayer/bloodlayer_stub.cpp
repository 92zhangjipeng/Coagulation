#define BLOODLAYER_STATIC
#include "bloodlayer_api.h"

#include <cstring>

const char *bloodlayer_version(void)
{
    return "bloodlayer-missing";
}

int bloodlayer_detect_interface(const char *utf8Path,
                                BloodDetectResultC *out,
                                const char *utf8AnnotatedPath)
{
    (void)utf8Path;
    (void)utf8AnnotatedPath;
    if (out) {
        std::memset(out, 0, sizeof(*out));
        out->ok = 0;
        out->rbcPlasmaPos = -1;
        out->plasmaAirPos = -1;
        std::strncpy(out->message,
                     "未链接 bloodlayer.lib，血浆分层识别不可用",
                     BLOODLAYER_MSG_LEN - 1);
    }
    return 0;
}
