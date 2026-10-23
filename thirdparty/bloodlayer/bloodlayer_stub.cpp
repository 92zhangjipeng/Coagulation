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
                     "\xe6\x9c\xaa\xe9\x93\xbe\xe6\x8e\xa5 bloodlayer.lib"
                     "\xef\xbc\x8c\xe8\xa1\x80\xe6\xb5\x86\xe5\x88\x86\xe5\xb1\x82"
                     "\xe8\xaf\x86\xe5\x88\xab\xe4\xb8\x8d\xe5\x8f\xaf\xe7\x94\xa8",
                     BLOODLAYER_MSG_LEN - 1);
    }
    return 0;
}
